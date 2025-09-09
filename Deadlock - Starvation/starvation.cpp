// This program demonstrates a scenario that can lead to starvation in the Dining Philosophers Problem.
// Starvation occurs when a philosopher is repeatedly denied access to forks while others eat.
// This is different from deadlock, where no one can proceed.

#include <iostream>
#include <vector>
#include <thread>
#include <mutex>

// Define the number of philosophers and forks
const int NUM_PHILOSOPHERS = 5;
const int NUM_FORKS = 5;

// An array of mutexes to represent the forks on the table.
std::vector<std::mutex> forks(NUM_FORKS);

// The function that each philosopher thread will execute.
void philosopher(int id) {
    int left_fork = id;
    int right_fork = (id + 1) % NUM_FORKS;

    while (true) {
        // Philosopher is thinking
        std::cout << "Philosopher " << id << " is thinking." << std::endl;
        std::this_thread::sleep_for(std::chrono::milliseconds(100));

        // The Starvation Scenario:
        // Instead of blocking, a philosopher uses try_lock() to check for forks.
        // If they can't get BOTH, they immediately release the one they have and retry.
        // This can lead to a "livelock" where they are always busy-waiting.

        std::cout << "Philosopher " << id << " is hungry." << std::endl;

        while (true) {
            // Try to acquire the left fork first.
            forks[left_fork].lock();

            // Try to acquire the right fork non-blockingly.
            // This is where starvation can occur. If the right fork is always busy,
            // the philosopher will immediately release the left one and retry.
            if (forks[right_fork].try_lock()) {
                // Success! Both forks acquired.
                std::cout << "Philosopher " << id << " is eating." << std::endl;
                std::this_thread::sleep_for(std::chrono::milliseconds(200));

                // Finished eating, release both forks.
                std::cout << "Philosopher " << id << " put down forks " << left_fork << " and " << right_fork << "." << std::endl;
                forks[right_fork].unlock();
                forks[left_fork].unlock();
                break; // Exit the inner loop and go back to thinking.
            } else {
                // Failed to acquire the right fork. Release the left one immediately.
                std::cout << "Philosopher " << id << " couldn't get fork " << right_fork << ". Releasing fork " << left_fork << " and trying again." << std::endl;
                forks[left_fork].unlock();
                std::this_thread::sleep_for(std::chrono::milliseconds(50));
            }
        }
    }
}

int main() {
    // Create an array of threads for each philosopher
    std::vector<std::thread> philosophers;

    // Start a thread for each philosopher
    for (int i = 0; i < NUM_PHILOSOPHERS; ++i) {
        philosophers.emplace_back(philosopher, i);
    }

    // Wait for all threads to finish (they won't in this case, due to infinite loop)
    for (int i = 0; i < NUM_PHILOSOPHERS; ++i) {
        philosophers[i].join();
    }

    return 0;
}
