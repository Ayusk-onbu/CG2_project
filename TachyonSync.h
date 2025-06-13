#pragma once
#include "Fence.h"

class TachyonSync
{
public:
	static Fence& GetCGPU() { return CGPU_; }
private:
	static Fence CGPU_;
};

