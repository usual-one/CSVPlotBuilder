#ifndef LOGICS_UTILS_H
#define LOGICS_UTILS_H

#include <string>
#include <vector>

// std::string ------------------------------------------------------------------------------------

bool isNumber(const std::string &str);

std::vector <std::string> splitStr(const std::string &str, const std::string &sep);

// int --------------------------------------------------------------------------------------------

int generateRandInt(int lowest, int highest);

// double -----------------------------------------------------------------------------------------

int getOrder(double magnitude);

// std::pair --------------------------------------------------------------------------------------

template  <typename T>
bool isNumberBetweenPair(T number, std::pair<T, T> years) {
    // if one member of pair == 0, then compares only to second
   if (!years.first) {
       return number <= years.second;
   }
   if (!years.second) {
       return number >= years.first;
   }
   return years.first <= number && number <= years.second;
}

// std::vector ------------------------------------------------------------------------------------

template <typename T>
int getIndex(const std::vector <T> &vec, T elem) {
    // returns first appearance of elem
    auto it = find(vec.begin(), vec.end(), elem);
    if (it == vec.end()) {
        return -1;
    }
    return std::distance(vec.begin(), it);
}

template <typename T>
std::vector <int> getIndices(const std::vector <T> &vec, T elem) {
    std::vector <int> indices = {};

    auto it = find(vec.begin(), vec.end(), elem);
    while (it != vec.end()) {
        indices.push_back(std::distance(vec.begin(), it));
        it = find(it + 1, vec.end(), elem);
    }
    return indices;
}

template <typename T>
std::vector <int> findPlaces(const std::vector <T> &vect, T value) {
    std::vector <int> places = {};
    for (auto it = vect.begin(); it < vect.end() - 1; it++) {
        if ((*it <= value && value <= *(it + 1)) || (*(it + 1) <= value && value <= *it)) {
            places.push_back(std::distance(vect.begin(), it));
        }
    }
    return places;
}

template <typename T>
T getMinimum(const std::vector <T> &vec) {
    return *min_element(vec.begin(), vec.end());
}

template <typename T>
T getMaximum(const std::vector <T> &vec) {
    return *max_element(vec.begin(), vec.end());
}

template <typename T>
T getMedian(const std::vector <T> &vec) {
    std::vector <T> tmp_vec(vec);
    sort(tmp_vec.begin(), tmp_vec.end());
    if (tmp_vec.size() % 2 == 1) {
        return tmp_vec[tmp_vec.size() / 2];
    }
    return (tmp_vec[tmp_vec.size() / 2] + tmp_vec[tmp_vec.size() / 2 - 1]) / 2;
}

// ------------------------------------------------------------------------------------------------

#endif // LOGICS_UTILS_H
