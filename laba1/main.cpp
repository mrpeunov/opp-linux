#include <unistd.h>
#include <sys/types.h>
#include <iostream>
using namespace std;

int main_process(){
    printf("Дочерний %d\n", getpid());
    printf("Родительский %d\n", getppid());
    return 0;
}

int children_fork_process(){
    // потомок 1
    printf("Родительский %d\n", getpid());
    printf("Дочерний %d\n", getppid());
    return 0;
}

int children_vfork_process(){
    // потомок 2
    printf("Родительский %d\n", getpid());
    printf("Дочерний %d\n", getppid());
    return 0;
}

int spawning_processes(){
    pid_t fork_process_id = fork(); //порождаем процесс

    if(fork_process_id == -1){
        // ошибка
        cout << "При создании процесса произошла ошибка";
        return 1;
    }

    if(fork_process_id > 0){
        // потомок 1
        children_fork_process();
        return 0;
    }

    if(fork_process_id == 0){
        // родитель
        pid_t vfork_process_id = vfork();

        if(vfork_process_id == -1){
            // ошибка
            cout << "При создании процесса произошла ошибка";
            return 1;
        }

        if(vfork_process_id == 0){
            // родитель
            main_process();
            return 0;
        }

        if(vfork_process_id > 0){
            // потомок 2
            children_vfork_process();
            return 0;
        }
    }
}

int main(){
    cout << "Лабораторная работа №3" << endl;
    spawning_processes();
    return 0;
}

