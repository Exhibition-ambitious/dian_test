#include<winsock2.h>
#include<iostream>
#pragma comment(lib,"ws2_32.lib")
#include<string>
using namespace std;
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


    SOCKET clientSocket;//客户端socket
    sockaddr_in client_sin;//客户端地址族
    char msg[100];//存储传送的消息
    int flag = 0;//是否已经连接上
    int len = sizeof(client_sin);
    /*while (true){
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
        flag = 1;
        int num = recv(clientSocket, msg, 100, 0);
        if (num > 0)
        {
            msg[num] = '\0';
            cout <<"Client say: "<< msg << endl;
        }

        string data;
        getline(cin, data);
        const char * sendData;
        sendData = data.c_str();
        send(clientSocket, sendData, strlen(sendData), 0);
        closesocket(clientSocket);
    }*/
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
    int num = recv(clientSocket, msg, 100, 0);
    if (num > 0)
    {
        msg[num] = '\0';
        cout <<"Client say: "<< msg << endl;
    }
    /*string data;
    getline(cin, data);
    const char * sendData;
    sendData = data.c_str();*/
    send(clientSocket, msg, strlen(msg), 0);
    closesocket(serverSocket);
    WSACleanup();
    cout<<"have closed..."<<endl;
    return 0;
}