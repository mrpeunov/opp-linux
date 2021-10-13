#include <iostream>
#include <string>
#include <fcntl.h>
#include <pthread.h>
#include <unistd.h>

using namespace std;


/*
 * Написать программу, которая открывает текстовый файл, порождает поток, а затем ожидает его завершения.
 * Потоку в качестве параметра передается дескриптор файла. Поток выводит на экран класс планирования,
 * текущий, минимальный и максимальный приоритеты, содержимое файла и закрывает файл.
 * После завершения работы потока программа должна вывести
 * текущий приоритет и проверить – закрыт ли файл, и если он не закрыт, то
 * принудительно закрыть. Результат проверки должен быть выведен на экран
*/

int openFile(const string& filePath, const string& threadName);
void closeFile(int handle, const string& threadName);
int error(const string& errorMessage);
void printThreadInfo(const string& threadName);
void* threadFunction(void* arg);
bool checkFileIsClosed(int handle);
void printFileContain(int handle, const string& threadName);
void print_current_priority(const string& threadName);



int main() {
    cout << "Лабораторная 4. Задание 1" << endl;

    //инициализация переменных
    pthread_t thread;
    string filePath = "/home/peunov/highschool/opp-linux/lr4/files/text.txt";

    //открываем файл и получаем дескрпитор (handle)
    int handle = openFile(filePath, "Главный поток");
    if(handle == -1){ return error("Не удалось открыть файл");}

    //порождаем поток и передаем дескриптор файла
    pthread_create(&thread, nullptr, threadFunction, &handle);

    //ожидает завершение потока
    pthread_join(thread, nullptr);

    //вывести текущий приоритет
    print_current_priority("Главный поток");

    //проверить закрыт ли файл
    bool fileIsClosed = checkFileIsClosed(handle);

    //если файл открыт, то закрыть
    if(!fileIsClosed){
        closeFile(handle, "Главный поток");
    }

    //вывести результат проверки
    if(fileIsClosed) {
        cout << "(Главный поток) - файл был закрыт дочерним потоком";
    } else {
        cout << "(Главный поток) - файл не был закрыт дочерним потоком";
    }

    return 0;
}


void* threadFunction(void* arg){
    //получение данных из главного потока
    int handle = *((int *) arg);

    // Вывод информации о потоке
    printThreadInfo("Дочерний поток");

    // Поток выводит содержимое файла
    printFileContain(handle, "Дочерний поток");

    // Поток закрывает файл (опционально)
    //closeFile(handle, "Дочерний поток");

    pthread_exit(nullptr);
}

bool checkFileIsClosed(int handle){
    return fcntl(handle, F_GETFD) == -1;
}

void closeFile(int handle, const string& threadName){
    cout << "(" << threadName << ") - закрываем файл с дескриптором: " << handle << endl;
    close(handle);
}

int openFile(const string& filePath, const string& threadName){
    int handle = open(filePath.c_str(), O_RDONLY);
    if(handle == -1){
        cout << "Ошибка открытия файла" << endl;
        return -1;
    }

    cout << "(" << threadName << ") - файл успешно открыт. " << "Дескриптор: " << handle << endl;
    return handle;
}

int error(const string& errorMessage){
    cout << "Ошибка:" << errorMessage;
    return -1;
}

void printThreadInfo(const string& threadName){
    //получение данных о потоке
    int policy;
    struct sched_param param{};
    pthread_getschedparam(pthread_self(), &policy, &param);

    // Поток выводит класс планирования
    switch(policy){
        case SCHED_FIFO:
            cout << "(" << threadName << ") - класс планирования: SCHED_FIFO" << endl;
            break;
        case SCHED_RR:
            cout << "(" << threadName << ") - класс планирования: SCHED_RR" << endl;
            break;
        case SCHED_OTHER:
            cout << "(" << threadName << ") - класс планирования: SCHED_OTHER" << endl;
            break;
        default:
            break;
    }

    // Поток выводит текущий, минимальный и максимальный приоритеты
    cout << "(" << threadName << ") - текущий приоритет: " << param.sched_priority << endl;
    cout << "(" << threadName << ") - минимальный приоритет: " << sched_get_priority_min(policy) << endl;
    cout << "(" << threadName << ") - максимальный приоритет: " << sched_get_priority_max(policy) << endl;
}

void printFileContain(int handle, const string& threadName){
    char buffer[1024];
    size_t buffer_size = sizeof(buffer);
    ssize_t bytes_read = read(handle, buffer, buffer_size);
    buffer[bytes_read] = '\0';
    cout << "(" << threadName << ") - содержимое файла: " << buffer;
}

void print_current_priority(const string& threadName){
    int policy;
    struct sched_param param{};
    pthread_getschedparam(pthread_self(), &policy, &param);
    // Поток выводит текущий, минимальный и максимальный приоритеты
    cout << "(" << threadName << ") - текущий приоритет: " << param.sched_priority << endl;
}