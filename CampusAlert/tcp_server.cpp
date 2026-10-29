#include <iostream>
#include <winsock2.h>
#include <string.h>

using namespace std;

int main()
{
    WSADATA wsa;
    SOCKET serverSocket, clientSocket;
    sockaddr_in server, client;
    int clientSize;
    char buffer[1024];

    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
    {
        cout << "Winsock initialization failed." << endl;
        return 1;
    }

    serverSocket = socket(AF_INET, SOCK_STREAM, 0);

    if (serverSocket == INVALID_SOCKET)
    {
        cout << "Socket creation failed." << endl;
        WSACleanup();
        return 1;
    }

    server.sin_family = AF_INET;
    server.sin_addr.s_addr = inet_addr("127.0.0.1");
    server.sin_port = htons(5000);

    if (bind(serverSocket, (sockaddr *)&server,
             sizeof(server)) == SOCKET_ERROR)
    {
        cout << "Bind failed. Port 5000 may be in use." << endl;
        closesocket(serverSocket);
        WSACleanup();
        return 1;
    }

    listen(serverSocket, 5);

    cout << "Campus Alert TCP Server is running..." << endl;
    cout << "Waiting for a client..." << endl;

    clientSize = sizeof(client);
    clientSocket = accept(serverSocket,
                          (sockaddr *)&client, &clientSize);

    if (clientSocket != INVALID_SOCKET)
    {
        int received = recv(clientSocket, buffer,
                            sizeof(buffer) - 1, 0);

        if (received > 0)
        {
            buffer[received] = '\0';
            cout << "Client says: " << buffer << endl;

            const char *reply =
                "Server received your campus alert!";
            send(clientSocket, reply, strlen(reply), 0);
        }

        closesocket(clientSocket);
    }

    closesocket(serverSocket);
    WSACleanup();

    cout << "Server stopped." << endl;
    return 0;
}