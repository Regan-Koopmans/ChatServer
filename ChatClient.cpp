#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <stdlib.h>
#include <iostream>
#include <unistd.h>
#include <string.h>
#include <string>
#include <thread>

using namespace std;

bool running;

void keepReading(int clientSocket)
{
	char buffer[256];
	while (running)
	{
		bzero(buffer,256);
		read(clientSocket,buffer,256);
		cout << buffer << endl;
	}
}

void keepReadingOnce(int clientSocket)
{
	char buffer[256];
		bzero(buffer,256);
		read(clientSocket,buffer,256);
		cout << buffer << endl;
}


int main(int argc, char* argv[])
{
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

	if (argc < 3)
	{
		cout << "Incorrect number of arguments." << endl;
		cout << "Usage is: " << argv[0] << " <hostname> <port>" << endl;
	}
	else
	{
		serverPort = atoi(argv[2]);
		clientSocket = socket(PF_INET,SOCK_STREAM,IPPROTO_TCP);
		if (clientSocket < 0)
		{
			cout << "Opening a socket failed." << endl;
		}
		else 
		{
			server=gethostbyname(argv[1]);
			if (server == NULL)
				cout << "No such host." << endl;
			else 
			{
				bzero((char *) &serverAddress, sizeof(serverAddress));
				serverAddress.sin_family = AF_INET;
				bcopy((char *)server->h_addr,
					  (char *)&serverAddress.sin_addr.s_addr,
						server->h_length);
				serverAddress.sin_port = htons(serverPort);

				if (connect(clientSocket, (struct sockaddr*) &serverAddress, sizeof(serverAddress)) < 0){
					cout << "Connection failed" << endl;
				}
				else 
				{
					char response[256];
					running = true;
					thread myThread(keepReading,clientSocket);
					while (!(strcmp(buffer,"ZZZ") == 0))
					{
						bzero(buffer,256);
						cin.getline(response,256);
						//if (strcmp(response,"ZZZ") == 0)
						write(clientSocket,response,strlen(response)); 
						if (strcmp(response,"ZZZ") == 0)
						{
							running = false;
							exit(0);
						}	
					}
					myThread.join();
				}
			}
			close(clientSocket);
		}
	}
	return 0;
}



