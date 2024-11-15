#include "pch.h"
#include "Session.h"

Session::Session(HANDLE iocpHandle, SOCKET listener): _listener(listener)
{
	_socket = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, NULL, WSA_FLAG_OVERLAPPED);
	assert(_socket != INVALID_SOCKET);
	assert(iocpHandle != INVALID_HANDLE_VALUE);
	assert(CreateIoCompletionPort((HANDLE)_socket, iocpHandle, 0, 0) != NULL);

	_recvContext->_owner = this;
	_sendContext->_owner = this;
	_acceptContext->_owner = this;
}

Session::~Session()
{
	closesocket(_socket);
}

void Session::Init()
{
	ZeroMemory(_recvBuffer, sizeof(_recvBuffer));
	ZeroMemory(_sendBuffer, sizeof(_sendBuffer));
	_recvContext->Init();
	_sendContext->Init();
	_acceptContext->Init();

	SetId( reinterpret_cast<int>(this) );
}

void Session::RegisterInitialSend()
{
	DWORD flags = 0;
	WSABUF sendbuf;
	_sendContext->Init();

	::memcpy(_sendBuffer, &_id, sizeof(_id));
	sendbuf.len = sizeof(_sendBuffer);
	sendbuf.buf = (char*)_sendBuffer;

	WSASend(_socket, &sendbuf, 1, NULL, NULL, (LPOVERLAPPED)_sendContext, NULL);
}

void Session::RegisterSend(PlayerData* data)
{
	DWORD flags = 0;
	WSABUF sendbuf;

	_sendContext->Init();

	::memcpy(_sendBuffer, data, 24);

	sendbuf.len = sizeof(_sendBuffer);
	sendbuf.buf = (char*)_sendBuffer;
	WSASend(_socket, &sendbuf, 1, NULL, NULL, (LPOVERLAPPED)_sendContext, NULL);
}

void Session::RegisterRecv()
{
	DWORD flags = 0;
	WSABUF wsaBuf;
	_recvContext->Init();
	
	wsaBuf.buf = (char*)_recvBuffer;
	wsaBuf.len = sizeof(_recvBuffer);
	WSARecv(_socket, (LPWSABUF)&wsaBuf, 1, 0, &flags, _recvContext, NULL);
}

void Session::Close()
{
	delete _recvContext;
	delete _sendContext;

	closesocket(_socket);
}

void Session::SetOwner(ServerService* owner)
{
	_owner = owner;
}

ServerService* Session::GetOwner()
{
	return _owner;
}

void Session::SetId(uint32 id)
{
	_id = id;
}

uint32 Session::GetId()
{
	return _id;
}

