#ifndef DATA_SOURCE_LAB4_ERRORS__HPP
#define DATA_SOURCE_LAB4_ERRORS__HPP
#include <iostream>
#include <cstring>
#include <cassert>
#include <sstream>


void printError(char const *errortext, int errorcode);

char const *strend(char const *str);

extern int ARGC;
extern char const** ARGV;

void usage();

template <typename T>
T parse_or_error(char const* string) {
  T result;
  std::stringstream ss;
  ss << string;
  ss >> result;
  if (!ss.eof()) {
    printError("Invalid argument value", 0);
    usage();
  }
  return result;
}

#endif // DATA_SOURCE_LAB4_ERRORS__HPP