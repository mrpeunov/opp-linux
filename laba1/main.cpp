#include <iostream>
#include <fstream>
#include <unistd.h>
#include <sys/types.h>
#include <string>
#include <sys/wait.h>

using namespace std;

void output_in_file(const string& process_name){
    string file_path = "/home/peunov/highschool/opp-linux/output/output.txt";
    ofstream file;
    file.open(file_path, ios::app);

    if (file.is_open()) {
        pid_t process_id = getpid();
        file << process_name << " - идентификатор процесса: " <<  process_id << endl;
        file << process_name << " - идентификатор предка: " << getppid() << endl;
        file << process_name << " - идентификатор сессии процесса: " << getsid(process_id) << endl;
        file << process_name << " - идентификатор группы процессов: " << getpgid(process_id) << endl;
        file << process_name << " - реальный идентификатор пользователя: " << getuid() << endl;
        file << process_name << " - эффективный идентификатор пользователя: " << geteuid() << endl;
        file << process_name << " - реальный групповой идентификатор: " << getgid() << endl;
        file << process_name << " - эффективный групповой идентификатор: " << getegid() << endl;
    }

    file.close();
}

void main_process(){
    output_in_file("Родитель");
}

void child1_process(){
    output_in_file("Потомок 1");
}

void child2_process(){
    const char *child2_program_path = "/home/peunov/highschool/opp-linux/laba1.2/cmake-build-debug/laba1_2";
    output_in_file("Потомок 2");
    execlp(child2_program_path, NULL);
}

int error(){
    cout << "При создании процесса произошла ошибка";
    return 1;
}

int spawning_processes(){
    pid_t fork_process_id = fork();

    if(fork_process_id == -1){
        return error();
    }

    if(fork_process_id > 0){
        sleep(5);
        child1_process();
    }

    if(fork_process_id == 0){
        pid_t vfork_process_id = vfork();

        if(vfork_process_id == -1){
            return error();
        }

        if(vfork_process_id > 0){
            sleep(3);
            child2_process();
        }

        if(vfork_process_id == 0){
            sleep(10);
            int status;
            wait(&status);
            main_process();
        }
    }

    return 0;
}

int main(){
    cout << "Лабораторная работа №3" << endl;
    return spawning_processes();
}


