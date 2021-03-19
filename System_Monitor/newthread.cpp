#include "newthread.h"
#include <iostream>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/sem.h>
#include <sys/stat.h>
#include <qdebug.h>
Newthread::Newthread(QString file){
    this->filename=file;
}

Newthread::~Newthread(){

}
void Newthread::openfile(){
    qDebug()<<"open!";
    pid_t pid1;
    QByteArray fname=filename.toLatin1();
    const char *path=fname.data();
    pid1=fork();
    if(pid1==0){
        execv(path,NULL);
    }
    return;
}
