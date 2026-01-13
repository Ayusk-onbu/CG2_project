#include "Music.h"
#include "Trigonometric.h"

std::unique_ptr<Music> Music::instance_ = nullptr;

void Music::Initialize() {
	Audio::Initialize();
}

void Music::UnLoad() {
	Audio::Finalize();
	if(bgm_)bgm_->Unload();
	for (auto& se : SEs_) {
		se.second->Unload();
	}
}

void Music::PlaySE(const std::string& name) {
	auto it = SEs_.find(name);
	if (it == SEs_.end()) {
		return;
	}
	it->second->Play();
}

std::string Music::LoadSE(const std::string& filename, const std::string& path) {
	// 1, ここですでにないか確認し、あったらそれを返す
	// [ 登録する名前を拡張子ごと受け取る ](例) rei.png
	std::string name = filename;
	// [ 登録する名前から拡張子を外す ](例) rei
	remove_extension_in_place(name);
	// [ すでに登録されているか確認する ]
	if (SEs_.find(name) != SEs_.end()) {
		// すでに登録されていることを確認出来たのでその名前を返す
		return name;
	}

	// 2, まだ登録されていないことを確認したので手続きを行う
	// 作成
	std::unique_ptr<SE>se = std::make_unique<SE>();
	// [ ファイルパスを作成する ]
	std::string filepath = path + "/" + filename;
	// [ Textureを作成 ]
	se->LoadWAVE(filepath);
	// [ 登録 ]
	SEs_.emplace(name, std::move(se));

	// 3, 登録完了したのでカギを渡す
	return name;
}