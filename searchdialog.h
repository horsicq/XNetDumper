#ifndef SEARCHDIALOG_H
#define SEARCHDIALOG_H

#include <QDialog>
#include <QLineEdit>
#include <QPushButton>

class SearchDialog : public QDialog {
    Q_OBJECT

public:
    SearchDialog(QWidget *parent = nullptr);

signals:
    void searchRequested(const QString &searchText);

private slots:
    void onSearchButtonClicked();

private:
    QLineEdit *searchLineEdit;
    QPushButton *searchButton;
};

#endif // SEARCHDIALOG_H
