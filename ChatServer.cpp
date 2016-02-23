//
//  ChatServer.cpp
//  
//
//  Created by Linda Marshall on 2016/02/06.
//
//  Compile with g++ -c -std=c++14 ChatServer_2.cpp -pthread
//

#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <iostream>
//#include <list>
#include <thread>
#include <string.h>
#include <vector>
#include <unistd.h>
#include <algorithm>
#include <map>

using namespace std;


class ChatServer {
    public:
        ChatServer(unsigned short, int);
        void serverRun();
        vector<int> getList();
        void removeSocket(int);
		map<string,int> userList;
    protected:
        ChatServer();
    private:
        vector<int> clientList;
        unsigned short serverPort;
        int serverSocket;
        vector<thread> threads; // Required

};


void chatFunction(int, ChatServer*);
void broadcast(char[256],char[256],ChatServer*);


int main(int argc, char* argv[]) {
	cout << "\e[0m" << endl;
	cout << "\e[97m" << endl;	
    unsigned short serverPort = 9200;
    int serverSocket;
    struct sockaddr_in serverAddress;

    unsigned int clientLength;
    struct sockaddr_in clientAddress;
    int clientSocket;
    
    unsigned int n = thread::hardware_concurrency();
    cout << n << " concurrent threads are supported." << endl;
    
    if (argc > 2) {
        cout << "Incorrect number of arguments" << endl;
        cout << "Usage is: " << argv[0] << " [<port>]" << endl;
    } else {
        if (argc == 2) {
            serverPort = atoi(argv[1]);
        }
        
        serverSocket = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
        if (serverSocket < 0) {
            cout << "Opening a socket failed" << endl;
        } else {
            
            bzero((char *) &serverAddress, sizeof(serverAddress));
            serverAddress.sin_family = AF_INET;
            serverAddress.sin_addr.s_addr = htonl(INADDR_ANY);
            serverAddress.sin_port = htons(serverPort);
            
            if (::bind(serverSocket, (struct sockaddr *) &serverAddress, sizeof(serverAddress)) < 0) { // Compiling with C++11, requires the scoping operator when calling the bind function
                cout << "Bind failed" << endl;
            } else {
                if (listen(serverSocket,5) < 0) {
                    cout << "Listen failed" << endl;
                } else {
                    ChatServer cs(serverPort,serverSocket);
                    cs.serverRun();
                }
            }
            
        }

    }
}

ChatServer::ChatServer(unsigned short port, int socket) {
    serverPort = port;
    serverSocket = socket;
}

void readOnce()
{
	string input;
	if (input =="QUIT" || input == "QUIT\r\n")
		exit(0);
}

void ChatServer::serverRun() {
    unsigned int clientLength;
    struct sockaddr_in clientAddress;
    int clientSocket;
    
    cout << "Chat server is running on port " << serverPort << endl;
    while (true) {
        clientLength = sizeof(clientAddress);
        if ((clientSocket = accept(serverSocket, (struct sockaddr *) &clientAddress, &clientLength)) < 0) {
            cout << "Accept failed" << endl;
        } else {
            cout << "New client joined the chat " << endl;
            threads.push_back(thread((*chatFunction), clientSocket, this));
            clientList.push_back(clientSocket);
		}
    }
    
}

vector<int> ChatServer::getList() {
    return clientList;
}

void ChatServer::removeSocket(int socket) {
    vector<int>::iterator it;

	if (clientList.size() == 0)
		exit(0);    

    it = find (clientList.begin(), clientList.end(), socket);
    if (it != clientList.end()) {
        clientList.erase(it);
    }
}

void chatFunction(int socket, ChatServer* cs) {
    char message[256];
    int n;
    char name[256];
    char buffer[256];
    
    bzero(message,256);
    strcpy(message,"\e[107m \e[31mWelcome to the chat room.\e[0m \n \e[44mType \e[1mQUIT to leave.\e[0m \n What is your name? ");
    n = write(socket,message,strlen(message));
    if (n < 0) cout << "ERROR writing to socket" << endl;
    n = read(socket,name,255);
	string saveName = name;
	saveName = saveName.substr(0,saveName.length());
	cs->userList[saveName] = socket;
	cout << "Saved socket is " << cs->userList[saveName] << endl;
    if (n < 0) cout << "ERROR reading from socket" << endl;
    bzero(message,256);
    strcat(message,"has joined the chat");
    broadcast(message,name, cs);  
    bzero(message,256);
    strcat(message,"\n\e[5m\e[41m\e[97m>>\e[0m");
    n = write(socket,message,strlen(message));
    if (n < 0) cout << "ERROR writing to socket" << endl;
    bzero(buffer,256);
    n = read(socket,buffer,255);
    if (n < 0) cout << "ERROR reading from socket" << endl;
    
    while (!((strcmp(buffer,"ZZZ\r\n") == 0) || (strcmp(buffer,"ZZZ") == 0)))
	{
		if (strstr(buffer,"--help"))
		{
			string output = "";
			output += "\n\t Chat Server Commands";
			output += "\n\e[97m BROADCAST <message>\e[0m    Sends a message to all other users.";
			output += "\n\e[97m SEND <user> <message>\e[0m   Sends a message to a specific online user.";		
			output += "\n\e[97m DISPLAY\e[0m   Displays all online users.";
			output += "\n\e[97m QUIT\e[0m   Ends the chat session.";
			write(socket,output.c_str(),strlen(output.c_str()));
		}	
		else if (strstr(buffer,"BROADCAST"))
		{
			string message = buffer;
			message = message.replace(0,9,"");
			message = "\e[30m\e[47m" + message + "\e[0m\n";
        	broadcast(const_cast<char*>(message.c_str()),name,cs);
 		}
		else if (strstr(buffer, "QUIT"))
		{
			exit(0);			//threads	
		}
		else if (strstr(buffer,"SEND"))
		{

			string message = buffer;
			message = message.replace(0,5,"");
			string saveName(message.c_str() + 0, message.find(" "));
			
			message.erase(0,saveName.length());
			string temp = message;
			message = "\n\e[30m\e[47m";
			message += name;
			message += "\e[1m Personal Message \e[21m:" + temp + "\e[0m \e[0m \n";			
			n = write(cs->userList[saveName],message.c_str(),strlen(message.c_str()));
			//n = write(cs->userList[saveNameAlt],message.c_str(),strlen(message.c_str()));
			if (n < 0) cout << "FAILED" << endl;
			else cout << name << " successfully sent " << saveName << " a message." << endl;
		}
		else if (strstr(buffer,"DISPLAY"))
		{
				
				cout << "----------" << endl;
				for(auto it = cs->userList.begin(); it != cs->userList.end(); ++it)
					cout << "| \e[44m" <<  it->first << "\e[0m \t |" << endl;
				cout << "----------" << endl;

				string output = "";

				output += "--------\n";
				for (auto it = cs->userList.begin(); it != cs->userList.end(); ++it)
					output += "|" + it->first + "|\n";
				output += "--------\n";
				write(socket,output.c_str(),strlen(output.c_str()));
		}       
       	 
			bzero(message,256);
        	strcat(message,"\e[5m\e[41m\e[97m>>\e[0m");
        	n = write(socket,message,strlen(message)+4);
        	if (n < 0) cout << "ERROR writing to socket" << endl;
        	bzero(buffer,256);
        	n = read(socket,buffer,255);
        	if (n < 0)
            	cout << "ERROR reading from socket" << endl;
 
	}
    bzero(message,256);
    strcat(message,"has left the chat...");
    broadcast(message,name, cs);
	cs->userList.erase(saveName);
    cs->removeSocket(socket);
    close(socket);
	return;
}

void broadcast(char msg[256], char name[256], ChatServer* cs, int clientSocket)
{
    char message[256];
    int n;
    vector<int> cl;

    bzero(message,256);
    strcat(message,name);
    strcat(message," - ");
    strcat(message,msg);
 
    cl = cs->getList();
    
    for (auto it = std::begin(cl); it!=std::end(cl); ++it) 
	{
		if (*it == clientSocket)
       		n = write(*it,message,strlen(message));
        if (n < 0) cout << "ERROR writing to socket" << endl;
    }

    cout << message << endl;
}

void broadcast(char msg[256], char name[256], ChatServer* cs) {
    char message[256];
    int n;
    vector<int> cl;

    bzero(message,256);
    strcat(message,name);
    strcat(message," - ");
    strcat(message,msg);
 
    cl = cs->getList();
    
    for (auto it = std::begin(cl); it!=std::end(cl); ++it) {
		n = write(*it,message,strlen(message));
        if (n < 0) cout << "ERROR writing to socket" << endl;
    }

    cout << message << endl;
}

