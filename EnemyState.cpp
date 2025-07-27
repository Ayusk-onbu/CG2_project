#include "EnemyState.h"
#include "Enemy.h"

void EnemyStateApproach::UpDate(Enemy* enemy, Vector3* pos, Vector3* rotation) {
	if (!initialize_) {
		enemy->FireReset();
		initialize_ = true;
	}
	pos->z -= kMoveSpeed_;
	if (pos->z < 0.0f) {
		enemy->ChangeState(new EnemyStateLeave());
	}
}

void EnemyStateLeave::UpDate(Enemy* enemy, Vector3* pos, Vector3* rotation) {
	switch (enemy->GetMovePattern()) {
	case 0:
		*pos += kLeaveSpeed_;
		rotation->y += Deg2Rad(1);
		break;
	case 1:
		*pos += {-kLeaveSpeed_.x, kLeaveSpeed_.y, kLeaveSpeed_.z};
		rotation->y += Deg2Rad(-1);
		break;
	}
	
}