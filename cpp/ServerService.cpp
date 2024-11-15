#include "pch.h"
#include "ServerService.h"
#include "SessionContext.h"
#include "Session.h"

ServerService::ServerService()
{
}

ServerService::~ServerService()
{
}


void ServerService::Init()
{
	InitIocp();

	assert( bind(_listenSocket, (sockaddr*) &_addr, sizeof(_addr)) == 0 );
	assert( listen(_listenSocket, SOMAXCONN) == 0 );
	assert( CreateIoCompletionPort( (HANDLE)_listenSocket, _iocpHandle, 0, 0 ) != NULL ) ;
	
	InitializeCriticalSection(&cs);

	return;
}

void ServerService::InitIocp()
{
	_iocpHandle = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 0);
	assert(_iocpHandle != NULL);

	_listenSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	
	_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	_addr.sin_port = htons(PORT);
	_addr.sin_family = AF_INET;

	GUID acceptExGuid = WSAID_ACCEPTEX;
	GUID connectExGuid = WSAID_CONNECTEX;
	GUID disconnectGuid = WSAID_DISCONNECTEX;
	DWORD bytes = 0;

	WSAIoctl(_listenSocket, SIO_GET_EXTENSION_FUNCTION_POINTER, &acceptExGuid, sizeof(acceptExGuid), &_AcceptEx, sizeof(_AcceptEx), &bytes, NULL, NULL);
	WSAIoctl(_listenSocket, SIO_GET_EXTENSION_FUNCTION_POINTER, &connectExGuid, sizeof(connectExGuid), &_ConnectEx, sizeof(_ConnectEx), &bytes, NULL, NULL);
	WSAIoctl(_listenSocket, SIO_GET_EXTENSION_FUNCTION_POINTER, &disconnectGuid, sizeof(disconnectGuid), &_DisconnectEx, sizeof(_DisconnectEx), &bytes, NULL, NULL);
}

void ServerService::Accept()
{
	for (int i = 0; i < NUM_MAX_SESSIONS; i++) {
		Session* session = new Session(_iocpHandle, _listenSocket);

		session->Init();
		session->SetOwner(this);

		bool ret = _AcceptEx(
			_listenSocket,
			session->_socket,
			&session->_recvBuffer,
			sizeof(session->_recvBuffer) - sizeof(sockaddr_in)*2 -32,
			sizeof(sockaddr_in) + 16,
			sizeof(sockaddr_in) + 16,
			NULL,
			(LPOVERLAPPED)session->_acceptContext
		);

		if (ret == false)
		{
			int err = WSAGetLastError();
			if(err != WSA_IO_PENDING)
				cout << "ACCEPTEX ERROR:: " << WSAGetLastError() << endl;
		}

		_sessions.push_back(session);
	}
}

void ServerService::CloseWorkers()
{
	vector<HANDLE> threads = GetWorkers();
	for (HANDLE t : threads)
		CloseHandle(t);
}

void ServerService::CloseSessions()
{
	for (Session* session : _sessions)
	{
		session->Close();
	}
}

void ServerService::Close()
{
	closesocket(_listenSocket);
	CloseHandle(_iocpHandle);
	CloseWorkers();
	CloseSessions();
}


void ServerService::Broadcast(PlayerData* data)
{
	for (Session* session : _sessions)
	{
		if(data->id != session->GetId())
			session->RegisterSend(data);
	}

	delete data;
		
}

vector<HANDLE>& ServerService::GetWorkers()
{
	return _workers;
}

void ServerService::AddWorker(HANDLE hthread)
{
	_workers.push_back(hthread);
}



HANDLE ServerService::GetHandle()
{
	return _iocpHandle;
}
