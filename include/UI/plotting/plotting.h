#ifndef PLOTTING_H
#define PLOTTING_H

#include "include/UI/plotting/plotting_types.h"

// Const values ---------------------------------

// colors
const QColor AXES_COLOR   = Qt::black;
const QColor CANVAS_COLOR = Qt::white;
const QColor GRID_COLOR   = Qt::lightGray;

// font
const QString CANVAS_FONT_FAMILY = "arial";
const int CANVAS_FONT_SIZE = 11;

// graph
const int DOT_DIAMETR = 6;
const double DOT_CRITICAL_RATIO = 1.5;
const int DASH_LENGTH = 10;

// margins
const int CANVAS_MARGIN = 5;
const int CANVAS_INNER_MARGIN = 2;

// ----------------------------------------------

canvas_t createCanvas(const QSize &size, const plot_t &plot);

graph_size_t countPlotSize(const QVector <graph_t> &graphs, double margin);
// margin = percents from max graph size

#endif // PLOTTING_H
