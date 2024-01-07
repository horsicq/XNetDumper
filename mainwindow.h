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
    QStringList getRunningProcesses();  // Declare the function
public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    bool isDarkModeWindows();
    void handlePaletteChange();
    void checkTheme();
    void changePalette();
    bool m_isDark;
    bool eventFilter(QObject *obj, QEvent *event);
    void performSearch(const QString &searchText);
public slots:
    void onSearchShortcut();
    void fileActionTriggered();
    void openActionTriggered();
    void saveActionTriggered();
    void loadActionTriggered();
protected:
    bool nativeEvent(const QByteArray &eventType, void *message, long *result) override;

private:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
