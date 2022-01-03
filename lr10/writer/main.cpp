#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <unistd.h>
#include <fstream>
using namespace std;

const char* file_path = "/home/peunov/highschool/opp-linux/lr10/files/output.txt";
#define SEM_KEY 183


//0 - мьютекс файла
struct sembuf file_mute = {0, -1, 0};
struct sembuf file_unmute = {0, 1, 0};

//1 - семафор количества процессов
struct sembuf process_increase = {1, 1, 0};
struct sembuf process_decrease = {1, -1, 0};

//2 - семафор количества писателей желающих писать
struct sembuf writers_wont_write_increase = {2, 1, 0};
struct sembuf writers_wont_write_decrease = {2, -1, 0};
struct sembuf writers_wont_write_is_zero = {2, 0, 0};

//3 - семафор количества читателей
struct sembuf active_readers_increase = {3, 1, 0};
struct sembuf active_readers_decrease = {3, -1, 0};
struct sembuf active_readers_is_zero = {3, 0, 0};


void create_file(){
    ofstream output;
    output.open(file_path, ios::out);
    output.close();
}


void print_in_file(int pid, int i, int count){
    ofstream output;
    output.open(file_path, ios::app);
    output << "Процесс " << pid << " записал - " << i << "/" << count << endl;
    cout << "Процесс " << pid << " записал - " << i << "/" << count << endl;
    output.close();
}

void close_semaphore(int semaphore_id){
    if(semctl(semaphore_id, 1, GETVAL, 0 ) == 0){
        semctl(semaphore_id,IPC_RMID,0);
        printf("Семафор уничтожены\n");
    }
}

int semaphore_init(int pid){
    int semaphore_id = semget(SEM_KEY,4,IPC_CREAT|IPC_EXCL|0666);
    if(semaphore_id > 0){
        create_file();
        cout << "Процесс " << pid << ": создал семафор. ID - " << semaphore_id << endl;
        semop(semaphore_id, &file_unmute, 1); // размьючиваем файл
    } else {
        semaphore_id = semget(SEM_KEY,4,IPC_CREAT);
        cout << "Процесс " << pid << ": используем существующий семафор. ID - " << semaphore_id << endl;
    }
    semop(semaphore_id, &process_increase, 1);
    return semaphore_id;
}

int main(int argc, char **argv){
    if(argc != 3){
        printf("Недостаточное количество аргументов: ./name <количество операций>");
    }

    // парсим аргументы
    int number_of_writing = atoi(argv[1]);
    int delay = atoi(argv[2]);

    // получаем номер процесса
    int pid = getpid();

    // инициализируем семафоры
    int semaphore_id = semaphore_init(pid);

    cout << "Процесс начал работу" << endl;

    for(int i = 0; i < number_of_writing; i++){
        semop(semaphore_id, &writers_wont_write_increase, 1); // повышает количество писателей желающих начать писать
        semop(semaphore_id, &active_readers_is_zero, 1); // ждет всех читателей
        semop(semaphore_id, &file_mute, 1); // мьютит файл
        print_in_file(pid, i+1, number_of_writing); // пишет в файл
        semop(semaphore_id, &writers_wont_write_decrease, 1); //понижает количество писателей желающих писать
        semop(semaphore_id, &file_unmute, 1); //размьючивает файл
        sleep(delay);
    }
    semop(semaphore_id, &process_decrease, 1);

    close_semaphore(semaphore_id);

    return 0;
}



