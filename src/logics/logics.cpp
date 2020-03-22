#include "../../include/logics/logics.h"

#include <cstdlib>
#include <fstream>
#include <utility>
#include <tuple>

vector <string> HEADERS = {};
vector <vector<string>> FIELDS = {};

res_t exec_op(op_args args) {
    res_t results = {};
    switch (args.operation_type) {
        case LOAD_DATA: {
            auto csv = readCSV(args.path, args.regions.at(0), args.years);
            results.error_type = get<0>(csv);
            HEADERS = results.headers = get<1>(csv);
            FIELDS = results.arr = get<2>(csv);
            break;
        }
        case CALCULATE_METRICS: {
            auto metrics = getMetrics(args.column, MIN);
            results.metrics.push_back(get<1>(metrics));
            metrics = getMetrics(args.column, MAX);
            results.metrics.push_back(get<1>(metrics));
            metrics = getMetrics(args.column, MEDIAN);
            results.metrics.push_back(get<1>(metrics));
            results.error_type = get<0>(metrics);
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

static err_t isValid(const vector <string> &record, const string &region, pair<int, int> years) {
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

vector <string> splitStr(const string &str, const string &sep) {
    vector <string> arr;
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

tuple <err_t, vector <string>, vector <vector <string>>> readCSV(const string &path, const string &region, pair<int, int> years) {
    ifstream fin(path);
    if (!fin.is_open()) {
        return {FILE_OPENING_ERROR, {}, {}};
    }

    string received_str;
    getline(fin, received_str);
    vector <string> headers = splitStr(received_str, ",");

    vector <vector <string>> records;

    err_t error = OK;
    while (!fin.eof()) {
        getline(fin, received_str);
        vector <string> record = splitStr(received_str, ",");
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

static size_t nameToInt(const vector <string> &names, const string &name) {
    auto it = find(names.begin(), names.end(), name);
    if (it == names.end()) {
        return -1;
    }
    return distance(names.begin(), it);
}

static vector <double> svectorTodvector(const vector <string> &s_vec) {
    vector <double> d_vec;
    for (auto it = s_vec.begin(); it != s_vec.end(); it++) {
        if (!(*it).size()) {
            continue;
        }
        if (isNumber(*it)) {
            d_vec.push_back(stod(*it));
        } else {
            return {};
        }
    }
    return d_vec;
}

tuple <err_t, double> getMetrics(const string &column, metrics_t type) {
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

    if (index < 0 || index >= (int) HEADERS.size()) {
        return {WRONG_COLUMN_NAME, 0};
    }

    vector <string> s_col;
    for (auto it = FIELDS.begin(); it != FIELDS.end(); it++) {
        if ((*it).at(index).size()) {
            s_col.push_back((*it).at(index));
        }
    }
    if (!s_col.size()) {
        return {COLUMN_IS_EMPTY, 0};
    }

    vector <double> d_col = svectorTodvector(s_col);
    if (!d_col.size()) {
        return {INVALID_COLUMN_VALUES, 0};
    }

    switch (type) {
        case MIN:
            return {OK, getMinimum(d_col)};
        case MAX:
            return {OK, getMaximum(d_col)};
        case MEDIAN:
            return {OK, getMedian(d_col)};
        default:
            return {BAD_CODE, 0};
    }
}

double getMinimum(const vector <double> &arr) {
    return *min_element(arr.begin(), arr.end());
}

double getMaximum(const vector <double> &arr) {
    return *max_element(arr.begin(), arr.end());
}

double getMedian(const vector <double> &arr) {
    vector <double> tmp_arr(arr);
    sort(tmp_arr.begin(), tmp_arr.end());
    if (tmp_arr.size() % 2 == 1) {
        return tmp_arr.at(tmp_arr.size() / 2);
    }
    return (tmp_arr.at(tmp_arr.size() / 2) + tmp_arr.at(tmp_arr.size() / 2 - 1)) / 2;
}
