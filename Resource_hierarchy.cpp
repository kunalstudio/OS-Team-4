// Dining Philosophers - Resource Hierarchy (Ordered Forks) Solution
// Improved readable output

#include <iostream>
#include <vector>
#include <string>
#include <algorithm> // For std::min and std::max

const int NUM_PHILOSOPHERS = 5;

// Define states for philosophers and forks
enum class PhilosopherState { THINKING, HUNGRY, EATING };
enum class ForkState { FREE, HELD };

// Represents a philosopher in the simulation
struct Philosopher {
    int id;
    PhilosopherState state;
    int forks_held_count; // To track how many forks a philosopher has
};

// Helper to stringify philosopher state
std::string state_to_string(PhilosopherState st) {
    switch (st) {
        case PhilosopherState::THINKING: return "THINKING";
        case PhilosopherState::HUNGRY:   return "HUNGRY";
        case PhilosopherState::EATING:   return "EATING";
    }
    return "";
}

// Helper to print forks ownership
void print_forks(const std::vector<ForkState>& forks) {
    std::cout << "  Forks: ";
    for (int i = 0; i < forks.size(); i++) {
        std::cout << i << "("
                  << (forks[i] == ForkState::FREE ? "FREE" : "HELD")
                  << ") ";
    }
    std::cout << "\n";
}

// Main simulation function
void run_simulation() {
    // Initialize philosophers
    std::vector<Philosopher> philosophers(NUM_PHILOSOPHERS);
    for (int i = 0; i < NUM_PHILOSOPHERS; ++i) {
        philosophers[i] = {i, PhilosopherState::THINKING, 0};
    }

    // Initialize forks
    std::vector<ForkState> forks(NUM_PHILOSOPHERS, ForkState::FREE);

    int turn = 0;
    bool all_are_thinking = false;

    // Simulation loop
    while (!all_are_thinking) {
        std::cout << "\n=== Turn " << turn << " ===\n";
        all_are_thinking = true;

        for (int i = 0; i < NUM_PHILOSOPHERS; ++i) {
            // Fork indices (ordered to avoid deadlock)
            int fork1_idx = std::min(i, (i + 1) % NUM_PHILOSOPHERS);
            int fork2_idx = std::max(i, (i + 1) % NUM_PHILOSOPHERS);

            switch (philosophers[i].state) {
                case PhilosopherState::THINKING:
                    philosophers[i].state = PhilosopherState::HUNGRY;
                    std::cout << "[Philosopher " << i << "] was THINKING -> now HUNGRY.\n";
                    all_are_thinking = false;
                    break;

                case PhilosopherState::HUNGRY:
                    if (forks[fork1_idx] == ForkState::FREE && forks[fork2_idx] == ForkState::FREE) {
                        forks[fork1_idx] = ForkState::HELD;
                        forks[fork2_idx] = ForkState::HELD;
                        philosophers[i].state = PhilosopherState::EATING;
                        philosophers[i].forks_held_count = 2;
                        std::cout << "[Philosopher " << i << "] picked up forks "
                                  << fork1_idx << " & " << fork2_idx
                                  << " -> now EATING.\n";
                        all_are_thinking = false;
                    } else {
                        std::cout << "[Philosopher " << i << "] is HUNGRY, waiting for forks "
                                  << fork1_idx << " & " << fork2_idx << ".\n";
                    }
                    break;

                case PhilosopherState::EATING:
                    std::cout << "[Philosopher " << i << "] finished EATING -> back to THINKING.\n";
                    forks[fork1_idx] = ForkState::FREE;
                    forks[fork2_idx] = ForkState::FREE;
                    philosophers[i].forks_held_count = 0;
                    philosophers[i].state = PhilosopherState::THINKING;
                    all_are_thinking = false;
                    break;
            }
        }

        // Show fork ownership at end of turn
        print_forks(forks);

        turn++;

        // Stop after 20 turns (safety cutoff)
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
