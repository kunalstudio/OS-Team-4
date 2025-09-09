// This program simulates a deadlock scenario for the Dining Philosophers Problem
// in a single-threaded environment. It demonstrates how the state of the system
// becomes locked in a circular wait, preventing any further progress.

#include <iostream>
#include <vector>
#include <string>

const int NUM_PHILOSOPHERS = 5;

// Define states for philosophers and forks
enum class PhilosopherState {
    THINKING,
    HUNGRY,
    HOLDING_LEFT_FORK,
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
        philosophers[i] = {i, PhilosopherState::HUNGRY}; // All start hungry to induce deadlock
    }

    // Initialize forks and their states
    std::vector<ForkState> forks(NUM_PHILOSOPHERS, ForkState::FREE);

    int turn = 0;
    bool system_deadlocked = false;

    // Simulation loop
    while (!system_deadlocked) {
        std::cout << "\n--- Turn " << turn << " ---" << std::endl;
        system_deadlocked = true; // Assume deadlock unless a state change occurs

        for (int i = 0; i < NUM_PHILOSOPHERS; ++i) {
            int left_fork = i;
            int right_fork = (i + 1) % NUM_PHILOSOPHERS;

            switch (philosophers[i].state) {
                case PhilosopherState::THINKING:
                    philosophers[i].state = PhilosopherState::HUNGRY;
                    system_deadlocked = false;
                    break;

                case PhilosopherState::HUNGRY:
                    // Check if the left fork is free to pick up
                    if (forks[left_fork] == ForkState::FREE) {
                        forks[left_fork] = ForkState::HELD;
                        philosophers[i].state = PhilosopherState::HOLDING_LEFT_FORK;
                        std::cout << "Philosopher " << i << " picked up fork " << left_fork << "." << std::endl;
                        system_deadlocked = false;
                    }
                    break;

                case PhilosopherState::HOLDING_LEFT_FORK:
                    // This is the deadlock condition. Check if the right fork is free.
                    if (forks[right_fork] == ForkState::FREE) {
                        forks[right_fork] = ForkState::HELD;
                        philosophers[i].state = PhilosopherState::EATING;
                        std::cout << "Philosopher " << i << " picked up fork " << right_fork << " and is now eating." << std::endl;
                        system_deadlocked = false;
                    } else {
                        std::cout << "Philosopher " << i << " is holding fork " << left_fork << " and waiting for fork " << right_fork << "." << std::endl;
                    }
                    break;

                case PhilosopherState::EATING:
                    std::cout << "Philosopher " << i << " finished eating." << std::endl;
                    forks[left_fork] = ForkState::FREE;
                    forks[right_fork] = ForkState::FREE;
                    philosophers[i].state = PhilosopherState::THINKING;
                    system_deadlocked = false;
                    break;
            }
        }

        turn++;
    }

    std::cout << "\n--- Simulation ended in Deadlock at Turn " << turn - 1 << " ---" << std::endl;
    for (int i = 0; i < NUM_PHILOSOPHERS; ++i) {
        std::cout << "Philosopher " << i << " is deadlocked, holding fork " << i << " and waiting for fork " << (i + 1) % NUM_PHILOSOPHERS << "." << std::endl;
    }
}

int main() {
    run_simulation();
    return 0;
}
