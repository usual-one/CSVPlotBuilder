#ifndef UI_UTILS_H
#define UI_UTILS_H

#include "lib/qcustomplot.h"

#include <vector>

#include <QPixmap>
#include <QColor>
#include <QTableView>
#include <QListWidget>
#include <QStandardItemModel>

// QPixmap ----------------------------------------------------------------------------------------

QPixmap createRect(int width, int height, QColor &color);

void clearPlot(QLabel *wdg_plot);

// QTableView, QStandartItemModel -----------------------------------------------------------------

void clearTable(QTableView *table);

void setBoldFont(QStandardItem *item);

// QListWidget ------------------------------------------------------------------------------------

QList <QString> getAllListItemsText(QListWidget *list_widget);

// QVector ----------------------------------------------------------------------------------------

template <typename T>
QVector <T> stdvectorToQvector(const std::vector <T> &vec) {
    return QVector <T> (vec.begin(), vec.end());
}

template <typename T>
std::vector <T> qvectorToStdvector(const QVector <T> &qvec) {
    return std::vector <T> (qvec.begin(), qvec.end());
}

// ------------------------------------------------------------------------------------------------

#endif // UI_UTILS_H
