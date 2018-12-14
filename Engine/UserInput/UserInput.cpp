#include "UserInput.h"
#include "Engine/Time/Time.h"
#include "Engine/UserOutput/UserOutput.h"
namespace eae6320
{
	namespace UserInput
	{
		
		void SetElapsedSecondsForMouse() {
			uint64_t elapsedTicks;
			if (MouseMovement::tickCount_systemTime_cached == 0) {
				elapsedTicks = 1;
			}
			else {
				elapsedTicks = eae6320::Time::GetCurrentSystemTimeTickCount() - MouseMovement::tickCount_systemTime_cached;
			}
			MouseMovement::tickCount_systemTime_cached = eae6320::Time::GetCurrentSystemTimeTickCount();
			//eae6320::UserOutput::DebugPrint("%d", elapsedTicks);
			MouseMovement::elapsedSeconds = eae6320::Time::ConvertTicksToSeconds(elapsedTicks);
		}
		namespace MouseMovement
		{
			uint64_t tickCount_systemTime_cached = 0;
			double elapsedSeconds = 0;
			int xPosCached = -1;
			int yPosCached = -1;
			
		}
	}
}