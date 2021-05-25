#include "widget.h"
#include <QApplication>

/*
 * реализован алгоритм размытия Гаусса с раздельным проходом одномерным окном
 * по строкам и столбцам.
 * дропните картинку в виджет и кликните на неё, чтобы применить размытие.
 * изменить радиус размытия можно в файле widget.h в переменной blurMatrixRadius.
 */

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Widget w;
    w.show();
    return a.exec();
}
