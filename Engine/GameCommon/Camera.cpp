#include "Camera.h"
#include "Engine/UserInput/UserInput.h"
eae6320::GameCommon::Camera::Camera(Math::sVector i_position, Math::cQuaternion i_orientation, const float i_verticalFieldOfView_inRadians, const float i_aspectRatio, const float i_z_nearPlane, const float i_z_farPlane) {
	m_State.position = i_position;
	m_State.orientation = i_orientation;

	m_verticalFieldOfView_inRadians = i_verticalFieldOfView_inRadians;
	m_aspectRatio = i_aspectRatio;
	m_z_nearPlane = i_z_nearPlane;
	m_z_farPlane = i_z_farPlane;
}


void eae6320::GameCommon::Camera::Initialize(Math::sVector i_position, Math::cQuaternion i_orientation, const float i_verticalFieldOfView_inRadians, const float i_aspectRatio, const float i_z_nearPlane, const float i_z_farPlane) {
	m_State.position = i_position;
	m_State.orientation = i_orientation;

	m_verticalFieldOfView_inRadians = i_verticalFieldOfView_inRadians;
	m_aspectRatio = i_aspectRatio;
	m_z_nearPlane = i_z_nearPlane;
	m_z_farPlane = i_z_farPlane;
}

void eae6320::GameCommon::Camera::UpdateState(const float i_secondCountToIntegrate) {
	m_State.Update(i_secondCountToIntegrate);
}

eae6320::Math::cMatrix_transformation eae6320::GameCommon::Camera::GetWorldToCameraMat() {
	return Math::cMatrix_transformation::CreateWorldToCameraTransform(m_State.orientation, m_State.position);
}

eae6320::Math::cMatrix_transformation eae6320::GameCommon::Camera::GetCameraToProjectedMat() {
	return Math::cMatrix_transformation::CreateCameraToProjectedTransform_perspective(m_verticalFieldOfView_inRadians, m_aspectRatio, m_z_nearPlane, m_z_farPlane);
}

void eae6320::GameCommon::Camera::UpdateCameraBasedOnInput() {
	m_State.velocity = Math::sVector(0, 0, 0);
	m_State.axis_X_velocity = 0.0f;
	m_State.axis_Y_velocity = 0.0f;
	m_State.axis_Z_velocity = 0.0f;

	//camera rotation
	float velFactor = -10.0f;
	float mouseXVelocity = (float)UserInput::GetMouseXVelocity();
	float mouseYVelocity = (float)UserInput::GetMouseYVelocity();

	m_State.axis_Y_velocity = mouseXVelocity / velFactor;
	m_State.axis_X_velocity = -1 * mouseYVelocity / velFactor;
	
	//camera movement
	Math::cMatrix_transformation localToWorldMat = Math::cMatrix_transformation::cMatrix_transformation(m_State.orientation, m_State.position);
	Math::sVector forwardVector = localToWorldMat.GetBackDirection();
	forwardVector.Normalize();
	forwardVector = forwardVector * -5;

	Math::sVector rightVector = localToWorldMat.GetRightDirection();
	rightVector.Normalize();
	rightVector = rightVector * 5;

	if (UserInput::IsKeyPressed(UserInput::KeyCodes::D))
	{
		m_State.velocity = rightVector;
	}
	if (UserInput::IsKeyPressed(UserInput::KeyCodes::A))
	{
		m_State.velocity = -1 * rightVector;
	}
	if (UserInput::IsKeyPressed(UserInput::KeyCodes::W))
	{
		m_State.velocity = forwardVector;
	}
	if (UserInput::IsKeyPressed(UserInput::KeyCodes::S))
	{
		m_State.velocity = forwardVector * -1;
	}

	
}
/*
eae6320::Math::cQuaternion eae6320::GameCommon::Camera::PredictFutureOrientation(const float i_secondCountToExtrapolate) {
	auto rotationWorldY = Math::cQuaternion(angularVelocityY * i_secondCountToExtrapolate, Math::sVector(0.0f, 1.0f, 0.0f));
	auto rotationLocalX = Math::cQuaternion(angularVelocityX * i_secondCountToExtrapolate, Math::sVector(1.0f, 0.0f, 0.0f));
	auto predictedOrientation = rotationWorldY * m_State.orientation * rotationLocalX;
	return predictedOrientation.GetNormalized();
}
*/