#include <iostream>
#include <fstream>
#include <pthread.h>
using namespace std;


/*
 * Написать программу, которая открывает входной файл и 2 выходных файла.
 * Затем она должна в цикле построчно читать входной файл и порождать 2 потока.
 * Одному потоку передавать нечетную строку, а другому – четную.
 * Оба потока должны работать параллельно.
 * Каждый поток записывает в свой выходной файл полученную строку и завершает работу.
 * Программа должна ожидать завершения работы каждого потока и
 * повторять цикл порождения потоков и чтения строк входного файла,
 * пока не прочтет последнюю строку, после чего закрыть все файлы.
 */

ofstream outputFirst, outputSecond;
ifstream input;

void* threadFunctionFirst(void *arg){
    string line = *((string *) arg);
    outputFirst << line << endl;
    pthread_exit(nullptr);
}

void* threadFunctionSecond(void *arg){
    string line = *((string *) arg);
    outputSecond << line << endl;
    pthread_exit(nullptr);
}

int main(){
    //инициализация переменных
    pthread_t thread;
    void* (*thread_function)(void *arg);
    string line;
    bool flag = true;

    //конфиг
    string inputFilePath = "/home/peunov/highschool/opp-linux/lr4/files/input.txt";
    string outputFirstFilePath = "/home/peunov/highschool/opp-linux/lr4/files/output1.txt";
    string outputSecondFilePath = "/home/peunov/highschool/opp-linux/lr4/files/output2.txt";

    //откроем ввод-вывод
    input.open(inputFilePath);
    outputFirst.open(outputFirstFilePath);
    outputSecond.open(outputSecondFilePath);

    while(getline(input, line) ){
        //обработка четности/нечетности
        if(flag) {
            thread_function = threadFunctionFirst;
        } else {
            thread_function = threadFunctionSecond;
        }
        flag = !flag;

        //создание потока
        pthread_create(&thread, nullptr, thread_function, &line);

        //ожидание завершения
        pthread_join(thread, nullptr);
    }

    //закрываем ввод вывод
    input.close();
    outputFirst.close();
    outputSecond.close();

    return 0;
}