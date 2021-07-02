#include <iostream>
#include <WS2tcpip.h>

#pragma comment (lib, "ws2_32.lib")

using namespace std;

void main() {

	//initilaze winsock
	WSADATA wsData;
	WORD ver = MAKEWORD(2, 2);

	int wsOk = WSAStartup(ver, &wsData);
	if (wsOk != 0)
	{
		cerr << "Can't initilaze winsock! Quitting" << endl;
		return;
	}
	//create a socket
	SOCKET listening = socket(AF_INET, SOCK_STREAM, 0);
	if (listening == INVALID_SOCKET)
	{
		cerr << "Can't create a socket! Quitting" << endl; 
			return;
	}

	// Bind the ip address and port to a socket
	sockaddr_in hint;
	hint.sin_family = AF_INET;
	hint.sin_port = htons(54000);
	hint.sin_addr.S_un.S_addr = INADDR_ANY; // Could also use inet_pton
	
	bind(listening, (sockaddr*)&hint, sizeof(hint));

	// Tell windosck the socket is for listenimng
	listen(listening, SOMAXCONN);
	//wait for connection

	sockaddr_in client;
	int clientSize = sizeof(client);

	SOCKET clientSocket = accept(listening, (sockaddr*)&client, &clientSize);
	char host[NI_MAXHOST]; //Client's remote name
	char service[NI_MAXHOST]; // Servive (i.e. port) the client is connect om

	ZeroMemory(host, NI_MAXHOST); // same as memset(host, 0, NI_MAXHOST)
	ZeroMemory(service, NI_MAXHOST);

	if (getnameinfo((sockaddr*)&client, sizeof(client), host, NI_MAXHOST, service, NI_MAXSERV, 0) == 0)
	{
		cout << host << " connected on port " << service << endl;
	}
	else
	{
		inet_ntop(AF_INET, &client.sin_addr, host, NI_MAXHOST);
		cout << host << " connected on port " <<
			ntohs(client.sin_port) << endl;
	}

	// Close listening socket

	closesocket(listening);


	// While loop: accept and echo message back to client
	char buff[4096];

	while (true)
	{
		ZeroMemory(buff, 4096);

		// Wait for client to send data
		int bytesReceived = recv(clientSocket, buff, 4096, 0);
		if (bytesReceived == SOCKET_ERROR)
		{
			cerr << "Error in recv(). Quitting" << endl;
			break;
		}
		if (bytesReceived == 0)
		{
			cout << "Client disconnected" << endl;
			break;
		}
		cout << string(buff, 0, bytesReceived);
		// Echo message back to client
		send(clientSocket, buff, bytesReceived + 1, 0);
	}

	// Close the socket
	closesocket(clientSocket);
	// Cleanup winsocket
	WSACleanup();

}