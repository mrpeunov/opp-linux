#include<stdlib.h>
#include<sys/socket.h>
#include<arpa/inet.h>
#include <time.h>
#include <iostream>

#define SIZE 5
using namespace std;

int *request;

void generate_nums(){
    request = (int*)malloc(SIZE);
    for(int i = 0; i < SIZE; i++){
        request[i] = rand() % 1000;
    }
}

void print_array(const char *message, int* array){
    cout << message;
    for(int i = 0; i < SIZE; i++){
        cout << array[i] << " ";
    }
    cout << endl;
}


int main(int argc , char *argv[]){
    srand(time(nullptr));

    struct timeval tv;
    int response[1024];
    int current_time, connection;
    fd_set fds;
    struct sockaddr_in server;

    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_family = AF_INET;
    server.sin_port = htons( 9000);

    int socket_descriptor = socket(AF_INET , SOCK_STREAM , 0);
    cout << "Клиент ожидает подключения к серверу..." << endl;

    time_t start = time(nullptr);
    do {
        current_time = time(nullptr) - start;
        connection = connect(socket_descriptor, (struct sockaddr *) &server, sizeof(server));
    }
    while (current_time < 10 && connection < 0);

    if(connection < 0){
        cout << "Истекло время ожидания" << endl;
        return 0;
    }

    generate_nums();
    print_array("Запрос: ", request);

    if (send(socket_descriptor, request, SIZE * sizeof(int), 0) < 0) {
        cout << "Отправка не удалась" << endl;
        exit(EXIT_FAILURE);
    }
    cout << "Данные отправлены" << endl;

    FD_ZERO(&fds);
    FD_SET(socket_descriptor, &fds);
    tv.tv_sec=10;
    tv.tv_usec=0;

    if(select(FD_SETSIZE, &fds, nullptr, nullptr, &tv) == 0){
        cout << "Истекло время ожидания" << endl;
        exit(EXIT_FAILURE);
    }

    recv(socket_descriptor, response, 1024, 0);
    print_array("Ответ: ", response);
    return 0;
}
