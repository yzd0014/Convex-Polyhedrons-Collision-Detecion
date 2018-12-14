// Includes
//=========

#include "../UserInput.h"

#include <Engine/Windows/Includes.h>
#include "Engine/UserOutput/UserOutput.h"
#include <iostream>
#include "Winuser.h"
// Interface
//==========

bool eae6320::UserInput::IsKeyPressed( const uint_fast8_t i_keyCode )
{
	const auto keyState = GetAsyncKeyState( i_keyCode );
	const short isKeyDownMask = ~1;
	return ( keyState & isKeyDownMask ) != 0;
}

double eae6320::UserInput::GetMouseXVelocity() {
	int xDistance;
	POINT screenPos[1];
	int screenWidth = GetSystemMetrics(SM_CXSCREEN);
	GetCursorPos(screenPos);
	if (MouseMovement::xPosCached < 0) {
		xDistance = 0;
	}
	else {
		xDistance = (int)screenPos[0].x - MouseMovement::xPosCached;
		if (abs(xDistance) > screenWidth / 2) {
			xDistance = 0;
		}
	}
	MouseMovement::xPosCached = (int)screenPos[0].x;
	
	//eae6320::UserOutput::DebugPrint("%d", xDistance);
	return xDistance / MouseMovement::elapsedSeconds;
}

double eae6320::UserInput::GetMouseYVelocity() {
	int yDistance;
	POINT screenPos[1];
	int screenHeight = GetSystemMetrics(SM_CYSCREEN);
	GetCursorPos(screenPos);
	if (MouseMovement::yPosCached < 0) {
		yDistance = 0;
	}
	else {
		yDistance = (int)screenPos[0].y - MouseMovement::yPosCached;
		if (abs(yDistance) > screenHeight / 2) {
			yDistance = 0;
		}

	}
	MouseMovement::yPosCached = (int)screenPos[0].y;

	//eae6320::UserOutput::DebugPrint("%d", xDistance);
	return -1 * yDistance / MouseMovement::elapsedSeconds;
}

void eae6320::UserInput::ClampCursorPosition() {
	int screenWidth = GetSystemMetrics(SM_CXSCREEN);
	int screenHeight = GetSystemMetrics(SM_CYSCREEN);

	POINT screenPos[1];
	GetCursorPos(screenPos);
	int currentX, currentY;
	currentX = (int)screenPos[0].x;
	currentY = (int)screenPos[0].y;
	//clamp for x
	if (currentX > screenWidth - 5) {
		SetCursorPos(5, currentY);
		currentX = 5;
		
		MouseMovement::xPosCached = currentX;
		MouseMovement::yPosCached = currentY;
	}
	if (currentX < 5) {
		SetCursorPos(screenWidth - 5, currentY);
		currentX = screenWidth - 5;
		
		MouseMovement::xPosCached = currentX;
		MouseMovement::yPosCached = currentY;
	}

	//clamp for y
	if (currentY > screenHeight - 5) {
		SetCursorPos(currentX, 5);
		currentY = 5;

		MouseMovement::xPosCached = currentX;
		MouseMovement::yPosCached = currentY;
	}
	if (currentY < 5) {
		SetCursorPos(currentX, screenHeight - 5);
		currentY = screenHeight - 5;

		MouseMovement::xPosCached = currentX;
		MouseMovement::yPosCached = currentY;
	}
}