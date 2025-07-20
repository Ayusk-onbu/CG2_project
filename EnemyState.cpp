#include "EnemyState.h"
#include "Enemy.h"

void EnemyStateApproach::UpDate(Enemy* enemy, Vector3* pos, Vector3* rotation) {
	pos->z -= kMoveSpeed_;
	if (pos->z < 0.0f) {
		enemy->ChangeState(new EnemyStateLeave());
	}
}

void EnemyStateLeave::UpDate(Enemy* enemy, Vector3* pos, Vector3* rotation) {
	*pos += kLeaveSpeed_;
	rotation->y += Deg2Rad(1);
}