#pragma once
#include "Fence.h"

class TachyonSync
{
public:
	Fence& GetCGPU() { return CGPU_; }
private:
	Fence CGPU_;
};

