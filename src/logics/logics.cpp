#include "include/logics/logics.h"
#include "include/logics/logics_utils.h"

#include <fstream>
#include <utility>
#include <algorithm>

const int FIRST_VALID_COLUMN_INDEX = 2;
const int LAST_VALID_COLUMN_INDEX = 6;

vector <string> g_headers = {};
vector <vector<string>> g_fields = {};

static err_t isValid(const vector <string> &record, const string &region, pair<int, int> years) {
    if (years.first != 0 || years.second != 0) {
        if (!record[0].size() || !isNumber(record[0])) {
            return YEARS_NOT_FOUND;
        }
        if (!isNumberBetweenPair(stoi(record[0]), years)) {
            return YEARS_NOT_FOUND;
        }
    }
    if (region.size()) {
        if (!record[1].size()) {
            return REGION_NOT_FOUND;
        }
        if (record[1] != region) {
            return REGION_NOT_FOUND;
        }
    }
    return OK;
}

static pair <err_t, pair <vector <string>, vector <vector <string>>>>
readCSV(const string &path, const string &region, pair<int, int> years) {
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

    fin.close();
    if (!records.size()) {
        return {error, {{}, {}}};
    }
    return {OK, {headers, records}};
}

static pair <err_t, vector <vector <string>>>
getRegionsData(const string &path, const vector <string> &regions, pair<int, int> years) {
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

static int nameToInt(const vector <string> &names, const string &name) {
    int column_index = -1;
    if (isNumber(name)) {
        column_index = stoi(name) - 1;
    } else {
        column_index = getIndex(names, name);
    }
    return column_index;
}

static pair <err_t, vector <column_values_t>> getColValues(const vector <string> &regions, const string &column) {
    int column_index = nameToInt(g_headers, column);

    if (!(FIRST_VALID_COLUMN_INDEX <= column_index && column_index <= LAST_VALID_COLUMN_INDEX)) {
        return {WRONG_COLUMN_NAME, {}};
    }

    vector <column_values_t> all_col_values = {};
    for (auto region : regions) {
        all_col_values.push_back({});
    }

    for (auto record : g_fields) {
        int region_index = distance(regions.begin(), find(regions.begin(), regions.end(), record[1]));
        if (!record[0].size() || !record[column_index].size()) {
            continue;
        }
        all_col_values[region_index].years.push_back(stod(record[0]));
        all_col_values[region_index].values.push_back(stod(record[column_index]));
    }

    for (auto region : all_col_values) {
        if (!region.years.size() || !region.values.size()) {
            return {COLUMN_IS_EMPTY, {}};
        }
    }
    return {OK, all_col_values};
}

static vector <metric_values_t> calculateAllMetrics(const vector <column_values_t> &region_values) {
    vector <metric_values_t> all_metrics = {};
    // all_metrics = [region1[metric_values], region2[metric_values], ...]
    for (auto region : region_values) {
        metric_values_t region_metrics = {};
        region_metrics.metrics.push_back(getMinimum(region.values));
        region_metrics.metrics.push_back(getMaximum(region.values));
        region_metrics.metrics.push_back(getMedian(region.values));

        region_metrics.years.push_back({});
        for (auto index : getIndices(region.values, region_metrics.metrics[0])) { // minimum
            region_metrics.years[0].push_back(region.years[index]);
        }
        region_metrics.years.push_back({});
        for (auto index : getIndices(region.values, region_metrics.metrics[1])) { // maximum
            region_metrics.years[1].push_back(region.years[index]);
        }

        region_metrics.years.push_back({});
        vector <int> median_indices = getIndices(region.values, region_metrics.metrics[2]); // median
        for (auto index : median_indices) {
            region_metrics.years[2].push_back(region.years[index]);
        }
        if (!median_indices.size()) {
            vector <int> median_places = findPlaces(region.values, region_metrics.metrics[2]);
            for (auto place : median_places) {
                // x0 = (((y0 - y1) * (x2 - x1)) / (y2 - y1)) + x1
                // x0 - median_year, y0 - median_value
                // x1, x2 - year, y1, y2 - values (nearest points)
                double median_year = (((region_metrics.metrics[2] - region.values[place]) * (region.years[place + 1] - region.years[place])) /
                        (region.values[place + 1] - region.values[place])) + region.years[place];
                region_metrics.years[2].push_back(median_year);
            }
        }

        all_metrics.push_back(region_metrics);
    }
    return all_metrics;
}

res_t exec_op(op_args args) {
    res_t results = {};
    if (args.operation_type == LOAD_DATA) {
        auto csv = readCSV(args.path, args.regions[0], args.years);
        if (csv.first) {
            results.error_type = csv.first;
            return results;
        }
        g_headers = results.headers = csv.second.first;
        g_fields = results.arr = csv.second.second;
    } else if (args.operation_type == CALCULATE_METRICS) {
        auto regions_data_response = getRegionsData(args.path, args.regions, args.years);
        if (regions_data_response.first) {
            results.error_type = regions_data_response.first;
            return results;
        }
        g_fields.clear();
        g_fields = regions_data_response.second;

        auto col_values_response = getColValues(args.regions, args.column);
        if (col_values_response.first) {
            results.error_type = col_values_response.first;
            return results;
        }
        results.col_values = col_values_response.second;

        results.col_name = g_headers[nameToInt(g_headers, args.column)];

        results.metrics = calculateAllMetrics(results.col_values);
    } else {
        results.error_type = BAD_CODE;
    }
    results.error_type = OK;
    return results;
}
