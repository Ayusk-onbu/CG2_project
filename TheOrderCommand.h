#pragma once
#include "CommandQueue.h"
#include "CommandList.h"

class TheOrderCommand
{
public:

	CommandQueue& GetQueue() { return queue_; }

	CommandList& GetList() { return list_; }

private:
	CommandQueue queue_;
	CommandList list_;
};

