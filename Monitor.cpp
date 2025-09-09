// monitor_output.cpp
// Dining Philosophers with Monitor (Condition Variables)
// Compile: g++ -std=c++17 monitor_output.cpp -pthread -o monitor_output

#include <iostream>
#include <thread>
#include <vector>
#include <mutex>
#include <condition_variable>
#include <chrono>
using namespace std;

const int N = 5;
const int EAT_COUNT = 1;   // each philosopher eats once for clarity
enum State { THINKING, HUNGRY, EATING };

class Monitor {
    mutex m;
    condition_variable self[N];
    State state[N];

    int left(int i) { return (i + N - 1) % N; }
    int right(int i) { return (i + 1) % N; }

    void test(int i) {
        if (state[i] == HUNGRY &&
            state[left(i)] != EATING &&
            state[right(i)] != EATING) {
            state[i] = EATING;
            self[i].notify_one();
        }
    }

public:
    Monitor() { for (int i = 0; i < N; i++) state[i] = THINKING; }

    void pickup(int i) {
        unique_lock<mutex> lk(m);
        state[i] = HUNGRY;
        test(i);
        while (state[i] != EATING)
            self[i].wait(lk);
        cout << "Philosopher " << i << " picked up left fork " << i << ".\n";
        cout << "Philosopher " << i << " picked up right fork " << (i+1)%N << ".\n";
    }

    void putdown(int i) {
        unique_lock<mutex> lk(m);
        state[i] = THINKING;
        cout << "Philosopher " << i << " put down right fork " << (i+1)%N << ".\n";
        cout << "Philosopher " << i << " put down left fork " << i << ".\n";
        cout << "Philosopher " << i << " is full and has finished eating.\n";
        test(left(i));
        test(right(i));
    }
};

void philosopher(Monitor &mon, int id) {
    mon.pickup(id);
    cout << "Philosopher " << id << " is eating.\n";
    this_thread::sleep_for(chrono::milliseconds(200));
    mon.putdown(id);
}

int main() {
    Monitor mon;
    vector<thread> th;
    for (int i = 0; i < N; i++)
        th.emplace_back(philosopher, ref(mon), i);

    for (auto &t : th) t.join();

    cout << "All philosophers are full and the program has completed.\n";
    return 0;
}
