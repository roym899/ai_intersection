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
    bool fields[4];
    std::vector<double> n_cars;
    std::vector<double> s_cars;
    std::vector<double> e_cars;
    std::vector<double> w_cars;

signals:

public slots:
};

#endif // QINTERSECTIONWIDGET_H
