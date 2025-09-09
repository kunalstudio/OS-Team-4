// This program demonstrates a classic deadlock scenario in the Dining Philosophers Problem.
// It uses threads, mutexes, and a specific order of fork acquisition
// that leads to a situation where all philosophers are blocked forever.

#include <iostream>
#include <vector>
#include <thread>
#include <mutex>

// Define the number of philosophers and forks
const int NUM_PHILOSOPHERS = 5;
const int NUM_FORKS = 5;

// An array of mutexes to represent the forks on the table.
// Each mutex protects a single shared resource (a fork).
std::vector<std::mutex> forks(NUM_FORKS);

// The function that each philosopher thread will execute.
void philosopher(int id) {
    // Determine the left and right fork indices for this philosopher.
    // The modulo operator (%) ensures a circular arrangement.
    int left_fork = id;
    int right_fork = (id + 1) % NUM_FORKS;

    while (true) {
        // Philosopher is thinking
        std::cout << "Philosopher " << id << " is thinking." << std::endl;
        std::this_thread::sleep_for(std::chrono::milliseconds(100));

        // The Deadlock Scenario:
        // Each philosopher tries to pick up their LEFT fork first,
        // then their RIGHT fork. This creates a circular wait.

        // Try to pick up the left fork.
        // The .lock() method will block until the mutex is available.
        std::cout << "Philosopher " << id << " is hungry and trying to pick up fork " << left_fork << "." << std::endl;
        forks[left_fork].lock();

        std::cout << "Philosopher " << id << " picked up fork " << left_fork << "." << std::endl;

        // Simulate a slight delay to ensure other threads can pick up their
        // first fork, making the deadlock more likely to occur.
        std::this_thread::sleep_for(std::chrono::milliseconds(100));

        // Try to pick up the right fork.
        std::cout << "Philosopher " << id << " is trying to pick up fork " << right_fork << "." << std::endl;

        // This is the point of deadlock. All philosophers will try to lock
        // their right fork, which is the left fork of their neighbor.
        // Since all left forks are already locked, no one can proceed.
        forks[right_fork].lock();

        std::cout << "Philosopher " << id << " picked up fork " << right_fork << " and is now eating." << std::endl;
        std::this_thread::sleep_for(std::chrono::milliseconds(500));

        // Eating is finished, put down the forks
        std::cout << "Philosopher " << id << " finished eating and put down forks " << left_fork << " and " << right_fork << "." << std::endl;
        forks[right_fork].unlock();
        forks[left_fork].unlock();
    }
}

int main() {
    // Create an array of threads for each philosopher
    std::vector<std::thread> philosophers;

    // Start a thread for each philosopher
    for (int i = 0; i < NUM_PHILOSOPHERS; ++i) {
        philosophers.emplace_back(philosopher, i);
    }

    // Wait for all threads to finish (they won't in this case, due to deadlock)
    for (int i = 0; i < NUM_PHILOSOPHERS; ++i) {
        philosophers[i].join();
    }

    return 0;
}
