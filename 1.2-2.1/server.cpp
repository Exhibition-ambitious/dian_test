#include<winsock2.h>
#include<iostream>
#include<stdlib.h>
#pragma comment(lib,"ws2_32.lib")
#include<string>
#include <cstring>
using namespace std;
#include <time.h>
int main(int argc, char* argv[]){
    //初始化DLL
    WORD sockVersion = MAKEWORD(2, 2);
    WSADATA wsdata;
    if (WSAStartup(sockVersion, &wsdata) != 0)
    {
        return 1;
    }    
    //创建套接字
    SOCKET serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket == INVALID_SOCKET)
    {
        cout << "Socket error" << endl;
        return 1;
    }
    //绑定套接字
    sockaddr_in sockAddr;
    sockAddr.sin_family = AF_INET;
    sockAddr.sin_port = htons(8888);
    sockAddr.sin_addr.S_un.S_addr = INADDR_ANY; //本地ip地址
    if (bind(serverSocket, (sockaddr*)&sockAddr, sizeof(sockAddr)) == SOCKET_ERROR){
        cout << "Bind error" << endl;
        return 1;
    }
    //开始监听
    if (listen(serverSocket, 5) == SOCKET_ERROR){
        cout << "Listen error" << endl;
        return 1;
    }
    char url[20];
    int CSeq;
    char Transport[10];
    char client_port[10];
    int i;
    SOCKET clientSocket;//客户端socket
    sockaddr_in client_sin;//客户端地址族
    char msg1[300];//存储传送的消息
    char msg2[300];
    char msg3[300];
    char msg4[300];
    int flag = 0;//是否已经连接上
    int len = sizeof(client_sin);
    if (!flag)
        cout << "waiting..." << endl;
    clientSocket = accept(serverSocket, (sockaddr*)&client_sin, &len);//accept接收
    if (clientSocket == INVALID_SOCKET){
        cout << "Accept error" << endl;
    flag = 0;
        return 1;
    }
    if (!flag)
        cout << "receive a link.." << inet_ntoa(client_sin.sin_addr) << endl;//将客户端地址转换成“.”点隔的字符串格式
    //OPTIONS
    int num;
    num = recv(clientSocket, msg1, 100, 0);
    if (num > 0)
    {
        msg1[num] = '\0';
        cout << msg1 << endl;
    }
    if(msg1[0]=='O')
    {
        cout<<"OPTIONS"<<endl;
        i=0;
        while(msg1[i]!=' ')
            i++;
        i++;
        for(int m=0;msg1[i]!=' ';m++,i++)
        {
            url[m]=msg1[i];
        }
        i++;
        while(msg1[i]!='C')
        {
            i++;
        }
        i=i+5;
        CSeq=int(msg1[i]);
        string send_msg1="0.5 200 OK\r\nCSeq:1\r\nOPTIONS SETUP PLAY TEARDOWN\r\n\r\n";
        const char* Msg1=send_msg1.c_str();
        send(clientSocket, Msg1, strlen(Msg1), 0);
    }
    else
        cout<<"error"<<endl;
    //SETUP
    num = recv(clientSocket, msg2, 100, 0);
    if (num > 0)
    {
        msg2[num] = '\0';
        cout << msg2 << endl;
    }
    if(msg2[0]=='S')
    {
        i=0;
        while(msg2[i]!=' ')
            i++;
        i++;
        for(int m=0;msg2[i]!=' ';m++,i++)
        {
            url[m]=msg2[i];
        }
        while(msg2[i]!='T')
        {
            i++;
        }
        i=i+10;
        for(int m=0;m<3;m++,i++)
        {
            Transport[m]=msg2[i];
        }
        for(int m=0,n=10;url[n]=='\0';m++,n++)
        {
            client_port[m]=url[n];
        }
        srand(time(NULL));
        int session_id=rand()%100;
        string Session_id=to_string(session_id);
        string send_msg2;
        send_msg2="0.5 200 OK\r\nCSeq:2\r\nsession_id:"+Session_id+"\r\n\r\n";
        const char* Msg2=send_msg2.c_str();
        send(clientSocket, Msg2, strlen(Msg2), 0);
    }
    else 
        cout<<"error"<<endl;
    //PLAY
    num = recv(clientSocket, msg3, 100, 0);
    if (num > 0)
    {
        msg3[num] = '\0';
        cout << msg3 << endl;
    }
    char begin[3];
    char end[3];
    string Begin;
    string End;
    if(msg3[0]=='P')
    {
        const char* Msg3;
        char text[100];
        int i=0,n=0;
        while(msg3[i]!='R')
        {
            i++;
        }
        i=i+6;
        for(n=0;msg3[i]!='-';n++,i++)
        {
            begin[n]=msg3[i];
        }
        begin[n]='\0';
        Begin=begin;
        i++;
        for(n=0;msg3[i]>='0'&&msg3[i]<='9';i++,n++)
        {
            end[n]=msg3[i];
        }
        end[n]='\0';
        End=end;
        int m;
        for(n=stoi(Begin);n<stoi(End);n++)
        {
            text[n]='1'+n%9;
        }
        text[n]='\0';
        string Text=text;
        string send_msg3="0.5 200 OK\r\nCSeq:3\r\n"+Text+"\r\n\r\n";
        Msg3=send_msg3.c_str();
        send(clientSocket, Msg3, strlen(Msg3), 0);
    }
    else 
        cout<<"error"<<endl;
    //TEARDOWN
    num = recv(clientSocket, msg4, 100, 0);
    if (num > 0)
    {
        msg4[num] = '\0';
        cout << msg4 << endl;
    }
    if(msg4[0]=='T')
    {
        const char* Msg4;
        string send_msg4="0.5 200 OK\r\nCSeq:4\r\nstop...\r\n\r\n";
        Msg4=send_msg4.c_str();
        send(clientSocket, Msg4, strlen(Msg4), 0);
    }
    else 
        cout<<"error"<<endl;
    //关闭连接
    closesocket(serverSocket);
    WSACleanup();
    cout<<"have closed..."<<endl;
    return 0;
}

