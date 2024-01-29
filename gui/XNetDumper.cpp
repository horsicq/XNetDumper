#include "XNetDumper.h"
#include "./ui_XNetDumper.h"
#include "loadedmodulesdialog.h"
#include "memorymodificationdialog.h"
#include "updater.h"
#include "DLLInfoDialog.h"
#include "LoadAssembly.h"
#include "searchdialog.h"

#include <QtCore>
#include <QProcess>
#include <QTableWidgetItem>
#include <QApplication>
#include <QStyleFactory>
#include <QMessageBox>
#include <QDebug>
#include <QFileDialog>
#include <QDir>
#include <QShortcut>
#include <QVersionNumber>
#include <QVBoxLayout>
#include <QLabel>
#include <QFileInfo>

#ifdef Q_OS_WIN
#include <psapi.h>
#include <tchar.h>
#include <tlhelp32.h>
#define OS_PATH_SEPARATOR "\\"
#elif defined(Q_OS_MAC)
#include <sys/sysctl.h>
#include <sys/proc_info.h>
#endif



XNetDumper::XNetDumper(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::XNetDumper)  // Adjust the class name here
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
    connect(searchShortcut, &QShortcut::activated, this, &XNetDumper::onSearchShortcut);


    ui->tableWidget->setColumnCount(3);
    ui->tableWidget->setHorizontalHeaderLabels(QStringList() << "Process ID" << "Process Name"  << "Arch");
    ui->tableWidget->setShowGrid(false);
    ui->tableWidget->verticalHeader()->setVisible(false);
    ui->tableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);

    // Align the column headers to the left
    ui->tableWidget->horizontalHeader()->setDefaultAlignment(Qt::AlignLeft);
    ui->tableWidget->setColumnWidth(0, 62);
    ui->tableWidget->setColumnWidth(1, 216);
    ui->tableWidget->setColumnWidth(2, 67);
    ui->tableWidget->setColumnWidth(3, 44);

    QStringList processList = getProcesses();
    // Add the processes to the QTableWidget
    QSet<QString> addedProcesses;
    for (const QString &process : processList) {
        QStringList parts = process.split(",", Qt::SkipEmptyParts);
        if (parts.size() >= 2) {
            QString processName = parts[1];
            if (!addedProcesses.contains(processName)) {
                int row = ui->tableWidget->rowCount();
                ui->tableWidget->insertRow(row);
                ui->tableWidget->setItem(row, 0, new QTableWidgetItem(parts[0])); // PID
                ui->tableWidget->setItem(row, 1, new QTableWidgetItem(processName)); // Process Name
                ui->tableWidget->setItem(row, 2, new QTableWidgetItem(parts[2])); // Arch
                addedProcesses.insert(processName);
            }
        }
    }

    ui->tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Interactive);
    ui->tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
    QHBoxLayout *buttonLayout = new QHBoxLayout;
    buttonLayout->addWidget(ui->pushButton);
    ui->pushButton->setFixedSize(91, 24);
    connect(ui->pushButton, &QPushButton::clicked, this, &XNetDumper::onButtonClick);
    ui->refreshButton->setFixedSize(91, 24);
    buttonLayout->addWidget(ui->refreshButton);
    connect(ui->refreshButton, &QPushButton::clicked, this, &XNetDumper::refreshTableWidget);


    QVBoxLayout *layout = new QVBoxLayout;
    layout->addWidget(ui->tableWidget);
    layout->addLayout(buttonLayout);
    layout->setAlignment(buttonLayout, Qt::AlignLeft);
    QWidget *centralWidget = new QWidget(this);
    centralWidget->setLayout(layout);
    setCentralWidget(centralWidget);

    QMenu *fileMenu = menuBar()->addMenu("File");
    // Create a "Open" action and add it to the "File" menu
    QAction *openAction = new QAction("Open", this);
    connect(openAction, &QAction::triggered, this, &XNetDumper::openActionTriggered);
    fileMenu->addAction(openAction);

    // Create a "Save" action and add it to the "File" menu
    QAction *saveAction = new QAction("Save", this);
    connect(saveAction, &QAction::triggered, this, &XNetDumper::saveActionTriggered);
    fileMenu->addAction(saveAction);

    // Create a new QMenu
    QMenu *assemblyMenu = menuBar()->addMenu("Assembly");
    QAction *loadAction = new QAction("Load", this);
    connect(loadAction, &QAction::triggered, this, &XNetDumper::loadActionTriggered);
    assemblyMenu->addAction(loadAction);

    //Used for checking updates
    QTimer::singleShot(0, this, &XNetDumper::checkForUpdates);
    currentDbCommit  = "e42e19a5082e641065122ebbbb24b42166a9e0f9";  // Replace with your initial commit
    latestDbCommit = "";

    ui->tableWidget->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui->tableWidget, &QTableWidget::customContextMenuRequested, this, &XNetDumper::contextMenuRequested);

}

void XNetDumper::refreshTableWidget()
{

    ui->tableWidget->clearContents();
    ui->tableWidget->setRowCount(0);


    QStringList processList = getProcesses();
    QSet<QString> addedProcesses;
    for (const QString &process : processList) {
        QStringList parts = process.split(",", Qt::SkipEmptyParts);
        if (parts.size() >= 2) {
            QString processName = parts[1];
            if (!addedProcesses.contains(processName)) {
                int row = ui->tableWidget->rowCount();
                ui->tableWidget->insertRow(row);
                ui->tableWidget->setItem(row, 0, new QTableWidgetItem(parts[0]));
                ui->tableWidget->setItem(row, 1, new QTableWidgetItem(processName));
                ui->tableWidget->setItem(row, 2, new QTableWidgetItem(parts[2]));
                addedProcesses.insert(processName);
            }
        }
    }
}


QStringList XNetDumper::getProcesses()
{
    QStringList processList;

#ifdef Q_OS_WIN
    processList = getWindowsProcesses();
#elif defined(Q_OS_MAC)
    processList = getMacProcesses();
#elif defined(Q_OS_LINUX)
    processList = getLinuxProcesses();
#endif

    return processList;
}

#ifdef Q_OS_WIN
QStringList XNetDumper::getWindowsProcesses() {
    QStringList processList;
    QStringList systemProcesses = {"Registry", "[System Process]","Secure System","System", "XNetDumper.exe"};

    HANDLE hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

    if (hProcessSnap == INVALID_HANDLE_VALUE) {
        return processList;
    }

    PROCESSENTRY32 pe32;
    pe32.dwSize = sizeof(PROCESSENTRY32);

    if (Process32First(hProcessSnap, &pe32)) {
        do {
            qint64 processId = pe32.th32ProcessID;
            QString processName = QString::fromWCharArray(pe32.szExeFile);
            HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, processId);
            if (hProcess != NULL) {
                bool is64bit = Is64BitProcess(hProcess);
                bool isNetProcess = NETProcess(processId);
                CloseHandle(hProcess);
                if (isNetProcess) {
                    processList.append(QString("%1,%2,%3").arg(processId).arg(processName).arg(is64bit ? "64-bit" : "32-bit"));
                }
            }
        } while (Process32Next(hProcessSnap, &pe32));
    }

    CloseHandle(hProcessSnap);

    return processList;
}
bool XNetDumper::NETProcess(DWORD processID) {
    HANDLE hModuleSnap = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, processID);
    if (hModuleSnap == INVALID_HANDLE_VALUE) {
        return false;
    }

    MODULEENTRY32 me32;
    me32.dwSize = sizeof(MODULEENTRY32);

    if (Module32First(hModuleSnap, &me32)) {
        do {
            QString moduleName = QString::fromWCharArray(me32.szModule);
            if (moduleName == "clr.dll" || moduleName == "mscorwks.dll" || moduleName == "coreclr.dll") {
                CloseHandle(hModuleSnap);
                return true;
            }
        } while (Module32Next(hModuleSnap, &me32));
    }

    CloseHandle(hModuleSnap);
    return false;
}


bool XNetDumper::Is64BitProcess(HANDLE hProcess) {
    BOOL bIsWow64 = FALSE;

    typedef BOOL (WINAPI *LPFN_ISWOW64PROCESS) (HANDLE, PBOOL);
    LPFN_ISWOW64PROCESS fnIsWow64Process;

    fnIsWow64Process = (LPFN_ISWOW64PROCESS) GetProcAddress(GetModuleHandle(TEXT("kernel32")),"IsWow64Process");

    if (NULL != fnIsWow64Process) {
        if (!fnIsWow64Process(hProcess, &bIsWow64)) {
            // handle error
        }
    }
    return bIsWow64 == FALSE;
}

#elif defined(Q_OS_LINUX)
QStringList XNetDumper::getLinuxProcesses() {
    QStringList processList;

    QDir procDir("/proc");
    QStringList processDirs = procDir.entryList(QDir::Dirs | QDir::NoDotAndDotDot);

    for (const QString &processDir : processDirs) {
        bool ok;
        int pid = processDir.toInt(&ok);

        if (ok) {
            QString processName = getProcessNameLinux(pid);
            QString architecture = getProcessArchitectureLinux(pid);
            if (NETProcess(pid)) {
                processList.append(QString("%1,%2,%3").arg(pid).arg(processName).arg(architecture));
            }
        }
    }

    return processList;
}


QString XNetDumper::getProcessNameLinux(int pid) {
    QFile cmdlineFile(QString("/proc/%1/cmdline").arg(pid));
    if (cmdlineFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QByteArray data = cmdlineFile.readAll();
        return QString::fromUtf8(data).split(QChar('\0'), Qt::SkipEmptyParts).first();
    }

    return QString();
}

QString XNetDumper::getProcessArchitectureLinux(int pid) {
    QProcess process;
    process.start("file", QStringList() << QString("/proc/%1/exe").arg(pid));
    process.waitForFinished();
    QString output = process.readAllStandardOutput();
    if (output.contains("64-bit")) {
        return "64-bit";
    } else if (output.contains("32-bit")) {
        return "32-bit";
    } else {
        return "unknown";
    }
}
    bool XNetDumper::NETProcess(qint64 processId) {
        QProcess process;
        process.start("cat", QStringList() << QString("/proc/%1/maps").arg(processId));
        process.waitForFinished();
        QString output = process.readAllStandardOutput();
        return output.contains("libcoreclr.so") || output.contains("libmono.so");
    }

#elif defined(Q_OS_MAC)


struct kinfo_proc {
    struct extern_proc kp_proc;  // 'struct extern_proc' is defined in <sys/proc_info.h>
};



QStringList XNetDumper::getMacProcesses()
{
    QStringList processList;

#ifdef Q_OS_MAC
    int mib[] = {CTL_KERN, KERN_PROC, KERN_PROC_ALL, 0};
    size_t size;
    sysctl(mib, 4, nullptr, &size, nullptr, 0);

    struct kinfo_proc *procs = static_cast<struct kinfo_proc *>(malloc(size));
    sysctl(mib, 4, procs, &size, nullptr, 0);

    size_t count = size / sizeof(struct kinfo_proc);

    for (size_t i = 0; i < count; ++i) {
        QString processName = QString::fromUtf8(procs[i].kp_proc.p_comm);
        processList.append(QString("%1,%2").arg(procs[i].kp_proc.p_pid).arg(processName));
    }

    free(procs);
#endif

    return processList;
}
#endif


void XNetDumper::checkForUpdates()
{

    QString currentVersion = "3.0.9";  // Replace with your current version
    bool updateAvailable = CheckForUpdate(currentVersion);
    if (updateAvailable) {
        //qDebug() << "Update is available.";
    } else {
        //qDebug() << "No updates available.";
    }
}

bool XNetDumper::CheckForUpdate(const QString &currentVersion)
{
    QSslConfiguration sslConfig = QSslConfiguration::defaultConfiguration();
    sslConfig.setProtocol(QSsl::AnyProtocol); // or QSsl::TlsV1_3 if supported

    QSslConfiguration::setDefaultConfiguration(sslConfig);
   // qDebug() << "Checking for updates...";
    QNetworkAccessManager manager;
    QUrl serverUrl("https://horsicq.github.io/die_update.json");
    QNetworkReply *reply = manager.get(QNetworkRequest(serverUrl));

    QEventLoop loop;
    connect(reply, &QNetworkReply::finished, &loop, &QEventLoop::quit);
    loop.exec();
    if (reply->error() != QNetworkReply::NoError)
    {
        //qDebug() << "Network error:" << reply->errorString();
        QMessageBox::critical(this, "Error", "Failed to check for updates. Error: " + reply->errorString());
        reply->deleteLater();
        return false;
    }

    QByteArray responseData = reply->readAll();
    reply->deleteLater();
    //qDebug() << "Server Response:" << responseData;
    QJsonDocument jsonDocument = QJsonDocument::fromJson(responseData);
    //qDebug() << "JSON Response:" << jsonDocument.toJson(QJsonDocument::Indented);
    QJsonObject rootObject = jsonDocument.object();
    QJsonArray updatesArray = rootObject.value("updates").toArray();
    if (updatesArray.isEmpty())
    {

       // qDebug() << "Empty updates array.";
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
   // qDebug() << "Latest Version:" << latestVersion;
   // qDebug() << "Download URL:" << downloadUrl;

    if (currentVersion != latestVersion)
    {
        //qDebug() << "A new version is available.";
        QMessageBox msgBox;
        msgBox.setWindowTitle("Update"); // Change this line
        msgBox.setText("A new version is available. Would you like to download it?");
        msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
        msgBox.setDefaultButton(QMessageBox::Yes);
        int ret = msgBox.exec();

        if (ret == QMessageBox::Yes)
        {

            QUrl url(downloadUrl);
            QString filename = QFileInfo(url.path()).fileName();
            destinationPath = QApplication::applicationDirPath() + "/" + filename;
           // qDebug() << "Destination Path:" << destinationPath;
            updater *downloadDialog;


           // qDebug() << "Creating DownloadProgressDialog...";
            downloadDialog = new updater(this);

            QObject::connect(downloadDialog, &updater::downloadProgress, downloadDialog, &updater::updateDownloadProgress);
            QObject::connect(downloadDialog, &updater::finished, downloadDialog, &QObject::deleteLater);
            downloadDialog->startDownload(downloadUrl, destinationPath);
            downloadDialog->exec();
        }
        if (ret == QMessageBox::No)
        {
            //qDebug() << "User chose not to update. Update cancelled.";

        }
    }
}

void XNetDumper::loadActionTriggered()
{
    QString filePath = QFileDialog::getOpenFileName(this, "Select DLL", QDir::currentPath(), "Dynamic Link Libraries (*.dll)");
    if (filePath.isEmpty()) {
        return;
    }
    QString absolutePath = QDir::toNativeSeparators(QFileInfo(filePath).absoluteFilePath());

    LoadAssembly loadAssembly(absolutePath);
    QLibrary library(loadAssembly.GetPath());
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
void XNetDumper::fileActionTriggered() {
    // Code to execute when the "File" action is triggered
}

void XNetDumper::openActionTriggered() {
    // Code to execute when the "Open" action is triggered
}

void XNetDumper::saveActionTriggered() {
    // Code to execute when the "Save" action is triggered
}

void XNetDumper::onSearchShortcut() {
    SearchDialog *searchDialog = new SearchDialog(this);
    connect(searchDialog, &SearchDialog::searchRequested, this, &XNetDumper::performSearch);
    searchDialog->exec();
}

void XNetDumper::performSearch(const QString &searchText) {
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
                    font.setBold(false);
                    item->setFont(font);

                    item->setSelected(false);
                }
            }
        }
    }
}

void XNetDumper::ProcessInfo()
{
    QModelIndexList selectedIndexes = ui->tableWidget->selectionModel()->selectedIndexes();
    if (selectedIndexes.isEmpty())
    {
        QMessageBox::information(this, "No Selection", "Please select a process from the list.");
        return;
    }

    int selectedRow = selectedIndexes.first().row();
    DWORD processId = ui->tableWidget->item(selectedRow, 0)->text().toUInt(); // Assuming the process ID is in the first column
    ProcessInformation(processId);
}


// Implement the showModulesForProcess function
void XNetDumper::showModulesForProcess()
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

void XNetDumper::contextMenuRequested(const QPoint &pos)
{
    // Ensure that there is an item at the clicked position
    QTableWidgetItem *item = ui->tableWidget->itemAt(pos);
    if (item == nullptr) {
        return;
    }

    QMenu contextMenu(tr("Context Menu"), this);

    // Add actions for existing functionalities
    QAction processInfoAction(tr("Process Information"), this);
    connect(&processInfoAction, &QAction::triggered, this, &XNetDumper::ProcessInfo);
    contextMenu.addAction(&processInfoAction);

    QAction showModulesAction(tr("Show Loaded Modules"), this);
    connect(&showModulesAction, &QAction::triggered, this, &XNetDumper::showModulesForProcess);
    contextMenu.addAction(&showModulesAction);

    // Add an action for memory modification
    QAction modifyMemoryAction(tr("Modify Memory"), this);
    connect(&modifyMemoryAction, &QAction::triggered, this, &XNetDumper::openMemoryModificationWindow);
    contextMenu.addAction(&modifyMemoryAction);

    // Show the context menu at the clicked position
    contextMenu.exec(ui->tableWidget->mapToGlobal(pos));
}

void XNetDumper::openMemoryModificationWindow()
{
    // Get the selected process ID
    QTableWidgetItem *item = ui->tableWidget->currentItem();
    if (item != nullptr) {
        DWORD processId = item->text().toUInt();

        // Open MemoryModificationDialog with the selected process ID
        MemoryModificationDialog modificationDialog(processId);
        modificationDialog.exec();
    }
}

void XNetDumper::ProcessInformationLinuxMac(DWORD processId)
{

    QProcess process;

#if defined(Q_OS_LINUX)
    process.start("cat", QStringList() << "/proc/" + QString::number(processId) + "/cmdline");
#elif defined(Q_OS_MAC)
    process.start("sysctl", QStringList() << "kern.proc.pid=" + QString::number(processId) << " | awk -F '\\0' '{print $2}'");
#endif

    process.waitForFinished();

    if (process.exitCode() != 0)
    {
        QMessageBox::warning(this, "Error", "Failed to retrieve process information.");
        return;
    }

    QString processName = process.readAll().trimmed();
    if (processName.isEmpty())
    {
        QMessageBox::warning(this, "Error", "Failed to retrieve process name.");
        return;
    }

    QDialog informationDialog(this);
    informationDialog.setWindowTitle("Process Information");
    QVBoxLayout *layout = new QVBoxLayout(&informationDialog);

    QLabel *processNameLabel = new QLabel("Process Name: " + processName);
    layout->addWidget(processNameLabel);
    informationDialog.exec();
}


void XNetDumper::ProcessInformation(DWORD processId)
{
    #ifdef Q_OS_WIN
    HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, processId);
    if (hProcess == NULL)
    {
        QMessageBox::warning(this, "Error", "Failed to open process, error " + QString::number(GetLastError()));
        return;
    }

    HMODULE hModule;
    DWORD cbNeeded;
    if (!EnumProcessModules(hProcess, &hModule, sizeof(hModule), &cbNeeded))
    {
        QMessageBox::warning(this, "Error", "Failed to enumerate process modules, error " + QString::number(GetLastError()));
        CloseHandle(hProcess);
        return;
    }

    IMAGE_DOS_HEADER dosHeader;
    IMAGE_NT_HEADERS ntHeader;

    if (ReadProcessMemory(hProcess, hModule, &dosHeader, sizeof(dosHeader), NULL))
    {
        if (dosHeader.e_magic == IMAGE_DOS_SIGNATURE)
        {
            if (ReadProcessMemory(hProcess, (LPBYTE)hModule + dosHeader.e_lfanew, &ntHeader, sizeof(ntHeader), NULL))
            {
                if (ntHeader.Signature == IMAGE_NT_SIGNATURE)
                {
                    QDialog informationDialog(this);
                    informationDialog.setWindowTitle("Process Information");
                    QVBoxLayout *layout = new QVBoxLayout(&informationDialog);

                    // Display Process Name
                    TCHAR szProcessName[MAX_PATH];
                    if (GetModuleFileNameEx(hProcess, hModule, szProcessName, MAX_PATH) > 0)
                    {
                        PTSTR pszProcessName = _tcsrchr(szProcessName, TEXT('\\'));
                        if (pszProcessName != NULL)
                        {
                            pszProcessName++; // Move past the backslash
                        }
                        else
                        {
                            pszProcessName = szProcessName; // Use the full path if the backslash is not found
                        }

                        QLabel *processNameLabel = new QLabel("Process Name: " + QString::fromWCharArray(pszProcessName));
                        layout->addWidget(processNameLabel);

                        QLabel *baseAddressLabel = new QLabel("Base Address of Module: " + QString("0x%1").arg((quintptr)hModule, 0, 16));
                        layout->addWidget(baseAddressLabel);

                        // PE Optional Header Information
                        QLabel *peHeaderLabel = new QLabel("<b>PE Optional Header:</b>");
                        layout->addWidget(peHeaderLabel);

                        QLabel *magicLabel = new QLabel("  Magic: 0x" + QString::number(ntHeader.OptionalHeader.Magic, 16));
                        layout->addWidget(magicLabel);

                        QLabel *majorLinkerVersionLabel = new QLabel("  Major Linker Version: " + QString::number(ntHeader.OptionalHeader.MajorLinkerVersion));
                        layout->addWidget(majorLinkerVersionLabel);

                        QLabel *minorLinkerVersionLabel = new QLabel("  Minor Linker Version: " + QString::number(ntHeader.OptionalHeader.MinorLinkerVersion));
                        layout->addWidget(minorLinkerVersionLabel);

                        QLabel *sizeOfCodeLabel = new QLabel("  Size of Code: " + QString::number(ntHeader.OptionalHeader.SizeOfCode) + " bytes");
                        layout->addWidget(sizeOfCodeLabel);

                        DWORD comDescriptorRVA = ntHeader.OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_COM_DESCRIPTOR].VirtualAddress;
                        LPVOID comDescriptorAddress = (LPBYTE)hModule + comDescriptorRVA;
                        DWORD value;
                        SIZE_T bytesRead;
                        bool comDescriptorFound = false;

                        QLabel *dataDirectoryLabel = new QLabel("<b>Data Directory:</b>");
                        layout->addWidget(dataDirectoryLabel);

                        // Read COM_DESCRIPTOR value only if it exists
                        if (comDescriptorRVA != 0 &&
                            ReadProcessMemory(hProcess, comDescriptorAddress, &value, sizeof(DWORD), &bytesRead) &&
                            bytesRead == sizeof(DWORD))
                        {
                            comDescriptorFound = true;
                            QLabel *comDescriptorInfoLabel = new QLabel("  Com_Descriptor: Address - " + QString("0x%1").arg((quintptr)comDescriptorAddress, 0, 16) + ", Value - " + QString("0x%1").arg(value, 0, 16));
                            layout->addWidget(comDescriptorInfoLabel);
                        }

                        DWORD iatRVA = ntHeader.OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IAT].VirtualAddress;
                        DWORD iatSize = ntHeader.OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IAT].Size;

                        LPVOID iatAddress = (LPBYTE)hModule + iatRVA;
                        QLabel *iatInfoLabel = new QLabel("  IAT: Address - " + QString("0x%1").arg((quintptr)iatAddress, 0, 16) + ", Size - " + QString::number(iatSize) + " bytes");
                        layout->addWidget(iatInfoLabel);


                        // Metadata Header Information
                        QLabel *metadataHeaderLabel = new QLabel("<b>Metadata Header:</b>");
                        layout->addWidget(metadataHeaderLabel);

                        IMAGE_DATA_DIRECTORY metadataDirectory = ntHeader.OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_COM_DESCRIPTOR];
                        if (metadataDirectory.VirtualAddress != 0)
                        {
                            IMAGE_COR20_HEADER metadataHeader;
                            LPVOID metadataAddress = (LPBYTE)hModule + metadataDirectory.VirtualAddress;

                            if (ReadProcessMemory(hProcess, metadataAddress, &metadataHeader, sizeof(metadataHeader), NULL))
                            {
                                QLabel *metadataSignatureLabel = new QLabel("  Signature: 0x" + QString::number(metadataHeader.MajorRuntimeVersion, 16) +
                                                                            QString::number(metadataHeader.MinorRuntimeVersion, 16));
                                layout->addWidget(metadataSignatureLabel);

                                QLabel *metadataMajorVersionLabel = new QLabel("  Major Version: " + QString::number(metadataHeader.MajorRuntimeVersion));
                                layout->addWidget(metadataMajorVersionLabel);

                                QLabel *metadataMinorVersionLabel = new QLabel("  Minor Version: " + QString::number(metadataHeader.MinorRuntimeVersion));
                                layout->addWidget(metadataMinorVersionLabel);
                            }
                        }

                        // Execute the dialog
                        informationDialog.exec();
                    }
                }
            }
        }
    }

    // Close the process handle
    CloseHandle(hProcess);
#elif defined(Q_OS_LINUX) || defined(Q_OS_MAC)
    // Call the Linux and macOS-specific function
    ProcessInformationLinuxMac(processId);

#else
    QMessageBox::warning(this, "Error", "Unsupported operating system.");
#endif
}


void XNetDumper::getAndShowLoadedModulesForProcess(const QString &processName)
{
    qDebug() << "Getting loaded DLL modules for process:" << processName;
    QStringList dllModules;
    QString program;
    QStringList arguments;
#ifdef Q_OS_WIN
    program = "tasklist.exe";
    arguments << "/M" << "/FI" << "IMAGENAME eq " + processName;
#elif Q_OS_LINUX
    program = "bash";
    arguments << "-c" << "lsof -p $(pidof " + processName + ") | awk '{if ($5==\"REG\") print $9}'";
#elif Q_OS_MAC
    program = "bash";
    arguments << "-c" << "lsof -p $(pgrep " + processName + ") | awk '{if ($4==\"txt\") print $9}'";
#endif

    QProcess process;
    process.setProgram(program);
    process.setArguments(arguments);
    process.start();
    process.waitForFinished();

    QString output = process.readAllStandardOutput();
    QString error = process.readAllStandardError();

    if (!error.isEmpty())
    {
        qDebug() << "Error running command:" << error;
        return;
    }

    QStringList lines = output.split('\n', Qt::SkipEmptyParts);
    for (const QString &line : lines)
    {
        qDebug() << "Processing line:" << line;

#ifdef Q_OS_WIN
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
#elif Q_OS_LINUX || Q_OS_MAC
                QString moduleName = line.trimmed();
        qDebug() << "Found module:" << moduleName;

        if (moduleName.endsWith(".so", Qt::CaseInsensitive) || moduleName.endsWith(".dylib", Qt::CaseInsensitive))
        {
            dllModules.append(moduleName);
            qDebug() << "Added shared object module:" << moduleName;
        }
#endif
    }

    // Now you can perform an action directly with the loaded DLL modules
    if (dllModules.isEmpty())
    {
        QMessageBox::information(this, "No Modules", "No loaded modules found for the selected process.");
    }
    else
    {
        // Show the LoadedModulesDialog with the loaded modules
        LoadedModulesDialog *dialog = new LoadedModulesDialog(this);
        dialog->setLoadedModules(dllModules);
        dialog->exec();
    }
}

// Assuming your QTableWidget is named tableWidget
void XNetDumper::onButtonClick()
{
    // Check if a cell is selected in the table
    QModelIndexList selectedIndexes = ui->tableWidget->selectionModel()->selectedIndexes();

    if (!selectedIndexes.isEmpty()) {
        QMessageBox::information(this, "Process Selected", "Cell is selected. Not implemented yet.");

    } else {
        QMessageBox::warning(this, "No Process Selected", "Please select a cell before clicking the button.");
    }
}


XNetDumper::~XNetDumper()
{
    delete ui;
}

