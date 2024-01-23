#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>


QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT
    QStringList getRunningProcesses();
public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    void getAndShowLoadedModulesForProcess(const QString &processName);
    QString destinationPath;
    bool CheckForUpdate(const QString &currentVersion);
    void performSearch(const QString &searchText);
    QString latestDbCommit;
    QString currentDbCommit;
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
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
