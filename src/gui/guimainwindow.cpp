#include "guimainwindow.h"
#include "./ui_guimainwindow.h"

GuiMainWindow::GuiMainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::GuiMainWindow)
{
    ui->setupUi(this);

    XProcess::setDebugPrivilege(true);
}

GuiMainWindow::~GuiMainWindow()
{
    delete ui;
}

