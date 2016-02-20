//
//  EchoClient.cpp
//  
//
//  Created by Linda Marshall on 2016/02/06.
//
//  Based on the TCPEchoClient in TCP/IP Sockets in C - Practical Guide for Programmers

#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <stdlib.h>
#include <iostream>

using namespace std;

int main(int argc, char* argv[]) {
    
    int clientSocket;
    struct sockaddr_in serverAddress;
    struct hostent *server;
    char* echoString;
    unsigned int echoStringLength;
    unsigned int serverPort;
    char buffer[256];
    char message[256];
    int totalBytesReceived;
    int bytesReceived;
    int n;
    
    
    if (argc < 3)  {
        cout << "Incorrect number of arguments" << endl;
        cout << "Usage is: " << argv[0] << " <hostname> <port>" << endl;
    } else {
        serverPort = atoi(argv[2]);
        
        clientSocket = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
        if (clientSocket < 0 ) {
            cout << "Opening a socket failed" << endl;
        } else {
            server= gethostbyname(argv[1]);
            if (server == NULL) {
                cout << "No such host" << endl;
            } else {
                bzero((char *) &serverAddress, sizeof(serverAddress));
                serverAddress.sin_family = AF_INET;
                bcopy((char *)server->h_addr,
                      (char *)&serverAddress.sin_addr.s_addr,
                      server->h_length);
                serverAddress.sin_port = htons(serverPort);
            
                if (connect(clientSocket, (struct sockaddr*) &serverAddress, sizeof(serverAddress)) < 0) {
                    cout << "Connection failed" << endl;
                } else {
                    bzero(buffer,256);
                    n = read(clientSocket,buffer,255);
                    if (n < 0)
                        cout << "ERROR reading from socket" << endl;
                    else {
                        cout << buffer;
                        bzero(message,256);
                        cin.get(message,256);
                        n = write(clientSocket,message,strlen(message));
                        if (n < 0)
                            cout << "ERROR writing to socket" << endl;
                        else {
                            bzero(buffer,256);
                            n = read(clientSocket,buffer,255);
                            if (n < 0)
                                cout << "ERROR reading from socket" << endl;
                            else
                                cout << "Echo'ed message: " << buffer <<  endl;
                        }
                        
                    
                    }
                    close(clientSocket);
                }

            }
        }
    }
    return 0;
        
}