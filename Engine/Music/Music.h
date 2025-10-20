#pragma once
#include "BGM.h"
#include "SE.h"

#include <vector>

class Music
{
public:
	Music() { bgm_ = {}; }
	//~Music() { bgm_.Unload(); }

	void Initialize();

	void UnLoad();

	BGM& GetBGM() { return bgm_; };
private:
	BGM bgm_;
};

