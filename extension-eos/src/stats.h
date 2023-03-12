#ifndef H_EOS_STATS
#define H_EOS_STATS

#include "eos_sdk.h"

namespace extEOS
{
    bool InitializeStats(EOS_HPlatform platform_handle);
    void FinalizeStats();
    int GetStatsDefinitions(lua_State* L);
}

#endif