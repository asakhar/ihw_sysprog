#include <atomic>
#include <bitset>
#include <cassert>
#include <chrono>
#include <concepts>
#include <cstring>
#include <functional>
#include <iostream>
#include <limits>
#include <random>
#include <sstream>
#include <string>
#include <thread>
#include <vector>

using namespace std::chrono_literals;

#define green "\033[0;32m"
#define red "\033[0;31m"
#define reset "\033[0m"
#define yellow "\033[0;33m"

using string = std::vector<std::uint8_t>;

static int ARGC;
static char const** ARGV;
void usage();
template <typename T>
T input_checked(char const* string);
string gen_random_string(size_t const size, std::mt19937& mt);
bool check_hash(size_t hash, size_t Z);
template <std::integral T>
inline void hash_combine(std::size_t& seed, const T& v);

using std::chrono::duration_cast;

int main(int argc, char const* argv[]) {
  ARGC = argc;
  ARGV = argv;
  if (argc < 3) {
    std::cerr << red "Invalid number of arguments!\n" reset;
    usage();
  }
  auto const Z = input_checked<size_t>(argv[1]);
  auto const N = input_checked<size_t>(argv[2]);

  std::random_device rd;
  std::mt19937 mt{rd()};
  std::uniform_int_distribution<size_t> urd{20, 1000};
  string const target_string = gen_random_string(urd(mt), mt);
  std::vector<std::thread> threads;
  std::atomic_bool complete = false;
  std::uint64_t result;
  std::size_t result_hash;

  auto started_time = std::chrono::high_resolution_clock::now();
  size_t const hash_checkpoint = [&target_string] {
    size_t hash_calc = 0;
    for (auto item : target_string) {
      hash_combine(hash_calc, item);
    }
    return hash_calc;
  }();

  for (size_t i = 0; i < N; ++i) {
    auto seed = rd();
    threads.emplace_back(
        [hash_checkpoint, seed, Z, &result, &complete, &result_hash]() {
          thread_local static std::mt19937 mt{seed};
          std::uniform_int_distribution<uint64_t> uid{
              0, std::numeric_limits<uint64_t>::max()};

          size_t hash;
          std::uint64_t attempt;
          do {
            hash = hash_checkpoint;
            attempt = uid(mt);
            hash_combine(hash, attempt);
          } while (!check_hash(hash, Z) &&
                   !complete.load(std::memory_order_relaxed));
          if (complete.exchange(true, std::memory_order_seq_cst)) return;
          result = attempt;
          result_hash = hash;
        });
  }
  for (auto& th : threads) th.join();
  auto ended_time = std::chrono::high_resolution_clock::now();
  std::cout << green "Original string size: ` " yellow << target_string.size()
            << green " `\nOriginal string bytes: ` " yellow;
  for (auto item : target_string) std::cout << (uint16_t)item << " ";
  std::cout << green " `\nOriginal string: ` " yellow;
  for (auto item : target_string)
    if (isgraph(item)) std::cout << item; else std::cout << ".";
  std::cout << green " `\nResult suffix: ` " yellow;
  union {
    uint8_t bytes[8];
    uint64_t big;
  } res;
  res.big = result;
  for (auto item : res.bytes) std::cout << (uint16_t)item << " ";
  std::cout << green "`\nResult hash: ` " yellow << std::bitset<64>(result_hash)
            << green " `\nTime spent: ` " yellow
            << duration_cast<std::chrono::milliseconds>(ended_time -
                                                        started_time)
                   .count()
            << "ms" green " `\n";

  return 0;
}

bool check_hash(size_t hash, size_t Z) {
  const size_t mask = ((1ul << Z) - 1ul) << (sizeof(size_t) * 8 - Z);
  return (hash & mask) == 0;
}

template <std::integral T>
inline void hash_combine(std::size_t& seed, const T& v) {
  thread_local static std::hash<T> hasher;
  seed ^= hasher(v) + 0x9e3779b9 + (seed << 6ul) + (seed >> 2ul);
}

string gen_random_string(size_t const size, std::mt19937& mt) {
  static std::uniform_int_distribution<uint8_t> distr{0, 255};
  string ss;
  ss.reserve(size);
  for (size_t i = 0; i < size; ++i) {
    ss.emplace_back(distr(mt));
  }
  return ss;
}

template <typename T>
T input_checked(char const* string) {
  T result;
  std::stringstream ss;
  ss << string;
  ss >> result;
  if (!ss.eof()) {
    std::cerr << red "Invalid argument value\n" reset;
    usage();
  }
  return result;
}

void usage() {
  assert(ARGC > 0);
  std::cerr << "Usage:\n\t " << ARGV[0]
            << " <target number of zeros> <number of threads>\n";
  exit(1);
}
