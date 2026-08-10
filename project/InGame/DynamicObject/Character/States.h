#pragma once
#include <memory>

class Character;

template <typename TState>
class StateMachine {
public:
	StateMachine() = default;

	void Update(float deltaTime) {
		if (currentState_) {
			currentState_->Update(deltaTime);
		}
	}

	template <typename TConcreteState, typename... Args>
	void ChangeState(Args&&... args) {
		if (currentState_) {
			currentState_->Exit();
		}
		auto newState = std::make_unique<TConcreteState>();
		newState->Enter(std::forward<Args>(args)...);
		currentState_ = std::move(newState);
	}

	TState* GetCurrentState() const { return currentState_.get(); }

private:
	std::unique_ptr<TState> currentState_ = nullptr;
};

namespace CharacterState {
	class Base {
	public:
		virtual ~Base() = default;
	public:
		virtual void Enter() {}
		virtual void Update(float deltaTime) {}
		virtual void Exit() {}
	public:
		void SetInfo(Character* character, Base* parent = nullptr) { character_ = character; parentState_ = parent; }
		void SetParentState(Base* parent) { parentState_ = parent; }
	protected:
		Character* character_ = nullptr;
		Base* parentState_ = nullptr;
	};

	namespace Movement {
		// =============
		// 【 親State 】
		// =============
		// 地上にいるときの共通処理
		class GroundBase : public Base {
		public:
			~GroundBase()override = default;
		public:
			void Enter() override;
			void Update(float deltaTime) override;
			void Exit() override;

		};
		// 空中にいるときの共通処理
		class AirBase : public Base {
		public:
			~AirBase()override = default;
		public:
			void Enter() override;
			void Update(float deltaTime) override;
			void Exit() override;
		};
		// 移動制限状態（例: スタン、拘束、特殊なアクションなど）
		class Restricted : public Base {
		public:
			~Restricted()override = default;
		public:
			void Enter() override;
			void Update(float deltaTime) override;
			void Exit() override;
		};

		// ===================
		// 【 子State：Ground 】
		// ===================

		class Idle : public Base {
		public:
			~Idle()override = default;
		public:
			void Enter() override;
			void Update(float deltaTime) override;
			void Exit() override;
		};

		class Walk : public Base {
		public:
			~Walk()override = default;
		public:
			void Enter() override;
			void Update(float deltaTime) override;
			void Exit() override;
		};
	}
}

class ActionStateBase {
public:
	virtual ~ActionStateBase() = default;

	virtual void Enter() {}
	virtual void Update(float deltaTime) {}
	virtual void Exit() {}

	// ★ 移動を許可するか（大技や硬直中は false）
	virtual bool IsMovementAllowed() const { return true; }

	// ★ 移動速度への補正（例: ガード中 0.5倍速）
	virtual float GetSpeedMultiplier() const { return 1.0f; }

protected:
	Character* owner_ = nullptr;
};