#include <iostream>
#include <winsock2.h>

using namespace std;

int main()
{
    WSADATA wsa;
    SOCKET serverSocket;
    sockaddr_in server, client;
    int clientSize;
    char buffer[1024];

    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
    {
        cout << "Winsock initialization failed." << endl;
        return 1;
    }

    serverSocket = socket(AF_INET, SOCK_DGRAM, 0);

    if (serverSocket == INVALID_SOCKET)
    {
        cout << "UDP socket creation failed." << endl;
        WSACleanup();
        return 1;
    }

    server.sin_family = AF_INET;
    server.sin_addr.s_addr = inet_addr("127.0.0.1");
    server.sin_port = htons(5001);

    if (bind(serverSocket, (sockaddr *)&server,
             sizeof(server)) == SOCKET_ERROR)
    {
        cout << "UDP bind failed." << endl;
        closesocket(serverSocket);
        WSACleanup();
        return 1;
    }

    cout << "Campus Alert UDP Server is running..." << endl;
    cout << "Waiting for quick alerts..." << endl;

    clientSize = sizeof(client);

    int received = recvfrom(serverSocket, buffer,
                            sizeof(buffer) - 1, 0,
                            (sockaddr *)&client, &clientSize);

    if (received != SOCKET_ERROR)
    {
        buffer[received] = '\0';
        cout << "Quick alert received: " << buffer << endl;

        const char *reply = "UDP alert received!";
        sendto(serverSocket, reply, strlen(reply), 0,
               (sockaddr *)&client, clientSize);
    }
    else
    {
        cout << "Error receiving UDP alert." << endl;
    }

    closesocket(serverSocket);
    WSACleanup();

    return 0;
}