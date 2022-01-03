#include <cstdio>
#include <cstdlib>
#include <unistd.h>
#include <csignal>
#include <iostream>
#include <fstream>


using namespace std;

ofstream output;
int readPipeFd, childNumber;
bool readFlag = true;
bool doneWhite = false;


void end_write_signal(int sig){
    cout << "Запись в канал закончена" << endl;
    doneWhite = true;
}

void read_for_pipe(int signal){
    cout << "Поток " << childNumber << " получил сигнал" << endl;

    char symbol;

    if(read(readPipeFd, &symbol, 1) != -1){
        cout << "Потомок " << childNumber << ". Прочитанный символ: " << symbol << endl;
        output << symbol;
    } else {
        cout << "Потомок " << childNumber << ": канал пуст" << endl;
        if(doneWhite) readFlag = false;
    }
    kill(0, signal);
}

void first_signal(int sig){
    //если потомок получает свой же сигнал просто выходим
    if(childNumber == 1){
        //cout << "Потомок 1 получил свой же SIGUSR1" << endl;
        return;
    }
    // иначе читаем из канала
    read_for_pipe(SIGUSR2);
}

void second_signal(int sig){
    //если потомок получает свой же сигнал просто выходим
    if(childNumber == 2){
        //cout << "Потомок 2 получил свой же SIGUSR2" << endl;
        return;
    }
    // иначе читаем из канала
    read_for_pipe(SIGUSR1);
}

void setSignals(){
    signal(SIGINT, end_write_signal);
    signal(SIGUSR1, first_signal);
    signal(SIGUSR2, second_signal);
}

int main(int argc, char** argv){
    if(argc != 3){
        printf("Недостаточно аргументов для запуска\n");
        return 0;
    }
    childNumber = atoi(argv[0]);
    readPipeFd = *argv[1];
    char* fileOutputPath = argv[2];

    cout << "Запущен потомок : " << childNumber << endl;

    setSignals();

    output.open(fileOutputPath, ios::out);
    while(readFlag)
        pause();
    output.close();

    cout << "Выход из потомка " << childNumber << endl;
    exit(EXIT_SUCCESS);
}