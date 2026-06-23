#pragma once
#include <memory>

template <typename T>
class ISingleton {
public:
	static T* GetInstance() {
		if (instance_ == nullptr) {
			instance_ = std::unique_ptr<T>(new T());
		}
		return instance_.get();
	}

	static void ReleaseInstance() {
		instance_.reset();
	}

protected:
	ISingleton() = default;
	virtual ~ISingleton() = default;

private:
	// inline static を使うことで、.cpp 側での「実体定義」が不要
	inline static std::unique_ptr<T> instance_ = nullptr;
};