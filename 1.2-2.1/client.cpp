#include<winsock2.h>
#include<iostream>
#include<string>
#include <cstring>
using namespace std;
#pragma comment(lib, "ws2_32.lib")
int main()
{
    WORD sockVersion = MAKEWORD(2, 2);
    WSADATA data;
    if (WSAStartup(sockVersion, &data) != 0)
    {
        return 1;
    }
        //创建套接字
        SOCKET clientSocket = socket(AF_INET, SOCK_STREAM, 0);
        if (clientSocket == INVALID_SOCKET){
            cout << "Socket error" << endl;
            return 1;
        }
        //目标地址族
        sockaddr_in sock_in;
        sock_in.sin_family = AF_INET;
        sock_in.sin_port = htons(8888);
        sock_in.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");
        if (connect(clientSocket, (sockaddr*)&sock_in, sizeof(sock_in) )== SOCKET_ERROR){
            cout << "Connect error" << endl;
            return 1;
        }
        int CSeq;
        char sessionId[2];
        string Session_id;
        int num;
        //请求OPTIONS
        string data1;
        data1="OPTIONS diantp://127.0.0.1:8888 0.5\r\nCSeq: 1\r\n\r\n";
        const char * msg1;
        msg1 = data1.c_str();
        send(clientSocket, msg1, strlen(msg1), 0);
        char revdata1[300];
        //得到响应
        num = recv(clientSocket, revdata1, 300, 0);
        if (num > 0){
            revdata1[num] = '\0';
            cout << revdata1 << endl;
        }
        //请求SETUP
        string data2;
        data2="SETUP diantp://127.0.0.1:8888 0.5\r\nCSeq: 2\r\nTransport:TCP\r\nclient_port=(random)\r\n\r\n";
        const char * msg2;
        msg2 = data2.c_str();
        send(clientSocket, msg2, strlen(msg2), 0);
        char revdata2[300];
        //得到响应
        num = recv(clientSocket, revdata2, 300, 0);
        if (num > 0){
            revdata2[num] = '\0';
            cout << revdata2 << endl;
        }
        int i,m;
        for(i=0;revdata2[i]!='s';i++);
        while(revdata2[i++]!=':');
        m=0;
        while(revdata2[i]>='0'&&revdata2[i]<='9')
        {
            sessionId[m]=revdata2[i];
            m++;
            i++;
        }
        sessionId[m]='\0';
        Session_id=sessionId;  
        //请求PLAY
        string data3;
        int begin=0,end=100;
        data3="PLAY diantp://127.0.0.1:8888 0.5\r\nCSeq: 3\r\nSession_id="+Session_id+"\r\nRange:"+to_string(begin)+"-"+to_string(end)+"\r\n\r\n";
        const char * msg3;
        msg3 = data3.c_str();
        send(clientSocket, msg3, strlen(msg3), 0);
        char revdata3[300];
        //得到响应
        num = recv(clientSocket, revdata3, 300, 0);
        if (num > 0){
            revdata3[num] = '\0';
            cout << revdata3 << endl;
        }
        //请求TEARDOWN
        string data4;
        data4="TEARDOWN diantp://127.0.0.1:8888 0.5\r\nCSeq: 4\r\nSession_id="+Session_id+"\r\n\r\n";
        const char * msg4;
        msg4 = data4.c_str();
        send(clientSocket, msg4, strlen(msg4), 0);
        char revdata4[300];
        //得到响应
        num = recv(clientSocket, revdata4, 300, 0);
        if (num > 0){
            revdata4[num] = '\0';
            cout << revdata4 << endl;
        }
        //关闭连接
    closesocket(clientSocket);
    WSACleanup();
    cout<<"have closed..."<<endl;
    return 0;
}