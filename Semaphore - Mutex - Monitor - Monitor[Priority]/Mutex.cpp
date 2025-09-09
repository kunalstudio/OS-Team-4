#include <iostream>
#include <thread>
#include <mutex>
#include <vector>
#include <chrono>
using namespace std;

const int N = 5;   // number of philosophers
mutex forks[N];    // one mutex per fork
mutex cout_mtx;    // extra mutex to protect console output

void philosopher(int id) {
    int left = id;
    int right = (id + 1) % N;

    // Deadlock prevention: last philosopher picks right fork first
    if (id == N - 1) {
        forks[right].lock();
        {
            lock_guard<mutex> lock(cout_mtx);
            cout << "Philosopher " << id << " picked up right fork " << right << ".\n";
        }
        forks[left].lock();
        {
            lock_guard<mutex> lock(cout_mtx);
            cout << "Philosopher " << id << " picked up left fork " << left << ".\n";
        }
    } else {
        forks[left].lock();
        {
            lock_guard<mutex> lock(cout_mtx);
            cout << "Philosopher " << id << " picked up left fork " << left << ".\n";
        }
        forks[right].lock();
        {
            lock_guard<mutex> lock(cout_mtx);
            cout << "Philosopher " << id << " picked up right fork " << right << ".\n";
        }
    }

    {
        lock_guard<mutex> lock(cout_mtx);
        cout << "Philosopher " << id << " is eating.\n";
    }
    this_thread::sleep_for(chrono::milliseconds(500));

    forks[left].unlock();
    {
        lock_guard<mutex> lock(cout_mtx);
        cout << "Philosopher " << id << " put down left fork " << left << ".\n";
    }
    forks[right].unlock();
    {
        lock_guard<mutex> lock(cout_mtx);
        cout << "Philosopher " << id << " put down right fork " << right << ".\n";
        cout << "Philosopher " << id << " is full and has finished eating.\n";
    }
}

int main() {
    vector<thread> th;
    for (int i = 0; i < N; i++)
        th.emplace_back(philosopher, i);

    for (auto &t : th) t.join();

    {
        lock_guard<mutex> lock(cout_mtx);
        cout << "All philosophers are full and the program has completed.\n";
    }
    return 0;
}
