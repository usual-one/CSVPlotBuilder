#ifndef LOGICS_UTILS_H
#define LOGICS_UTILS_H

#include <string>
#include <vector>

// std::string ------------------------------------------------------------------------------------

bool isNumber(const std::string &str);

std::vector <std::string> splitStr(const std::string &str, const std::string &sep);

// int --------------------------------------------------------------------------------------------

int generateRandInt(int lowest, int highest);

// std::vector ------------------------------------------------------------------------------------

template <typename T>
int getIndex(const std::vector <T> &vect, T elem) {
    auto it = find(vect.begin(), vect.end(), elem);
    if (it == vect.end()) {
        return -1;
    }
    return std::distance(vect.begin(), it);
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

#endif // LOGICS_UTILS_H
