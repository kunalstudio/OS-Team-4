// Dining Philosophers - Waiter (Arbitrator) Solution
// Improved readable output in single-threaded simulation

#include <iostream>
#include <vector>
#include <string>

const int NUM_PHILOSOPHERS = 5;

// States
enum class PhilosopherState { THINKING, HUNGRY, EATING };
enum class ForkState { FREE, HELD };

// Philosopher struct
struct Philosopher {
    int id;
    PhilosopherState state;
};

// Helpers
std::string state_to_string(PhilosopherState st) {
    switch (st) {
        case PhilosopherState::THINKING: return "THINKING";
        case PhilosopherState::HUNGRY:   return "HUNGRY";
        case PhilosopherState::EATING:   return "EATING";
    }
    return "";
}

void print_forks(const std::vector<ForkState>& forks) {
    std::cout << "  Forks: ";
    for (int i = 0; i < forks.size(); i++) {
        std::cout << i << "("
                  << (forks[i] == ForkState::FREE ? "FREE" : "HELD")
                  << ") ";
    }
    std::cout << "\n";
}

// Simulation
void run_simulation() {
    std::vector<Philosopher> philosophers(NUM_PHILOSOPHERS);
    for (int i = 0; i < NUM_PHILOSOPHERS; i++) {
        philosophers[i] = {i, PhilosopherState::THINKING};
    }

    std::vector<ForkState> forks(NUM_PHILOSOPHERS, ForkState::FREE);

    int turn = 0;
    bool all_are_thinking = false;

    while (!all_are_thinking) {
        std::cout << "\n=== Turn " << turn << " ===\n";
        all_are_thinking = true;

        for (int i = 0; i < NUM_PHILOSOPHERS; i++) {
            int left = i;
            int right = (i + 1) % NUM_PHILOSOPHERS;

            switch (philosophers[i].state) {
                case PhilosopherState::THINKING:
                    philosophers[i].state = PhilosopherState::HUNGRY;
                    std::cout << "[Philosopher " << i << "] was THINKING -> now HUNGRY.\n";
                    all_are_thinking = false;
                    break;

                case PhilosopherState::HUNGRY:
                    // Waiter ensures safety: only lets philosopher eat if both forks free
                    if (forks[left] == ForkState::FREE && forks[right] == ForkState::FREE) {
                        forks[left] = ForkState::HELD;
                        forks[right] = ForkState::HELD;
                        philosophers[i].state = PhilosopherState::EATING;
                        std::cout << "[Philosopher " << i << "] got permission, picked up forks "
                                  << left << " & " << right << " -> now EATING.\n";
                        all_are_thinking = false;
                    } else {
                        std::cout << "[Philosopher " << i << "] is HUNGRY, waiting for forks "
                                  << left << " & " << right << ".\n";
                    }
                    break;

                case PhilosopherState::EATING:
                    std::cout << "[Philosopher " << i << "] finished EATING -> back to THINKING.\n";
                    forks[left] = ForkState::FREE;
                    forks[right] = ForkState::FREE;
                    philosophers[i].state = PhilosopherState::THINKING;
                    all_are_thinking = false;
                    break;
            }
        }

        // Show fork ownership at end of turn
        print_forks(forks);

        turn++;
        if (turn > 20) {
            std::cout << "\n=== Simulation ended after 20 turns. ===\n";
            return;
        }
    }

    std::cout << "\n=== Simulation ended as all philosophers are THINKING. ===\n";
}

int main() {
    run_simulation();
    return 0;
}
