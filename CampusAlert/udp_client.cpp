#include <iostream>
#include <winsock2.h>
#include <string.h>

using namespace std;

int main()
{
    WSADATA wsa;
    SOCKET clientSocket;
    sockaddr_in server;
    int serverSize;
    char buffer[1024];

    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
    {
        cout << "Winsock initialization failed." << endl;
        return 1;
    }

    clientSocket = socket(AF_INET, SOCK_DGRAM, 0);

    if (clientSocket == INVALID_SOCKET)
    {
        cout << "UDP socket creation failed." << endl;
        WSACleanup();
        return 1;
    }

    server.sin_family = AF_INET;
    server.sin_addr.s_addr = inet_addr("127.0.0.1");
    server.sin_port = htons(5001);

    const char *message =
        "QUICK ALERT: A student ID card was found near the lab.";

    sendto(clientSocket, message, strlen(message), 0,
           (sockaddr *)&server, sizeof(server));

    cout << "Quick alert sent to the server!" << endl;
    cout << "Alert: " << message << endl;

    serverSize = sizeof(server);

    int received = recvfrom(clientSocket, buffer,
                            sizeof(buffer) - 1, 0,
                            (sockaddr *)&server, &serverSize);

    if (received != SOCKET_ERROR)
    {
        buffer[received] = '\0';
        cout << "Server reply: " << buffer << endl;
    }
    else
    {
        cout << "No reply received from server." << endl;
    }

    closesocket(clientSocket);
    WSACleanup();

    return 0;
}