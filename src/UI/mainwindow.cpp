#include "include/UI/mainwindow.h"
#include "ui_mainwindow.h"

#include <QFileDialog>
#include <QStandardItemModel>

#include <QDebug>

#include "include/logics/logics.h"

#define ERROR_DISPLAYING_TIMEOUT 2000

MainWindow::MainWindow(QWidget *parent)
        : QMainWindow(parent), ui(new Ui::MainWindow) {
    ui->setupUi(this);
    setlocale(LC_ALL, "C");

    connect(ui->btn_browse, SIGNAL(clicked()), this, SLOT(setPath()));
    connect(ui->btn_load, SIGNAL(clicked()), this, SLOT(showFields()));
    connect(ui->btn_calculate, SIGNAL(clicked()), this, SLOT(showResults()));
    connect(ui->btn_add, SIGNAL(clicked()), this, SLOT(addRegion()));
}

MainWindow::~MainWindow() {
    delete ui;
}

void MainWindow::setPath() {
    QString file_path = QFileDialog::getOpenFileName(this, "../lw-2/resource", "../lw-2/resource",
                                                     tr("CSV tables (*.csv)"));
    ui->ln_path->setText(file_path);
}


void MainWindow::showFields() {
    op_args request_args = {};
    request_args.path = ui->ln_path->text().toStdString();
    request_args.regions.push_back(ui->ln_region->text().toStdString());
    request_args.years = {ui->ln_init_year->text().toInt(), ui->ln_final_year->text().toInt()};
    request_args.operation_type = LOAD_DATA;
    res_t response = exec_op(request_args);

    QStandardItemModel *model = new QStandardItemModel;

    if (response.error_type == FILE_OPENING_ERROR) {
        ui->statusBar->showMessage("Cannot open the file", ERROR_DISPLAYING_TIMEOUT);
        return;
    }
    if (response.error_type == REGION_NOT_FOUND) {
        ui->statusBar->showMessage("No records with such region", ERROR_DISPLAYING_TIMEOUT);
        ui->tbl_found->setModel(model);
        return;
    }
    if (response.error_type == YEARS_NOT_FOUND) {
        ui->statusBar->showMessage("No records with such years", ERROR_DISPLAYING_TIMEOUT);
        ui->tbl_found->setModel(model);
        return;
    }

    vector <string> headers = response.headers;
    vector <vector<string>> fields = response.arr;


    QStringList horizontal_headers;
    for (auto it = headers.begin(); it != headers.end(); it++) {
        horizontal_headers.append(QString::fromStdString(*it));
    }

    model->setHorizontalHeaderLabels(horizontal_headers);

    for (size_t i = 0; i < fields.size(); i++) {
        for (size_t j = 0; j < fields.at(i).size(); j++) {
            model->setItem(i, j, new QStandardItem(QString::fromStdString(fields.at(i).at(j))));
        }
    }

    ui->tbl_found->setModel(model);

    if (ui->ln_region->text().size()) {
        ui->lst_regions->clear();
        ui->lst_regions->addItem(ui->ln_region->text());
    }
}

void MainWindow::showResults() {
    op_args request_args = {};
    request_args.path = ui->ln_path->text().toStdString();
    request_args.column = ui->ln_column->text().toStdString();
    request_args.years = {ui->ln_init_year->text().toInt(), ui->ln_final_year->text().toInt()};
    request_args.operation_type = CALCULATE_METRICS;

    QList<QListWidgetItem*> items = ui->lst_regions->findItems(QString("*"), Qt::MatchWrap | Qt::MatchWildcard);
    for (auto item : items) {
        request_args.regions.push_back(item->text().toStdString());
    }

    res_t response = exec_op(request_args);

    if (response.error_type == DATA_NOT_FOUND) {
        ui->statusBar->showMessage("Data is not loaded", ERROR_DISPLAYING_TIMEOUT);
        return;
    }
    if (response.error_type == WRONG_COLUMN_NAME) {
        ui->statusBar->showMessage("Such column does not exist", ERROR_DISPLAYING_TIMEOUT);
        return;
    }
    if (response.error_type == INVALID_COLUMN_VALUES) {
        ui->statusBar->showMessage("Invalid column values", ERROR_DISPLAYING_TIMEOUT);
        return;
    }
    if (response.error_type == COLUMN_IS_EMPTY) {
        ui->statusBar->showMessage("Column is empty", ERROR_DISPLAYING_TIMEOUT);
        return;
    }

    showMetrics(response);
    showPlot(response);
}

void MainWindow::showMetrics(res_t loaded_data) {
    const int metrics_count = 3;
    QStandardItemModel *model = new QStandardItemModel;
    model->setHorizontalHeaderLabels({"Metric", "Value"});

    for (size_t region_index = 0; region_index < loaded_data.metrics.size(); region_index++) {
        model->setItem(region_index * metrics_count, 0, new QStandardItem("Minimum"));
        model->setItem(region_index * metrics_count + 1, 0, new QStandardItem("Maximum"));
        model->setItem(region_index * metrics_count + 2, 0, new QStandardItem("Median"));
        model->setItem(region_index * metrics_count, 1, new QStandardItem(QString::number(loaded_data.metrics.at(region_index).at(0))));
        model->setItem(region_index * metrics_count + 1, 1, new QStandardItem(QString::number(loaded_data.metrics.at(region_index).at(1))));
        model->setItem(region_index * metrics_count + 2, 1, new QStandardItem(QString::number(loaded_data.metrics.at(region_index).at(2))));
    }

    ui->tbl_res->setModel(model);
}

void MainWindow::showPlot(res_t loaded_data)
{
    for (size_t i = 0; i < loaded_data.col_values.size(); i++) {
        ui->wdg_plot->addGraph();
        QVector <double> years = QVector<double>(loaded_data.col_values.at(i).at(0).begin(), loaded_data.col_values.at(i).at(0).end());
        QVector <double> column = QVector<double>(loaded_data.col_values.at(i).at(1).begin(), loaded_data.col_values.at(i).at(1).end());
        ui->wdg_plot->graph(i)->setData(years, column);
        if (!i) {
            ui->wdg_plot->graph(i)->rescaleAxes();
        } else {
            ui->wdg_plot->graph(i)->rescaleAxes(true);
        }
    }
    ui->wdg_plot->replot();
}

void MainWindow::addRegion()
{
    QString region = ui->ln_add_region->text();
    ui->ln_add_region->clear();

    if (!ui->lst_regions->count()) {
        ui->statusBar->showMessage("Main region is not selected", ERROR_DISPLAYING_TIMEOUT);
        return;
    }

    if (ui->lst_regions->findItems(region, Qt::MatchExactly).size()) {
        ui->statusBar->showMessage("Such region has been already added", ERROR_DISPLAYING_TIMEOUT);
        return;
    }

    ui->lst_regions->addItem(region);
}
