#ifndef UI_UTILS_H
#define UI_UTILS_H

#include "lib/qcustomplot.h"

#include <QPixmap>
#include <QColor>
#include <QTableView>
#include <QListWidget>
#include <QStandardItemModel>

// QPixmap ----------------------------------------------------------------------------------------

QPixmap createRect(int width, int height, QColor &color);

// QTableView, QStandartItemModel -----------------------------------------------------------------

void clearTable(QTableView *table);

void setBoldFont(QStandardItem *item);

// QListWidget ------------------------------------------------------------------------------------

QList <QString> getAllListItemsText(QListWidget *list_widget);

// QCustomPlot ------------------------------------------------------------------------------------

void clearPlot(QCustomPlot *plot_widget);

#endif // UI_UTILS_H
