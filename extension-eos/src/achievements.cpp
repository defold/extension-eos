#include <dmsdk/sdk.h>
#include "eos_achievements.h"
#include "eos_sdk.h"

#include "achievements.h"

static EOS_HAchievements g_AchievementsHandle = 0;
static EOS_NotificationId g_AchievementsUnlockedNotificationId;


namespace extEOS
{
    bool InitAchievements(EOS_HPlatform platform_handle)
    {
        g_AchievementsHandle = EOS_Platform_GetAchievementsInterface(platform_handle);
        dmLogInfo("InitAchievements %p %p", g_AchievementsHandle, platform_handle);
        if (g_AchievementsHandle == 0)
        {
            return 0;
        }
        return 1;
    }

    int SetAchievementListener(lua_State* L)
    {
        DM_LUA_STACK_CHECK(L, 1);

        return 1;
    }

    void EOS_CALL AchievementsUnlockedReceivedCallbackFn(const EOS_Achievements_OnAchievementsUnlockedCallbackV2Info* Data)
    {
        dmLogInfo("Achievements Unlocked %s", Data->AchievementId);
        // const std::wstring WideAchievementId = FStringUtils::Widen(Data->AchievementId);
        // FDebugLog::Log(L"Achievements Unlocked: %ls", WideAchievementId.c_str());
        // FGame::Get().GetAchievements()->NotifyUnlockedAchievement(WideAchievementId);

        // FGameEvent Event(EGameEventType::AchievementsUnlocked, Data->UserId);
        // FGame::Get().OnGameEvent(Event);
    }

    void UnsubscribeFromAchievementsUnlockedNotification()
    {
        if (g_AchievementsUnlockedNotificationId != EOS_INVALID_NOTIFICATIONID)
        {
            EOS_Achievements_RemoveNotifyAchievementsUnlocked(g_AchievementsHandle, g_AchievementsUnlockedNotificationId);
            g_AchievementsUnlockedNotificationId = EOS_INVALID_NOTIFICATIONID;
        }
    }

    void SubscribeToAchievementsUnlockedNotification()
    {
        extEOS::UnsubscribeFromAchievementsUnlockedNotification();

        EOS_Achievements_AddNotifyAchievementsUnlockedV2Options AchievementsUnlockedNotifyOptions = {};
        AchievementsUnlockedNotifyOptions.ApiVersion = EOS_ACHIEVEMENTS_ADDNOTIFYACHIEVEMENTSUNLOCKEDV2_API_LATEST;
        g_AchievementsUnlockedNotificationId = EOS_Achievements_AddNotifyAchievementsUnlockedV2(g_AchievementsHandle, &AchievementsUnlockedNotifyOptions, nullptr, extEOS::AchievementsUnlockedReceivedCallbackFn);
    }

    int GetAchievementDefinitions(lua_State* L)
    {
        DM_LUA_STACK_CHECK(L, 1);

        EOS_Achievements_GetAchievementDefinitionCountOptions achievementDefinitionsCountOptions = {};
        achievementDefinitionsCountOptions.ApiVersion = EOS_ACHIEVEMENTS_GETACHIEVEMENTDEFINITIONCOUNT_API_LATEST;

        uint32_t achievementDefintionsCount = EOS_Achievements_GetAchievementDefinitionCount(g_AchievementsHandle, &achievementDefinitionsCountOptions);

        EOS_Achievements_CopyAchievementDefinitionV2ByIndexOptions copyOptions = {};
        copyOptions.ApiVersion = EOS_ACHIEVEMENTS_COPYDEFINITIONV2BYACHIEVEMENTID_API_LATEST;

        lua_newtable(L);

        for (copyOptions.AchievementIndex = 0; copyOptions.AchievementIndex < achievementDefintionsCount; ++copyOptions.AchievementIndex)
        {
            EOS_Achievements_DefinitionV2* achievementDef = NULL;
            EOS_EResult copyAchievementDefinitionsResult = EOS_Achievements_CopyAchievementDefinitionV2ByIndex(g_AchievementsHandle, &copyOptions, &achievementDef);
            if (copyAchievementDefinitionsResult != EOS_EResult::EOS_Success)
            {
                dmLogInfo("CopyAchievementDefinitions Failure!");
                break;
            }

            lua_pushnumber(L, copyOptions.AchievementIndex + 1);
            lua_newtable(L);
            lua_rawset(L, -3);

            lua_pushstring(L, "is_hidden");
            lua_pushnumber(L, achievementDef->bIsHidden);
            lua_rawset(L, -3);

            lua_pushstring(L, "achievement_id");
            lua_pushstring(L, achievementDef->AchievementId);
            lua_rawset(L, -3);

            if (achievementDef->UnlockedDisplayName)
            {
                lua_pushstring(L, "unlocked_display_name");
                lua_pushstring(L, achievementDef->UnlockedDisplayName);
                lua_rawset(L, -3);
            }

            if (achievementDef->UnlockedDescription)
            {
                lua_pushstring(L, "unlocked_description");
                lua_pushstring(L, achievementDef->UnlockedDescription);
                lua_rawset(L, -3);
            }

            if (achievementDef->LockedDisplayName)
            {
                lua_pushstring(L, "locked_display_name");
                lua_pushstring(L, achievementDef->LockedDisplayName);
                lua_rawset(L, -3);
            }

            if (achievementDef->LockedDescription)
            {
                lua_pushstring(L, "locked_description");
                lua_pushstring(L, achievementDef->LockedDescription);
                lua_rawset(L, -3);
            }

            if (achievementDef->FlavorText)
            {
                lua_pushstring(L, "flavor_text");
                lua_pushstring(L, achievementDef->FlavorText);
                lua_rawset(L, -3);
            }

            if (achievementDef->UnlockedIconURL)
            {
                lua_pushstring(L, "unlocked_icon_url");
                lua_pushstring(L, achievementDef->UnlockedIconURL);
                lua_rawset(L, -3);
            }

            if (achievementDef->LockedIconURL)
            {
                lua_pushstring(L, "locked_icon_url");
                lua_pushstring(L, achievementDef->LockedIconURL);
                lua_rawset(L, -3);
            }

            // for (uint32_t statIndex = 0; statIndex < achievementDef->StatThresholdsCount; ++statIndex)
            // {
            //     FStatInfo StatInfo;
            //     StatInfo.Name = FStringUtils::Widen(achievementDef->StatThresholds[statIndex].Name);
            //     StatInfo.ThresholdValue = achievementDef->StatThresholds[statIndex].Threshold;
            //     AchievementsDefinition->StatInfo.emplace_back(StatInfo);
            // }


            // Release Achievement Definition
            EOS_Achievements_DefinitionV2_Release(achievementDef);
        }

        // PrintAchievementDefinitions();

        // FGameEvent Event(EGameEventType::DefinitionsReceived);
        // FGame::Get().OnGameEvent(Event);

        return 1;
    }







}