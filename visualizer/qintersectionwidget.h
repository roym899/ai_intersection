#ifndef QINTERSECTIONWIDGET_H
#define QINTERSECTIONWIDGET_H

#include <QWidget>
#include <QPainter>

class QIntersectionWidget : public QWidget
{
    Q_OBJECT
public:
    explicit QIntersectionWidget(QWidget *parent = nullptr);

protected:
    void paintEvent(QPaintEvent *event);

private:

signals:

public slots:
};

#endif // QINTERSECTIONWIDGET_H
