// monitor_priority_output.cpp
// Dining Philosophers with Monitor + FIFO fairness
// Compile: g++ -std=c++17 monitor_priority_output.cpp -pthread -o monitor_priority_output

#include <iostream>
#include <thread>
#include <vector>
#include <mutex>
#include <condition_variable>
#include <deque>
#include <chrono>
using namespace std;

const int N = 5;
const int EAT_COUNT = 1;
enum State { THINKING, HUNGRY, EATING };

class PriorityMonitor {
    mutex m;
    condition_variable cond[N];
    State state[N];
    deque<int> waitQ;

    int left(int i) { return (i + N - 1) % N; }
    int right(int i) { return (i + 1) % N; }

    bool canEat(int i) {
        return state[i] == HUNGRY &&
               state[left(i)] != EATING &&
               state[right(i)] != EATING;
    }

public:
    PriorityMonitor() { for (int i = 0; i < N; i++) state[i] = THINKING; }

    void pickup(int i) {
        unique_lock<mutex> lk(m);
        state[i] = HUNGRY;
        waitQ.push_back(i);
        while (!(waitQ.front() == i && canEat(i)))
            cond[i].wait(lk);
        waitQ.pop_front();
        state[i] = EATING;
        cout << "Philosopher " << i << " picked up left fork " << i << ".\n";
        cout << "Philosopher " << i << " picked up right fork " << (i+1)%N << ".\n";
    }

    void putdown(int i) {
        unique_lock<mutex> lk(m);
        state[i] = THINKING;
        cout << "Philosopher " << i << " put down right fork " << (i+1)%N << ".\n";
        cout << "Philosopher " << i << " put down left fork " << i << ".\n";
        cout << "Philosopher " << i << " is full and has finished eating.\n";
        for (int pid : waitQ) {
            if (canEat(pid)) {
                cond[pid].notify_one();
                break;
            }
        }
    }
};

void philosopher(PriorityMonitor &mon, int id) {
    mon.pickup(id);
    cout << "Philosopher " << id << " is eating.\n";
    this_thread::sleep_for(chrono::milliseconds(200));
    mon.putdown(id);
}

int main() {
    PriorityMonitor mon;
    vector<thread> th;
    for (int i = 0; i < N; i++)
        th.emplace_back(philosopher, ref(mon), i);

    for (auto &t : th) t.join();

    cout << "All philosophers are full and the program has completed.\n";
    return 0;
}
