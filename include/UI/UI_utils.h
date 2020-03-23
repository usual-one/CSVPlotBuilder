#ifndef UI_UTILS_H
#define UI_UTILS_H

#include <QPixmap>
#include <QColor>
#include <QTableView>

QPixmap createRect(int width, int height, QColor &color);

void clearTable(QTableView *table);

#endif // UI_UTILS_H
