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

    // Create and set up the button
    button = new QPushButton("Dump", this);
    connect(button, SIGNAL(clicked()), this, SLOT(onButtonClick()));
    layout->addWidget(button);
    setLayout(layout);

    // Set a minimum size for the dialog to avoid size constraints
    setMinimumSize(200, 200);  // Adjust the size as needed
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
    // Display a message box indicating that the functionality is not implemented yet
    QMessageBox::information(this, "Not Implemented", "This functionality is not implemented yet.");
}
