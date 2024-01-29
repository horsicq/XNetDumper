#ifndef MEMORYMODIFICATIONDIALOG_H
#define MEMORYMODIFICATIONDIALOG_H

#include "qdialog.h"
#include <windows.h>


namespace Ui {
class MemoryModificationDialog;
}

class MemoryModificationDialog : public QDialog
{
    Q_OBJECT

public:
    explicit MemoryModificationDialog(DWORD processId, QWidget *parent = nullptr);
    ~MemoryModificationDialog();
    void modifyMemory(DWORD processId, LPVOID address, int value);
private slots:

private:
    Ui::MemoryModificationDialog *ui;
    LPVOID m_address;
    int m_value;
};

#endif // MEMORYMODIFICATIONDIALOG_H
