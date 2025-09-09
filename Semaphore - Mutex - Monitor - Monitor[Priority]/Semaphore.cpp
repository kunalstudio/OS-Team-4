// dining_semaphore_fixed.cpp
// Arbitrator (waiter) solution using semaphores (fixed version).
// - Finite eat cycles per philosopher (program terminates).
// - Uses vector<unique_ptr<Semaphore>> to avoid vector-of-nonmovable-objects issues.
//
// Compile (Linux/GCC):
//   g++ -std=c++17 dining_semaphore_fixed.cpp -pthread -O2 -o dining_semaphore_fixed
// Run:
//   ./dining_semaphore_fixed

#include <iostream>
#include <vector>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <chrono>
#include <random>
#include <memory>

class Semaphore {
private:
    std::mutex mtx;
    std::condition_variable cv;
    int count;

public:
    explicit Semaphore(int initial_count) : count(initial_count) {}
    Semaphore(const Semaphore&) = delete;
    Semaphore& operator=(const Semaphore&) = delete;
    // Allow move? we delete move to keep it simple (we will store pointers instead)
    Semaphore(Semaphore&&) = delete;
    Semaphore& operator=(Semaphore&&) = delete;

    void wait() {
        std::unique_lock<std::mutex> lock(mtx);
        cv.wait(lock, [this] { return count > 0; });
        --count;
    }
    void signal() {
        std::unique_lock<std::mutex> lock(mtx);
        ++count;
        cv.notify_one();
    }
};

constexpr int NUM_PHILOSOPHERS = 5;
constexpr int EAT_TIMES = 5;               // how many times each philosopher eats
std::vector<std::unique_ptr<Semaphore>> forks; // will hold unique_ptr<Semaphore>
Semaphore room(NUM_PHILOSOPHERS - 1);     // waiter semaphore

std::mutex cout_mtx;

void philosopher(int id) {
    std::mt19937 rng((unsigned)std::chrono::high_resolution_clock::now().time_since_epoch().count() + id);
    std::uniform_int_distribution<int> dist(80, 200);

    for (int iter = 0; iter < EAT_TIMES; ++iter) {
        { // thinking
            std::lock_guard<std::mutex> lg(cout_mtx);
            std::cout << "Philosopher " << id << " is thinking (round " << iter+1 << ").\n";
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(dist(rng)));

        // Request permission from waiter (arbitrator)
        room.wait();

        // pick up left fork
        forks[id]->wait();
        {
            std::lock_guard<std::mutex> lg(cout_mtx);
            std::cout << "Philosopher " << id << " picked up left fork " << id << ".\n";
        }

        // pick up right fork
        int right = (id + 1) % NUM_PHILOSOPHERS;
        forks[right]->wait();
        {
            std::lock_guard<std::mutex> lg(cout_mtx);
            std::cout << "Philosopher " << id << " picked up right fork " << right << ".\n";
            std::cout << "Philosopher " << id << " is eating (round " << iter+1 << ").\n";
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(dist(rng)));

        // put down right, then left
        forks[right]->signal();
        {
            std::lock_guard<std::mutex> lg(cout_mtx);
            std::cout << "Philosopher " << id << " put down right fork " << right << ".\n";
        }
        forks[id]->signal();
        {
            std::lock_guard<std::mutex> lg(cout_mtx);
            std::cout << "Philosopher " << id << " put down left fork " << id << ".\n";
            std::cout << "Philosopher " << id << " is full for round " << iter+1 << ".\n";
        }

        // leave room (signal waiter)
        room.signal();

        // short pause before next round
        std::this_thread::sleep_for(std::chrono::milliseconds(40 + (dist(rng) % 50)));
    }

    std::lock_guard<std::mutex> lg(cout_mtx);
    std::cout << "Philosopher " << id << " has finished all " << EAT_TIMES << " rounds.\n";
}

int main() {
    std::cout << "Dining Philosophers (Arbitrator/Semaphore)\n";
    std::cout << "Each philosopher will eat " << EAT_TIMES << " times.\n";

    // initialize forks as unique_ptr<Semaphore>
    forks.reserve(NUM_PHILOSOPHERS);
    for (int i = 0; i < NUM_PHILOSOPHERS; ++i) {
        forks.emplace_back(std::make_unique<Semaphore>(1));
    }

    // spawn philosopher threads
    std::vector<std::thread> threads;
    threads.reserve(NUM_PHILOSOPHERS);
    for (int i = 0; i < NUM_PHILOSOPHERS; ++i) {
        threads.emplace_back(philosopher, i);
    }

    // join
    for (auto &t : threads) t.join();

    std::cout << "All philosophers finished. Program exiting normally.\n";
    return 0;
}
