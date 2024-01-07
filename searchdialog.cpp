#include "searchdialog.h"
#include "qboxlayout.h"

SearchDialog::SearchDialog(QWidget *parent) : QDialog(parent) {
    searchLineEdit = new QLineEdit(this);
    searchButton = new QPushButton("Search", this);

    connect(searchButton, &QPushButton::clicked, this, &SearchDialog::onSearchButtonClicked);

    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->addWidget(searchLineEdit);
    layout->addWidget(searchButton);

    setLayout(layout);
}

void SearchDialog::onSearchButtonClicked() {
    QString searchText = searchLineEdit->text();
    emit searchRequested(searchText);
    close();
}
