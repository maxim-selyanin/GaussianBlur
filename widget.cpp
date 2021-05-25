#include "widget.h"
#include <QPixmap>
#include <QMouseEvent>
#include "gaussianbluralgo.h"
#include <QDebug>
#include "gaussfunction.h"
#include <QMimeData>

void Widget::mousePressEvent(QMouseEvent *)
{
    if (image.isNull()) {
        return;
    }
    //здесь можно изменить радиус окна
    gb::blurImage(image, gf::getMatrix(blurMatrixRadius));
    this->setPixmap(QPixmap::fromImage(image));
}

void Widget::dragEnterEvent(QDragEnterEvent *event)
{
    event->accept();
}

void Widget::dragMoveEvent(QDragMoveEvent *event)
{
    event->accept();
}

void Widget::dropEvent(QDropEvent *event)
{
    QString mimeText = event->mimeData()->text();
    if (mimeText.startsWith("file:///")
            &&//текст является локальным адресом картинки
            (mimeText.endsWith(".jpg") || mimeText.endsWith(".jpeg") || mimeText.endsWith(".bmp") || mimeText.endsWith(".png"))) {
        mimeText.remove(0, 8);//удаляем "file:///"
        QPixmap p(mimeText);
        image = p.toImage();
        this->setFixedSize(p.size());
        this->setPixmap(p);
    }
}

Widget::Widget(QWidget *parent)
    : QLabel(parent)
{
    this->setAcceptDrops(true);
    this->setFixedSize(500, 500);
}

Widget::~Widget()
{
}

