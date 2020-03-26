#include "include/logics/logics_utils.h"

#include <ctime>

// std::string ------------------------------------------------------------------------------------

bool isNumber(const std::string &str) {
    if (!str.size()) {
        return false;
    }

    for (auto ch : str) {
        if (!isspace(ch) && !isdigit(ch) && ch != '.' && ch != '-') {
            return false;
        }
    }

    return true;
}

std::vector <std::string> splitStr(const std::string &str, const std::string &sep) {
    std::vector <std::string> arr;
    size_t prev = 0;
    size_t next;
    size_t delta = sep.length();
    while ((next = str.find(sep, prev)) != std::string::npos) {
        arr.push_back(str.substr(prev, next - prev));
        prev = next + delta;
    }
    arr.push_back(str.substr(prev));
    return arr;
}

// int --------------------------------------------------------------------------------------------

int generateRandInt(int lowest, int highest)
{
    if (highest == 0) {
        return 0;
    }
    srand(time(nullptr));
    return rand() % highest + lowest;
}

// ------------------------------------------------------------------------------------------------
