#ifndef PLOT_H
#define PLOT_H

#include "mainwindow.h"
#include "../../lib/qcustomplot.h"

#include <vector>

namespace plot {
    void createGraph(std::vector<double> &xAxisValues, std::vector<double> &yAxisValues);
}


#endif // PLOT_H
