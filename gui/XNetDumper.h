#ifndef XNETDUMPER_H
#define XNETDUMPER_H

#include <QMainWindow>

#ifdef Q_OS_WIN
#include <Windows.h>
#endif

QT_BEGIN_NAMESPACE
namespace Ui {
class XNetDumper;
}
QT_END_NAMESPACE

class XNetDumper : public QMainWindow
{
    Q_OBJECT
    QStringList getRunningProcesses();
public:
    Ui::XNetDumper *ui;
    XNetDumper(QWidget *parent = nullptr);
    ~XNetDumper();
    void openMemoryModificationWindow();
    bool NETProcess(DWORD processID);
    bool Is64BitProcess(HANDLE hProcess);
    void ProcessInformationLinuxMac(DWORD processId);
    void ProcessInformation(DWORD processId);
    void displayComDescriptorValue(DWORD processId, const QString& processName, DWORD comDescriptorValue);
    QStringList getProcesses();
    bool isSystemProcess(const QString &processName);
    void ProcessInfo();
    void getAndShowLoadedModulesForProcess(const QString &processName);
    QString destinationPath;
    bool CheckForUpdate(const QString &currentVersion);
    void performSearch(const QString &searchText);
    QString latestDbCommit;
    QString currentDbCommit;
    QString getProcessNameLinux(int pid);
    QStringList getLinuxProcesses();
    QStringList getWindowsProcesses();
    QStringList getMacProcesses();
    void refreshTableWidget();
public slots:
    void onButtonClick();
    void showModulesForProcess();
    void contextMenuRequested(const QPoint &pos);
    void checkForUpdates();
    void onSearchShortcut();
    void fileActionTriggered();
    void openActionTriggered();
    void saveActionTriggered();
    void loadActionTriggered();
signals:
    void updateAvailable();


private:

};

#endif // XNETDUMPER_H
