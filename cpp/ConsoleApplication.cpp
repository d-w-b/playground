#include "pch.h"
#include "main.h"

int main()
{
	Server server;

	server.Init();
	InitThreads(&server);
	
	server.Join();
	server.Close();
    return 0;
}


DWORD ProcessIoCompletion(LPVOID param)
{
	Server* server		= static_cast<Server*>(param);
	
	//함수 Lookup 배열
	void (*task[])(Session*, DWORD) = {
			DoReceive,
			DoSend,
			DoAccept,
	};

	while( server->Running() )
	{
		Sleep(1);

		HANDLE				iocpHandle	= server->GetHandle();
		DWORD				dwBytes		= 0;
		ULONG_PTR			key			= 0;
		SessionContext*		ctx			= nullptr;
		
		if (GetQueuedCompletionStatus((HANDLE)iocpHandle, &dwBytes, (PULONG_PTR)&key, (LPOVERLAPPED*)&ctx, INFINITE))
		{
			Session* session = ctx->_owner;
			uint8 type = static_cast<uint8>(ctx->_type);
			task[type](session, dwBytes);
		}
		else
		{
			DWORD error = WSAGetLastError();
			std::cout << "ERROR: " << error << endl;

			if (error == 64)
			{
				cout << "Session Closed" << endl;
				break;
			}
		}

	}

	return 0;
}


void DoAccept(Session* session, DWORD dwBytes)
{
	::setsockopt(session->_socket, SOL_SOCKET, SO_UPDATE_ACCEPT_CONTEXT, (char*)&session->_listener, sizeof(SOCKET));
	cout << "ACCEPTED" << endl;
	session->RegisterInitialSend();
	session->RegisterRecv();
	
}


void DoReceive(Session* session, DWORD dwBytes)
{
	
	ServerService* service = session->GetOwner();
	
	PlayerData* data = new PlayerData();
	::memcpy(&data->id,			session->_recvBuffer		, 4);
	::memcpy(&data->x,			session->_recvBuffer + 4	, 4);
	::memcpy(&data->y,			session->_recvBuffer + 8	, 4);
	::memcpy(&data->state,		session->_recvBuffer + 12	, 4);
	::memcpy(&data->index,		session->_recvBuffer + 16	, 4);
	::memcpy(&data->direction,  session->_recvBuffer + 20	, 4);
	
	cout << dwBytes << " bytes received from id: "<< data->id << " " << data->x << "," << data->y << " STATE: " << data->state << endl;
	
	service->Broadcast(data);
	session->RegisterRecv();
}

void DoSend(Session* session, DWORD dwBytes)
{
	cout << dwBytes << " bytes sent to id: " << session->GetId() << endl;
}

void InitThreads(LPVOID params)
{

	Server* server = static_cast<Server*>(params);

	for (int i = 0; i < NUM_THREADS; i++)
	{
		DWORD tid = 0;
		HANDLE hThread = NULL;
		hThread = CreateThread(NULL, 0, ProcessIoCompletion, (LPVOID)server, NULL, &tid);
		assert(hThread != NULL);

		server->AddWorker(hThread);
	}

	return;
}