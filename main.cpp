#include "Poravalit.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Poravalit w;
    w.show();
    return a.exec();
}
