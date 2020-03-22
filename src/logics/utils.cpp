#include "include/logics/utils.h"

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
