#include "errors.hpp"


int ARGC;
char const** ARGV;

void printError(char const *errortext, int errorcode) {
  std::cerr << "Error: " << errortext;
  if (errorcode != 0)
    std::cerr << ": " << strerror(errorcode);
  std::cerr << "\n";
}
char const *strend(char const *str) { return str + strlen(str); }
void usage() {
  assert(ARGC > 0);
  std::cerr << "Usage:\n\t " << ARGV[0]
            << " <radius of circle> <number of dots>\n";
  exit(1);
}
