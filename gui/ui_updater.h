/********************************************************************************
** Form generated from reading UI file 'updater.ui'
**
** Created by: Qt User Interface Compiler version 5.15.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_UPDATER_H
#define UI_UPDATER_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QDialog>
#include <QtWidgets/QLabel>
#include <QtWidgets/QProgressBar>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QTextEdit>

QT_BEGIN_NAMESPACE

class Ui_updater
{
public:
    QProgressBar *progressBar;
    QCheckBox *checkBox;
    QLabel *labelMessage;
    QTextEdit *textEdit;
    QLabel *imageLabel;
    QPushButton *cancelButton;

    void setupUi(QDialog *updater)
    {
        if (updater->objectName().isEmpty())
        updater->setObjectName(QString::fromUtf8("updater"));
        updater->resize(414, 118);
        progressBar = new QProgressBar(updater);
        progressBar->setObjectName(QString::fromUtf8("progressBar"));
        progressBar->setGeometry(QRect(160, 90, 251, 23));
        checkBox = new QCheckBox(updater);
        checkBox->setObjectName(QString::fromUtf8("checkBox"));
        checkBox->setGeometry(QRect(340, 40, 71, 17));
        labelMessage = new QLabel(updater);
        labelMessage->setObjectName(QString::fromUtf8("labelMessage"));
        labelMessage->setGeometry(QRect(160, 10, 151, 16));
        textEdit = new QTextEdit(updater);
        textEdit->setObjectName(QString::fromUtf8("textEdit"));
        textEdit->setGeometry(QRect(0, 120, 411, 70));
        imageLabel = new QLabel(updater);
        imageLabel->setObjectName(QString::fromUtf8("imageLabel"));
        imageLabel->setGeometry(QRect(0, 10, 141, 101));
        cancelButton = new QPushButton(updater);
        cancelButton->setObjectName(QString::fromUtf8("cancelButton"));
        cancelButton->setGeometry(QRect(330, 60, 80, 24));
        cancelButton->setAutoDefault(true);

        retranslateUi(updater);

        QMetaObject::connectSlotsByName(updater);
    } // setupUi

    void retranslateUi(QDialog *updater)
    {
        updater->setWindowTitle(QCoreApplication::translate("updater", "Download Progress", nullptr));
        checkBox->setText(QCoreApplication::translate("updater", "Advance", nullptr));
        labelMessage->setText(QCoreApplication::translate("updater", "Downloading...", nullptr));
        imageLabel->setText(QString());
        cancelButton->setText(QCoreApplication::translate("updater", "Cancel", nullptr));
    } // retranslateUi

};

namespace Ui {
    class updater: public Ui_updater {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_UPDATER_H
