#include <iostream>
#include <signal.h>
using namespace std;

void printMenu(){
    cout << "0. Выход;" << endl;
    cout << "1. Деление на ноль;" << endl;
    cout << "2. Нарушение защиты памяти;\n" << endl;
}

void divByZeroErrorSignal(int signum){
    cout <<  "Вызван сигнал SIGFPE (деление на ноль)" << endl;
    exit(1);
}

void memoryErrorSignal(int signum){
    cout << "Вызван сигнал SIGSEGV (нарушение защиты памяти)" << endl;
    exit(2);
}

void setSignals(){
    signal(SIGSEGV, memoryErrorSignal);
    signal(SIGFPE, divByZeroErrorSignal);
}

void doMemoryError(){
    int *value = nullptr;
    cout << *value;
}

void doDivByZeroError(){
    int infinity = 1 / 0;
    cout << infinity;
}

int main(){
    setSignals();
    printMenu();

    while(true){
        int value;

        cout << "Ввод: ";
        cin >> value;

        switch(value){
            case 0:
                return 0;
            case 1:
                doDivByZeroError();
                break;
            case 2:
                doMemoryError();
                break;
            default:
                cout << "\nПопробуйте снова:";
                break;
        }
    }
}

