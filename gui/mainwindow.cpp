#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "loadedmodulesdialog.h"
#include "qjsonarray.h"
#include "qjsondocument.h"
#include "qjsonobject.h"
#include "qlibrary.h"
#include <QProcess>
#include <QTableWidgetItem>
#include <QApplication>
#include <QStyleFactory>
#include <QMessageBox>
#include <QTimer>
#include <QVBoxLayout>
#include <QDebug>
#include <QSettings>
#include <QFileDialog>
#include <QDir>
#include "DLLInfoDialog.h"
#include "LoadAssembly.h"
#include "searchdialog.h"
#include <QShortcut>
#include "updater.h"
#include <QVersionNumber>
#include <QtGlobal>




MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    QApplication::setStyle(QStyleFactory::create("Fusion"));
    ui->setupUi(this);
    qApp->installEventFilter(this);

    // Connect the Ctrl+F shortcut to the search function
#ifdef Q_OS_MAC
    QShortcut *searchShortcut = new QShortcut(QKeySequence(Qt::META + Qt::Key_F), this);
#else
    QShortcut *searchShortcut = new QShortcut(QKeySequence(Qt::CTRL + Qt::Key_F), this);
#endif
    connect(searchShortcut, &QShortcut::activated, this, &MainWindow::onSearchShortcut);

    QProcess process;
#ifdef Q_OS_WIN
    // Code specific to Windows
    process.setProgram("C:\\Windows\\System32\\tasklist.exe");
   process.setArguments({"/FO", "CSV", "/NH", "/FI", "USERNAME ne NT AUTHORITY\\SYSTEM", "/FI", "IMAGENAME ne System"});
#elif defined(Q_OS_LINUX)
    // Code specific to Linux
    process.setProgram("/bin/ps");
    process.setArguments({"-ef"});
#elif defined(Q_OS_MAC)
    // Code specific to macOS
    process.setProgram("/bin/ps");
    process.setArguments({"-ax"});
#endif
    process.start();
    process.waitForFinished(-1); // wait until the process finishes
    QString output = process.readAllStandardOutput();
    QStringList processList = output.split("\n");
    ui->tableWidget->setColumnCount(4);
    ui->tableWidget->setHorizontalHeaderLabels(QStringList() << "Process ID" << "Process Name" << "Framework" << "Arch");
    ui->tableWidget->setShowGrid(false);
    ui->tableWidget->verticalHeader()->setVisible(false);
    ui->tableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);

    // Align the column headers to the left
    ui->tableWidget->horizontalHeader()->setDefaultAlignment(Qt::AlignLeft);
    ui->tableWidget->setColumnWidth(0, 62);
    ui->tableWidget->setColumnWidth(1, 106);
    ui->tableWidget->setColumnWidth(2, 67);
    ui->tableWidget->setColumnWidth(3, 44);

    // Add the processes to the QTableWidget
    QRegularExpression re("\"(.*?)\",\"(.*?)\",(.*),(.*),");
    QSet<QString> addedProcesses;
    for (const QString &process : processList) {
        QRegularExpressionMatch match = re.match(process);
        if (match.hasMatch()) {
            QString processName = match.captured(1);
            if (!addedProcesses.contains(processName)) {
                int row = ui->tableWidget->rowCount();
                ui->tableWidget->insertRow(row);
                ui->tableWidget->setItem(row, 0, new QTableWidgetItem(match.captured(2))); // PID
                ui->tableWidget->setItem(row, 1, new QTableWidgetItem(processName)); // Process Name
                ui->tableWidget->setItem(row, 2, new QTableWidgetItem("")); // Framework
                ui->tableWidget->setItem(row, 3, new QTableWidgetItem("")); // Arch
                addedProcesses.insert(processName);
            }
        }
    }
    ui->tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Interactive);
    ui->tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
    QVBoxLayout *layout = new QVBoxLayout;
    layout->addWidget(ui->tableWidget);
    layout->addWidget(ui->pushButton);
    ui->pushButton->setFixedSize(91, 24);
    ui->centralwidget->setLayout(layout);

    QMenu *fileMenu = menuBar()->addMenu("File");
    // Create a "Open" action and add it to the "File" menu
    QAction *openAction = new QAction("Open", this);
    connect(openAction, &QAction::triggered, this, &MainWindow::openActionTriggered);
    fileMenu->addAction(openAction);

    // Create a "Save" action and add it to the "File" menu
    QAction *saveAction = new QAction("Save", this);
    connect(saveAction, &QAction::triggered, this, &MainWindow::saveActionTriggered);
    fileMenu->addAction(saveAction);

    // Create a new QMenu
    QMenu *assemblyMenu = menuBar()->addMenu("Assembly");
    QAction *loadAction = new QAction("Load", this);
    connect(loadAction, &QAction::triggered, this, &MainWindow::loadActionTriggered);
    assemblyMenu->addAction(loadAction);

    //Used for checking updates
    QTimer::singleShot(0, this, &MainWindow::checkForUpdates);
    currentDbCommit  = "e42e19a5082e641065122ebbbb24b42166a9e0f9";  // Replace with your initial commit
    latestDbCommit = "";

    ui->tableWidget->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui->tableWidget, &QTableWidget::customContextMenuRequested, this, &MainWindow::contextMenuRequested);
}

void MainWindow::checkForUpdates()
{
    QString currentVersion = "3.0.9";  // Replace with your current version
    bool updateAvailable = CheckForUpdate(currentVersion);
    if (updateAvailable) {
        qDebug() << "Update is available.";
    } else {
        qDebug() << "No updates available.";
    }
}

bool MainWindow::CheckForUpdate(const QString &currentVersion)
{
    QSslConfiguration sslConfig = QSslConfiguration::defaultConfiguration();
    sslConfig.setProtocol(QSsl::AnyProtocol); // or QSsl::TlsV1_3 if supported

    QSslConfiguration::setDefaultConfiguration(sslConfig);
    qDebug() << "Checking for updates...";
    QNetworkAccessManager manager;
    QUrl serverUrl("https://horsicq.github.io/die_update.json");
    QNetworkReply *reply = manager.get(QNetworkRequest(serverUrl));

    QEventLoop loop;
    connect(reply, &QNetworkReply::finished, &loop, &QEventLoop::quit);
    loop.exec();
    if (reply->error() != QNetworkReply::NoError)
    {
        qDebug() << "Network error:" << reply->errorString();
        QMessageBox::critical(this, "Error", "Failed to check for updates. Error: " + reply->errorString());
        reply->deleteLater();
        return false;
    }

    QByteArray responseData = reply->readAll();
    reply->deleteLater();
    qDebug() << "Server Response:" << responseData;
    QJsonDocument jsonDocument = QJsonDocument::fromJson(responseData);
    qDebug() << "JSON Response:" << jsonDocument.toJson(QJsonDocument::Indented);
    QJsonObject rootObject = jsonDocument.object();
    QJsonArray updatesArray = rootObject.value("updates").toArray();
    if (updatesArray.isEmpty())
    {

        qDebug() << "Empty updates array.";
        return false;
    }

    // Extract information from the first object in the array
    QJsonObject updateInfo = updatesArray.at(0).toObject();
    QString latestVersion = updateInfo["latestVersion"].toString();
    QString downloadUrl = updateInfo["downloadUrl"].toString();
    QString releaseNotes = updateInfo["releaseNotes"].toString();

    // Convert version strings to QVersionNumber
    QVersionNumber currentVer = QVersionNumber::fromString(currentVersion);
    QVersionNumber latestVer = QVersionNumber::fromString(latestVersion);
    qDebug() << "Latest Version:" << latestVersion;
    qDebug() << "Download URL:" << downloadUrl;

    if (currentVersion != latestVersion)
    {
        qDebug() << "A new version is available.";
        QMessageBox msgBox;
        msgBox.setText("A new version is available. Would you like to download it?");
        msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
        msgBox.setDefaultButton(QMessageBox::Yes);
        int ret = msgBox.exec();

        if (ret == QMessageBox::Yes)
        {

            QUrl url(downloadUrl);
            QString filename = QFileInfo(url.path()).fileName();
            destinationPath = QApplication::applicationDirPath() + "/" + filename;
            qDebug() << "Destination Path:" << destinationPath;
            updater *downloadDialog;


            qDebug() << "Creating DownloadProgressDialog...";
            downloadDialog = new updater(this);

            QObject::connect(downloadDialog, &updater::downloadProgress, downloadDialog, &updater::updateDownloadProgress);
            QObject::connect(downloadDialog, &updater::finished, downloadDialog, &QObject::deleteLater);
            downloadDialog->startDownload(downloadUrl, destinationPath);
            downloadDialog->exec();
        }
        if (ret == QMessageBox::No)
        {
            qDebug() << "User chose not to update. Update cancelled.";

        }
    }
}

void MainWindow::loadActionTriggered()
{
    QString filePath = QFileDialog::getOpenFileName(this, "Select DLL", QDir::currentPath(), "Dynamic Link Libraries (*.dll)");
    if (filePath.isEmpty()) {
        return;
    }
    QString absolutePath = QDir::toNativeSeparators(QFileInfo(filePath).absoluteFilePath());

    QLibrary library(absolutePath);
    if (library.load()) {
        qDebug() << "DLL loaded successfully: " << absolutePath;
        QString assemblyInfo = GetAssemblyInfo(&library);
        qDebug() << "Assembly Info:\n" << assemblyInfo;

        DLLInfoDialog *dllInfoDialog = new DLLInfoDialog(assemblyInfo, library, this);
        dllInfoDialog->exec();
        delete dllInfoDialog;
    } else {
        qWarning() << "Failed to load DLL: " << absolutePath;
        qWarning() << "Error: " << library.errorString();
        QMessageBox::critical(this, "DLL Loading Error", "Failed to load DLL:\n" + absolutePath + "\n\nError: " + library.errorString());
    }
}

void MainWindow::fileActionTriggered() {
    // Code to execute when the "File" action is triggered
}

void MainWindow::openActionTriggered() {
    // Code to execute when the "Open" action is triggered
}

void MainWindow::saveActionTriggered() {
    // Code to execute when the "Save" action is triggered
}

void MainWindow::onSearchShortcut() {
    SearchDialog *searchDialog = new SearchDialog(this);
    connect(searchDialog, &SearchDialog::searchRequested, this, &MainWindow::performSearch);
    searchDialog->exec();
}

void MainWindow::performSearch(const QString &searchText) {
    static QTableWidgetItem *previousSelectedItem = nullptr;
    ui->tableWidget->setSelectionBehavior(QAbstractItemView::SelectItems);
    ui->tableWidget->setSelectionMode(QAbstractItemView::SingleSelection);

    for (int row = 0; row < ui->tableWidget->rowCount(); ++row) {
        for (int col = 0; col < ui->tableWidget->columnCount(); ++col) {
            QTableWidgetItem *item = ui->tableWidget->item(row, col);

            if (item) {
                QString cellText = item->text();
                int index = cellText.indexOf(searchText, 0, Qt::CaseInsensitive);

                QFont font = item->font();
                if (index != -1) {
                    item->setSelected(true);
                    ui->tableWidget->scrollToItem(item);
                    if (item->isSelected()) {
                        font.setBold(true);
                        item->setFont(font);
                    }
                    previousSelectedItem = item;
                } else {
                    // If the search text is not found or the item is not selected, reset the font and deselect the item
                    font.setBold(false);
                    item->setFont(font);

                    item->setSelected(false);
                }
            }
        }
    }
}

// Implement the showModulesForProcess function
void MainWindow::showModulesForProcess()
{
    QModelIndexList selectedIndexes = ui->tableWidget->selectionModel()->selectedIndexes();
    if (selectedIndexes.isEmpty())
    {
        QMessageBox::information(this, "No Selection", "Please select a process from the list.");
        return;
    }

    int selectedRow = selectedIndexes.first().row();
    QString processName = ui->tableWidget->item(selectedRow, 1)->text(); // Assuming the process name is in the second column

    getAndShowLoadedModulesForProcess(processName);
}

// Implement the contextMenuRequested function
void MainWindow::contextMenuRequested(const QPoint &pos)
{
    QMenu contextMenu(tr("Context Menu"), this);
    QAction showModulesAction(tr("Show Loaded Modules"), this);
    connect(&showModulesAction, &QAction::triggered, this, &MainWindow::showModulesForProcess);

    contextMenu.addAction(&showModulesAction);
    contextMenu.exec(ui->tableWidget->mapToGlobal(pos));
}

void MainWindow::getAndShowLoadedModulesForProcess(const QString &processName)
{
    qDebug() << "Getting loaded DLL modules for process:" << processName;

    QStringList dllModules;

    QProcess process;
    process.setProgram("tasklist.exe");
    process.setArguments({"/M", "/FI", "IMAGENAME eq " + processName});
    process.start();
    process.waitForFinished();

    QString output = process.readAllStandardOutput();
    QString error = process.readAllStandardError();

    if (!error.isEmpty())
    {
        qDebug() << "Error running tasklist:" << error;
        return;
    }

    QStringList lines = output.split('\n', Qt::SkipEmptyParts);
    for (const QString &line : lines)
    {
        qDebug() << "Processing line:" << line;

        // Skip the header line
        if (line.startsWith("Image Name", Qt::CaseInsensitive))
            continue;

        // Split the line into process info and module list
        QStringList parts = line.split(" ", Qt::SkipEmptyParts);
        if (parts.size() < 3)
            continue;

        // Get the list of modules
        QStringList modules = parts[2].split(",", Qt::SkipEmptyParts);
        for (const QString &module : modules)
        {
            QString moduleName = module.trimmed();
            qDebug() << "Found module:" << moduleName;

            if (moduleName.endsWith(".dll", Qt::CaseInsensitive))
            {
                dllModules.append(moduleName);
                qDebug() << "Added DLL module:" << moduleName;
            }
        }
    }

    // Now you can perform an action directly with the loaded DLL modules
    if (dllModules.isEmpty())
    {
        QMessageBox::information(this, "No DLL Modules", "No loaded DLL modules found for the selected process.");
    }
    else
    {
        // Show the LoadedModulesDialog with the loaded DLL modules
        LoadedModulesDialog *dialog = new LoadedModulesDialog(this);
        dialog->setLoadedModules(dllModules);
        dialog->exec();
    }
}





MainWindow::~MainWindow()
{
    delete ui;
}

