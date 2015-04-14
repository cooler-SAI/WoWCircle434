/*
 * Copyright (C) 2012-2014 Cerber Project <https://bitbucket.org/mojitoice/>
 * Copyright (C) 2008-2015 TrinityCore <http://www.trinitycore.org/>
 * Copyright (C) 2005-2009 MaNGOS <http://getmangos.com/>
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation; either version 2 of the License, or (at your
 * option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program. If not, see <http://www.gnu.org/licenses/>.
 */


#include "ScriptMgr.h"
#include "Config.h"
#include "DatabaseEnv.h"
#include "DBCStores.h"
#include "ObjectMgr.h"
#include "OutdoorPvPMgr.h"
#include "ScriptLoader.h"
#include "ScriptSystem.h"
#include "Transport.h"
#include "Vehicle.h"
#include "SpellInfo.h"
#include "SpellScript.h"
#include "GossipDef.h"
#include "CreatureAI.h"

// This is the global static registry of scripts.
template<class TScript>
class ScriptRegistry
{
public:

    typedef std::map<uint32, TScript*> ScriptMap;
    typedef typename ScriptMap::iterator ScriptMapIterator;

    // The actual list of scripts. This will be accessed concurrently, so it must not be modified
    // after server startup.
    static ScriptMap ScriptPointerList;

    static void AddScript(TScript* const script)
    {
        ASSERT(script);

        // See if the script is using the same memory as another script. If this happens, it means that
        // someone forgot to allocate new memory for a script.
        for (ScriptMapIterator it = ScriptPointerList.begin(); it != ScriptPointerList.end(); ++it)
        {
            if (it->second == script)
            {
                sLog->outError(LOG_FILTER_TSCR, "Script '%s' has same memory pointer as '%s'.",
                               script->GetName().c_str(), it->second->GetName().c_str());

                return;
            }
        }

        if (script->IsDatabaseBound())
        {
            // Get an ID for the script. An ID only exists if it's a script that is assigned in the database
            // through a script name (or similar).
            uint32 id = sObjectMgr->GetScriptId(script->GetName().c_str());
            if (id)
            {
                // Try to find an existing script.
                bool existing = false;
                for (ScriptMapIterator it = ScriptPointerList.begin(); it != ScriptPointerList.end(); ++it)
                {
                    // If the script names match...
                    if (it->second->GetName() == script->GetName())
                    {
                        // ... It exists.
                        existing = true;
                        break;
                    }
                }

                // If the script isn't assigned -> assign it!
                if (!existing)
                {
                    ScriptPointerList[id] = script;
                    sScriptMgr->IncrementScriptCount();
                } else
                {
                    // If the script is already assigned -> delete it!
                    sLog->outError(LOG_FILTER_TSCR, "Script '%s' already assigned with the same script name, so the script can't work.",
                                   script->GetName().c_str());

                    ASSERT(false); // Error that should be fixed ASAP.
                }
            } else
            {
                // The script uses a script name from database, but isn't assigned to anything.
                if (script->GetName().find("example") == std::string::npos && script->GetName().find("Smart") == std::string::npos)
                    sLog->outError(LOG_FILTER_SQL, "Script named '%s' does not have a script name assigned in database.",
                    script->GetName().c_str());
            }
        } else
        {
            // We're dealing with a code-only script; just add it.
            ScriptPointerList[_scriptIdCounter++] = script;
            sScriptMgr->IncrementScriptCount();
        }
    }

    // Gets a script by its ID (assigned by ObjectMgr).
    static TScript* GetScriptById(uint32 id)
    {
        ScriptMapIterator it = ScriptPointerList.find(id);
        if (it != ScriptPointerList.end())
            return it->second;

        return NULL;
    }

private:

    // Counter used for code-only scripts.
    static uint32 _scriptIdCounter;
};

// Utility macros to refer to the script registry.
#define SCR_REG_MAP(T) ScriptRegistry<T>::ScriptMap
#define SCR_REG_ITR(T) ScriptRegistry<T>::ScriptMapIterator
#define SCR_REG_LST(T) ScriptRegistry<T>::ScriptPointerList

// Utility macros for looping over scripts.
#define FOR_SCRIPTS(T, C, E) \
if (SCR_REG_LST(T).empty()) \
    return; \
for (SCR_REG_ITR(T) C = SCR_REG_LST(T).begin(); \
    C != SCR_REG_LST(T).end(); ++C)
#define FOR_SCRIPTS_RET(T, C, E, R) \
if (SCR_REG_LST(T).empty()) \
    return R; \
for (SCR_REG_ITR(T) C = SCR_REG_LST(T).begin(); \
    C != SCR_REG_LST(T).end(); ++C)
#define FOREACH_SCRIPT(T) \
    FOR_SCRIPTS(T, itr, end) \
    itr->second

// Utility macros for finding specific scripts.
#define GET_SCRIPT(T, I, V) \
    T* V = ScriptRegistry<T>::GetScriptById(I); \
if (!V) \
    return;
#define GET_SCRIPT_RET(T, I, V, R) \
    T* V = ScriptRegistry<T>::GetScriptById(I); \
if (!V) \
    return R;

void DoScriptText(int32 iTextEntry, WorldObject* pSource, Unit* target)
{
    if (!pSource)
    {
        sLog->outError(LOG_FILTER_TSCR, "DoScriptText entry %i, invalid Source pointer.", iTextEntry);
        return;
    }

    if (iTextEntry >= 0)
    {
        sLog->outError(LOG_FILTER_TSCR, "DoScriptText with source entry %u (TypeId=%u, guid=%u) attempts to process text entry %i, but text entry must be negative.", pSource->GetEntry(), pSource->GetTypeId(), pSource->GetGUIDLow(), iTextEntry);
        return;
    }

    const StringTextData* pData = sScriptSystemMgr->GetTextData(iTextEntry);

    if (!pData)
    {
        sLog->outError(LOG_FILTER_TSCR, "DoScriptText with source entry %u (TypeId=%u, guid=%u) could not find text entry %i.", pSource->GetEntry(), pSource->GetTypeId(), pSource->GetGUIDLow(), iTextEntry);
        return;
    }

    sLog->outDebug(LOG_FILTER_TSCR, "DoScriptText: text entry=%i, Sound=%u, Type=%u, Language=%u, Emote=%u", iTextEntry, pData->uiSoundId, pData->uiType, pData->uiLanguage, pData->uiEmote);

    if (pData->uiSoundId)
    {
        if (sSoundEntriesStore.LookupEntry(pData->uiSoundId))
            pSource->SendPlaySound(pData->uiSoundId, false);
        else
            sLog->outError(LOG_FILTER_TSCR, "DoScriptText entry %i tried to process invalid sound id %u.", iTextEntry, pData->uiSoundId);
    }

    if (pData->uiEmote)
    {
        if (pSource->GetTypeId() == TYPEID_UNIT || pSource->GetTypeId() == TYPEID_PLAYER)
            ((Unit*)pSource)->HandleEmoteCommand(pData->uiEmote);
        else
            sLog->outError(LOG_FILTER_TSCR, "DoScriptText entry %i tried to process emote for invalid TypeId (%u).", iTextEntry, pSource->GetTypeId());
    }

    switch (pData->uiType)
    {
        case CHAT_TYPE_SAY:
            pSource->MonsterSay(iTextEntry, pData->uiLanguage, target ? target->GetGUID() : 0);
            break;
        case CHAT_TYPE_YELL:
            pSource->MonsterYell(iTextEntry, pData->uiLanguage, target ? target->GetGUID() : 0);
            break;
        case CHAT_TYPE_TEXT_EMOTE:
            pSource->MonsterTextEmote(iTextEntry, target ? target->GetGUID() : 0);
            break;
        case CHAT_TYPE_BOSS_EMOTE:
            pSource->MonsterTextEmote(iTextEntry, target ? target->GetGUID() : 0, true);
            break;
        case CHAT_TYPE_WHISPER:
        {
                                  if (target && target->GetTypeId() == TYPEID_PLAYER)
                                      pSource->MonsterWhisper(iTextEntry, target->GetGUID());
                                  else
                                      sLog->outError(LOG_FILTER_TSCR, "DoScriptText entry %i cannot whisper without target unit (TYPEID_PLAYER).", iTextEntry);

                                  break;
        }
        case CHAT_TYPE_BOSS_WHISPER:
        {
                                       if (target && target->GetTypeId() == TYPEID_PLAYER)
                                           pSource->MonsterWhisper(iTextEntry, target->GetGUID(), true);
                                       else
                                           sLog->outError(LOG_FILTER_TSCR, "DoScriptText entry %i cannot whisper without target unit (TYPEID_PLAYER).", iTextEntry);

                                       break;
        }
        case CHAT_TYPE_ZONE_YELL:
            pSource->MonsterYellToZone(iTextEntry, pData->uiLanguage, target ? target->GetGUID() : 0);
            break;
    }
}

ScriptMgr::ScriptMgr()
: _scriptCount(0), _scheduledScripts(0)
{ }

ScriptMgr::~ScriptMgr()
{ }

void ScriptMgr::Initialize()
{
    uint32 oldMSTime = getMSTime();

    LoadDatabase();

    sLog->outInfo(LOG_FILTER_SERVER_LOADING, "Loading C++ scripts");

    FillSpellSummary();
    AddScripts();

    sLog->outInfo(LOG_FILTER_SERVER_LOADING, ">> Loaded %u C++ scripts in %u ms", GetScriptCount(), GetMSTimeDiffToNow(oldMSTime));
}

void ScriptMgr::Unload()
{
#define SCR_CLEAR(T) \
    for (SCR_REG_ITR(T) itr = SCR_REG_LST(T).begin(); itr != SCR_REG_LST(T).end(); ++itr) \
    delete itr->second; \
    SCR_REG_LST(T).clear();

    // Clear scripts for every script type.
    SCR_CLEAR(SpellScriptLoader);
    SCR_CLEAR(ServerScript);
    SCR_CLEAR(WorldScript);
    SCR_CLEAR(FormulaScript);
    SCR_CLEAR(WorldMapScript);
    SCR_CLEAR(InstanceMapScript);
    SCR_CLEAR(BattlegroundMapScript);
    SCR_CLEAR(ItemScript);
    SCR_CLEAR(CreatureScript);
    SCR_CLEAR(GameObjectScript);
    SCR_CLEAR(AreaTriggerScript);
    SCR_CLEAR(BattlegroundScript);
    SCR_CLEAR(OutdoorPvPScript);
    SCR_CLEAR(CommandScript);
    SCR_CLEAR(VehicleScript);
    SCR_CLEAR(TransportScript);
    SCR_CLEAR(AchievementCriteriaScript);
    SCR_CLEAR(PlayerScript);
    SCR_CLEAR(GroupScript);
    SCR_CLEAR(AuctionHouseScript);
    SCR_CLEAR(GuildScript);

#undef SCR_CLEAR
}

void ScriptMgr::LoadDatabase()
{
    sScriptSystemMgr->LoadScriptTexts();
    sScriptSystemMgr->LoadScriptTextsCustom();
    sScriptSystemMgr->LoadScriptWaypoints();
}

struct TSpellSummary
{
    uint8 Targets;                                          // set of enum SelectTarget
    uint8 Effects;                                          // set of enum SelectEffect
} *SpellSummary;

void ScriptMgr::FillSpellSummary()
{
    SpellSummary = new TSpellSummary[sSpellMgr->GetSpellInfoStoreSize()];

    SpellInfo const* pTempSpell;

    for (uint32 i = 0; i < sSpellMgr->GetSpellInfoStoreSize(); ++i)
    {
        SpellSummary[i].Effects = 0;
        SpellSummary[i].Targets = 0;

        pTempSpell = sSpellMgr->GetSpellInfo(i);
        // This spell doesn't exist.
        if (!pTempSpell)
            continue;

        for (uint32 j = 0; j < MAX_SPELL_EFFECTS; ++j)
        {
            // Spell targets self.
            if (pTempSpell->Effects[j].TargetA.GetTarget() == TARGET_UNIT_CASTER)
                SpellSummary[i].Targets |= 1 << (SELECT_TARGET_SELF-1);

            // Spell targets a single enemy.
            if (pTempSpell->Effects[j].TargetA.GetTarget() == TARGET_UNIT_TARGET_ENEMY ||
                pTempSpell->Effects[j].TargetA.GetTarget() == TARGET_DEST_TARGET_ENEMY)
                SpellSummary[i].Targets |= 1 << (SELECT_TARGET_SINGLE_ENEMY-1);

            // Spell targets AoE at enemy.
            if (pTempSpell->Effects[j].TargetA.GetTarget() == TARGET_UNIT_SRC_AREA_ENEMY ||
                pTempSpell->Effects[j].TargetA.GetTarget() == TARGET_UNIT_DEST_AREA_ENEMY ||
                pTempSpell->Effects[j].TargetA.GetTarget() == TARGET_SRC_CASTER ||
                pTempSpell->Effects[j].TargetA.GetTarget() == TARGET_DEST_DYNOBJ_ENEMY)
                SpellSummary[i].Targets |= 1 << (SELECT_TARGET_AOE_ENEMY-1);

            // Spell targets an enemy.
            if (pTempSpell->Effects[j].TargetA.GetTarget() == TARGET_UNIT_TARGET_ENEMY ||
                pTempSpell->Effects[j].TargetA.GetTarget() == TARGET_DEST_TARGET_ENEMY ||
                pTempSpell->Effects[j].TargetA.GetTarget() == TARGET_UNIT_SRC_AREA_ENEMY ||
                pTempSpell->Effects[j].TargetA.GetTarget() == TARGET_UNIT_DEST_AREA_ENEMY ||
                pTempSpell->Effects[j].TargetA.GetTarget() == TARGET_SRC_CASTER ||
                pTempSpell->Effects[j].TargetA.GetTarget() == TARGET_DEST_DYNOBJ_ENEMY)
                SpellSummary[i].Targets |= 1 << (SELECT_TARGET_ANY_ENEMY-1);

            // Spell targets a single friend (or self).
            if (pTempSpell->Effects[j].TargetA.GetTarget() == TARGET_UNIT_CASTER ||
                pTempSpell->Effects[j].TargetA.GetTarget() == TARGET_UNIT_TARGET_ALLY ||
                pTempSpell->Effects[j].TargetA.GetTarget() == TARGET_UNIT_TARGET_PARTY)
                SpellSummary[i].Targets |= 1 << (SELECT_TARGET_SINGLE_FRIEND-1);

            // Spell targets AoE friends.
            if (pTempSpell->Effects[j].TargetA.GetTarget() == TARGET_UNIT_CASTER_AREA_PARTY ||
                pTempSpell->Effects[j].TargetA.GetTarget() == TARGET_UNIT_LASTTARGET_AREA_PARTY ||
                pTempSpell->Effects[j].TargetA.GetTarget() == TARGET_SRC_CASTER)
                SpellSummary[i].Targets |= 1 << (SELECT_TARGET_AOE_FRIEND-1);

            // Spell targets any friend (or self).
            if (pTempSpell->Effects[j].TargetA.GetTarget() == TARGET_UNIT_CASTER ||
                pTempSpell->Effects[j].TargetA.GetTarget() == TARGET_UNIT_TARGET_ALLY ||
                pTempSpell->Effects[j].TargetA.GetTarget() == TARGET_UNIT_TARGET_PARTY ||
                pTempSpell->Effects[j].TargetA.GetTarget() == TARGET_UNIT_CASTER_AREA_PARTY ||
                pTempSpell->Effects[j].TargetA.GetTarget() == TARGET_UNIT_LASTTARGET_AREA_PARTY ||
                pTempSpell->Effects[j].TargetA.GetTarget() == TARGET_SRC_CASTER)
                SpellSummary[i].Targets |= 1 << (SELECT_TARGET_ANY_FRIEND-1);

            // Make sure that this spell includes a damage effect.
            if (pTempSpell->Effects[j].Effect == SPELL_EFFECT_SCHOOL_DAMAGE ||
                pTempSpell->Effects[j].Effect == SPELL_EFFECT_INSTAKILL ||
                pTempSpell->Effects[j].Effect == SPELL_EFFECT_ENVIRONMENTAL_DAMAGE ||
                pTempSpell->Effects[j].Effect == SPELL_EFFECT_HEALTH_LEECH)
                SpellSummary[i].Effects |= 1 << (SELECT_EFFECT_DAMAGE-1);

            // Make sure that this spell includes a healing effect (or an apply aura with a periodic heal).
            if (pTempSpell->Effects[j].Effect == SPELL_EFFECT_HEAL ||
                pTempSpell->Effects[j].Effect == SPELL_EFFECT_HEAL_MAX_HEALTH ||
                pTempSpell->Effects[j].Effect == SPELL_EFFECT_HEAL_MECHANICAL ||
                (pTempSpell->Effects[j].Effect == SPELL_EFFECT_APPLY_AURA  && pTempSpell->Effects[j].ApplyAuraName == 8))
                SpellSummary[i].Effects |= 1 << (SELECT_EFFECT_HEALING-1);

            // Make sure that this spell applies an aura.
            if (pTempSpell->Effects[j].Effect == SPELL_EFFECT_APPLY_AURA)
                SpellSummary[i].Effects |= 1 << (SELECT_EFFECT_AURA-1);
        }
    }
}

void ScriptMgr::CreateSpellScripts(uint32 spellId, std::list<SpellScript*>& scriptVector)
{
    SpellScriptsBounds bounds = sObjectMgr->GetSpellScriptsBounds(spellId);

    for (SpellScriptsContainer::iterator itr = bounds.first; itr != bounds.second; ++itr)
    {
        SpellScriptLoader* tmpscript = ScriptRegistry<SpellScriptLoader>::GetScriptById(itr->second);
        if (!tmpscript)
            continue;

        SpellScript* script = tmpscript->GetSpellScript();

        if (!script)
            continue;

        script->_Init(&tmpscript->GetName(), spellId);

        scriptVector.push_back(script);
    }
}

void ScriptMgr::CreateAuraScripts(uint32 spellId, std::list<AuraScript*>& scriptVector)
{
    SpellScriptsBounds bounds = sObjectMgr->GetSpellScriptsBounds(spellId);

    for (SpellScriptsContainer::iterator itr = bounds.first; itr != bounds.second; ++itr)
    {
        SpellScriptLoader* tmpscript = ScriptRegistry<SpellScriptLoader>::GetScriptById(itr->second);
        if (!tmpscript)
            continue;

        AuraScript* script = tmpscript->GetAuraScript();

        if (!script)
            continue;

        script->_Init(&tmpscript->GetName(), spellId);

        scriptVector.push_back(script);
    }
}

void ScriptMgr::CreateSpellScriptLoaders(uint32 spellId, std::vector<std::pair<SpellScriptLoader*, SpellScriptsContainer::iterator> >& scriptVector)
{
    SpellScriptsBounds bounds = sObjectMgr->GetSpellScriptsBounds(spellId);
    scriptVector.reserve(std::distance(bounds.first, bounds.second));

    for (SpellScriptsContainer::iterator itr = bounds.first; itr != bounds.second; ++itr)
    {
        SpellScriptLoader* tmpscript = ScriptRegistry<SpellScriptLoader>::GetScriptById(itr->second);
        if (!tmpscript)
            continue;

        scriptVector.push_back(std::make_pair(tmpscript, itr));
    }
}

void ScriptMgr::OnOpenStateChange(bool open)
{
    FOREACH_SCRIPT(WorldScript)->OnOpenStateChange(open);
}

void ScriptMgr::OnConfigLoad(bool reload)
{
    FOREACH_SCRIPT(WorldScript)->OnConfigLoad(reload);
}

void ScriptMgr::OnMotdChange(std::string& newMotd)
{
    FOREACH_SCRIPT(WorldScript)->OnMotdChange(newMotd);
}

void ScriptMgr::OnShutdownInitiate(ShutdownExitCode code, ShutdownMask mask)
{
    FOREACH_SCRIPT(WorldScript)->OnShutdownInitiate(code, mask);
}

void ScriptMgr::OnShutdownCancel()
{
    FOREACH_SCRIPT(WorldScript)->OnShutdownCancel();
}

void ScriptMgr::OnWorldUpdate(uint32 diff)
{
    FOREACH_SCRIPT(WorldScript)->OnUpdate(diff);
}

InstanceScript* ScriptMgr::CreateInstanceData(InstanceMap* map)
{
    ASSERT(map);

    GET_SCRIPT_RET(InstanceMapScript, map->GetScriptId(), tmpscript, NULL);
    return tmpscript->GetInstanceScript(map);
}

bool ScriptMgr::OnItemUse(Player* player, Item* item, SpellCastTargets const& targets)
{
    ASSERT(player);
    ASSERT(item);

    GET_SCRIPT_RET(ItemScript, item->GetScriptId(), tmpscript, false);
    return tmpscript->OnUse(player, item, targets);
}

bool ScriptMgr::OnItemExpire(Player* player, ItemTemplate const* proto)
{
    ASSERT(player);
    ASSERT(proto);

    GET_SCRIPT_RET(ItemScript, proto->ScriptId, tmpscript, false);
    return tmpscript->OnExpire(player, proto);
}

bool ScriptMgr::OnGossipHello(Player* player, Creature* creature)
{
    ASSERT(player);
    ASSERT(creature);

    GET_SCRIPT_RET(CreatureScript, creature->GetScriptId(), tmpscript, false);
    player->PlayerTalkClass->ClearMenus();
    return tmpscript->OnGossipHello(player, creature);
}

bool ScriptMgr::OnGossipSelect(Player* player, Creature* creature, uint32 sender, uint32 action)
{
    ASSERT(player);
    ASSERT(creature);

    GET_SCRIPT_RET(CreatureScript, creature->GetScriptId(), tmpscript, false);
    return tmpscript->OnGossipSelect(player, creature, sender, action);
}

bool ScriptMgr::OnGossipSelectCode(Player* player, Creature* creature, uint32 sender, uint32 action, const char* code)
{
    ASSERT(player);
    ASSERT(creature);
    ASSERT(code);

    GET_SCRIPT_RET(CreatureScript, creature->GetScriptId(), tmpscript, false);
    return tmpscript->OnGossipSelectCode(player, creature, sender, action, code);
}

bool ScriptMgr::OnQuestAccept(Player* player, Creature* creature, Quest const* quest)
{
    ASSERT(player);
    ASSERT(creature);
    ASSERT(quest);

    GET_SCRIPT_RET(CreatureScript, creature->GetScriptId(), tmpscript, false);
    player->PlayerTalkClass->ClearMenus();
    return tmpscript->OnQuestAccept(player, creature, quest);
}

bool ScriptMgr::OnQuestSelect(Player* player, Creature* creature, Quest const* quest)
{
    ASSERT(player);
    ASSERT(creature);
    ASSERT(quest);

    GET_SCRIPT_RET(CreatureScript, creature->GetScriptId(), tmpscript, false);
    player->PlayerTalkClass->ClearMenus();
    return tmpscript->OnQuestSelect(player, creature, quest);
}

bool ScriptMgr::OnQuestComplete(Player* player, Creature* creature, Quest const* quest)
{
    ASSERT(player);
    ASSERT(creature);
    ASSERT(quest);

    GET_SCRIPT_RET(CreatureScript, creature->GetScriptId(), tmpscript, false);
    player->PlayerTalkClass->ClearMenus();
    return tmpscript->OnQuestComplete(player, creature, quest);
}

bool ScriptMgr::OnQuestReward(Player* player, Creature* creature, Quest const* quest, uint32 opt)
{
    ASSERT(player);
    ASSERT(creature);
    ASSERT(quest);

    GET_SCRIPT_RET(CreatureScript, creature->GetScriptId(), tmpscript, false);
    player->PlayerTalkClass->ClearMenus();
    return tmpscript->OnQuestReward(player, creature, quest, opt);
}

uint32 ScriptMgr::GetDialogStatus(Player* player, Creature* creature)
{
    ASSERT(player);
    ASSERT(creature);

    // TODO: 100 is a funny magic number to have hanging around here...
    GET_SCRIPT_RET(CreatureScript, creature->GetScriptId(), tmpscript, 100);
    player->PlayerTalkClass->ClearMenus();
    return tmpscript->GetDialogStatus(player, creature);
}

CreatureAI* ScriptMgr::GetCreatureAI(Creature* creature)
{
    ASSERT(creature);

    GET_SCRIPT_RET(CreatureScript, creature->GetScriptId(), tmpscript, NULL);
    return tmpscript->GetAI(creature);
}

GameObjectAI* ScriptMgr::GetGameObjectAI(GameObject* gameobject)
{
    ASSERT(gameobject);

    GET_SCRIPT_RET(GameObjectScript, gameobject->GetScriptId(), tmpscript, NULL);
    return tmpscript->GetAI(gameobject);
}

void ScriptMgr::OnCreatureUpdate(Creature* creature, uint32 diff)
{
    ASSERT(creature);

    GET_SCRIPT(CreatureScript, creature->GetScriptId(), tmpscript);
    tmpscript->OnUpdate(creature, diff);
}

bool ScriptMgr::OnGossipHello(Player* player, GameObject* go)
{
    ASSERT(player);
    ASSERT(go);

    GET_SCRIPT_RET(GameObjectScript, go->GetScriptId(), tmpscript, false);
    player->PlayerTalkClass->ClearMenus();
    return tmpscript->OnGossipHello(player, go);
}

bool ScriptMgr::OnGossipSelect(Player* player, GameObject* go, uint32 sender, uint32 action)
{
    ASSERT(player);
    ASSERT(go);

    GET_SCRIPT_RET(GameObjectScript, go->GetScriptId(), tmpscript, false);
    return tmpscript->OnGossipSelect(player, go, sender, action);
}

bool ScriptMgr::OnGossipSelectCode(Player* player, GameObject* go, uint32 sender, uint32 action, const char* code)
{
    ASSERT(player);
    ASSERT(go);
    ASSERT(code);

    GET_SCRIPT_RET(GameObjectScript, go->GetScriptId(), tmpscript, false);
    return tmpscript->OnGossipSelectCode(player, go, sender, action, code);
}

bool ScriptMgr::OnQuestAccept(Player* player, GameObject* go, Quest const* quest)
{
    ASSERT(player);
    ASSERT(go);
    ASSERT(quest);

    GET_SCRIPT_RET(GameObjectScript, go->GetScriptId(), tmpscript, false);
    player->PlayerTalkClass->ClearMenus();
    return tmpscript->OnQuestAccept(player, go, quest);
}

bool ScriptMgr::OnQuestComplete(Player* player, GameObject* go, Quest const* quest)
{
    ASSERT(player);
    ASSERT(go);
    ASSERT(quest);

    GET_SCRIPT_RET(GameObjectScript, go->GetScriptId(), tmpscript, false);
    player->PlayerTalkClass->ClearMenus();
    return tmpscript->OnQuestComplete(player, go, quest);
}

bool ScriptMgr::OnQuestReward(Player* player, GameObject* go, Quest const* quest, uint32 opt)
{
    ASSERT(player);
    ASSERT(go);
    ASSERT(quest);

    GET_SCRIPT_RET(GameObjectScript, go->GetScriptId(), tmpscript, false);
    player->PlayerTalkClass->ClearMenus();
    return tmpscript->OnQuestReward(player, go, quest, opt);
}

uint32 ScriptMgr::GetDialogStatus(Player* player, GameObject* go)
{
    ASSERT(player);
    ASSERT(go);

    // TODO: 100 is a funny magic number to have hanging around here...
    GET_SCRIPT_RET(GameObjectScript, go->GetScriptId(), tmpscript, 100);
    player->PlayerTalkClass->ClearMenus();
    return tmpscript->GetDialogStatus(player, go);
}

void ScriptMgr::OnGameObjectDestroyed(GameObject* go, Player* player)
{
    ASSERT(go);

    GET_SCRIPT(GameObjectScript, go->GetScriptId(), tmpscript);
    tmpscript->OnDestroyed(go, player);
}

void ScriptMgr::OnGameObjectDamaged(GameObject* go, Player* player)
{
    ASSERT(go);

    GET_SCRIPT(GameObjectScript, go->GetScriptId(), tmpscript);
    tmpscript->OnDamaged(go, player);
}

void ScriptMgr::OnGameObjectLootStateChanged(GameObject* go, uint32 state, Unit* unit)
{
    ASSERT(go);

    GET_SCRIPT(GameObjectScript, go->GetScriptId(), tmpscript);
    tmpscript->OnLootStateChanged(go, state, unit);
}

void ScriptMgr::OnGameObjectStateChanged(GameObject* go, uint32 state)
{
    ASSERT(go);

    GET_SCRIPT(GameObjectScript, go->GetScriptId(), tmpscript);
    tmpscript->OnGameObjectStateChanged(go, state);
}

void ScriptMgr::OnGameObjectUpdate(GameObject* go, uint32 diff)
{
    ASSERT(go);

    GET_SCRIPT(GameObjectScript, go->GetScriptId(), tmpscript);
    tmpscript->OnUpdate(go, diff);
}

bool ScriptMgr::OnAreaTrigger(Player* player, AreaTriggerEntry const* trigger)
{
    ASSERT(player);
    ASSERT(trigger);

    GET_SCRIPT_RET(AreaTriggerScript, sObjectMgr->GetAreaTriggerScriptId(trigger->id), tmpscript, false);
    return tmpscript->OnTrigger(player, trigger);
}

Battleground* ScriptMgr::CreateBattleground(BattlegroundTypeId /*typeId*/)
{
    // TODO: Implement script-side battlegrounds.
    ASSERT(false);
    return NULL;
}

OutdoorPvP* ScriptMgr::CreateOutdoorPvP(OutdoorPvPData const* data)
{
    ASSERT(data);

    GET_SCRIPT_RET(OutdoorPvPScript, data->ScriptId, tmpscript, NULL);
    return tmpscript->GetOutdoorPvP();
}

std::vector<ChatCommand*> ScriptMgr::GetChatCommands()
{
    std::vector<ChatCommand*> table;

    FOR_SCRIPTS_RET(CommandScript, itr, end, table)
        table.push_back(itr->second->GetCommands());

    return table;
}

void ScriptMgr::OnInstall(Vehicle* veh)
{
    ASSERT(veh);
    ASSERT(veh->GetBase()->GetTypeId() == TYPEID_UNIT);

    GET_SCRIPT(VehicleScript, veh->GetBase()->ToCreature()->GetScriptId(), tmpscript);
    tmpscript->OnInstall(veh);
}

void ScriptMgr::OnUninstall(Vehicle* veh)
{
    ASSERT(veh);
    ASSERT(veh->GetBase()->GetTypeId() == TYPEID_UNIT);

    GET_SCRIPT(VehicleScript, veh->GetBase()->ToCreature()->GetScriptId(), tmpscript);
    tmpscript->OnUninstall(veh);
}

void ScriptMgr::OnReset(Vehicle* veh)
{
    ASSERT(veh);
    ASSERT(veh->GetBase()->GetTypeId() == TYPEID_UNIT);

    GET_SCRIPT(VehicleScript, veh->GetBase()->ToCreature()->GetScriptId(), tmpscript);
    tmpscript->OnReset(veh);
}

void ScriptMgr::OnInstallAccessory(Vehicle* veh, Creature* accessory)
{
    ASSERT(veh);
    ASSERT(veh->GetBase()->GetTypeId() == TYPEID_UNIT);
    ASSERT(accessory);

    GET_SCRIPT(VehicleScript, veh->GetBase()->ToCreature()->GetScriptId(), tmpscript);
    tmpscript->OnInstallAccessory(veh, accessory);
}

void ScriptMgr::OnAddPassenger(Vehicle* veh, Unit* passenger, int8 seatId)
{
    ASSERT(veh);
    ASSERT(veh->GetBase()->GetTypeId() == TYPEID_UNIT);
    ASSERT(passenger);

    GET_SCRIPT(VehicleScript, veh->GetBase()->ToCreature()->GetScriptId(), tmpscript);
    tmpscript->OnAddPassenger(veh, passenger, seatId);
}

void ScriptMgr::OnRemovePassenger(Vehicle* veh, Unit* passenger)
{
    ASSERT(veh);
    ASSERT(veh->GetBase()->GetTypeId() == TYPEID_UNIT);
    ASSERT(passenger);

    GET_SCRIPT(VehicleScript, veh->GetBase()->ToCreature()->GetScriptId(), tmpscript);
    tmpscript->OnRemovePassenger(veh, passenger);
}

void ScriptMgr::OnAddPassenger(Transport* transport, Player* player)
{
    ASSERT(transport);
    ASSERT(player);

    GET_SCRIPT(TransportScript, transport->GetScriptId(), tmpscript);
    tmpscript->OnAddPassenger(transport, player);
}

void ScriptMgr::OnAddCreaturePassenger(Transport* transport, Creature* creature)
{
    ASSERT(transport);
    ASSERT(creature);

    GET_SCRIPT(TransportScript, transport->GetScriptId(), tmpscript);
    tmpscript->OnAddCreaturePassenger(transport, creature);
}

void ScriptMgr::OnRemovePassenger(Transport* transport, Player* player)
{
    ASSERT(transport);
    ASSERT(player);

    GET_SCRIPT(TransportScript, transport->GetScriptId(), tmpscript);
    tmpscript->OnRemovePassenger(transport, player);
}

void ScriptMgr::OnTransportUpdate(Transport* transport, uint32 diff)
{
    ASSERT(transport);

    GET_SCRIPT(TransportScript, transport->GetScriptId(), tmpscript);
    tmpscript->OnUpdate(transport, diff);
}

void ScriptMgr::OnRelocate(Transport* transport, uint32 waypointId, uint32 mapId, float x, float y, float z)
{
    GET_SCRIPT(TransportScript, transport->GetScriptId(), tmpscript);
    tmpscript->OnRelocate(transport, waypointId, mapId, x, y, z);
}

bool ScriptMgr::OnCriteriaCheck(uint32 scriptId, Player* source, Unit* target)
{
    ASSERT(source);
    // target can be NULL.

    GET_SCRIPT_RET(AchievementCriteriaScript, scriptId, tmpscript, false);
    return tmpscript->OnCheck(source, target);
}

void ScriptMgr::OnPlayerEnterCombat(Player* player, Unit* enemy)
{
    FOREACH_SCRIPT(PlayerScript)->OnPlayerEnterCombat(player, enemy);
}

void ScriptMgr::OnPlayerLeaveCombat(Player* player)
{
    FOREACH_SCRIPT(PlayerScript)->OnPlayerLeaveCombat(player);
}

void ScriptMgr::OnPlayerStartWatchingMovie(Player* player)
{
    FOREACH_SCRIPT(PlayerScript)->OnStartWatching(player);
}

void ScriptMgr::OnPlayerEndWatchingMovie(Player* player)
{
    FOREACH_SCRIPT(PlayerScript)->OnEndWatching(player);
}

// Player

void ScriptMgr::OnPVPKill(Player* killer, Player* killed)
{
    FOREACH_SCRIPT(PlayerScript)->OnPVPKill(killer, killed);
}

void ScriptMgr::OnCreatureKill(Player* killer, Creature* killed)
{
    FOREACH_SCRIPT(PlayerScript)->OnCreatureKill(killer, killed);
}

void ScriptMgr::OnPlayerKilledByCreature(Creature* killer, Player* killed)
{
    FOREACH_SCRIPT(PlayerScript)->OnPlayerKilledByCreature(killer, killed);
}

void ScriptMgr::OnPlayerLevelChanged(Player* player, uint8 oldLevel)
{
    FOREACH_SCRIPT(PlayerScript)->OnLevelChanged(player, oldLevel);
}

void ScriptMgr::OnPlayerDuelRequest(Player* target, Player* challenger)
{
    FOREACH_SCRIPT(PlayerScript)->OnDuelRequest(target, challenger);
}

void ScriptMgr::OnPlayerDuelStart(Player* player1, Player* player2)
{
    FOREACH_SCRIPT(PlayerScript)->OnDuelStart(player1, player2);
}

void ScriptMgr::OnPlayerDuelEnd(Player* winner, Player* loser, DuelCompleteType type)
{
    FOREACH_SCRIPT(PlayerScript)->OnDuelEnd(winner, loser, type);
}

void ScriptMgr::OnPlayerChat(Player* player, uint32 type, uint32 lang, std::string& msg)
{
    FOREACH_SCRIPT(PlayerScript)->OnChat(player, type, lang, msg);
}

void ScriptMgr::OnPlayerChat(Player* player, uint32 type, uint32 lang, std::string& msg, Player* receiver)
{
    FOREACH_SCRIPT(PlayerScript)->OnChat(player, type, lang, msg, receiver);
}

void ScriptMgr::OnPlayerChat(Player* player, uint32 type, uint32 lang, std::string& msg, Group* group)
{
    FOREACH_SCRIPT(PlayerScript)->OnChat(player, type, lang, msg, group);
}

void ScriptMgr::OnPlayerChat(Player* player, uint32 type, uint32 lang, std::string& msg, Guild* guild)
{
    FOREACH_SCRIPT(PlayerScript)->OnChat(player, type, lang, msg, guild);
}

void ScriptMgr::OnPlayerChat(Player* player, uint32 type, uint32 lang, std::string& msg, Channel* channel)
{
    FOREACH_SCRIPT(PlayerScript)->OnChat(player, type, lang, msg, channel);
}

void ScriptMgr::OnPlayerEmote(Player* player, uint32 emote)
{
    FOREACH_SCRIPT(PlayerScript)->OnEmote(player, emote);
}

void ScriptMgr::OnPlayerTextEmote(Player* player, uint32 textEmote, uint32 emoteNum, uint64 guid)
{
    FOREACH_SCRIPT(PlayerScript)->OnTextEmote(player, textEmote, emoteNum, guid);
}

void ScriptMgr::OnPlayerSpellCast(Player* player, Spell* spell, bool skipCheck)
{
    FOREACH_SCRIPT(PlayerScript)->OnSpellCast(player, spell, skipCheck);
}

void ScriptMgr::OnPlayerLogin(Player* player)
{
    FOREACH_SCRIPT(PlayerScript)->OnLogin(player);
}

void ScriptMgr::OnPlayerLogout(Player* player)
{
    FOREACH_SCRIPT(PlayerScript)->OnLogout(player);
}

void ScriptMgr::OnPlayerBindToInstance(Player* player, Difficulty difficulty, uint32 mapid, bool permanent)
{
    FOREACH_SCRIPT(PlayerScript)->OnBindToInstance(player, difficulty, mapid, permanent);
}

// Group
void ScriptMgr::OnGroupAddMember(Group* group, uint64 guid)
{
    ASSERT(group);
    FOREACH_SCRIPT(GroupScript)->OnAddMember(group, guid);
}

void ScriptMgr::OnGroupInviteMember(Group* group, uint64 guid)
{
    ASSERT(group);
    FOREACH_SCRIPT(GroupScript)->OnInviteMember(group, guid);
}

void ScriptMgr::OnGroupRemoveMember(Group* group, uint64 guid, RemoveMethod method, uint64 kicker, const char* reason)
{
    ASSERT(group);
    FOREACH_SCRIPT(GroupScript)->OnRemoveMember(group, guid, method, kicker, reason);
}

void ScriptMgr::OnGroupChangeLeader(Group* group, uint64 newLeaderGuid, uint64 oldLeaderGuid)
{
    ASSERT(group);
    FOREACH_SCRIPT(GroupScript)->OnChangeLeader(group, newLeaderGuid, oldLeaderGuid);
}

void ScriptMgr::OnGroupDisband(Group* group)
{
    ASSERT(group);
    FOREACH_SCRIPT(GroupScript)->OnDisband(group);
}

SpellScriptLoader::SpellScriptLoader(const char* name)
: ScriptObject(name)
{
    ScriptRegistry<SpellScriptLoader>::AddScript(this);
}

WorldScript::WorldScript(const char* name)
: ScriptObject(name)
{
    ScriptRegistry<WorldScript>::AddScript(this);
}

AuctionHouseScript::AuctionHouseScript(const char* name)
: ScriptObject(name)
{
    ScriptRegistry<AuctionHouseScript>::AddScript(this);
}

GuildScript::GuildScript(const char* name)
: ScriptObject(name)
{
    ScriptRegistry<GuildScript>::AddScript(this);
}

ServerScript::ServerScript(const char* name)
: ScriptObject(name)
{
    ScriptRegistry<ServerScript>::AddScript(this);
}

FormulaScript::FormulaScript(const char* name)
: ScriptObject(name)
{
    ScriptRegistry<FormulaScript>::AddScript(this);
}

WorldMapScript::WorldMapScript(const char* name, uint32 mapId)
: ScriptObject(name), MapScript<Map>(mapId)
{
    if (GetEntry() && !GetEntry()->IsWorldMap())
        sLog->outError(LOG_FILTER_TSCR, "WorldMapScript for map %u is invalid.", mapId);

    ScriptRegistry<WorldMapScript>::AddScript(this);
}

InstanceMapScript::InstanceMapScript(const char* name, uint32 mapId)
: ScriptObject(name), MapScript<InstanceMap>(mapId)
{
    if (GetEntry() && !GetEntry()->IsDungeon())
        sLog->outError(LOG_FILTER_TSCR, "InstanceMapScript for map %u is invalid.", mapId);

    ScriptRegistry<InstanceMapScript>::AddScript(this);
}

BattlegroundMapScript::BattlegroundMapScript(const char* name, uint32 mapId)
: ScriptObject(name), MapScript<BattlegroundMap>(mapId)
{
    if (GetEntry() && !GetEntry()->IsBattleground())
        sLog->outError(LOG_FILTER_TSCR, "BattlegroundMapScript for map %u is invalid.", mapId);

    ScriptRegistry<BattlegroundMapScript>::AddScript(this);
}

ItemScript::ItemScript(const char* name)
: ScriptObject(name)
{
    ScriptRegistry<ItemScript>::AddScript(this);
}

CreatureScript::CreatureScript(const char* name)
: ScriptObject(name)
{
    ScriptRegistry<CreatureScript>::AddScript(this);
}

GameObjectScript::GameObjectScript(const char* name)
: ScriptObject(name)
{
    ScriptRegistry<GameObjectScript>::AddScript(this);
}

AreaTriggerScript::AreaTriggerScript(const char* name)
: ScriptObject(name)
{
    ScriptRegistry<AreaTriggerScript>::AddScript(this);
}

BattlegroundScript::BattlegroundScript(const char* name)
: ScriptObject(name)
{
    ScriptRegistry<BattlegroundScript>::AddScript(this);
}

OutdoorPvPScript::OutdoorPvPScript(const char* name)
: ScriptObject(name)
{
    ScriptRegistry<OutdoorPvPScript>::AddScript(this);
}

CommandScript::CommandScript(const char* name)
: ScriptObject(name)
{
    ScriptRegistry<CommandScript>::AddScript(this);
}

VehicleScript::VehicleScript(const char* name)
: ScriptObject(name)
{
    ScriptRegistry<VehicleScript>::AddScript(this);
}

TransportScript::TransportScript(const char* name)
: ScriptObject(name)
{
    ScriptRegistry<TransportScript>::AddScript(this);
}

AchievementCriteriaScript::AchievementCriteriaScript(const char* name)
: ScriptObject(name)
{
    ScriptRegistry<AchievementCriteriaScript>::AddScript(this);
}

PlayerScript::PlayerScript(const char* name)
: ScriptObject(name)
{
    ScriptRegistry<PlayerScript>::AddScript(this);
}

GroupScript::GroupScript(const char* name)
: ScriptObject(name)
{
    ScriptRegistry<GroupScript>::AddScript(this);
}

// Instantiate static members of ScriptRegistry.
template<class TScript> std::map<uint32, TScript*> ScriptRegistry<TScript>::ScriptPointerList;
template<class TScript> uint32 ScriptRegistry<TScript>::_scriptIdCounter = 0;

// Specialize for each script type class like so:
template class ScriptRegistry<SpellScriptLoader>;
template class ScriptRegistry<WorldScript>;
template class ScriptRegistry<WorldMapScript>;
template class ScriptRegistry<InstanceMapScript>;
template class ScriptRegistry<BattlegroundMapScript>;
template class ScriptRegistry<ItemScript>;
template class ScriptRegistry<CreatureScript>;
template class ScriptRegistry<GameObjectScript>;
template class ScriptRegistry<AreaTriggerScript>;
template class ScriptRegistry<BattlegroundScript>;
template class ScriptRegistry<OutdoorPvPScript>;
template class ScriptRegistry<CommandScript>;
template class ScriptRegistry<VehicleScript>;
template class ScriptRegistry<TransportScript>;
template class ScriptRegistry<AchievementCriteriaScript>;
template class ScriptRegistry<PlayerScript>;
template class ScriptRegistry<GroupScript>;

// Undefine utility macros.
#undef GET_SCRIPT_RET
#undef GET_SCRIPT
#undef FOREACH_SCRIPT
#undef FOR_SCRIPTS_RET
#undef FOR_SCRIPTS
#undef SCR_REG_LST
#undef SCR_REG_ITR
#undef SCR_REG_MAP
