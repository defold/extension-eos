#include <dmsdk/sdk.h>

#include "eos_stats.h"
#include "eos_sdk.h"

#include "stats.h"

static EOS_HStats g_StatsHandle = 0;

namespace extEOS
{
    bool InitializeStats(EOS_HPlatform platform_handle)
    {
        g_StatsHandle = EOS_Platform_GetStatsInterface(platform_handle);
        return g_StatsHandle != 0;
    }

    void FinalizeStats()
    {
        g_StatsHandle = 0;
    }


    int GetStatsDefinitions(lua_State* L)
    {
        DM_LUA_STACK_CHECK(L, 1);

        EOS_ProductUserId userId = EOS_ProductUserId_FromString(luaL_checkstring(L, -1));

        EOS_Stats_GetStatCountOptions statCountOptions = {};
        statCountOptions.ApiVersion = EOS_STATS_GETSTATCOUNT_API_LATEST;
        statCountOptions.TargetUserId = userId;

        uint32_t statsCount = EOS_Stats_GetStatsCount(g_StatsHandle, &statCountOptions);

        dmLogInfo("GetStatsDefinitions %d", statsCount);

        EOS_Stats_Stat* stat = NULL;
        EOS_Stats_CopyStatByIndexOptions copyByIndexOptions = {};
        copyByIndexOptions.ApiVersion = EOS_STATS_COPYSTATBYINDEX_API_LATEST;
        copyByIndexOptions.TargetUserId = userId;

        lua_newtable(L);

        for (copyByIndexOptions.StatIndex = 0; copyByIndexOptions.StatIndex < statsCount; ++copyByIndexOptions.StatIndex)
        {
            EOS_EResult copyStatResult = EOS_Stats_CopyStatByIndex(g_StatsHandle, &copyByIndexOptions, &stat);
            if (copyStatResult != EOS_EResult::EOS_Success)
            {
                dmLogError("Copy Stat failure");
                break;
            }

            lua_pushnumber(L, copyByIndexOptions.StatIndex + 1);
            lua_newtable(L);
            lua_rawset(L, -3);

            if (stat->Name)
            {
                lua_pushstring(L, "name");
                lua_pushstring(L, stat->Name);
                lua_rawset(L, -3);
            }

            lua_pushstring(L, "start_time");
            lua_pushnumber(L, stat->StartTime);
            lua_rawset(L, -3);

            lua_pushstring(L, "stop_time");
            lua_pushnumber(L, stat->EndTime);
            lua_rawset(L, -3);

            lua_pushstring(L, "value");
            lua_pushnumber(L, stat->Value);
            lua_rawset(L, -3);

            // Release
            EOS_Stats_Stat_Release(stat);
        }
        return 1;
    }
}