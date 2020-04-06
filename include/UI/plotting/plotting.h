#ifndef PLOTTING_H
#define PLOTTING_H

#include "include/UI/plotting/plotting_types.h"

canvas_t createCanvas(const QSize &size, const plot_t &plot);

graph_size_t countPlotSize(const QVector <graph_t> &graphs, double margin);
// margin = percents from max graph size

#endif // PLOTTING_H
