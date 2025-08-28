#pragma once
#include <cstdint>


class PlayerStats
{
public:
	void Initialize();
public:
	float GetHp() const { return hp_; }
	float GetMaxHp() const { return maxHp_; }
	float GetStrength() const { return strength_; }
	float GetDefense() const { return defense_; }
	float GetAgility() const { return agility_; }
	float GetLuck() const { return luck_; }
	float GetAll() const { return all_; }

	void SetHp(float hp) { hp_ = hp; }
	void SetMaxHp(float maxHp) { maxHp_ = maxHp; }
	void SetStrength(float strength) { strength_ = strength; }
	void SetDefense(float defense) { defense_ = defense; }
	void SetAgility(float agility) { agility_ = agility; }
	void SetLuck(float luck) { luck_ = luck; }
	void SetAll(float all) { all_ = all; }
private:
	float hp_;// Current health points
	float maxHp_;// Maximum health points

	float strength_;// Physical attack power
	float defense_;
	float agility_;// Speed of movement
	float luck_;
	float all_;
};

