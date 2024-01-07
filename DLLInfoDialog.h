// DLLInfoDialog.h
#ifndef DLLINFODIALOG_H
#define DLLINFODIALOG_H

#include <QDialog>
#include <QLibrary>

class DLLInfoDialog : public QDialog {
    Q_OBJECT
public:
    DLLInfoDialog(const QString& dllInfo, QLibrary& library, QWidget *parent = nullptr);
    ~DLLInfoDialog();

private slots:
    void unloadLibrary();
protected:
    void closeEvent(QCloseEvent *event) override;

private:
    QLibrary& myLibrary;
};

#endif // DLLINFODIALOG_H
