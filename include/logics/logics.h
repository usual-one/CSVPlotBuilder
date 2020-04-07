#ifndef LOGICS_H
#define LOGICS_H

#include <vector>
#include <string>

using namespace std;

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
    vector <double> metrics;
    vector <vector <double>> years;
    // [meric1[], metric2[], ...]
} metric_values_t;

typedef struct {
    vector <double> values;
    vector <double> years;
} column_values_t;


typedef struct {
    op_t operation_type;
    string path;
    vector <string> regions;
    pair <int, int> years;
    string column;
} op_args;

typedef struct {
    err_t error_type;
    vector <vector <string>> arr;
    vector <string> headers;
    vector <metric_values_t> metrics;
    // [region1[metric_values], region2[metric_values], ...]
    string col_name;
    vector <column_values_t> col_values;
    // [region1[column_values], region2[column_values], ...]
} res_t;

res_t exec_op(op_args args);

#endif // LOGICS_H
