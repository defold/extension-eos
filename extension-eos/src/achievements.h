#ifndef H_EOS_ACHIEVEMENTS
#define H_EOS_ACHIEVEMENTS

#include <dmsdk/sdk.h>
#include "eos_sdk.h"

namespace extEOS
{
	bool InitAchievements(EOS_HPlatform platform_handle);
	int SetAchievementListener(lua_State* L);
	int GetAchievementDefinitions(lua_State* L);
}


#endif