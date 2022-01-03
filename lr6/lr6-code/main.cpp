#include <ctime>
#include <iostream>
#include <sys/wait.h>
#include <cstdio>
#include <cstdlib>
#include <unistd.h>
#include <csignal>
#include <sys/time.h>

using namespace std;

clock_t startChild;

/*
 * Написать периодическую программу, в которой период запуска и
 * количество запусков должны задаваться в качестве ее параметров.
 * При каждом очередном запуске программа должна порождать новый процесс,
 * который выводить на экран свой идентификатор, дату и время старта.
 * Программа и ее дочерний процесс должны быть заблокированы от
 * завершения при нажатии клавиши Ctrl/z. После завершения дочернего
 * процесса программа должна вывести на экран информацию о времени своей
 * работы и дочернего процесса.
 */

void signalFunction(int sig){
    if (sig == SIGALRM) {
        if (fork() == 0) {
            startChild = clock();
            time_t startTime = time(nullptr);
            cout << "Process ID: " << getpid() << endl;
            cout << "Время старта: " << ctime(&startTime);
            exit(EXIT_SUCCESS);
        }
    }
}

bool correctArguments(int argc){
    if(argc != 3){
        printf("Недостаточно аргументов для запуска\n");
        return false;
    }
    return true;
}

int getCountStarts(char** argv){
    return atoi(argv[1]);
}

int getPeriod(char** argv){
    return atoi(argv[2]);
}

struct itimerval getTimerValue(int period){
    struct itimerval timerValue;
    timerclear(&timerValue.it_interval);
    timerclear(&timerValue.it_value);

    timerValue.it_interval.tv_sec = period;
    timerValue.it_value.tv_sec = period;
    return timerValue;
}

void setSignal(){
    signal(SIGTSTP, SIG_IGN);
    signal(SIGALRM, signalFunction);
}

int main(int argc, char** argv){
    if(not correctArguments(argc)){
        exit(EXIT_SUCCESS);
    }

    int countStarts = getCountStarts(argv);

    int period = getPeriod(argv);

    struct itimerval timerValue = getTimerValue(period);

    setSignal();

    setitimer(ITIMER_REAL, &timerValue, nullptr);

    time_t startTime = time(nullptr);

    time_t startMain = time(nullptr);
    for(int i = 0; i < countStarts; i++){
        cout << "Потомок: №" << i+1 << endl;
        pause();
        wait(nullptr);
        clock_t finishChild = clock();
        cout << "Время работы: " << finishChild-startChild << " тиков" << endl << endl;
    }
    time_t finishMain = time(nullptr);

    cout << "Родитель" << endl;
    cout << "Process ID: " << getpid() << endl;
    cout << "Время старта: " << ctime(&startTime);
    cout << "Время работы: " << finishMain-startMain << " секунд" << endl;

    return 0;
}