#include "qintersectionwidget.h"

QIntersectionWidget::QIntersectionWidget(QWidget *parent) : QWidget(parent)
{
    // TODO: update timer

}

void QIntersectionWidget::paintEvent(QPaintEvent *event)
{
    // units multiplied by 10 to account for integer accuracy (everything given in meters)
    const double scale_factor = 10;

    const double lane_width = 3.0 * scale_factor;
    const double lane_length = 50.0 * scale_factor;
    const double margin = 2.0 * scale_factor;
    const double total_dimensions = 2*lane_width+2*lane_length+2*margin;
    const double car_width = 1.8 * scale_factor;
    const double car_length = 4 * scale_factor;
    const double line_thickness = 0.25 *scale_factor;
    const double corner_radius = 0.3 * scale_factor;

    // define the colors for the intersection
    QColor green(15, 140, 15);
    QColor black(20, 20, 20);

    // minimum side length to fit the intersection into it
    int side = qMin(width(), height());

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.translate(width() / 2, height() / 2);
    painter.scale(side / total_dimensions, -side / total_dimensions);

    QLine street_line_n_1(-lane_width, lane_width, -lane_width, lane_width+lane_length);
    QLine street_line_n_2(lane_width, lane_width, lane_width, lane_width+lane_length);
    QLine street_line_s_1(-lane_width, -lane_width, -lane_width, -lane_width-lane_length);
    QLine street_line_s_2(lane_width, -lane_width, lane_width, -lane_width-lane_length);
    QLine street_line_e_1(lane_width, lane_width, lane_width+lane_length, lane_width);
    QLine street_line_e_2(lane_width, -lane_width, lane_width+lane_length, -lane_width);
    QLine street_line_w_1(-lane_width, lane_width, -lane_width-lane_length, lane_width);
    QLine street_line_w_2(-lane_width, -lane_width, -lane_width-lane_length, -lane_width);

    painter.setPen(QPen(black, line_thickness));
    painter.setBrush(black);

    painter.drawLine(street_line_n_1);
    painter.drawLine(street_line_n_2);
    painter.drawLine(street_line_s_1);
    painter.drawLine(street_line_s_2);
    painter.drawLine(street_line_e_1);
    painter.drawLine(street_line_e_2);
    painter.drawLine(street_line_w_1);
    painter.drawLine(street_line_w_2);

    // TODO: replace with member variables
    std::vector<double> n_cars;
    n_cars.push_back(0.0*10);
    n_cars.push_back(20.0*10);
    std::vector<double> s_cars;
    s_cars.push_back(10.0*10);
    s_cars.push_back(15.0*10);
    std::vector<double> e_cars;
    e_cars.push_back(15.0*10);
    e_cars.push_back(35.0*10);
    std::vector<double> w_cars;
    w_cars.push_back(5.0*10);
    w_cars.push_back(15.0*10);

    painter.setPen(QPen(green, 0));
    painter.setBrush(green);

    for(const double &distance : n_cars) {
        QRect car_rect(-lane_width/2-car_width/2, lane_width+distance, car_width, car_length);
        painter.drawRoundedRect(car_rect, corner_radius, corner_radius);
    }
    for(const double &distance : s_cars) {
        QRect car_rect(lane_width/2-car_width/2, -lane_width-distance-car_length, car_width, car_length);
        painter.drawRoundedRect(car_rect, corner_radius, corner_radius);
    }
    for(const double &distance : e_cars) {
        QRect car_rect(lane_width+distance, lane_width/2-car_width/2, car_length, car_width);
        painter.drawRoundedRect(car_rect, corner_radius, corner_radius);
    }
    for(const double &distance : w_cars) {
        QRect car_rect(-lane_width-distance-car_length, -lane_width/2-car_width/2, car_length, car_width);
        painter.drawRoundedRect(car_rect, corner_radius, corner_radius);
    }

    // TODO: replace fields by member variable
    bool fields[] = {true, false, true, false};
    painter.setPen(QPen(green, 0));
    painter.setBrush(green);

    if(fields[0]) {
        QRect field_rect(-lane_width, 0, lane_width, lane_width);
        painter.drawRect(field_rect);
    }
    if(fields[1]) {
        QRect field_rect(0, 0, lane_width, lane_width);
        painter.drawRect(field_rect);
    }
    if(fields[2]) {
        QRect field_rect(-lane_width, -lane_width, lane_width, lane_width);
        painter.drawRect(field_rect);
    }
    if(fields[3]) {
        QRect field_rect(0, -lane_width, lane_width, lane_width);
        painter.drawRect(field_rect);
    }
}
