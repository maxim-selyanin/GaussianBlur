#include "widget.h"
#include <QPixmap>
#include <QMouseEvent>
#include "gaussianbluralgo.h"
#include <QDebug>
#include "gaussfunction.h"

void Widget::mousePressEvent(QMouseEvent *)
{
    //здесь можно изменить радиус окна
    gb::blurImage(image, gf::getMatrix(3));
    this->setPixmap(QPixmap::fromImage(image));
}

Widget::Widget(QWidget *parent)
    : QLabel(parent)
{
    //здесь можно изменить картинку
    QPixmap p(pic1Path);
    image = p.toImage();
    this->setPixmap(p);
}

Widget::~Widget()
{
}

