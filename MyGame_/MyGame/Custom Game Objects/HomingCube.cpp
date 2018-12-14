#include "HomingCube.h"
#include "Engine/Math/sVector.h"
eae6320::HomingCube::~HomingCube(){}

void eae6320::HomingCube::EventTick(const float i_secondCountToIntegrate) {
	Math::sVector moveDir = m_target->m_State.position - m_State.position;
	if (moveDir.GetLength() > 0.01f) {
		moveDir.Normalize();
		m_State.acceleration = 2 * moveDir;
	}
}