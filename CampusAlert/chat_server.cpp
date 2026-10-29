#include <iostream>
#include <winsock2.h>
#include <windows.h>
#include <string.h>

using namespace std;

const int MAX_CLIENTS = 10;

SOCKET clients[MAX_CLIENTS];
CRITICAL_SECTION clientsLock;

struct ClientInfo
{
    SOCKET socket;
};

void broadcastMessage(const char *message, int length)
{
    EnterCriticalSection(&clientsLock);

    for (int i = 0; i < MAX_CLIENTS; i++)
    {
        if (clients[i] != INVALID_SOCKET)
        {
            send(clients[i], message, length, 0);
        }
    }

    LeaveCriticalSection(&clientsLock);
}

DWORD WINAPI handleClient(LPVOID parameter)
{
    ClientInfo *info = (ClientInfo *)parameter;
    SOCKET clientSocket = info->socket;
    delete info;

    char buffer[1024];
    int received;

    while ((received = recv(clientSocket, buffer,
                            sizeof(buffer), 0)) > 0)
    {
        cout << "Message received: ";
        cout.write(buffer, received);
        cout << endl;

        broadcastMessage(buffer, received);
    }

    EnterCriticalSection(&clientsLock);

    for (int i = 0; i < MAX_CLIENTS; i++)
    {
        if (clients[i] == clientSocket)
        {
            clients[i] = INVALID_SOCKET;
            break;
        }
    }

    closesocket(clientSocket);

    LeaveCriticalSection(&clientsLock);

    cout << "A client disconnected." << endl;

    return 0;
}

int main()
{
    WSADATA wsa;
    SOCKET serverSocket;
    sockaddr_in server, client;
    int clientSize;

    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
    {
        cout << "Winsock initialization failed." << endl;
        return 1;
    }

    InitializeCriticalSection(&clientsLock);

    for (int i = 0; i < MAX_CLIENTS; i++)
        clients[i] = INVALID_SOCKET;

    serverSocket = socket(AF_INET, SOCK_STREAM, 0);

    if (serverSocket == INVALID_SOCKET)
    {
        cout << "Socket creation failed." << endl;
        DeleteCriticalSection(&clientsLock);
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
        DeleteCriticalSection(&clientsLock);
        WSACleanup();
        return 1;
    }

    if (listen(serverSocket, 5) == SOCKET_ERROR)
    {
        cout << "Listen failed." << endl;
        closesocket(serverSocket);
        DeleteCriticalSection(&clientsLock);
        WSACleanup();
        return 1;
    }

    cout << "=== Campus Chat Server ===" << endl;
    cout << "Waiting for clients on port 5000..." << endl;

    while (true)
    {
        clientSize = sizeof(client);

        SOCKET clientSocket = accept(serverSocket,
            (sockaddr *)&client, &clientSize);

        if (clientSocket == INVALID_SOCKET)
        {
            cout << "Accept failed." << endl;
            continue;
        }

        int slot = -1;

        EnterCriticalSection(&clientsLock);

        for (int i = 0; i < MAX_CLIENTS; i++)
        {
            if (clients[i] == INVALID_SOCKET)
            {
                clients[i] = clientSocket;
                slot = i;
                break;
            }
        }

        LeaveCriticalSection(&clientsLock);

        if (slot == -1)
        {
            const char *fullMessage =
                "Server is full. Try again later.\n";

            send(clientSocket, fullMessage,
                 strlen(fullMessage), 0);

            closesocket(clientSocket);
            continue;
        }

        ClientInfo *info = new ClientInfo;
        info->socket = clientSocket;

        HANDLE threadHandle = CreateThread(
            NULL, 0, handleClient, info, 0, NULL);

        if (threadHandle == NULL)
        {
            EnterCriticalSection(&clientsLock);
            clients[slot] = INVALID_SOCKET;
            closesocket(clientSocket);
            LeaveCriticalSection(&clientsLock);

            delete info;
            cout << "Could not create client thread." << endl;
        }
        else
        {
            CloseHandle(threadHandle);
            cout << "A client connected." << endl;
        }
    }

    // The server runs continuously until you stop it.
    closesocket(serverSocket);
    DeleteCriticalSection(&clientsLock);
    WSACleanup();

    return 0;
}