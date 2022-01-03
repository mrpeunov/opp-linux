#include <iostream>
#include <fstream>
#include <unistd.h>
#include <string>

using namespace std;

int mainStop = 10;
int childFirstStop = 5;
int childSecondStop = 3;
const char *childFirstProgramPath = "/home/peunov/highschool/opp-linux/laba1.2/cmake-build-debug/laba1_2";


void outputInFile(string processName, string filePath, int stop);
int error();
string getFilePath();

int main(int argc, char** argv){
    if(argc == 4){
        mainStop = atoi(argv[1]);
        childFirstStop = atoi(argv[2]);
        childSecondStop = atoi(argv[3]);
    }

    string filePath = getFilePath();

    pid_t fork_process_id = fork();

    if(fork_process_id == -1){
        return error();
    }

    if(fork_process_id == 0){
        sleep(childFirstStop);
        outputInFile("Потомок 1", filePath, childFirstStop);
        exit(EXIT_SUCCESS);
    }

    if(fork_process_id > 0){
        pid_t vfork_process_id = vfork();

        if(vfork_process_id == -1){
            return error();
        }

        if(vfork_process_id > 0){
            sleep(mainStop);
            outputInFile("Родитель", filePath, mainStop);
            exit(EXIT_SUCCESS);
        }

        if(vfork_process_id == 0){
            sleep(childSecondStop);
            outputInFile("Потомок 2", filePath, childSecondStop);
            execlp(childFirstProgramPath, nullptr);
            exit(EXIT_SUCCESS);
        }
    }

    return 0;
}

void outputInFile(string processName, string filePath, int stop){
    ofstream file;
    file.open(filePath, ios::app);

    if (file.is_open()) {
        pid_t process_id = getpid();
        file << processName << " - задержка: " << stop << endl;
        file << processName << " - идентификатор процесса: " <<  process_id << endl;
        file << processName << " - идентификатор предка: " << getppid() << endl;
        file << processName << " - идентификатор сессии процесса: " << getsid(process_id) << endl;
        file << processName << " - идентификатор группы процессов: " << getpgid(process_id) << endl;
        file << processName << " - реальный идентификатор пользователя: " << getuid() << endl;
        file << processName << " - эффективный идентификатор пользователя: " << geteuid() << endl;
        file << processName << " - реальный групповой идентификатор: " << getgid() << endl;
        file << processName << " - эффективный групповой идентификатор: " << getegid() << endl;
    }

    file.close();
}


int error(){
    cout << "При создании процесса произошла ошибка";
    return 1;
}

string getFilePath(){
    string filePath;
    cout << "Введите путь к файлу: ";
    cin >> filePath;
    return filePath;
}