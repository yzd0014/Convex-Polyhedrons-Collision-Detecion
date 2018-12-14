#include <cmath> 
#include <vector>

#include "CollisionDetection.h"
#include "Engine/Physics/sRigidBodyState.h"
#include "Engine/Physics/CollisionPair.h"
#include "Engine/GameCommon/GameObject.h"
#include "Engine/Math/cMatrix_transformation.h"
#include "Engine/Math/Functions.h"

#define fakeZero -0.000001f
#define epsilon 1.0e-9f

namespace eae6320 {
	namespace Physics {
		bool  CollisionDetection(sRigidBodyState  & i_object_A, sRigidBodyState & i_object_B, float i_dt, Math::sVector &o_normal4A, float &o_collisionTime) { 
	
			Math::cMatrix_transformation A2World_rotation(i_object_A.orientation, Math::sVector(0.0f, 0.0f, 0.0f));
			Math::cMatrix_transformation B2World_rotation(i_object_B.orientation, Math::sVector(0.0f, 0.0f, 0.0f));

			Math::sVector normal4A;
			float maxCloseTime = -10.0f;
			float minOpenTime = 10.0f;

			int axesCount = i_object_A.surfaceNormalsCount + i_object_B.surfaceNormalsCount + i_object_A.edgesCount * i_object_B.edgesCount;

			bool flag = false;
			for (int c = 0; c < axesCount; c++) {
				//printf("%d", omp_get_num_threads()); 
				if (flag) continue;
				//check object A normals
				if (c < i_object_A.surfaceNormalsCount) {
					Math::sVector axis = A2World_rotation * i_object_A.surfaceNormals[c];
					bool potentialCollision = AxisCheck(i_object_A, i_object_B, axis, i_dt, maxCloseTime, minOpenTime, normal4A);
					if (potentialCollision == false) flag = true;
				}
				//check object B normals
				else if (c >= i_object_A.surfaceNormalsCount && c < i_object_A.surfaceNormalsCount + i_object_B.surfaceNormalsCount) {
					Math::sVector axis = B2World_rotation * i_object_B.surfaceNormals[c - i_object_A.surfaceNormalsCount];
					bool potentialCollision = AxisCheck(i_object_A, i_object_B, axis, i_dt, maxCloseTime, minOpenTime, normal4A);
					if (potentialCollision == false) flag = true;
				}
				//check edge to edge collisions
				else {
					int i = (c - (i_object_A.surfaceNormalsCount + i_object_B.surfaceNormalsCount)) / i_object_B.edgesCount;
					int j = (c - (i_object_A.surfaceNormalsCount + i_object_B.surfaceNormalsCount)) % i_object_B.edgesCount;
					Math::sVector edge_A = A2World_rotation * i_object_A.edges[i];
					Math::sVector edge_B = B2World_rotation * i_object_B.edges[j];
					Math::sVector crossVector = Math::Cross(edge_A, edge_B);

					if (abs(crossVector.x) > epsilon || abs(crossVector.y) > epsilon || abs(crossVector.z) > epsilon) {
						crossVector.Normalize();
						bool potentialCollision = AxisCheck(i_object_A, i_object_B, crossVector, i_dt, maxCloseTime, minOpenTime, normal4A);
						if (potentialCollision == false) flag = true;
					}
				}
			}
			if (flag == true) return false;

			if (maxCloseTime <= minOpenTime) {
				o_normal4A = normal4A;
				o_collisionTime = maxCloseTime;
				return true;
			}

			return false;
		}
		bool AxisCheck(sRigidBodyState  & i_object_A, sRigidBodyState & i_object_B, Math::sVector & i_axis, float i_dt, float & o_maxCloseTime, float & o_minOpenTime, Math::sVector &o_normal4A) {
			float BProjectionOnAxis_left = 0.0f;
			float BProjectionOnAxis_right = 0.0f;

			Math::cMatrix_transformation B2World_rotation(i_object_B.orientation, Math::sVector(0.0f, 0.0f, 0.0f));

			for (int i = 0; i < i_object_B.verticesCount; i++) {
				Math::sVector vertex_world_B = B2World_rotation * i_object_B.vertices[i];
				float BProjectionTemp = Math::Dot(vertex_world_B, i_axis);
				if (BProjectionTemp > BProjectionOnAxis_right) {
					BProjectionOnAxis_right = BProjectionTemp;
				}
				else if (BProjectionTemp < BProjectionOnAxis_left) {
					BProjectionOnAxis_left = BProjectionTemp;
				}
			}
			BProjectionOnAxis_left = abs(BProjectionOnAxis_left);
			//*********************************************************************************
			float AProjectionOnAxis_left = 0.0f;
			float AProjectionOnAxis_right = 0.0f;

			Math::cMatrix_transformation A2World_rotation(i_object_A.orientation, Math::sVector(0.0f, 0.0f, 0.0f));

			for (int i = 0; i < i_object_A.verticesCount; i++) {
				Math::sVector vertex_world_A = A2World_rotation * i_object_A.vertices[i];
				float AProjectionTemp = Math::Dot(vertex_world_A, i_axis);
				if (AProjectionTemp > AProjectionOnAxis_right) {
					AProjectionOnAxis_right = AProjectionTemp;
				}
				else if (AProjectionTemp < AProjectionOnAxis_left) {
					AProjectionOnAxis_left = AProjectionTemp;
				}
			}
			AProjectionOnAxis_left = abs(AProjectionOnAxis_left);
			//********************************************************************************
			//get B velocity relative to A, assume A is static and B is moving 
			Math::sVector BVelocityInA = i_object_B.velocity - i_object_A.velocity;
			float BVeloInAProjectionOnAxis = Math::Dot(BVelocityInA, i_axis);

			Math::sVector BCenter2ACenter = i_object_A.position - i_object_B.position;
			float distance = Math::Dot(BCenter2ACenter, i_axis);
			float A_extends_left = AProjectionOnAxis_left + BProjectionOnAxis_right;
			float A_extends_right = AProjectionOnAxis_right + BProjectionOnAxis_left;

			float distanceClose = distance - A_extends_left;
			if (Math::floatEqual(distanceClose, 0.0f)) distanceClose = 0.0f;
			float distanceOpen = distance + A_extends_right;
			if (Math::floatEqual(distanceOpen, 0.0f)) distanceOpen = 0.0f;

			float closeTime;
			float openTime;
			Math::sVector possibleNormal4A = i_axis;

			if (BVeloInAProjectionOnAxis != 0) {
				closeTime = distanceClose / BVeloInAProjectionOnAxis;
				openTime = distanceOpen / BVeloInAProjectionOnAxis;

				if (closeTime <= 0 && openTime <= 0) {
					return false;
				}
				if (closeTime * openTime < 0) {
					if (BVeloInAProjectionOnAxis > 0) {
						closeTime = fakeZero;
					}
					else {
						openTime = closeTime;
						closeTime = fakeZero;
						//update normal
						possibleNormal4A = -1 * i_axis;
					}
				}
				if (closeTime > openTime) {
					float temp = closeTime;
					closeTime = openTime;
					openTime = temp;
					//update normal 
					possibleNormal4A = -1 * i_axis;
				}
				if (closeTime > i_dt) {
					return false;
				}
			}
			else {
				if (distanceClose < 0 && distanceOpen > 0) {
					closeTime = fakeZero;
					openTime = i_dt;
				}
				else {
					return false;
				}
			}

			//update close and open time
			if (closeTime > o_maxCloseTime) {
				o_maxCloseTime = closeTime;
				//update normal
				o_normal4A = possibleNormal4A;
			}
			if (openTime < o_minOpenTime) o_minOpenTime = openTime;

			return true;
		}

		bool FindEarliestCollision(std::vector<GameCommon::GameObject *> & i_allGameObjects, float i_dt, CollisionPair & o_earliestCollision) {
			o_earliestCollision.collisionTime = i_dt;
			bool foundCollision_earliest = false;
			size_t count = i_allGameObjects.size();

			for (size_t i = 0; i < count - 1; i++) {
				for (size_t j = i + 1; j < count; j++) {
					Math::sVector colNormal4A;
					float colTime;
					bool collisionFound = CollisionDetection(i_allGameObjects[i]->m_State, i_allGameObjects[j]->m_State, i_dt, colNormal4A, colTime);
					if (collisionFound && colTime != fakeZero) {
						if (colTime < o_earliestCollision.collisionTime) {
							o_earliestCollision.collisionTime = colTime;
							o_earliestCollision.collisionNormal4A = colNormal4A;
							o_earliestCollision.collisionObjects[0] = &i_allGameObjects[i]->m_State;
							o_earliestCollision.collisionObjects[1] = &i_allGameObjects[j]->m_State;
							//UserOutput::DebugPrint("x: %f, y: %f, z: %f", colNormal4A.x, colNormal4A.y, colNormal4A.z);
						}
						foundCollision_earliest = true;
					}
					
				}
			}

			return foundCollision_earliest;
		}

		bool FindRotationCollision(std::vector<GameCommon::GameObject *> & i_allGameObjects, float i_dt, size_t indexOfObjectChecked) {
			//make a copy of phyics state
			sRigidBodyState objectChecked = i_allGameObjects[indexOfObjectChecked]->m_State;
			objectChecked.UpdateOrientation(i_dt);
			objectChecked.velocity = Math::sVector(0.0f, 0.0f, 0.0f);

			size_t numOfObjects = i_allGameObjects.size();
			for (size_t i = 0; i < numOfObjects; i++) {
				if (i != indexOfObjectChecked) {
					//make a copy 
					sRigidBodyState otherObject = i_allGameObjects[i]->m_State;
					otherObject.velocity = Math::sVector(0.0f, 0.0f, 0.0f);

					Math::sVector placeHolderNormal;
					float placeHolderTime;
					bool isCollided = CollisionDetection(objectChecked, otherObject, i_dt, placeHolderNormal, placeHolderTime);
					if (isCollided) return true;
				}
			}

			return false;
		}

	}
}