// LoadedModulesDialog.h
#ifndef LOADEDMODULESDIALOG_H
#define LOADEDMODULESDIALOG_H

#include <QDialog>
#include <QTableWidget>

class LoadedModulesDialog : public QDialog
{
    Q_OBJECT

public:
    LoadedModulesDialog(QWidget *parent = nullptr);

    void setLoadedModules(const QStringList &modules);
public slots:
    void onButtonClick();
private:
    QTableWidget *tableWidget;
    QPushButton *button; // Added QPushButton member
};

#endif // LOADEDMODULESDIALOG_H
