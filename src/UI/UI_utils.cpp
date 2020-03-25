#include "include/UI/UI_utils.h"

#include <QImage>

QPixmap createRect(int width, int height, QColor &color)
{
    QImage rect_img = QImage(width, height, QImage::Format_RGB32);
    rect_img.fill(color);
    QPixmap rect_pixmap = QPixmap();
    rect_pixmap.convertFromImage(rect_img);
    return rect_pixmap;
}

void clearTable(QTableView *table)
{
    QStandardItemModel *model = new QStandardItemModel;
    model->clear();
    table->setModel(model);
}

QList <QString> getAllListItemsText(QListWidget *list_widget) {
    QList <QListWidgetItem *> items = list_widget->findItems(QString("*"), Qt::MatchWrap | Qt::MatchWildcard);
    QList <QString> items_text = {};
    for (auto item : items) {
       items_text.push_back(item->text());
    }
    return items_text;
}

void setBoldFont(QStandardItem *item) {
    QFont new_font = item->font();
    new_font.setBold(true);
    item->setFont(new_font);
}
