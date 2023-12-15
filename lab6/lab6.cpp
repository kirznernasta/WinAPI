#include <windows.h>
#include <iostream>
using namespace std;

const int BUFFER_SIZE = 30000;
int buffer[BUFFER_SIZE];
int in = 0;
int out = 0;

CONDITION_VARIABLE notFull;
CONDITION_VARIABLE notEmpty;
CRITICAL_SECTION criticalSection;

void produceItem(int item) {
    buffer[in] = item;
    in = (in + 1) % BUFFER_SIZE;
}

int consumeItem() {
    int item = buffer[out];
    out = (out + 1) % BUFFER_SIZE;
    return item;
}

void producer(int id) {
    while (true) {
        int newItem = (id * 100) + (rand() % 100);

        EnterCriticalSection(&criticalSection);

        while ((in + 1) % BUFFER_SIZE == out) {
            cout << "Producer " << id << " is waiting..." << endl;
            SleepConditionVariableCS(&notFull, &criticalSection, INFINITE);
        }

        produceItem(newItem);
        cout << "Producer " << id << " produced: " << newItem << endl;

        WakeConditionVariable(&notEmpty);

        LeaveCriticalSection(&criticalSection);

        Sleep(rand() % 3000);
    }
}

void consumer(int id) {
    while (true) {
        EnterCriticalSection(&criticalSection);

        while (in == out) {
            cout << "Consumer " << id << " is waiting..." << endl;
            SleepConditionVariableCS(&notEmpty, &criticalSection, INFINITE);
        }

        int item = consumeItem();
        cout << "Consumer " << id << " consumed: " << item << endl;

        WakeConditionVariable(&notFull);

        LeaveCriticalSection(&criticalSection);

        Sleep(rand() % 3000);
    }
}

int main() {
    InitializeConditionVariable(&notFull);
    InitializeConditionVariable(&notEmpty);
    InitializeCriticalSection(&criticalSection);

    const int NUM_PRODUCERS = 2;
    const int NUM_CONSUMERS = 2;

    HANDLE producerThreadHandles[NUM_PRODUCERS];
    HANDLE consumerThreadHandles[NUM_CONSUMERS];

    for (int i = 0; i < NUM_PRODUCERS; i++) {
        producerThreadHandles[i] = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)producer, (LPVOID)i, 0, NULL);
    }

    for (int i = 0; i < NUM_CONSUMERS; i++) {
        consumerThreadHandles[i] = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)consumer, (LPVOID)i, 0, NULL);
    }

    WaitForMultipleObjects(NUM_PRODUCERS, producerThreadHandles, TRUE, INFINITE);
    WaitForMultipleObjects(NUM_CONSUMERS, consumerThreadHandles, TRUE, INFINITE);

    for (int i = 0; i < NUM_PRODUCERS; i++) {
        CloseHandle(producerThreadHandles[i]);
    }

    for (int i = 0; i < NUM_CONSUMERS; i++) {
        CloseHandle(consumerThreadHandles[i]);
    }

    DeleteCriticalSection(&criticalSection);

    return 0;
}
