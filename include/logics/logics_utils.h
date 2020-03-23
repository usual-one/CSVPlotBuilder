#ifndef LOGICS_UTILS_H
#define LOGICS_UTILS_H

#include <string>
#include <vector>

// std::string ------------------------------------------------------------------------------------

bool isNumber(const std::string &str);

std::vector <std::string> splitStr(const std::string &str, const std::string &sep);

// int --------------------------------------------------------------------------------------------

int generateRandInt(int lowest, int highest);

#endif // LOGICS_UTILS_H
