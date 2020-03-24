#include "include/logics/logics.h"
#include "include/logics/logics_utils.h"

#include <fstream>
#include <utility>
#include <algorithm>

vector <string> HEADERS = {};
vector <vector<string>> FIELDS = {};

static int nameToInt(const vector <string> &names, const string &name) {
    int column_index = -1;
    if (isNumber(name)) {
        column_index = stoi(name) - 1;
    } else {
        auto it = find(names.begin(), names.end(), name);
        if (it == names.end()) {
            return -1;
        }
        column_index = distance(names.begin(), it);
    }
    return column_index;
}

res_t exec_op(op_args args) {
    res_t results = {};
    if (args.operation_type == LOAD_DATA) {
        auto csv = readCSV(args.path, args.regions.at(0), args.years);
        results.error_type = csv.first;
        HEADERS = results.headers = csv.second.first;
        FIELDS = results.arr = csv.second.second;
    } else if (args.operation_type == CALCULATE_METRICS) {
        auto regions_data_response = getRegionsData(args.path, args.regions, args.years);
        if (regions_data_response.first) {
            results.error_type = regions_data_response.first;
            return results;
        }
        FIELDS.clear();
        FIELDS = regions_data_response.second;
        auto col_values_response = getColValues(args.regions, args.column);
        if (col_values_response.first) {
            results.error_type = col_values_response.first;
            return results;
        }
        results.col_name = HEADERS.at(nameToInt(HEADERS, args.column));
        results.col_values = col_values_response.second;
        results.metrics = calculateAllMetrics(results.col_values);
    } else {
        results.error_type = BAD_CODE;
    }
    results.error_type = OK;
    return results;
}

vector <vector <vector <double>>> calculateAllMetrics(const vector <vector <vector <double>>> &col_values) {
    vector <vector <vector <double>>> all_metrics = {};
    // all_metrics = [region1[[years], [metrics]], region2[[years], [metrics]], ...]
    for (auto region : col_values) {
        // region = [[years], [values]]
        vector <vector <double>> region_metrics = {{}, {}};
        // region_metrics = [[years], [metrics]]
        region_metrics[1].push_back(getMinimum(region[1]));
        region_metrics[1].push_back(getMaximum(region[1]));
        region_metrics[1].push_back(getMedian(region[1]));
        region_metrics[0].push_back(region[0][getIndex(region[1], region_metrics[1][0])]); // minimum
        region_metrics[0].push_back(region[0][getIndex(region[1], region_metrics[1][1])]); // maximum
        int median_index = getIndex(region[1], region_metrics[1][2]);
        if (median_index == -1) {
            vector <int> places = findPlaces(region[1], region_metrics[1][2]);
            for (auto place : places) {
                // x0 = (((y0 - y1) * (x2 - x1)) / (y2 - y1)) + x1
                // x0 - median_year, y0 - median_value
                // x1, x2 - year, y1, y2 - values (nearest points)
                double median_year = (((region_metrics[1][2] - region[1][place]) * (region[0][place + 1] - region[0][place])) / (region[1][place + 1] - region[1][place])) + region[0][place];
                region_metrics[0].push_back(median_year);
                region_metrics[1].push_back(region_metrics[1][2]);
            }
            region_metrics[1].pop_back();
        } else {
            region_metrics[0].push_back(region[0][median_index]);
        }
        all_metrics.push_back(region_metrics);
    }
    return all_metrics;
}

pair <err_t, vector <vector <vector <double>>>> getColValues(const vector <string> &regions, const string &column) {
    const vector <int> column_borders = {2, 6};

    int column_index = nameToInt(HEADERS, column);

    if (!(column_borders[0] <= column_index && column_index <= column_borders[1])) {
        return {WRONG_COLUMN_NAME, {}};
    }

    vector <vector <vector <double>>> all_col_values = {};
    for (size_t i = 0; i < regions.size(); i++) {
        all_col_values.push_back({{}, {}});
    }

    for (auto record : FIELDS) {
        int region_index = distance(regions.begin(), find(regions.begin(), regions.end(), record.at(1)));
        all_col_values.at(region_index).at(0).push_back(stod(record.at(0)));
        all_col_values.at(region_index).at(1).push_back(stod(record.at(column_index)));
    }
    return {OK, all_col_values};
}

pair <err_t, vector <vector <string>>> getRegionsData(const string &path, const vector <string> &regions, pair<int, int> years) {
    vector <vector <string>> region_data= {};
    for (auto region : regions) {
        auto reading_res = readCSV(path, region, years);
        if (reading_res.first) {
            return {reading_res.first, {}};
        }
        region_data.insert(region_data.end(), reading_res.second.second.begin(), reading_res.second.second.end());
    }
    return {OK, region_data};
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

pair <err_t, pair <vector <string>, vector <vector <string>>>> readCSV(const string &path, const string &region, pair<int, int> years) {
    ifstream fin(path);
    if (!fin.is_open()) {
        return {FILE_OPENING_ERROR, {{}, {}}};
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
        return {error, {{}, {}}};
    }
    return {OK, {headers, records}};
}

//static vector <double> svectorTodvector(const vector <string> &s_vec) {
//    vector <double> d_vec;
//    for (auto it = s_vec.begin(); it != s_vec.end(); it++) {
//        if (!(*it).size()) {
//            continue;
//        }
//        if (isNumber(*it)) {
//            d_vec.push_back(stod(*it));
//        } else {
//            return {};
//        }
//    }
//    return d_vec;
//}

//tuple <err_t, double> getMetrics(const string &column, metrics_t type) {
//    if (!FIELDS.size() || !HEADERS.size()) {
//        return {DATA_NOT_FOUND, 0};
//    }



//    int index = -1;

//    if (isNumber(column)) {
//        index = stoi(column);
//    } else {
//        index = nameToInt(HEADERS, column);
//    }

//    if (index < 0 || index >= (int) HEADERS.size()) {
//        return {WRONG_COLUMN_NAME, 0};
//    }

//    vector <string> s_col;
//    for (auto it = FIELDS.begin(); it != FIELDS.end(); it++) {
//        if ((*it).at(index).size()) {
//            s_col.push_back((*it).at(index));
//        }
//    }
//    if (!s_col.size()) {
//        return {COLUMN_IS_EMPTY, 0};
//    }

//    vector <double> d_col = svectorTodvector(s_col);
//    if (!d_col.size()) {
//        return {INVALID_COLUMN_VALUES, 0};
//    }

//    switch (type) {
//        case MIN:
//            return {OK, getMinimum(d_col)};
//        case MAX:
//            return {OK, getMaximum(d_col)};
//        case MEDIAN:
//            return {OK, getMedian(d_col)};
//        default:
//            return {BAD_CODE, 0};
//    }
//}

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
