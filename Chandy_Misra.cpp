#include <iostream>
#include <vector>
#include <string>

constexpr int NUM_PHILOSOPHERS = 5;

// Define the state of a philosopher
enum class PhilosopherState {
    THINKING,
    HUNGRY,
    EATING
};

// Represents a fork, with ownership and cleanliness status
struct Fork {
    bool is_clean;
    int owner_id;
};

// Represents a philosopher in the simulation
struct Philosopher {
    int id;
    PhilosopherState state;
    bool has_requested_left;
    bool has_requested_right;
};

// Simulation function
void run_simulation() {
    std::vector<Philosopher> philosophers(NUM_PHILOSOPHERS);
    std::vector<Fork> forks(NUM_PHILOSOPHERS);

    // Initialize philosophers and forks
    for (int i = 0; i < NUM_PHILOSOPHERS; ++i) {
        philosophers[i] = {i, PhilosopherState::THINKING, false, false};
        // Simplified initialization: each fork initially belongs to the philosopher with the same id
        forks[i] = {true, i};
    }

    int turn = 0;

    while (true) {
        std::cout << "\n--- Turn " << turn << " ---" << std::endl;

        // Print fork owners (helps to debug/see progress)
        for (int f = 0; f < NUM_PHILOSOPHERS; ++f) {
            std::cout << "Fork " << f << " owner: " << forks[f].owner_id
                      << (forks[f].is_clean ? " (clean)" : " (dirty)") << std::endl;
        }

        // Allow passive transfers: if a fork owner is not eating and a neighbor requested it,
        // transfer the fork to the requester. This simple transfer rule helps the single-threaded
        // simulation make progress (it is a simulation, not a full distributed runtime).
        for (int k = 0; k < NUM_PHILOSOPHERS; ++k) {
            int owner = forks[k].owner_id;
            int left = k;
            int right = (k + 1) % NUM_PHILOSOPHERS;

            if (philosophers[owner].state != PhilosopherState::EATING) {
                if (owner != left && philosophers[left].has_requested_left) {
                    forks[k].owner_id = left;
                    philosophers[left].has_requested_left = false;
                    forks[k].is_clean = true; // passed fork becomes clean
                    std::cout << "Fork " << k << " passed to Philosopher " << left << " (requested)." << std::endl;
                } else if (owner != right && philosophers[right].has_requested_right) {
                    forks[k].owner_id = right;
                    philosophers[right].has_requested_right = false;
                    forks[k].is_clean = true;
                    std::cout << "Fork " << k << " passed to Philosopher " << right << " (requested)." << std::endl;
                }
            }
        }

        bool all_are_thinking = true;

        for (int i = 0; i < NUM_PHILOSOPHERS; ++i) {
            int left_fork_id = i;
            int right_fork_id = (i + 1) % NUM_PHILOSOPHERS;

            switch (philosophers[i].state) {
                case PhilosopherState::THINKING: {
                    // A thinking philosopher sometimes becomes hungry (staggered by index)
                    if (turn % (i + 2) == 0) {
                        philosophers[i].state = PhilosopherState::HUNGRY;
                        std::cout << "Philosopher " << i << " is now hungry." << std::endl;
                    }
                    break;
                }

                case PhilosopherState::HUNGRY: {
                    // Try to acquire forks from neighbors based on ownership
                    bool has_left = (forks[left_fork_id].owner_id == i);
                    bool has_right = (forks[right_fork_id].owner_id == i);

                    if (has_left && has_right) {
                        philosophers[i].state = PhilosopherState::EATING;
                        std::cout << "Philosopher " << i << " is eating." << std::endl;
                        forks[left_fork_id].is_clean = false; // forks become dirty after use
                        forks[right_fork_id].is_clean = false;
                    } else {
                        // Request forks they don't have
                        if (!has_left && !philosophers[i].has_requested_left) {
                            std::cout << "Philosopher " << i << " requesting fork " << left_fork_id
                                      << " from Philosopher " << forks[left_fork_id].owner_id << "." << std::endl;
                            philosophers[i].has_requested_left = true;
                        }
                        if (!has_right && !philosophers[i].has_requested_right) {
                            std::cout << "Philosopher " << i << " requesting fork " << right_fork_id
                                      << " from Philosopher " << forks[right_fork_id].owner_id << "." << std::endl;
                            philosophers[i].has_requested_right = true;
                        }
                    }
                    break;
                }

                case PhilosopherState::EATING: {
                    // A philosopher finishes eating and releases forks (goes back to thinking)
                    std::cout << "Philosopher " << i << " finished eating." << std::endl;
                    philosophers[i].state = PhilosopherState::THINKING;

                    // Pass forks to neighbors if they were requested while eating
                    int left_neighbor = (i + NUM_PHILOSOPHERS - 1) % NUM_PHILOSOPHERS;
                    int right_neighbor = (i + 1) % NUM_PHILOSOPHERS;

                    if (philosophers[left_neighbor].has_requested_right) {
                        forks[left_fork_id].owner_id = left_neighbor;
                        philosophers[left_neighbor].has_requested_right = false;
                        forks[left_fork_id].is_clean = true;
                        std::cout << "Philosopher " << i << " passed fork " << left_fork_id
                                  << " to Philosopher " << left_neighbor << "." << std::endl;
                    }
                    if (philosophers[right_neighbor].has_requested_left) {
                        forks[right_fork_id].owner_id = right_neighbor;
                        philosophers[right_neighbor].has_requested_left = false;
                        forks[right_fork_id].is_clean = true;
                        std::cout << "Philosopher " << i << " passed fork " << right_fork_id
                                  << " to Philosopher " << right_neighbor << "." << std::endl;
                    }

                    // Clear this philosopher's outstanding requests (they were served or are no longer relevant)
                    philosophers[i].has_requested_left = false;
                    philosophers[i].has_requested_right = false;
                    break;
                }
            }

            if (philosophers[i].state != PhilosopherState::THINKING) {
                all_are_thinking = false;
            }
        }

        turn++;

        // End simulation after a certain number of turns to prevent an infinite loop
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
