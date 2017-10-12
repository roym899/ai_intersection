#include "qintersectionwidget.h"

#include <iostream>
#include <sstream>
#include <cstdlib>

QIntersectionWidget::QIntersectionWidget(QWidget *parent) : QWidget(parent), timer_interval(33), current_sim_time(0)
{
    // read in as much as possible
    std::string input_line;
    while(std::getline(std::cin, input_line)) {
        std::istringstream line_stream(input_line);
        std::string token;
        int count = 0;
        double time_stamp;
        line_stream >> unscaled_lane_length >> time_stamp;
        sim_strings.push(std::make_pair(line_stream.str(), time_stamp));
    }

    update_timer = new QTimer(this);
    connect(update_timer, SIGNAL(timeout()), this, SLOT(update()));
    update_timer->start(timer_interval);

    for(int i=0; i<sizeof(fields); ++i)
        fields[i] = 0;
}

void QIntersectionWidget::paintEvent(QPaintEvent *event)
{
    // units multiplied by 10 to account for integer accuracy (everything given in meters)
    const double scale_factor = 10;

    const double lane_width = 3.0 * scale_factor;
    const double lane_length = unscaled_lane_length * scale_factor;
    const double margin = 2.0 * scale_factor;
    const double total_dimensions = 2*lane_width+2*lane_length+2*margin;
    const double car_width = 1.8 * scale_factor;
    const double car_length = 4 * scale_factor;
    const double line_thickness = 0.25 *scale_factor;
    const double corner_radius = 0.3 * scale_factor;

    current_sim_time += timer_interval/1000;
    std::cout << current_sim_time << std::endl;


    // search for most up-to-date element in queue
    while(sim_strings.size() != 0 && sim_strings.front().second < current_sim_time) {
        std::cout << "remove from queue" << std::endl;
        n_cars.clear();
        s_cars.clear();
        e_cars.clear();
        w_cars.clear();
        std::istringstream string_stream(sim_strings.front().first);
        sim_strings.pop();
        std::string token;
        string_stream >> token;
        string_stream >> token;
        int count =  0;
        char current_lane;
        while(string_stream >> token) {
            if(count < 4) {
                fields[count] = std::stoi(token);
                ++count;
            }
            else {
                if(token == "N")
                    current_lane = 'N';
                else if(token == "E")
                    current_lane = 'E';
                else if(token == "W")
                    current_lane = 'W';
                else if(token == "S")
                    current_lane = 'S';
                else {
                    int id = std::stoi(token);
                    while (id > car_colors.size()) {
                        // id appearing for the first time, generate new color for this car
                        // while loop, if e.g. id 2 appears first -> there should be a color for id 1 as well
                        // note that car id 1 has color 0 !
                        int r = rand() % 255 + 0;
                        int g = rand() % 255 + 0;
                        int b = rand() % 255 + 0;
                        car_colors.push_back(QColor(r,g,b));
                    }
                    string_stream >>  token;
                    if(std::stod(token) > 0) {
                        switch(current_lane) {
                        case 'N':
                            n_cars.push_back(std::make_pair(std::stod(token)*scale_factor, id));
                            break;
                        case 'S':
                            s_cars.push_back(std::make_pair(std::stod(token)*scale_factor, id));
                            break;
                        case 'E':
                            e_cars.push_back(std::make_pair(std::stod(token)*scale_factor, id));
                            break;
                        case 'W':
                            w_cars.push_back(std::make_pair(std::stod(token)*scale_factor, id));
                            break;

                        }
                    }
                }
            }
        }
    }


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



    for(auto &pair : n_cars) {
        double distance = pair.first;
        int id = pair.second;
        painter.setPen(car_colors[id-1]);
        painter.setBrush(car_colors[id-1]);
        QRect car_rect(-lane_width/2-car_width/2, lane_width+distance, car_width, car_length);
        painter.drawRoundedRect(car_rect, corner_radius, corner_radius);
    }
    for(const auto &pair : s_cars) {
        double distance = pair.first;
        int id = pair.second;
        painter.setPen(car_colors[id-1]);
        painter.setBrush(car_colors[id-1]);
        QRect car_rect(lane_width/2-car_width/2, -lane_width-distance-car_length, car_width, car_length);
        painter.drawRoundedRect(car_rect, corner_radius, corner_radius);
    }
    for(const auto &pair : e_cars) {
        double distance = pair.first;
        int id = pair.second;
        painter.setPen(car_colors[id-1]);
        painter.setBrush(car_colors[id-1]);
        QRect car_rect(lane_width+distance, lane_width/2-car_width/2, car_length, car_width);
        painter.drawRoundedRect(car_rect, corner_radius, corner_radius);
    }
    for(const auto &pair : w_cars) {
        double distance = pair.first;
        int id = pair.second;
        painter.setPen(car_colors[id-1]);
        painter.setBrush(car_colors[id-1]);
        QRect car_rect(-lane_width-distance-car_length, -lane_width/2-car_width/2, car_length, car_width);
        painter.drawRoundedRect(car_rect, corner_radius, corner_radius);
    }


    if(fields[0] != 0) {
        painter.setPen(car_colors[fields[0]-1]);
        painter.setBrush(car_colors[fields[0]-1]);
        QRect field_rect(-lane_width, 0, lane_width, lane_width);
        painter.drawRect(field_rect);
    }
    if(fields[1] != 0) {
        painter.setPen(car_colors[fields[1]-1]);
        painter.setBrush(car_colors[fields[1]-1]);
        QRect field_rect(0, 0, lane_width, lane_width);
        painter.drawRect(field_rect);
    }
    if(fields[2] != 0) {
        painter.setPen(car_colors[fields[2]-1]);
        painter.setBrush(car_colors[fields[2]-1]);
        QRect field_rect(-lane_width, -lane_width, lane_width, lane_width);
        painter.drawRect(field_rect);
    }
    if(fields[3] != 0) {
        painter.setPen(car_colors[fields[3]-1]);
        painter.setBrush(car_colors[fields[3]-1]);
        QRect field_rect(0, -lane_width, lane_width, lane_width);
        painter.drawRect(field_rect);
    }
}
