#include "include/UI/mainwindow.h"
#include "ui_mainwindow.h"
#include "include/logics/logics.h"
#include "include/logics/logics_utils.h"
#include "include/UI/UI_utils.h"

#include <QFileDialog>
#include <QStandardItemModel>

#define ERROR_DISPLAYING_TIMEOUT 2000

MainWindow::MainWindow(QWidget *parent)
        : QMainWindow(parent), ui(new Ui::MainWindow) {
    ui->setupUi(this);
    setlocale(LC_ALL, "C");

    connect(ui->btn_browse, SIGNAL(clicked()), this, SLOT(setPath()));
    connect(ui->btn_load, SIGNAL(clicked()), this, SLOT(showFields()));
    connect(ui->btn_calculate, SIGNAL(clicked()), this, SLOT(showResults()));
    connect(ui->btn_add, SIGNAL(clicked()), this, SLOT(addRegion()));

    ui->lst_regions->setContextMenuPolicy(Qt::CustomContextMenu);

    connect(ui->lst_regions, SIGNAL(customContextMenuRequested(const QPoint &)), this, SLOT(showRegionsContextMenu(const QPoint &)));
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

        used_colors.clear();
        setColors(ui->lst_regions->count());
        ui->lst_regions->item(ui->lst_regions->count() - 1)->setIcon(createRect(32, 32, *(used_colors.end() - 1)));

        ui->ln_column->clear();
        ui->ln_add_region->clear();

        ui->wdg_plot->clearGraphs();
        ui->wdg_plot->replot();

        clearTable(ui->tbl_res);
    }
}

void MainWindow::showResults() {
    op_args request_args = {};
    request_args.path = ui->ln_path->text().toStdString();
    request_args.column = ui->ln_column->text().toStdString();
    request_args.years = {ui->ln_init_year->text().toInt(), ui->ln_final_year->text().toInt()};
    request_args.operation_type = CALCULATE_METRICS;

    for (auto item : getAllListItemsText(ui->lst_regions)) {
        request_args.regions.push_back(item.toStdString());
    }

    res_t response = exec_op(request_args);

    if (response.error_type == DATA_NOT_FOUND) {
        ui->statusBar->showMessage("Data is not loaded", ERROR_DISPLAYING_TIMEOUT);
        return;
    }
    if (response.error_type == WRONG_COLUMN_NAME) {
        ui->statusBar->showMessage("Wrong column name or number", ERROR_DISPLAYING_TIMEOUT);
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

    ui->ln_column->setText(QString::fromStdString(response.col_name));

    showMetrics(response);
    showPlot(response);
}

void MainWindow::setColors(int number) {
    QVector <QColor> usable_colors = {Qt::black, Qt::cyan, Qt::darkCyan, Qt::red, Qt::darkRed, Qt::magenta, Qt::darkMagenta,
                                      Qt::green, Qt::darkGreen, Qt::darkYellow, Qt::blue, Qt::darkBlue, Qt::gray, Qt::darkGray};
    for (auto color : used_colors) {
        usable_colors.removeAll(color);
    }
    number -= used_colors.size();
    if (usable_colors.size() < number) {
        return;
    }

    for (int i = 0; i < number; i++) {
        int color_index = generateRandInt(0, usable_colors.size() - 1);
        used_colors.push_back(usable_colors.at(color_index));
        usable_colors.removeAll(*(used_colors.end() - 1));
    }
}

void MainWindow::showMetrics(res_t loaded_data) {
    QList <QString> region_names = getAllListItemsText(ui->lst_regions);

    const int metrics_count = 3;
    QStandardItemModel *model = new QStandardItemModel;
    model->setHorizontalHeaderLabels({"Metric", "Value"});

    for (size_t region_index = 0; region_index < loaded_data.metrics.size(); region_index++) {
        auto item_region_label = new QStandardItem("Region");
        setBoldFont(item_region_label);
        model->setItem(region_index * (metrics_count + 1) + 0, 0, item_region_label);
        model->setItem(region_index * (metrics_count + 1) + 1, 0, new QStandardItem("Minimum"));
        model->setItem(region_index * (metrics_count + 1) + 2, 0, new QStandardItem("Maximum"));
        model->setItem(region_index * (metrics_count + 1) + 3, 0, new QStandardItem("Median"));

        auto item_region_value = new QStandardItem(region_names[region_index]);
        setBoldFont(item_region_value);
        model->setItem(region_index * (metrics_count + 1) + 0, 1, item_region_value);
        model->setItem(region_index * (metrics_count + 1) + 1, 1, new QStandardItem(QString::number(loaded_data.metrics[region_index].metrics[0])));
        model->setItem(region_index * (metrics_count + 1) + 2, 1, new QStandardItem(QString::number(loaded_data.metrics[region_index].metrics[1])));
        model->setItem(region_index * (metrics_count + 1) + 3, 1, new QStandardItem(QString::number(loaded_data.metrics[region_index].metrics[2])));
    }

    ui->tbl_res->setModel(model);
}

void MainWindow::showPlot(res_t loaded_data)
{
    const QString xAxisLabel = "year";
    ui->wdg_plot->xAxis->setLabel(xAxisLabel);
    ui->wdg_plot->yAxis->setLabel(QString::fromStdString(loaded_data.col_name));
    for (size_t i = 0; i < loaded_data.col_values.size(); i++) {
        ui->wdg_plot->addGraph();
        QVector <double> years = QVector<double>(loaded_data.col_values[i][0].begin(), loaded_data.col_values[i][0].end());
        QVector <double> column = QVector<double>(loaded_data.col_values[i][1].begin(), loaded_data.col_values[i][1].end());
        ui->wdg_plot->graph(i * 2)->setData(years, column);
        ui->wdg_plot->graph(i * 2)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssDisc, 4));
        ui->wdg_plot->graph(i * 2)->setPen(QPen(used_colors[i]));
        if (!i) {
            ui->wdg_plot->graph(i * 2)->rescaleAxes();
        } else {
            ui->wdg_plot->graph(i * 2)->rescaleAxes(true);
        }

        double max_range = ui->wdg_plot->yAxis->range().upper;
        double min_range = ui->wdg_plot->yAxis->range().lower;
        if (min_range > loaded_data.metrics[i].metrics[0] - 0.1 * abs(loaded_data.metrics[i].metrics[0])) {
            min_range = loaded_data.metrics[i].metrics[0] - 0.1 * abs(loaded_data.metrics[i].metrics[0]);
            ui->wdg_plot->yAxis->setRange(min_range, max_range);
        }
        if (max_range < loaded_data.metrics[i].metrics[1] + 0.1 * abs(loaded_data.metrics[i].metrics[1])) {
            max_range = loaded_data.metrics[i].metrics[1] + 0.1 * abs(loaded_data.metrics[i].metrics[1]);
            ui->wdg_plot->yAxis->setRange(min_range, max_range);
        }

        QVector <double> metrics_years = {};
        QVector <double> metrics_values = {};
        ui->wdg_plot->addGraph();
        for (auto metric_years : loaded_data.metrics[i].years) {
            for (auto year : metric_years) {
                metrics_years.push_back(year);
            }
        }
        for (size_t metric_index = 0; metric_index < loaded_data.metrics[i].metrics.size(); metric_index++) {
            for (size_t year_index = 0; year_index < loaded_data.metrics[i].years[metric_index].size(); year_index++) {
                metrics_values.push_back(loaded_data.metrics[i].metrics[metric_index]);
            }
        }
        ui->wdg_plot->graph(i * 2 + 1)->setData(metrics_years, metrics_values);
        ui->wdg_plot->graph(i * 2 + 1)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssDisc, 6));
        ui->wdg_plot->graph(i * 2 + 1)->setLineStyle(QCPGraph::lsNone);
        ui->wdg_plot->graph(i * 2 + 1)->setPen(QPen(used_colors[i]));
    }
    ui->wdg_plot->replot();
}

void MainWindow::addRegion()
{
    const int color_count = 14;
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

    if (ui->lst_regions->count() >= color_count) {
        ui->statusBar->showMessage("Too many regions", ERROR_DISPLAYING_TIMEOUT);
        return;
    }

    ui->lst_regions->addItem(region);
    setColors(ui->lst_regions->count());
    ui->lst_regions->item(ui->lst_regions->count() - 1)->setIcon(createRect(32, 32, *(used_colors.end() - 1)));
}

void MainWindow::showRegionsContextMenu(const QPoint &pos) {
    QPoint globalPos = ui->lst_regions->mapToGlobal(pos);

    QMenu menu;
    menu.addAction("Delete", this, SLOT(deleteRegionsItem()));

    menu.exec(globalPos);
}

void MainWindow::deleteRegionsItem() {
    for (int i = 0; i < ui->lst_regions->selectedItems().size(); i++) {
        QListWidgetItem *item = ui->lst_regions->takeItem(ui->lst_regions->currentRow());
        delete item;
    }
}
