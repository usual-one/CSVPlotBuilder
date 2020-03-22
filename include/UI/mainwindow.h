#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "include/logics/logics.h"

#include <QMainWindow>

#include <string>
#include <vector>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void setPath();

    void showFields();

    void showResults();

    void addRegion();

private:
    void showMetrics(res_t loaded_data);

    void showPlot(res_t loaded_data);

    Ui::MainWindow *ui;
};
#endif // MAINWINDOW_H
