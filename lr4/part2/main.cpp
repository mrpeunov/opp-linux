#include <iostream>
#include <fstream>
#include <pthread.h>
using namespace std;

ofstream outputFirst, outputSecond;
ifstream input;

string inputPath = "/home/peunov/highschool/opp-linux/lr4/files/input.txt";
string outputFirstPath = "/home/peunov/highschool/opp-linux/lr4/files/output1.txt";
string outputSecondPath = "/home/peunov/highschool/opp-linux/lr4/files/output2.txt";

void* threadFunctionFirst(void *arg){
    string line = *((string *) arg);
    outputFirst << line << endl;
    pthread_exit(NULL);
}

void* threadFunctionSecond(void *arg){
    string line = *((string *) arg);
    outputSecond << line << endl;
    pthread_exit(NULL);
}

int main(){
    pthread_t thread1, thread2;
    string str1, str2;
    bool flag1, flag2;

    input.open(inputPath);
    outputFirst.open(outputFirstPath);
    outputSecond.open(outputSecondPath);

    while(true){
        flag1 = (bool)getline(input, str1);
        flag2 = (bool)getline(input, str2);

        if(flag1) pthread_create(&thread1, NULL, threadFunctionFirst, &str1);
        if(flag2) pthread_create(&thread2, NULL, threadFunctionSecond, &str2);

        if(flag1) pthread_join(thread1, NULL);
        if(flag1) pthread_join(thread2, NULL);

        if(not flag1 or not flag2) break;
    }

    input.close();
    outputFirst.close();
    outputSecond.close();

    return 0;
}
