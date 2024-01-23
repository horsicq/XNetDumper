#include "QtWidgets/qapplication.h"
#include "XNetDumper.h"



int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    XNetDumper w;
    w.show();
    return a.exec();
}
