#ifndef WIDGET_H
#define WIDGET_H

#include <QLabel>
#include "globaldefinitions.h"

class Widget : public QLabel
{
    Q_OBJECT

    QImage image{};
    const int blurMatrixRadius = 3;

protected:
    virtual void mousePressEvent(QMouseEvent *) override;
    virtual void dragEnterEvent(QDragEnterEvent *event) override;
    virtual void dragMoveEvent(QDragMoveEvent *event) override;
    virtual void dropEvent(QDropEvent *event) override;

public:
    Widget(QWidget *parent = nullptr);
    ~Widget() override;
};
#endif // WIDGET_H
