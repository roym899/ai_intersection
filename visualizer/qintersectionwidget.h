#ifndef QINTERSECTIONWIDGET_H
#define QINTERSECTIONWIDGET_H

#include <QWidget>
#include <QPainter>
#include <QTimer>

#include <queue>
#include <utility>
#include <string>

class QIntersectionWidget : public QWidget
{
    Q_OBJECT
public:
    explicit QIntersectionWidget(QWidget *parent = nullptr);

protected:
    void paintEvent(QPaintEvent *event);

private:
    QTimer* update_timer;
    double timer_interval;
    double unscaled_lane_length;
    std::queue< std::pair<std::string, double> > sim_strings;
    double current_sim_time;

    int fields[4];
    std::vector< std::pair<double, int> > n_cars;
    std::vector< std::pair<double, int> > s_cars;
    std::vector< std::pair<double, int> > e_cars;
    std::vector< std::pair<double, int> > w_cars;

    std::vector< QColor > car_colors;

signals:

public slots:
};

#endif // QINTERSECTIONWIDGET_H
