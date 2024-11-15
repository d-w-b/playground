#pragma once
#include "pch.h"

class Session;

enum class EventType
{
	Recv,
	Send,
	Accept,
	Connect,
	None,
};



class SessionContext : public OVERLAPPED
{
public:
	SessionContext(EventType type);
	~SessionContext();

	void Init();
	void Close();

public:
	EventType _type = EventType::None;
	Session* _owner = nullptr;
};

