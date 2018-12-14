// Includes
//=========

#include "cMyGame.h"

#include <Engine/Asserts/Asserts.h>
#include <Engine/UserInput/UserInput.h>
#include "Engine/Graphics/Graphics.h"
#include "Engine/Graphics/Mesh.h"
#include "Engine/Graphics/cRenderState.h"
#include "Engine/Math/Functions.h"
#include "Engine/Math/cMatrix_transformation.h"
#include "Engine/Math/cQuaternion.h"
#include "Engine/Math/sVector.h"
#include "Engine/UserOutput/UserOutput.h"
#include "Engine/Physics/CollisionDetection.h"
#include "Engine/Physics/PhysicsSimulation.h"
#include "Custom Game Objects/HomingCube.h"
#include "Custom Game Objects/MoveableCube.h"

// Inherited Implementation
//=========================

// Run
//----

void eae6320::cMyGame::UpdateBasedOnInput()
{
	// Is the user pressing the ESC key?
	if ( UserInput::IsKeyPressed( UserInput::KeyCodes::Escape ) )
	{
		// Exit the application
		const auto result = Exit( EXIT_SUCCESS );
		EAE6320_ASSERT( result );
	}
}

// Initialization / Clean Up
//--------------------------

eae6320::cResult eae6320::cMyGame::Initialize()
{
	//initialize camera 
	mainCamera.Initialize(Math::sVector(0.0f, 0.0f, 10.0f), Math::cQuaternion(), Math::ConvertDegreesToRadians(45), 1.0f, 0.1f, 500.0f);
	
	//create two meshes 	
	eae6320::Assets::cHandle<Mesh> mesh_cube;
	eae6320::Assets::cHandle<Mesh> mesh_circle;
	eae6320::Assets::cHandle<Mesh> mesh_plane;
	eae6320::Assets::cHandle<Mesh> mesh_train;
	
	auto result = eae6320::Results::Success;
	if (!(result = Mesh::s_manager.Load("data/meshes/cube.mesh", mesh_cube))) {
		EAE6320_ASSERT(false);
	}
	
	if (!(result = Mesh::s_manager.Load("data/meshes/circle.mesh", mesh_circle))) {
		EAE6320_ASSERT(false);
	}
	if (!(result = Mesh::s_manager.Load("data/meshes/plane.mesh", mesh_plane))) {
		EAE6320_ASSERT(false);
	}
	if (!(result = Mesh::s_manager.Load("data/meshes/train.mesh", mesh_train))) {
		EAE6320_ASSERT(false);
	}
	
	masterMeshArray.push_back(mesh_cube);
	masterMeshArray.push_back(mesh_circle);
	masterMeshArray.push_back(mesh_plane);
	masterMeshArray.push_back(mesh_train);

	//create two effect
	Effect* pEffect_white;
	Effect* pEffect_purple;
	Effect::Load("data/effects/white.effect", pEffect_white);
	Effect::Load("data/effects/purple.effect", pEffect_purple);

	masterEffectArray.push_back(pEffect_white);
	masterEffectArray.push_back(pEffect_purple);

	//add a cube game object
	{//player 
		Physics::sRigidBodyState objState;
		//objState.boundingBox.center = Math::sVector(0.0f, 0.0f, 0.0f);
		//objState.boundingBox.extends = Math::sVector(1.0f, 1.0f, 1.0f);
		objState.surfaceNormals[0] = Math::sVector(1.0f, 0.0f, 0.0f);
		objState.surfaceNormals[1] = Math::sVector(0.0f, 1.0f, 0.0f);
		objState.surfaceNormals[2] = Math::sVector(0.0f, 0.0f, 1.0f);
		objState.surfaceNormalsCount = 3;

		objState.edges[0] = Math::sVector(1.0f, 0.0f, 0.0f);
		objState.edges[1] = Math::sVector(0.0f, 1.0f, 0.0f);
		objState.edges[2] = Math::sVector(0.0f, 0.0f, 1.0f);
		objState.edgesCount = 3;

		objState.vertices[0] = Math::sVector(-1.0f, 1.0f, 1.0f);
		objState.vertices[1] = Math::sVector(-1.0f, 1.0f, -1.0f);
		objState.vertices[2] = Math::sVector(1.0f, 1.0f, -1.0f);
		objState.vertices[3] = Math::sVector(1.0f, 1.0f, 1.0f);
		objState.vertices[4] = Math::sVector(-1.0f, -1.0f, 1.0f);
		objState.vertices[5] = Math::sVector(-1.0f, -1.0f, -1.0f);
		objState.vertices[6] = Math::sVector(1.0f, -1.0f, -1.0f);
		objState.vertices[7] = Math::sVector(1.0f, -1.0f, 1.0f);
		objState.verticesCount = 8;

		objState.position = Math::sVector(0.0f, 0.0f, 0.0f);
		objState.orientation = Math::cQuaternion();
		MoveableCube * pGameObject = new MoveableCube(masterEffectArray[0], masterMeshArray[0], objState);
		masterGameObjectArr.push_back(pGameObject);
	}
	/*
	{//AI
		Physics::sRigidBodyState objState;
		objState.boundingBox.center = Math::sVector(0.0f, 0.0f, 0.0f);
		objState.boundingBox.extends = Math::sVector(1.0f, 1.0f, 1.0f);
		objState.position = Math::sVector(5.0f, 0.0f, 0.0f);
		objState.orientation = Math::cQuaternion();
		objState.axis_X_velocity = 400;
		objState.axis_Y_velocity = 400;
		HomingCube * pGameObject = new HomingCube(masterEffectArray[1], masterMeshArray[0], objState);
		pGameObject->m_target = masterGameObjectArr[0];
		masterGameObjectArr.push_back(pGameObject);
	}
	
	{//AI
		Physics::sRigidBodyState objState;
		objState.boundingBox.center = Math::sVector(0.0f, 0.0f, 0.0f);
		objState.boundingBox.extends = Math::sVector(1.0f, 1.0f, 1.0f);
		objState.position = Math::sVector(-5.0f, 0.0f, 0.0f);
		objState.orientation = Math::cQuaternion();
		//objState.axis_X_velocity = 400;
		objState.axis_Y_velocity = 400;
		HomingCube * pGameObject = new HomingCube(masterEffectArray[1], masterMeshArray[0], objState);
		pGameObject->m_target = masterGameObjectArr[0];
		masterGameObjectArr.push_back(pGameObject);
	}
	
	{//AI
		Physics::sRigidBodyState objState;
		objState.boundingBox.center = Math::sVector(0.0f, 0.0f, 0.0f);
		objState.boundingBox.extends = Math::sVector(1.0f, 1.0f, 1.0f);
		objState.position = Math::sVector(0.0f, 5.0f, 0.0f);
		objState.orientation = Math::cQuaternion();
		HomingCube * pGameObject = new HomingCube(masterEffectArray[1], masterMeshArray[0], objState);
		pGameObject->m_target = masterGameObjectArr[0];
		masterGameObjectArr.push_back(pGameObject);
	}
	*/
	//add plane
	
	{
		Physics::sRigidBodyState objState;
		objState.position = Math::sVector(8.0f, 0.0f, 0.0f);
		//objState.boundingBox.center = Math::sVector(0.0f, 0.0f, 0.0f);
		//objState.boundingBox.extends = Math::sVector(5.0f, 0.0f, 5.0f);
		objState.surfaceNormals[0] = Math::sVector(-0.942809f, 0.333333f, 0.0f);
		objState.surfaceNormals[1] = Math::sVector(0.471405f, 0.333333f, -0.816497f);
		objState.surfaceNormals[2] = Math::sVector(0.471405f, 0.333333f, 0.816497f);
		objState.surfaceNormals[3] = Math::sVector(0.0f, -1.0f, 0.0f);
		objState.surfaceNormalsCount = 4;

		Math::sVector top = Math::sVector(0.0f, 1.63299f, 0.0f);
		Math::sVector base_1 = Math::sVector(-1.1547f, -1.63299f, 2.0f);
		Math::sVector base_2 = Math::sVector(-1.1547f, -1.63299f, -2.0f);
		Math::sVector base_3 = Math::sVector(2.3094f, -1.63299f, 0.0f);
		objState.edges[0] = top - base_1;
		objState.edges[1] = top - base_2;
		objState.edges[2] = top - base_3;
		objState.edges[3] = base_1 - base_2;
		objState.edges[4] = base_2 - base_3;
		objState.edges[5] = base_1 - base_3;
		objState.edgesCount = 6;

		objState.vertices[0] = top;
		objState.vertices[1] = base_1;
		objState.vertices[2] = base_2;
		objState.vertices[3] = base_3;
		objState.verticesCount = 4;

		objState.orientation = Math::cQuaternion();
		objState.euler_x = 15;
		GameCommon::GameObject * PGameObject = new GameCommon::GameObject(masterEffectArray[0], mesh_train, objState);
		masterGameObjectArr.push_back(PGameObject);
	}
	
	return Results::Success;
}

void eae6320::cMyGame::UpdateSimulationBasedOnInput() {
	mainCamera.UpdateCameraBasedOnInput();

	size_t numOfObjects = masterGameObjectArr.size();
	for (size_t i = 0; i < numOfObjects; i++) {
		masterGameObjectArr[i]->UpdateGameObjectBasedOnInput();
	}
}

void  eae6320::cMyGame::UpdateSimulationBasedOnTime(const float i_elapsedSecondCount_sinceLastUpdate) {
	//run AI
	size_t numOfObjects = masterGameObjectArr.size();
	for (size_t i = 0; i < numOfObjects; i++) {
		masterGameObjectArr[i]->EventTick(i_elapsedSecondCount_sinceLastUpdate);
	}
	
// ***********************run physics****************************************************	
	//update game objects
	Physics::PhysicsUpdate(masterGameObjectArr, i_elapsedSecondCount_sinceLastUpdate);
	//update camera
	mainCamera.UpdateState(i_elapsedSecondCount_sinceLastUpdate);
}


eae6320::cResult eae6320::cMyGame::CleanUp()
{	//release all game objects first
	size_t numOfObjects = masterGameObjectArr.size();
	for (size_t i = 0; i < numOfObjects; i++) {
		delete masterGameObjectArr[i];
	}
	masterGameObjectArr.clear();
	
	//release effect
	for (size_t i = 0; i < masterEffectArray.size(); i++) {
		masterEffectArray[i]->DecrementReferenceCount();
		masterEffectArray[i] = nullptr;
	}
	masterEffectArray.clear();

	//release mesh handle
	for (size_t i = 0; i < masterMeshArray.size(); i++) {
		Mesh::s_manager.Release(masterMeshArray[i]);
	}
	masterMeshArray.clear();
	
	return Results::Success;
}

void eae6320::cMyGame::SubmitDataToBeRendered(const float i_elapsedSecondCount_systemTime, const float i_elapsedSecondCount_sinceLastSimulationUpdate) {	
//submit background color
	float color[] = { 0, 1, 0, 1 };
	if (collision) {
		color[0] = 0.2f;
		color[1] = 0.4f;
		color[2] = 0.8f;
	}
	eae6320::Graphics::SubmitBGColor(color);

	//submit gameObject
	for (size_t i = 0; i < masterGameObjectArr.size(); i++) {
		//smooth movement first
		Math::sVector predictedPosition = masterGameObjectArr[i]->m_State.PredictFuturePosition(i_elapsedSecondCount_sinceLastSimulationUpdate);
		Math::cQuaternion predictedOrientation = masterGameObjectArr[i]->m_State.PredictFutureOrientation(i_elapsedSecondCount_sinceLastSimulationUpdate);
		//submit
		eae6320::Graphics::SubmitObject(Math::cMatrix_transformation(predictedOrientation, predictedPosition),
			masterGameObjectArr[i]->GetEffect(), Mesh::s_manager.Get(masterGameObjectArr[i]->GetMesh()));

	}
	
	//submit camera
	{
		//smooth camera movemnt first before it's submitted
		Math::sVector predictedPosition = mainCamera.m_State.PredictFuturePosition(i_elapsedSecondCount_sinceLastSimulationUpdate);
		Math::cQuaternion predictedOrientation = mainCamera.m_State.PredictFutureOrientation(i_elapsedSecondCount_sinceLastSimulationUpdate);
		//submit
		eae6320::Graphics::SubmitCamera(Math::cMatrix_transformation::CreateWorldToCameraTransform(predictedOrientation, predictedPosition),
			mainCamera.GetCameraToProjectedMat());
	}	
}