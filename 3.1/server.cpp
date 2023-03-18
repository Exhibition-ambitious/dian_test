#include<winsock2.h>
#include<iostream>
#include<stdlib.h>
#pragma comment(lib,"ws2_32.lib")
#include<string>
#include <cstring>
#include <windows.h>
#include <fstream>
#include <time.h>
using namespace std;
struct SockInfo
{
    SOCKET clientsocket;       // 通信
    PDWORD tid;                // 线程ID
    struct sockaddr_in addr;   // 客户端地址信息
};
struct SockInfo infos[128];
void* working(void* arg)
{
    while(1)
    {
        struct SockInfo* info = (struct SockInfo*)arg;
        // 接收数据
        char buf[1024];
        char url[35];
        int CSeq;
        char Cseq[3];
        string scseq;
        char Transport[10];
        char client_port[10];
        int i;
        char begin[3];
        char end[3];
        string Begin;
        string End;
        int session_id;
        string Session_id;
        int ret = recv(info->clientsocket, buf, sizeof(buf),0);
        if(ret == 0)
        {
            printf("have been closed...\n");
            info->clientsocket = INVALID_SOCKET;
            break;
        }
        else if(ret == -1)
        {
            printf("error receive...\n");
            info->clientsocket = INVALID_SOCKET;
            break;
        }
        else
        {
            cout<<"receive a link. "<<"the port is "<<info->addr.sin_port<<endl;
            //OPTIONS
            if(buf[0]=='O')
            {
                i=0;
                while(buf[i]!=' ')
                    i++;
                i++;
                for(int m=0;buf[i]!=' ';m++,i++)
                {
                    url[m]=buf[i];
                }
                i++;
                while(buf[i]!='C')
                {
                    i++;
                }
                i=i+5;
                int m;
                for(m=0;buf[i]>='0'&&buf[i]<='9';m++,i++)
                {
                    Cseq[m]=buf[i];
                }
                Cseq[m]='\0';
                scseq=Cseq;
                string send_msg1="0.5 200 OK\r\nCSeq:"+scseq+"\r\nOPTIONS SETUP PLAY TEARDOWN\r\n\r\n";
                const char* Msg1=send_msg1.c_str();
                send(info->clientsocket, Msg1, strlen(Msg1), 0);
                memset(buf,0,1024);
            }
            else if(buf[0]=='S')//SETUP
            {
                i=0;
                while(buf[i]!=' ')
                    i++;
                i++;
                //cout<<buf<<endl;
                int m;
                for(m=0;buf[i]!=' ';m++,i++)
                {
                    url[m]=buf[i];
                }
                url[m]='\0';
                //cout<<url<<endl;
                while(buf[i]!='C')
                {
                    i++;
                }
                i=i+5;
                //cout<<buf[i]<<endl;
                for(m=0;buf[i]>='0'&&buf[i]<='9';m++,i++)
                {
                    Cseq[m]=buf[i];
                }
                Cseq[m]='\0';
                scseq=Cseq;
                while(buf[i]!='T')
                {
                    i++;
                }
                i=i+10;
                for(int m=0;m<3;m++,i++)
                {
                    Transport[m]=buf[i];
                }
                srand(time(NULL));
                session_id=rand()%100;
                Session_id=to_string(session_id);
                string send_msg2;
                send_msg2="0.5 200 OK\r\nCSeq:"+scseq+"\r\nsession_id:"+Session_id+"\r\n\r\n";
                const char* Msg2=send_msg2.c_str();
                send(info->clientsocket, Msg2, strlen(Msg2), 0);
                memset(buf,0,1024);
            }
            else if(buf[0]=='P')//PLAY
            {
                char csessionId[5];
                string cSession_id;
                int csession_id;
                const char* Msg3;
                int i=0,n=0;
                while(buf[i]!='C')
                {
                    i++;
                }
                i=i+5;
                int m;
                for(m=0;buf[i]>='0'&&buf[i]<='9';m++,i++)
                {
                    Cseq[m]=buf[i];
                }
                Cseq[m]='\0';
                scseq=Cseq;
                while(buf[i]!='S')
                {
                    i++;
                }
                i=i+11;
                for(m=0;buf[i]>='0'&&buf[i]<='9';m++,i++)
                {
                    csessionId[m]=buf[i];
                }
                csessionId[m]='\0';
                cSession_id=csessionId;
                csession_id=stoi(cSession_id);
                if(csession_id!=session_id)
                {
                    cout<<"illegal request..."<<endl;
                }
                while(buf[i]!='R')
                {
                    i++;
                }
                i=i+6;
                for(n=0;buf[i]!='-';n++,i++)
                {
                    begin[n]=buf[i];
                }
                begin[n]='\0';
                Begin=begin;
                i++;
                for(n=0;buf[i]>='0'&&buf[i]<='9';i++,n++)
                {
                    end[n]=buf[i];
                }       
                end[n]='\0';
                End=end;
                FILE *fp;
                fp = fopen("text.txt" , "r");
                if(fp == NULL) 
                {
                    perror("text error...");
                    return 0;
                }
                char text[100];
                fgets (text, 100, fp);
                fclose(fp);
                string Text=text;
                string send_msg3="0.5 200 OK\r\nCSeq:"+scseq+"\r\n"+Text.substr(stoi(Begin),stoi(End))+"\r\n\r\n";
                Msg3=send_msg3.c_str();
                send(info->clientsocket, Msg3, strlen(Msg3), 0);
                memset(buf,0,1024);
            }
            else if(buf[0]=='T')//TEARDOWN
            {
                char csessionId[5];
                string cSession_id;
                int csession_id;
                i=0;
                while(buf[i]!='C')
                {
                    i++;
                }
                i=i+5;
                int m;
                for(m=0;buf[i]>='0'&&buf[i]<='9';m++,i++)
                {
                    Cseq[m]=buf[i];
                }
                Cseq[m]='\0';
                scseq=Cseq;
                while(buf[i]!='S')
                {
                    i++;
                }
                i=i+11;
                for(m=0;buf[i]>='0'&&buf[i]<='9';m++,i++)
                {
                    csessionId[m]=buf[i];
                }
                csessionId[m]='\0';
                cSession_id=csessionId;
                csession_id=stoi(cSession_id);
                const char* Msg4;
                string send_msg4="0.5 200 OK\r\nCSeq:"+scseq+"\r\nstop...\r\n\r\n";
                Msg4=send_msg4.c_str();
                send(info->clientsocket, Msg4, strlen(Msg4), 0);
                memset(buf,0,1024);
                if(csession_id!=session_id)
                {
                    cout<<"illegal request..."<<endl;
                }
                ret=0;
            }
            else 
                cout<<"error"<<endl;
        }
    }
    return NULL;
}
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
    int len = sizeof(struct sockaddr);
    int max = sizeof(infos) / sizeof(infos[0]);
    for(int i=0; i<max; ++i)
    {
        memset(&infos[i], 0,sizeof(infos[i]));
        infos[i].clientsocket = INVALID_SOCKET;
        infos[i].tid = 0;
    }
    while(true)
    {
        struct SockInfo* pinfo;   
        for(int i=0; i<max; ++i)
        {
            if(infos[i].clientsocket == INVALID_SOCKET)
            {
                pinfo = &infos[i];
                break;
            }
            if(i == max-1)
            {
                Sleep(1);//线程切换
                i--;
            }
        }
        SOCKET client_socket = accept(serverSocket, (struct sockaddr*)&pinfo->addr, &len);
        if (client_socket == INVALID_SOCKET){
            cout << "Accept error" << endl;
            return 1;
        }
        string username;
        char Username[20];
        memset(Username,0,20);
        string password;
        char Password[20];
        memset(Password,0,20);
        int is_right=0;
        if(recv(client_socket,Username,sizeof(Username),0)>0)
        {
            username=Username;
        }
        if(recv(client_socket,Password,sizeof(Password),0))
        {
            password=Password;
        }
        ifstream infile("clientname.txt");
        char Line[20];
        while (infile.getline(Line,20))
        {
            if(!strcmp(Username,Line))
            {
                infile.getline(Line,20);
                if(!strcmp(Password,Line))
                {
                    is_right=1;
                }
            }
        }
        if(is_right==0)
        {
            send(client_socket,"0",1,0);
            cout<<"one client verify error..."<<endl;
            continue;
        }
        else 
        {
            send(client_socket,"1",1,0);
        }
        pinfo->clientsocket = client_socket;
        CreateThread(NULL,                              //保证线程的安全性
                    0,                                  //指定线程的堆栈深度
                    (LPTHREAD_START_ROUTINE)working,    //线程函数名
                    pinfo,                              //线程函数参数
                    0,                                  //控制线程状态的附加标志
                    pinfo->tid);                        //线程ID
    }
    closesocket(serverSocket);
    WSACleanup();
    cout<<"have closed..."<<endl;
    return 0;
}