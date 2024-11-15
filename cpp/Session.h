#pragma once
#include "SessionContext.h"
#include "ServerService.h"
class Session
{
public:
	Session(HANDLE iocpHandle, SOCKET listener);
	~Session();

	void Init();
	void RegisterInitialSend();
	void RegisterSend(PlayerData* data);
	void RegisterRecv();
	void Close();

	void SetOwner(ServerService* owner);
	ServerService* GetOwner();

	void SetId(uint32 id);
	uint32 GetId();

public:
	uint32			_id					= 0;
	ServerService*	_owner				= nullptr;
	SOCKET			_socket				= NULL;
	SOCKET			_listener			= NULL;
	HANDLE			_iocpHandle			= NULL;

	BYTE			_recvBuffer[1024]	= { 0 };
	BYTE			_sendBuffer[1024]	= { 0 };

	SessionContext* _recvContext		= new SessionContext(EventType::Recv);
	SessionContext* _sendContext		= new SessionContext(EventType::Send);
	SessionContext* _acceptContext		= new SessionContext(EventType::Accept);

};

