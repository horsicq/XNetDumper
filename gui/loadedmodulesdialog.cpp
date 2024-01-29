// LoadedModulesDialog.cpp
#include "LoadedModulesDialog.h"
#include "qpushbutton.h"
#include <QVBoxLayout>
#include <QHeaderView>
#include <QTableWidgetItem>
#include <QMessageBox>

LoadedModulesDialog::LoadedModulesDialog(QWidget *parent)
    : QDialog(parent)
{
    tableWidget = new QTableWidget(this);
    tableWidget->setColumnCount(1);
    tableWidget->setHorizontalHeaderLabels(QStringList() << "Loaded Modules");
    tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    QVBoxLayout *layout = new QVBoxLayout;
    layout->addWidget(tableWidget);

    button = new QPushButton("Dump", this);
    connect(button, SIGNAL(clicked()), this, SLOT(onButtonClick()));
    layout->addWidget(button);
    setLayout(layout);
    setMinimumSize(200, 200);
}

void LoadedModulesDialog::setLoadedModules(const QStringList &modules)
{
    tableWidget->setRowCount(modules.size());
    for (int i = 0; i < modules.size(); ++i)
    {
        QTableWidgetItem *item = new QTableWidgetItem(modules.at(i));
        tableWidget->setItem(i, 0, item);
    }
}

void LoadedModulesDialog::onButtonClick()
{
    QMessageBox::information(this, "Not Implemented", "This functionality is not implemented yet.");
}
