#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/time.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <signal.h>
#include <iostream>
#include <fstream>

using namespace std;

/*
 * Написать 3 программы, которые запускаются в произвольном порядке и построчно
 * записывают свои индивидуальные данные в один файл через определенный промежуток времени.
 * Пока не закончит писать строку одна программа, другие две не должны обращаться к файлу.
 * Частота записи данных в файл и количество записываемых строк определяются входными
 * параметрами, задаваемыми при запуске каждой программы. При завершении
 * работы одной из программ другие должны продолжить свою работу.
 * Синхронизация работы программ должна осуществляться с помощью общих переменных,
 * размещенных в разделяемой памяти.
 *
 * Откомпилировать 3 программы и запустить их на разных терминалах с различными входными параметрами.
 * */

#define COUNT_PROCESSES 3
const char *file_path = "/home/peunov/highschool/opp-linux/lr9/output/result.txt";

typedef struct _memory {
    bool choosing[COUNT_PROCESSES];
    int number[COUNT_PROCESSES];
    int count;
} memory;

memory* shared_memory;
int program_id;


void lock(memory*, int);
void unlock(memory*, int);


int get_max(int* array) {
    int max = array[0];
    for (size_t j = 0; j < COUNT_PROCESSES; ++j) {
        if (array[j] > max) {
            max = array[j];
        }
    }
    return max;
}

bool more(int* array, int i, int j){
    return (array[j] < array[i] || (array[i] == array[j] && j < i));
}

void lock(memory* m, int process) {
    // Алгоритм пекарни Лампорта
    m->choosing[process] = true;
    m->number[process] = get_max(m->number) + 1;
    m->choosing[process] = false;

    for (int j = 0; j < COUNT_PROCESSES; ++j) {
        while (m->choosing[j]);
        while (m->number[j] != 0 && more(m->number, process, j));
    }
}

void unlock(memory* m, int process) {
    m->number[process] = 0;
}

void output_in_file(){
    time_t current_time = time(nullptr);
    ofstream output;
    output.open(file_path, ios::app);
    output << "Программа №" << program_id << ": PID - " << getpid() << ", время -" << ctime(&current_time);
    output.close();
}

void signal_handler(int sig){
    if (sig == SIGALRM) {
        cout << "Программа №" << program_id << " ожидает освобождения ресурса" << endl;

        lock(shared_memory, program_id);
        output_in_file();
        unlock(shared_memory, program_id);

        cout << "Программа №" << program_id << " освободила ресурс" << endl;

    }
}

void set_timer(int period){
    struct itimerval timer;
    timerclear(&timer.it_interval);
    timerclear(&timer.it_value);
    timer.it_interval.tv_sec = period;
    timer.it_value.tv_sec = period;
    setitimer(ITIMER_REAL, &timer, nullptr);
}

int main(int argc, char** argv){
    if(argc != 5){
        cout << "Ошибка! Недостаточно аргументов для запуска" << endl;
        cout << "Правильный порядок аргументов: номер программы, ключ, количество запусков, период" << endl;
    }

    // получаем аргументы
    program_id = atoi(argv[1]);
    int key = atoi(argv[2]);
    int count_starts = atoi(argv[3]);
    int period = atoi(argv[4]);

    //устанавливаем сигнал
    signal(SIGALRM, signal_handler);

    int shared_memory_id = shmget(key, sizeof(memory), (0666 | IPC_CREAT));
    cout << "Программа №" << program_id << " получила id: " << shared_memory_id << " разделяемой памяти" << endl;

    void* shared_memory_address = shmat(shared_memory_id,nullptr,0);
    cout << "Программа №" << program_id << " присоединила разделенную память" << endl;
    shared_memory = (memory*)shared_memory_address;

    //ожидаем открытия всех программ
    shared_memory->count += 1;
    cout << "Ожидаем остальных......." << endl;
    while (shared_memory->count != 3);
    cout << "........................дождались" << endl;

    // установка таймера
    set_timer(period);

    for(int i = 0; i < count_starts; i++){
        pause();
    }

    shmdt(shared_memory_address);
    cout << "Программа №" << program_id << " отсоединила разделяемую память" << endl;

    exit(EXIT_SUCCESS);
}