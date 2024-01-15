// DLLInfoDialog.cpp
#include "DLLInfoDialog.h"
#include "qdebug.h"
#include "qpushbutton.h"
#include <QVBoxLayout>
#include <QTextEdit>
#include <QCloseEvent>
#include <QMessageBox>

DLLInfoDialog::DLLInfoDialog(const QString& dllInfo, QLibrary& library, QWidget *parent)
    : QDialog(parent), myLibrary(library) {
    setWindowTitle("DLL Information");
    QVBoxLayout *layout = new QVBoxLayout(this);
    QTextEdit *textEdit = new QTextEdit(this);
    textEdit->setPlainText(dllInfo);
    layout->addWidget(textEdit);

    // Add the "Dump" button
    QPushButton *dumpButton = new QPushButton("Dump", this);
    layout->addWidget(dumpButton);
    connect(dumpButton, &QPushButton::clicked, this, &DLLInfoDialog::onDumpButtonClicked);

    // Add the "Close" button
    QPushButton *closeButton = new QPushButton("Close", this);
    layout->addWidget(closeButton);
    connect(closeButton, &QPushButton::clicked, this, &DLLInfoDialog::onCloseButtonClicked);
    setFixedSize(400, 300);
}

void DLLInfoDialog::onDumpButtonClicked() {
    QMessageBox::information(this, "Not Implemented Yet", "This feature is not implemented in this version.");
    qDebug() << "Dump button clicked!";
}

void DLLInfoDialog::onCloseButtonClicked() {
    close();
}

DLLInfoDialog::~DLLInfoDialog() {
    unloadLibrary();
}

void DLLInfoDialog::unloadLibrary() {
    if (myLibrary.isLoaded()) {
        myLibrary.unload();
        qDebug() << "Library unloaded successfully.";
    }
}

void DLLInfoDialog::closeEvent(QCloseEvent *event) {
    unloadLibrary();
    event->accept();
}
