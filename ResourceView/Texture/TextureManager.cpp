#include "TextureManager.h"

std::unique_ptr<TextureManager> TextureManager::instance_ = nullptr;

void TextureManager::Initialize(Fngine& fngine) {
	p_fngine_ = &fngine;
	textures_.resize(textureMax_);
	textureCount_ = 0;
}

int TextureManager::LoadTexture(const char* filePath) {
	if (textureCount_ >= textureMax_) {
		return textureCount_;
	}
	textureCount_++;
	textures_[textureCount_].Initialize(p_fngine_->GetD3D12System(), p_fngine_->GetSRV(), filePath, textureCount_);
	return textureCount_;
}