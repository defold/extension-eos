#define EXTENSION_NAME EOS
#define LIB_NAME "EOS"
#define MODULE_NAME "eos"

#define DLIB_LOG_DOMAIN LIB_NAME

#include <dmsdk/sdk.h>
#include "eos.h"
#include "stats.h"
#include "achievements.h"

#include "eos_init.h"
#include "eos_base.h"

static EOS_HPlatform g_PlatformHandle = 0;


static char* duplicate_checkstring(lua_State* L, int index)
{
    return strndup(luaL_checkstring(L, -1), lua_objlen(L, -1));
}

namespace extEOS
{
    int Init(lua_State* L) {
        DM_LUA_STACK_CHECK(L, 2);
        if (g_PlatformHandle != 0)
        {
            dmLogError("EOS Already initialized");
            lua_pushboolean(L, 1);
            lua_pushnil(L);
            return 2;
        }

        EOS_InitializeOptions options;
        options.ProductName = "";
        options.ProductVersion = "";
        options.ApiVersion = EOS_INITIALIZE_API_LATEST;
        options.Reserved = nullptr;
        options.AllocateMemoryFunction = nullptr;
        options.ReallocateMemoryFunction = nullptr;
        options.ReleaseMemoryFunction = nullptr;
        options.SystemInitializeOptions = nullptr;
        options.OverrideThreadAffinity = nullptr;

        EOS_Platform_Options platform_options;
        platform_options.ProductId = "";
        platform_options.SandboxId = "";
        platform_options.DeploymentId = "";
        platform_options.ClientCredentials.ClientId = "";
        platform_options.ClientCredentials.ClientSecret = "";
        platform_options.CacheDirectory = "";
        platform_options.ApiVersion = EOS_PLATFORM_OPTIONS_API_LATEST;
        platform_options.Reserved = nullptr;
        platform_options.bIsServer = 0;
        platform_options.OverrideCountryCode = nullptr;
        platform_options.OverrideLocaleCode = nullptr;
        platform_options.Flags = EOS_PF_WINDOWS_ENABLE_OVERLAY_OPENGL;
        platform_options.CacheDirectory = nullptr;
        platform_options.TickBudgetInMilliseconds = 0;
        platform_options.RTCOptions = nullptr;
        platform_options.EncryptionKey = nullptr;
        platform_options.IntegratedPlatformOptionsContainerHandle = nullptr;

        luaL_checktype(L, 1, LUA_TTABLE);
        lua_pushvalue(L, 1);
        lua_pushnil(L);
        while (lua_next(L, -2)) {
            const char* key = lua_tostring(L, -2);
            if (strcmp(key, "product_name") == 0)
            {
                options.ProductName = duplicate_checkstring(L, -1);
            }
            else if (strcmp(key, "product_version") == 0)
            {
                options.ProductVersion = duplicate_checkstring(L, -1);
            }
            else if (strcmp(key, "product_id") == 0)
            {
                platform_options.ProductId = duplicate_checkstring(L, -1);
            }
            else if (strcmp(key, "sandbox_id") == 0)
            {
                platform_options.SandboxId = duplicate_checkstring(L, -1);
            }
            else if (strcmp(key, "deployment_id") == 0)
            {
                platform_options.DeploymentId = duplicate_checkstring(L, -1);
            }
            else if (strcmp(key, "client_id") == 0)
            {
                platform_options.ClientCredentials.ClientId = duplicate_checkstring(L, -1);
            }
            else if (strcmp(key, "client_secret") == 0)
            {
                platform_options.ClientCredentials.ClientSecret = duplicate_checkstring(L, -1);
            }
            else if (strcmp(key, "cache_directory") == 0)
            {
                platform_options.CacheDirectory = duplicate_checkstring(L, -1);
            }
            lua_pop(L, 1); // pop value, leave key for lua_next
        }
        lua_pop(L, 1);

        EOS_EResult r = EOS_Initialize(&options);
        if (r == EOS_EResult::EOS_AlreadyConfigured)
        {
            dmLogWarning("EOS Already configured");
        }
        else if (r != EOS_EResult::EOS_Success)
        {
            dmLogError("EOS Initializing failed (%d)", r);
            lua_pushboolean(L, 0);
            lua_pushstring(L, EOS_EResult_ToString(r));
            return 2;
        }

        g_PlatformHandle = EOS_Platform_Create(&platform_options);
        if (g_PlatformHandle == 0)
        {
            dmLogError("EOS Platform not initialized");
            lua_pushboolean(L, 0);
            lua_pushstring(L, "Platform not initialized");
            return 2;
        }

        if (!extEOS::InitializeAchievements(g_PlatformHandle))
        {
            dmLogError("EOS Achievements not initialized");
            lua_pushboolean(L, 0);
            lua_pushstring(L, "Achievements not initialized");
            return 2;
        }

        if (!extEOS::InitializeStats(g_PlatformHandle))
        {
            dmLogError("EOS Stats not initialized");
            lua_pushboolean(L, 0);
            lua_pushstring(L, "Stats not initialized");
            return 2;
        }

        lua_pushboolean(L, 1);
        lua_pushnil(L);
        return 2;
    }

}


static const luaL_reg Module_methods[] =
{
    {"init", extEOS::Init},
    {"get_achievement_definitions", extEOS::GetAchievementDefinitions},
    {"set_achievement_listener", extEOS::SetAchievementListener},
    {"get_stats_definitions", extEOS::GetStatsDefinitions},
    {0,0}
};

static void LuaInit(lua_State* L)
{
    DM_LUA_STACK_CHECK(L, 0);
    int top = lua_gettop(L);
    luaL_register(L, MODULE_NAME, Module_methods);
    lua_pop(L, 1);
}

static dmExtension::Result ExtEOS_Update(dmExtension::Params* params)
{
    return dmExtension::RESULT_OK;
}

static dmExtension::Result ExtEOS_AppInitialize(dmExtension::AppParams* params)
{
    dmLogInfo("ExtEOS_AppInitialize");
    return dmExtension::RESULT_OK;
}

static dmExtension::Result ExtEOS_Initialize(dmExtension::Params* params)
{
    dmLogInfo("ExtEOS_Initialize");
    LuaInit(params->m_L);
    return dmExtension::RESULT_OK;
}

static dmExtension::Result ExtEOS_AppFinalize(dmExtension::AppParams* params)
{
    dmLogInfo("ExtEOS_AppFinalize");
    return dmExtension::RESULT_OK;
}

static dmExtension::Result ExtEOS_Finalize(dmExtension::Params* params)
{
    dmLogInfo("ExtEOS_Finalize");
    // extEOS::FinalizeAchievements();
    // extEOS::FinalizeStats();

    // if (g_PlatformHandle != 0)
    // {
    //     EOS_Platform_Release(g_PlatformHandle);
    //     g_PlatformHandle = 0;
    // }

    // EOS_EResult r = EOS_Shutdown();
    // dmLogInfo("EOS_Shutdown %d",r);
    return dmExtension::RESULT_OK;
}

DM_DECLARE_EXTENSION(EXTENSION_NAME, LIB_NAME, ExtEOS_AppInitialize, ExtEOS_AppFinalize, ExtEOS_Initialize, ExtEOS_Update, 0, ExtEOS_Finalize)
