#include <iostream>
#include <sys/wait.h>
#include <unistd.h>
#include <cstring>
#include <fcntl.h>
#include <fstream>
#include <csignal>

using namespace std;

/*
 * Написать программу, которая обменивается данными через канал с двумя потомками.
 * Программа открывает входной файл, построчно читает из него данные и записывает их в канал.
 * Потомки выполняют свои программы и поочередно читают символы из канала и записывают их в свои выходные файлы:
 * первый потомок – нечетные символы, а второй – четные.
 * Синхронизация работы потомков должна осуществляться напрямую с использованием сигналов SIGUSR1 и SIGUSR2.
 * Об окончании записи файла в канал программа оповещает потомков сигналом SIGQUIT и ожидает
 * завершения работы потомков.
 * Когда они заканчивают работу, программа закрывает канал.
 * */

#define SIZE 128
string inputFilePath = "/home/peunov/highschool/opp-linux/lr7/files/input";
const char * childProgramPath = "/home/peunov/highschool/opp-linux/lr7/child/cmake-build-debug/child";
const char * outputFirstFilePath = "/home/peunov/highschool/opp-linux/lr7/files/output1.txt";
const char * outputSecondFilePath = "/home/peunov/highschool/opp-linux/lr7/files/output2.txt";


void signal_handler(int sig){
    if (sig == SIGQUIT) {
        cout << "Родитель получил SIGQUIT" << endl;
    }
    if (sig == SIGUSR1) {
        cout << "Родитель получил SIGUSR1" << endl;
    }
    if (sig == SIGUSR2) {
        cout << "Родитель получил SIGUSR2" << endl;
    }
}

void setSignals(){
    signal(SIGQUIT, signal_handler);
    signal(SIGUSR1, signal_handler);
    signal(SIGUSR2, signal_handler);
}

int main(){
    cout << "Предок старт" << endl;

    int pipeFd[2];


    pipe(pipeFd);

    pid_t pid_1 = fork();
    if (not pid_1) {
        execl(childProgramPath,"1", &pipeFd[0], outputFirstFilePath, NULL);
    }
    pid_t pid_2 = fork();
    if (not pid_2) {
        execl(childProgramPath, "2", &pipeFd[0], outputSecondFilePath, NULL);
    }
    sleep(1);
    setSignals();

    ifstream input;
    input.open(inputFilePath, ios_base::in);
    fcntl(*pipeFd, F_SETFL, O_NONBLOCK);

    kill(pid_1, SIGUSR2);

    while (getline(input, inputStr)) {
        cout << "Прочитана строка: " << inputStr << endl;
        write(pipeFd[1], inputStr.c_str(), inputStr.length());
    }


    cout << "Предок закончил писать в канал и послал SIGQUIT" << endl;
    kill(pid_1, SIGINT);
    kill(pid_2, SIGINT);

    waitpid(pid_1, nullptr, 0);
    waitpid(pid_2, nullptr, 0);
    cout << "Предок дождался потомков" << endl;

    close(pipeFd[0]);
    close(pipeFd[1]);
    input.close(); //перенести

    return 0;
}
