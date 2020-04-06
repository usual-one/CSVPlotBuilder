#ifndef PLOTTING_TYPES_H
#define PLOTTING_TYPES_H

#include <QPixmap>

// drawing types --------------------------------

typedef struct {
    double x;
    double y;
} point_t;

typedef struct {
    point_t p1;
    point_t p2;
} line_t;

typedef struct {
    double width;
    double height;
} rect_t;

// plotting types -------------------------------

typedef struct {
    double x_top;
    double x_bottom;
    double y_top;
    double y_bottom;
} graph_size_t;

typedef struct {
    QVector <double> x_values;
    QVector <double> y_values;
    QVector <double> critical_x_values;
    QVector <double> critical_y_values;
    graph_size_t size;
    QColor color;
} graph_t;

typedef struct {
    QString text;
    rect_t size;
    point_t pos;
} label_t;

typedef struct {
    QString name;
    line_t canvas_line;
    line_t plot_line;
    QVector <point_t> canvas_points;
    QVector <point_t> plot_points;
    int order;
    double factor;
    // canvas_distance = factor * plot_distance
} axis_t;

typedef struct {
    QVector <graph_t> graphs;
    graph_size_t size;
    QPair <QString, QString> labels;
} plot_t;

typedef struct {
    QPixmap *pixmap;
    QPainter *painter;
} canvas_t;

#endif // PLOTTING_TYPES_H
