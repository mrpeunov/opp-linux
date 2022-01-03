#include<stdlib.h>
#include<sys/socket.h>
#include<arpa/inet.h>
#include <unistd.h>
#include <iostream>

using namespace std;

int socket_handle, request_socket;
int reply[1024];

#define SIZE 5

int compare(const void *a, const void *b) {
    return *(int*)a - *(int*)b;
}

void print_array(const char *message){
    cout << message;
    for(int i = 0; i < SIZE; i++){
        cout << reply[i] << " ";
    }
    cout << endl;
}

void service_process(){
    if(!fork()) {
        close(socket_handle);
        recv(request_socket, reply, 1024, 0);

        print_array("Сервер принял данные: ");
        qsort(reply, SIZE, sizeof(int), compare);
        print_array("Сервер отсортировал данные: ");
        cout << "Сервер отправил данные" << endl;

        send(request_socket, reply, SIZE * sizeof(int), 0);
        close(request_socket);
        exit(EXIT_SUCCESS);
    }
}

int main(int argc , char *argv[]){
    struct sockaddr_in server;
    struct timeval tv;
    int cv;
    fd_set fds;

    int socket_handle = socket(AF_INET , SOCK_STREAM , 0);
    cout << "Сервер создал сокет" << endl;

    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons( 9000 );

    bind(socket_handle, (struct sockaddr *)&server, sizeof(server));
    cout << "Произошла привязка к сетевому адресу" << endl;

    listen(socket_handle, 5);

    cout << "Сервер ждет клиентов..." << endl << endl;

    while(true){
        FD_ZERO(&fds);
        FD_SET(socket_handle, &fds);
        tv.tv_sec = 10;
        tv.tv_usec = 0;

        cv = select(FD_SETSIZE, &fds, nullptr, nullptr, &tv);
        if(cv == 0){
            cout << "Истекло время ожидания" << endl;
            break;
        } else {
            request_socket = accept(socket_handle, nullptr, nullptr);
            cout << "К серверу подключился клиент" << endl;
            service_process();
        }
    }

    close(socket_handle);
    return 0;
}
