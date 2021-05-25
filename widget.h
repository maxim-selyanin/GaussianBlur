#ifndef WIDGET_H
#define WIDGET_H

#include <QLabel>
#include "globaldefinitions.h"

class Widget : public QLabel
{
    Q_OBJECT

    QImage image;

protected:
    virtual void mousePressEvent(QMouseEvent *) override;

public:
    Widget(QWidget *parent = nullptr);
    ~Widget() override;
};
#endif // WIDGET_H
