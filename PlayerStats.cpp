#include "PlayerStats.h"

void PlayerStats::Initialize() {
	hp_ = 1.0f;          // Initial health points
	maxHp_ = 1.0f;      // Maximum health points
	strength_ = 1.0f;    // Initial physical attack power
	defense_ = 1.0f;      // Initial defense value
	agility_ = 1.0f;      // Initial speed of movement
	luck_ = 1.0f;         // Initial luck value
	all_ = 1.0f; // Total stats
}