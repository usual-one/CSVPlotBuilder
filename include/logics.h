#ifndef LOGICS_H
#define LOGICS_H

#include <QDebug>

#include <fstream>
#include <string>
#include <vector>
#include <utility>
#include <tuple>

using namespace std;

enum metrics_t {
    MIN,
    MAX,
    MEDIAN
};

enum op_t {
    LOAD_DATA,
    CALCULATE_METRICS
};

enum err_t {
    OK = 0,
    FILE_OPENING_ERROR = 1,
    REGION_NOT_FOUND = 2,
    YEARS_NOT_FOUND = 3,
    WRONG_COLUMN_NAME = 4,
    COLUMN_IS_EMPTY = 5,
    INVALID_COLUMN_VALUES = 6,
    DATA_NOT_FOUND = 7,
    BAD_CODE = 8
};

typedef struct {
    op_t operation_type;
    string path;
    string region;
    pair<int, int> years;
    string column;
    metrics_t metrics_type;
} op_args;

typedef struct {
    err_t error_type;
    vector<vector<string>> arr;
    vector<string> headers;
    double metric;
} res_t;

res_t exec_op(op_args args);

tuple<err_t, vector<string>, vector<vector<string>>> readCSV(const string &path, const string &region, pair<int, int> years);

vector<string> splitStr(const string &str, const string &sep);

tuple<err_t, double> getMetrics(const string &column, metrics_t type);

double getMinimum(const vector<double> &arr);

double getMaximum(const vector<double> &arr);

double getMedian(const vector<double> &arr);

#endif // LOGICS_H
