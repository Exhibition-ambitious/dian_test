#include<winsock2.h>
#include<iostream>
#include<sstream>
#include<string>
#include <cstring>
using namespace std;
#pragma comment(lib, "ws2_32.lib")
enum Method{
    OPTIONS,SETUP,PLAY,TEARDOWN
};
struct request
{
    Method method;
    int CSeq;
    const char* body;
    struct request* nestRequest;
};
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
    if (clientSocket == INVALID_SOCKET)
    {
        cout << "Socket error" << endl;
        return 1;
    }
    char url[35];
    string IP;
    char ip[15];//IP地址
    string Port;
    char port[10];//端口号
    char protocol[10];//协议名
    //从文件中读取url并识别出服务端的ip地址和端口号
    FILE *fp;
    fp = fopen("url.txt" , "r");
    if(fp == NULL) 
    {
        perror("url error...");
        return(-1);
    }
    fgets (url, 35, fp);
    fclose(fp);
    int i,m;
    for(i=0,m=0;url[i]!='/';i++)
    {
        protocol[m++]=url[i];        
    }
    i+=2;
    for(m=0;url[i]!=':';m++,i++)
    {
        ip[m]=url[i];
    }
    ip[m]='\0';
    IP=ip;
    const char* Ip=IP.c_str();
    i++;
    for(m=0;url[i]!='\0';m++,i++)
    {
        port[m]=url[i];
    }
    port[m]='\0';
    Port=port;
    stringstream geek(Port);
    int s=0;
    geek>>s;//将端口号转化为整型
    //目标地址族
    sockaddr_in sock_in;
    sock_in.sin_family = AF_INET;
    sock_in.sin_port = htons(s);
    sock_in.sin_addr.S_un.S_addr = inet_addr(Ip);
    if (connect(clientSocket, (sockaddr*)&sock_in, sizeof(sock_in) )== SOCKET_ERROR){
        cout << "Connect error" << endl;
        return 1;
    }
    int CSeq=1;
    char sessionId[2];
    string Session_id;
    int num;
    string Url=url;
    //请求OPTIONS
    string data1;
    data1="OPTIONS "+Url+" 0.5\r\nCSeq:"+to_string(CSeq)+"\r\n\r\n";
    CSeq++;
    const char * msg1;
    msg1 = data1.c_str();
    //存入结构体
    request* firstRequest;
    request* p=(struct request*)malloc(sizeof(struct request));
    p->CSeq=CSeq-1;
    p->body=data1.c_str();
    p->method=OPTIONS;
    firstRequest=p;

    send(clientSocket, msg1, strlen(msg1), 0);
    char revdata1[300];
    //得到响应
    num = recv(clientSocket, revdata1, 300, 0);
    if (num > 0)
    {
        revdata1[num] = '\0';
        cout << revdata1 << endl;
    }
    //请求SETUP
    string data2;
    data2="SETUP "+Url+" 0.5\r\nCSeq:"+to_string(CSeq)+"\r\nTransport:TCP\r\nclient_port=(random)\r\n\r\n";
    CSeq++;
    const char * msg2;
    msg2 = data2.c_str();

    p->nestRequest=(struct request*)malloc(sizeof(struct request));
    p=p->nestRequest;
    p->CSeq=CSeq-1;
    p->body=data2.c_str();
    p->method=SETUP;

    send(clientSocket, msg2, strlen(msg2), 0);
    char revdata2[300];
    //得到响应
    num = recv(clientSocket, revdata2, 300, 0);
    if (num > 0)
    {
        revdata2[num] = '\0';
        cout << revdata2 << endl;
    }
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
    data3="PLAY "+Url+" 0.5\r\nCSeq:"+to_string(CSeq)+"\r\nSession_id="+Session_id+"\r\nRange:"+to_string(begin)+"-"+to_string(end)+"\r\n\r\n";
    CSeq++;
    const char * msg3;
    msg3 = data3.c_str();

    p->nestRequest=(struct request*)malloc(sizeof(struct request));
    p=p->nestRequest;
    p->CSeq=CSeq-1;
    p->body=data3.c_str();
    p->method=PLAY;

    send(clientSocket, msg3, strlen(msg3), 0);
    char revdata3[300];
    //得到响应
    num = recv(clientSocket, revdata3, 300, 0);
    if (num > 0)
    {
        revdata3[num] = '\0';
        cout << revdata3 << endl;
    }
    Sleep(1000);//切换线程可用于测试
    //请求TEARDOWN
    string data4;
    data4="TEARDOWN "+Url+" 0.5\r\nCSeq:"+to_string(CSeq)+"\r\nSession_id="+Session_id+"\r\n\r\n";
    CSeq++;
    const char * msg4;
    msg4 = data4.c_str();

    p->nestRequest=(struct request*)malloc(sizeof(struct request));
    p=p->nestRequest;
    p->CSeq=CSeq-1;
    p->body=data4.c_str();
    p->method=TEARDOWN;

    send(clientSocket, msg4, strlen(msg4), 0);
    char revdata4[300];
    //得到响应
    num = recv(clientSocket, revdata4, 300, 0);
    if (num > 0)
    {
        revdata4[num] = '\0';
        cout << revdata4 << endl;
    }
    p->nestRequest=NULL;
    request* q1=firstRequest;
    request* q2=q1;
    while(q2!=NULL)
    {
        cout<<"method:"<<q1->method<<endl;
        cout<<"CSeq:"<<q1->CSeq<<endl;
        cout<<"request body:"<<endl<<q1->body<<endl;
        q2=q1->nestRequest;
        free(q1);
        q1=q2;
    }
    //关闭连接
    closesocket(clientSocket);
    WSACleanup();
    cout<<"have closed..."<<endl;
    return 0;
}