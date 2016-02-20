//
//  EchoServer.c
//  
//
//  Created by Linda Marshall on 2016/02/05.
//
//  Based on the TCPEchoServer in TCP/IP Sockets in C - Practical Guide for Programmers

#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <iostream>

using namespace std;

int main(int argc, char* argv[]) {
    int serverSocket;
    int clientSocket;
    struct sockaddr_in serverAddress;
    struct sockaddr_in clientAddress;
    unsigned short port;
    unsigned int clientLength;
    char buffer[256];
    char message[256];
    int n;
    int exit = -1;

    
    if (argc != 2) {
        cout << "Incorrect number of arguments" << endl;
        cout << "Usage is: " << argv[0] << " <port>" << endl;
    } else {
        port = atoi(argv[1]);
        
        cout << "Opening a connection on port " << port << endl;
        
        serverSocket = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
        if (serverSocket < 0) {
            cout << "Opening a socket failed" << endl;
        } else {
            
            bzero((char *) &serverAddress, sizeof(serverAddress));
            serverAddress.sin_family = AF_INET;
            serverAddress.sin_addr.s_addr = htonl(INADDR_ANY);
            serverAddress.sin_port = htons(port);
            
            if (bind(serverSocket, (struct sockaddr *) &serverAddress, sizeof(serverAddress)) < 0) {
                cout << "Bind failed" << endl;
            } else {
                if (listen(serverSocket,5) < 0) {
                    cout << "Listen failed" << endl;
                } else {
                    while (exit != 0) {
                        clientLength = sizeof(clientAddress);
                        if ((clientSocket = accept(serverSocket, (struct sockaddr *) &clientAddress, &clientLength)) < 0) {
                            cout << "Accept failed" << endl;
                        } else {
                            bzero(buffer,256);
                            bzero(message,256);
                            strcpy(message,"Please type in your message (type ZZZ to shutdown the server): ");
                            n = write(clientSocket,message,strlen(message));
                            if (n < 0)
                                cout << "ERROR writing to socket" << endl;
                            n = read(clientSocket,buffer,255);
                            if (n < 0)
                                cout << "ERROR reading from socket" << endl;
                            else {
                                if ((strcmp(buffer,"ZZZ\r\n") == 0) || (strcmp(buffer,"ZZZ") == 0)) {
                                    // "ZZZ\r\n" for telnet, "ZZZ" for own client
                                    exit = 0;
                                } else {
                                    cout << "Here is the message: " << buffer << endl;
                                    bzero(message,256);
                                    strcpy(message,"I got your message\n ");
                                    n = write(clientSocket,message,strlen(message));
                                    if (n < 0)
                                        cout << "ERROR writing to socket" << endl;
                                    
                                }
                                close(clientSocket);
                            }
                        }
                    }
                    
                    cout << "Server shutting down...." << endl;
                    close(serverSocket);

                }
            }
        }
    }


    return 0;
}
