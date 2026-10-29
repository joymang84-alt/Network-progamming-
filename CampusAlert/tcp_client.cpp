#include <iostream>
#include <winsock2.h>
#include <string.h>

using namespace std;

int main()
{
    WSADATA wsa;
    SOCKET clientSocket;
    sockaddr_in server;
    char buffer[1024];

    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
    {
        cout << "Winsock initialization failed." << endl;
        return 1;
    }

    clientSocket = socket(AF_INET, SOCK_STREAM, 0);

    if (clientSocket == INVALID_SOCKET)
    {
        cout << "Socket creation failed." << endl;
        WSACleanup();
        return 1;
    }

    server.sin_family = AF_INET;
    server.sin_addr.s_addr = inet_addr("127.0.0.1");
    server.sin_port = htons(5000);

    if (connect(clientSocket, (sockaddr *)&server,
                sizeof(server)) == SOCKET_ERROR)
    {
        cout << "Could not connect to the server." << endl;
        closesocket(clientSocket);
        WSACleanup();
        return 1;
    }

    cout << "Connected to Campus Alert Server!" << endl;

    const char *message =
        "ALERT: A black wallet was found near the library.";

    send(clientSocket, message, strlen(message), 0);

    cout << "Message sent: " << message << endl;

    int received = recv(clientSocket, buffer,
                        sizeof(buffer) - 1, 0);

    if (received > 0)
    {
        buffer[received] = '\0';
        cout << "Server reply: " << buffer << endl;
    }

    closesocket(clientSocket);
    WSACleanup();

    return 0;
}