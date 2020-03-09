#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QFileDialog>
#include <QStandardItemModel>

#include <QDebug>

#include "include/logics.h"

#define ERROR_DISPLAYING_TIMEOUT 1000

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setlocale(LC_ALL, "C");

    connect(ui->btn_browse, SIGNAL(clicked()), this, SLOT(setPath()));
    connect(ui->btn_load, SIGNAL(clicked()), this, SLOT(showRegionFields()));
    connect(ui->btn_calculate, SIGNAL(clicked()), this, SLOT(showCalculationResults()));
}

MainWindow::~MainWindow() {
    delete ui;
}

void MainWindow::setPath() {
    QString file_path = QFileDialog::getOpenFileName(this, "../lw-2/resource", "../lw-2/resource", tr("CSV tables (*.csv)"));
    ui->ln_path->setText(file_path);
}


void MainWindow::showRegionFields() {
    op_args request_args = {};
    request_args.path = ui->ln_path->text().toStdString();
    request_args.region = ui->ln_region->text().toStdString();
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

    vector<string> headers = response.headers;
    vector<vector<string>> fields = response.arr;


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
}

void MainWindow::showCalculationResults()
{
    op_args request_args = {};
    request_args.column = ui->ln_column->text().toStdString();
    request_args.operation_type = CALCULATE_METRICS;

    QStandardItemModel *model = new QStandardItemModel;
    model->setHorizontalHeaderLabels({"Metric", "Value"});
    model->setItem(0, 0, new QStandardItem("Minimum"));
    model->setItem(1, 0, new QStandardItem("Maximum"));
    model->setItem(2, 0, new QStandardItem("Median"));

    request_args.metrics_type = MIN;
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
    model->setItem(0, 1, new QStandardItem(QString::number(exec_op(request_args).metric)));
    request_args.metrics_type = MAX;
    model->setItem(1, 1, new QStandardItem(QString::number(exec_op(request_args).metric)));
    request_args.metrics_type = MEDIAN;
    model->setItem(2, 1, new QStandardItem(QString::number(exec_op(request_args).metric)));

    ui->tbl_res->setModel(model);
}
