#include <iostream>
#include <signal.h>
using namespace std;

/*
 * Написать программу, которая реагирует на ошибки при выполнении операции деления и неверном использовании указателя
 * (деление на ноль, нарушение защиты памяти).
 *
 * При обнаружении ошибки программа должна передать управление функции,
 * которая выведет сообщение и завершит работу программы с кодом ошибки (1 или 2).
 *
 * Тип ошибки, который должна зафиксировать программа, задается как параметр при ее запуске.
 * */

void print_menu();
void set_signals();
void div_by_zero_error_signal(int signum);
void memory_error_signal(int signum);
void do_memory_error();
void do_div_by_zero_error();

int main(){
    set_signals();
    print_menu();

    while(true){
        int value;

        cout << "Введите значение: ";
        cin >> value;

        switch(value){
            case 0:
                return 0;
            case 1:
                do_div_by_zero_error();
                break;
            case 2:
                do_memory_error();
                break;
            default:
                cout << "\n\nВведено недопустимое значение. Попробуйте снова: ";
                break;
        }
    }
}

void print_menu(){
    cout << "0. Выход;" << endl;
    cout << "1. Деление на ноль;" << endl;
    cout << "2. Нарушение защиты памяти;\n" << endl;
}

void set_signals(){
    signal(SIGSEGV, memory_error_signal);
    signal(SIGFPE, div_by_zero_error_signal);
}

void div_by_zero_error_signal(int signum){
    cout <<  "Вызван сигнал SIGFPE (деление на ноль)" << endl;
    exit(1);
}

void memory_error_signal(int signum){
    cout << "Вызван сигнал SIGSEGV (нарушение защиты памяти)" << endl;
    exit(2);
}

void do_memory_error(){
    int *value = nullptr;
    *value = 228;
    cout << value;
}

void do_div_by_zero_error(){
    int infinity = 1 / 0;
    cout << infinity;
}