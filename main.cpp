#include "widget.h"
#include <QApplication>

/*
 * реализован алгоритм размытия Гаусса с раздельным проходом одномерным окном
 * по строкам и столбцам.
 * изменение радиуса окна и картинки происходит в widget.cpp
 */

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Widget w;
    w.show();
    return a.exec();
}
