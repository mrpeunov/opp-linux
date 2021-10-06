#include <iostream>
#include <fstream>
#include <unistd.h>
#include <string>
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
    exit(EXIT_SUCCESS);
}

int main(){
    output_in_file("Процесс из другого файла");
    return 0;
}