#include <iostream>
#include <vector>

constexpr int NUM_PHILOSOPHERS = 5;

// Define states for philosophers and forks
enum class PhilosopherState {
    THINKING,
    HUNGRY,
    HOLDING_FIRST_FORK,
    EATING
};

enum class ForkState {
    FREE,
    HELD
};

// Represents a philosopher in the simulation
struct Philosopher {
    int id;
    PhilosopherState state;
};

// Main simulation function
void run_simulation() {
    // Initialize philosophers and their states
    std::vector<Philosopher> philosophers(NUM_PHILOSOPHERS);
    for (int i = 0; i < NUM_PHILOSOPHERS; ++i) {
        philosophers[i] = {i, PhilosopherState::HUNGRY};
    }

    // Initialize forks and their states
    std::vector<ForkState> forks(NUM_PHILOSOPHERS, ForkState::FREE);

    int turn = 0;

    // Simulation loop
    while (true) {
        std::cout << "\n--- Turn " << turn << " ---" << std::endl;
        bool all_are_thinking = true;

        for (int i = 0; i < NUM_PHILOSOPHERS; ++i) {
            int left_fork = i;
            int right_fork = (i + 1) % NUM_PHILOSOPHERS;

            switch (philosophers[i].state) {
                case PhilosopherState::THINKING: {
                    // A thinking philosopher sometimes becomes hungry again
                    if (turn % (i + 2) == 0) {
                        philosophers[i].state = PhilosopherState::HUNGRY;
                        std::cout << "Philosopher " << i << " is now hungry." << std::endl;
                        all_are_thinking = false;
                    }
                    break;
                }

                case PhilosopherState::HUNGRY: {
                    // Asymmetric rule: Odd-numbered philosophers pick up the left fork first.
                    // Even-numbered philosophers pick up the right fork first.
                    if (i % 2 != 0) { // Odd philosopher
                        if (forks[left_fork] == ForkState::FREE) {
                            forks[left_fork] = ForkState::HELD;
                            philosophers[i].state = PhilosopherState::HOLDING_FIRST_FORK;
                            std::cout << "Philosopher " << i << " (Odd) picked up left fork " << left_fork << "." << std::endl;
                        }
                    } else { // Even philosopher
                        if (forks[right_fork] == ForkState::FREE) {
                            forks[right_fork] = ForkState::HELD;
                            philosophers[i].state = PhilosopherState::HOLDING_FIRST_FORK;
                            std::cout << "Philosopher " << i << " (Even) picked up right fork " << right_fork << "." << std::endl;
                        }
                    }
                    all_are_thinking = false;
                    break;
                }

                case PhilosopherState::HOLDING_FIRST_FORK: {
                    // Now, try to pick up the second fork
                    if (i % 2 != 0) { // Odd philosopher
                        if (forks[right_fork] == ForkState::FREE) {
                            forks[right_fork] = ForkState::HELD;
                            philosophers[i].state = PhilosopherState::EATING;
                            std::cout << "Philosopher " << i << " picked up right fork " << right_fork
                                      << " and is now eating." << std::endl;
                        }
                    } else { // Even philosopher
                        if (forks[left_fork] == ForkState::FREE) {
                            forks[left_fork] = ForkState::HELD;
                            philosophers[i].state = PhilosopherState::EATING;
                            std::cout << "Philosopher " << i << " picked up left fork " << left_fork
                                      << " and is now eating." << std::endl;
                        }
                    }
                    all_are_thinking = false;
                    break;
                }

                case PhilosopherState::EATING: {
                    std::cout << "Philosopher " << i << " finished eating." << std::endl;
                    forks[left_fork] = ForkState::FREE;
                    forks[right_fork] = ForkState::FREE;
                    philosophers[i].state = PhilosopherState::THINKING;
                    all_are_thinking = false;
                    break;
                }
            }
        }

        turn++;

        // End simulation after a certain number of turns to prevent infinite loop
        if (turn > 50) {
            std::cout << "\n--- Simulation ended after 50 turns. ---" << std::endl;
            break;
        }

        if (all_are_thinking) {
            std::cout << "\n--- Simulation ended because all philosophers are thinking. ---" << std::endl;
            break;
        }
    }
}

int main() {
    run_simulation();
    return 0;
}
