#include <atomic>
#include <chrono>
#include <cmath>
#include <cstdio>
#include <cwchar>
#include <iostream>
#include <random>
#include <sstream>
#include <thread>
#include <vector>

#include "errors.hpp"
#define todo() assert(false && "todo")
#define loop while (1)

using namespace std::chrono_literals;

int main(int argc, char const* argv[]) {
  std::locale::global(std::locale("en_US.utf8"));
  std::wcout.imbue(std::locale());
  std::wprintf(L"\n");
  ARGC = argc;
  ARGV = argv;
  if (argc != 3) {
    usage();
  }
  auto const radius = parse_or_error<double>(argv[1]);
  auto const radiussq = radius * radius;
  auto const total = parse_or_error<size_t>(argv[2]);
  auto const NUM_THREADS = 9;
  std::atomic<ssize_t> remaining = total;
  std::atomic<size_t> inside = 0;

  std::vector<std::thread> threads{};
  auto const before = std::chrono::high_resolution_clock::now();
  std::random_device rd;
  for (size_t i = 0; i < NUM_THREADS; ++i) {
    auto const seed = rd();
    threads.emplace_back([seed, radius, radiussq, &remaining, &inside]() {
      auto const BATCH = 10000l;
      std::mt19937 mt{seed};
      std::uniform_real_distribution<double> urd{-radius, radius};
      loop {
        ssize_t local_remaining = remaining.load(std::memory_order_seq_cst);
        if (local_remaining == 0) {
          return;
        }
        if (!remaining.compare_exchange_strong(
                local_remaining, std::max(0l, local_remaining - BATCH),
                std::memory_order_seq_cst, std::memory_order_relaxed)) {
          continue;
        }
        auto const batch_size = std::min(BATCH, local_remaining);
        size_t batch_inside = 0;
        for (ssize_t i = 0; i < batch_size; ++i) {
          double const magsq = pow(urd(mt), 2) + pow(urd(mt), 2);
          batch_inside += magsq <= radiussq;
        }
        inside.fetch_add(batch_inside, std::memory_order_relaxed);
      }
    });
  }
  loop {
    auto const local_remaining = remaining.load(std::memory_order_relaxed);
    auto const done = (total - local_remaining) * 100 / total;
    auto const divisor = 100. / 45.;
    auto const done_3 = done / divisor;
    std::wstring progress;
    progress.insert(0, (45 - done_3), L' ');
    progress.insert(0, done_3, L'\u2588');
    std::wstring control = L"\u001b[33m";
    if (done == 100) {
      control = L"\u001b[32m";
    }
    std::wprintf(L"\r      [ %ls%ls\u001b[0m ] %3lu%%  ", control.c_str(),
                 progress.c_str(), done);
    std::wcout.flush();
    if (local_remaining == 0) {
      break;
    }
    std::this_thread::sleep_for(100ms);
  }
  for (auto& thread : threads) thread.join();
  auto const after = std::chrono::high_resolution_clock::now();

  auto const final_inside = (double)inside.load(std::memory_order_relaxed);
  auto const final_ratio = final_inside / (double)total;
  auto const final_area = final_ratio * radiussq * 4;
  auto const expected_area = M_PI * radiussq;
  auto const absolute_error = std::abs(expected_area - final_area);
  auto const relative_error = absolute_error / expected_area * 100;
  auto const time =
      std::chrono::duration_cast<std::chrono::milliseconds>(after - before)
          .count();

  std::wprintf(
      LR"(

      Amount of points ended up inside circle: %13lu
      Total amount of points:                  %13lu
      Elapsed time:                            %13lu ms
      Ratio:                                   %13.7lf
      Calculated area:                         %13.7lf
      Expected area:                           %13.7lf
      Absolute error:                          %13.7lf
      Relative error:                          %13.7lf %%

)",
      (size_t)final_inside, total, time, final_ratio, final_area, expected_area,
      absolute_error, relative_error);
  return 0;
}
