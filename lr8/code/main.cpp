#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <errno.h>
#include <time.h>
#include <unistd.h>
#include <iostream>
#include <fstream>

using namespace std;


typedef struct Message_T {
    long type; //получатель
    int sender;  // отправитель
    int responseQue; //очередь, куда отправить ответ
    time_t time; // создания запроса
    bool reply;
} Message;

int programNumber;
bool owner = false;
int QUE_KEY = 333;
string filePath = "/home/peunov/highschool/opp-linux/lr8/files/input";
Message globalMessages[2]; // здесь собираются запросы от других файлов


int getNeighbor(bool next){
    if (programNumber == 1 && next) return 2;
    if (programNumber == 1 && !next) return 3;

    if (programNumber == 2 && next) return 1;
    if (programNumber == 2 && !next) return 3;

    if (programNumber == 3 && next) return 2;
    if (programNumber == 3 && !next) return 1;
}


Message createMessage(long receiver, int responseQue, time_t time){
    Message message;

    message.type = receiver;
    message.sender = programNumber;
    message.responseQue = responseQue;
    message.time = time;
    message.reply = false;

    return message;
}

void sendRequestOnRead(bool neighborBefore, int globalQue, int localQue, time_t time){
    int neighbor = getNeighbor(neighborBefore);
    Message request = createMessage( neighbor, localQue, time);
    msgsnd(globalQue, &request, sizeof(request), 0);
    cout << "№" << programNumber << ": отправлен запрос программе "  << neighbor
    << "; время: " << time << endl;
}

void sendPermission(int neighbor, int que, time_t time){
    Message request = createMessage(neighbor, 0, time);
    msgsnd(que, &request, sizeof(request), 0);
    cout << "№" << programNumber << ": отправлено разрешение программе " << neighbor
    << "; время: " << time << endl;
}

void sendPermissions(){
    for(auto & globalMessage : globalMessages){
        if(not globalMessage.reply){
            sendPermission(globalMessage.sender, globalMessage.responseQue, time(nullptr));
        }
    }
}

int getOrCreateGlobalQue(){
    int generalQue = msgget(QUE_KEY, 0606|IPC_CREAT|IPC_EXCL);
    if(generalQue == -1){
        generalQue =  msgget(QUE_KEY, 0606|IPC_CREAT); // queOwner = 0;
        owner = true;
    } else {
        owner = false;
    }
    return generalQue;
}

int getLocalQue(){
    return msgget(IPC_PRIVATE, 0606|IPC_CREAT);
}

void printProgramInfo(int globalQue, int localQue) {
    cout << "№" << programNumber << ": глобальная очередь - " << globalQue << endl;
    cout << "№" << programNumber << ": локальная очередь - " << localQue << endl << endl;
}

void waitPermissions(int globalQue, int localQue, time_t requestTime){
    Message permissions[2];
    ssize_t status;
    int count_permission = 0, global = 0;

    while(count_permission < 2){
        // получаем сообщения из глобальной очереди
        status = msgrcv(globalQue, &globalMessages[global], sizeof(globalMessages[global]), programNumber, IPC_NOWAIT);
        if(status!=-1){
            // получали сообщение из глобальной очереди
            if(globalMessages[global].time < requestTime){
                //отправляем в ответ разрешение на чтение
                sendPermission(globalMessages[global].sender, globalMessages[global].responseQue, time(nullptr));
                globalMessages[global].reply = true;
            } else {
                cout << "№" << programNumber << ": отказано программе " << globalMessages[global].sender
                << "; время: " << globalMessages[global].time << endl;
            }
            global+=1;
        }

        // получаем разрешение
        status = msgrcv(localQue, &permissions[count_permission], sizeof(permissions[count_permission]), 0, IPC_NOWAIT);
        if(status!=-1){
            //получили разрешение
            cout << "№" << programNumber << ": получили разрешение от " << permissions[count_permission].sender
            << "; время: " << permissions[count_permission].time << endl;
            count_permission += 1;
        }

        sleep(1);
    }
}

void closeQue(int globalQue, int localQue){
    if(owner) msgctl(globalQue, IPC_RMID,nullptr);
    msgctl(localQue,IPC_RMID,nullptr);
}

void readFile(){
    ifstream input;
    string inputStr;
    input.open(filePath, ios_base::in);

    cout << "№" << programNumber << ": Все разрешения получены. Начинается чтение файла" << endl;
    cout << "№" << programNumber << ": Содержимое файла" << endl;
    while (getline(input, inputStr)) {
        cout << "\t" << inputStr << endl;
    }
    cout << "№" << programNumber << ": чтение закончено" << endl;

    input.close();
}

void sendRequests(int globalQue, int localQue, time_t sendTime){
    sendRequestOnRead(true, globalQue, localQue, sendTime);
    sendRequestOnRead(false, globalQue, localQue, sendTime);
}

int main(int argc, char** argv){
    time_t sendTime;

    // 1. Получаем данные из параметров
    if(argc != 3){
        cout << "Недостаточно аргументов" << endl;
        return 0;
    }
    programNumber = atoi(argv[1]);
    QUE_KEY = atoi(argv[2]);

    // 2. Создаем/получаем глобальную очередь
    int globalQue = getOrCreateGlobalQue();

    // 3. Создаем локальную очередь
    int localQue = getLocalQue();

    printProgramInfo(globalQue, localQue);

    // 4. Отправляем запрос на чтение файла другим очередям
    sendTime = time(nullptr);
    sendRequests(globalQue, localQue, sendTime);

    // 5.1. Читаем запросы с глобальной очереди
    // 5.2. Если есть запрос раньше нашего, отправляем разрешение на чтение
    // 5.3. Иначе нет
    waitPermissions(globalQue, localQue, sendTime);

    // 6. Когда получаем все разрешения - читаем файл
    readFile();

    // 7. Отправляем разрешения другим программам
    sendPermissions();

    // 8. Закрываем очереди
    closeQue(globalQue, localQue);

    return 0;
}
