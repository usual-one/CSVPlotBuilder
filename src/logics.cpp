#include "../include/logics.h"

#include <cstdlib>

vector<string> HEADERS = {};
vector<vector<string>> FIELDS = {};

res_t exec_op(op_args args) {
    res_t results = {};
    switch (args.operation_type) {
    case LOAD_DATA: {
        auto csv = readCSV(args.path, args.region, args.years);
        results.error_type = get<0>(csv);
        HEADERS = results.headers = get<1>(csv);
        FIELDS = results.arr = get<2>(csv);
        break;
    }
    case CALCULATE_METRICS: {
        auto metrics = getMetrics(args.column, args.metrics_type);
        results.error_type = get<0>(metrics);
        results.metric = get<1>(metrics);
        break;
    }
    default: {
        results.error_type = BAD_CODE;
        break;
    }
    }
    return results;
}

static bool isNumber(const string &str) {
    for (auto it = str.begin(); it != str.end(); it++) {
        if (!isspace(*it) && !isdigit(*it) && *it != '.' && *it != '-') {
            return false;
        }
    }
    return true;
}

static bool positiveNumberBetween(int number, pair<int, int> years) {
    if (!years.first) {
        return number <= years.second;
    }
    if (!years.second) {
        return number >= years.first;
    }
    return years.first <= number && number <= years.second;
}

static bool stringCmp(string str, string model) {
    return str == model;
}

static err_t isValid(const vector<string> &record, const string &region, pair<int, int> years) {
    if (years.first != 0 || years.second != 0) {
        if (!record.at(0).size() || !isNumber(record.at(0))) {
            return YEARS_NOT_FOUND;
        }
        if (!positiveNumberBetween(stoi(record.at(0)), years)) {
            return YEARS_NOT_FOUND;
        }
    }
    if (region.size()) {
        if (!record.at(1).size()) {
            return REGION_NOT_FOUND;
        }
        if (!stringCmp(record.at(1), region)) {
            return REGION_NOT_FOUND;
        }
    }
    return OK;
}

vector<string> splitStr(const string &str, const string &sep)
{
    vector<string> arr;
    size_t prev = 0;
    size_t next;
    size_t delta = sep.length();
    while ((next = str.find(sep, prev)) != string::npos) {
        arr.push_back(str.substr(prev, next - prev));
        prev = next + delta;
    }
    arr.push_back(str.substr(prev));
    return arr;
}

tuple<err_t, vector<string>, vector<vector<string>>> readCSV(const string &path, const string &region, pair<int, int> years) {
    ifstream fin(path);
    if (!fin.is_open()) {
        return {FILE_OPENING_ERROR, {}, {}};
    }

    string received_str;
    getline(fin, received_str);
    vector<string> headers = splitStr(received_str, ",");

    vector<vector<string>> records;

    err_t error = OK;
    while (!fin.eof()) {
        getline(fin, received_str);
        vector<string> record = splitStr(received_str, ",");
        if (record.size() != headers.size()) {
            continue;
        }

        error = isValid(record, region, years);
        if (!error) {
            records.push_back(record);
        }
    }
    if (!records.size()) {
        return {error, {}, {}};
    }
    return {OK, headers, records};
}

static size_t nameToInt(const vector<string> &names, const string &name)
{
    auto it = find(names.begin(), names.end(), name);
    if (it == names.end()) {
        return -1;
    }
    return distance(names.begin(), it);
}

tuple<err_t, double> getMetrics(const string &column, metrics_t type) {
    if (!FIELDS.size() || !HEADERS.size()) {
        return {DATA_NOT_FOUND, 0};
    }

    if (!column.size()) {
        return {WRONG_COLUMN_NAME, 0};
    }

    int index = -1;

    if (isNumber(column)) {
        index = stoi(column);
    } else {
        index = nameToInt(HEADERS, column);
    }

    if (index < 0 || index >= (int) FIELDS.size()) {
        return {WRONG_COLUMN_NAME, 0};
    }

    vector<double> col;
    for (auto it = FIELDS.begin(); it != FIELDS.end(); it++) {
        if (!(*it).at(index).size()) {
            continue;
        }
        if (isNumber((*it).at(index))) {
            col.push_back(stod((*it).at(index)));
        } else {
            return {INVALID_COLUMN_VALUES, 0};
        }
    }

    if (!col.size()) {
        return {COLUMN_IS_EMPTY, 0};
    }
    switch (type) {
    case MIN:
        return {OK, getMinimum(col)};
    case MAX:
        return {OK, getMaximum(col)};
    case MEDIAN:
        return {OK, getMedian(col)};
    default:
        return {BAD_CODE, 0};
    }
}

double getMinimum(const vector<double> &arr) {
    return *min_element(arr.begin(), arr.end());
}

double getMaximum(const vector<double> &arr) {
    return *max_element(arr.begin(), arr.end());
}

double getMedian(const vector<double> &arr) {
    if (arr.size() % 2 == 1) {
        return arr.at(arr.size() / 2);
    }
    return (arr.at(arr.size() / 2) + arr.at(arr.size() / 2 - 1)) / 2;
}
