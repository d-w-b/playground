#pragma once
#include "pch.h"
#include "SessionContext.h"

using namespace std;


class ServerService
{
public:
    ServerService();
    ~ServerService();

    void Init();
    void InitIocp();
    void Accept();
    void Close();
    void Broadcast(PlayerData* data);

    vector<HANDLE>& GetWorkers();
    void AddWorker(HANDLE hthread);
    void CloseWorkers();
    void CloseSessions();


    HANDLE GetHandle();


    HANDLE              _iocpHandle               = NULL;
    SOCKET              _listenSocket             = INVALID_SOCKET;
    LPFN_ACCEPTEX       _AcceptEx                 = NULL;
    LPFN_CONNECTEX      _ConnectEx                = NULL;
    LPFN_DISCONNECTEX   _DisconnectEx             = NULL;
    sockaddr_in         _addr;
    vector<Session*>    _sessions;
    vector<HANDLE>      _workers;
    CRITICAL_SECTION    cs;
};



