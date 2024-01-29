#include "memorymodificationdialog.h"
#include "ui_memorymodificationdialog.h"
#include <QtWidgets/QMessageBox>
#include <QTableWidgetItem>


MemoryModificationDialog::MemoryModificationDialog(DWORD processId, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::MemoryModificationDialog)
{
    ui->setupUi(this);

connect(ui->pushbuttonModify, &QPushButton::clicked, this, [=]() {
    modifyMemory(processId, m_address, m_value);
    });

    connect(ui->pushbuttonCancel, &QPushButton::clicked, this, &QDialog::close);
}

MemoryModificationDialog::~MemoryModificationDialog()
{
    delete ui;
}


void MemoryModificationDialog::modifyMemory(DWORD processId, LPVOID address, int value)
{

}

