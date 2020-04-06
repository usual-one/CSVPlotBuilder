#include "include/UI/plotting/plotting.h"
#include "include/logics/logics_utils.h"

#include <QPainter>
#include <cmath>

static QPair <axis_t, axis_t> getAxes(canvas_t &canvas, const plot_t &plot) {
    const QVector <double> points = {1, 2, 3, 4, 5, 6, 7, 8, 9};

    QFont font = canvas.painter->font();
    QFontMetrics font_metrics(font);
    int font_height = font_metrics.height();

    axis_t x_axis = {};

    x_axis.name = plot.labels.first;

    x_axis.canvas_line.p1.x = CANVAS_MARGIN + font_height * 2 + CANVAS_INNER_MARGIN * 2;
    x_axis.canvas_line.p1.y = canvas.pixmap->height() - (CANVAS_MARGIN + font_height * 2 + CANVAS_INNER_MARGIN * 2);
    x_axis.canvas_line.p2.x = canvas.pixmap->width() - CANVAS_MARGIN;
    x_axis.canvas_line.p2.y = canvas.pixmap->height() - (CANVAS_MARGIN + font_height * 2 + CANVAS_INNER_MARGIN * 2);

    x_axis.plot_line.p1.x = plot.size.x_bottom;
    x_axis.plot_line.p1.y = plot.size.y_bottom;
    x_axis.plot_line.p2.x = plot.size.x_top;
    x_axis.plot_line.p2.y = plot.size.y_bottom;

    x_axis.order = getOrder(plot.size.x_top - plot.size.x_bottom);

    x_axis.factor = (x_axis.canvas_line.p2.x - x_axis.canvas_line.p1.x) / (x_axis.plot_line.p2.x - x_axis.plot_line.p1.x);

    for (auto point : points) {
        double plot_point_x = point * pow(10, x_axis.order - 1) + (int) x_axis.plot_line.p1.x;
        if (plot_point_x > x_axis.plot_line.p1.x && plot_point_x <= x_axis.plot_line.p2.x) {
            point_t plot_point;
            plot_point.x = plot_point_x;
            plot_point.y = x_axis.plot_line.p1.y;
            x_axis.plot_points.push_back(plot_point);
            point_t canvas_point;
            canvas_point.x = x_axis.canvas_line.p1.x + x_axis.factor * (plot_point_x - x_axis.plot_line.p1.x);
            canvas_point.y = x_axis.canvas_line.p1.y;
            x_axis.canvas_points.push_back(canvas_point);
        }
    }


    axis_t y_axis = {};

    y_axis.name = plot.labels.second;

    y_axis.canvas_line.p1.x = CANVAS_MARGIN + font_height * 2 + CANVAS_INNER_MARGIN * 2;
    y_axis.canvas_line.p1.y = canvas.pixmap->height() - (CANVAS_MARGIN + font_height * 2 + CANVAS_INNER_MARGIN * 2);
    y_axis.canvas_line.p2.x = CANVAS_MARGIN + font_height * 2 + CANVAS_INNER_MARGIN * 2;
    y_axis.canvas_line.p2.y = CANVAS_MARGIN;

    y_axis.plot_line.p1.x = plot.size.x_bottom;
    y_axis.plot_line.p1.y = plot.size.y_bottom;
    y_axis.plot_line.p2.x = plot.size.x_bottom;
    y_axis.plot_line.p2.y = plot.size.y_top;

    y_axis.order = getOrder(plot.size.y_top - plot.size.y_bottom);

    y_axis.factor = (y_axis.canvas_line.p1.y - y_axis.canvas_line.p2.y) / (y_axis.plot_line.p2.y - y_axis.plot_line.p1.y);

    for (auto point : points) {
        double plot_point_y = point * pow(10, y_axis.order - 1) + (int) y_axis.plot_line.p1.y;
        if (plot_point_y > y_axis.plot_line.p1.y && plot_point_y <= y_axis.plot_line.p2.y) {
            point_t plot_point;
            plot_point.x = y_axis.plot_line.p1.x;
            plot_point.y = plot_point_y;
            y_axis.plot_points.push_back(plot_point);
            point_t canvas_point;
            canvas_point.x = y_axis.canvas_line.p1.x;
            canvas_point.y = y_axis.canvas_line.p1.y - y_axis.factor * (plot_point_y - y_axis.plot_line.p1.y);
            y_axis.canvas_points.push_back(canvas_point);
        }
    }


    return {x_axis, y_axis};
}

static void drawRotatedText(canvas_t &canvas, const point_t &point, const QString &text, qreal angle) {
    canvas.painter->save();
    canvas.painter->translate(point.x, point.y);
    canvas.painter->rotate(angle);
    canvas.painter->drawText(0, 0, text);
    canvas.painter->restore();
}

static void drawAxes(canvas_t &canvas, const QPair <axis_t, axis_t> &axes) {
    QFont font = canvas.painter->font();
    QFontMetrics font_metrics(font);


    canvas.painter->setPen(AXES_COLOR);

    // axes
    canvas.painter->drawLine(axes.first.canvas_line.p1.x, axes.first.canvas_line.p1.y,
                             axes.first.canvas_line.p2.x, axes.first.canvas_line.p2.y);
    canvas.painter->drawLine(axes.second.canvas_line.p1.x, axes.second.canvas_line.p1.y,
                             axes.second.canvas_line.p2.x, axes.second.canvas_line.p2.y);

    // point dashes, grid, point labels
    for (int i = 0; i < axes.first.canvas_points.size(); i++) {
        auto point = axes.first.canvas_points[i];
        canvas.painter->setPen(GRID_COLOR);
        canvas.painter->drawLine(QLine(QPoint(point.x, point.y),
                                       QPoint(point.x, axes.second.canvas_line.p2.y)));
        canvas.painter->setPen(AXES_COLOR);
        canvas.painter->drawLine(QLine(QPoint(point.x, point.y),
                                       QPoint(point.x, point.y - DASH_LENGTH)));
        label_t point_label;
        point_label.text = QString::number(axes.first.plot_points[i].x);
        point_label.size = {(double) font_metrics.horizontalAdvance(point_label.text),
                            (double) font_metrics.height()};
        point_label.pos = {point.x - point_label.size.width / 2,
                           point.y + point_label.size.height + CANVAS_INNER_MARGIN};
        canvas.painter->drawText(point_label.pos.x, point_label.pos.y, point_label.text);
    }

    for (int i = 0; i < axes.second.canvas_points.size(); i++) {
        auto point = axes.second.canvas_points[i];
        canvas.painter->setPen(GRID_COLOR);
        canvas.painter->drawLine(QLine(QPoint(point.x, point.y),
                                       QPoint(axes.first.canvas_line.p2.x, point.y)));
        canvas.painter->setPen(AXES_COLOR);
        canvas.painter->drawLine(QLine(QPoint(point.x, point.y),
                                       QPoint(point.x + DASH_LENGTH, point.y)));
        label_t point_label;
        point_label.text = QString::number(axes.second.plot_points[i].y);
        point_label.size = {(double) font_metrics.horizontalAdvance(point_label.text),
                            (double) font_metrics.height()};
        point_label.pos = {point.x - CANVAS_INNER_MARGIN,
                           point.y + point_label.size.width / 2};
        drawRotatedText(canvas, point_label.pos, point_label.text, -90);
    }

    // axes labels
    label_t x_label;
    x_label.text = axes.first.name;
    x_label.size = {(double) font_metrics.horizontalAdvance(x_label.text),
                    (double) font_metrics.height()};
    x_label.pos = {axes.first.canvas_line.p1.x + ((axes.first.canvas_line.p2.x - axes.first.canvas_line.p1.x) / 2) - x_label.size.width / 2,
                   axes.first.canvas_line.p1.y + 2 * (x_label.size.height + CANVAS_INNER_MARGIN)};
    canvas.painter->drawText(x_label.pos.x, x_label.pos.y, x_label.text);

    label_t y_label;
    y_label.text = axes.second.name;
    y_label.size = {(double) font_metrics.horizontalAdvance(y_label.text),
                   (double) font_metrics.height()};
    y_label.pos = {axes.second.canvas_line.p1.x - 2 * CANVAS_INNER_MARGIN - y_label.size.height,
                  axes.second.canvas_line.p1.y - ((axes.second.canvas_line.p1.y - axes.second.canvas_line.p2.y) / 2) + y_label.size.width / 2};
    drawRotatedText(canvas, y_label.pos, y_label.text, -90);
}

static graph_t getGraph(const graph_t &graph, const QPair <axis_t, axis_t> &axes) {
    graph_t new_graph = {};
    new_graph.color = graph.color;
    new_graph.size = graph.size;

    for (auto value : graph.x_values) {
        new_graph.x_values.push_back(axes.first.canvas_line.p1.x + axes.first.factor * (value - axes.first.plot_line.p1.x));
    }
    for (auto value : graph.y_values) {
        new_graph.y_values.push_back(axes.second.canvas_line.p1.y - axes.second.factor * (value - axes.second.plot_line.p1.y));
    }
    for (auto value : graph.critical_x_values) {
        new_graph.critical_x_values.push_back(axes.first.canvas_line.p1.x + axes.first.factor * (value - axes.first.plot_line.p1.x));
    }
    for (auto value : graph.critical_y_values) {
        new_graph.critical_y_values.push_back(axes.second.canvas_line.p1.y - axes.second.factor * (value - axes.second.plot_line.p1.y));
    }

    return new_graph;
}

static void drawGraph(canvas_t &canvas, const graph_t &graph) {
    const double critical_dot_size = DOT_DIAMETR * DOT_CRITICAL_RATIO;
    const int pie_circle_angle = 5760;

    canvas.painter->setPen(graph.color);
    canvas.painter->setBrush(QBrush(graph.color));

    // values
    for (int i = 0; i < graph.x_values.size() - 1; i++) {
        canvas.painter->drawPie(graph.x_values[i] - DOT_DIAMETR / 2,
                                graph.y_values[i] - DOT_DIAMETR / 2,
                                DOT_DIAMETR, DOT_DIAMETR,
                                pie_circle_angle, pie_circle_angle);
        canvas.painter->drawLine(graph.x_values[i], graph.y_values[i],
                                 graph.x_values[i + 1], graph.y_values[i + 1]);
    }
    canvas.painter->drawPie(graph.x_values[graph.x_values.size() - 1] - DOT_DIAMETR / 2,
                            graph.y_values[graph.x_values.size() - 1] - DOT_DIAMETR / 2,
                            DOT_DIAMETR, DOT_DIAMETR,
                            pie_circle_angle, pie_circle_angle);

    // critical values
    for (int i = 0; i < graph.critical_x_values.size(); i++) {
        canvas.painter->drawPie(graph.critical_x_values[i] - critical_dot_size / 2,
                                graph.critical_y_values[i] - critical_dot_size / 2,
                                critical_dot_size, critical_dot_size,
                                pie_circle_angle, pie_circle_angle);
    }
}

static void drawPlot(canvas_t &canvas, const plot_t &plot) {
    const QFont canvas_font(CANVAS_FONT_FAMILY, CANVAS_FONT_SIZE);

    canvas.painter->setFont(canvas_font);
    canvas.painter->fillRect(0, 0, canvas.pixmap->width(), canvas.pixmap->height(), CANVAS_COLOR);

    auto axes = getAxes(canvas, plot);
    drawAxes(canvas, axes);

    for (auto graph : plot.graphs) {
        graph_t canvas_graph = getGraph(graph, axes);
        drawGraph(canvas, canvas_graph);
    }
}

canvas_t createCanvas(const QSize &size, const plot_t &plot) {
    canvas_t canvas = {};
    canvas.pixmap = new QPixmap(size);
    canvas.painter = new QPainter(canvas.pixmap);

    canvas.painter->setRenderHint(QPainter::Antialiasing);
    canvas.painter->setRenderHint(QPainter::HighQualityAntialiasing);

    drawPlot(canvas, plot);

    return canvas;
}

graph_size_t countPlotSize(const QVector<graph_t> &graphs, double margin) {
    graph_size_t size;
    size.x_top = graphs[0].size.x_top;
    size.x_bottom = graphs[0].size.x_bottom;
    size.y_top = graphs[0].size.y_top;
    size.y_bottom = graphs[0].size.y_bottom;

    for (auto graph : graphs) {
        if (graph.size.x_top > size.x_top) {
            size.x_top = graph.size.x_top;
        }
        if (graph.size.x_bottom < size.x_bottom) {
            size.x_bottom = graph.size.x_bottom;
        }
        if (graph.size.y_top > size.y_top) {
            size.y_top = graph.size.y_top;
        }
        if (graph.size.y_bottom < size.y_bottom) {
            size.y_bottom = graph.size.y_bottom;
        }
    }

    double diff = size.x_top - size.x_bottom;
    size.x_top += margin * diff;
    size.x_bottom -= margin * diff;
    diff = size.y_top - size.y_bottom;
    size.y_top += margin * diff;
    size.y_bottom -= margin * diff;

    return size;
}
