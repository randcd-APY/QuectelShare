#include "mainwindow.h"
#include <QApplication>
#include <ql_bt.h>
#include <stdio.h>
#include <string.h>
#include <iostream>

char AD[30][40];
int pos = 0;
char addr[18];
char addr_temp[18];
int position = -1;
MainWindow *w;

void connect_index(int index){
    strcpy(addr_temp,AD[index]);
    QL_BT_pair(addr_temp);
    position = index;
    printf("connect_index over!\n");
}

void clear_ble_list(){
    pos = 0;
}

void update(const char *ap,const char *address){
    printf("ap:%s --> address:%s.\n",ap,address);
    int flag = 0;
    string data;
    if(ap != NULL){
        data = ap; 
        data = data + "--------------";
        flag ++; 
    }   
    if(address != NULL){
        data = data + address;
        flag ++; 
    }   
    
    if(flag == 2){ 
        for(int i = 0;i < pos;i ++){
            if(!strcmp(AD[i],address)){
                return;
            }   
        }   
        strcpy(AD[pos++],address);
        w->update(data);
        //cout << "lxzha ble list count :" << pos << endl;
    }   
}

void update_address(const char *address){
    int length = strlen(address);
    for(int i = 0;i < length;i ++){
        addr[i] = address[i];
    }   
    addr[17] = '\0';
    cout << "lxzha qt address----" << address << "-------" << length << endl;
}

void recv_data(const char *data){
    w->updateRecvData(data);
}

void sock_connect(){ 
    cout << "lxzha sock connect" << endl;
    if(position != -1){
        strcpy(addr_temp,AD[position]);
        QL_BT_socket_connect(addr_temp);
    }else{
        strcpy(addr_temp,addr);
        QL_BT_socket_connect(addr_temp);
    }
    w->updateConnState(0);
}

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    w = new MainWindow;
    w->show();
    w->set_ble_clear_list(clear_ble_list);
    w->set_connect_index(connect_index);
    w->set_sock_connect(sock_connect);
    QL_BT_set_scan_cb(update);
    QL_BT_set_pair_cb(update_address);
    QL_BT_set_recv_cb(recv_data);
    return a.exec();
}
