#include <iostream>
#include <winsock2.h>
#include <windows.h>
#include <string.h>

using namespace std;

DWORD WINAPI receiveMessages(LPVOID parameter)
{
    SOCKET *socketPointer = (SOCKET *)parameter;
    SOCKET clientSocket = *socketPointer;
    delete socketPointer;

    char buffer[1024];
    int received;

    while ((received = recv(clientSocket, buffer,
                            sizeof(buffer), 0)) > 0)
    {
        cout << "\n";
        cout.write(buffer, received);
        cout << "\n> " << flush;
    }

    return 0;
}

int main()
{
    WSADATA wsa;
    SOCKET clientSocket;
    sockaddr_in server;
    char message[1024];

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

    cout << "Connected to Campus Chat!" << endl;
    cout << "Type a message and press Enter." << endl;
    cout << "Type exit to leave the chat." << endl;

    SOCKET *socketPointer = new SOCKET;
    *socketPointer = clientSocket;

    HANDLE receiverThread = CreateThread(
        NULL, 0, receiveMessages, socketPointer, 0, NULL);

    if (receiverThread == NULL)
    {
        cout << "Could not start receiver thread." << endl;
        delete socketPointer;
        closesocket(clientSocket);
        WSACleanup();
        return 1;
    }

    while (true)
    {
        cout << "> " << flush;
        cin.getline(message, sizeof(message));

        if (cin.fail())
        {
            cin.clear();
            cin.ignore(10000, '\n');
            cout << "Message too long. Try again." << endl;
            continue;
        }

        if (strcmp(message, "exit") == 0)
            break;

        strcat(message, "\n");

        int length = strlen(message);

        if (send(clientSocket, message, length, 0)
            == SOCKET_ERROR)
        {
            cout << "Message could not be sent." << endl;
            break;
        }
    }

    shutdown(clientSocket, SD_BOTH);
    closesocket(clientSocket);

    WaitForSingleObject(receiverThread, INFINITE);
    CloseHandle(receiverThread);

    WSACleanup();

    cout << "You left the chat." << endl;

    return 0;
}