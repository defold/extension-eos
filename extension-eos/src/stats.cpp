#include <dmsdk/sdk.h>

#include "eos_stats.h"
#include "eos_sdk.h"

#include "stats.h"

static EOS_HStats g_StatsHandle = 0;

namespace extEOS
{
    bool InitStats(EOS_HPlatform platform_handle)
    {
        g_StatsHandle = EOS_Platform_GetStatsInterface(platform_handle);
        return g_StatsHandle != 0;
    }
}