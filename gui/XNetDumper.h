#ifndef XNETDUMPER_H
#define XNETDUMPER_H

#include <QMainWindow>

#include <windows.h>

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
    XNetDumper(QWidget *parent = nullptr);
    ~XNetDumper();
    bool NETProcess(DWORD processID);
    bool Is64BitProcess(HANDLE hProcess);
    void ProcessInformation(DWORD processId);
    DWORD getComDescriptorValue(HANDLE hProcess, HMODULE hModule);
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
    Ui::XNetDumper *ui;
};

#endif // XNETDUMPER_H
