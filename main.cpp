#include "w4od.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    w4od w;
    w.show();

    return a.exec();
}
