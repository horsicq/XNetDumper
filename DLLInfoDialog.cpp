// DLLInfoDialog.cpp
#include "DLLInfoDialog.h"
#include "qdebug.h"
#include <QVBoxLayout>
#include <QTextEdit>
#include <QCloseEvent>

DLLInfoDialog::DLLInfoDialog(const QString& dllInfo, QLibrary& library, QWidget *parent)
    : QDialog(parent), myLibrary(library) {
    setWindowTitle("DLL Information");

    QVBoxLayout *layout = new QVBoxLayout(this);

    QTextEdit *textEdit = new QTextEdit(this);
    textEdit->setPlainText(dllInfo);
    textEdit->setReadOnly(true);
    layout->addWidget(textEdit);

    // Add more widgets as needed

    setFixedSize(400, 300);

}

DLLInfoDialog::~DLLInfoDialog() {
    // Ensure that the DLL is unloaded when the dialog is destroyed
    unloadLibrary();
}

void DLLInfoDialog::unloadLibrary() {
    if (myLibrary.isLoaded()) {
        myLibrary.unload();
        qDebug() << "Library unloaded successfully.";
    }
}

void DLLInfoDialog::closeEvent(QCloseEvent *event) {
    // Override close event to ensure that the DLL is unloaded when the dialog is closed
    unloadLibrary();
    event->accept();
}
