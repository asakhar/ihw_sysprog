#include <iomanip>
#include <iostream>
#include <sstream>

#include "cockroach.hpp"
#include "helper.hpp"

int main() {
  std::string const names[5] = {"Jim", "Sally", "Fluffy", "George",
                                "Lucky One"};
  size_t constexpr SIZE = sizeof(names) / sizeof(names[0]);

  std::cout << "In todays race paticipate 5 cockroaches: \n";
  for (size_t i = 0; auto& name : names) {
    std::cout << "[" << ++i << "] " << name << "\n";
  }
  std::cout << "\nChoose one of cockroaches that you bet for: ";
  long choosen = SIZE;
  do {
    std::string input;
    std::getline(std::cin, input);
    std::stringstream ss;
    ss << input;
    ss >> choosen;
    if (!ss.eof()) {
      auto iter = std::find(begin(names), end(names), input);
      if (iter != end(names)) {
        choosen = iter - begin(names);
        break;
      }
    }
    if (choosen > 0 && choosen <= static_cast<long>(SIZE)) {
      break;
    }
    std::cerr << "Invalid option provided: " << input << "\nTry again: ";
  } while (1);

  std::condition_variable startWait;
  std::condition_variable finishWait;
  std::atomic<Cockroach*> winner = nullptr;
  bool started = false;
  const double distance = 2;
  std::vector<std::unique_ptr<Cockroach>> roaches;
  for (size_t i = 0; i < 5; ++i) {
    roaches.emplace_back(std::make_unique<Cockroach>(
        names[i], finishWait, distance, startWait, winner, started));
  }
  started = true;
  startWait.notify_all();

  std::mutex finished;
  std::unique_lock<std::mutex> lock{finished};
  finishWait.wait(lock, [&winner]() { return winner.load() != nullptr; });

  auto won = winner.load();
  std::cout << "\nDistances runned by cockroaches:\n";

  size_t const maxNameSize = max(foreach (
      begin(names), end(names), [](auto name) { return name.size(); }));

  for (auto& roach : roaches) {
    std::cout << roach->getName() << ":    ";
    for (size_t i = roach->getName().size(); i < maxNameSize; ++i) {
      std::cout << ' ';
    }
    std::cout << std::setprecision(5) << std::fixed << roach->getDist()
              << "mm\n";
  }
  std::cout << "\nCockroach named " << won->getName() << " won this race!\n\n";

  if (std::find(begin(names), end(names), won->getName()) - begin(names) ==
      choosen - 1) {
    std::cout << "You won the bid. Congrats!\n";
  } else {
    std::cout << "Nah... You lost all your money.\n";
  }

  return 0;
}
