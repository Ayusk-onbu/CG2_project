#pragma once
#include "BGM.h"
#include "SE.h"

#include <unordered_map>

class Music
{
public:
	static Music* GetInstance() {
		if (instance_ == nullptr) {
			instance_ = std::make_unique<Music>();
		}
		return instance_.get();
	}
	void ReleaseInstance() { instance_.reset(); }
public:
	void Initialize();
	void UnLoad();
public:
	BGM& GetBGM() { return *bgm_.get(); }
	void PlaySE(const std::string& name);
	std::string LoadSE(const std::string& filename, const std::string& path);
private:
	static std::unique_ptr<Music>instance_;
	std::unique_ptr<BGM> bgm_;
	std::unordered_map<std::string, std::unique_ptr<SE>>SEs_;
};

