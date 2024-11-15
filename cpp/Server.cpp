#include "pch.h"
#include "Server.h"

Server::Server()
{

}

Server::~Server()
{
	delete _service;
}

void Server::Init()
{
	WSADATA wsaData;
	assert(WSAStartup(MAKEWORD(2, 2), &wsaData) == 0);

	_service->Init();
	_service->Accept();
}

void Server::Broadcast()
{
	//_service->Broadcast();
}

HANDLE Server::GetHandle()
{
	return _service->GetHandle();
}

bool Server::Running()
{
	if (_nThreads == 0)
		_bRunning = false;

	return _bRunning;
}

void Server::AddWorker(HANDLE hthread)
{
	_service->AddWorker(hthread);
}

vector<HANDLE>& Server::GetWorkers()
{
	return _service->GetWorkers();
}



void Server::Join()
{
	vector<HANDLE> threads = GetWorkers();
	for(int i=0; i<NUM_THREADS; i++)
		WaitForMultipleObjects(NUM_THREADS, threads.data(), true, INFINITE);
}

void Server::Close()
{
	_bRunning = false;
	_service->Close();
}
