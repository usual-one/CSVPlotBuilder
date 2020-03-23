#include "include/UI/UI_utils.h"

#include <QImage>
#include <QStandardItemModel>

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
