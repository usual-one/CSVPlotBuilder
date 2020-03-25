#ifndef UI_UTILS_H
#define UI_UTILS_H

#include <QPixmap>
#include <QColor>
#include <QTableView>
#include <QListWidget>
#include <QStandardItemModel>

QPixmap createRect(int width, int height, QColor &color);

void clearTable(QTableView *table);

QList <QString> getAllListItemsText(QListWidget *list_widget);

void setBoldFont(QStandardItem *item);

#endif // UI_UTILS_H
