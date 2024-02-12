#include "guimainwindow.h"
#include "./ui_guimainwindow.h"

GuiMainWindow::GuiMainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::GuiMainWindow)
{
    ui->setupUi(this);

    XProcess::setDebugPrivilege(true);

    ui->widgetProcesses->reload();
}

GuiMainWindow::~GuiMainWindow()
{
    delete ui;
}

