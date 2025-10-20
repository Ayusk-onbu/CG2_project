#pragma once
#include "Fngine.h"
#include "Texture.h"
#include <string>
#include <vector>

class TextureManager {
public:
	TextureManager() = default;
public:
	static TextureManager* GetInstance() {
		if (instance_ == nullptr) {
			instance_ = std::make_unique<TextureManager>();
		}
		return instance_.get();
	}
	static void ReleaseInstance() { instance_.reset(); }
public:
	void Initialize(Fngine& fngine);
	int LoadTexture(const char* filePath);
	Texture& GetTexture(int num) { return textures_[num]; }
private:
	static std::unique_ptr<TextureManager>instance_;
	Fngine* p_fngine_ = nullptr;
	std::vector<Texture>textures_;
	uint32_t textureCount_ = 0;
	uint32_t textureMax_ = 100;
};