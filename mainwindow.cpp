#include "mainwindow.h"
#include "./ui_mainwindow.h"
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

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    QApplication::setStyle(QStyleFactory::create("Fusion"));
    ui->setupUi(this);
    m_isDark = isDarkModeWindows();
    changePalette();
    qApp->installEventFilter(this);

    // Connect the Ctrl+F shortcut to the search function
    QShortcut *searchShortcut = new QShortcut(QKeySequence(Qt::CTRL + Qt::Key_F), this);
    connect(searchShortcut, &QShortcut::activated, this, &MainWindow::onSearchShortcut);

    QProcess process;
    process.start("tasklist /FO CSV /NH");
    process.waitForFinished(-1); // wait until the process finishes

    QString output = process.readAllStandardOutput();
    QStringList processList = output.split("\n");
    // Set the column count and headers for the QTableWidget
    ui->tableWidget->setColumnCount(4);
    ui->tableWidget->setHorizontalHeaderLabels(QStringList() << "Process ID" << "Process Name" << "Framework" << "Arch");
    // Hide the grid
    ui->tableWidget->setShowGrid(false);
    // Hide the row numbers
    ui->tableWidget->verticalHeader()->setVisible(false);
    // Set the selection behavior to select rows instead of individual cells
    ui->tableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);
    // Remove highlight on selection
    ui->tableWidget->setStyleSheet("QTableWidget::item:selected{ background-color: transparent }");

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
            if (processName.endsWith(".exe", Qt::CaseInsensitive) && !addedProcesses.contains(processName)) {
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
    // Set the resizing mode to Interactive
    ui->tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Interactive);
    QVBoxLayout *layout = new QVBoxLayout;
    layout->addWidget(ui->tableWidget);
    layout->addWidget(ui->pushButton);
    ui->pushButton->setFixedSize(91, 24); // adjust the values as needed
    // Set the layout on the central widget
    ui->centralwidget->setLayout(layout);
    // Create a new QMenu
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

}

void MainWindow::loadActionTriggered()
{
    // Show a file open dialog to select a DLL
    QString filePath = QFileDialog::getOpenFileName(this, "Select DLL", QDir::currentPath(), "Dynamic Link Libraries (*.dll)");

    // Check if the user canceled the dialog
    if (filePath.isEmpty()) {
        return;
    }

    // Convert the file path to an absolute path
    QString absolutePath = QDir::toNativeSeparators(QFileInfo(filePath).absoluteFilePath());

    // Perform the DLL loading logic with the selected file path (absolutePath)
    QLibrary library(absolutePath);
    if (library.load()) {
        // Do something with the loaded library
        qDebug() << "DLL loaded successfully: " << absolutePath;

        // Add further logic as needed, such as displaying information about the loaded DLL
        QString assemblyInfo = GetAssemblyInfo(&library);
        qDebug() << "Assembly Info:\n" << assemblyInfo;

        // Open the DLL information dialog
        DLLInfoDialog *dllInfoDialog = new DLLInfoDialog(assemblyInfo, library, this);
        dllInfoDialog->exec();
        delete dllInfoDialog;
    } else {
        // Handle the error
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

bool MainWindow::eventFilter(QObject *obj, QEvent *event)
{
    if (obj == qApp && event->type() == QEvent::ApplicationPaletteChange) {
        handlePaletteChange();
    }

    return QMainWindow::eventFilter(obj, event);
}

void MainWindow::handlePaletteChange()
{
    bool isDark = isDarkModeWindows();
    if (isDark != m_isDark) {
        m_isDark = isDark;
        changePalette();
    }
}

void MainWindow::changePalette() {
    if (m_isDark) {
        // Set dark palette
        QPalette darkPalette;
        darkPalette.setColor(QPalette::Window, QColor(53, 53, 53));
        darkPalette.setColor(QPalette::WindowText, Qt::white);
        darkPalette.setColor(QPalette::Base, QColor(25, 25, 25));
        darkPalette.setColor(QPalette::AlternateBase, QColor(53, 53, 53));
        darkPalette.setColor(QPalette::ToolTipBase, Qt::white);
        darkPalette.setColor(QPalette::ToolTipText, Qt::white);
        darkPalette.setColor(QPalette::Text, Qt::white);
        darkPalette.setColor(QPalette::Button, QColor(53, 53, 53));
        darkPalette.setColor(QPalette::ButtonText, Qt::white);
        darkPalette.setColor(QPalette::BrightText, Qt::red);
        darkPalette.setColor(QPalette::Link, QColor(42, 130, 218));
        darkPalette.setColor(QPalette::Highlight, QColor(42, 130, 218));
        darkPalette.setColor(QPalette::HighlightedText, Qt::black);
        // Assuming titleBar is a member variable
        qApp->setPalette(darkPalette);
    } else {
        QPalette lightPalette = qApp->style()->standardPalette();
        qApp->setPalette(lightPalette);
        ui->tableWidget->setStyleSheet(""); // Reset the style sheet

    }

}

bool MainWindow::nativeEvent(const QByteArray &eventType, void *message, long *result) {
    MSG* msg = static_cast<MSG*>(message);
    if (msg->message == WM_SETTINGCHANGE) {
        handlePaletteChange();
    }

    return QMainWindow::nativeEvent(eventType, message, result);
}

bool MainWindow::isDarkModeWindows()
{
    QSettings registrySettings("HKEY_CURRENT_USER\\Software\\Microsoft\\Windows\\CurrentVersion\\Themes\\Personalize", QSettings::NativeFormat);
    int value = registrySettings.value("AppsUseLightTheme").toInt();
    return value == 0;
}

void MainWindow::onSearchShortcut() {
    // Create and show a search dialog
    SearchDialog *searchDialog = new SearchDialog(this);

    // Connect the searchRequested signal from the dialog to your search slot
    connect(searchDialog, &SearchDialog::searchRequested, this, &MainWindow::performSearch);

    // Show the search dialog
    searchDialog->exec();
}

void MainWindow::performSearch(const QString &searchText) {
    static QTableWidgetItem *previousSelectedItem = nullptr;

    // Set selection behavior and mode to ensure only one item is selected
    ui->tableWidget->setSelectionBehavior(QAbstractItemView::SelectItems);
    ui->tableWidget->setSelectionMode(QAbstractItemView::SingleSelection);

    // Iterate through the rows and columns of the QTableWidget
    for (int row = 0; row < ui->tableWidget->rowCount(); ++row) {
        for (int col = 0; col < ui->tableWidget->columnCount(); ++col) {
            QTableWidgetItem *item = ui->tableWidget->item(row, col);

            if (item) {
                QString cellText = item->text();
                int index = cellText.indexOf(searchText, 0, Qt::CaseInsensitive);

                QFont font = item->font();

                // If the search text is found in the cell, select and highlight the text
                if (index != -1) {
                    // Select the item visually
                    ui->tableWidget->setItemSelected(item, true);

                    // Scroll to the selected item
                    ui->tableWidget->scrollToItem(item);

                    // Highlight the item only if it's selected
                    if (ui->tableWidget->isItemSelected(item)) {
                        font.setBold(true); // You can customize the highlighting style
                        item->setFont(font);
                    }

                    // Update the previously selected item
                    previousSelectedItem = item;
                } else {
                    // If the search text is not found or the item is not selected, reset the font and deselect the item
                    font.setBold(false);
                    item->setFont(font);

                    ui->tableWidget->setItemSelected(item, false);
                }
            }
        }
    }
}


MainWindow::~MainWindow()
{
    delete ui;
}

