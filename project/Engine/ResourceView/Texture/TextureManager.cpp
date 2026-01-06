#include "TextureManager.h"
#include "Trigonometric.h"

std::unique_ptr<TextureManager> TextureManager::instance_ = nullptr;

void TextureManager::Initialize(Fngine& fngine) {
	p_fngine_ = &fngine;
	textureCount_ = 0;
}

std::string TextureManager::LoadTexture(const std::string& filename, const std::string& filePath) {
	// 1, ここですでにないか確認し、あったらそれを返す
	// [ 登録する名前を拡張子ごと受け取る ](例) rei.png
	std::string name = filename;
	// [ 登録する名前から拡張子を外す ](例) rei
	remove_extension_in_place(name);
	// [ すでに登録されているか確認する ]
	if (textures_.find(name) != textures_.end()) {
		// すでに登録されていることを確認出来たのでその名前を返す
		return name;
	}

	// 2, まだ登録されていないことを確認したので手続きを行う
	// [ 最大数を超えないかを確認する ]
	if (textureCount_ >= textureMax_) {
		// 超えていたら最初のキーを返す
		return textures_.begin()->first;
	}
	// [ 値を更新する ]
	textureCount_++;
	// 作成
	std::unique_ptr<Texture>texture = std::make_unique<Texture>();
	// [ ファイルパスを作成する ]
	std::string path = filePath + "/" + filename;
	// [ Textureを作成 ]
	texture->Initialize(p_fngine_->GetD3D12System(), p_fngine_->GetSRV(), path, textureCount_);
	// [ 登録 ]
	textures_.emplace(name, std::move(texture));

	// 3, 登録完了したのでカギを渡す
	return name;
}

Texture& TextureManager::GetTexture(const std::string& name) {
	auto it = textures_.find(name);
	if (it == textures_.end()) {
		
	}
	return *it->second.get();
}