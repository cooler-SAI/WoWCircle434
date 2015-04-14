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

#include "Common.h"
#include "CreatureAIImpl.h"
#include "Log.h"
#include "Opcodes.h"
#include "WorldPacket.h"
#include "MovementStructures.h"
#include "WorldSession.h"
#include "World.h"
#include "ObjectMgr.h"
#include "SpellMgr.h"
#include "Unit.h"
#include "QuestDef.h"
#include "Player.h"
#include "Creature.h"
#include "Spell.h"
#include "Group.h"
#include "SpellAuras.h"
#include "SpellAuraEffects.h"
#include "MapManager.h"
#include "ObjectAccessor.h"
#include "CreatureAI.h"
#include "Formulas.h"
#include "Pet.h"
#include "Util.h"
#include "Totem.h"
#include "Battleground.h"
#include "OutdoorPvP.h"
#include "InstanceSaveMgr.h"
#include "GridNotifiersImpl.h"
#include "CellImpl.h"
#include "CreatureGroups.h"
#include "PetAI.h"
#include "PassiveAI.h"
#include "TemporarySummon.h"
#include "Vehicle.h"
#include "Transport.h"
#include "InstanceScript.h"
#include "SpellInfo.h"
#include "MoveSplineInit.h"
#include "MoveSpline.h"
#include "ConditionMgr.h"
#include "UpdateFieldFlags.h"
#include "Battlefield.h"
#include "BattlefieldMgr.h"

#include <math.h>

float baseMoveSpeed[MAX_MOVE_TYPE] =
{
    2.5f,                  // MOVE_WALK
    7.0f,                  // MOVE_RUN
    4.5f,                  // MOVE_RUN_BACK
    4.722222f,             // MOVE_SWIM
    2.5f,                  // MOVE_SWIM_BACK
    3.141594f,             // MOVE_TURN_RATE
    7.0f,                  // MOVE_FLIGHT
    4.5f,                  // MOVE_FLIGHT_BACK
    3.14f                  // MOVE_PITCH_RATE
};

float playerBaseMoveSpeed[MAX_MOVE_TYPE] =
{
    2.5f,                  // MOVE_WALK
    7.0f,                  // MOVE_RUN
    4.5f,                  // MOVE_RUN_BACK
    4.722222f,             // MOVE_SWIM
    2.5f,                  // MOVE_SWIM_BACK
    3.141594f,             // MOVE_TURN_RATE
    7.0f,                  // MOVE_FLIGHT
    4.5f,                  // MOVE_FLIGHT_BACK
    3.14f                  // MOVE_PITCH_RATE
};

// Used for prepare can/can`t triggr aura
static bool InitTriggerAuraData();
// Define can trigger auras
static bool isTriggerAura[TOTAL_AURAS];
// Define can't trigger auras (need for disable second trigger)
static bool isNonTriggerAura[TOTAL_AURAS];
// Triggered always, even from triggered spells
static bool isAlwaysTriggeredAura[TOTAL_AURAS];
// Prepare lists
static bool procPrepared = InitTriggerAuraData();

DamageInfo::DamageInfo(Unit* _attacker, Unit* _victim, uint32 _damage, SpellInfo const* _spellInfo, SpellSchoolMask _schoolMask, DamageEffectType _damageType)
: m_attacker(_attacker), m_victim(_victim), m_damage(_damage), m_spellInfo(_spellInfo), m_schoolMask(_schoolMask),
m_damageType(_damageType), m_attackType(BASE_ATTACK)
{
    m_absorb = 0;
    m_resist = 0;
    m_block = 0;
}
DamageInfo::DamageInfo(CalcDamageInfo& dmgInfo)
: m_attacker(dmgInfo.attacker), m_victim(dmgInfo.target), m_damage(dmgInfo.damage), m_spellInfo(NULL), m_schoolMask(SpellSchoolMask(dmgInfo.damageSchoolMask)),
m_damageType(DIRECT_DAMAGE), m_attackType(dmgInfo.attackType)
{
    m_absorb = 0;
    m_resist = 0;
    m_block = 0;
}

void DamageInfo::ModifyDamage(int32 amount)
{
    amount = std::min(amount, int32(GetDamage()));
    m_damage += amount;
}

void DamageInfo::AbsorbDamage(uint32 amount)
{
    amount = std::min(amount, GetDamage());
    m_absorb += amount;
    m_damage -= amount;
}

void DamageInfo::ResistDamage(uint32 amount)
{
    amount = std::min(amount, GetDamage());
    m_resist += amount;
    m_damage -= amount;
}

void DamageInfo::BlockDamage(uint32 amount)
{
    amount = std::min(amount, GetDamage());
    m_block += amount;
    m_damage -= amount;
}

ProcEventInfo::ProcEventInfo(Unit* actor, Unit* actionTarget, Unit* procTarget, uint32 typeMask, uint32 spellTypeMask, uint32 spellPhaseMask, uint32 hitMask, Spell* spell, DamageInfo* damageInfo, HealInfo* healInfo)
:_actor(actor), _actionTarget(actionTarget), _procTarget(procTarget), _typeMask(typeMask), _spellTypeMask(spellTypeMask), _spellPhaseMask(spellPhaseMask),
_hitMask(hitMask), _spell(spell), _damageInfo(damageInfo), _healInfo(healInfo)
{
}

// we can disable this warning for this since it only
// causes undefined behavior when passed to the base class constructor
#ifdef _MSC_VER
#pragma warning(disable:4355)
#endif
Unit::Unit(bool isWorldObject): WorldObject(isWorldObject)
    , m_movedPlayer(NULL)
    , m_lastSanctuaryTime(0)
    , m_TempSpeed(0.0f)
    , IsAIEnabled(false)
    , NeedChangeAI(false)
    , m_ControlledByPlayer(false)
    , movespline(new Movement::MoveSpline())
    , i_AI(NULL)
    , i_disabledAI(NULL)
    , m_AutoRepeatFirstCast(false)
    , m_procDeep(0)
    , m_removedAurasCount(0)
    , i_motionMaster(this)
    , m_ThreatManager(this)
    , m_vehicle(NULL)
    , m_vehicleKit(NULL)
    , m_unitTypeMask(UNIT_MASK_NONE)
    , m_HostileRefManager(this)
    , _lastDamagedTime(0)
{
#ifdef _MSC_VER
#pragma warning(default:4355)
#endif
    m_objectType |= TYPEMASK_UNIT;
    m_objectTypeId = TYPEID_UNIT;

    m_updateFlag = UPDATEFLAG_LIVING;

    m_attackTimer[BASE_ATTACK] = 0;
    m_attackTimer[OFF_ATTACK] = 0;
    m_attackTimer[RANGED_ATTACK] = 0;
    m_modAttackSpeedPct[BASE_ATTACK] = 1.0f;
    m_modAttackSpeedPct[OFF_ATTACK] = 1.0f;
    m_modAttackSpeedPct[RANGED_ATTACK] = 1.0f;

    m_extraAttacks = 0;
    m_canDualWield = false;
    m_VisibilityUpdScheduled = false;

    m_rootTimes = 0;
    m_movementCounters = 0;

    m_state = 0;
    m_deathState = ALIVE;

    for (uint8 i = 0; i < CURRENT_MAX_SPELL; ++i)
        m_currentSpells[i] = NULL;

    m_addDmgOnce = 0;

    for (uint8 i = 0; i < MAX_SUMMON_SLOT; ++i)
        m_SummonSlot[i] = 0;

    for (uint8 i = 0; i < MAX_GAMEOBJECT_SLOT; ++i)
        m_ObjectSlot[i] = 0;

    m_auraUpdateIterator = m_ownedAuras.end();

    m_interruptMask = 0;
    m_transform = 0;
    m_canModifyStats = false;

    for (uint8 i = 0; i < MAX_SPELL_IMMUNITY; ++i)
        m_spellImmune[i].clear();

    for (uint8 i = 0; i < UNIT_MOD_END; ++i)
    {
        m_auraModifiersGroup[i][BASE_VALUE] = 0.0f;
        m_auraModifiersGroup[i][BASE_PCT] = 1.0f;
        m_auraModifiersGroup[i][TOTAL_VALUE] = 0.0f;
        m_auraModifiersGroup[i][TOTAL_PCT] = 1.0f;
    }
                                                            // implement 50% base damage from offhand
    m_auraModifiersGroup[UNIT_MOD_DAMAGE_OFFHAND][TOTAL_PCT] = 0.5f;

    for (uint8 i = 0; i < MAX_ATTACK; ++i)
    {
        m_weaponDamage[i][MINDAMAGE] = BASE_MINDAMAGE;
        m_weaponDamage[i][MAXDAMAGE] = BASE_MAXDAMAGE;
    }

    for (uint8 i = 0; i < MAX_STATS; ++i)
        m_createStats[i] = 0.0f;

    m_attacking = NULL;
    m_modMeleeHitChance = 0.0f;
    m_modRangedHitChance = 0.0f;
    m_modSpellHitChance = 0.0f;
    m_baseSpellCritChance = 5;

    m_CombatTimer = 0;

    simulacrumTarget = NULL;

    for (uint8 i = 0; i < MAX_SPELL_SCHOOL; ++i)
        m_threatModifier[i] = 1.0f;

    m_isSorted = true;

    for (uint8 i = 0; i < MAX_MOVE_TYPE; ++i)
        m_speed_rate[i] = 1.0f;

    m_charmInfo = NULL;
    m_reducedThreatPercent = 0;
    m_misdirectionTargetGUID = 0;

    // remove aurastates allowing special moves
    for (uint8 i = 0; i < MAX_REACTIVE; ++i)
        m_reactiveTimer[i] = 0;

    m_cleanupDone = false;
    m_duringRemoveFromWorld = false;

    m_serverSideVisibility.SetValue(SERVERSIDE_VISIBILITY_GHOST, GHOST_VISIBILITY_ALIVE);

    _focusSpell = NULL;
    _lastLiquid = NULL;
    _isWalkingBeforeCharm = false;
    _mount = NULL;

    m_ownerGuid = 0;
    m_charmerGuid = 0;
}

////////////////////////////////////////////////////////////
// Methods of class GlobalCooldownMgr
bool GlobalCooldownMgr::HasGlobalCooldown(SpellInfo const* spellInfo) const
{
    GlobalCooldownList::const_iterator itr = m_GlobalCooldowns.find(spellInfo->StartRecoveryCategory);
    return itr != m_GlobalCooldowns.end() && itr->second.duration && getMSTimeDiff(itr->second.cast_time, getMSTime() + 120) < itr->second.duration;
}

void GlobalCooldownMgr::AddGlobalCooldown(SpellInfo const* spellInfo, uint32 gcd)
{
    m_GlobalCooldowns[spellInfo->StartRecoveryCategory] = GlobalCooldown(gcd, getMSTime());
}

void GlobalCooldownMgr::CancelGlobalCooldown(SpellInfo const* spellInfo)
{
    m_GlobalCooldowns[spellInfo->StartRecoveryCategory].duration = 0;
}

////////////////////////////////////////////////////////////
// Methods of class Unit
Unit::~Unit()
{
    // set current spells as deletable
    for (uint8 i = 0; i < CURRENT_MAX_SPELL; ++i)
        if (m_currentSpells[i])
        {
            m_currentSpells[i]->SetReferencedFromCurrent(false);
            m_currentSpells[i]->SetExecutedCurrently(false);
            m_currentSpells[i] = NULL;
        }

    _DeleteRemovedAuras();

    delete m_charmInfo;
    delete movespline;

    ASSERT(!m_duringRemoveFromWorld);
    ASSERT(!m_attacking);
    ASSERT(m_attackers.empty());
    ASSERT(m_sharedVision.empty());
    ASSERT(m_Controlled.empty());
    ASSERT(m_appliedAuras.empty());
    ASSERT(m_ownedAuras.empty());
    ASSERT(m_removedAuras.empty());
    ASSERT(m_gameObj.empty());
    ASSERT(m_dynObj.empty());
}

void Unit::Update(uint32 p_time)
{
    // WARNING! Order of execution here is important, do not change.
    // Spells must be processed with event system BEFORE they go to _UpdateSpells.
    // Or else we may have some SPELL_STATE_FINISHED spells stalled in pointers, that is bad.
    m_Events.Update(p_time);

    if (!IsInWorld())
        return;

    _UpdateSpells(p_time);

    // If this is set during update SetCantProc(false) call is missing somewhere in the code
    // Having this would prevent spells from being proced, so let's crash
    ASSERT(!m_procDeep);

    if (CanHaveThreatList() && getThreatManager().isNeedUpdateToClient(p_time))
        SendThreatListUpdate();

    // update combat timer only for players and pets (only pets with PetAI)
    if (isInCombat() && (GetTypeId() == TYPEID_PLAYER || (ToCreature()->isPet() && IsControlledByPlayer()) || ToCreature()->isBattlegroundVehicle()))
    {
        // Check UNIT_STATE_MELEE_ATTACKING or UNIT_STATE_CHASE (without UNIT_STATE_FOLLOW in this case) so pets can reach far away
        // targets without stopping half way there and running off.
        // These flags are reset after target dies or another command is given.
        if (m_HostileRefManager.isEmpty())
        {
            // m_CombatTimer set at aura start and it will be freeze until aura removing
            if (m_CombatTimer <= p_time)
                ClearInCombat();
            else
                m_CombatTimer -= p_time;
        }
    }

    // not implemented before 3.0.2
    if (uint32 base_att = getAttackTimer(BASE_ATTACK))
        setAttackTimer(BASE_ATTACK, (p_time >= base_att ? 0 : base_att - p_time));
    if (uint32 ranged_att = getAttackTimer(RANGED_ATTACK))
        setAttackTimer(RANGED_ATTACK, (p_time >= ranged_att ? 0 : ranged_att - p_time));
    if (uint32 off_att = getAttackTimer(OFF_ATTACK))
        setAttackTimer(OFF_ATTACK, (p_time >= off_att ? 0 : off_att - p_time));

    // update abilities available only for fraction of time
    UpdateReactives(p_time);

    if (isAlive())
    {
        ModifyAuraState(AURA_STATE_HEALTHLESS_20_PERCENT, HealthBelowPct(20));
        ModifyAuraState(AURA_STATE_HEALTHLESS_35_PERCENT, HealthBelowPct(35));
        ModifyAuraState(AURA_STATE_HEALTH_ABOVE_75_PERCENT, HealthAbovePct(75));
    }

    UpdateSplineMovement(p_time);
    i_motionMaster.UpdateMotion(p_time);
}

bool Unit::haveOffhandWeapon() const
{
    if (GetTypeId() == TYPEID_PLAYER)
        return ToPlayer()->GetWeaponForAttack(OFF_ATTACK, true);
    else
        return m_canDualWield;
}

void Unit::MonsterMoveWithSpeed(float x, float y, float z, float speed)
{
    Movement::MoveSplineInit init(*this);
    init.MoveTo(x,y,z);
    init.SetVelocity(speed);
    init.Launch();
}

void Unit::UpdateSplineMovement(uint32 t_diff)
{
    if (movespline->Finalized())
        return;

    movespline->updateState(t_diff);
    bool arrived = movespline->Finalized();

    if (arrived)
        DisableSpline();

    m_movesplineTimer.Update(t_diff);
    if (m_movesplineTimer.Passed() || arrived)
        UpdateSplinePosition();
}

void Unit::SendMonsterMove(float NewPosX, float NewPosY, float NewPosZ, uint32 Time, Player* player)
{
    WorldPacket data(SMSG_MONSTER_MOVE, 12+4+1+4+4+4+12+GetPackGUID().size());
    data.append(GetPackGUID());

    data << uint8(0);                                       // new in 3.1
    data << GetPositionX() << GetPositionY() << GetPositionZ();
    data << getMSTime();

    data << uint8(0);
    data << uint32((GetUnitMovementFlags() & MOVEMENTFLAG_DISABLE_GRAVITY) ? SPLINEFLAG_FLYING : SPLINEFLAG_WALKMODE);
    data << Time;                                           // Time in between points
    data << uint32(1);                                      // 1 single waypoint
    data << NewPosX << NewPosY << NewPosZ;                  // the single waypoint Point B

    if (player)
        player->GetSession()->SendPacket(&data);
    else
        SendMessageToSet(&data, true);

    AddUnitState(UNIT_STATE_MOVE);
}

void Unit::UpdateSplinePosition()
{
    uint32 const positionUpdateDelay = 400;

    m_movesplineTimer.Reset(positionUpdateDelay);
    Movement::Location loc = movespline->ComputePosition();
    if (GetTransGUID())
    {
        Position& pos = m_movementInfo.t_pos;
        pos.m_positionX = loc.x;
        pos.m_positionY = loc.y;
        pos.m_positionZ = loc.z;
        pos.SetOrientation(loc.orientation);

        if (Unit* vehicle = GetVehicleBase())
        {
            loc.x += vehicle->GetPositionX();
            loc.y += vehicle->GetPositionY();
            loc.z += vehicle->GetPositionZMinusOffset();
            loc.orientation = vehicle->GetOrientation();
        }
        else if (TransportBase* transport = GetDirectTransport())
                transport->CalculatePassengerPosition(loc.x, loc.y, loc.z, loc.orientation);
    }

        if (HasUnitState(UNIT_STATE_CANNOT_TURN))
            loc.orientation = GetOrientation();

    UpdatePosition(loc.x, loc.y, loc.z, loc.orientation);
}

void Unit::DisableSpline()
{
    m_movementInfo.RemoveMovementFlag(MOVEMENTFLAG_FORWARD);
    m_movementInfo.RemoveServerMovementFlag(ServerMovementFlags(SERVERMOVEFLAG_SPLINE1 | SERVERMOVEFLAG_SPLINE2));
    movespline->_Interrupt();
}

void Unit::resetAttackTimer(WeaponAttackType type)
{
    m_attackTimer[type] = uint32(GetAttackTime(type) * m_modAttackSpeedPct[type]);
}

bool Unit::IsWithinCombatRange(const Unit* obj, float dist2compare) const
{
    if (!obj || !IsInMap(obj) || !InSamePhase(obj))
        return false;

    float dx = GetPositionX() - obj->GetPositionX();
    float dy = GetPositionY() - obj->GetPositionY();
    float dz = GetPositionZ() - obj->GetPositionZ();
    float distsq = dx * dx + dy * dy + dz * dz;

    float sizefactor = GetCombatReach() + obj->GetCombatReach();
    float maxdist = dist2compare + sizefactor;

    return distsq < maxdist * maxdist;
}

bool Unit::IsWithinMeleeRange(const Unit* obj, float dist) const
{
    if (!obj || !IsInMap(obj) || !InSamePhase(obj))
        return false;

    float dx = GetPositionX() - obj->GetPositionX();
    float dy = GetPositionY() - obj->GetPositionY();
    float dz = GetPositionZ() - obj->GetPositionZ();
    float distsq = dx*dx + dy*dy + dz*dz;

    float sizefactor = GetMeleeReach() + obj->GetMeleeReach();
    float maxdist = dist + sizefactor;

    return distsq < maxdist * maxdist;
}

void Unit::GetRandomContactPoint(const Unit* obj, float &x, float &y, float &z, float distance2dMin, float distance2dMax) const
{
    float combat_reach = GetCombatReach();
    if (combat_reach < 0.1f) // sometimes bugged for players
        combat_reach = DEFAULT_COMBAT_REACH;

    uint32 attacker_number = getAttackers().size();
    if (attacker_number > 0)
        --attacker_number;
    GetNearPoint(obj, x, y, z, obj->GetCombatReach(), distance2dMin+(distance2dMax-distance2dMin) * (float)rand_norm()
        , GetAngle(obj) + (attacker_number ? (static_cast<float>(M_PI/2) - static_cast<float>(M_PI) * (float)rand_norm()) * float(attacker_number) / combat_reach * 0.3f : 0));
}

void Unit::UpdateInterruptMask()
{
    m_interruptMask = 0;
    for (AuraApplicationList::const_iterator i = m_interruptableAuras.begin(); i != m_interruptableAuras.end(); ++i)
        m_interruptMask |= (*i)->GetBase()->GetSpellInfo()->AuraInterruptFlags;

    if (Spell* spell = m_currentSpells[CURRENT_CHANNELED_SPELL])
        if (spell->getState() == SPELL_STATE_CASTING)
            m_interruptMask |= spell->m_spellInfo->ChannelInterruptFlags;
}

bool Unit::HasAuraTypeWithFamilyFlags(AuraType auraType, uint32 familyName, uint32 familyFlags) const
{
    if (!HasAuraType(auraType))
        return false;
    AuraEffectList const& auras = GetAuraEffectsByType(auraType);
    for (AuraEffectList::const_iterator itr = auras.begin(); itr != auras.end(); ++itr)
        if (SpellInfo const* iterSpellProto = (*itr)->GetSpellInfo())
            if (iterSpellProto->SpellFamilyName == familyName && iterSpellProto->SpellFamilyFlags[0] & familyFlags)
                return true;
    return false;
}

bool Unit::HasCrowdControlAuraType(AuraType type, uint32 excludeAura) const
{
    AuraEffectList const& auras = GetAuraEffectsByType(type);
    for (AuraEffectList::const_iterator itr = auras.begin(); itr != auras.end(); ++itr)
        if ((!excludeAura || excludeAura != (*itr)->GetSpellInfo()->Id) && //Avoid self interrupt of channeled Crowd Control spells like Seduction
            ((*itr)->GetSpellInfo()->Attributes & SPELL_ATTR0_BREAKABLE_BY_DAMAGE || (*itr)->GetSpellInfo()->AuraInterruptFlags & AURA_INTERRUPT_FLAG_TAKE_DAMAGE))
            return true;
    return false;
}

bool Unit::HasCrowdControlAura(Unit* excludeCasterChannel) const
{
    uint32 excludeAura = 0;
    if (Spell* currentChanneledSpell = excludeCasterChannel ? excludeCasterChannel->GetCurrentSpell(CURRENT_CHANNELED_SPELL) : NULL)
        excludeAura = currentChanneledSpell->GetSpellInfo()->Id; //Avoid self interrupt of channeled Crowd Control spells like Seduction

    return (   HasCrowdControlAuraType(SPELL_AURA_MOD_CONFUSE, excludeAura)
            || HasCrowdControlAuraType(SPELL_AURA_MOD_FEAR, excludeAura)
            || HasCrowdControlAuraType(SPELL_AURA_MOD_STUN, excludeAura)
            || HasCrowdControlAuraType(SPELL_AURA_TRANSFORM, excludeAura));
}

bool Unit::HasBreakableByDamageAuraType(AuraType type, uint32 excludeAura) const
{
    AuraEffectList const& auras = GetAuraEffectsByType(type);
    for (AuraEffectList::const_iterator itr = auras.begin(); itr != auras.end(); ++itr)
        if ((!excludeAura || excludeAura != (*itr)->GetSpellInfo()->Id) && //Avoid self interrupt of channeled Crowd Control spells like Seduction
            ((*itr)->GetSpellInfo()->Attributes & SPELL_ATTR0_BREAKABLE_BY_DAMAGE || (*itr)->GetSpellInfo()->AuraInterruptFlags & AURA_INTERRUPT_FLAG_TAKE_DAMAGE))
            return true;
    return false;
}

bool Unit::HasBreakableByDamageCrowdControlAura(Unit* excludeCasterChannel) const
{
    uint32 excludeAura = 0;
    if (Spell* currentChanneledSpell = excludeCasterChannel ? excludeCasterChannel->GetCurrentSpell(CURRENT_CHANNELED_SPELL) : NULL)
        excludeAura = currentChanneledSpell->GetSpellInfo()->Id; //Avoid self interrupt of channeled Crowd Control spells like Seduction

    return (   HasBreakableByDamageAuraType(SPELL_AURA_MOD_CONFUSE, excludeAura)
            || HasBreakableByDamageAuraType(SPELL_AURA_MOD_FEAR, excludeAura)
            || HasBreakableByDamageAuraType(SPELL_AURA_MOD_STUN, excludeAura)
            || HasBreakableByDamageAuraType(SPELL_AURA_MOD_ROOT, excludeAura)
            || HasBreakableByDamageAuraType(SPELL_AURA_TRANSFORM, excludeAura));
}

void Unit::DealDamageMods(Unit* victim, uint32 &damage, uint32* absorb)
{
    if (!victim || !victim->isAlive() || victim->HasUnitState(UNIT_STATE_IN_FLIGHT) || (victim->GetTypeId() == TYPEID_UNIT && victim->ToCreature()->IsInEvadeMode()))
    {
        if (absorb)
            *absorb += damage;
        damage = 0;
    }
}

uint32 Unit::DealDamage(Unit* victim, uint32 damage, CleanDamage const* cleanDamage, DamageEffectType damagetype, SpellSchoolMask damageSchoolMask, SpellInfo const* spellProto, bool durabilityLoss)
{
    if (victim->IsAIEnabled)
        victim->GetAI()->DamageTaken(this, damage);

    if (IsAIEnabled)
        GetAI()->DamageDealt(victim, damage, damagetype);

    if (victim->GetTypeId() == TYPEID_PLAYER && this != victim)
    {
        // Signal to pets that their owner was attacked
        Pet* pet = victim->ToPlayer()->GetPet();

        if (pet && pet->isAlive())
            pet->AI()->OwnerAttackedBy(this);

        if (victim->ToPlayer()->GetCommandStatus(CHEAT_GOD))
            return 0;
    }

    // Signal the pet it was attacked so the AI can respond if needed
    if (victim->GetTypeId() == TYPEID_UNIT && this != victim && victim->isPet() && victim->isAlive())
        victim->ToPet()->AI()->AttackedBy(this);

    if (damagetype == DIRECT_DAMAGE || damagetype == SPELL_DIRECT_DAMAGE)
    {
        if (victim->GetTypeId() == TYPEID_PLAYER)
            victim->ToPlayer()->SetDamageTakenForSecond(damage);
    }

    // Signal the pet it was attacked so the AI can respond if needed
    if (victim->GetTypeId() == TYPEID_UNIT && this != victim && victim->isPet() && victim->isAlive())
        victim->ToPet()->AI()->AttackedBy(this);

    if (damagetype != NODAMAGE)
    {
        // interrupting auras with AURA_INTERRUPT_FLAG_DAMAGE before checking !damage (absorbed damage breaks that type of auras)
        if (spellProto)
        {
            if (!(spellProto->AttributesEx4 & SPELL_ATTR4_DAMAGE_DOESNT_BREAK_AURAS))
            {
                // Repentance shared damage removal
                if (cleanDamage && cleanDamage->mitigated_damage)
                    victim->RemoveAurasDueToSpell(20066);

                victim->RemoveAurasWithInterruptFlags(AURA_INTERRUPT_FLAG_TAKE_DAMAGE, spellProto->Id);
            }
        }
        else
            victim->RemoveAurasWithInterruptFlags(AURA_INTERRUPT_FLAG_TAKE_DAMAGE, 0);

        // We're going to call functions which can modify content of the list during iteration over it's elements
        // Let's copy the list so we can prevent iterator invalidation
        AuraEffectList vCopyDamageCopy(victim->GetAuraEffectsByType(SPELL_AURA_SHARE_DAMAGE_PCT));
        // copy damage to casters of this aura
        for (AuraEffectList::iterator i = vCopyDamageCopy.begin(); i != vCopyDamageCopy.end(); ++i)
        {
            // Check if aura was removed during iteration - we don't need to work on such auras
            if (!((*i)->GetBase()->IsAppliedOnTarget(victim->GetGUID())))
                continue;
            // check damage school mask
            if (((*i)->GetMiscValue() & damageSchoolMask) == 0)
                continue;

            Unit* shareDamageTarget = (*i)->GetCaster();
            if (!shareDamageTarget)
                continue;
            SpellInfo const* spell = (*i)->GetSpellInfo();

            uint32 share = CalculatePct(damage, (*i)->GetAmount());

            // TODO: check packets if damage is done by victim, or by attacker of victim
            DealDamageMods(shareDamageTarget, share, NULL);
            DealDamage(shareDamageTarget, share, NULL, NODAMAGE, spell->GetSchoolMask(), spell, false);
        }
    }

    // Rage from Damage made (only from direct weapon damage)
    if (cleanDamage && damagetype == DIRECT_DAMAGE && this != victim && getPowerType() == POWER_RAGE
        && (!spellProto || !spellProto->HasAura(SPELL_AURA_SPLIT_DAMAGE_PCT)))
    {
        uint32 weaponSpeedHitFactor;
        uint32 rage_damage = damage + cleanDamage->absorbed_damage;

        switch(cleanDamage->attackType)
        {
            case BASE_ATTACK:
            {
                weaponSpeedHitFactor = uint32(GetAttackTime(cleanDamage->attackType) / 1000.0f * 6.5f);

                RewardRage(weaponSpeedHitFactor, true);

                break;
            }
            case OFF_ATTACK:
            {
                weaponSpeedHitFactor = uint32(GetAttackTime(cleanDamage->attackType) / 1000.0f * 3.25f);

                RewardRage(weaponSpeedHitFactor, true);

                break;
            }
            case RANGED_ATTACK:
                break;
            default:
                break;
        }
    }
    if (damagetype != NODAMAGE && (damage || (cleanDamage && cleanDamage->absorbed_damage) ))
    {
        if (victim != this && victim->GetTypeId() == TYPEID_PLAYER) // does not support creature push_back
        {
            if (damagetype != DOT || (spellProto && spellProto->IsChanneled()))
            {
                if (Spell* spell = victim->m_currentSpells[CURRENT_GENERIC_SPELL])
                    if (spell->getState() == SPELL_STATE_PREPARING)
                    {
                        uint32 interruptFlags = spell->m_spellInfo->InterruptFlags;
                        if (interruptFlags & SPELL_INTERRUPT_FLAG_ABORT_ON_DMG)
                            victim->InterruptNonMeleeSpells(false);
                    }
            }
        }
    }


    if (!damage)
    {
        // Rage from absorbed damage
        if (cleanDamage && cleanDamage->absorbed_damage && victim->getPowerType() == POWER_RAGE)
        {
            victim->RewardRage(cleanDamage->absorbed_damage, false);
        }

        return 0;
    }

    uint32 health = victim->GetHealth();

    // duel ends when player has 1 or less hp
    bool duel_hasEnded = false;
    bool duel_wasMounted = false;
    if (victim->GetTypeId() == TYPEID_PLAYER && victim->ToPlayer()->duel && damage >= (health-1))
    {
        // prevent kill only if killed in duel and killed by opponent or opponent controlled creature
        if (victim->ToPlayer()->duel->opponent == this || victim->ToPlayer()->duel->opponent->GetGUID() == GetOwnerGUID() || victim == this)
            damage = health - 1;

        duel_hasEnded = true;
    }
    else if (victim->IsVehicle() && damage >= (health-1) && victim->GetCharmer() && victim->GetCharmer()->GetTypeId() == TYPEID_PLAYER)
    {
        Player* victimRider = victim->GetCharmer()->ToPlayer();

        if (victimRider && victimRider->duel && victimRider->duel->isMounted)
        {
            // prevent kill only if killed in duel and killed by opponent or opponent controlled creature
            if (victimRider->duel->opponent == this || victimRider->duel->opponent->GetGUID() == GetCharmerGUID())
                damage = health - 1;

            duel_wasMounted = true;
            duel_hasEnded = true;
        }
    }

    if (this != victim)
    {
        // damage of hunter pets must be counted in bg stat
        if (isHunterPet())
        {
            if (Player * owner = GetCharmerOrOwnerPlayerOrPlayerItself())
            {
                // in bg, count dmg if victim is also a player
                if (victim->GetTypeId() == TYPEID_PLAYER)
                    if (Battleground* bg = owner->GetBattleground())
                        bg->UpdatePlayerScore(owner, SCORE_DAMAGE_DONE, damage);
            }
        }
        else if (GetTypeId() == TYPEID_PLAYER)
        {
            Player * killer = ToPlayer();

            // in bg, count dmg if victim is also a player
            if (victim->GetTypeId() == TYPEID_PLAYER)
                if (Battleground* bg = killer->GetBattleground())
                    bg->UpdatePlayerScore(killer, SCORE_DAMAGE_DONE, damage);

            killer->UpdateAchievementCriteria(ACHIEVEMENT_CRITERIA_TYPE_DAMAGE_DONE, damage, 0, 0, victim);
            killer->UpdateAchievementCriteria(ACHIEVEMENT_CRITERIA_TYPE_HIGHEST_HIT_DEALT, damage);
        }
    }

    if (victim->GetTypeId() == TYPEID_PLAYER)
        victim->ToPlayer()->UpdateAchievementCriteria(ACHIEVEMENT_CRITERIA_TYPE_HIGHEST_HIT_RECEIVED, damage);
    else if (!victim->IsControlledByPlayer() || victim->IsVehicle())
    {
        if (!victim->ToCreature()->hasLootRecipient())
            victim->ToCreature()->SetLootRecipient(this);

        if (IsControlledByPlayer())
            victim->ToCreature()->LowerPlayerDamageReq(health < damage ?  health : damage);
    }

    if (health <= damage)
    {
        if (victim->GetTypeId() == TYPEID_PLAYER && victim != this)
            victim->ToPlayer()->UpdateAchievementCriteria(ACHIEVEMENT_CRITERIA_TYPE_TOTAL_DAMAGE_RECEIVED, health);

        Kill(victim, durabilityLoss);
    }
    else
    {
        if (victim->GetTypeId() == TYPEID_PLAYER)
            victim->ToPlayer()->UpdateAchievementCriteria(ACHIEVEMENT_CRITERIA_TYPE_TOTAL_DAMAGE_RECEIVED, damage);

        victim->ModifyHealth(- (int32)damage);

        if (damagetype == DIRECT_DAMAGE || damagetype == SPELL_DIRECT_DAMAGE)
            victim->RemoveAurasWithInterruptFlags(AURA_INTERRUPT_FLAG_DIRECT_DAMAGE, spellProto ? spellProto->Id : 0);

        if (victim->GetTypeId() != TYPEID_PLAYER)
            victim->AddThreat(this, float(damage), damageSchoolMask, spellProto);
        else                                                // victim is a player
        {
            // random durability for items (HIT TAKEN)
            if (roll_chance_f(sWorld->getRate(RATE_DURABILITY_LOSS_DAMAGE)))
            {
                EquipmentSlots slot = EquipmentSlots(urand(0, EQUIPMENT_SLOT_END-1));
                victim->ToPlayer()->DurabilityPointLossForEquipSlot(slot);
            }
        }

        // Rage from damage received
        if (this != victim && victim->getPowerType() == POWER_RAGE)
        {
            uint32 rage_damage = damage + (cleanDamage ? cleanDamage->absorbed_damage : 0);
            victim->RewardRage(rage_damage, false);
        }

        if (GetTypeId() == TYPEID_PLAYER)
        {
            // random durability for items (HIT DONE)
            if (roll_chance_f(sWorld->getRate(RATE_DURABILITY_LOSS_DAMAGE)))
            {
                EquipmentSlots slot = EquipmentSlots(urand(0, EQUIPMENT_SLOT_END-1));
                ToPlayer()->DurabilityPointLossForEquipSlot(slot);
            }
        }

        if (damagetype != NODAMAGE && damage)
        {
            if (victim != this && victim->GetTypeId() == TYPEID_PLAYER) // does not support creature push_back
            {
                if (damagetype != DOT)
                    if (Spell* spell = victim->m_currentSpells[CURRENT_GENERIC_SPELL])
                        if (spell->getState() == SPELL_STATE_PREPARING)
                        {
                            uint32 interruptFlags = spell->m_spellInfo->InterruptFlags;
                            if (interruptFlags & SPELL_INTERRUPT_FLAG_ABORT_ON_DMG)
                                victim->InterruptNonMeleeSpells(false);
                            else if (interruptFlags & SPELL_INTERRUPT_FLAG_PUSH_BACK)
                                spell->Delayed();
                        }

                if (Spell* spell = victim->m_currentSpells[CURRENT_CHANNELED_SPELL])
                    if (spell->getState() == SPELL_STATE_CASTING)
                    {
                        uint32 channelInterruptFlags = spell->m_spellInfo->ChannelInterruptFlags;
                        if (((channelInterruptFlags & CHANNEL_FLAG_DELAY) != 0) && (damagetype != DOT))
                            spell->DelayedChannel();
                    }
            }
        }

        // last damage from duel opponent
        if (duel_hasEnded)
        {
            Player* he = duel_wasMounted ? victim->GetCharmer()->ToPlayer() : victim->ToPlayer();

            ASSERT(he && he->duel);

            if (duel_wasMounted) // In this case victim==mount
                victim->SetHealth(1);
            else
                he->SetHealth(1);

            he->duel->opponent->CombatStopWithPets(true);
            he->CombatStopWithPets(true);

            he->CastSpell(he, 7267, true);                  // beg
            he->DuelComplete(DUEL_WON);
        }
    }

    return damage;
}

void Unit::CastStop(uint32 except_spellid)
{
    for (uint32 i = CURRENT_FIRST_NON_MELEE_SPELL; i < CURRENT_MAX_SPELL; i++)
        if (m_currentSpells[i] && m_currentSpells[i]->m_spellInfo->Id != except_spellid)
            InterruptSpell(CurrentSpellTypes(i), false);
}

void Unit::CastSpell(SpellCastTargets const& targets, SpellInfo const* spellInfo, CustomSpellValues const* value, TriggerCastFlags triggerFlags, Item* castItem, AuraEffect const* triggeredByAura, uint64 originalCaster, float periodicDamageModifier /* = 0.0f*/)
{
    if (!spellInfo)
    {
        sLog->outError(LOG_FILTER_UNITS, "CastSpell: unknown spell by caster: %s %u)", (GetTypeId() == TYPEID_PLAYER ? "player (GUID:" : "creature (Entry:"), (GetTypeId() == TYPEID_PLAYER ? GetGUIDLow() : GetEntry()));
        return;
    }

    // TODO: this is a workaround - not needed anymore, but required for some scripts :(
    if (!originalCaster && triggeredByAura)
        originalCaster = triggeredByAura->GetCasterGUID();

    Spell* spell = new Spell(this, spellInfo, triggerFlags, originalCaster);

    if (value)
        for (CustomSpellValues::const_iterator itr = value->begin(); itr != value->end(); ++itr)
            spell->SetSpellValue(itr->first, itr->second);

    spell->m_CastItem = castItem;
    spell->SetPeriodicDamageModifier(periodicDamageModifier);
    spell->prepare(&targets, triggeredByAura);
}

void Unit::CastSpell(Unit* victim, uint32 spellId, bool triggered, Item* castItem, AuraEffect const* triggeredByAura, uint64 originalCaster, float periodicDamageModifier)
{
    CastSpell(victim, spellId, triggered ? TRIGGERED_FULL_MASK : TRIGGERED_NONE, castItem, triggeredByAura, originalCaster, periodicDamageModifier);
}

void Unit::CastSpell(Unit* victim, uint32 spellId, TriggerCastFlags triggerFlags /*= TRIGGER_NONE*/, Item* castItem /*= NULL*/, AuraEffect const* triggeredByAura /*= NULL*/, uint64 originalCaster /*= 0*/, float periodicDamageModifier /* = 0.0f */)
{
    SpellInfo const* spellInfo = sSpellMgr->GetSpellInfo(spellId);
    if (!spellInfo)
    {
        //sLog->outError(LOG_FILTER_UNITS, "CastSpell: unknown spell id %u by caster: %s %u)", spellId, (GetTypeId() == TYPEID_PLAYER ? "player (GUID:" : "creature (Entry:"), (GetTypeId() == TYPEID_PLAYER ? GetGUIDLow() : GetEntry()));
        return;
    }

    CastSpell(victim, spellInfo, triggerFlags, castItem, triggeredByAura, originalCaster, periodicDamageModifier);
}

void Unit::CastSpell(Unit* victim, SpellInfo const* spellInfo, bool triggered, Item* castItem/*= NULL*/, AuraEffect const* triggeredByAura /*= NULL*/, uint64 originalCaster /*= 0*/)
{
    CastSpell(victim, spellInfo, triggered ? TRIGGERED_FULL_MASK : TRIGGERED_NONE, castItem, triggeredByAura, originalCaster);
}

void Unit::CastSpell(Unit* victim, SpellInfo const* spellInfo, TriggerCastFlags triggerFlags, Item* castItem, AuraEffect const* triggeredByAura, uint64 originalCaster, float periodicDamageModifier /* = 0.0f */)
{
    SpellCastTargets targets;
    targets.SetUnitTarget(victim);
    CastSpell(targets, spellInfo, NULL, triggerFlags, castItem, triggeredByAura, originalCaster, periodicDamageModifier);
}

void Unit::CastCustomSpell(Unit* target, uint32 spellId, int32 const* bp0, int32 const* bp1, int32 const* bp2, bool triggered, Item* castItem, AuraEffect const* triggeredByAura, uint64 originalCaster)
{
    CustomSpellValues values;
    if (bp0)
        values.AddSpellMod(SPELLVALUE_BASE_POINT0, *bp0);
    if (bp1)
        values.AddSpellMod(SPELLVALUE_BASE_POINT1, *bp1);
    if (bp2)
        values.AddSpellMod(SPELLVALUE_BASE_POINT2, *bp2);
    CastCustomSpell(spellId, values, target, triggered ? TRIGGERED_FULL_MASK : TRIGGERED_NONE, castItem, triggeredByAura, originalCaster);
}

void Unit::CastCustomSpell(uint32 spellId, SpellValueMod mod, int32 value, Unit* target, bool triggered, Item* castItem, AuraEffect const* triggeredByAura, uint64 originalCaster)
{
    CustomSpellValues values;
    values.AddSpellMod(mod, value);
    CastCustomSpell(spellId, values, target, triggered ? TRIGGERED_FULL_MASK : TRIGGERED_NONE, castItem, triggeredByAura, originalCaster);
}

void Unit::CastCustomSpell(uint32 spellId, SpellValueMod mod, int32 value, Unit* target, TriggerCastFlags triggerFlags, Item* castItem, AuraEffect const* triggeredByAura, uint64 originalCaster)
{
    CustomSpellValues values;
    values.AddSpellMod(mod, value);
    CastCustomSpell(spellId, values, target, triggerFlags, castItem, triggeredByAura, originalCaster);
}

void Unit::CastCustomSpell(uint32 spellId, CustomSpellValues const& value, Unit* victim, TriggerCastFlags triggerFlags, Item* castItem, AuraEffect const* triggeredByAura, uint64 originalCaster)
{
    SpellInfo const* spellInfo = sSpellMgr->GetSpellInfo(spellId);
    if (!spellInfo)
    {
        //sLog->outError(LOG_FILTER_UNITS, "CastSpell: unknown spell id %u by caster: %s %u)", spellId, (GetTypeId() == TYPEID_PLAYER ? "player (GUID:" : "creature (Entry:"), (GetTypeId() == TYPEID_PLAYER ? GetGUIDLow() : GetEntry()));
        return;
    }
    SpellCastTargets targets;
    targets.SetUnitTarget(victim);

    CastSpell(targets, spellInfo, &value, triggerFlags, castItem, triggeredByAura, originalCaster);
}

void Unit::CastSpell(float x, float y, float z, uint32 spellId, bool triggered, Item* castItem, AuraEffect const* triggeredByAura, uint64 originalCaster)
{
    SpellInfo const* spellInfo = sSpellMgr->GetSpellInfo(spellId);
    if (!spellInfo)
    {
        //sLog->outError(LOG_FILTER_UNITS, "CastSpell: unknown spell id %u by caster: %s %u)", spellId, (GetTypeId() == TYPEID_PLAYER ? "player (GUID:" : "creature (Entry:"), (GetTypeId() == TYPEID_PLAYER ? GetGUIDLow() : GetEntry()));
        return;
    }
    SpellCastTargets targets;
    targets.SetDst(x, y, z, GetOrientation());

    CastSpell(targets, spellInfo, NULL, triggered ? TRIGGERED_FULL_MASK : TRIGGERED_NONE, castItem, triggeredByAura, originalCaster);
}

void Unit::CastSpell(GameObject* go, uint32 spellId, bool triggered, Item* castItem, AuraEffect* triggeredByAura, uint64 originalCaster)
{
    SpellInfo const* spellInfo = sSpellMgr->GetSpellInfo(spellId);
    if (!spellInfo)
    {
        //sLog->outError(LOG_FILTER_UNITS, "CastSpell: unknown spell id %u by caster: %s %u)", spellId, (GetTypeId() == TYPEID_PLAYER ? "player (GUID:" : "creature (Entry:"), (GetTypeId() == TYPEID_PLAYER ? GetGUIDLow() : GetEntry()));
        return;
    }
    SpellCastTargets targets;
    targets.SetGOTarget(go);

    CastSpell(targets, spellInfo, NULL, triggered ? TRIGGERED_FULL_MASK : TRIGGERED_NONE, castItem, triggeredByAura, originalCaster);
}

// Obsolete func need remove, here only for comotability vs another patches
uint32 Unit::SpellNonMeleeDamageLog(Unit* victim, uint32 spellID, uint32 damage)
{
    SpellInfo const* spellInfo = sSpellMgr->GetSpellInfo(spellID);
    SpellNonMeleeDamage damageInfo(this, victim, spellInfo->Id, spellInfo->SchoolMask);
    damage = SpellDamageBonusDone(victim, spellInfo, damage, SPELL_DIRECT_DAMAGE);
    damage = victim->SpellDamageBonusTaken(this, spellInfo, damage, SPELL_DIRECT_DAMAGE);

    CalculateSpellDamageTaken(&damageInfo, damage, spellInfo);
    DealDamageMods(damageInfo.target, damageInfo.damage, &damageInfo.absorb);
    SendSpellNonMeleeDamageLog(&damageInfo);
    DealSpellDamage(&damageInfo, true);
    return damageInfo.damage;
}

void Unit::CalculateSpellDamageTaken(SpellNonMeleeDamage* damageInfo, int32 damage, SpellInfo const* spellInfo, WeaponAttackType attackType, bool crit)
{
    if (damage < 0)
        return;

    Unit* victim = damageInfo->target;
    if (!victim || !victim->isAlive())
        return;

    SpellSchoolMask damageSchoolMask = SpellSchoolMask(damageInfo->schoolMask);

    if (IsDamageReducedByArmor(damageSchoolMask, spellInfo))
        damage = CalcArmorReducedDamage(victim, damage, spellInfo, attackType);

    bool blocked = false;
    // Per-school calc
    switch (spellInfo->DmgClass)
    {
        // Melee and Ranged Spells
        case SPELL_DAMAGE_CLASS_RANGED:
        case SPELL_DAMAGE_CLASS_MELEE:
        {
            // Physical Damage
            if (damageSchoolMask & SPELL_SCHOOL_MASK_NORMAL)
            {
                // Get blocked status
                blocked = isSpellBlocked(victim, spellInfo, attackType);
            }

            if (crit)
            {
                damageInfo->HitInfo |= SPELL_HIT_TYPE_CRIT;

                // Calculate crit bonus
                uint32 crit_bonus = damage;
                // Apply crit_damage bonus for melee spells
                if (Player* modOwner = GetSpellModOwner())
                    modOwner->ApplySpellMod(spellInfo->Id, SPELLMOD_CRIT_DAMAGE_BONUS, crit_bonus);
                damage += crit_bonus;

                // Apply SPELL_AURA_MOD_ATTACKER_RANGED_CRIT_DAMAGE or SPELL_AURA_MOD_ATTACKER_MELEE_CRIT_DAMAGE
                float critPctDamageMod = 0.0f;
                if (attackType == RANGED_ATTACK)
                    critPctDamageMod += victim->GetTotalAuraModifier(SPELL_AURA_MOD_ATTACKER_RANGED_CRIT_DAMAGE);
                else
                    critPctDamageMod += victim->GetTotalAuraModifier(SPELL_AURA_MOD_ATTACKER_MELEE_CRIT_DAMAGE);

                // Increase crit damage from SPELL_AURA_MOD_CRIT_DAMAGE_BONUS
                critPctDamageMod += (GetTotalAuraMultiplierByMiscMask(SPELL_AURA_MOD_CRIT_DAMAGE_BONUS, spellInfo->GetSchoolMask()) - 1.0f) * 100;

                if (critPctDamageMod != 0)
                    AddPct(damage, critPctDamageMod);
            }

            // Spell weapon based damage CAN BE crit & blocked at same time
            if (blocked)
            {
                // double blocked amount if block is critical
                uint32 value = victim->GetBlockPercent();
                if (victim->isBlockCritical())
                    value *= 2; // double blocked percent
                damageInfo->blocked = CalculatePct(damage, value);
                damage -= damageInfo->blocked;
            }

            if (!spellInfo->HasCustomAttribute(SPELL_ATTR0_CU_TRIGGERED_IGNORE_RESILENCE))
                ApplyResilience(victim, &damage);
            break;
        }
        // Magical Attacks
        case SPELL_DAMAGE_CLASS_NONE:
        case SPELL_DAMAGE_CLASS_MAGIC:
        {
            // If crit add critical bonus
            if (crit)
            {
                damageInfo->HitInfo |= SPELL_HIT_TYPE_CRIT;
                damage = SpellCriticalDamageBonus(spellInfo, damage, victim);
            }

            if (!spellInfo->HasCustomAttribute(SPELL_ATTR0_CU_TRIGGERED_IGNORE_RESILENCE))
                ApplyResilience(victim, &damage);
            break;
        }
        default:
            break;
    }

    // Calculate absorb resist
    if (damage > 0)
    {
        CalcAbsorbResist(victim, damageSchoolMask, SPELL_DIRECT_DAMAGE, damage, &damageInfo->absorb, &damageInfo->resist, spellInfo);
        damage -= damageInfo->absorb + damageInfo->resist;
    }
    else
        damage = 0;

    damageInfo->damage = damage;
}

void Unit::DealSpellDamage(SpellNonMeleeDamage* damageInfo, bool durabilityLoss)
{
    if (damageInfo == 0)
        return;

    Unit* victim = damageInfo->target;

    if (!victim)
        return;

    if (!victim->isAlive() || victim->HasUnitState(UNIT_STATE_IN_FLIGHT) || (victim->GetTypeId() == TYPEID_UNIT && victim->ToCreature()->IsInEvadeMode()))
        return;

    SpellInfo const* spellProto = sSpellMgr->GetSpellInfo(damageInfo->SpellID);
    if (spellProto == NULL)
        return;

    // Call default DealDamage
    CleanDamage cleanDamage(damageInfo->cleanDamage, damageInfo->absorb, BASE_ATTACK, MELEE_HIT_NORMAL);
    DealDamage(victim, damageInfo->damage, &cleanDamage, SPELL_DIRECT_DAMAGE, SpellSchoolMask(damageInfo->schoolMask), spellProto, durabilityLoss);
}

// TODO for melee need create structure as in
void Unit::CalculateMeleeDamage(Unit* victim, uint32 damage, CalcDamageInfo* damageInfo, WeaponAttackType attackType)
{
    damageInfo->attacker         = this;
    damageInfo->target           = victim;
    damageInfo->damageSchoolMask = GetMeleeDamageSchoolMask();
    damageInfo->attackType       = attackType;
    damageInfo->damage           = 0;
    damageInfo->cleanDamage      = 0;
    damageInfo->absorb           = 0;
    damageInfo->resist           = 0;
    damageInfo->blocked_amount   = 0;

    damageInfo->TargetState      = 0;
    damageInfo->HitInfo          = 0;
    damageInfo->procAttacker     = PROC_FLAG_NONE;
    damageInfo->procVictim       = PROC_FLAG_NONE;
    damageInfo->procEx           = PROC_EX_NONE;
    damageInfo->hitOutCome       = MELEE_HIT_EVADE;

    if (!victim)
        return;

    if (!isAlive() || !victim->isAlive())
        return;

    // Select HitInfo/procAttacker/procVictim flag based on attack type
    switch (attackType)
    {
        case BASE_ATTACK:
            damageInfo->procAttacker = PROC_FLAG_DONE_MELEE_AUTO_ATTACK | PROC_FLAG_DONE_MAINHAND_ATTACK;
            damageInfo->procVictim   = PROC_FLAG_TAKEN_MELEE_AUTO_ATTACK;
            break;
        case OFF_ATTACK:
            damageInfo->procAttacker = PROC_FLAG_DONE_MELEE_AUTO_ATTACK | PROC_FLAG_DONE_OFFHAND_ATTACK;
            damageInfo->procVictim   = PROC_FLAG_TAKEN_MELEE_AUTO_ATTACK;
            damageInfo->HitInfo      = HITINFO_OFFHAND;
            break;
        default:
            return;
    }

    // Physical Immune check
    if (damageInfo->target->IsImmunedToDamage(SpellSchoolMask(damageInfo->damageSchoolMask)))
    {
       damageInfo->HitInfo       |= HITINFO_NORMALSWING;
       damageInfo->TargetState    = VICTIMSTATE_IS_IMMUNE;

       damageInfo->procEx        |= PROC_EX_IMMUNE;
       damageInfo->damage         = 0;
       damageInfo->cleanDamage    = 0;
       return;
    }

    damage += CalculateDamage(damageInfo->attackType, false, true);
    // Add melee damage bonus
    damage = MeleeDamageBonusDone(damageInfo->target, damage, damageInfo->attackType);
    damage = damageInfo->target->MeleeDamageBonusTaken(this, damage, damageInfo->attackType);

    // Calculate armor reduction
    if (IsDamageReducedByArmor((SpellSchoolMask)(damageInfo->damageSchoolMask)))
    {
        damageInfo->damage = CalcArmorReducedDamage(damageInfo->target, damage, NULL, damageInfo->attackType);
        damageInfo->cleanDamage += damage - damageInfo->damage;
    }
    else
        damageInfo->damage = damage;

    damageInfo->hitOutCome = RollMeleeOutcomeAgainst(damageInfo->target, damageInfo->attackType);

    switch (damageInfo->hitOutCome)
    {
        case MELEE_HIT_EVADE:
            damageInfo->HitInfo        |= HITINFO_MISS | HITINFO_SWINGNOHITSOUND;
            damageInfo->TargetState     = VICTIMSTATE_EVADES;
            damageInfo->procEx         |= PROC_EX_EVADE;
            damageInfo->damage = 0;
            damageInfo->cleanDamage = 0;
            return;
        case MELEE_HIT_MISS:
            damageInfo->HitInfo        |= HITINFO_MISS;
            damageInfo->TargetState     = VICTIMSTATE_INTACT;
            damageInfo->procEx         |= PROC_EX_MISS;
            damageInfo->damage          = 0;
            damageInfo->cleanDamage     = 0;
            break;
        case MELEE_HIT_NORMAL:
            damageInfo->TargetState     = VICTIMSTATE_HIT;
            damageInfo->procEx         |= PROC_EX_NORMAL_HIT;
            break;
        case MELEE_HIT_CRIT:
        {
            damageInfo->HitInfo        |= HITINFO_CRITICALHIT;
            damageInfo->TargetState     = VICTIMSTATE_HIT;

            damageInfo->procEx         |= PROC_EX_CRITICAL_HIT;
            // Crit bonus calc
            damageInfo->damage += damageInfo->damage;
            float mod = 0.0f;
            // Apply SPELL_AURA_MOD_ATTACKER_RANGED_CRIT_DAMAGE or SPELL_AURA_MOD_ATTACKER_MELEE_CRIT_DAMAGE
            if (damageInfo->attackType == RANGED_ATTACK)
                mod += damageInfo->target->GetTotalAuraModifier(SPELL_AURA_MOD_ATTACKER_RANGED_CRIT_DAMAGE);
            else
                mod += damageInfo->target->GetTotalAuraModifier(SPELL_AURA_MOD_ATTACKER_MELEE_CRIT_DAMAGE);

            // Increase crit damage from SPELL_AURA_MOD_CRIT_DAMAGE_BONUS
            mod += (GetTotalAuraMultiplierByMiscMask(SPELL_AURA_MOD_CRIT_DAMAGE_BONUS, damageInfo->damageSchoolMask) - 1.0f) * 100;

            if (mod != 0)
                AddPct(damageInfo->damage, mod);
            break;
        }
        case MELEE_HIT_PARRY:
            damageInfo->TargetState  = VICTIMSTATE_PARRY;
            damageInfo->procEx      |= PROC_EX_PARRY;
            damageInfo->cleanDamage += damageInfo->damage;
            damageInfo->damage = 0;
            break;
        case MELEE_HIT_DODGE:
            damageInfo->TargetState  = VICTIMSTATE_DODGE;
            damageInfo->procEx      |= PROC_EX_DODGE;
            damageInfo->cleanDamage += damageInfo->damage;
            damageInfo->damage = 0;
            break;
        case MELEE_HIT_BLOCK:
            damageInfo->TargetState = VICTIMSTATE_HIT;
            damageInfo->HitInfo    |= HITINFO_BLOCK;
            damageInfo->procEx     |= PROC_EX_BLOCK | PROC_EX_NORMAL_HIT;
            // 30% damage blocked, double blocked amount if block is critical
            damageInfo->blocked_amount = CalculatePct(damageInfo->damage, damageInfo->target->isBlockCritical() ? damageInfo->target->GetBlockPercent() * 2 : damageInfo->target->GetBlockPercent());
            damageInfo->damage      -= damageInfo->blocked_amount;
            damageInfo->cleanDamage += damageInfo->blocked_amount;
            break;
        case MELEE_HIT_GLANCING:
        {
            damageInfo->HitInfo     |= HITINFO_GLANCING;
            damageInfo->TargetState  = VICTIMSTATE_HIT;
            damageInfo->procEx      |= PROC_EX_NORMAL_HIT;
            int32 leveldif = int32(victim->getLevel()) - int32(getLevel());
            if (leveldif > 3)
                leveldif = 3;
            float reducePercent = 1 - leveldif * 0.1f;
            damageInfo->cleanDamage += damageInfo->damage - uint32(reducePercent * damageInfo->damage);
            damageInfo->damage = uint32(reducePercent * damageInfo->damage);
            break;
        }
        case MELEE_HIT_CRUSHING:
            damageInfo->HitInfo     |= HITINFO_CRUSHING;
            damageInfo->TargetState  = VICTIMSTATE_HIT;
            damageInfo->procEx      |= PROC_EX_NORMAL_HIT;
            // 150% normal damage
            damageInfo->damage += (damageInfo->damage / 2);
            break;
        default:
            break;
    }

    // Always apply HITINFO_AFFECTS_VICTIM in case its not a miss
    if (!(damageInfo->HitInfo & HITINFO_MISS))
        damageInfo->HitInfo |= HITINFO_AFFECTS_VICTIM;

    int32 resilienceReduction = damageInfo->damage;
    ApplyResilience(victim, &resilienceReduction);
    resilienceReduction = damageInfo->damage - resilienceReduction;
    damageInfo->damage      -= resilienceReduction;
    damageInfo->cleanDamage += resilienceReduction;

    // Calculate absorb resist
    if (int32(damageInfo->damage) > 0)
    {
        damageInfo->procVictim |= PROC_FLAG_TAKEN_DAMAGE;
        // Calculate absorb & resists
        CalcAbsorbResist(damageInfo->target, SpellSchoolMask(damageInfo->damageSchoolMask), DIRECT_DAMAGE, damageInfo->damage, &damageInfo->absorb, &damageInfo->resist);

        if (damageInfo->absorb)
        {
            damageInfo->HitInfo |= (damageInfo->damage - damageInfo->absorb == 0 ? HITINFO_FULL_ABSORB : HITINFO_PARTIAL_ABSORB);
            damageInfo->procEx  |= PROC_EX_ABSORB;
        }

        if (damageInfo->resist)
            damageInfo->HitInfo |= (damageInfo->damage - damageInfo->resist == 0 ? HITINFO_FULL_RESIST : HITINFO_PARTIAL_RESIST);

        damageInfo->damage -= damageInfo->absorb + damageInfo->resist;
    }
    else // Impossible get negative result but....
        damageInfo->damage = 0;
}

void Unit::DealMeleeDamage(CalcDamageInfo* damageInfo, bool durabilityLoss)
{
    Unit* victim = damageInfo->target;

    if (!victim->isAlive() || victim->HasUnitState(UNIT_STATE_IN_FLIGHT) || (victim->GetTypeId() == TYPEID_UNIT && victim->ToCreature()->IsInEvadeMode()))
        return;

    // Hmmmm dont like this emotes client must by self do all animations
    if (damageInfo->HitInfo & HITINFO_CRITICALHIT)
        victim->HandleEmoteCommand(EMOTE_ONESHOT_WOUND_CRITICAL);
    if (damageInfo->blocked_amount && damageInfo->TargetState != VICTIMSTATE_BLOCKS)
        victim->HandleEmoteCommand(EMOTE_ONESHOT_PARRY_SHIELD);

    if (damageInfo->TargetState == VICTIMSTATE_PARRY)
    {
        // Get attack timers
        float offtime  = float(victim->getAttackTimer(OFF_ATTACK));
        float basetime = float(victim->getAttackTimer(BASE_ATTACK));
        // Reduce attack time
        if (victim->haveOffhandWeapon() && offtime < basetime)
        {
            float percent20 = victim->GetAttackTime(OFF_ATTACK) * 0.20f;
            float percent60 = 3.0f * percent20;
            if (offtime > percent20 && offtime <= percent60)
                victim->setAttackTimer(OFF_ATTACK, uint32(percent20));
            else if (offtime > percent60)
            {
                offtime -= 2.0f * percent20;
                victim->setAttackTimer(OFF_ATTACK, uint32(offtime));
            }
        }
        else
        {
            float percent20 = victim->GetAttackTime(BASE_ATTACK) * 0.20f;
            float percent60 = 3.0f * percent20;
            if (basetime > percent20 && basetime <= percent60)
                victim->setAttackTimer(BASE_ATTACK, uint32(percent20));
            else if (basetime > percent60)
            {
                basetime -= 2.0f * percent20;
                victim->setAttackTimer(BASE_ATTACK, uint32(basetime));
            }
        }
    }

    // Call default DealDamage
    CleanDamage cleanDamage(damageInfo->cleanDamage, damageInfo->absorb, damageInfo->attackType, damageInfo->hitOutCome);
    DealDamage(victim, damageInfo->damage, &cleanDamage, DIRECT_DAMAGE, SpellSchoolMask(damageInfo->damageSchoolMask), NULL, durabilityLoss);

    // If this is a creature and it attacks from behind it has a probability to daze it's victim
    if ((damageInfo->hitOutCome == MELEE_HIT_CRIT || damageInfo->hitOutCome == MELEE_HIT_CRUSHING || damageInfo->hitOutCome == MELEE_HIT_NORMAL || damageInfo->hitOutCome == MELEE_HIT_GLANCING) &&
        GetTypeId() != TYPEID_PLAYER && !ToCreature()->IsControlledByPlayer() && !victim->HasInArc(M_PI, this)
        && (victim->GetTypeId() == TYPEID_PLAYER || !victim->ToCreature()->isWorldBoss()))
    {
        // -probability is between 0% and 40%
        // 20% base chance
        float Probability = 20.0f;

        // there is a newbie protection, at level 10 just 7% base chance; assuming linear function
        if (victim->getLevel() < 30)
            Probability = 0.65f * victim->getLevel() + 0.5f;

        uint32 VictimDefense = victim->GetMaxSkillValueForLevel(this);
        uint32 AttackerMeleeSkill = GetMaxSkillValueForLevel();

        Probability *= AttackerMeleeSkill/(float)VictimDefense;

        if (Probability > 40.0f)
            Probability = 40.0f;

        if (roll_chance_f(Probability))
            CastSpell(victim, 1604, true);
    }

    if (GetTypeId() == TYPEID_PLAYER)
        ToPlayer()->CastItemCombatSpell(victim, damageInfo->attackType, damageInfo->procVictim, damageInfo->procEx);

    // Do effect if any damage done to target

    if (isTotem())
        return;

    if (damageInfo->damage)
    {
        // We're going to call functions which can modify content of the list during iteration over it's elements
        // Let's copy the list so we can prevent iterator invalidation
        AuraEffectList vDamageShieldsCopy(victim->GetAuraEffectsByType(SPELL_AURA_DAMAGE_SHIELD));
        for (AuraEffectList::const_iterator dmgShieldItr = vDamageShieldsCopy.begin(); dmgShieldItr != vDamageShieldsCopy.end(); ++dmgShieldItr)
        {
            SpellInfo const* i_spellProto = (*dmgShieldItr)->GetSpellInfo();
            // Damage shield can be resisted...
            if (SpellMissInfo missInfo = victim->SpellHitResult(this, i_spellProto, false))
            {
                victim->SendSpellMiss(this, i_spellProto->Id, missInfo);
                continue;
            }

            // ...or immuned
            if (IsImmunedToDamage(i_spellProto))
            {
                victim->SendSpellDamageImmune(this, i_spellProto->Id);
                continue;
            }

            uint32 damage = (*dmgShieldItr)->GetAmount();

            if (Unit* caster = (*dmgShieldItr)->GetCaster())
            {
                // Thorns, calculate damage
                if ((*dmgShieldItr)->GetSpellInfo()->Id == 467)
                {
                    if (caster->GetTypeId() == TYPEID_PLAYER)
                    {
                        if (caster->ToPlayer()->GetPrimaryTalentTree(caster->ToPlayer()->GetActiveSpec()) == TALENT_TREE_DRUID_FERAL_COMBAT)
                            damage += int32(0.168f * caster->GetTotalAttackPowerValue(BASE_ATTACK));
                        else
                            damage += int32(0.168f * caster->SpellBaseDamageBonusDone(SpellSchoolMask((*dmgShieldItr)->GetSpellInfo()->SchoolMask)));
                    }
                }

                damage = caster->SpellDamageBonusDone(this, i_spellProto, damage, SPELL_DIRECT_DAMAGE);
                damage = this->SpellDamageBonusTaken(caster, i_spellProto, damage, SPELL_DIRECT_DAMAGE);
            }

            // No Unit::CalcAbsorbResist here - opcode doesn't send that data - this damage is probably not affected by that
            victim->DealDamageMods(this, damage, NULL);

            // TODO: Move this to a packet handler
            WorldPacket data(SMSG_SPELLDAMAGESHIELD, 8 + 8 + 4 + 4 + 4 + 4 + 4);
            data << uint64(victim->GetGUID());
            data << uint64(GetGUID());
            data << uint32(i_spellProto->Id);
            data << uint32(damage);                  // Damage
            int32 overkill = int32(damage) - int32(GetHealth());
            data << uint32(overkill > 0 ? overkill : 0); // Overkill
            data << uint32(i_spellProto->SchoolMask);
            data << uint32(0); // FIX ME: Send resisted damage, both fully resisted and partly resisted
            victim->SendMessageToSet(&data, true);

            victim->DealDamage(this, damage, 0, SPELL_DIRECT_DAMAGE, i_spellProto->GetSchoolMask(), i_spellProto, true);
        }
    }
}

void Unit::HandleEmoteCommand(uint32 anim_id)
{
    WorldPacket data(SMSG_EMOTE, 4 + 8);
    data << uint32(anim_id);
    data << uint64(GetGUID());
    SendMessageToSet(&data, true);
}

bool Unit::IsDamageReducedByArmor(SpellSchoolMask schoolMask, SpellInfo const* spellInfo, uint8 effIndex)
{
    // only physical spells damage gets reduced by armor
    if ((schoolMask & SPELL_SCHOOL_MASK_NORMAL) == 0)
        return false;
    if (spellInfo)
    {
        // there are spells with no specific attribute but they have "ignores armor" in tooltip
        if (spellInfo->AttributesCu & SPELL_ATTR0_CU_IGNORE_ARMOR)
            return false;

        // bleeding effects are not reduced by armor
        if (effIndex != MAX_SPELL_EFFECTS)
        {
            if (spellInfo->Effects[effIndex].ApplyAuraName == SPELL_AURA_PERIODIC_DAMAGE ||
                spellInfo->Effects[effIndex].Effect == SPELL_EFFECT_SCHOOL_DAMAGE)
                if (spellInfo->GetEffectMechanicMask(effIndex) & (1<<MECHANIC_BLEED))
                    return false;
        }
    }
    return true;
}

uint32 Unit::CalcArmorReducedDamage(Unit* victim, const uint32 damage, SpellInfo const* spellInfo, WeaponAttackType /*attackType*/)
{
    uint32 newdamage = 0;
    float armor = float(victim->GetArmor());

    // bypass enemy armor by SPELL_AURA_BYPASS_ARMOR_FOR_CASTER
    int32 armorBypassPct = 0;
    AuraEffectList const & reductionAuras = victim->GetAuraEffectsByType(SPELL_AURA_BYPASS_ARMOR_FOR_CASTER);
    for (AuraEffectList::const_iterator i = reductionAuras.begin(); i != reductionAuras.end(); ++i)
        if ((*i)->GetCasterGUID() == GetGUID())
            armorBypassPct += (*i)->GetAmount();
    armor = CalculatePct(armor, 100 - std::min(armorBypassPct, 100));

    // Ignore enemy armor by SPELL_AURA_MOD_TARGET_RESISTANCE aura
    armor += GetTotalAuraModifierByMiscMask(SPELL_AURA_MOD_TARGET_RESISTANCE, SPELL_SCHOOL_MASK_NORMAL);

    if (spellInfo)
        if (Player* modOwner = GetSpellModOwner())
            modOwner->ApplySpellMod(spellInfo->Id, SPELLMOD_IGNORE_ARMOR, armor);

    AuraEffectList const& ResIgnoreAuras = GetAuraEffectsByType(SPELL_AURA_MOD_IGNORE_TARGET_RESIST);
    for (AuraEffectList::const_iterator j = ResIgnoreAuras.begin(); j != ResIgnoreAuras.end(); ++j)
    {
        if ((*j)->GetMiscValue() & SPELL_SCHOOL_MASK_NORMAL)
            armor = floor(AddPct(armor, -(*j)->GetAmount()));
    }

    // Apply Player CR_ARMOR_PENETRATION rating
    if (GetTypeId() == TYPEID_PLAYER)
    {
        float maxArmorPen = 0;
        if (victim->getLevel() < 60)
            maxArmorPen = float(400 + 85 * victim->getLevel());
        else
            maxArmorPen = 400 + 85 * victim->getLevel() + 4.5f * 85 * (victim->getLevel() - 59);

        // Cap armor penetration to this number
        maxArmorPen = std::min((armor + maxArmorPen) / 3, armor);
        // Figure out how much armor do we ignore
        float armorPen = CalculatePct(maxArmorPen, ToPlayer()->GetRatingBonusValue(CR_ARMOR_PENETRATION));
        // Got the value, apply it
        armor -= std::min(armorPen, maxArmorPen);
    }

    if (armor < 0.0f)
        armor = 0.0f;

    float tmpvalue = armor / (armor + 85.0f * getLevel() + 400.0f);
    if (getLevel() > 59)
        tmpvalue = armor / (armor + 467.5f * getLevel() - 22167.5f);
    if (getLevel() > 80)
        tmpvalue = armor / (armor + 2167.5f * getLevel() - 158167.5f);

    if (tmpvalue < 0.0f)
        tmpvalue = 0.0f;
    if (tmpvalue > 0.75f)
        tmpvalue = 0.75f;

    newdamage = uint32(damage - (damage * tmpvalue));

    return (newdamage > 1) ? newdamage : 1;
}

bool Unit::IsSpellResisted(Unit* victim, SpellSchoolMask schoolMask, SpellInfo const* spellInfo)
{
    if (!victim || !victim->isAlive())
        return false;

    Player* owner = GetCharmerOrOwnerPlayerOrPlayerItself();
    if (!owner)
        return false;

    if ((schoolMask & SPELL_SCHOOL_MASK_NORMAL) == 0 && (!spellInfo || (spellInfo->AttributesEx4 & SPELL_ATTR4_IGNORE_RESISTANCES) == 0))
    {
        float victimResistance = float(victim->GetResistance(schoolMask));
        victimResistance += float(owner->GetTotalAuraModifierByMiscMask(SPELL_AURA_MOD_TARGET_RESISTANCE, schoolMask));

        victimResistance -= float(owner->GetSpellPenetrationItemMod());

        // Resistance can't be lower then 0.
        if (victimResistance < 0.0f)
            victimResistance = 0.0f;

        uint32 level = victim->getLevel();
        float resistanceConstant = 0.0f;

        // http://elitistjerks.com/f15/t29453-combat_ratings_level_85_cataclysm/
        if (level >= 61)
            resistanceConstant = 150 + ((level - 60) * (level - 67.5));
        else if (level >= 21 && level <= 60)
            resistanceConstant = 50 + ((level - 20) * 2.5);
        else 
            resistanceConstant = 50.0f;

        float averageResist = victimResistance / (victimResistance + resistanceConstant);
        
        int32 tmp = int32(averageResist * 10000);
        int32 rand = irand(0, 10000);
        return (rand < tmp);
    }
    return false;
}

void Unit::CalcAbsorbResist(Unit* victim, SpellSchoolMask schoolMask, DamageEffectType damagetype, uint32 const damage, uint32 *absorb, uint32 *resist, SpellInfo const* spellInfo)
{
    if (!victim || !victim->isAlive() || !damage)
        return;

    DamageInfo dmgInfo = DamageInfo(this, victim, damage, spellInfo, schoolMask, damagetype);

    // Magic damage, check for resists
    // Ignore spells that cant be resisted
    if ((schoolMask & SPELL_SCHOOL_MASK_NORMAL) == 0 && (!spellInfo || (spellInfo->AttributesEx4 & SPELL_ATTR4_IGNORE_RESISTANCES) == 0))
    {
        float victimResistance = float(victim->GetResistance(schoolMask));

        Player* player_owner = GetCharmerOrOwnerPlayerOrPlayerItself();

        victimResistance += float(player_owner ? player_owner->GetTotalAuraModifierByMiscMask(SPELL_AURA_MOD_TARGET_RESISTANCE, schoolMask) : GetTotalAuraModifierByMiscMask(SPELL_AURA_MOD_TARGET_RESISTANCE, schoolMask));

        if (player_owner)
            victimResistance -= float(player_owner->GetSpellPenetrationItemMod());

        // Resistance can't be lower then 0.
        if (victimResistance < 0.0f)
            victimResistance = 0.0f;

        uint32 level = victim->getLevel();
        float resistanceConstant = 0.0f;

        // http://elitistjerks.com/f15/t29453-combat_ratings_level_85_cataclysm/
        if (level >= 61)
            resistanceConstant = 150 + ((level - 60) * (level - 67.5));
        else if (level >= 21 && level <= 60)
            resistanceConstant = 50 + ((level - 20) * 2.5);
        else 
            resistanceConstant = 50.0f;

        float averageResist = victimResistance / (victimResistance + resistanceConstant);
        float discreteResistProbability[11];
        for (uint32 i = 0; i < 11; ++i)
        {
            discreteResistProbability[i] = 0.5f - 2.5f * fabs(0.1f * i - averageResist);
            if (discreteResistProbability[i] < 0.0f)
                discreteResistProbability[i] = 0.0f;
        }

        if (averageResist <= 0.1f)
        {
            discreteResistProbability[0] = 1.0f - 7.5f * averageResist;
            discreteResistProbability[1] = 5.0f * averageResist;
            discreteResistProbability[2] = 2.5f * averageResist;
        }

        float r = float(rand_norm());
        uint32 i = 0;
        float probabilitySum = discreteResistProbability[0];

        while (r >= probabilitySum && i < 10)
            probabilitySum += discreteResistProbability[++i];

        float damageResisted = float(damage * i / 10);

        // There is no spell with SPELL_AURA_MOD_IGNORE_TARGET_RESIST aura in 4.3.4
        // Skip this block
        /*AuraEffectList const& ResIgnoreAuras = GetAuraEffectsByType(SPELL_AURA_MOD_IGNORE_TARGET_RESIST);
        for (AuraEffectList::const_iterator j = ResIgnoreAuras.begin(); j != ResIgnoreAuras.end(); ++j)
            if ((*j)->GetMiscValue() & schoolMask)
                AddPct(damageResisted, -(*j)->GetAmount());*/

        dmgInfo.ResistDamage(uint32(damageResisted));
    }

    // Ignore Absorption Auras
    float auraAbsorbMod = 0;
    AuraEffectList const& AbsIgnoreAurasA = GetAuraEffectsByType(SPELL_AURA_MOD_TARGET_ABSORB_SCHOOL);
    for (AuraEffectList::const_iterator itr = AbsIgnoreAurasA.begin(); itr != AbsIgnoreAurasA.end(); ++itr)
    {
        if (!((*itr)->GetMiscValue() & schoolMask))
            continue;

        if ((*itr)->GetAmount() > auraAbsorbMod)
            auraAbsorbMod = float((*itr)->GetAmount());
    }

    AuraEffectList const& AbsIgnoreAurasB = GetAuraEffectsByType(SPELL_AURA_MOD_TARGET_ABILITY_ABSORB_SCHOOL);
    for (AuraEffectList::const_iterator itr = AbsIgnoreAurasB.begin(); itr != AbsIgnoreAurasB.end(); ++itr)
    {
        if (!((*itr)->GetMiscValue() & schoolMask))
            continue;

        if (((*itr)->GetAmount() > auraAbsorbMod) && (*itr)->IsAffectingSpell(spellInfo))
            auraAbsorbMod = float((*itr)->GetAmount());
    }
    RoundToInterval(auraAbsorbMod, 0.0f, 100.0f);

    // We're going to call functions which can modify content of the list during iteration over it's elements
    // Let's copy the list so we can prevent iterator invalidation
    AuraEffectList vSchoolAbsorbCopy(victim->GetAuraEffectsByType(SPELL_AURA_SCHOOL_ABSORB));
    vSchoolAbsorbCopy.sort(Trinity::AbsorbAuraOrderPred());

    // absorb without mana cost
    for (AuraEffectList::iterator itr = vSchoolAbsorbCopy.begin(); (itr != vSchoolAbsorbCopy.end()) && (dmgInfo.GetDamage() > 0); ++itr)
    {
        AuraEffect* absorbAurEff = *itr;
        Aura* aura = absorbAurEff->GetBase();

        // Check if aura was removed during iteration - we don't need to work on such auras
        AuraApplication const* aurApp = aura->GetApplicationOfTarget(victim->GetGUID());
        if (!aurApp)
            continue;
        if (!(absorbAurEff->GetMiscValue() & schoolMask))
            continue;

        // get amount which can be still absorbed by the aura
        int32 currentAbsorb = absorbAurEff->GetAmount();
        // aura with infinite absorb amount - let the scripts handle absorbtion amount, set here to 0 for safety
        if (currentAbsorb < 0)
            currentAbsorb = 0;

        uint32 tempAbsorb = uint32(currentAbsorb);

        bool defaultPrevented = false;

        aura->CallScriptEffectAbsorbHandlers(absorbAurEff, aurApp, dmgInfo, tempAbsorb, defaultPrevented);
        currentAbsorb = tempAbsorb;

        if (defaultPrevented)
            continue;

        // Apply absorb mod auras
        AddPct(currentAbsorb, -auraAbsorbMod);

        // absorb must be smaller than the damage itself
        currentAbsorb = RoundToInterval(currentAbsorb, 0, int32(dmgInfo.GetDamage()));

        dmgInfo.AbsorbDamage(currentAbsorb);

        tempAbsorb = currentAbsorb;
        aura->CallScriptEffectAfterAbsorbHandlers(absorbAurEff, aurApp, dmgInfo, tempAbsorb);

        // Check if our aura is using amount to count damage
        if (absorbAurEff->GetAmount() >= 0)
        {
            // Reduce shield amount
            absorbAurEff->SetAmount(absorbAurEff->GetAmount() - currentAbsorb);
            // Aura cannot absorb anything more - remove it
            if (absorbAurEff->GetAmount() <= 0)
                aura->Remove(AURA_REMOVE_BY_ENEMY_SPELL);
        }
    }

    // absorb by mana cost
    AuraEffectList vManaShieldCopy(victim->GetAuraEffectsByType(SPELL_AURA_MANA_SHIELD));
    for (AuraEffectList::const_iterator itr = vManaShieldCopy.begin(); (itr != vManaShieldCopy.end()) && (dmgInfo.GetDamage() > 0); ++itr)
    {
        AuraEffect* absorbAurEff = *itr;
        Aura* aura = absorbAurEff->GetBase();

        // Check if aura was removed during iteration - we don't need to work on such auras
        AuraApplication const* aurApp = aura->GetApplicationOfTarget(victim->GetGUID());
        if (!aurApp)
            continue;
        // check damage school mask
        if (!(absorbAurEff->GetMiscValue() & schoolMask))
            continue;

        // get amount which can be still absorbed by the aura
        int32 currentAbsorb = absorbAurEff->GetAmount();
        // aura with infinite absorb amount - let the scripts handle absorbtion amount, set here to 0 for safety
        if (currentAbsorb < 0)
            currentAbsorb = 0;

        uint32 tempAbsorb = currentAbsorb;

        bool defaultPrevented = false;

        aura->CallScriptEffectManaShieldHandlers(absorbAurEff, aurApp, dmgInfo, tempAbsorb, defaultPrevented);
        currentAbsorb = tempAbsorb;

        if (defaultPrevented)
            continue;

        AddPct(currentAbsorb, -auraAbsorbMod);

        // absorb must be smaller than the damage itself
        currentAbsorb = RoundToInterval(currentAbsorb, 0, int32(dmgInfo.GetDamage()));

        int32 manaReduction = currentAbsorb;

        // lower absorb amount by talents
        if (float manaMultiplier = absorbAurEff->GetSpellInfo()->Effects[absorbAurEff->GetEffIndex()].CalcValueMultiplier(absorbAurEff->GetCaster()))
            manaReduction = int32(float(manaReduction) * manaMultiplier);

        int32 manaTaken = -victim->ModifyPower(POWER_MANA, -manaReduction);

        // take case when mana has ended up into account
        currentAbsorb = currentAbsorb ? int32(float(currentAbsorb) * (float(manaTaken) / float(manaReduction))) : 0;

        dmgInfo.AbsorbDamage(currentAbsorb);

        tempAbsorb = currentAbsorb;
        aura->CallScriptEffectAfterManaShieldHandlers(absorbAurEff, aurApp, dmgInfo, tempAbsorb);

        // Check if our aura is using amount to count damage
        if (absorbAurEff->GetAmount() >= 0)
        {
            absorbAurEff->SetAmount(absorbAurEff->GetAmount() - currentAbsorb);
            if ((absorbAurEff->GetAmount() <= 0))
                aura->Remove(AURA_REMOVE_BY_ENEMY_SPELL);
        }
    }

    // split damage auras - only when not damaging self
    if (victim != this)
    {
        // We're going to call functions which can modify content of the list during iteration over it's elements
        // Let's copy the list so we can prevent iterator invalidation
        AuraEffectList vSplitDamagePctCopy(victim->GetAuraEffectsByType(SPELL_AURA_SPLIT_DAMAGE_PCT));
        for (AuraEffectList::iterator itr = vSplitDamagePctCopy.begin(); (itr != vSplitDamagePctCopy.end()) &&  (dmgInfo.GetDamage() > 0); ++itr)
        {
            // Check if aura was removed during iteration - we don't need to work on such auras
            AuraApplication const* aurApp = (*itr)->GetBase()->GetApplicationOfTarget(victim->GetGUID());
            if (!aurApp)
                continue;

            // check damage school mask
            if (!((*itr)->GetMiscValue() & schoolMask))
                continue;

            // Damage can be splitted only if aura has an alive caster
            Unit* caster = (*itr)->GetCaster();
            if (!caster || (caster == victim) || !caster->IsInWorld() || !caster->isAlive())
                continue;

            uint32 splitDamage = CalculatePct(dmgInfo.GetDamage(), (*itr)->GetAmount());

            (*itr)->GetBase()->CallScriptEffectSplitHandlers((*itr), aurApp, dmgInfo, splitDamage);

            // absorb must be smaller than the damage itself
            splitDamage = RoundToInterval(splitDamage, uint32(0), uint32(dmgInfo.GetDamage()));

            dmgInfo.AbsorbDamage(splitDamage);

            // don't damage caster if he has immunity
            if (caster->IsImmunedToDamage((*itr)->GetSpellInfo()))
                continue;

            uint32 splitted = splitDamage;
            uint32 split_absorb = 0;
            DealDamageMods(caster, splitted, &split_absorb);

            SendSpellNonMeleeDamageLog(caster, (*itr)->GetSpellInfo()->Id, splitted, schoolMask, split_absorb, 0, false, 0, false);

            CleanDamage cleanDamage = CleanDamage(splitted, 0, BASE_ATTACK, MELEE_HIT_NORMAL);
            DealDamage(caster, splitted, &cleanDamage, DIRECT_DAMAGE, schoolMask, (*itr)->GetSpellInfo(), false);
        }
    }

    *resist = dmgInfo.GetResist();
    *absorb = dmgInfo.GetAbsorb();
}

void Unit::CalcHealAbsorb(Unit* victim, const SpellInfo* healSpell, uint32 &healAmount, uint32 &absorb)
{
    if (!healAmount)
        return;

    int32 RemainingHeal = healAmount;

    // Need remove expired auras after
    bool existExpired = false;

    // absorb without mana cost
    AuraEffectList const& vHealAbsorb = victim->GetAuraEffectsByType(SPELL_AURA_SCHOOL_HEAL_ABSORB);
    for (AuraEffectList::const_iterator i = vHealAbsorb.begin(); i != vHealAbsorb.end() && RemainingHeal > 0; ++i)
    {
        if (!((*i)->GetMiscValue() & healSpell->SchoolMask))
            continue;

        // Max Amount can be absorbed by this aura
        int32 currentAbsorb = (*i)->GetAmount();

        // Found empty aura (impossible but..)
        if (currentAbsorb <= 0)
        {
            existExpired = true;
            continue;
        }

        // currentAbsorb - damage can be absorbed by shield
        // If need absorb less damage
        if (RemainingHeal < currentAbsorb)
            currentAbsorb = RemainingHeal;

        RemainingHeal -= currentAbsorb;

        // Consuming Shroud, Warmaster Blackhorn, Dragon Soul
        if ((*i)->GetSpellInfo()->Id == 110214 || (*i)->GetSpellInfo()->Id == 110598)
            if (victim)
            {
                int32 bp0 = (RemainingHeal > 0 ? (healAmount - RemainingHeal) : healAmount);
                victim->CastCustomSpell((Unit*)NULL, 110215, &bp0, NULL, NULL, true);
            }

        // Reduce shield amount
        (*i)->SetAmount((*i)->GetAmount() - currentAbsorb);
        // Need remove it later
        if ((*i)->GetAmount() <= 0)
            existExpired = true;
    }

    // Remove all expired absorb auras
    if (existExpired)
    {
        for (AuraEffectList::const_iterator i = vHealAbsorb.begin(); i != vHealAbsorb.end();)
        {
            AuraEffect* auraEff = *i;
            ++i;
            if (auraEff->GetAmount() <= 0)
            {
                uint32 removedAuras = victim->m_removedAurasCount;
                auraEff->GetBase()->Remove(AURA_REMOVE_BY_ENEMY_SPELL);
                if (removedAuras+1 < victim->m_removedAurasCount)
                    i = vHealAbsorb.begin();
            }
        }
    }

    absorb = RemainingHeal > 0 ? (healAmount - RemainingHeal) : healAmount;
    healAmount = RemainingHeal;
}

void Unit::AttackerStateUpdate (Unit* victim, WeaponAttackType attType, bool extra)
{
    if (HasUnitState(UNIT_STATE_CANNOT_AUTOATTACK) || HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PACIFIED))
        return;

    if (!victim->isAlive())
        return;

    if (!IsAIEnabled || !GetMap()->Instanceable())
        if ((attType == BASE_ATTACK || attType == OFF_ATTACK) && !IsWithinLOSInMap(victim))
            return;

    CombatStart(victim);
    RemoveAurasWithInterruptFlags(AURA_INTERRUPT_FLAG_MELEE_ATTACK);

    if (attType != BASE_ATTACK && attType != OFF_ATTACK)
        return;                                             // ignore ranged case

    // melee attack spell casted at main hand attack only - no normal melee dmg dealt
    if (attType == BASE_ATTACK && m_currentSpells[CURRENT_MELEE_SPELL] && !extra)
        m_currentSpells[CURRENT_MELEE_SPELL]->cast();
    else
    {
        // attack can be redirected to another target
        victim = GetMeleeHitRedirectTarget(victim);

        CalcDamageInfo damageInfo;
        CalculateMeleeDamage(victim, 0, &damageInfo, attType);
        // Send log damage message to client
        DealDamageMods(victim, damageInfo.damage, &damageInfo.absorb);
        SendAttackStateUpdate(&damageInfo);

        //TriggerAurasProcOnEvent(damageInfo);
        ProcDamageAndSpell(damageInfo.target, damageInfo.procAttacker, damageInfo.procVictim, damageInfo.procEx, damageInfo.damage, damageInfo.attackType);

        DealMeleeDamage(&damageInfo, true);
    }
}

void Unit::HandleProcExtraAttackFor(Unit* victim)
{
    while (m_extraAttacks)
    {
        AttackerStateUpdate(victim, BASE_ATTACK, true);
        --m_extraAttacks;
    }
}

MeleeHitOutcome Unit::RollMeleeOutcomeAgainst(const Unit* victim, WeaponAttackType attType) const
{
    // This is only wrapper

    // Miss chance based on melee
    //float miss_chance = MeleeMissChanceCalc(victim, attType);
    float miss_chance = MeleeSpellMissChance(victim, attType, 0);

    // Critical hit chance
    float crit_chance = GetUnitCriticalChance(attType, victim);

    // stunned target cannot dodge and this is check in GetUnitDodgeChance() (returned 0 in this case)
    float dodge_chance = victim->GetUnitDodgeChance();
    float block_chance = victim->GetUnitBlockChance();
    float parry_chance = victim->GetUnitParryChance();

    return RollMeleeOutcomeAgainst(victim, attType, int32(crit_chance*100), int32(miss_chance*100), int32(dodge_chance*100), int32(parry_chance*100), int32(block_chance*100));
}

MeleeHitOutcome Unit::RollMeleeOutcomeAgainst (const Unit* victim, WeaponAttackType attType, int32 crit_chance, int32 miss_chance, int32 dodge_chance, int32 parry_chance, int32 block_chance) const
{
    if (victim->GetTypeId() == TYPEID_UNIT && victim->ToCreature()->IsInEvadeMode())
        return MELEE_HIT_EVADE;

    int32 attackerMaxSkillValueForLevel = GetMaxSkillValueForLevel(victim);
    int32 victimMaxSkillValueForLevel = victim->GetMaxSkillValueForLevel(this);

    // bonus from skills is 0.04%
    int32    skillBonus  = 4 * (attackerMaxSkillValueForLevel - victimMaxSkillValueForLevel);
    int32    sum = 0;
    int32    roll = urand (0, 10000);

    int32 tmp = miss_chance;

    if (tmp > 0 && roll < (sum += tmp))
        return MELEE_HIT_MISS;

    // always crit against a sitting target (except 0 crit chance)
    if (victim->GetTypeId() == TYPEID_PLAYER && crit_chance > 0 && !victim->IsStandState())
        return MELEE_HIT_CRIT;

    // Dodge chance

    // only players can't dodge if attacker is behind
    if (victim->GetTypeId() == TYPEID_PLAYER && !victim->HasInArc(M_PI, this) && !victim->HasAuraType(SPELL_AURA_IGNORE_HIT_DIRECTION))
    {
        //sLog->outDebug(LOG_FILTER_UNITS, "RollMeleeOutcomeAgainst: attack came from behind and victim was a player.");
    }
    else
    {
        // Reduce dodge chance by attacker expertise rating
        if (GetTypeId() == TYPEID_PLAYER)
            dodge_chance -= int32(ToPlayer()->GetExpertiseDodgeOrParryReduction(attType) * 100);
        else
            dodge_chance -= GetTotalAuraModifier(SPELL_AURA_MOD_EXPERTISE) * 25;

        // Modify dodge chance by attacker SPELL_AURA_MOD_COMBAT_RESULT_CHANCE
        dodge_chance+= GetTotalAuraModifierByMiscValue(SPELL_AURA_MOD_COMBAT_RESULT_CHANCE, VICTIMSTATE_DODGE) * 100;
        dodge_chance = int32 (float (dodge_chance) * GetTotalAuraMultiplier(SPELL_AURA_MOD_ENEMY_DODGE));

        tmp = dodge_chance;
        if ((tmp > 0)                                        // check if unit _can_ dodge
            && ((tmp -= skillBonus) > 0)
            && roll < (sum += tmp))
        {
            return MELEE_HIT_DODGE;
        }
    }

    // parry & block chances

    // check if attack comes from behind, nobody can parry or block if attacker is behind
    if (!victim->HasInArc(M_PI, this) && !victim->HasAuraType(SPELL_AURA_IGNORE_HIT_DIRECTION))
    {
        //sLog->outDebug(LOG_FILTER_UNITS, "RollMeleeOutcomeAgainst: attack came from behind.");
    }
    else
    {
        // Reduce parry chance by attacker expertise rating
        if (GetTypeId() == TYPEID_PLAYER)
            parry_chance -= int32(ToPlayer()->GetExpertiseDodgeOrParryReduction(attType) * 100);
        else
            parry_chance -= GetTotalAuraModifier(SPELL_AURA_MOD_EXPERTISE) * 25;

        if (victim->GetTypeId() == TYPEID_PLAYER || !(victim->ToCreature()->GetCreatureTemplate()->flags_extra & CREATURE_FLAG_EXTRA_NO_PARRY))
        {
            int32 tmp2 = int32(parry_chance);
            if (tmp2 > 0                                         // check if unit _can_ parry
                && (tmp2 -= skillBonus) > 0
                && roll < (sum += tmp2))
            {
                return MELEE_HIT_PARRY;
            }
        }

        if (victim->GetTypeId() == TYPEID_PLAYER || !(victim->ToCreature()->GetCreatureTemplate()->flags_extra & CREATURE_FLAG_EXTRA_NO_BLOCK))
        {
            tmp = block_chance;
            if (tmp > 0                                          // check if unit _can_ block
                && (tmp -= skillBonus) > 0
                && roll < (sum += tmp))
            {
                return MELEE_HIT_BLOCK;
            }
        }
    }

    // Critical chance
    tmp = crit_chance;

    if (tmp > 0 && roll < (sum += tmp))
    {
        if (GetTypeId() == TYPEID_UNIT && (ToCreature()->GetCreatureTemplate()->flags_extra & CREATURE_FLAG_EXTRA_NO_CRIT))
        {
            //sLog->outDebug(LOG_FILTER_UNITS, "RollMeleeOutcomeAgainst: CRIT DISABLED)");
        }
        else
            return MELEE_HIT_CRIT;
    }

    // Max 40% chance to score a glancing blow against mobs that are higher level (can do only players and pets and not with ranged weapon)
    if (attType != RANGED_ATTACK &&
        (GetTypeId() == TYPEID_PLAYER || ToCreature()->isPet()) &&
        victim->GetTypeId() != TYPEID_PLAYER && !victim->ToCreature()->isPet() &&
        getLevel() < victim->getLevelForTarget(this))
    {
        // cap possible value (with bonuses > max skill)
        int32 skill = attackerMaxSkillValueForLevel;

        tmp = (10 + (victimMaxSkillValueForLevel - skill)) * 100;
        tmp = tmp > 4000 ? 4000 : tmp;
        if (roll < (sum += tmp))
        {
            return MELEE_HIT_GLANCING;
        }
    }

    // mobs can score crushing blows if they're 4 or more levels above victim
    if (getLevelForTarget(victim) >= victim->getLevelForTarget(this) + 4 &&
        // can be from by creature (if can) or from controlled player that considered as creature
        !IsControlledByPlayer() &&
        !(GetTypeId() == TYPEID_UNIT && ToCreature()->GetCreatureTemplate()->flags_extra & CREATURE_FLAG_EXTRA_NO_CRUSH))
    {
        // when their weapon skill is 15 or more above victim's defense skill
        tmp = victimMaxSkillValueForLevel;
        // tmp = mob's level * 5 - player's current defense skill
        tmp = attackerMaxSkillValueForLevel - tmp;
        if (tmp >= 15)
        {
            // add 2% chance per lacking skill point, min. is 15%
            tmp = tmp * 200 - 1500;
            if (roll < (sum += tmp))
            {
                return MELEE_HIT_CRUSHING;
            }
        }
    }

    return MELEE_HIT_NORMAL;
}

uint32 Unit::CalculateDamage(WeaponAttackType attType, bool normalized, bool addTotalPct)
{
    float min_damage, max_damage;

    if (GetTypeId() == TYPEID_PLAYER && (normalized || !addTotalPct))
        ToPlayer()->CalculateMinMaxDamage(attType, normalized, addTotalPct, min_damage, max_damage);
    else
    {
        switch (attType)
        {
            case RANGED_ATTACK:
                min_damage = GetFloatValue(UNIT_FIELD_MINRANGEDDAMAGE);
                max_damage = GetFloatValue(UNIT_FIELD_MAXRANGEDDAMAGE);
                break;
            case BASE_ATTACK:
                min_damage = GetFloatValue(UNIT_FIELD_MINDAMAGE);
                max_damage = GetFloatValue(UNIT_FIELD_MAXDAMAGE);
                break;
            case OFF_ATTACK:
                min_damage = GetFloatValue(UNIT_FIELD_MINOFFHANDDAMAGE);
                max_damage = GetFloatValue(UNIT_FIELD_MAXOFFHANDDAMAGE);
                break;
                // Just for good manner
            default:
                min_damage = 0.0f;
                max_damage = 0.0f;
                break;
        }
    }

    float DoneTotalMod = 1.0f;

    min_damage *= DoneTotalMod;
    max_damage *= DoneTotalMod;

    if (min_damage > max_damage)
        std::swap(min_damage, max_damage);

    if (max_damage == 0.0f)
        max_damage = 5.0f;

    return urand((uint32)min_damage, (uint32)max_damage);
}

float Unit::CalculateLevelPenalty(SpellInfo const* spellProto) const
{
    if (spellProto->SpellLevel <= 0 || spellProto->SpellLevel >= spellProto->MaxLevel)
        return 1.0f;

    float LvlPenalty = 0.0f;

    if (spellProto->SpellLevel < 20)
        LvlPenalty = 20.0f - spellProto->SpellLevel * 3.75f;
    float LvlFactor = (float(spellProto->SpellLevel) + 6.0f) / float(getLevel());
    if (LvlFactor > 1.0f)
        LvlFactor = 1.0f;

    return AddPct(LvlFactor, -LvlPenalty);
}

void Unit::SendMeleeAttackStart(Unit* victim)
{
    WorldPacket data(SMSG_ATTACKSTART, 8 + 8);
    data << uint64(GetGUID());
    data << uint64(victim->GetGUID());
    SendMessageToSet(&data, true);
}

void Unit::SendMeleeAttackStop(Unit* victim)
{
    WorldPacket data(SMSG_ATTACKSTOP, (8+8+4));
    data.append(GetPackGUID());
    data.append(victim ? victim->GetPackGUID() : 0);
    data << uint32(0);                                     //! Can also take the value 0x01, which seems related to updating rotation
    SendMessageToSet(&data, true);
}

bool Unit::isSpellBlocked(Unit* victim, SpellInfo const* spellProto, WeaponAttackType /*attackType*/)
{
    // These spells can't be blocked
    if (spellProto && spellProto->Attributes & SPELL_ATTR0_IMPOSSIBLE_DODGE_PARRY_BLOCK)
        return false;

    if (victim->HasAuraType(SPELL_AURA_IGNORE_HIT_DIRECTION) || victim->HasInArc(M_PI, this))
    {
        // Check creatures flags_extra for disable block
        if (victim->GetTypeId() == TYPEID_UNIT &&
            victim->ToCreature()->GetCreatureTemplate()->flags_extra & CREATURE_FLAG_EXTRA_NO_BLOCK)
                return false;

        if (roll_chance_f(victim->GetUnitBlockChance()))
            return true;
    }
    return false;
}

bool Unit::isBlockCritical()
{
    if (roll_chance_i(GetTotalAuraModifier(SPELL_AURA_MOD_BLOCK_CRIT_CHANCE)))
        return true;
    return false;
}

int32 Unit::GetMechanicResistChance(const SpellInfo* spell)
{
    if (!spell)
        return 0;
    int32 resist_mech = 0;
    for (uint8 eff = 0; eff < MAX_SPELL_EFFECTS; ++eff)
    {
        if (!spell->Effects[eff].IsEffect())
           break;
        int32 effect_mech = spell->GetEffectMechanic(eff);
        if (effect_mech)
        {
            int32 temp = GetTotalAuraModifierByMiscValue(SPELL_AURA_MOD_MECHANIC_RESISTANCE, effect_mech);
            if (resist_mech < temp)
                resist_mech = temp;
        }
    }
    return resist_mech;
}

// Melee based spells hit result calculations
SpellMissInfo Unit::MeleeSpellHitResult(Unit* victim, SpellInfo const* spell)
{
    // Spells with SPELL_ATTR3_IGNORE_HIT_RESULT will additionally fully ignore
    // resist and deflect chances
    if (spell->AttributesEx3 & SPELL_ATTR3_IGNORE_HIT_RESULT || spell->IsInterruptSpell())
        return SPELL_MISS_NONE;

    WeaponAttackType attType = BASE_ATTACK;

    // Check damage class instead of attack type to correctly handle judgements
    // - they are meele, but can't be dodged/parried/deflected because of ranged dmg class
    if (spell->DmgClass == SPELL_DAMAGE_CLASS_RANGED)
        attType = RANGED_ATTACK;

    uint32 roll = urand (0, 10000);

    uint32 missChance = uint32(MeleeSpellMissChance(victim, attType, spell->Id) * 100.0f);
    // Roll miss
    uint32 tmp = missChance;
    if (roll < tmp)
        return SPELL_MISS_MISS;

    // Chance resist mechanic (select max value from every mechanic spell effect)
    int32 resist_mech = 0;
    // Get effects mechanic and chance
    for (uint8 eff = 0; eff < MAX_SPELL_EFFECTS; ++eff)
    {
        int32 effect_mech = spell->GetEffectMechanic(eff);
        if (effect_mech)
        {
            int32 temp = victim->GetTotalAuraModifierByMiscValue(SPELL_AURA_MOD_MECHANIC_RESISTANCE, effect_mech);
            if (resist_mech < temp*100)
                resist_mech = temp*100;
        }
    }
    // Roll chance
    tmp += resist_mech;
    if (roll < tmp)
        return SPELL_MISS_RESIST;

    bool canDodge = true;
    bool canParry = true;
    bool canBlock = spell->AttributesEx3 & SPELL_ATTR3_BLOCKABLE_SPELL;

    // Same spells cannot be parry/dodge
    if (spell->Attributes & SPELL_ATTR0_IMPOSSIBLE_DODGE_PARRY_BLOCK)
        return SPELL_MISS_NONE;

    // Chance resist mechanic
    int32 resist_chance = victim->GetMechanicResistChance(spell) * 100;
    tmp += resist_chance;
    if (roll < tmp)
        return SPELL_MISS_RESIST;

    // Ranged attacks can only miss, resist and deflect
    if (attType == RANGED_ATTACK)
    {
        canParry = false;
        canDodge = false;

        // only if in front
        if (victim->HasInArc(M_PI, this) || victim->HasAuraType(SPELL_AURA_IGNORE_HIT_DIRECTION))
        {
            int32 deflect_chance = victim->GetTotalAuraModifier(SPELL_AURA_DEFLECT_SPELLS) * 100;
            tmp += deflect_chance;
            if (roll < tmp)
                return SPELL_MISS_DEFLECT;
        }
        return SPELL_MISS_NONE;
    }

    // Check for attack from behind
    if (!victim->HasInArc(M_PI, this))
    {
        if (!victim->HasAuraType(SPELL_AURA_IGNORE_HIT_DIRECTION))
        {
            // Can`t dodge from behind in PvP (but its possible in PvE)
            if (victim->GetTypeId() == TYPEID_PLAYER)
                canDodge = false;
            // Can`t parry or block
            canParry = false;
            canBlock = false;
        }
        else // Only deterrence as of 3.3.5
        {
            if (spell->AttributesCu & SPELL_ATTR0_CU_REQ_CASTER_BEHIND_TARGET)
                canParry = false;
        }
    }
    // Check creatures flags_extra for disable parry
    if (victim->GetTypeId() == TYPEID_UNIT)
    {
        uint32 flagEx = victim->ToCreature()->GetCreatureTemplate()->flags_extra;
        if (flagEx & CREATURE_FLAG_EXTRA_NO_PARRY)
            canParry = false;
        // Check creatures flags_extra for disable block
        if (flagEx & CREATURE_FLAG_EXTRA_NO_BLOCK)
            canBlock = false;
    }
    // Ignore combat result aura
    AuraEffectList const& ignore = GetAuraEffectsByType(SPELL_AURA_IGNORE_COMBAT_RESULT);
    for (AuraEffectList::const_iterator i = ignore.begin(); i != ignore.end(); ++i)
    {
        if (!(*i)->IsAffectingSpell(spell))
            continue;
        switch ((*i)->GetMiscValue())
        {
            case MELEE_HIT_DODGE: canDodge = false; break;
            case MELEE_HIT_BLOCK: canBlock = false; break;
            case MELEE_HIT_PARRY: canParry = false; break;
            default:
                break;
        }
    }

    if (canDodge)
    {
        // Roll dodge
        int32 dodgeChance = int32(victim->GetUnitDodgeChance() * 100.0f);
        // Reduce enemy dodge chance by SPELL_AURA_MOD_COMBAT_RESULT_CHANCE
        dodgeChance += GetTotalAuraModifierByMiscValue(SPELL_AURA_MOD_COMBAT_RESULT_CHANCE, VICTIMSTATE_DODGE) * 100;
        dodgeChance = int32(float(dodgeChance) * GetTotalAuraMultiplier(SPELL_AURA_MOD_ENEMY_DODGE));
        // Reduce dodge chance by attacker expertise rating
        if (GetTypeId() == TYPEID_PLAYER)
            dodgeChance -= int32(ToPlayer()->GetExpertiseDodgeOrParryReduction(attType) * 100.0f);
        else
            dodgeChance -= GetTotalAuraModifier(SPELL_AURA_MOD_EXPERTISE) * 25;
        if (dodgeChance < 0)
            dodgeChance = 0;

        if (roll < (tmp += dodgeChance))
            return SPELL_MISS_DODGE;
    }

    if (canParry)
    {
        // Roll parry
        int32 parryChance = int32(victim->GetUnitParryChance() * 100.0f);
        // Reduce parry chance by attacker expertise rating
        if (GetTypeId() == TYPEID_PLAYER)
            parryChance -= int32(ToPlayer()->GetExpertiseDodgeOrParryReduction(attType) * 100.0f);
        else
            parryChance -= GetTotalAuraModifier(SPELL_AURA_MOD_EXPERTISE) * 25;
        if (parryChance < 0)
            parryChance = 0;

        tmp += parryChance;
        if (roll < tmp)
            return SPELL_MISS_PARRY;
    }

    if (canBlock)
    {
        int32 blockChance = int32(victim->GetUnitBlockChance() * 100.0f);
        if (blockChance < 0)
            blockChance = 0;
        tmp += blockChance;

        if (roll < tmp)
            return SPELL_MISS_BLOCK;
    }

    return SPELL_MISS_NONE;
}

// TODO need use unit spell resistances in calculations
SpellMissInfo Unit::MagicSpellHitResult(Unit* victim, SpellInfo const* spell)
{
    // Can`t miss on dead target (on skinning for example)
    if (!victim->isAlive() && victim->GetTypeId() != TYPEID_PLAYER)
        return SPELL_MISS_NONE;

    if (spell->IsInterruptSpell())
    {
        // only deflect works here
        int32 deflect_chance = victim->GetTotalAuraModifier(SPELL_AURA_DEFLECT_SPELLS);
        if (roll_chance_i(deflect_chance))
            return SPELL_MISS_DEFLECT;

        return SPELL_MISS_NONE;
    }

    SpellSchoolMask schoolMask = spell->GetSchoolMask();
    // PvP - PvE spell misschances per leveldif > 2
    int32 lchance = victim->GetTypeId() == TYPEID_PLAYER ? 7 : 11;
    int32 thisLevel = getLevelForTarget(victim);
    if (GetTypeId() == TYPEID_UNIT && ToCreature()->isTrigger())
        thisLevel = std::max<int32>(thisLevel, spell->SpellLevel);
    int32 leveldif = int32(victim->getLevelForTarget(this)) - thisLevel;

    // Base hit chance from attacker and victim levels
    int32 modHitChance;
    if (leveldif < 3)
        modHitChance = 96 - leveldif;
    else
        modHitChance = 94 - (leveldif - 2) * lchance;

    // Spellmod from SPELLMOD_RESIST_MISS_CHANCE
    if (Player* modOwner = GetSpellModOwner())
        modOwner->ApplySpellMod(spell->Id, SPELLMOD_RESIST_MISS_CHANCE, modHitChance);

    // Spells with SPELL_ATTR3_IGNORE_HIT_RESULT will ignore target's avoidance effects
    if (!(spell->AttributesEx3 & SPELL_ATTR3_IGNORE_HIT_RESULT))
    {
        // Chance hit from victim SPELL_AURA_MOD_ATTACKER_SPELL_HIT_CHANCE auras
        modHitChance += victim->GetTotalAuraModifierByMiscMask(SPELL_AURA_MOD_ATTACKER_SPELL_HIT_CHANCE, schoolMask);

        // Decrease hit chance from victim rating bonus
        if (victim->GetTypeId() == TYPEID_PLAYER)
            modHitChance -= int32(victim->ToPlayer()->GetRatingBonusValue(CR_HIT_TAKEN_SPELL));
    }

    int32 HitChance = modHitChance * 100;
    // Increase hit chance from attacker SPELL_AURA_MOD_SPELL_HIT_CHANCE and attacker ratings
    HitChance += int32(m_modSpellHitChance * 100.0f);

    if (HitChance < 100)
        HitChance = 100;
    else if (HitChance > 10000)
        HitChance = 10000;

    int32 tmp = 10000 - HitChance;

    int32 rand = irand(0, 10000);

    if (rand < tmp)
        return SPELL_MISS_MISS;

    // Spells with SPELL_ATTR3_IGNORE_HIT_RESULT will additionally fully ignore
    // resist and deflect chances
    if (spell->AttributesEx3 & SPELL_ATTR3_IGNORE_HIT_RESULT)
        return SPELL_MISS_NONE;

    // Chance resist mechanic (select max value from every mechanic spell effect)
    int32 resist_chance = victim->GetMechanicResistChance(spell) * 100;
    tmp += resist_chance;

   // Roll chance
    if (rand < tmp)
        return SPELL_MISS_RESIST;

    // cast by caster in front of victim
    if (victim->HasInArc(M_PI, this) || victim->HasAuraType(SPELL_AURA_IGNORE_HIT_DIRECTION))
    {
        int32 deflect_chance = victim->GetTotalAuraModifier(SPELL_AURA_DEFLECT_SPELLS) * 100;
        tmp += deflect_chance;
        if (rand < tmp)
            return SPELL_MISS_DEFLECT;
    }

    return SPELL_MISS_NONE;
}

// Calculate spell hit result can be:
// Every spell can: Evade/Immune/Reflect/Sucesful hit
// For melee based spells:
//   Miss
//   Dodge
//   Parry
// For spells
//   Resist
SpellMissInfo Unit::SpellHitResult(Unit* victim, SpellInfo const* spell, bool CanReflect)
{
    // Check for immune
    if (victim->IsImmunedToSpell(spell))
        return SPELL_MISS_IMMUNE;

    // All positive spells can`t miss
    // TODO: client not show miss log for this spells - so need find info for this in dbc and use it!
    if (spell->IsPositive()
        &&(!IsHostileTo(victim)))  // prevent from affecting enemy by "positive" spell
        return SPELL_MISS_NONE;
    // Check for immune
    if (victim->IsImmunedToDamage(spell))
        return SPELL_MISS_IMMUNE;

    if (this == victim)
        return SPELL_MISS_NONE;

    // Return evade for units in evade mode
    if (victim->GetTypeId() == TYPEID_UNIT && victim->ToCreature()->IsInEvadeMode())
        return SPELL_MISS_EVADE;

    // Try victim reflect spell
    if (CanReflect)
    {
        int32 reflectchance = victim->GetTotalAuraModifier(SPELL_AURA_REFLECT_SPELLS);
        Unit::AuraEffectList const& mReflectSpellsSchool = victim->GetAuraEffectsByType(SPELL_AURA_REFLECT_SPELLS_SCHOOL);
        for (Unit::AuraEffectList::const_iterator i = mReflectSpellsSchool.begin(); i != mReflectSpellsSchool.end(); ++i)
            if ((*i)->GetMiscValue() & spell->GetSchoolMask())
                reflectchance += (*i)->GetAmount();

        if (reflectchance > 0 && roll_chance_i(reflectchance))
        {
//             Unit::AuraEffectList alist = victim->GetAuraEffectsByType(SPELL_AURA_REFLECT_SPELLS);
//             for (AuraEffectList::const_iterator i = alist.begin(); i != alist.end(); ++i)
//                 (*i)->GetBase()->DropCharge();

            return SPELL_MISS_REFLECT;
        }
    }

    // Taunt-like spells can't miss since 4.0.1
    if (spell->HasEffect(SPELL_EFFECT_ATTACK_ME))
        return SPELL_MISS_NONE;

    // Taunt-like spells can't miss since 4.0.1
    if (spell->HasEffect(SPELL_EFFECT_ATTACK_ME))
        return SPELL_MISS_NONE;
    Unit * checker = (isTotem() && GetOwner()) ? GetOwner() : this;
    switch (spell->DmgClass)
    {
        case SPELL_DAMAGE_CLASS_RANGED:
        case SPELL_DAMAGE_CLASS_MELEE:
            return checker->MeleeSpellHitResult(victim, spell);
        case SPELL_DAMAGE_CLASS_NONE:
        {
            // Warrior Charge
            if (spell->SpellFamilyName == SPELLFAMILY_WARRIOR && (spell->SpellFamilyFlags[0] & 0x01000000))
                return checker->MeleeSpellHitResult(victim, spell);

            return SPELL_MISS_NONE;
        }
        case SPELL_DAMAGE_CLASS_MAGIC:
            return checker->MagicSpellHitResult(victim, spell);
    }
    return SPELL_MISS_NONE;
}

uint32 Unit::GetShieldBlockValue(uint32 soft_cap, uint32 hard_cap) const
{
    uint32 value = GetShieldBlockValue();
    if (value >= hard_cap)
    {
        value = (soft_cap + hard_cap) / 2;
    }
    else if (value > soft_cap)
    {
        value = soft_cap + ((value - soft_cap) / 2);
    }

    return value;
}

float Unit::GetUnitDodgeChance() const
{
    if (IsNonMeleeSpellCasted(false) || HasUnitState(UNIT_STATE_CONTROLLED))
        return 0.0f;

    if (GetTypeId() == TYPEID_PLAYER)
        return GetFloatValue(PLAYER_DODGE_PERCENTAGE);
    else
    {
        if (ToCreature()->isTotem())
            return 0.0f;
        else
        {
            float dodge = 5.0f;
            dodge += GetTotalAuraModifier(SPELL_AURA_MOD_DODGE_PERCENT);
            return dodge > 0.0f ? dodge : 0.0f;
        }
    }
}

float Unit::GetUnitParryChance() const
{
    if (IsNonMeleeSpellCasted(false) || HasUnitState(UNIT_STATE_CONTROLLED))
        return 0.0f;

    float chance = 0.0f;

    if (Player const* player = ToPlayer())
    {
        if (player->CanParry())
        {
            Item* tmpitem = player->GetWeaponForAttack(BASE_ATTACK, true);
            if (!tmpitem)
                tmpitem = player->GetWeaponForAttack(OFF_ATTACK, true);

            if (tmpitem)
                chance = GetFloatValue(PLAYER_PARRY_PERCENTAGE);
        }
    }
    else if (GetTypeId() == TYPEID_UNIT)
    {
        if (GetCreatureType() == CREATURE_TYPE_HUMANOID)
        {
            chance = 5.0f;
            chance += GetTotalAuraModifier(SPELL_AURA_MOD_PARRY_PERCENT);
        }
    }

    return chance > 0.0f ? chance : 0.0f;
}

float Unit::GetUnitMissChance(WeaponAttackType attType) const
{
    float miss_chance = 5.00f;

    if (attType == RANGED_ATTACK)
        miss_chance -= GetTotalAuraModifier(SPELL_AURA_MOD_ATTACKER_RANGED_HIT_CHANCE);
    else
        miss_chance -= GetTotalAuraModifier(SPELL_AURA_MOD_ATTACKER_MELEE_HIT_CHANCE);

    return miss_chance;
}

float Unit::GetUnitBlockChance() const
{
    if (IsNonMeleeSpellCasted(false) || HasUnitState(UNIT_STATE_CONTROLLED))
        return 0.0f;

    if (Player const* player = ToPlayer())
    {
        if (player->CanBlock())
        {
            Item* tmpitem = player->GetUseableItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_OFFHAND);
            if (tmpitem && !tmpitem->IsBroken())
                return GetFloatValue(PLAYER_BLOCK_PERCENTAGE);
        }
        // is player but has no block ability or no not broken shield equipped
        return 0.0f;
    }
    else
    {
        if (ToCreature()->isTotem())
            return 0.0f;
        else
        {
            float block = 5.0f;
            block += GetTotalAuraModifier(SPELL_AURA_MOD_BLOCK_PERCENT);
            return block > 0.0f ? block : 0.0f;
        }
    }
}

float Unit::GetUnitCriticalChance(WeaponAttackType attackType, const Unit* victim) const
{
    float crit;

    if (GetTypeId() == TYPEID_PLAYER)
    {
        switch (attackType)
        {
            case BASE_ATTACK:
                crit = GetFloatValue(PLAYER_CRIT_PERCENTAGE);
                break;
            case OFF_ATTACK:
                crit = GetFloatValue(PLAYER_OFFHAND_CRIT_PERCENTAGE);
                break;
            case RANGED_ATTACK:
                crit = GetFloatValue(PLAYER_RANGED_CRIT_PERCENTAGE);
                break;
                // Just for good manner
            default:
                crit = 0.0f;
                break;
        }
    }
    else
    {
        crit = 5.0f;
        crit += GetTotalAuraModifier(SPELL_AURA_MOD_WEAPON_CRIT_PERCENT);
        crit += GetTotalAuraModifier(SPELL_AURA_MOD_CRIT_PCT);
    }

    // flat aura mods
    if (attackType == RANGED_ATTACK)
        crit += victim->GetTotalAuraModifier(SPELL_AURA_MOD_ATTACKER_RANGED_CRIT_CHANCE);
    else
        crit += victim->GetTotalAuraModifier(SPELL_AURA_MOD_ATTACKER_MELEE_CRIT_CHANCE);

    crit += victim->GetTotalAuraModifier(SPELL_AURA_MOD_ATTACKER_SPELL_AND_WEAPON_CRIT_CHANCE);

    if (crit < 0.0f)
        crit = 0.0f;
    return crit;
}

void Unit::_DeleteRemovedAuras()
{
    while (!m_removedAuras.empty())
    {
        delete m_removedAuras.front();
        m_removedAuras.pop_front();
    }
}

void Unit::_UpdateSpells(uint32 time)
{
    if (m_currentSpells[CURRENT_AUTOREPEAT_SPELL])
        _UpdateAutoRepeatSpell();

    // remove finished spells from current pointers
    for (uint32 i = 0; i < CURRENT_MAX_SPELL; ++i)
    {
        if (m_currentSpells[i] && m_currentSpells[i]->getState() == SPELL_STATE_FINISHED)
        {
            m_currentSpells[i]->SetReferencedFromCurrent(false);
            m_currentSpells[i] = NULL;                      // remove pointer
        }
    }

    // m_auraUpdateIterator can be updated in indirect called code at aura remove to skip next planned to update but removed auras
    for (m_auraUpdateIterator = m_ownedAuras.begin(); m_auraUpdateIterator != m_ownedAuras.end();)
    {
        Aura* i_aura = m_auraUpdateIterator->second;
        ++m_auraUpdateIterator;                            // need shift to next for allow update if need into aura update

        i_aura->UpdateOwner(time, this);
    }

    // remove expired auras - do that after updates(used in scripts?)
    for (AuraMap::iterator i = m_ownedAuras.begin(); i != m_ownedAuras.end();)
    {
        if (i->second->IsExpired())
            RemoveOwnedAura(i, AURA_REMOVE_BY_EXPIRE);
        else
            ++i;
    }

    for (VisibleAuraMap::iterator itr = m_visibleAuras.begin(); itr != m_visibleAuras.end(); ++itr)
        if (itr->second->IsNeedClientUpdate())
            itr->second->ClientUpdate();

    _DeleteRemovedAuras();

    if (!m_gameObj.empty())
    {
        GameObjectList::iterator itr;
        for (itr = m_gameObj.begin(); itr != m_gameObj.end();)
        {
            if (!(*itr)->isSpawned())
            {
                (*itr)->SetOwnerGUID(0);
                (*itr)->SetRespawnTime(0);
                (*itr)->Delete();
                m_gameObj.erase(itr++);
            }
            else
                ++itr;
        }
    }
}

void Unit::_UpdateAutoRepeatSpell()
{
    // check "realtime" interrupts
    // don't cancel spells which are affected by a SPELL_AURA_CAST_WHILE_WALKING effect
    if (((GetTypeId() == TYPEID_PLAYER && ToPlayer()->isMoving()) || IsNonMeleeSpellCasted(false, false, true, m_currentSpells[CURRENT_AUTOREPEAT_SPELL]->m_spellInfo->Id == 75)) &&
        !HasAuraTypeWithAffectMask(SPELL_AURA_CAST_WHILE_WALKING, m_currentSpells[CURRENT_AUTOREPEAT_SPELL]->m_spellInfo))
    {
        // cancel wand shoot
        if (m_currentSpells[CURRENT_AUTOREPEAT_SPELL]->m_spellInfo->Id != 75)
            InterruptSpell(CURRENT_AUTOREPEAT_SPELL);
        m_AutoRepeatFirstCast = true;
        return;
    }

    // apply delay (Auto Shot (spellID 75) not affected)
    if (m_AutoRepeatFirstCast && getAttackTimer(RANGED_ATTACK) < 500 && m_currentSpells[CURRENT_AUTOREPEAT_SPELL]->m_spellInfo->Id != 75)
        setAttackTimer(RANGED_ATTACK, 500);
    m_AutoRepeatFirstCast = false;

    // castroutine
    if (isAttackReady(RANGED_ATTACK))
    {
        // Check if able to cast
        for (uint8 i = 0; i < CURRENT_MAX_SPELL; ++i)
        {
            if (Spell* currentSpell = m_currentSpells[i])
            {
                if (i == CURRENT_AUTOREPEAT_SPELL && currentSpell->CheckCast(true) != SPELL_CAST_OK)
                {
                    InterruptSpell(CURRENT_AUTOREPEAT_SPELL);
                    return;
                }
            }
        }

        // we want to shoot
        Spell* spell = new Spell(this, m_currentSpells[CURRENT_AUTOREPEAT_SPELL]->m_spellInfo, TRIGGERED_FULL_MASK);
        spell->prepare(&(m_currentSpells[CURRENT_AUTOREPEAT_SPELL]->m_targets));

        // all went good, reset attack
        resetAttackTimer(RANGED_ATTACK);
    }
}

void Unit::SetCurrentCastedSpell(Spell* pSpell)
{
    ASSERT(pSpell);                                         // NULL may be never passed here, use InterruptSpell or InterruptNonMeleeSpells

    CurrentSpellTypes CSpellType = pSpell->GetCurrentContainer();

    if (pSpell == m_currentSpells[CSpellType])             // avoid breaking self
        return;

    // break same type spell if it is not delayed
    InterruptSpell(CSpellType, false);

    // special breakage effects:
    switch (CSpellType)
    {
        case CURRENT_GENERIC_SPELL:
        {
            // generic spells always break channeled not delayed spells
            InterruptSpell(CURRENT_CHANNELED_SPELL, false);

            // autorepeat breaking
            if (m_currentSpells[CURRENT_AUTOREPEAT_SPELL])
            {
                // break autorepeat if not Auto Shot
                if (m_currentSpells[CURRENT_AUTOREPEAT_SPELL]->m_spellInfo->Id != 75)
                    InterruptSpell(CURRENT_AUTOREPEAT_SPELL);
                m_AutoRepeatFirstCast = true;
            }
            if (pSpell->m_spellInfo->CalcCastTime(this) > 0)
                AddUnitState(UNIT_STATE_CASTING);

            break;
        }
        case CURRENT_CHANNELED_SPELL:
        {
            // channel spells always break generic non-delayed and any channeled spells
            InterruptSpell(CURRENT_GENERIC_SPELL, false);
            InterruptSpell(CURRENT_CHANNELED_SPELL);

            // it also does break autorepeat if not Auto Shot
            if (m_currentSpells[CURRENT_AUTOREPEAT_SPELL] &&
                m_currentSpells[CURRENT_AUTOREPEAT_SPELL]->m_spellInfo->Id != 75)
                InterruptSpell(CURRENT_AUTOREPEAT_SPELL);
            AddUnitState(UNIT_STATE_CASTING);

            break;
        }
        case CURRENT_AUTOREPEAT_SPELL:
        {
            // only Auto Shoot does not break anything
            if (pSpell->m_spellInfo->Id != 75)
            {
                // generic autorepeats break generic non-delayed and channeled non-delayed spells
                InterruptSpell(CURRENT_GENERIC_SPELL, false);
                InterruptSpell(CURRENT_CHANNELED_SPELL, false);
            }
            // special action: set first cast flag
            m_AutoRepeatFirstCast = true;

            break;
        }
        default:
            break; // other spell types don't break anything now
    }

    // current spell (if it is still here) may be safely deleted now
    if (m_currentSpells[CSpellType])
        m_currentSpells[CSpellType]->SetReferencedFromCurrent(false);

    // set new current spell
    m_currentSpells[CSpellType] = pSpell;
    pSpell->SetReferencedFromCurrent(true);

    WorldPacket unitFrame(SMSG_UNIT_SPELLCAST_START, 29);

    unitFrame << pSpell->GetCaster()->GetGUID();
    unitFrame << pSpell->m_targets.GetObjectTargetGUID();
    unitFrame << pSpell->GetSpellInfo()->Id;
    unitFrame << int32(0);                  //time casted
    unitFrame << pSpell->GetCastTime();
    unitFrame << uint8(0);

    SendMessageToSet(&unitFrame, false);

    pSpell->m_selfContainer = &(m_currentSpells[pSpell->GetCurrentContainer()]);
}

void Unit::InterruptSpell(CurrentSpellTypes spellType, bool withDelayed, bool withInstant)
{
    ASSERT(spellType < CURRENT_MAX_SPELL);

    Spell* spell = m_currentSpells[spellType];
    if (spell
        && (withDelayed || spell->getState() != SPELL_STATE_DELAYED)
        && (withInstant || spell->GetCastTime() > 0))
    {
        // for example, do not let self-stun aura interrupt itself
        if (!spell->IsInterruptable())
            return;

        // send autorepeat cancel message for autorepeat spells
        if (spellType == CURRENT_AUTOREPEAT_SPELL)
            if (GetTypeId() == TYPEID_PLAYER)
                ToPlayer()->SendAutoRepeatCancel(this);

        if (spell->getState() != SPELL_STATE_FINISHED)
            spell->cancel();

        m_currentSpells[spellType] = NULL;
        spell->SetReferencedFromCurrent(false);
    }
}

void Unit::FinishSpell(CurrentSpellTypes spellType, bool ok /*= true*/)
{
    Spell* spell = m_currentSpells[spellType];
    if (!spell)
        return;

    if (spellType == CURRENT_CHANNELED_SPELL)
        spell->SendChannelUpdate(0);

    spell->finish(ok);
}

bool Unit::IsNonMeleeSpellCasted(bool withDelayed, bool skipChanneled, bool skipAutorepeat, bool isAutoshoot, bool skipInstant) const
{
    // We don't do loop here to explicitly show that melee spell is excluded.
    // Maybe later some special spells will be excluded too.

    // if skipInstant then instant spells shouldn't count as being casted
    if (skipInstant && m_currentSpells[CURRENT_GENERIC_SPELL] && !m_currentSpells[CURRENT_GENERIC_SPELL]->GetCastTime())
        return false;

    // generic spells are casted when they are not finished and not delayed
    if (m_currentSpells[CURRENT_GENERIC_SPELL] &&
        (m_currentSpells[CURRENT_GENERIC_SPELL]->getState() != SPELL_STATE_FINISHED) &&
        (withDelayed || m_currentSpells[CURRENT_GENERIC_SPELL]->getState() != SPELL_STATE_DELAYED))
    {
        if (!isAutoshoot || !(m_currentSpells[CURRENT_GENERIC_SPELL]->m_spellInfo->AttributesEx2 & SPELL_ATTR2_NOT_RESET_AUTO_ACTIONS))
            return true;
    }
    // channeled spells may be delayed, but they are still considered casted
    else if (!skipChanneled && m_currentSpells[CURRENT_CHANNELED_SPELL] &&
        (m_currentSpells[CURRENT_CHANNELED_SPELL]->getState() != SPELL_STATE_FINISHED))
    {
        if (!isAutoshoot || !(m_currentSpells[CURRENT_CHANNELED_SPELL]->m_spellInfo->AttributesEx2 & SPELL_ATTR2_NOT_RESET_AUTO_ACTIONS))
            return true;
    }
    // autorepeat spells may be finished or delayed, but they are still considered casted
    else if (!skipAutorepeat && m_currentSpells[CURRENT_AUTOREPEAT_SPELL])
        return true;

    return false;
}

void Unit::InterruptNonMeleeSpells(bool withDelayed, uint32 spell_id, bool withInstant)
{
    // generic spells are interrupted if they are not finished or delayed
    if (m_currentSpells[CURRENT_GENERIC_SPELL] && (!spell_id || m_currentSpells[CURRENT_GENERIC_SPELL]->m_spellInfo->Id == spell_id))
        InterruptSpell(CURRENT_GENERIC_SPELL, withDelayed, withInstant);

    // autorepeat spells are interrupted if they are not finished or delayed
    if (m_currentSpells[CURRENT_AUTOREPEAT_SPELL] && (!spell_id || m_currentSpells[CURRENT_AUTOREPEAT_SPELL]->m_spellInfo->Id == spell_id))
        InterruptSpell(CURRENT_AUTOREPEAT_SPELL, withDelayed, withInstant);

    // channeled spells are interrupted if they are not finished, even if they are delayed
    if (m_currentSpells[CURRENT_CHANNELED_SPELL] && (!spell_id || m_currentSpells[CURRENT_CHANNELED_SPELL]->m_spellInfo->Id == spell_id))
        InterruptSpell(CURRENT_CHANNELED_SPELL, true, true);
}

void Unit::InterruptNonMeleeSpellsExcept(bool withDelayed, uint32 except, bool withInstant)
{
    // generic spells are interrupted if they are not finished or delayed
    if (m_currentSpells[CURRENT_GENERIC_SPELL] && m_currentSpells[CURRENT_GENERIC_SPELL]->m_spellInfo->Id != except)
        InterruptSpell(CURRENT_GENERIC_SPELL, withDelayed, withInstant);

    // autorepeat spells are interrupted if they are not finished or delayed
    if (m_currentSpells[CURRENT_AUTOREPEAT_SPELL] && m_currentSpells[CURRENT_AUTOREPEAT_SPELL]->m_spellInfo->Id != except)
        InterruptSpell(CURRENT_AUTOREPEAT_SPELL, withDelayed, withInstant);

    // channeled spells are interrupted if they are not finished, even if they are delayed
    if (m_currentSpells[CURRENT_CHANNELED_SPELL] && m_currentSpells[CURRENT_CHANNELED_SPELL]->m_spellInfo->Id != except)
        InterruptSpell(CURRENT_CHANNELED_SPELL, true, true);
}

Spell* Unit::FindCurrentSpellBySpellId(uint32 spell_id) const
{
    for (uint32 i = 0; i < CURRENT_MAX_SPELL; i++)
        if (m_currentSpells[i] && m_currentSpells[i]->m_spellInfo->Id == spell_id)
            return m_currentSpells[i];
    return NULL;
}

int32 Unit::GetCurrentSpellCastTime(uint32 spell_id) const
{
    if (Spell const* spell = FindCurrentSpellBySpellId(spell_id))
        return spell->GetCastTime();
    return 0;
}

bool Unit::isInFrontInMap(Unit const* target, float distance,  float arc) const
{
    return IsWithinDistInMap(target, distance) && HasInArc(arc, target);
}

bool Unit::isInBackInMap(Unit const* target, float distance, float arc) const
{
    return IsWithinDistInMap(target, distance) && !HasInArc(2 * M_PI - arc, target);
}

bool Unit::isInAccessiblePlaceFor(Creature const* c) const
{
    if (IsInWater())
        return c->canSwim();
    else
        return c->canWalk() || c->CanFly();
}

bool Unit::IsInWater() const
{
    return GetBaseMap()->IsInWater(GetPositionX(), GetPositionY(), GetPositionZ());
}

bool Unit::IsUnderWater() const
{
    return GetBaseMap()->IsUnderWater(GetPositionX(), GetPositionY(), GetPositionZ());
}

void Unit::UpdateUnderwaterState(Map* m, float x, float y, float z)
{
    if (!isPet() && !IsVehicle())
        return;

    LiquidData liquid_status;
    ZLiquidStatus res = m->getLiquidStatus(x, y, z, MAP_ALL_LIQUIDS, &liquid_status);
    if (!res)
    {
        if (_lastLiquid && _lastLiquid->SpellId)
            RemoveAurasDueToSpell(_lastLiquid->SpellId);

        RemoveAurasWithInterruptFlags(AURA_INTERRUPT_FLAG_NOT_UNDERWATER);
        _lastLiquid = NULL;
        return;
    }

    if (uint32 liqEntry = liquid_status.entry)
    {
        LiquidTypeEntry const* liquid = sLiquidTypeStore.LookupEntry(liqEntry);
        if (_lastLiquid && _lastLiquid->SpellId && _lastLiquid->Id != liqEntry)
            RemoveAurasDueToSpell(_lastLiquid->SpellId);

        if (liquid && liquid->SpellId)
        {
            if (res & (LIQUID_MAP_UNDER_WATER | LIQUID_MAP_IN_WATER))
                CastSpell(this, liquid->SpellId, true);
            else
                RemoveAurasDueToSpell(liquid->SpellId);
        }

        RemoveAurasWithInterruptFlags(AURA_INTERRUPT_FLAG_NOT_ABOVEWATER);
        _lastLiquid = liquid;
    }
    else if (_lastLiquid && _lastLiquid->SpellId)
    {
        RemoveAurasDueToSpell(_lastLiquid->SpellId);
        RemoveAurasWithInterruptFlags(AURA_INTERRUPT_FLAG_NOT_UNDERWATER);
        _lastLiquid = NULL;
    }
}

void Unit::DeMorph()
{
    SetDisplayId(GetNativeDisplayId());
}

Aura* Unit::_TryStackingOrRefreshingExistingAura(SpellInfo const* newAura, uint8 effMask, Unit* caster, int32* baseAmount /*= NULL*/, Item* castItem /*= NULL*/, uint64 casterGUID /*= 0*/)
{
    ASSERT(casterGUID || caster);
    if (!casterGUID)
        casterGUID = caster->GetGUID();

    // Special handling for Sunder Armor
    if (newAura->Id == 58567)
    {
        if (Aura * aura = GetAura(58567))
        {
            aura->ModStackAmount(1);
            return aura;
        }
    }
    
    // passive and Incanter's Absorption and auras with different type can stack with themselves any number of times
    if (!newAura->IsMultiSlotAura())
    {
        // check if cast item changed
        uint64 castItemGUID = 0;
        if (castItem)
            castItemGUID = castItem->GetGUID();

        // find current aura from spell and change it's stackamount, or refresh it's duration
        if (Aura* foundAura = GetOwnedAura(newAura->Id, casterGUID, (newAura->AttributesCu & SPELL_ATTR0_CU_ENCHANT_STACK) ? castItemGUID : 0, 0))
        {
            // effect masks do not match
            // extremely rare case
            // let's just recreate aura
            if (effMask != foundAura->GetEffectMask())
                return NULL;

            if (foundAura->GetId() == 44614 && !caster->HasAura(61205))
                return NULL;

            // update basepoints with new values - effect amount will be recalculated in ModStackAmount
            for (uint8 i = 0; i < MAX_SPELL_EFFECTS; ++i)
            {
                if (!foundAura->HasEffect(i))
                    continue;

                int bp;
                if (baseAmount)
                    bp = *(baseAmount + i);
                else
                    bp = foundAura->GetSpellInfo()->Effects[i].BasePoints;

                int32* oldBP = const_cast<int32*>(&(foundAura->GetEffect(i)->m_baseAmount));
                *oldBP = bp;
            }

            // correct cast item guid if needed
            if (castItemGUID != foundAura->GetCastItemGUID())
            {
                uint64* oldGUID = const_cast<uint64 *>(&foundAura->m_castItemGuid);
                *oldGUID = castItemGUID;
            }

            // try to increase stack amount
            foundAura->ModStackAmount(1);
            return foundAura;
        }
    }

    return NULL;
}

void Unit::_AddAura(UnitAura* aura, Unit* caster)
{
    ASSERT(!m_cleanupDone);

    // temporary hack: dynamic auras should be updated from dynobject::update
    if (!aura->GetSpellInfo()->HasPersistenAura())
        m_ownedAuras.insert(AuraMap::value_type(aura->GetId(), aura));

    _RemoveNoStackAurasDueToAura(aura);

    if (aura->IsRemoved())
        return;

    aura->SetIsSingleTarget(caster && aura->GetSpellInfo()->IsSingleTarget(caster));
    if (aura->IsSingleTarget())
    {
        ASSERT((IsInWorld() && !IsDuringRemoveFromWorld()) || (aura->GetCasterGUID() == GetGUID()));
        // register single target aura
        //caster->GetSingleCastAuras().push_back(aura);
        for (AuraIdList::iterator itr = caster->GetSingleCastAuras().begin(); itr != caster->GetSingleCastAuras().end(); ++itr)
        {
            if ((*itr).id == aura->GetId())
            if (Unit* pTarget = ObjectAccessor::FindUnit((*itr).guid))
            {
                pTarget->RemoveAura((*itr).id, caster->GetGUID());
                break;
            }
        }
        caster->GetSingleCastAuras().push_back(SingleCastAura(aura->GetId(), GetGUID()));
        // remove other single target auras
       /* Unit::AuraList tempList;
        Unit::AuraList& scAuras = caster->GetSingleCastAuras();
        for (AuraList::const_iterator itr = scAuras.begin(); itr != scAuras.end(); ++itr)
        {
            if ((*itr) != aura && (*itr)->GetSpellInfo() &&
                (*itr)->GetSpellInfo()->IsSingleTargetWith(aura->GetSpellInfo()))
                    tempList.push_back((*itr));
        }

        if (!tempList.empty())
        {
            for (AuraList::const_iterator t_itr = tempList.begin(); t_itr != tempList.end(); ++t_itr)
                if (Aura* aura = *t_itr)
                    aura->Remove();
        } 
        */
    }
}

// creates aura application instance and registers it in lists
// aura application effects are handled separately to prevent aura list corruption
AuraApplication * Unit::_CreateAuraApplication(Aura* aura, uint8 effMask)
{
    // can't apply aura on unit which is going to be deleted - to not create a memory leak
    ASSERT(!m_cleanupDone);
    // aura musn't be removed
    ASSERT(!aura->IsRemoved());

    // aura mustn't be already applied on target
    ASSERT (!aura->IsAppliedOnTarget(GetGUID()) && "Unit::_CreateAuraApplication: aura musn't be applied on target");

    SpellInfo const* aurSpellInfo = aura->GetSpellInfo();
    uint32 aurId = aurSpellInfo->Id;

    // ghost spell check, allow apply any auras at player loading in ghost mode (will be cleanup after load)
    if (!isAlive() && !aurSpellInfo->IsDeathPersistent() &&
        (GetTypeId() != TYPEID_PLAYER || !ToPlayer()->GetSession()->PlayerLoading()))
        return NULL;

    Unit* caster = aura->GetCaster();

    AuraApplication * aurApp = new AuraApplication(this, caster, aura, effMask);
    m_appliedAuras.insert(AuraApplicationMap::value_type(aurId, aurApp));

    if (aurSpellInfo->AuraInterruptFlags)
    {
        m_interruptableAuras.push_back(aurApp);
        AddInterruptMask(aurSpellInfo->AuraInterruptFlags);
    }

    if (AuraStateType aState = aura->GetSpellInfo()->GetAuraState())
        m_auraStateAuras.insert(AuraStateAurasMap::value_type(aState, aurApp));

    aura->_ApplyForTarget(this, caster, aurApp);
    return aurApp;
}

void Unit::_ApplyAuraEffect(Aura* aura, uint8 effIndex)
{
    ASSERT(aura);
    ASSERT(aura->HasEffect(effIndex));
    AuraApplication * aurApp = aura->GetApplicationOfTarget(GetGUID());
    ASSERT(aurApp);
    if (!aurApp->GetEffectMask())
        _ApplyAura(aurApp, 1<<effIndex);
    else
        aurApp->_HandleEffect(effIndex, true);
}

// handles effects of aura application
// should be done after registering aura in lists
void Unit::_ApplyAura(AuraApplication * aurApp, uint8 effMask)
{
    Aura* aura = aurApp->GetBase();

    _RemoveNoStackAurasDueToAura(aura);

    if (aurApp->GetRemoveMode())
        return;

    // Update target aura state flag
    if (AuraStateType aState = aura->GetSpellInfo()->GetAuraState())
        ModifyAuraState(aState, true);

    if (aurApp->GetRemoveMode())
        return;

    // Sitdown on apply aura req seated
    if (aura->GetSpellInfo()->AuraInterruptFlags & AURA_INTERRUPT_FLAG_NOT_SEATED && !IsSitState())
        SetStandState(UNIT_STAND_STATE_SIT);

    Unit* caster = aura->GetCaster();

    if (aurApp->GetRemoveMode())
        return;

    aura->HandleAuraSpecificMods(aurApp, caster, true, false);

    // apply effects of the aura
    for (uint8 i = 0; i < MAX_SPELL_EFFECTS; ++i)
    {
        if (effMask & 1<<i && (!aurApp->GetRemoveMode()))
            aurApp->_HandleEffect(i, true);
    }
}

// removes aura application from lists and unapplies effects
void Unit::_UnapplyAura(AuraApplicationMap::iterator &i, AuraRemoveMode removeMode)
{
    AuraApplication * aurApp = i->second;
    ASSERT(aurApp);
    ASSERT(!aurApp->GetRemoveMode());
    ASSERT(aurApp->GetTarget() == this);

    aurApp->SetRemoveMode(removeMode);
    Aura* aura = aurApp->GetBase();

    // dead loop is killing the server probably
    ASSERT(m_removedAurasCount < 0xFFFFFFFF);

    ++m_removedAurasCount;

    Unit* caster = aura->GetCaster();

    // Remove all pointers from lists here to prevent possible pointer invalidation on spellcast/auraapply/auraremove
    m_appliedAuras.erase(i);

    if (aura->GetSpellInfo()->AuraInterruptFlags)
    {
        m_interruptableAuras.remove(aurApp);
        UpdateInterruptMask();
    }

    bool auraStateFound = false;
    AuraStateType auraState = aura->GetSpellInfo()->GetAuraState();
    if (auraState)
    {
        bool canBreak = false;
        // Get mask of all aurastates from remaining auras
        for (AuraStateAurasMap::iterator itr = m_auraStateAuras.lower_bound(auraState); itr != m_auraStateAuras.upper_bound(auraState) && !(auraStateFound && canBreak);)
        {
            if (itr->second == aurApp)
            {
                m_auraStateAuras.erase(itr);
                itr = m_auraStateAuras.lower_bound(auraState);
                canBreak = true;
                continue;
            }
            auraStateFound = true;
            ++itr;
        }
    }

    aurApp->_Remove();
    aura->_UnapplyForTarget(this, caster, aurApp);

    // remove effects of the spell - needs to be done after removing aura from lists
    for (uint8 itr = 0; itr < MAX_SPELL_EFFECTS; ++itr)
    {
        if (aurApp->HasEffect(itr))
            aurApp->_HandleEffect(itr, false);
    }

    // all effect mustn't be applied
    ASSERT(!aurApp->GetEffectMask());

    // Remove aurastates only if were not found
    if (!auraStateFound)
        ModifyAuraState(auraState, false);

    aura->HandleAuraSpecificMods(aurApp, caster, false, false);

    // only way correctly remove all auras from list
    //if (removedAuras != m_removedAurasCount) new aura may be added
        i = m_appliedAuras.begin();
}

void Unit::_UnapplyAura(AuraApplication * aurApp, AuraRemoveMode removeMode)
{
    // aura can be removed from unit only if it's applied on it, shouldn't happen
    ASSERT(aurApp->GetBase()->GetApplicationOfTarget(GetGUID()) == aurApp);
    uint32 spellId = aurApp->GetBase()->GetId();

    AuraApplicationMapBoundsNonConst range = m_appliedAuras.equal_range(spellId);
    for (AuraApplicationMap::iterator iter = range.first; iter != range.second;)
    {
        if (iter->second == aurApp)
        {
            _UnapplyAura(iter, removeMode);
            return;
        }
        else
            ++iter;
    }
    ASSERT(false);
}

void Unit::_RemoveNoStackAurasDueToAura(Aura* aura)
{
    SpellInfo const* spellProto = aura->GetSpellInfo();

    // passive spell special case (only non stackable with ranks)
    if (spellProto->IsPassiveStackableWithRanks())
        return;

    bool remove = false;
    for (AuraApplicationMap::iterator i = m_appliedAuras.begin(); i != m_appliedAuras.end(); ++i)
    {
        if (remove)
        {
            remove = false;
            i = m_appliedAuras.begin();
        }

        if (aura->CanStackWith(i->second->GetBase()))
            continue;

        RemoveAura(i, AURA_REMOVE_BY_DEFAULT);
        if (i == m_appliedAuras.end())
            break;
        remove = true;
    }
}

void Unit::_RegisterAuraEffect(AuraEffect* aurEff, bool apply)
{
    if (apply)
        m_modAuras[aurEff->GetAuraType()].push_back(aurEff);
    else
        m_modAuras[aurEff->GetAuraType()].remove(aurEff);
}

// All aura base removes should go threw this function!
void Unit::RemoveOwnedAura(AuraMap::iterator &i, AuraRemoveMode removeMode)
{
    Aura* aura = i->second;
    ASSERT(!aura->IsRemoved());

    // if unit currently update aura list then make safe update iterator shift to next
    if (m_auraUpdateIterator == i)
        ++m_auraUpdateIterator;

    m_ownedAuras.erase(i);
    m_removedAuras.push_back(aura);

    // Unregister single target aura
    if (aura->IsSingleTarget())
        aura->UnregisterSingleTarget();

    aura->_Remove(removeMode);

    i = m_ownedAuras.begin();
}

void Unit::RemoveOwnedAura(uint32 spellId, uint64 casterGUID, uint8 reqEffMask, AuraRemoveMode removeMode)
{
    for (AuraMap::iterator itr = m_ownedAuras.lower_bound(spellId); itr != m_ownedAuras.upper_bound(spellId);)
        if (((itr->second->GetEffectMask() & reqEffMask) == reqEffMask) && (!casterGUID || itr->second->GetCasterGUID() == casterGUID))
        {
            RemoveOwnedAura(itr, removeMode);
            itr = m_ownedAuras.lower_bound(spellId);
        }
        else
            ++itr;
}

void Unit::RemoveOwnedAura(Aura* aura, AuraRemoveMode removeMode)
{
    if (aura->IsRemoved())
        return;

    ASSERT(aura->GetOwner() == this);

    uint32 spellId = aura->GetId();
    if (aura->GetSpellInfo()->HasPersistenAura())
        return;

    AuraMapBoundsNonConst range = m_ownedAuras.equal_range(spellId);

    for (AuraMap::iterator itr = range.first; itr != range.second; ++itr)
    {
        if (itr->second == aura)
        {
            RemoveOwnedAura(itr, removeMode);
            return;
        }
    }
    ASSERT(false);
}

Aura* Unit::GetOwnedAura(uint32 spellId, uint64 casterGUID, uint64 itemCasterGUID, uint8 reqEffMask, Aura* except) const
{
    AuraMapBounds range = m_ownedAuras.equal_range(spellId);
    for (AuraMap::const_iterator itr = range.first; itr != range.second; ++itr)
    {
        if (((itr->second->GetEffectMask() & reqEffMask) == reqEffMask)
                && (!casterGUID || itr->second->GetSpellInfo()->HasCustomAttribute(SPELL_ATTR0_CU_CAN_STACK_FROM_DIFF_CASTERS) || itr->second->GetCasterGUID() == casterGUID)
                && (!itemCasterGUID || itr->second->GetCastItemGUID() == itemCasterGUID)
                && (!except || except != itr->second))
        {
            return itr->second;
        }
    }
    return NULL;
}

void Unit::RemoveAura(AuraApplicationMap::iterator &i, AuraRemoveMode mode)
{
    AuraApplication * aurApp = i->second;
    // Do not remove aura which is already being removed
    if (aurApp->GetRemoveMode())
        return;
    Aura* aura = aurApp->GetBase();
    _UnapplyAura(i, mode);
    // Remove aura - for Area and Target auras
    if (aura->GetOwner() == this)
        aura->Remove(mode);
}

void Unit::RemoveAura(uint32 spellId, uint64 caster, uint8 reqEffMask, AuraRemoveMode removeMode)
{
    AuraApplicationMapBoundsNonConst range = m_appliedAuras.equal_range(spellId);
    for (AuraApplicationMap::iterator iter = range.first; iter != range.second;)
    {
        Aura const* aura = iter->second->GetBase();
        if (((aura->GetEffectMask() & reqEffMask) == reqEffMask)
            && (!caster || aura->GetCasterGUID() == caster))
        {
            RemoveAura(iter, removeMode);
            return;
        }
        else
            ++iter;
    }
}

void Unit::RemoveAura(AuraApplication * aurApp, AuraRemoveMode mode)
{
    // we've special situation here, RemoveAura called while during aura removal
    // this kind of call is needed only when aura effect removal handler
    // or event triggered by it expects to remove
    // not yet removed effects of an aura
    if (aurApp->GetRemoveMode())
    {
        // remove remaining effects of an aura
        for (uint8 itr = 0; itr < MAX_SPELL_EFFECTS; ++itr)
        {
            if (aurApp->HasEffect(itr))
                aurApp->_HandleEffect(itr, false);
        }
        return;
    }
    // no need to remove
    if (aurApp->GetBase()->GetApplicationOfTarget(GetGUID()) != aurApp || aurApp->GetBase()->IsRemoved())
        return;
    uint32 spellId = aurApp->GetBase()->GetId();
    AuraApplicationMapBoundsNonConst range = m_appliedAuras.equal_range(spellId);
    for (AuraApplicationMap::iterator iter = range.first; iter != range.second;)
    {
        if (aurApp == iter->second)
        {
            RemoveAura(iter, mode);
            return;
        }
        else
            ++iter;
    }
}

void Unit::RemoveAura(Aura* aura, AuraRemoveMode mode)
{
    if (aura->IsRemoved())
        return;
    if (AuraApplication * aurApp = aura->GetApplicationOfTarget(GetGUID()))
        RemoveAura(aurApp, mode);
}

void Unit::RemoveAurasDueToSpell(uint32 spellId, uint64 casterGUID, uint8 reqEffMask, AuraRemoveMode removeMode)
{
    for (AuraApplicationMap::iterator iter = m_appliedAuras.lower_bound(spellId); iter != m_appliedAuras.upper_bound(spellId);)
    {
        Aura const* aura = iter->second->GetBase();
        if (((aura->GetEffectMask() & reqEffMask) == reqEffMask)
            && (!casterGUID || aura->GetCasterGUID() == casterGUID))
        {
            RemoveAura(iter, removeMode);
            iter = m_appliedAuras.lower_bound(spellId);
        }
        else
            ++iter;
    }
}

void Unit::RemoveAuraFromStack(uint32 spellId, uint64 casterGUID, AuraRemoveMode removeMode, int32 num)
{
    AuraMapBoundsNonConst range = m_ownedAuras.equal_range(spellId);
    for (AuraMap::iterator iter = range.first; iter != range.second;)
    {
        Aura* aura = iter->second;
        if ((aura->GetType() == UNIT_AURA_TYPE)
            && (!casterGUID || aura->GetCasterGUID() == casterGUID))
        {
            aura->ModStackAmount(-num, removeMode);
            return;
        }
        else
            ++iter;
    }
}

void Unit::RemoveAurasDueToSpellByDispel(uint32 spellId, uint32 dispellerSpellId, uint64 casterGUID, Unit* dispeller, uint8 chargesRemoved/*= 1*/)
{
    AuraMapBoundsNonConst range = m_ownedAuras.equal_range(spellId);
    for (AuraMap::iterator iter = range.first; iter != range.second;)
    {
        Aura* aura = iter->second;
        if (aura->GetCasterGUID() == casterGUID)
        {
            DispelInfo dispelInfo(dispeller, dispellerSpellId, chargesRemoved);

            // Call OnDispel hook on AuraScript
            aura->CallScriptDispel(&dispelInfo);

            if (aura->GetSpellInfo()->AttributesEx7 & SPELL_ATTR7_DISPEL_CHARGES)
                aura->ModCharges(-dispelInfo.GetRemovedCharges(), AURA_REMOVE_BY_ENEMY_SPELL);
            else
                aura->ModStackAmount(-dispelInfo.GetRemovedCharges(), AURA_REMOVE_BY_ENEMY_SPELL);

            // Call AfterDispel hook on AuraScript
            aura->CallScriptAfterDispel(&dispelInfo);

            return;
        }
        else
            ++iter;
    }
}

void Unit::RemoveAurasDueToSpellBySteal(uint32 spellId, uint64 casterGUID, Unit* stealer)
{
    AuraMapBoundsNonConst range = m_ownedAuras.equal_range(spellId);
    for (AuraMap::iterator iter = range.first; iter != range.second;)
    {
        Aura* aura = iter->second;
        if (aura->GetCasterGUID() == casterGUID)
        {
            int32 damage[MAX_SPELL_EFFECTS];
            int32 baseDamage[MAX_SPELL_EFFECTS];
            uint8 effMask = 0;
            uint8 recalculateMask = 0;
            Unit* caster = aura->GetCaster();
            for (uint8 i = 0; i < MAX_SPELL_EFFECTS; ++i)
            {
                if (aura->GetEffect(i))
                {
                    baseDamage[i] = aura->GetEffect(i)->GetBaseAmount();
                    damage[i] = aura->GetEffect(i)->GetAmount();
                    effMask |= (1<<i);
                    if (aura->GetEffect(i)->CanBeRecalculated())
                        recalculateMask |= (1<<i);
                }
                else
                {
                    baseDamage[i] = 0;
                    damage[i] = 0;
                }
            }

            bool stealCharge = aura->GetSpellInfo()->AttributesEx7 & SPELL_ATTR7_DISPEL_CHARGES;
            // Cast duration to unsigned to prevent permanent aura's such as Righteous Fury being permanently added to caster
            uint32 dur = std::min(2u * MINUTE * IN_MILLISECONDS, uint32(aura->GetDuration()));

            if (Aura* oldAura = stealer->GetAura(aura->GetId(), aura->GetCasterGUID()))
            {
                if (stealCharge)
                    oldAura->ModCharges(1);
                else
                    oldAura->ModStackAmount(1);
                oldAura->SetDuration(int32(dur));
            }
            else
            {
                // single target state must be removed before aura creation to preserve existing single target aura
                if (aura->IsSingleTarget())
                    aura->UnregisterSingleTarget();

                if (Aura* newAura = Aura::TryRefreshStackOrCreate(aura->GetSpellInfo(), effMask, stealer, NULL, &baseDamage[0], NULL, aura->GetCasterGUID()))
                {
                    // created aura must not be single target aura,, so stealer won't loose it on recast
                    if (newAura->IsSingleTarget())
                    {
                        newAura->UnregisterSingleTarget();
                        // bring back single target aura status to the old aura
                        aura->SetIsSingleTarget(true);
                        //caster->GetSingleCastAuras().push_back(aura);
                        caster->GetSingleCastAuras().push_back(SingleCastAura(aura->GetId(), stealer->GetGUID()));
                    }
                    // FIXME: using aura->GetMaxDuration() maybe not blizzlike but it fixes stealing of spells like Innervate
                    newAura->SetLoadedState(aura->GetMaxDuration(), int32(dur), stealCharge ? 1 : aura->GetCharges(), 1, recalculateMask, &damage[0]);
                    newAura->ApplyForTargets();
                }
            }

            if (stealCharge)
                aura->ModCharges(-1, AURA_REMOVE_BY_ENEMY_SPELL);
            else
                aura->ModStackAmount(-1, AURA_REMOVE_BY_ENEMY_SPELL);

            return;
        }
        else
            ++iter;
    }
}

void Unit::RemoveAurasDueToItemSpell(uint32 spellId, uint64 castItemGuid)
{
    for (AuraApplicationMap::iterator iter = m_appliedAuras.lower_bound(spellId); iter != m_appliedAuras.upper_bound(spellId);)
    {
        if (iter->second->GetBase()->GetCastItemGUID() == castItemGuid)
        {
            RemoveAura(iter);
            iter = m_appliedAuras.lower_bound(spellId);
        }
        else
            ++iter;
    }
}

void Unit::RemoveAurasByType(AuraType auraType, uint64 casterGUID, Aura* exceptAura, uint32 exceptAuraId, bool negative, bool positive)
{
    for (AuraEffectList::iterator iter = m_modAuras[auraType].begin(); iter != m_modAuras[auraType].end();)
    {
        Aura* aura = (*iter)->GetBase();
        AuraApplication * aurApp = aura->GetApplicationOfTarget(GetGUID());

        ++iter;
        if (aura != exceptAura && (!exceptAuraId || aura->GetId() != exceptAuraId) &&
            (!casterGUID || aura->GetCasterGUID() == casterGUID) &&
            ((negative && !aurApp->IsPositive()) || (positive && aurApp->IsPositive())))
        {
            uint32 removedAuras = m_removedAurasCount;
            RemoveAura(aurApp);
            if (m_removedAurasCount > removedAuras + 1)
                iter = m_modAuras[auraType].begin();
        }
    }
}

void Unit::RemoveDotsNotSWD()
{
    RemoveAurasByType(SPELL_AURA_PERIODIC_DAMAGE, 0, 0, 32409);         // SW:D shall not be removed.
    RemoveAurasByType(SPELL_AURA_PERIODIC_DAMAGE_PERCENT, 0, 0, 49016); // Unholy Frenzy is except too
    RemoveAurasByType(SPELL_AURA_PERIODIC_LEECH);    
}

void Unit::RemoveAurasWithAttribute(uint32 flags)
{
    for (AuraApplicationMap::iterator iter = m_appliedAuras.begin(); iter != m_appliedAuras.end();)
    {
        SpellInfo const* spell = iter->second->GetBase()->GetSpellInfo();
        if (spell->Attributes & flags)
            RemoveAura(iter);
        else
            ++iter;
    }
}

void Unit::RemoveNotOwnSingleTargetAuras(uint32 newPhase)
{
    // single target auras from other casters
    for (AuraApplicationMap::iterator iter = m_appliedAuras.begin(); iter != m_appliedAuras.end();)
    {
        AuraApplication const* aurApp = iter->second;
        Aura const* aura = aurApp->GetBase();

        if (aura->GetCasterGUID() != GetGUID() && aura->GetSpellInfo()->IsSingleTarget(aura->GetCaster()))
        {
            if (!newPhase)
                RemoveAura(iter);
            else
            {
                Unit* caster = aura->GetCaster();
                if (!caster || !caster->InSamePhase(newPhase))
                    RemoveAura(iter);
                else
                    ++iter;
            }
        }
        else
            ++iter;
    }

    AuraIdList& scAuras = GetSingleCastAuras();
    for (AuraIdList::iterator itr = scAuras.begin(); itr != scAuras.end(); ++itr)
    {
        if (Unit* pTarget = ObjectAccessor::FindUnit((*itr).guid))
        {
            if ((*itr).guid != GetGUID() && !pTarget->InSamePhase(newPhase))
            {
                pTarget->RemoveAura((*itr).id, GetGUID());
                break;
            }
        }                
    }

    // single target auras at other targets
    /*AuraList tempList;
    AuraList& scAuras = GetSingleCastAuras();
    for (AuraList::const_iterator itr = scAuras.begin(); itr != scAuras.end(); ++itr)
        if (Aura* aura = *itr)
            if (aura->GetUnitOwner() != this && !aura->GetUnitOwner()->InSamePhase(newPhase) && !aura->IsRemoved())
                tempList.push_back(aura);

    if (!tempList.empty())
        for (AuraList::const_iterator itr = tempList.begin(); itr != tempList.end(); ++itr)
            if (Aura* aura = *itr)
                aura->Remove();*/


    /*for (AuraList::iterator iter = scAuras.begin(); iter != scAuras.end();)
    {
        if (Aura* aura = *iter)
        {
            if (aura->GetUnitOwner() != this && !aura->GetUnitOwner()->InSamePhase(newPhase))
            {
                aura->Remove();
                iter = scAuras.begin();
            }
            else
                ++iter;
        }
        else
            ++iter;
    }*/
}

void Unit::RemoveAurasWithInterruptFlags(uint32 flag, uint32 except)
{
    if (!(m_interruptMask & flag))
        return;

    // interrupt auras
    for (AuraApplicationList::iterator iter = m_interruptableAuras.begin(); iter != m_interruptableAuras.end();)
    {
        Aura* aura = (*iter)->GetBase();
        ++iter;
        if ((aura->GetSpellInfo()->AuraInterruptFlags & flag) && (!except || aura->GetId() != except))
        {
            uint32 removedAuras = m_removedAurasCount;
            RemoveAura(aura);
            if (m_removedAurasCount > removedAuras + 1)
                iter = m_interruptableAuras.begin();
        }
    }

    // interrupt channeled spell
    if (Spell* spell = m_currentSpells[CURRENT_CHANNELED_SPELL])
        if (spell->getState() == SPELL_STATE_CASTING
            && (spell->m_spellInfo->ChannelInterruptFlags & flag)
            && spell->m_spellInfo->Id != except
            && !((flag & AURA_INTERRUPT_FLAG_MOVE) && HasAuraTypeWithAffectMask(SPELL_AURA_CAST_WHILE_WALKING, spell->m_spellInfo)))
            InterruptNonMeleeSpells(false);

    UpdateInterruptMask();
}

void Unit::RemoveAurasWithFamily(SpellFamilyNames family, uint32 familyFlag1, uint32 familyFlag2, uint32 familyFlag3, uint64 casterGUID)
{
    for (AuraApplicationMap::iterator iter = m_appliedAuras.begin(); iter != m_appliedAuras.end();)
    {
        Aura const* aura = iter->second->GetBase();
        if (!casterGUID || aura->GetCasterGUID() == casterGUID)
        {
            SpellInfo const* spell = aura->GetSpellInfo();
            if (spell->SpellFamilyName == uint32(family) && spell->SpellFamilyFlags.HasFlag(familyFlag1, familyFlag2, familyFlag3))
            {
                RemoveAura(iter);
                continue;
            }
        }
        ++iter;
    }
}

void Unit::RemoveMovementImpairingAuras()
{
    RemoveAurasWithMechanic((1<<MECHANIC_SNARE)|(1<<MECHANIC_ROOT));
}

void Unit::RemoveAurasWithMechanic(uint32 mechanic_mask, AuraRemoveMode removemode, uint32 except, uint8 count)
{
    for (AuraApplicationMap::iterator iter = m_appliedAuras.begin(); iter != m_appliedAuras.end();)
    {
        uint8 aurasCount = 0;
        Aura const* aura = iter->second->GetBase();
        if (!except || aura->GetId() != except)
        {
            if (aura->GetSpellInfo()->GetAllEffectsMechanicMask() & mechanic_mask)
            {
                RemoveAura(iter, removemode);
                aurasCount++;
                if (count && aurasCount == count)
                    break;

                continue;
            }
        }
        ++iter;
    }
}

void Unit::RemoveAreaAurasDueToLeaveWorld()
{
    // make sure that all area auras not applied on self are removed - prevent access to deleted pointer later
    for (AuraMap::iterator iter = m_ownedAuras.begin(); iter != m_ownedAuras.end();)
    {
        Aura* aura = iter->second;
        ++iter;
        Aura::ApplicationMap const& appMap = aura->GetApplicationMap();
        for (Aura::ApplicationMap::const_iterator itr = appMap.begin(); itr!= appMap.end();)
        {
            AuraApplication * aurApp = itr->second;
            ++itr;
            Unit* target = aurApp->GetTarget();
            if (target == this)
                continue;
            target->RemoveAura(aurApp);
            // things linked on aura remove may apply new area aura - so start from the beginning
            iter = m_ownedAuras.begin();
        }
    }

    // remove area auras owned by others
    for (AuraApplicationMap::iterator iter = m_appliedAuras.begin(); iter != m_appliedAuras.end();)
    {
        if (iter->second->GetBase()->GetOwner() != this)
        {
            RemoveAura(iter);
        }
        else
            ++iter;
    }
}

void Unit::RemoveAllAuras()
{
    // this may be a dead loop if some events on aura remove will continiously apply aura on remove
    // we want to have all auras removed, so use your brain when linking events
    while (!m_appliedAuras.empty() || !m_ownedAuras.empty())
    {
        AuraApplicationMap::iterator aurAppIter;
        for (aurAppIter = m_appliedAuras.begin(); aurAppIter != m_appliedAuras.end();)
            _UnapplyAura(aurAppIter, AURA_REMOVE_BY_DEFAULT);

        AuraMap::iterator aurIter;
        for (aurIter = m_ownedAuras.begin(); aurIter != m_ownedAuras.end();)
            RemoveOwnedAura(aurIter);
    }
}

void Unit::RemoveArenaAuras()
{
    CleanupSwapAuras(77616); // Dark Simulacrum
    RemoveAura(77616);
    CleanupSwapAuras(77769); // Trap Launcher
    RemoveAura(77769);
    CleanupSwapAuras(81021); // Stampede
    RemoveAura(81021);
    CleanupSwapAuras(82926); // Fire
    RemoveAura(82926);
    CleanupSwapAuras(82946); // Trap Launcher
    RemoveAura(82946);
    CleanupSwapAuras(88688); // Surge of Light
    RemoveAura(88688);

    // in join, remove positive buffs, on end, remove negative
    // used to remove positive visible auras in arenas
    for (AuraApplicationMap::iterator iter = m_appliedAuras.begin(); iter != m_appliedAuras.end();)
    {
        AuraApplication const* aurApp = iter->second;
        Aura const* aura = aurApp->GetBase();
        if (!(aura->GetSpellInfo()->AttributesEx4 & SPELL_ATTR4_UNK21)  // don't remove stances, shadowform, pally/hunter auras
            && !aura->IsPassive()                                       // don't remove passive auras
            && !aura->HasEffectType(SPELL_AURA_MOD_INVISIBILITY)        // don't remove Invisibility
            && (aurApp->IsPositive() || !(aura->GetSpellInfo()->AttributesEx3 & SPELL_ATTR3_DEATH_PERSISTENT))) // not negative death persistent auras
            RemoveAura(iter);
        else
            ++iter;
    }
}

void Unit::CleanupSwapAuras(uint32 spellId)
{
    if (!this || !this->ToPlayer())
        return;

    Player* swapOwner = this->ToPlayer();
    swapOwner->RemoveSpellSwap(spellId);
}

void Unit::RemoveNegativeAuras()
{
    for (AuraApplicationMap::iterator iter = m_appliedAuras.begin(); iter != m_appliedAuras.end();)
    {
        AuraApplication const* aurApp = iter->second;
        Aura const* aura = aurApp->GetBase();
        if (!aurApp->IsPositive()                                       // don't remove stances, shadowform, pally/hunter auras
            && !aura->IsPassive()                                       // don't remove passive auras
            && !aura->IsDeathPersistent())                              // don't remove death persistent auras
            RemoveAura(iter);
        else
            ++iter;
    }
}

void Unit::RemoveAllAurasOnDeath()
{
    // used just after dieing to remove all visible auras
    // and disable the mods for the passive ones
    for (AuraApplicationMap::iterator iter = m_appliedAuras.begin(); iter != m_appliedAuras.end();)
    {
        Aura const* aura = iter->second->GetBase();
        if (!aura->IsPassive() && !aura->IsDeathPersistent())
            _UnapplyAura(iter, AURA_REMOVE_BY_DEATH);
        else
            ++iter;
    }

    for (AuraMap::iterator iter = m_ownedAuras.begin(); iter != m_ownedAuras.end();)
    {
        Aura* aura = iter->second;
        if (!aura->IsPassive() && !aura->IsDeathPersistent())
            RemoveOwnedAura(iter, AURA_REMOVE_BY_DEATH);
        else
            ++iter;
    }
}

void Unit::RemoveAllAurasRequiringDeadTarget()
{
    for (AuraApplicationMap::iterator iter = m_appliedAuras.begin(); iter != m_appliedAuras.end();)
    {
        Aura const* aura = iter->second->GetBase();
        if (!aura->IsPassive() && aura->GetSpellInfo()->IsRequiringDeadTarget())
            _UnapplyAura(iter, AURA_REMOVE_BY_DEFAULT);
        else
            ++iter;
    }

    for (AuraMap::iterator iter = m_ownedAuras.begin(); iter != m_ownedAuras.end();)
    {
        Aura* aura = iter->second;
        if (!aura->IsPassive() && aura->GetSpellInfo()->IsRequiringDeadTarget())
            RemoveOwnedAura(iter, AURA_REMOVE_BY_DEFAULT);
        else
            ++iter;
    }
}

void Unit::RemoveAllAurasExceptType(AuraType type)
{
    for (AuraApplicationMap::iterator iter = m_appliedAuras.begin(); iter != m_appliedAuras.end();)
    {
        Aura const* aura = iter->second->GetBase();
        if (!aura->GetSpellInfo()->HasAura(type))
            _UnapplyAura(iter, AURA_REMOVE_BY_DEFAULT);
        else
            ++iter;
    }

    for (AuraMap::iterator iter = m_ownedAuras.begin(); iter != m_ownedAuras.end();)
    {
        Aura* aura = iter->second;
        if (!aura->GetSpellInfo()->HasAura(type))
            RemoveOwnedAura(iter, AURA_REMOVE_BY_DEFAULT);
        else
            ++iter;
    }
}

void Unit::DelayOwnedAuras(uint32 spellId, uint64 caster, int32 delaytime)
{
    AuraMapBoundsNonConst range = m_ownedAuras.equal_range(spellId);
    for (; range.first != range.second; ++range.first)
    {
        Aura* aura = range.first->second;
        if (!caster || aura->GetCasterGUID() == caster)
        {
            if (aura->GetDuration() < delaytime)
                aura->SetDuration(0);
            else
                aura->SetDuration(aura->GetDuration() - delaytime);

            // update for out of range group members (on 1 slot use)
            aura->SetNeedClientUpdateForTargets();
        }
    }
}

void Unit::_RemoveAllAuraStatMods()
{
    for (AuraApplicationMap::iterator i = m_appliedAuras.begin(); i != m_appliedAuras.end(); ++i)
        (*i).second->GetBase()->HandleAllEffects(i->second, AURA_EFFECT_HANDLE_STAT, false);
}

void Unit::_ApplyAllAuraStatMods()
{
    for (AuraApplicationMap::iterator i = m_appliedAuras.begin(); i != m_appliedAuras.end(); ++i)
        (*i).second->GetBase()->HandleAllEffects(i->second, AURA_EFFECT_HANDLE_STAT, true);
}

AuraEffect* Unit::GetAuraEffect(uint32 spellId, uint8 effIndex, uint64 caster) const
{
    AuraApplicationMapBounds range = m_appliedAuras.equal_range(spellId);
    for (AuraApplicationMap::const_iterator itr = range.first; itr != range.second; ++itr)
    {
        if (itr->second->HasEffect(effIndex)
                && (!caster || itr->second->GetBase()->GetCasterGUID() == caster))
        {
            return itr->second->GetBase()->GetEffect(effIndex);
        }
    }
    return NULL;
}

AuraEffect* Unit::GetAuraEffectOfRankedSpell(uint32 spellId, uint8 effIndex, uint64 caster) const
{
    uint32 rankSpell = sSpellMgr->GetFirstSpellInChain(spellId);
    while (rankSpell)
    {
        if (AuraEffect* aurEff = GetAuraEffect(rankSpell, effIndex, caster))
            return aurEff;
        rankSpell = sSpellMgr->GetNextSpellInChain(rankSpell);
    }
    return NULL;
}

AuraEffect* Unit::GetAuraEffect(AuraType type, SpellFamilyNames name, uint32 iconId, uint8 effIndex) const
{
    AuraEffectList const& auras = GetAuraEffectsByType(type);
    for (Unit::AuraEffectList::const_iterator itr = auras.begin(); itr != auras.end(); ++itr)
    {
        if (effIndex != (*itr)->GetEffIndex())
            continue;
        SpellInfo const* spell = (*itr)->GetSpellInfo();
        if (spell->SpellIconID == iconId && spell->SpellFamilyName == uint32(name) && !spell->SpellFamilyFlags)
            return *itr;
    }
    return NULL;
}

AuraEffect* Unit::GetAuraEffect(AuraType type, SpellFamilyNames family, uint32 familyFlag1, uint32 familyFlag2, uint32 familyFlag3, uint64 casterGUID)
{
    AuraEffectList const& auras = GetAuraEffectsByType(type);
    for (AuraEffectList::const_iterator i = auras.begin(); i != auras.end(); ++i)
    {
        SpellInfo const* spell = (*i)->GetSpellInfo();
        if (spell->SpellFamilyName == uint32(family) && spell->SpellFamilyFlags.HasFlag(familyFlag1, familyFlag2, familyFlag3))
        {
            if (casterGUID && (*i)->GetCasterGUID() != casterGUID)
                continue;
            return (*i);
        }
    }
    return NULL;
}

AuraEffect* Unit::GetFirstAuraEffectByType(AuraType type) const
{
    AuraEffectList const& auras = GetAuraEffectsByType(type);
    for (AuraEffectList::const_iterator i = auras.begin(); i != auras.end(); ++i)
    {
        return *i;
    }
    return NULL;
}

AuraApplication * Unit::GetAuraApplication(uint32 spellId, uint64 casterGUID, uint64 itemCasterGUID, uint8 reqEffMask, AuraApplication * except) const
{
    AuraApplicationMapBounds range = m_appliedAuras.equal_range(spellId);
    for (; range.first != range.second; ++range.first)
    {
        AuraApplication* app = range.first->second;
        Aura const* aura = app->GetBase();

        if (((aura->GetEffectMask() & reqEffMask) == reqEffMask)
                && (!casterGUID || aura->GetCasterGUID() == casterGUID)
                && (!itemCasterGUID || aura->GetCastItemGUID() == itemCasterGUID)
                && (!except || except != app))
        {
            return app;
        }
    }
    return NULL;
}

Aura* Unit::GetAura(uint32 spellId, uint64 casterGUID, uint64 itemCasterGUID, uint8 reqEffMask) const
{
    AuraApplication * aurApp = GetAuraApplication(spellId, casterGUID, itemCasterGUID, reqEffMask);
    return aurApp ? aurApp->GetBase() : NULL;
}

AuraApplication * Unit::GetAuraApplicationOfRankedSpell(uint32 spellId, uint64 casterGUID, uint64 itemCasterGUID, uint8 reqEffMask, AuraApplication* except) const
{
    uint32 rankSpell = sSpellMgr->GetFirstSpellInChain(spellId);
    while (rankSpell)
    {
        if (AuraApplication * aurApp = GetAuraApplication(rankSpell, casterGUID, itemCasterGUID, reqEffMask, except))
            return aurApp;
        rankSpell = sSpellMgr->GetNextSpellInChain(rankSpell);
    }
    return NULL;
}

Aura* Unit::GetAuraOfRankedSpell(uint32 spellId, uint64 casterGUID, uint64 itemCasterGUID, uint8 reqEffMask) const
{
    AuraApplication * aurApp = GetAuraApplicationOfRankedSpell(spellId, casterGUID, itemCasterGUID, reqEffMask);
    return aurApp ? aurApp->GetBase() : NULL;
}

void Unit::GetDispellableAuraList(Unit* caster, uint32 dispelMask, DispelChargesList& dispelList)
{
    // we should not be able to dispel diseases if the target is affected by unholy blight
    if (dispelMask & (1 << DISPEL_DISEASE) && HasAura(50536))
        dispelMask &= ~(1 << DISPEL_DISEASE);

    AuraMap const& auras = GetOwnedAuras();
    for (AuraMap::const_iterator itr = auras.begin(); itr != auras.end(); ++itr)
    {
        Aura* aura = itr->second;
        AuraApplication * aurApp = aura->GetApplicationOfTarget(GetGUID());
        if (!aurApp)
            continue;

        // don't try to remove passive auras
        if (aura->IsPassive())
            continue;

        if (aura->GetSpellInfo()->GetDispelMask() & dispelMask)
        {
            if (aura->GetSpellInfo()->Dispel == DISPEL_MAGIC)
            {
                // do not remove positive auras if friendly target
                //               negative auras if non-friendly target
                if (aurApp->IsPositive() == IsFriendlyTo(caster))
                    continue;
            }

            // The charges / stack amounts don't count towards the total number of auras that can be dispelled.
            // Ie: A dispel on a target with 5 stacks of Winters Chill and a Polymorph has 1 / (1 + 1) -> 50% chance to dispell
            // Polymorph instead of 1 / (5 + 1) -> 16%.
            bool dispel_charges = aura->GetSpellInfo()->AttributesEx7 & SPELL_ATTR7_DISPEL_CHARGES;
            uint8 charges = dispel_charges ? aura->GetCharges() : aura->GetStackAmount();
            if (charges > 0)
                dispelList.push_back(std::make_pair(aura, charges));
        }
    }
}

bool Unit::HasAuraEffect(uint32 spellId, uint8 effIndex, uint64 caster) const
{
    AuraApplicationMapBounds range = m_appliedAuras.equal_range(spellId);
    for (AuraApplicationMap::const_iterator itr = range.first; itr != range.second; ++itr)
    {
        if (itr->second->HasEffect(effIndex)
                && (!caster || itr->second->GetBase()->GetCasterGUID() == caster))
        {
            return true;
        }
    }
    return false;
}

uint32 Unit::GetAuraCount(uint32 spellId) const
{
    uint32 count = 0;
    AuraApplicationMapBounds range = m_appliedAuras.equal_range(spellId);

    for (AuraApplicationMap::const_iterator itr = range.first; itr != range.second; ++itr)
    {
        if (itr->second->GetBase()->GetStackAmount() == 0)
            ++count;
        else
            count += (uint32)itr->second->GetBase()->GetStackAmount();
    }
    return count;
}

bool Unit::HasAura(uint32 spellId, uint64 casterGUID, uint64 itemCasterGUID, uint8 reqEffMask) const
{
    if (GetAuraApplication(spellId, casterGUID, itemCasterGUID, reqEffMask))
        return true;
    return false;
}

bool Unit::HasAuraType(AuraType auraType) const
{
    return (!m_modAuras[auraType].empty());
}

bool Unit::HasAuraTypeWithCaster(AuraType auratype, uint64 caster) const
{
    AuraEffectList const& mTotalAuraList = GetAuraEffectsByType(auratype);
    for (AuraEffectList::const_iterator i = mTotalAuraList.begin(); i != mTotalAuraList.end(); ++i)
        if (caster == (*i)->GetCasterGUID())
            return true;
    return false;
}

bool Unit::HasAuraTypeWithMiscvalue(AuraType auratype, int32 miscvalue) const
{
    AuraEffectList const& mTotalAuraList = GetAuraEffectsByType(auratype);
    for (AuraEffectList::const_iterator i = mTotalAuraList.begin(); i != mTotalAuraList.end(); ++i)
        if (miscvalue == (*i)->GetMiscValue())
            return true;
    return false;
}

bool Unit::HasAuraTypeWithAffectMask(AuraType auratype, SpellInfo const* affectedSpell) const
{
    AuraEffectList const& mTotalAuraList = GetAuraEffectsByType(auratype);
    for (AuraEffectList::const_iterator i = mTotalAuraList.begin(); i != mTotalAuraList.end(); ++i)
        if ((*i)->IsAffectingSpell(affectedSpell))
            return true;
    return false;
}

bool Unit::HasAuraTypeWithValue(AuraType auratype, int32 value) const
{
    AuraEffectList const& mTotalAuraList = GetAuraEffectsByType(auratype);
    for (AuraEffectList::const_iterator i = mTotalAuraList.begin(); i != mTotalAuraList.end(); ++i)
        if (value == (*i)->GetAmount())
            return true;
    return false;
}

bool Unit::HasNegativeAuraWithInterruptFlag(uint32 flag, uint64 guid)
{
    if (!(m_interruptMask & flag))
        return false;
    for (AuraApplicationList::iterator iter = m_interruptableAuras.begin(); iter != m_interruptableAuras.end(); ++iter)
    {
        if (!(*iter)->IsPositive() && (*iter)->GetBase()->GetSpellInfo()->AuraInterruptFlags & flag && (!guid || (*iter)->GetBase()->GetCasterGUID() == guid))
            return true;
    }
    return false;
}

bool Unit::HasNegativeAuraWithAttribute(uint32 flag, uint64 guid)
{
    for (AuraApplicationMap::iterator iter = m_appliedAuras.begin(); iter != m_appliedAuras.end(); ++iter)
    {
        Aura const* aura = iter->second->GetBase();
        if (!iter->second->IsPositive() && aura->GetSpellInfo()->Attributes & flag && (!guid || aura->GetCasterGUID() == guid))
            return true;
    }
    return false;
}

bool Unit::HasAuraTypeMoreThanDuration(AuraType auratype, int32 duration)
{
    AuraEffectList const& auras = GetAuraEffectsByType(auratype);

    if (duration == -1 && auras.size())
        return true;

    for (AuraApplicationMap::iterator iter = m_appliedAuras.begin(); iter != m_appliedAuras.end(); ++iter)
    {
        Aura const* aura = iter->second->GetBase();
        if (aura->GetMaxDuration() != -1 && aura->GetMaxDuration() < duration)
            return true;
    }
    return false;
}

bool Unit::HasAuraWithNegativeCaster(uint32 spellid)
{
    AuraApplicationMapBounds range = m_appliedAuras.equal_range(spellid);
    for (AuraApplicationMap::const_iterator iter = range.first; iter != range.second; ++iter)
    {
        if (Unit * caster = (iter->second->GetBase()->GetCaster()))
        {
            if (!caster->IsFriendlyTo(this))
                return true;
        }
    }
    return false;
}

bool Unit::HasAuraWithMechanic(uint32 mechanicMask)
{
    for (AuraApplicationMap::iterator iter = m_appliedAuras.begin(); iter != m_appliedAuras.end(); ++iter)
    {
        SpellInfo const* spellInfo  = iter->second->GetBase()->GetSpellInfo();
        if (spellInfo->Mechanic && (mechanicMask & (1 << spellInfo->Mechanic)))
            return true;

        for (uint8 i = 0; i < MAX_SPELL_EFFECTS; ++i)
            if (iter->second->HasEffect(i) && spellInfo->Effects[i].Effect && spellInfo->Effects[i].Mechanic)
                if (mechanicMask & (1 << spellInfo->Effects[i].Mechanic))
                    return true;
    }

    return false;
}

AuraEffect* Unit::IsScriptOverriden(SpellInfo const* spell, int32 script) const
{
    AuraEffectList const& auras = GetAuraEffectsByType(SPELL_AURA_OVERRIDE_CLASS_SCRIPTS);
    for (AuraEffectList::const_iterator i = auras.begin(); i != auras.end(); ++i)
    {
        if ((*i)->GetMiscValue() == script)
            if ((*i)->IsAffectingSpell(spell))
                return (*i);
    }
    return NULL;
}

uint32 Unit::GetDiseasesByCaster(uint64 casterGUID, bool remove)
{
    static const AuraType diseaseAuraTypes[] =
    {
        SPELL_AURA_PERIODIC_DAMAGE,         // Frost Fever and Blood Plague
        SPELL_AURA_MOD_DAMAGE_FROM_CASTER,  // Ebon Plague
        SPELL_AURA_NONE
    };

    uint32 diseases = 0;
    for (AuraType const* itr = &diseaseAuraTypes[0]; itr && itr[0] != SPELL_AURA_NONE; ++itr)
    {
        for (AuraEffectList::iterator i = m_modAuras[*itr].begin(); i != m_modAuras[*itr].end();)
        {
            // Get auras with disease dispel type by caster
            if ((*i)->GetSpellInfo()->Dispel == DISPEL_DISEASE
                && (*i)->GetCasterGUID() == casterGUID)
            {
                ++diseases;

                if (remove)
                {
                    RemoveAura((*i)->GetId(), (*i)->GetCasterGUID());
                    i = m_modAuras[*itr].begin();
                    continue;
                }
            }
            ++i;
        }
    }

    // Burning Blood, Item - Death Knight T12 Blood 2P Bonus
    if (HasAura(98957, casterGUID))
    {
        uint32 _min = 2;
        diseases = std::max(diseases, _min);
    }

    return diseases;
}

uint32 Unit::GetDoTsByCaster(uint64 casterGUID) const
{
    static const AuraType diseaseAuraTypes[] =
    {
        SPELL_AURA_PERIODIC_DAMAGE,
        SPELL_AURA_PERIODIC_DAMAGE_PERCENT,
        SPELL_AURA_NONE
    };

    uint32 dots = 0;
    for (AuraType const* itr = &diseaseAuraTypes[0]; itr && itr[0] != SPELL_AURA_NONE; ++itr)
    {
        Unit::AuraEffectList const& auras = GetAuraEffectsByType(*itr);
        for (AuraEffectList::const_iterator i = auras.begin(); i != auras.end(); ++i)
        {
            // Get auras by caster
            if ((*i)->GetCasterGUID() == casterGUID)
                ++dots;
        }
    }
    return dots;
}

int32 Unit::GetTotalAuraModifier(AuraType auratype) const
{
    std::map<SpellGroup, int32> SameEffectSpellGroup;
    int32 modifier = 0;

    AuraEffectList const& mTotalAuraList = GetAuraEffectsByType(auratype);
    for (AuraEffectList::const_iterator i = mTotalAuraList.begin(); i != mTotalAuraList.end(); ++i)
        if (!sSpellMgr->AddSameEffectStackRuleSpellGroups((*i)->GetSpellInfo(), (*i)->GetAmount(), SameEffectSpellGroup))
            modifier += (*i)->GetAmount();

    for (std::map<SpellGroup, int32>::const_iterator itr = SameEffectSpellGroup.begin(); itr != SameEffectSpellGroup.end(); ++itr)
        modifier += itr->second;

    return modifier;
}

float Unit::GetTotalAuraMultiplier(AuraType auratype) const
{
    float multiplier = 1.0f;

    AuraEffectList const& mTotalAuraList = GetAuraEffectsByType(auratype);
    for (AuraEffectList::const_iterator i = mTotalAuraList.begin(); i != mTotalAuraList.end(); ++i)
        AddPct(multiplier, (*i)->GetAmount());

    return multiplier;
}

int32 Unit::GetMaxPositiveAuraModifier(AuraType auratype)
{
    int32 modifier = 0;

    AuraEffectList const& mTotalAuraList = GetAuraEffectsByType(auratype);
    for (AuraEffectList::const_iterator i = mTotalAuraList.begin(); i != mTotalAuraList.end(); ++i)
    {
        if ((*i)->GetAmount() > modifier)
            modifier = (*i)->GetAmount();
    }

    return modifier;
}

int32 Unit::GetMaxNegativeAuraModifier(AuraType auratype) const
{
    int32 modifier = 0;

    AuraEffectList const& mTotalAuraList = GetAuraEffectsByType(auratype);
    for (AuraEffectList::const_iterator i = mTotalAuraList.begin(); i != mTotalAuraList.end(); ++i)
        if ((*i)->GetAmount() < modifier)
            modifier = (*i)->GetAmount();

    return modifier;
}

int32 Unit::GetTotalAuraModifierByMiscMask(AuraType auratype, uint32 misc_mask) const
{
    std::map<SpellGroup, int32> SameEffectSpellGroup;
    int32 modifier = 0;

    AuraEffectList const& mTotalAuraList = GetAuraEffectsByType(auratype);

    for (AuraEffectList::const_iterator i = mTotalAuraList.begin(); i != mTotalAuraList.end(); ++i)
        if ((*i)->GetMiscValue() & misc_mask)
            if (!sSpellMgr->AddSameEffectStackRuleSpellGroups((*i)->GetSpellInfo(), (*i)->GetAmount(), SameEffectSpellGroup))
                modifier += (*i)->GetAmount();

    for (std::map<SpellGroup, int32>::const_iterator itr = SameEffectSpellGroup.begin(); itr != SameEffectSpellGroup.end(); ++itr)
        modifier += itr->second;

    return modifier;
}

float Unit::GetTotalAuraMultiplierByMiscMask(AuraType auratype, uint32 misc_mask) const
{
    std::map<SpellGroup, int32> SameEffectSpellGroup;
    float multiplier = 1.0f;

    AuraEffectList const& mTotalAuraList = GetAuraEffectsByType(auratype);
    for (AuraEffectList::const_iterator i = mTotalAuraList.begin(); i != mTotalAuraList.end(); ++i)
    {
        if (((*i)->GetMiscValue() & misc_mask))
        {
            // Check if the Aura Effect has a the Same Effect Stack Rule and if so, use the highest amount of that SpellGroup
            // If the Aura Effect does not have this Stack Rule, it returns false so we can add to the multiplier as usual
            if (!sSpellMgr->AddSameEffectStackRuleSpellGroups((*i)->GetSpellInfo(), (*i)->GetAmount(), SameEffectSpellGroup))
                AddPct(multiplier, (*i)->GetAmount());
        }
    }
    // Add the highest of the Same Effect Stack Rule SpellGroups to the multiplier
    for (std::map<SpellGroup, int32>::const_iterator itr = SameEffectSpellGroup.begin(); itr != SameEffectSpellGroup.end(); ++itr)
        AddPct(multiplier, itr->second);

    return multiplier;
}

int32 Unit::GetMaxPositiveAuraModifierByMiscMask(AuraType auratype, uint32 misc_mask, const AuraEffect* except) const
{
    int32 modifier = 0;

    AuraEffectList const& mTotalAuraList = GetAuraEffectsByType(auratype);
    for (AuraEffectList::const_iterator i = mTotalAuraList.begin(); i != mTotalAuraList.end(); ++i)
    {
        if (except != (*i) && (*i)->GetMiscValue()& misc_mask && (*i)->GetAmount() > modifier)
            modifier = (*i)->GetAmount();
    }

    return modifier;
}

int32 Unit::GetMaxNegativeAuraModifierByMiscMask(AuraType auratype, uint32 misc_mask) const
{
    int32 modifier = 0;

    AuraEffectList const& mTotalAuraList = GetAuraEffectsByType(auratype);
    for (AuraEffectList::const_iterator i = mTotalAuraList.begin(); i != mTotalAuraList.end(); ++i)
    {
        if ((*i)->GetMiscValue()& misc_mask && (*i)->GetAmount() < modifier)
            modifier = (*i)->GetAmount();
    }

    return modifier;
}

int32 Unit::GetTotalAuraModifierByMiscValue(AuraType auratype, int32 misc_value) const
{
    std::map<SpellGroup, int32> SameEffectSpellGroup;
    int32 modifier = 0;

    AuraEffectList const& mTotalAuraList = GetAuraEffectsByType(auratype);
    for (AuraEffectList::const_iterator i = mTotalAuraList.begin(); i != mTotalAuraList.end(); ++i)
    {
        if ((*i)->GetMiscValue() == misc_value)
            if (!sSpellMgr->AddSameEffectStackRuleSpellGroups((*i)->GetSpellInfo(), (*i)->GetAmount(), SameEffectSpellGroup))
                modifier += (*i)->GetAmount();
    }

    for (std::map<SpellGroup, int32>::const_iterator itr = SameEffectSpellGroup.begin(); itr != SameEffectSpellGroup.end(); ++itr)
        modifier += itr->second;

    return modifier;
}

float Unit::GetTotalAuraMultiplierByMiscValue(AuraType auratype, int32 misc_value) const
{
    std::map<SpellGroup, int32> SameEffectSpellGroup;
    float multiplier = 1.0f;

    AuraEffectList const& mTotalAuraList = GetAuraEffectsByType(auratype);
    for (AuraEffectList::const_iterator i = mTotalAuraList.begin(); i != mTotalAuraList.end(); ++i)
    {
        if ((*i)->GetMiscValue() == misc_value)
            if (!sSpellMgr->AddSameEffectStackRuleSpellGroups((*i)->GetSpellInfo(), (*i)->GetAmount(), SameEffectSpellGroup))
                AddPct(multiplier, (*i)->GetAmount());
    }

    for (std::map<SpellGroup, int32>::const_iterator itr = SameEffectSpellGroup.begin(); itr != SameEffectSpellGroup.end(); ++itr)
        AddPct(multiplier, itr->second);

    return multiplier;
}

int32 Unit::GetMaxPositiveAuraModifierByMiscValue(AuraType auratype, int32 misc_value) const
{
    int32 modifier = 0;

    AuraEffectList const& mTotalAuraList = GetAuraEffectsByType(auratype);
    for (AuraEffectList::const_iterator i = mTotalAuraList.begin(); i != mTotalAuraList.end(); ++i)
    {
        if ((*i)->GetMiscValue() == misc_value && (*i)->GetAmount() > modifier)
            modifier = (*i)->GetAmount();
    }

    return modifier;
}

int32 Unit::GetMaxNegativeAuraModifierByMiscValue(AuraType auratype, int32 misc_value) const
{
    int32 modifier = 0;

    AuraEffectList const& mTotalAuraList = GetAuraEffectsByType(auratype);
    for (AuraEffectList::const_iterator i = mTotalAuraList.begin(); i != mTotalAuraList.end(); ++i)
    {
        if ((*i)->GetMiscValue() == misc_value && (*i)->GetAmount() < modifier)
            modifier = (*i)->GetAmount();
    }

    return modifier;
}

int32 Unit::GetTotalAuraModifierByAffectMask(AuraType auratype, SpellInfo const* affectedSpell) const
{
    std::map<SpellGroup, int32> SameEffectSpellGroup;
    int32 modifier = 0;

    AuraEffectList const& mTotalAuraList = GetAuraEffectsByType(auratype);
    for (AuraEffectList::const_iterator i = mTotalAuraList.begin(); i != mTotalAuraList.end(); ++i)
    {
        if ((*i)->IsAffectingSpell(affectedSpell))
            if (!sSpellMgr->AddSameEffectStackRuleSpellGroups((*i)->GetSpellInfo(), (*i)->GetAmount(), SameEffectSpellGroup))
                modifier += (*i)->GetAmount();
    }

    for (std::map<SpellGroup, int32>::const_iterator itr = SameEffectSpellGroup.begin(); itr != SameEffectSpellGroup.end(); ++itr)
        modifier += itr->second;

    return modifier;
}

float Unit::GetTotalAuraMultiplierByAffectMask(AuraType auratype, SpellInfo const* affectedSpell) const
{
    std::map<SpellGroup, int32> SameEffectSpellGroup;
    float multiplier = 1.0f;

    AuraEffectList const& mTotalAuraList = GetAuraEffectsByType(auratype);
    for (AuraEffectList::const_iterator i = mTotalAuraList.begin(); i != mTotalAuraList.end(); ++i)
    {
        if ((*i)->IsAffectingSpell(affectedSpell))
            if (!sSpellMgr->AddSameEffectStackRuleSpellGroups((*i)->GetSpellInfo(), (*i)->GetAmount(), SameEffectSpellGroup))
                AddPct(multiplier, (*i)->GetAmount());
    }

    for (std::map<SpellGroup, int32>::const_iterator itr = SameEffectSpellGroup.begin(); itr != SameEffectSpellGroup.end(); ++itr)
        AddPct(multiplier, itr->second);

    return multiplier;
}

int32 Unit::GetMaxPositiveAuraModifierByAffectMask(AuraType auratype, SpellInfo const* affectedSpell) const
{
    int32 modifier = 0;

    AuraEffectList const& mTotalAuraList = GetAuraEffectsByType(auratype);
    for (AuraEffectList::const_iterator i = mTotalAuraList.begin(); i != mTotalAuraList.end(); ++i)
    {
        if ((*i)->IsAffectingSpell(affectedSpell) && (*i)->GetAmount() > modifier)
            modifier = (*i)->GetAmount();
    }

    return modifier;
}

int32 Unit::GetMaxNegativeAuraModifierByAffectMask(AuraType auratype, SpellInfo const* affectedSpell) const
{
    int32 modifier = 0;

    AuraEffectList const& mTotalAuraList = GetAuraEffectsByType(auratype);
    for (AuraEffectList::const_iterator i = mTotalAuraList.begin(); i != mTotalAuraList.end(); ++i)
    {
        if ((*i)->IsAffectingSpell(affectedSpell) && (*i)->GetAmount() < modifier)
            modifier = (*i)->GetAmount();
    }

    return modifier;
}

int32 Unit::GetAuraAmountNoStack(AuraEffect const* effect) const
{
    int32 amount = effect->GetAmount();
    if (!amount)
        return 0;

    int32 cur_amount = amount;

    AuraType auratype = effect->GetAuraType();
    AuraEffectList const& mTotalAuraList = GetAuraEffectsByType(auratype);
    for (AuraEffectList::const_iterator i = mTotalAuraList.begin(); i != mTotalAuraList.end(); ++i)
    {
        if ( (effect != (*i) && effect->GetSpellInfo() == (*i)->GetSpellInfo()) || 
            sSpellMgr->CheckSpellGroupStackRules(effect->GetSpellInfo(), (*i)->GetSpellInfo()) == SPELL_GROUP_STACK_RULE_EXCLUSIVE_SAME_EFFECT)
        {
            if (amount < 0)
            {
                if (amount < (*i)->GetAmount())
                {
                    cur_amount = amount - (*i)->GetAmount();
                }
                else
                {
                    return 0;
                }
            }
            else
            {
                if (amount > (*i)->GetAmount())
                {
                    cur_amount = amount - (*i)->GetAmount();
                }
                else
                {
                    return 0;
                }
            }
        }
    }
    return cur_amount;
}

int32 Unit::GetAuraCountForCaster(uint32 auraid, Unit *caster) const
{
    int32 count = 0;
    if (!caster)
        return count;

    AuraApplicationMapBounds range = m_appliedAuras.equal_range(auraid);
    for (AuraApplicationMap::const_iterator itr = range.first; itr != range.second; ++itr)
    {
        if (itr->second->GetBase()->GetCasterGUID() == caster->GetGUID())
            count++;
    }
    return count;
}

void Unit::_RegisterDynObject(DynamicObject* dynObj)
{
    m_dynObj.push_back(dynObj);
}

void Unit::_UnregisterDynObject(DynamicObject* dynObj)
{
    m_dynObj.remove(dynObj);
}

DynamicObject* Unit::GetDynObject(uint32 spellId)
{
    if (m_dynObj.empty())
        return NULL;
    for (DynObjectList::const_iterator i = m_dynObj.begin(); i != m_dynObj.end();++i)
    {
        DynamicObject* dynObj = *i;
        if (dynObj->GetSpellId() == spellId)
            return dynObj;
    }
    return NULL;
}

void Unit::RemoveDynObject(uint32 spellId)
{
    if (m_dynObj.empty())
        return;
    for (DynObjectList::iterator i = m_dynObj.begin(); i != m_dynObj.end();)
    {
        DynamicObject* dynObj = *i;
        if (dynObj->GetSpellId() == spellId)
        {
            dynObj->Remove();
            i = m_dynObj.begin();
        }
        else
            ++i;
    }
}

void Unit::RemoveAllDynObjects()
{
    while (!m_dynObj.empty())
        m_dynObj.front()->Remove();
}

GameObject* Unit::GetGameObject(uint32 spellId) const
{
    for (GameObjectList::const_iterator i = m_gameObj.begin(); i != m_gameObj.end(); ++i)
        if ((*i)->GetSpellId() == spellId)
            return *i;

    return NULL;
}

void Unit::AddGameObject(GameObject* gameObj)
{
    if (!gameObj || !gameObj->GetOwnerGUID() == 0)
        return;

    m_gameObj.push_back(gameObj);
    gameObj->SetOwnerGUID(GetGUID());

    if (GetTypeId() == TYPEID_PLAYER && gameObj->GetSpellId())
    {
        SpellInfo const* createBySpell = sSpellMgr->GetSpellInfo(gameObj->GetSpellId());
        // Need disable spell use for owner
        if (createBySpell && createBySpell->Attributes & SPELL_ATTR0_DISABLED_WHILE_ACTIVE)
            // note: item based cooldowns and cooldown spell mods with charges ignored (unknown existing cases)
            ToPlayer()->AddSpellAndCategoryCooldowns(createBySpell, 0, NULL, true);
    }
}

void Unit::RemoveGameObject(GameObject* gameObj, bool del)
{
    if (!gameObj || gameObj->GetOwnerGUID() != GetGUID())
        return;

    gameObj->SetOwnerGUID(0);

    for (uint8 i = 0; i < MAX_GAMEOBJECT_SLOT; ++i)
    {
        if (m_ObjectSlot[i] == gameObj->GetGUID())
        {
            m_ObjectSlot[i] = 0;
            break;
        }
    }

    // GO created by some spell
    if (uint32 spellid = gameObj->GetSpellId())
    {
        RemoveAurasDueToSpell(spellid);

        if (GetTypeId() == TYPEID_PLAYER)
        {
            SpellInfo const* createBySpell = sSpellMgr->GetSpellInfo(spellid);
            // Need activate spell use for owner
            if (createBySpell && createBySpell->Attributes & SPELL_ATTR0_DISABLED_WHILE_ACTIVE)
                // note: item based cooldowns and cooldown spell mods with charges ignored (unknown existing cases)
                ToPlayer()->SendCooldownEvent(createBySpell);
        }
    }

    m_gameObj.remove(gameObj);

    if (del)
    {
        gameObj->SetRespawnTime(0);
        gameObj->Delete();
    }
}

void Unit::RemoveGameObject(uint32 spellid, bool del)
{
    if (m_gameObj.empty())
        return;
    GameObjectList::iterator i, next;
    for (i = m_gameObj.begin(); i != m_gameObj.end(); i = next)
    {
        next = i;
        if (spellid == 0 || (*i)->GetSpellId() == spellid)
        {
            (*i)->SetOwnerGUID(0);
            if (del)
            {
                (*i)->SetRespawnTime(0);
                (*i)->Delete();
            }

            next = m_gameObj.erase(i);
        }
        else
            ++next;
    }
}

void Unit::RemoveAllGameObjects()
{
    // remove references to unit
    while (!m_gameObj.empty())
    {
        GameObjectList::iterator i = m_gameObj.begin();
        (*i)->SetOwnerGUID(0);
        (*i)->SetRespawnTime(0);
        (*i)->Delete();
        m_gameObj.erase(i);
    }
}

void Unit::PlayHoverAnimation(bool enable)
{
    ObjectGuid guid = GetGUID();

    WorldPacket data(SMSG_SET_PLAY_HOVER_ANIM, 10);
    data.WriteBit(guid[4]);
    data.WriteBit(guid[0]);
    data.WriteBit(guid[1]);
    data.WriteBit(enable);
    data.WriteBit(guid[3]);
    data.WriteBit(guid[7]);
    data.WriteBit(guid[5]);
    data.WriteBit(guid[2]);
    data.WriteBit(guid[6]);

    data.WriteByteSeq(guid[3]);
    data.WriteByteSeq(guid[2]);
    data.WriteByteSeq(guid[1]);
    data.WriteByteSeq(guid[7]);
    data.WriteByteSeq(guid[0]);
    data.WriteByteSeq(guid[5]);
    data.WriteByteSeq(guid[4]);
    data.WriteByteSeq(guid[6]);

    SendMessageToSet(&data, true);
}

void Unit::SendSpellNonMeleeDamageLog(SpellNonMeleeDamage* log)
{
    WorldPacket data(SMSG_SPELLNONMELEEDAMAGELOG, (16+4+4+4+1+4+4+1+1+4+4+1)); // we guess size
    data.append(log->target->GetPackGUID());
    data.append(log->attacker->GetPackGUID());
    data << uint32(log->SpellID);
    data << uint32(log->damage);                            // damage amount
    int32 overkill = log->damage - log->target->GetHealth();
    data << uint32(overkill > 0 ? overkill : 0);            // overkill
    data << uint8 (log->schoolMask);                        // damage school
    data << uint32(log->absorb);                            // AbsorbedDamage
    data << uint32(log->resist);                            // resist
    data << uint8 (log->physicalLog);                       // if 1, then client show spell name (example: %s's ranged shot hit %s for %u school or %s suffers %u school damage from %s's spell_name
    data << uint8 (log->unused);                            // unused
    data << uint32(log->blocked);                           // blocked
    data << uint32(log->HitInfo);
    data << uint8 (0);                                      // flag to use extend data
    SendMessageToSet(&data, true);
}

void Unit::SendSpellNonMeleeDamageLog(Unit* target, uint32 SpellID, uint32 Damage, SpellSchoolMask damageSchoolMask, uint32 AbsorbedDamage, uint32 Resist, bool PhysicalDamage, uint32 Blocked, bool CriticalHit)
{
    SpellNonMeleeDamage log(this, target, SpellID, damageSchoolMask);
    log.damage = Damage - AbsorbedDamage - Resist - Blocked;
    log.absorb = AbsorbedDamage;
    log.resist = Resist;
    log.physicalLog = PhysicalDamage;
    log.blocked = Blocked;
    log.HitInfo = SPELL_HIT_TYPE_UNK1 | SPELL_HIT_TYPE_UNK3 | SPELL_HIT_TYPE_UNK6;
    if (CriticalHit)
        log.HitInfo |= SPELL_HIT_TYPE_CRIT;
    SendSpellNonMeleeDamageLog(&log);
}

void Unit::ProcDamageAndSpell(Unit* victim, uint32 procAttacker, uint32 procVictim, uint32 procExtra, uint32 amount, uint32 absorb, WeaponAttackType attType, SpellInfo const* procSpell, SpellInfo const* procAura)
{
     // Not much to do if no flags are set.
    if (procAttacker)
        ProcDamageAndSpellFor(false, victim, procAttacker, procExtra, attType, procSpell, amount, absorb, procAura);
    // Now go on with a victim's events'n'auras
    // Not much to do if no flags are set or there is no victim
    if (victim && victim->isAlive() && procVictim)
        victim->ProcDamageAndSpellFor(true, this, procVictim, procExtra, attType, procSpell, amount, absorb, procAura);
}

void Unit::SendPeriodicAuraLog(SpellPeriodicAuraLogInfo* pInfo)
{
    AuraEffect const* aura = pInfo->auraEff;

    WorldPacket data(SMSG_PERIODICAURALOG, 30);
    data.append(GetPackGUID());
    data.appendPackGUID(aura->GetCasterGUID());
    data << uint32(aura->GetId());                          // spellId
    data << uint32(1);                                      // count
    data << uint32(aura->GetAuraType());                    // auraId
    switch (aura->GetAuraType())
    {
        case SPELL_AURA_PERIODIC_DAMAGE:
        case SPELL_AURA_PERIODIC_DAMAGE_PERCENT:
            data << uint32(pInfo->damage);                  // damage
            data << uint32(pInfo->overDamage);              // overkill?
            data << uint32(aura->GetSpellInfo()->GetSchoolMask());
            data << uint32(pInfo->absorb);                  // absorb
            data << uint32(pInfo->resist);                  // resist
            data << uint8(pInfo->critical);                 // new 3.1.2 critical tick
            break;
        case SPELL_AURA_PERIODIC_HEAL:
        case SPELL_AURA_OBS_MOD_HEALTH:
            data << uint32(pInfo->damage);                  // damage
            data << uint32(pInfo->overDamage);              // overheal
            data << uint32(pInfo->absorb);                  // absorb
            data << uint8(pInfo->critical);                 // new 3.1.2 critical tick
            break;
        case SPELL_AURA_OBS_MOD_POWER:
        case SPELL_AURA_PERIODIC_ENERGIZE:
            data << uint32(aura->GetMiscValue());           // power type
            data << uint32(pInfo->damage);                  // damage
            break;
        case SPELL_AURA_PERIODIC_MANA_LEECH:
            data << uint32(aura->GetMiscValue());           // power type
            data << uint32(pInfo->damage);                  // amount
            data << float(pInfo->multiplier);               // gain multiplier
            break;
        default:
            return;
    }

    SendMessageToSet(&data, true);
}

void Unit::SendSpellMiss(Unit* target, uint32 spellID, SpellMissInfo missInfo)
{
    WorldPacket data(SMSG_SPELLLOGMISS, (4+8+1+4+8+1));
    data << uint32(spellID);
    data << uint64(GetGUID());
    data << uint8(0);                                       // can be 0 or 1
    data << uint32(1);                                      // target count
    // for (i = 0; i < target count; ++i)
    data << uint64(target->GetGUID());                      // target GUID
    data << uint8(missInfo);
    // end loop
    SendMessageToSet(&data, true);
}

void Unit::SendSpellDamageResist(Unit* target, uint32 spellId)
{
    WorldPacket data(SMSG_PROCRESIST, 8+8+4+1);
    data << uint64(GetGUID());
    data << uint64(target->GetGUID());
    data << uint32(spellId);
    data << uint8(0); // bool - log format: 0-default, 1-debug
    SendMessageToSet(&data, true);
}

void Unit::SendSpellDamageImmune(Unit* target, uint32 spellId)
{
    WorldPacket data(SMSG_SPELLORDAMAGE_IMMUNE, 8+8+4+1);
    data << uint64(GetGUID());
    data << uint64(target->GetGUID());
    data << uint32(spellId);
    data << uint8(0); // bool - log format: 0-default, 1-debug
    SendMessageToSet(&data, true);
}

void Unit::SendMessageUnfriendlyToSetInRange(WorldPacket* data, float fist)
{
    Trinity::UnfriendlyMessageDistDeliverer notifier(this, data, GetVisibilityRange());
    VisitNearbyWorldObject(GetVisibilityRange(), notifier);
}


void Unit::SendAttackStateUpdate(CalcDamageInfo* damageInfo)
{
    uint32 count = 1;
    size_t maxsize = 4+5+5+4+4+1+4+4+4+4+4+1+4+4+4+4+4*12;
    WorldPacket data(SMSG_ATTACKERSTATEUPDATE, maxsize);    // we guess size
    data << uint32(damageInfo->HitInfo);
    data.append(damageInfo->attacker->GetPackGUID());
    data.append(damageInfo->target->GetPackGUID());
    data << uint32(damageInfo->damage);                     // Full damage
    int32 overkill = damageInfo->damage - damageInfo->target->GetHealth();
    data << uint32(overkill < 0 ? 0 : overkill);            // Overkill
    data << uint8(count);                                   // Sub damage count

    for (uint32 i = 0; i < count; ++i)
    {
        data << uint32(damageInfo->damageSchoolMask);       // School of sub damage
        data << float(damageInfo->damage);                  // sub damage
        data << uint32(damageInfo->damage);                 // Sub Damage
    }

    if (damageInfo->HitInfo & (HITINFO_FULL_ABSORB | HITINFO_PARTIAL_ABSORB))
    {
        for (uint32 i = 0; i < count; ++i)
            data << uint32(damageInfo->absorb);             // Absorb
    }

    if (damageInfo->HitInfo & (HITINFO_FULL_RESIST | HITINFO_PARTIAL_RESIST))
    {
        for (uint32 i = 0; i < count; ++i)
            data << uint32(damageInfo->resist);             // Resist
    }

    data << uint8(damageInfo->TargetState);
    data << uint32(0);  // Unknown attackerstate
    data << uint32(0);  // Melee spellid

    if (damageInfo->HitInfo & HITINFO_BLOCK)
        data << uint32(damageInfo->blocked_amount);

    if (damageInfo->HitInfo & HITINFO_RAGE_GAIN)
        data << uint32(0);

    //! Probably used for debugging purposes, as it is not known to appear on retail servers
    if (damageInfo->HitInfo & HITINFO_UNK1)
    {
        data << uint32(0);
        data << float(0);
        data << float(0);
        data << float(0);
        data << float(0);
        data << float(0);
        data << float(0);
        data << float(0);
        data << float(0);
        for (uint8 i = 0; i < 2; ++i)
        {
            data << float(0);
            data << float(0);
        }
        data << uint32(0);
    }

    SendMessageToSet(&data, true);
}

void Unit::SendAttackStateUpdate(uint32 HitInfo, Unit* target, uint8 /*SwingType*/, SpellSchoolMask damageSchoolMask, uint32 Damage, uint32 AbsorbDamage, uint32 Resist, VictimState TargetState, uint32 BlockedAmount)
{
    CalcDamageInfo dmgInfo;
    dmgInfo.HitInfo = HitInfo;
    dmgInfo.attacker = this;
    dmgInfo.target = target;
    dmgInfo.damage = Damage - AbsorbDamage - Resist - BlockedAmount;
    dmgInfo.damageSchoolMask = damageSchoolMask;
    dmgInfo.absorb = AbsorbDamage;
    dmgInfo.resist = Resist;
    dmgInfo.TargetState = TargetState;
    dmgInfo.blocked_amount = BlockedAmount;
    SendAttackStateUpdate(&dmgInfo);
}

bool Unit::HandleHasteAuraProc(Unit* victim, uint32 damage, AuraEffect* triggeredByAura, SpellInfo const* /*procSpell*/, uint32 /*procFlag*/, uint32 /*procEx*/, uint32 cooldown)
{
    SpellInfo const* hasteSpell = triggeredByAura->GetSpellInfo();

    Item* castItem = triggeredByAura->GetBase()->GetCastItemGUID() && GetTypeId() == TYPEID_PLAYER
        ? ToPlayer()->GetItemByGuid(triggeredByAura->GetBase()->GetCastItemGUID()) : NULL;

    uint32 triggered_spell_id = 0;
    Unit* target = victim;
    int32 basepoints0 = 0;

    switch (hasteSpell->SpellFamilyName)
    {
        case SPELLFAMILY_WARLOCK:
        {
            switch (hasteSpell->Id)
            {
                // Dark Intent Proc
                case 85768:
                case 85767:
                {
                    if (Unit * caster = triggeredByAura->GetCaster())
                    {
                        switch(caster->getClass())
                        {
                            case CLASS_WARRIOR:         triggered_spell_id = 94313; break;
                            case CLASS_PALADIN:         triggered_spell_id = 94323; break;
                            case CLASS_HUNTER:          triggered_spell_id = 94320; break;
                            case CLASS_ROGUE:           triggered_spell_id = 94324; break;
                            case CLASS_PRIEST:          triggered_spell_id = 94311; break;
                            case CLASS_DEATH_KNIGHT:    triggered_spell_id = 94312; break;
                            case CLASS_SHAMAN:          triggered_spell_id = 94319; break;
                            case CLASS_MAGE:            triggered_spell_id = 85759; break;
                            case CLASS_WARLOCK:         triggered_spell_id = 94310; break;
                            case CLASS_DRUID:           triggered_spell_id = 94318; break;
                            default: break;
                        }
                        // other targets gain ${$85759m1/3}% bonus.
                        if (hasteSpell->Id == 85767)
                        {
                            int32 basepoints0 = 1;
                            caster->CastCustomSpell(caster, triggered_spell_id, &basepoints0, 0, 0, true);
                        }
                        else
                        {
                            caster->CastSpell(caster, triggered_spell_id, true);
                        }
                    }
                    return true;
                }
            }
            break;
        }
    }

    // processed charge only counting case
    if (!triggered_spell_id)
        return true;

    SpellInfo const* triggerEntry = sSpellMgr->GetSpellInfo(triggered_spell_id);

    if (!triggerEntry)
        return false;

    if (cooldown && GetTypeId() == TYPEID_PLAYER && ToPlayer()->HasSpellCooldown(triggered_spell_id))
        return false;

    if (basepoints0)
        CastCustomSpell(target, triggered_spell_id, &basepoints0, NULL, NULL, true, castItem, triggeredByAura);
    else
        CastSpell(target, triggered_spell_id, true, castItem, triggeredByAura);

    if (cooldown && GetTypeId() == TYPEID_PLAYER)
        ToPlayer()->AddSpellCooldown(triggered_spell_id, 0, time(NULL) + cooldown);

    return true;
}

bool Unit::HandleModPowerRegenAuraProc(Unit* victim, uint32 damage, AuraEffect* triggeredByAura, SpellInfo const * procSpell, uint32 procFlag, uint32 procEx, uint32 cooldown)
{
    SpellInfo const* triggeredByAuraSpell = triggeredByAura->GetSpellInfo();

    Item* castItem = triggeredByAura->GetBase()->GetCastItemGUID() && GetTypeId() == TYPEID_PLAYER
        ? ToPlayer()->GetItemByGuid(triggeredByAura->GetBase()->GetCastItemGUID()) : NULL;

    uint32 triggered_spell_id = 0;
    Unit* target = victim;
    int32 basepoints0 = 0;

    switch(triggeredByAuraSpell->SpellFamilyName)
    {
        case SPELLFAMILY_ROGUE:
        {
            switch(triggeredByAuraSpell->Id)
            {
                // Blade Flurry
                case 13877:
                {
                    target = SelectNearbyTarget();
                    if (!target || target == victim)
                        return false;
                    basepoints0 = damage;
                    triggered_spell_id = 22482;
                    break;
                }
            }
            break;
        }
    }

    // processed charge only counting case
    if (!triggered_spell_id)
        return true;

    SpellInfo const* triggerEntry = sSpellMgr->GetSpellInfo(triggered_spell_id);

    if (!triggerEntry)
        return false;

    // default case
    if (!target || (target != this && !target->isAlive()))
        return false;

    if (cooldown && GetTypeId() == TYPEID_PLAYER && ToPlayer()->HasSpellCooldown(triggered_spell_id))
        return false;

    if (basepoints0)
        CastCustomSpell(target, triggered_spell_id, &basepoints0, NULL, NULL, true, castItem, triggeredByAura);
    else
        CastSpell(target, triggered_spell_id, true, castItem, triggeredByAura);

    if (cooldown && GetTypeId() == TYPEID_PLAYER)
        ToPlayer()->AddSpellCooldown(triggered_spell_id, 0, time(NULL) + cooldown);

    return true;
}

bool Unit::HandleSpellCritChanceAuraProc(Unit* victim, uint32 /*damage*/, AuraEffect* triggeredByAura, SpellInfo const* /*procSpell*/, uint32 /*procFlag*/, uint32 /*procEx*/, uint32 cooldown)
{
    SpellInfo const* triggeredByAuraSpell = triggeredByAura->GetSpellInfo();

    Item* castItem = triggeredByAura->GetBase()->GetCastItemGUID() && GetTypeId() == TYPEID_PLAYER
        ? ToPlayer()->GetItemByGuid(triggeredByAura->GetBase()->GetCastItemGUID()) : NULL;

    uint32 triggered_spell_id = 0;
    Unit* target = victim;
    int32 basepoints0 = 0;

    // processed charge only counting case
    if (!triggered_spell_id)
        return true;

    SpellInfo const* triggerEntry = sSpellMgr->GetSpellInfo(triggered_spell_id);

    if (!triggerEntry)
        return false;

    // default case
    if (!target || (target != this && !target->isAlive()))
        return false;

    if (cooldown && GetTypeId() == TYPEID_PLAYER && ToPlayer()->HasSpellCooldown(triggered_spell_id))
        return false;

    if (basepoints0)
        CastCustomSpell(target, triggered_spell_id, &basepoints0, NULL, NULL, true, castItem, triggeredByAura);
    else
        CastSpell(target, triggered_spell_id, true, castItem, triggeredByAura);

    if (cooldown && GetTypeId() == TYPEID_PLAYER)
        ToPlayer()->AddSpellCooldown(triggered_spell_id, 0, time(NULL) + cooldown);

    return true;
}

bool Unit::HandleAuraProcOnPowerAmount(Unit* victim, uint32 /*damage*/, AuraEffect* triggeredByAura, SpellInfo const *procSpell, uint32 procFlag, uint32 /*procEx*/, uint32 cooldown)
{
    // Get triggered aura spell info
    SpellInfo const* auraSpellInfo = triggeredByAura->GetSpellInfo();

    // Get effect index used for the proc
    uint32 effIndex = triggeredByAura->GetEffIndex();

    // Power amount required to proc the spell
    int32 powerAmountRequired = triggeredByAura->GetAmount();
    // Power type required to proc
    Powers powerRequired = Powers(auraSpellInfo->Effects[triggeredByAura->GetEffIndex()].MiscValue);

    // Set trigger spell id, target, custom basepoints
    uint32 trigger_spell_id = auraSpellInfo->Effects[triggeredByAura->GetEffIndex()].TriggerSpell;

    Unit*  target = NULL;
    int32  basepoints0 = 0;

    Item* castItem = triggeredByAura->GetBase()->GetCastItemGUID() && GetTypeId() == TYPEID_PLAYER
        ? ToPlayer()->GetItemByGuid(triggeredByAura->GetBase()->GetCastItemGUID()) : NULL;

    /* Try handle unknown trigger spells or with invalid power amount or misc value
    if (sSpellMgr->GetSpellInfo(trigger_spell_id) == NULL || powerAmountRequired == NULL || powerRequired >= MAX_POWER)
    {
        switch (auraSpellInfo->SpellFamilyName)
        {
            case SPELLFAMILY_GENERIC:
            {
                break;
            }
        }
    }*/

    // All ok. Check current trigger spell
    SpellInfo const* triggerEntry = sSpellMgr->GetSpellInfo(trigger_spell_id);
    if (triggerEntry == NULL)
    {
        // Not cast unknown spell
        // sLog->outError("Unit::HandleAuraProcOnPowerAmount: Spell %u have 0 in EffectTriggered[%d], not handled custom case?", auraSpellInfo->Id, triggeredByAura->GetEffIndex());
        return false;
    }

    // not allow proc extra attack spell at extra attack
    if (m_extraAttacks && triggerEntry->HasEffect(SPELL_EFFECT_ADD_EXTRA_ATTACKS))
        return false;

    if (!powerRequired || !powerAmountRequired)
        return false;

    if (GetPower(powerRequired) != powerAmountRequired)
        return false;

    // Custom requirements (not listed in procEx) Warning! damage dealing after this
    // Custom triggered spells
    switch (auraSpellInfo->SpellFamilyName)
    {
        case SPELLFAMILY_DRUID:
        {

            // Eclipse Mastery Driver Passive
            if (auraSpellInfo->Id == 79577)
            {
                uint32 solarEclipseMarker = 67483;
                uint32 lunarEclipseMarker = 67484;

                switch(effIndex)
                {
                    case 0:
                    {
                        // Can proc if proc spell is starfire and starsurge
                        if (procSpell->Id != 2912 && procSpell->Id != 78674 &&
                            procSpell->Id != 8921 && procSpell->Id != 93402 && procSpell->Id != 95746)
                            return false;

                        // Can proc if spell is moonfire or sunfire with lunar shower
                        if ((procSpell->Id == 8921 || procSpell->Id == 93402) &&
                            !(HasAura(81006) || HasAura(81191) || HasAura(81192)))
                            return false;

                        if (!HasAura(solarEclipseMarker))
                            return true;
                    
                        RemoveAurasDueToSpell(solarEclipseMarker);
                        CastSpell(this,lunarEclipseMarker,true);
                        OnEclipseBuff(false);
                        break;
                    }
                    case 1:
                    {
                        // Do not proc if proc spell isnt wrath and starsurge
                        if (procSpell->Id != 5176 && procSpell->Id != 78674 && procSpell->Id != 95746)
                            return false;

                        if (!HasAura(lunarEclipseMarker))
                            return true;

                        RemoveAurasDueToSpell(lunarEclipseMarker);
                        CastSpell(this,solarEclipseMarker,true);
                        OnEclipseBuff(true);
                        break;
                    }
                }
            }
            break;
        }
    }

    if (cooldown && GetTypeId() == TYPEID_PLAYER && ToPlayer()->HasSpellCooldown(trigger_spell_id))
        return false;

    // try detect target manually if not set
    if (target == NULL)
        target = !(procFlag & (PROC_FLAG_DONE_SPELL_MAGIC_DMG_CLASS_POS | PROC_FLAG_DONE_SPELL_NONE_DMG_CLASS_POS)) && triggerEntry && triggerEntry->IsPositive() ? this : victim;

    if (basepoints0)
        CastCustomSpell(target, trigger_spell_id, &basepoints0, NULL, NULL, true, castItem, triggeredByAura);
    else
        CastSpell(target, trigger_spell_id, true, castItem, triggeredByAura);

    if (cooldown && GetTypeId() == TYPEID_PLAYER)
        ToPlayer()->AddSpellCooldown(trigger_spell_id, 0, time(NULL) + cooldown);

    return true;
}

//victim may be NULL
bool Unit::HandleDummyAuraProc(Unit* victim, uint32 damage, AuraEffect* triggeredByAura, SpellInfo const* procSpell, uint32 procFlag, uint32 procEx, uint32 cooldown)
{
    SpellInfo const* dummySpell = triggeredByAura->GetSpellInfo();
    uint32 effIndex = triggeredByAura->GetEffIndex();
    int32  triggerAmount = triggeredByAura->GetAmount();

    Item* castItem = triggeredByAura->GetBase()->GetCastItemGUID() && GetTypeId() == TYPEID_PLAYER
        ? ToPlayer()->GetItemByGuid(triggeredByAura->GetBase()->GetCastItemGUID()) : NULL;

    uint32 triggered_spell_id = 0;
    uint32 cooldown_spell_id = 0; // for random trigger, will be one of the triggered spell to avoid repeatable triggers
                                  // otherwise, it's the triggered_spell_id by default
    Unit* target = victim;
    int32 basepoints0 = 0;
    uint64 originalCaster = 0;

    if (std::vector<SpellTriggered> const* spellTrigger = sSpellMgr->GetSpellTriggered(dummySpell->Id))
    {
        bool check = false;
        cooldown_spell_id = dummySpell->Id;
        if (cooldown && GetTypeId() == TYPEID_PLAYER && ToPlayer()->HasSpellCooldown(cooldown_spell_id))
            return false;

        for (std::vector<SpellTriggered>::const_iterator itr = spellTrigger->begin(); itr != spellTrigger->end(); ++itr)
        {
            if (!(itr->effectmask & (1<<effIndex)) || !target)
                continue;

            if(itr->target == 1)
                target = this;

            switch (itr->option)
            {
                case SPELL_TRIGGER_BP:
                {
                    if(itr->spell_trigger > 0)
                        basepoints0 = triggerAmount;
                    else
                        basepoints0 = -(triggerAmount);

                    triggered_spell_id = abs(itr->spell_trigger);
                    CastCustomSpell(target, triggered_spell_id, &basepoints0, &basepoints0, &basepoints0, true, castItem, triggeredByAura, originalCaster);
                    check = true;
                    continue;
                }
                break;
                case SPELL_TRIGGER_BP_CUSTOM:
                {
                    triggered_spell_id = abs(itr->spell_trigger);
                    CastCustomSpell(target, triggered_spell_id, &itr->bp0, &itr->bp1, &itr->bp2, true, castItem, triggeredByAura, originalCaster);
                    check = true;
                    continue;
                }
                break;
                case SPELL_TRIGGER_MANA_COST:
                {
                    if(!procSpell)
                        continue;
                    int32 cost = int32(procSpell->ManaCost + CalculatePct(GetCreateMana(), procSpell->ManaCostPercentage));
                    basepoints0 = CalculatePct(cost, itr->bp0);

                    triggered_spell_id = abs(itr->spell_trigger);
                    CastCustomSpell(target, triggered_spell_id, &basepoints0, &itr->bp1, &itr->bp2, true, castItem, triggeredByAura, originalCaster);
                    check = true;
                    continue;
                }
                break;
                case SPELL_TRIGGER_DAM_HEALTH:
                {
                    basepoints0 = CalculatePct(damage, triggerAmount);

                    triggered_spell_id = abs(itr->spell_trigger);
                    CastCustomSpell(target, triggered_spell_id, &basepoints0, &itr->bp1, &itr->bp2, true, castItem, triggeredByAura, originalCaster);
                    check = true;
                    continue;
                }
                break;
                case SPELL_TRIGGER_COOLDOWN:
                {
                    if(Player* player = target->ToPlayer())
                    {
                        uint32 spellid = abs(itr->spell_trigger);
                        if(itr->bp0 == 0)
                            player->RemoveSpellCooldown(spellid, true);
                        else
                        {
                            if(itr->aura && !procSpell)
                                continue;
                            if(itr->aura && procSpell && itr->aura != procSpell->Id)
                                continue;

                            float delay = float(itr->bp0 / 1000);
                            if(delay > -1.0f)
                            {
                                if(roll_chance_i(50))
                                    player->ChangeSpellCooldown(spellid, -1.0f);
                            }
                            else
                                player->ChangeSpellCooldown(spellid, delay);
                        }
                    }
                    check = true;
                    continue;
                }
                break;
                case SPELL_TRIGGER_UPDATE_DUR:
                {
                    if(Aura* aura = target->GetAura(abs(itr->spell_trigger), GetGUID()))
                        aura->RefreshDuration();
                    check = true;
                    continue;
                }
                break;
                case SPELL_TRIGGER_GET_DUR_AURA:
                {
                    if(Aura* aura = target->GetAura(itr->aura, GetGUID()))
                        basepoints0 = int32(aura->GetDuration() / 1000);
                    if(basepoints0)
                    {
                        triggered_spell_id = abs(itr->spell_trigger);
                        CastCustomSpell(this, triggered_spell_id, &basepoints0, &itr->bp1, &itr->bp2, true, castItem, triggeredByAura, originalCaster);
                    }
                    check = true;
                    continue;
                }
                break;
            }
        }

        if (cooldown && GetTypeId() == TYPEID_PLAYER)
            ToPlayer()->AddSpellCooldown(cooldown_spell_id, 0, time(NULL) + cooldown);
        if(check)
            return true;
    }

    switch (dummySpell->SpellFamilyName)
    {
        case SPELLFAMILY_GENERIC:
        {
            switch (dummySpell->Id)
            {
                case 105900: // Essence of Dreams, Ultraxion, Dragon Soul
                    basepoints0 = damage;
                    triggered_spell_id = 105996;
                    break;
                case 108007: // Indomitable, Indomitable Pride (normal)
                case 109785: // Indomitable, Indomitable Pride (lfr)
                case 109786: // Indomitable, Indomitable Pride (heroic)
                    if (!victim)
                        return false;
                    if (!damage)
                        return false;
                    if (effIndex != EFFECT_1)
                        return false;

                    if (!HealthBelowPctDamaged(50, damage))
                        return false;

                     if (Aura const* aur = triggeredByAura->GetBase())
                         if (AuraEffect const* aurEff = aur->GetEffect(EFFECT_1))
                             basepoints0 = int32(CalculatePct(damage, aurEff->GetAmount()));

                     triggered_spell_id = 108008;
                    break;
                // Fel Decay, Peroth'arn, Well of Eternity
                case 108124:
                    if (!victim)
                        return false;
                    if (!damage)
                        return false;
                    triggered_spell_id = 108128;
                    target = victim;
                    basepoints0 = damage;
                    break;
                // Matrix Restabilizer
                case 96976:
                case 97138:
                {
                    if (GetTypeId() != TYPEID_PLAYER)
                        return false;

                    uint32 crit = ToPlayer()->GetUInt32Value(PLAYER_FIELD_COMBAT_RATING_1 + CR_CRIT_MELEE);
                    uint32 mastery = ToPlayer()->GetUInt32Value(PLAYER_FIELD_COMBAT_RATING_1 + CR_MASTERY);
                    uint32 haste = ToPlayer()->GetUInt32Value(PLAYER_FIELD_COMBAT_RATING_1 + CR_HASTE_MELEE);

                    if (crit > mastery && crit > haste)
                        triggered_spell_id = (dummySpell->Id == 96976) ? 96978 : 97140;
                    else if (haste > mastery && haste > crit)
                        triggered_spell_id = (dummySpell->Id == 96976) ? 96977 : 97139;
                    else if (mastery > haste && mastery > crit)
                        triggered_spell_id = (dummySpell->Id == 96976) ? 96979 : 97141;

                    target = this;

                    break;
                }
                // Bane of Havoc
                case 85466:
                {
                    bool found = false;
                    // find target of bane
                    AuraIdList & scAuras = GetSingleCastAuras();
                    for (AuraIdList::iterator iter = scAuras.begin(); iter != scAuras.end(); ++iter)
                    {
                        if (Unit* pOwner = ObjectAccessor::FindUnit((*iter).guid))
                        {
                            if ((*iter).id == 80240)
                            {
                                if (Aura* aur = pOwner->GetAura((*iter).id, GetGUID())) 
                                {
                                    if (target->GetGUID() == (*iter).guid)
                                        return false;
                                    target = pOwner;
                                    basepoints0 = int32(CalculatePct(damage, aur->GetEffect(0)->GetAmount()));
                                    found = true;
                                }
                            }
                        }
                    }
                    /*AuraList & scAuras = GetSingleCastAuras();
                    for (AuraList::iterator iter = scAuras.begin(); iter != scAuras.end(); ++iter)
                        if( (*iter)->GetId() == 80240)
                        {
                            if (target == (*iter)->GetUnitOwner())
                                return false;
                            target = (*iter)->GetUnitOwner();
                            basepoints0 = int32(CalculatePct(damage, (*iter)->GetEffect(0)->GetAmount()));
                            found = true;
                        }*/
                    // no target found, break
                    if (!found)
                        return false;

                    triggered_spell_id = 85455;
                    break;
                }
                // Concentration, Majordomo Staghelm
                case 98229:
                    SetPower(POWER_ALTERNATE_POWER, 0);
                    break;
                // Vital Spark, Baleroc
                case 99262:
                {
                    if (!victim || !victim->HasAura(99252))
                        return false;
                    
                    if (Aura* aur = GetAura(99262))
                    {
                        int32 bp = aur->GetStackAmount() * 5;
                        CastCustomSpell(this, 99263, &bp, 0, 0, true);
                        aur->Remove();
                    }
                    break;
                }
                case 99256: // Torment, Baleroc (normal)
                case 100230:
                {
                    if (!victim || HasAura(99252) || victim->HasAura(99263))
                        return false;

                    if (Aura* aur = GetAura(dummySpell->Id))
                    {
                        uint8 stacks = aur->GetStackAmount();
                        if (stacks < 3)
                            return false;

                        int32 bp = int32(stacks / 3);
                        victim->CastCustomSpell(99262, SPELLVALUE_AURA_STACK, bp, victim, true);
                    }
                    break;
                }
                case 100231: // Torment, Baleroc (heroic)
                case 100232:
                {
                    if (!victim || HasAura(99252) || victim->HasAura(99263))
                        return false;

                    if (Aura* aur = GetAura(dummySpell->Id))
                    {
                        uint8 stacks = aur->GetStackAmount();
                        if (stacks < 5)
                            return false;

                        int32 bp = int32(stacks / 5);
                        victim->CastCustomSpell(99262, SPELLVALUE_AURA_STACK, bp, victim, true);
                    }
                    break;
                }
                case 107786: // Item - Dragon Soul - Proc - Agi Melee 1H Axe
                case 109873: // Item - Dragon Soul - Proc - Agi Melee 1H Axe Heroic
                case 109866: // Item - Dragon Soul - Proc - Agi Melee 1H Axe LFR
                {
                    const static uint32 DamageSpell[3][3]=
                    {
                    //   Flameblast  Shadowblast  Iceblast
                        {107785,     107787,      107789}, // Normal
                        {109872,     109868,      109870}, // Heroic
                        {109871,     109867,      109869}, // LFR
                    };

                    int32 index = -1;
                    switch (dummySpell->Id)
                    {
                        case 107786: index = 0; break; // Normal
                        case 109873: index = 1; break; // Heroic
                        case 109866: index = 2; break; // LFR
                    }

                    ASSERT(index >= 0 && index <= 2);
                    triggered_spell_id = DamageSpell[index][urand(0, 2)];
                    break;
                }
                // Weight of Feather, Scales of Life
                case 96879:
                case 97117:
                {
                    if (!victim)
                        return false;

                    int32 max_amount = triggeredByAura->GetAmount();
                    int32 add_heal = damage - (victim->GetMaxHealth() - victim->GetHealth());
                    if (add_heal <= 0)
                        return false;

                    int32 old_amount = 0;

                    if (AuraEffect* aurEff = victim->GetAuraEffect(96881, EFFECT_0))
                        old_amount = aurEff->GetAmount();

                    int32 new_amount = old_amount + add_heal;

                    int32 bp0 = std::min(new_amount, max_amount);
                    CastCustomSpell(victim, 96881, &bp0, 0, 0, true); 
                    return true;
                }
                // Pursuit of Justice
                case 26022:
                case 26023:
                {
                    if (!procSpell)
                        return false;

                    // Root, stun, immobilize
                    if (!(procSpell->GetAllEffectsMechanicMask() & ((1<<MECHANIC_ROOT) | (1<<MECHANIC_STUN)
                        | (1<<MECHANIC_FEAR))))
                        return false;

                    if (GetTypeId() == TYPEID_PLAYER)
                    {
                        if (ToPlayer()->HasSpellCooldown(31828) ||
                            ToPlayer()->HasSpellCooldown(31829))
                            return false;
                    }

                    // Denounce shouldn't give hp
                    if (procSpell->Id == 2812)
                        return false;

                    target = this;
                    triggered_spell_id = 89024;
                    break;
                }
                // Wrath of Tarecgosa
                case 101056:
                {
                    if (!procSpell)
                        return false;

                    if (!victim)
                        return false;

                    if (GetGUID() == victim->GetGUID())
                        return false;

                    if (procFlag & PROC_FLAG_DONE_PERIODIC)
                    {
                        if (!roll_chance_i(10))
                            return false;

                        basepoints0 = damage;
                        triggered_spell_id = 101085;
                        break;
                    }
                    else
                    {
                        if (!roll_chance_i(5))
                            return false;

                        triggered_spell_id = procSpell->Id;

                        // Fulmination              Improved Devouring Plague
                        if (procSpell->Id == 88767 || procSpell->Id == 63675)
                            basepoints0 = damage;

                        break;
                    }
                    break;
                }
                // Eye for an Eye
                case 9799:
                case 25988:
                {     
                    // return damage % to attacker but < 50% own total health
                    basepoints0 = int32(std::min(CalculatePct(damage, triggerAmount), CountPctFromMaxHealth(50)));
                    triggered_spell_id = 25997;
                    break;
                }
                // Sweeping Strikes
                case 18765:
                case 35429:
                {
                    target = SelectNearbyTarget(victim);
                    if (!target)
                        return false;

                    triggered_spell_id = 26654;
                    break;
                }
                // Unstable Power
                case 24658:
                {
                    if (!procSpell || procSpell->Id == 24659)
                        return false;
                    // Need remove one 24659 aura
                    RemoveAuraFromStack(24659);
                    return true;
                }
                // Restless Strength
                case 24661:
                {
                    // Need remove one 24662 aura
                    RemoveAuraFromStack(24662);
                    return true;
                }
                // Adaptive Warding (Frostfire Regalia set)
                case 28764:
                {
                    if (!procSpell)
                        return false;

                    // find Mage Armor
                    if (!GetAuraEffect(SPELL_AURA_MOD_MANA_REGEN_INTERRUPT, SPELLFAMILY_MAGE, 0x10000000, 0, 0))
                        return false;

                    switch (GetFirstSchoolInMask(procSpell->GetSchoolMask()))
                    {
                        case SPELL_SCHOOL_NORMAL:
                        case SPELL_SCHOOL_HOLY:
                            return false;                   // ignored
                        case SPELL_SCHOOL_FIRE:   triggered_spell_id = 28765; break;
                        case SPELL_SCHOOL_NATURE: triggered_spell_id = 28768; break;
                        case SPELL_SCHOOL_FROST:  triggered_spell_id = 28766; break;
                        case SPELL_SCHOOL_SHADOW: triggered_spell_id = 28769; break;
                        case SPELL_SCHOOL_ARCANE: triggered_spell_id = 28770; break;
                        default:
                            return false;
                    }

                    target = this;
                    break;
                }
                // Obsidian Armor (Justice Bearer`s Pauldrons shoulder)
                case 27539:
                {
                    if (!procSpell)
                        return false;

                    switch (GetFirstSchoolInMask(procSpell->GetSchoolMask()))
                    {
                        case SPELL_SCHOOL_NORMAL:
                            return false;                   // ignore
                        case SPELL_SCHOOL_HOLY:   triggered_spell_id = 27536; break;
                        case SPELL_SCHOOL_FIRE:   triggered_spell_id = 27533; break;
                        case SPELL_SCHOOL_NATURE: triggered_spell_id = 27538; break;
                        case SPELL_SCHOOL_FROST:  triggered_spell_id = 27534; break;
                        case SPELL_SCHOOL_SHADOW: triggered_spell_id = 27535; break;
                        case SPELL_SCHOOL_ARCANE: triggered_spell_id = 27540; break;
                        default:
                            return false;
                    }

                    target = this;
                    break;
                }
                // Mana Leech (Passive) (Priest Pet Aura)
                case 28305:
                {
                    // Cast on owner
                    target = GetOwner();
                    if (!target)
                        return false;

                    triggered_spell_id = 34650;

                    // Item - Priest T13 Shadow 4P Bonus (Shadowfiend and Shadowy Apparition)
                    if (AuraEffect * eff = target->GetAuraEffect(105844, 0))
                    {
                        if (roll_chance_i(eff->GetAmount()))
                        {
                            // don't apply if allready have orbs
                            bool found = false;
                            if (Aura * orbs = target->GetAura(77487))
                                if (orbs->GetStackAmount() == 3)
                                    found = true;

                            if (!found)
                            {
                                for (int i = 0; i < eff->GetSpellInfo()->Effects[1].BasePoints; i++)
                                {
                                    target->CastSpell(target, 77487, true);
                                }
                            }
                        }
                    }
                    break;
                }
                // Mark of Malice
                case 33493:
                {
                    // Cast finish spell at last charge
                    if (triggeredByAura->GetBase()->GetCharges() > 1)
                        return false;

                    target = this;
                    triggered_spell_id = 33494;
                    break;
                }
                // Twisted Reflection (boss spell)
                case 21063:
                    triggered_spell_id = 21064;
                    break;
                // Vampiric Aura (boss spell)
                case 38196:
                {
                    basepoints0 = 3 * damage;               // 300%
                    if (basepoints0 < 0)
                        return false;

                    triggered_spell_id = 31285;
                    target = this;
                    break;
                }
                // Aura of Madness (Darkmoon Card: Madness trinket)
                //=====================================================
                // 39511 Sociopath: +35 strength (Paladin, Rogue, Druid, Warrior)
                // 40997 Delusional: +70 attack power (Rogue, Hunter, Paladin, Warrior, Druid)
                // 40998 Kleptomania: +35 agility (Warrior, Rogue, Paladin, Hunter, Druid)
                // 40999 Megalomania: +41 damage/healing (Druid, Shaman, Priest, Warlock, Mage, Paladin)
                // 41002 Paranoia: +35 spell/melee/ranged crit strike rating (All classes)
                // 41005 Manic: +35 haste (spell, melee and ranged) (All classes)
                // 41009 Narcissism: +35 intellect (Druid, Shaman, Priest, Warlock, Mage, Paladin, Hunter)
                // 41011 Martyr Complex: +35 stamina (All classes)
                // 41406 Dementia: Every 5 seconds either gives you +5% damage/healing. (Druid, Shaman, Priest, Warlock, Mage, Paladin)
                // 41409 Dementia: Every 5 seconds either gives you -5% damage/healing. (Druid, Shaman, Priest, Warlock, Mage, Paladin)
                case 39446:
                {
                    if (GetTypeId() != TYPEID_PLAYER || !isAlive())
                        return false;

                    // Select class defined buff
                    switch (getClass())
                    {
                        case CLASS_PALADIN:                 // 39511, 40997, 40998, 40999, 41002, 41005, 41009, 41011, 41409
                        case CLASS_DRUID:                   // 39511, 40997, 40998, 40999, 41002, 41005, 41009, 41011, 41409
                            triggered_spell_id = RAND(39511, 40997, 40998, 40999, 41002, 41005, 41009, 41011, 41409);
                            cooldown_spell_id = 39511;
                            break;
                        case CLASS_ROGUE:                   // 39511, 40997, 40998, 41002, 41005, 41011
                        case CLASS_WARRIOR:                 // 39511, 40997, 40998, 41002, 41005, 41011
                        case CLASS_DEATH_KNIGHT:
                            triggered_spell_id = RAND(39511, 40997, 40998, 41002, 41005, 41011);
                            cooldown_spell_id = 39511;
                            break;
                        case CLASS_PRIEST:                  // 40999, 41002, 41005, 41009, 41011, 41406, 41409
                        case CLASS_SHAMAN:                  // 40999, 41002, 41005, 41009, 41011, 41406, 41409
                        case CLASS_MAGE:                    // 40999, 41002, 41005, 41009, 41011, 41406, 41409
                        case CLASS_WARLOCK:                 // 40999, 41002, 41005, 41009, 41011, 41406, 41409
                            triggered_spell_id = RAND(40999, 41002, 41005, 41009, 41011, 41406, 41409);
                            cooldown_spell_id = 40999;
                            break;
                        case CLASS_HUNTER:                  // 40997, 40999, 41002, 41005, 41009, 41011, 41406, 41409
                            triggered_spell_id = RAND(40997, 40999, 41002, 41005, 41009, 41011, 41406, 41409);
                            cooldown_spell_id = 40997;
                            break;
                        default:
                            return false;
                    }

                    target = this;
                    if (roll_chance_i(10))
                        ToPlayer()->Say("This is Madness!", LANG_UNIVERSAL); // TODO: It should be moved to database, shouldn't it?
                    break;
                }
                // Sunwell Exalted Caster Neck (??? neck)
                // cast ??? Light's Wrath if Exalted by Aldor
                // cast ??? Arcane Bolt if Exalted by Scryers
                case 46569:
                    return false;                           // old unused version
                // Sunwell Exalted Caster Neck (Shattered Sun Pendant of Acumen neck)
                // cast 45479 Light's Wrath if Exalted by Aldor
                // cast 45429 Arcane Bolt if Exalted by Scryers
                case 45481:
                {
                    if (GetTypeId() != TYPEID_PLAYER)
                        return false;

                    // Get Aldor reputation rank
                    if (ToPlayer()->GetReputationRank(932) == REP_EXALTED)
                    {
                        target = this;
                        triggered_spell_id = 45479;
                        break;
                    }
                    // Get Scryers reputation rank
                    if (ToPlayer()->GetReputationRank(934) == REP_EXALTED)
                    {
                        // triggered at positive/self casts also, current attack target used then
                        if (target && IsFriendlyTo(target))
                        {
                            target = getVictim();
                            if (!target)
                            {
                                uint64 selected_guid = ToPlayer()->GetSelection();
                                target = ObjectAccessor::GetUnit(*this, selected_guid);
                                if (!target)
                                    return false;
                            }
                            if (IsFriendlyTo(target))
                                return false;
                        }

                        triggered_spell_id = 45429;
                        break;
                    }
                    return false;
                }
                // Sunwell Exalted Melee Neck (Shattered Sun Pendant of Might neck)
                // cast 45480 Light's Strength if Exalted by Aldor
                // cast 45428 Arcane Strike if Exalted by Scryers
                case 45482:
                {
                    if (GetTypeId() != TYPEID_PLAYER)
                        return false;

                    // Get Aldor reputation rank
                    if (ToPlayer()->GetReputationRank(932) == REP_EXALTED)
                    {
                        target = this;
                        triggered_spell_id = 45480;
                        break;
                    }
                    // Get Scryers reputation rank
                    if (ToPlayer()->GetReputationRank(934) == REP_EXALTED)
                    {
                        triggered_spell_id = 45428;
                        break;
                    }
                    return false;
                }
                // Sunwell Exalted Tank Neck (Shattered Sun Pendant of Resolve neck)
                // cast 45431 Arcane Insight if Exalted by Aldor
                // cast 45432 Light's Ward if Exalted by Scryers
                case 45483:
                {
                    if (GetTypeId() != TYPEID_PLAYER)
                        return false;

                    // Get Aldor reputation rank
                    if (ToPlayer()->GetReputationRank(932) == REP_EXALTED)
                    {
                        target = this;
                        triggered_spell_id = 45432;
                        break;
                    }
                    // Get Scryers reputation rank
                    if (ToPlayer()->GetReputationRank(934) == REP_EXALTED)
                    {
                        target = this;
                        triggered_spell_id = 45431;
                        break;
                    }
                    return false;
                }
                // Sunwell Exalted Healer Neck (Shattered Sun Pendant of Restoration neck)
                // cast 45478 Light's Salvation if Exalted by Aldor
                // cast 45430 Arcane Surge if Exalted by Scryers
                case 45484:
                {
                    if (GetTypeId() != TYPEID_PLAYER)
                        return false;

                    // Get Aldor reputation rank
                    if (ToPlayer()->GetReputationRank(932) == REP_EXALTED)
                    {
                        target = this;
                        triggered_spell_id = 45478;
                        break;
                    }
                    // Get Scryers reputation rank
                    if (ToPlayer()->GetReputationRank(934) == REP_EXALTED)
                    {
                        triggered_spell_id = 45430;
                        break;
                    }
                    return false;
                }
                // Living Seed
                case 48504:
                {
                    triggered_spell_id = 48503;
                    basepoints0 = triggerAmount;
                    target = this;
                    break;
                }
                // Vampiric Touch (generic, used by some boss)
                case 52723:
                case 60501:
                {
                    triggered_spell_id = 52724;
                    basepoints0 = damage / 2;
                    target = this;
                    break;
                }
                // Shadowfiend Death (Gain mana if pet dies with Glyph of Shadowfiend)
                case 57989:
                {
                    Unit* owner = GetOwner();
                    if (!owner || owner->GetTypeId() != TYPEID_PLAYER)
                        return false;
                    // Glyph of Shadowfiend (need cast as self cast for owner, no hidden cooldown)
                    owner->CastSpell(owner, 58227, true, castItem, triggeredByAura);
                    return true;
                }
                // Divine purpose
                case 31871:
                case 31872:
                {
                    // Roll chane
                    if (!victim || !victim->isAlive() || !roll_chance_i(triggerAmount))
                        return false;

                    // Remove any stun effect on target
                    victim->RemoveAurasWithMechanic(1<<MECHANIC_STUN, AURA_REMOVE_BY_ENEMY_SPELL);
                    return true;
                }
                // Glyph of Life Tap
                case 63320:
                {
                    triggered_spell_id = 63321; // Life Tap
                    break;
                }
                // Purified Shard of the Scale - Onyxia 10 Caster Trinket
                case 69755:
                {
                    triggered_spell_id = (procFlag & PROC_FLAG_DONE_SPELL_MAGIC_DMG_CLASS_POS) ? 69733 : 69729;
                    break;
                }
                // Shiny Shard of the Scale - Onyxia 25 Caster Trinket
                case 69739:
                {
                    triggered_spell_id = (procFlag & PROC_FLAG_DONE_SPELL_MAGIC_DMG_CLASS_POS) ? 69734 : 69730;
                    break;
                }
                // Vampiric Might (Lady Deathwisper)
                case 70674:
                {
                    basepoints0 = CalculatePct(damage, triggerAmount);
                    triggered_spell_id = 70677;
                    break;
                }
                case 71519: // Deathbringer's Will Normal
                {
                    if (GetTypeId() != TYPEID_PLAYER)
                        return false;

                    if (ToPlayer()->HasSpellCooldown(dummySpell->Id))
                        return false;

                    std::vector<uint32> RandomSpells;
                    switch (getClass())
                    {
                        case CLASS_WARRIOR:
                        case CLASS_PALADIN:
                        case CLASS_DEATH_KNIGHT:
                            RandomSpells.push_back(71484);
                            RandomSpells.push_back(71491);
                            RandomSpells.push_back(71492);
                            break;
                        case CLASS_SHAMAN:
                        case CLASS_ROGUE:
                            RandomSpells.push_back(71486);
                            RandomSpells.push_back(71485);
                            RandomSpells.push_back(71492);
                            break;
                        case CLASS_DRUID:
                            RandomSpells.push_back(71484);
                            RandomSpells.push_back(71485);
                            RandomSpells.push_back(71492);
                            break;
                        case CLASS_HUNTER:
                            RandomSpells.push_back(71486);
                            RandomSpells.push_back(71491);
                            RandomSpells.push_back(71485);
                            break;
                        default:
                            return false;
                    }
                    if (RandomSpells.empty()) // shouldn't happen
                        return false;

                    triggered_spell_id = RandomSpells[irand(0, (RandomSpells.size() - 1))];
                    ToPlayer()->AddSpellCooldown(dummySpell->Id, 0, time(NULL) + cooldown);
                    break;
                }
                case 71562: // Deathbringer's Will Heroic
                {
                    if (GetTypeId() != TYPEID_PLAYER)
                        return false;

                    if (ToPlayer()->HasSpellCooldown(dummySpell->Id))
                        return false;

                    std::vector<uint32> RandomSpells;
                    switch (getClass())
                    {
                        case CLASS_WARRIOR:
                        case CLASS_PALADIN:
                        case CLASS_DEATH_KNIGHT:
                            RandomSpells.push_back(71561);
                            RandomSpells.push_back(71559);
                            RandomSpells.push_back(71560);
                            break;
                        case CLASS_SHAMAN:
                        case CLASS_ROGUE:
                            RandomSpells.push_back(71558);
                            RandomSpells.push_back(71556);
                            RandomSpells.push_back(71560);
                            break;
                        case CLASS_DRUID:
                            RandomSpells.push_back(71561);
                            RandomSpells.push_back(71556);
                            RandomSpells.push_back(71560);
                            break;
                        case CLASS_HUNTER:
                            RandomSpells.push_back(71558);
                            RandomSpells.push_back(71559);
                            RandomSpells.push_back(71556);
                            break;
                        default:
                            return false;
                    }
                    if (RandomSpells.empty()) // shouldn't happen
                        return false;

                    triggered_spell_id = RandomSpells[irand(0, (RandomSpells.size() - 1))];
                    ToPlayer()->AddSpellCooldown(dummySpell->Id, 0, time(NULL) + cooldown);
                    break;
                }
                case 71875: // Item - Black Bruise: Necrotic Touch Proc
                case 71877:
                {
                    basepoints0 = CalculatePct(int32(damage), triggerAmount);
                    triggered_spell_id = 71879;
                    break;
                }
                // Item - Shadowmourne Legendary
                case 71903:
                {
                    if (!victim || !victim->isAlive() || HasAura(73422))  // cant collect shards while under effect of Chaos Bane buff
                        return false;

                    CastSpell(this, 71905, true, NULL, triggeredByAura);

                    // this can't be handled in AuraScript because we need to know victim
                    Aura const* dummy = GetAura(71905);
                    if (!dummy || dummy->GetStackAmount() < 10)
                        return false;

                    RemoveAurasDueToSpell(71905);
                    triggered_spell_id = 71904;
                    target = victim;
                    break;
                }
                // Shadow's Fate (Shadowmourne questline)
                case 71169:
                {
                    target = triggeredByAura->GetCaster();
                    if (!target)
                        return false;
                    Player* player = target->ToPlayer();
                    if (!player)
                        return false;
                    // not checking Infusion auras because its in targetAuraSpell of credit spell
                    if (player->GetQuestStatus(24749) == QUEST_STATUS_INCOMPLETE)       // Unholy Infusion
                    {
                        if (GetEntry() != 36678)                                        // Professor Putricide
                            return false;
                        CastSpell(target, 71518, true);                                 // Quest Credit
                        return true;
                    }
                    else if (player->GetQuestStatus(24756) == QUEST_STATUS_INCOMPLETE)  // Blood Infusion
                    {
                        if (GetEntry() != 37955)                                        // Blood-Queen Lana'thel
                            return false;
                        CastSpell(target, 72934, true);                                 // Quest Credit
                        return true;
                    }
                    else if (player->GetQuestStatus(24757) == QUEST_STATUS_INCOMPLETE)  // Frost Infusion
                    {
                        if (GetEntry() != 36853)                                        // Sindragosa
                            return false;
                        CastSpell(target, 72289, true);                                 // Quest Credit
                        return true;
                    }
                    else if (player->GetQuestStatus(24547) == QUEST_STATUS_INCOMPLETE)  // A Feast of Souls
                        triggered_spell_id = 71203;
                    break;
                }
                // Essence of the Blood Queen
                case 70871:
                {
                    basepoints0 = CalculatePct(int32(damage), triggerAmount);
                    CastCustomSpell(70872, SPELLVALUE_BASE_POINT0, basepoints0, this, true);
                    return true;
                }
                case 65032: // Boom aura (321 Boombot)
                {
                    if (victim->GetEntry() != 33343)   // Scrapbot
                        return false;

                    InstanceScript* instance = GetInstanceScript();
                    if (!instance)
                        return false;

                    instance->DoCastSpellOnPlayers(65037);  // Achievement criteria marker
                    break;
                }
                // Dark Hunger (The Lich King encounter)
                case 69383:
                {
                    basepoints0 = CalculatePct(int32(damage), 50);
                    triggered_spell_id = 69384;
                    break;
                }
                case 47020: // Enter vehicle XT-002 (Scrapbot)
                {
                    if (GetTypeId() != TYPEID_UNIT)
                        return false;

                    Unit* vehicleBase = GetVehicleBase();
                    if (!vehicleBase)
                        return false;

                    // Todo: Check if this amount is blizzlike
                    vehicleBase->ModifyHealth(int32(vehicleBase->CountPctFromMaxHealth(1)));
                    break;
                }
            }
            break;
        }
        case SPELLFAMILY_MAGE:
        {
            // Nether Vortex
            if (dummySpell->SpellIconID == 2294)
            {
                if (target == this)
                    return false;

                triggered_spell_id = 31589; // slow
                AuraIdList & scAuras = GetSingleCastAuras();
                for (AuraIdList::iterator iter = scAuras.begin(); iter != scAuras.end(); ++iter)
                {
                    if ((*iter).id == triggered_spell_id)
                    {
                        // can update current target slow
                        if ((*iter).guid == target->GetGUID())
                            break;
                        return false;
                    }
                }
                break;
            }
            // Magic Absorption
            if (dummySpell->SpellIconID == 459)             // only this spell has SpellIconID == 459 and dummy aura
            {
                if (getPowerType() != POWER_MANA)
                    return false;

                // mana reward
                basepoints0 = CalculatePct(GetMaxPower(POWER_MANA), triggerAmount);
                target = this;
                triggered_spell_id = 29442;
                break;
            }
            // Master of Elements
            if (dummySpell->SpellIconID == 1920)
            {
                if (!procSpell)
                    return false;

                if (procSpell->Id == 2948)
                    if (HasAura(11367) || HasAura(11115))
                        return false;

                // mana cost save
                int32 cost = int32(procSpell->ManaCost + CalculatePct(GetCreateMana(), procSpell->ManaCostPercentage));
                basepoints0 = CalculatePct(cost, triggerAmount);
                if (basepoints0 <= 0)
                    return false;

                target = this;
                triggered_spell_id = 29077;
                break;
            }
            // Arcane Potency
            if (dummySpell->SpellIconID == 2120)
            {
                if (!procSpell)
                    return false;

                target = this;
                switch (dummySpell->Id)
                {
                    case 31571: triggered_spell_id = 57529; break;
                    case 31572: triggered_spell_id = 57531; break;
                    default:
                        return false;
                }
                break;
            }
            // Incanter's Regalia set (add trigger chance to Mana Shield)
            if (dummySpell->SpellFamilyFlags[0] & 0x8000)
            {
                if (GetTypeId() != TYPEID_PLAYER)
                    return false;

                target = this;
                triggered_spell_id = 37436;
                break;
            }
            switch (dummySpell->Id)
            {
                // Piercing Chill
                case 83156:
                case 83157:
                {
                    UnitList targets;
                    {
                        CellCoord p(Trinity::ComputeCellCoord(GetPositionX(), GetPositionY()));
                        Cell cell(p);

                        Trinity::AnyUnfriendlyAttackableVisibleUnitInObjectRangeCheck u_check(this, 8.0f);
                        Trinity::UnitListSearcher<Trinity::AnyUnfriendlyAttackableVisibleUnitInObjectRangeCheck> checker(this, targets, u_check);

                        TypeContainerVisitor<Trinity::UnitListSearcher<Trinity::AnyUnfriendlyAttackableVisibleUnitInObjectRangeCheck>, GridTypeMapContainer > grid_object_checker(checker);
                        TypeContainerVisitor<Trinity::UnitListSearcher<Trinity::AnyUnfriendlyAttackableVisibleUnitInObjectRangeCheck>, WorldTypeMapContainer > world_object_checker(checker);

                        cell.Visit(p, grid_object_checker,  *GetMap(), *this, 8.0f);
                        cell.Visit(p, world_object_checker, *GetMap(), *this, 8.0f);
                    }

                    if (targets.empty())
                        return true;
                    uint8 count = dummySpell->Id == 83156 ? 1: 2;
                    for (UnitList::const_iterator itr = targets.begin(); itr != targets.end(); ++itr)
                    {
                        if ((*itr)->GetGUID() == victim->GetGUID() || !(*itr)->IsWithinLOSInMap(this))
                            continue;
                        if (!count)
                            break;
                        CastSpell((*itr), 83154, true);
                        --count;
                    }
                    return true;
                }
                // Improved Cone of Cold (Rank 1)
                case 11190:
                    target = victim;
                    triggered_spell_id = 83301;
                    break;
                // Improved Cone of Cold (Rank 2)
                case 12489:
                    target = victim;
                    triggered_spell_id = 83302;
                    break;
                // Glyph of Polymorph
                case 56375:
                {
                    if (!target)
                        return false;
                    target->RemoveDotsNotSWD();
                    return true;
                }
                // Glyph of Icy Veins
                case 56374:
                {
                    RemoveAurasByType(SPELL_AURA_HASTE_SPELLS, 0, 0, 0, true, false);
                    RemoveAurasByType(SPELL_AURA_MOD_DECREASE_SPEED);
                    return true;
                }
                // Ignite
                case 11119:
                case 11120:
                case 12846:
                {
                    Unit* original_caster = GetProcOwner();
                    if (!original_caster)
                        return false;

                    // Trigger amount should be divided by ticks count. Assumed that player can't change ticks count
                    triggerAmount /= 2;
                    basepoints0 = CalculatePct(damage, triggerAmount);
                    // Mastery should affect damage
                    if (AuraEffect const* aurEff = original_caster->GetAuraEffect(SPELL_AURA_ADD_PCT_MODIFIER, SPELLFAMILY_MAGE, 37, EFFECT_0))
                        AddPct<int32>(basepoints0, aurEff->GetAmount());

                    triggered_spell_id = 12654;
                    basepoints0 += victim->GetRemainingPeriodicAmount(original_caster->GetGUID(), triggered_spell_id, SPELL_AURA_PERIODIC_DAMAGE);
                    originalCaster = original_caster->GetGUID();
                    break;
                }
                // Blessing of Ancient Kings (Val'anyr, Hammer of Ancient Kings)
                case 64411:
                {
                    if (!victim)
                        return false;
                    basepoints0 = int32(CalculatePct(damage, 15));
                    if (AuraEffect* aurEff = victim->GetAuraEffect(64413, 0, GetGUID()))
                    {
                        // The shield can grow to a maximum size of 20, 000 damage absorbtion
                        aurEff->SetAmount(std::min<int32>(aurEff->GetAmount() + basepoints0, 20000));

                        // Refresh and return to prevent replacing the aura
                        aurEff->GetBase()->RefreshDuration();
                        return true;
                    }
                    target = victim;
                    triggered_spell_id = 64413;
                    break;
                }
                // Permafrost
                case 11175:
                case 12569:
                case 12571:
                {
                    if (!GetGuardianPet())
                        return false;

                    // heal amount
                    basepoints0 = CalculatePct(damage, triggerAmount);
                    target = this;
                    triggered_spell_id = 91394;
                    break;
                }
                // Hot Streak
                case 44445:
                {
                    if (effIndex != 0)
                        return false;

                    // Selfproc from instant pyroblast
                    if (procSpell->Id == 92315 && !procSpell->CalcCastTime(this))
                        return false;

                    float crit = GetSpellCrit(victim, procSpell, SpellSchoolMask(procSpell->SchoolMask), BASE_ATTACK) / 100.0f;
                    float f_chance = (-1.7106f * crit + 0.7893f) * 100.0f;

                    if (f_chance < 0 || !f_chance || !roll_chance_f(f_chance))
                        return false;

                    target = this;
                    triggered_spell_id = 48108;
                    break;
                }
                // Improved Hot Streak
                case 44446:
                case 44448:
                {
                    if (effIndex != 0)
                        return false;

                    if (!roll_chance_i(triggerAmount))
                        return false;

                    // Selfproc from instant pyroblast
                    if (procSpell->Id == 92315 && !procSpell->CalcCastTime(this))
                        return false;

                    if (procEx & PROC_EX_CRITICAL_HIT && (procSpell->SpellFamilyFlags[1] & 0x00001000 || procSpell->SpellFamilyFlags[0] & 0x00400013))
                    {
                        if (triggeredByAura->GetBase()->GetCharges() <= 1)
                        {
                            triggeredByAura->GetBase()->SetCharges(2);
                            return true;
                        }
                    }
                    else
                    {
                        triggeredByAura->GetBase()->SetCharges(0);
                        return false;
                    }
                    triggeredByAura->GetBase()->SetCharges(0);
                    triggered_spell_id = 48108;
                    target = this;
                    break;
                }
            }
            break;
        }
        case SPELLFAMILY_WARRIOR:
        {
            switch (dummySpell->Id)
            {
                // Item - Warrior T13 Protection 2P Bonus (Revenge)
                case 105908:
                    if (!victim)
                        return false;

                    if (GetTypeId() != TYPEID_PLAYER)
                        return false;

                    if (victim != ToPlayer()->GetSelectedUnit())
                        return false;
                    
                    basepoints0 = int32(CalculatePct(damage, 20));
                    triggered_spell_id = 105909;

                    if (AuraEffect const* aurEff = GetAuraEffect(triggered_spell_id, EFFECT_0))
                        basepoints0 += aurEff->GetAmount();

                    target = this;

                    break;
                // Item - Warrior T12 Protection 2P Bonus
                case 99239:
                {
                    basepoints0 = int32(CalculatePct(damage, triggerAmount / 2)); // 2 ticks
                    triggered_spell_id = 99240;
                    target = victim;
                    break;
                }
                // Strikes of Opportunity (Mastery Arms Warrior)
                case 76838:
                {
                    if (effIndex != 0 || !roll_chance_i(triggeredByAura->GetAmount()))
                        return false;

                    triggered_spell_id = 76858;
                    break;
                }
                // Sweeping Strikes
                case 12328:
                {
                    if (!victim || !damage)
                        return false;

                    if (GetTypeId() != TYPEID_PLAYER)
                        return false;

                    if ((!procSpell || (procSpell->Id != 845 && procSpell->Id != 50622)) &&
                        (procEx & PROC_EX_INTERNAL_TRIGGERED || procEx & PROC_EX_INTERNAL_CANT_PROC))
                        return false;

                    triggered_spell_id = 12723;

                    // Sweeping Strikes can damage only one target per procSpell
                    // So add small cooldown to avoid repeated procs from procSpell
                    if (procSpell && procSpell->IsTargetingArea())
                    {
                        if (ToPlayer()->HasSpellCooldown(triggered_spell_id))
                            return false;
                        ToPlayer()->AddSpellCooldown(triggered_spell_id, 0, time(NULL) + 1);
                    }

                    target = SelectNearbyTarget(victim, 5);

                    if (!target)
                        return false;

                    basepoints0 = damage;
                    break;
                }
                // Victorious
                case 32216:
                {
                    RemoveAura(dummySpell->Id);
                    return false;
                }
                // Glyph of Colossus Smash
                case 89003:
                {
                    triggered_spell_id = 58567;
                    break;
                }
            }

            // Retaliation
            if (dummySpell->SpellFamilyFlags[1] & 0x8)
            {
                // check attack comes not from behind
                if (!HasInArc(M_PI, victim))
                    return false;

                triggered_spell_id = 22858;
                break;
            }
            // Second Wind
            if (dummySpell->SpellIconID == 1697)
            {
                // only for spells and hit/crit (trigger start always) and not start from self casted spells (5530 Mace Stun Effect for example)
                if (procSpell == 0 || !(procEx & (PROC_EX_NORMAL_HIT|PROC_EX_CRITICAL_HIT)) || this == victim)
                    return false;
                // Need stun or root mechanic
                if (!(procSpell->GetAllEffectsMechanicMask() & ((1<<MECHANIC_ROOT)|(1<<MECHANIC_STUN))))
                    return false;

                switch (dummySpell->Id)
                {
                    case 29838: 
                        triggered_spell_id = 29842; 
                        break;
                    case 29834: 
                        triggered_spell_id = 29841; 
                        break;
                    default:
                        break;
                    return false;
                }

                target = this;
                break;
            }
            // Blood and Thunder
            if (dummySpell->SpellIconID == 5057)
            {
                if (victim && victim->HasAura(94009, GetGUID()))
                    SpreadAura(94009, 8, false);
                return true;
            }
            // Glyph of Sunder Armor
            if (dummySpell->Id == 58387)
            {
                if (!victim || !victim->isAlive() || !procSpell)
                    return false;

                target = SelectNearbyTarget(victim);
                if (!target)
                    return false;

                triggered_spell_id = 58567;
                break;
            }
            break;
        }
        case SPELLFAMILY_WARLOCK:
        {
            // Seed of Corruption
            if (dummySpell->SpellFamilyFlags[1] & 0x00000010)
            {
                if (procSpell && procSpell->SpellFamilyFlags[1] & 0x8000)
                    return false;
                // if damage is more than need or target die from damage deal finish spell
                if (triggeredByAura->GetAmount() <= int32(damage) || GetHealth() <= damage)
                {
                    // remember guid before aura delete
                    uint64 casterGuid = triggeredByAura->GetCasterGUID();

                    // Remove aura (before cast for prevent infinite loop handlers)
                    RemoveAurasDueToSpell(triggeredByAura->GetId());

                    // Cast finish spell (triggeredByAura already not exist!)
                    if (Unit* caster = GetUnit(*this, casterGuid))
                        caster->CastSpell(this, 27285, true);
                    return true;                            // no hidden cooldown
                }

                // Damage counting
                triggeredByAura->SetAmount(triggeredByAura->GetAmount() - damage);
                return true;
            }
            // Seed of Corruption (Mobs cast) - no die req
            if (dummySpell->SpellFamilyFlags.IsEqual(0, 0, 0) && dummySpell->SpellIconID == 1932)
            {
                // if damage is more than need deal finish spell
                if (triggeredByAura->GetAmount() <= int32(damage))
                {
                    // remember guid before aura delete
                    uint64 casterGuid = triggeredByAura->GetCasterGUID();

                    // Remove aura (before cast for prevent infinite loop handlers)
                    RemoveAurasDueToSpell(triggeredByAura->GetId());

                    // Cast finish spell (triggeredByAura already not exist!)
                    if (Unit* caster = GetUnit(*this, casterGuid))
                        caster->CastSpell(this, 32865, true, castItem);
                    return true;                            // no hidden cooldown
                }
                // Damage counting
                triggeredByAura->SetAmount(triggeredByAura->GetAmount() - damage);
                return true;
            }
            // Fel Synergy
            if (dummySpell->SpellIconID == 3222)
            {
                target = GetGuardianPet();
                if (!target)
                    return false;
                basepoints0 = CalculatePct(int32(damage), triggerAmount);
                triggered_spell_id = 54181;
                break;
            }
            switch (dummySpell->Id)
            {
                // Fel Armor
                case 28176: 
                {
                    triggered_spell_id = 96379;
                    basepoints0 = CalculatePct(int32(damage), triggerAmount);
                    target = this;
                    break;
                }
                // Curse of Weakness - Jinx
                case 85479:
                case 18179:
                {
                    switch (target->getPowerType())
                    {
                        case POWER_RAGE:
                            triggered_spell_id = 85539;
                            break;
                        case POWER_ENERGY:
                            triggered_spell_id = 85540;
                            break;
                        case POWER_RUNIC_POWER:
                            triggered_spell_id = 85541;
                          break;
                        case POWER_FOCUS:
                            triggered_spell_id = 85542;
                            break;
                        default: return false;
                    }
                    basepoints0 = dummySpell->Effects[1].BasePoints;
                    break;
                }
                // Improved Soul Fire
                case 18119:
                case 18120:
                {
                    triggered_spell_id = 85383;
                    basepoints0 = triggerAmount;
                    CastSpell(this, 85385, true);
                    break;
                }
                // Burning Embers
                case 85112:
                case 91986:
                {
                    if (effIndex != 0)
                        return false;

                    Unit* caster = GetOwner() ? GetOwner() : this;
                    if (!caster)
                        return false;

                    triggered_spell_id = 85421;
                    basepoints0 = int32(CalculatePct(damage, triggerAmount) / 7);
                    int32 cap = int32((caster->SpellBaseDamageBonusDone(SPELL_SCHOOL_MASK_FIRE) * 1.4 + 141) / 7);
                    if (dummySpell->Id == 91986)
                        cap /= 2;
                    if (AuraEffect * aurEff = target->GetAuraEffect(triggered_spell_id, 0, caster->GetGUID()))
                        basepoints0 += aurEff->GetAmount();
                    basepoints0 = std::min(cap, basepoints0);
                    // Pet and owner should have the same aura
                    caster->CastCustomSpell(target, triggered_spell_id, &basepoints0, NULL, NULL, true);
                    return true;
                }
                // Glyph of Shadowburn
                case 56229:
                {
                    if (procSpell->ProcFlags & PROC_FLAG_KILL || HasAura(91001))
                        return false;

                    ToPlayer()->RemoveSpellCooldown(17877, true);
                    triggered_spell_id = 91001;
                    break;
                }
                // Glyph of Shadowflame
                case 63310:
                {
                    triggered_spell_id = 63311;
                    break;
                }
                // Nightfall
                case 18094:
                case 18095:
                // Glyph of corruption
                case 56218:
                {
                    target = this;
                    triggered_spell_id = 17941;
                    break;
                }
                // Shadowflame (Voidheart Raiment set bonus)
                case 37377:
                {
                    triggered_spell_id = 37379;
                    break;
                }
                // Pet Healing (Corruptor Raiment or Rift Stalker Armor)
                case 37381:
                {
                    target = GetGuardianPet();
                    if (!target)
                        return false;

                    // heal amount
                    basepoints0 = CalculatePct(int32(damage), triggerAmount);
                    triggered_spell_id = 37382;
                    break;
                }
                // Shadowflame Hellfire (Voidheart Raiment set bonus)
                case 39437:
                {
                    triggered_spell_id = 37378;
                    break;
                }
                // Glyph of Seduction
                case 56250:
                {
                    if (!target)
                        return false;
                    target->RemoveDotsNotSWD();
                    return true;
                }
            }
            break;
        }
        case SPELLFAMILY_PRIEST:
        {
            // Vampiric Touch
            if (dummySpell->SpellFamilyFlags[1] & 0x00000400)
            {
                if (!victim || !victim->isAlive())
                    return false;

                if (effIndex != 0)
                    return false;

                // victim is caster of aura
                if (triggeredByAura->GetCasterGUID() != victim->GetGUID())
                    return false;

                // Energize 1% of max. mana
                victim->CastSpell(victim, 57669, true, castItem, triggeredByAura);
                return true;                                // no hidden cooldown
            }
            switch (dummySpell->SpellIconID)
            {
                // Divine Aegis
                case 2820:
                {
                    if (!target)
                        return false;

                    // Procs from Prayer of Healing or crit effect of heals
                    if (procSpell->Id != 596 && !(procEx & PROC_EX_CRITICAL_HIT))
                        return false;

                    basepoints0 = CalculatePct(int32(damage), triggerAmount);
                    if (AuraEffect const* shieldDiscipline = GetDummyAuraEffect(SPELLFAMILY_HUNTER, 566, 0))
                        basepoints0 *= (shieldDiscipline->GetAmount() + 100.0f) / 100.0f;

                    // double basepoints from crit for Prayer of Healing
                    if (procSpell->Id == 596 && (procEx & PROC_EX_CRITICAL_HIT))
                        basepoints0 *= 2;

                    // Multiple effects stack, so let's try to find this aura.
                    if (AuraEffect *aurEff = target->GetAuraEffect(47753, 0))
                        basepoints0 += aurEff->GetAmount();

                    // Limit up to 40%
                    if (basepoints0 > int32(GetMaxHealth()*40/100))
                        basepoints0 = int32(GetMaxHealth()*40/100);

                    triggered_spell_id = 47753;
                    break;
                }
                // Body and Soul
                case 2218:
                {
                    // Proc only from Cure desease on self cast
                    switch(procSpell->Id)
                    {
                        case 528: // Cure desease
                        {
                            if (victim != this || !roll_chance_i(triggerAmount) || effIndex != 1)
                                return false;
                            triggered_spell_id = 64136;
                            target = this;
                            break;
                        }
                        case 17:    // Power Word: Shield
                        case 73325: // Leap of Faith
                        {
                            triggered_spell_id = dummySpell->Id == 64127 ? 64128: 65081; // first-second rank handling
                            break;
                        }
                    }
                    break;
                }
                // Atonement
                case 4938:
                {
                    basepoints0 = damage * triggerAmount / 100.0f;
                    target = victim;
                    triggered_spell_id = 81751;
                    break;
                }
            }
            switch (dummySpell->Id)
            {
                // Shadowflame, Item - Priest T12 Shadow 2P Bonus
                case 99155:
                {
                    if (!GetOwner())
                        return false;

                    if (!(GetOwner()->GetShapeshiftForm() == FORM_SHADOW) || !GetOwner()->HasAura(99154))
                        return false;

                    target = victim;
                    basepoints0 = int32(CalculatePct(damage, 20));
                    triggered_spell_id = 99156;
                    break;
                }
                // Echo of Light
                case 77485:
                {
                    if (effIndex != 0)
                        return false;

                    basepoints0 = int32(CalculatePct(damage, triggerAmount) / 6.0f);
                    triggered_spell_id = 77489;
                    basepoints0 += victim->GetRemainingPeriodicAmount(GetGUID(), triggered_spell_id, SPELL_AURA_PERIODIC_HEAL);
                    break;
                }
                // Phantasm
                case 47570:
                case 47569:
                {
                    RemoveMovementImpairingAuras();
                    break;
                }
                // Glyph of Spirit Tap
                case 63237:
                {
                    if (victim->GetHealth() > damage)
                        return false;

                    triggered_spell_id = 81301;
                    break;
                }
                // Sin and Punishment
                case 87099:
                case 87100:
                {
                    if (effIndex != EFFECT_1)
                        return false;

                    ToPlayer()->ReduceSpellCooldown(34433, (triggerAmount * 1000));
                    break;
                }
                // Holy Walk
                case 33333:
                {
                    if (target == this)
                        triggered_spell_id = 96219;
                    break;
                }
                // Train of Thought
                case 92295:
                case 92297:
                {
                    if (GetTypeId() != TYPEID_PLAYER)
                        return false;

                    if (procSpell->Id == 585)
                        ToPlayer()->ReduceSpellCooldown(47540, 500);
                    else
                        ToPlayer()->ReduceSpellCooldown(89485, 5000);
                    break;
                }
                // Shadow Orbs
                case 95740:
                {
                    uint32 chance = 10;
                    if (AuraEffect* aurEff = GetAuraEffect(SPELL_AURA_ADD_FLAT_MODIFIER, SPELLFAMILY_PRIEST, 554, 0))
                        chance += aurEff->GetAmount();
                    if (!roll_chance_i(chance))
                        return false;
                    break;
                }
                // Chakra
                case 14751:
                {
                    if (!procSpell)
                        return false;

                    target = this;

                    switch (procSpell->Id)
                    {
                        case 585: // Smite
                        case 73510: // Mind Spike
                            triggered_spell_id = 81209;
                            break;
                        case 2050: // Heal
                        case 2060: // Greater Heal
                        case 2061: // Flash Heal
                        case 32546: // Binding heal
                            triggered_spell_id = 81208;
                            break;
                        case 596: // Prayer of Healing
                        case 33076: // Prayer of Mending
                            triggered_spell_id = 81206;
                            break;
                        default:
                            return false;
                    }
                    break;
                }
                // Evangelism  Rank 1
                case 81659: 
                {
                    if (procSpell->Id == 15407)
                        triggered_spell_id = 87117;
                    else if (!(procFlag & PROC_FLAG_DONE_PERIODIC)) // for holy fire dot
                        triggered_spell_id = 81660;
                    break;
                }
                // Evangelism  Rank 2
                case 81662: 
                {
                    if (procSpell->Id == 15407)
                        triggered_spell_id = 87118;
                    else if (!(procFlag & PROC_FLAG_DONE_PERIODIC)) // for holy fire dot
                        triggered_spell_id = 81661;
                    break;
                }
                // Vampiric Embrace
                case 15286:
                {
                    if (!victim || !victim->isAlive() || procSpell->SpellFamilyFlags[1] & 0x80000)
                        return false;

                    // heal amount
                    int32 self = CalculatePct(int32(damage), triggerAmount);
                    int32 team = CalculatePct(int32(damage), triggerAmount / 2);
                    CastCustomSpell(this, 15290, &team, &self, NULL, true, castItem, triggeredByAura);
                    return true;                                // no hidden cooldown
                }
                // Priest Tier 6 Trinket (Ashtongue Talisman of Acumen)
                case 40438:
                {
                    // Shadow Word: Pain
                    if (procSpell->SpellFamilyFlags[0] & 0x8000)
                        triggered_spell_id = 40441;
                    // Renew
                    else if (procSpell->SpellFamilyFlags[0] & 0x40)
                        triggered_spell_id = 40440;
                    else
                        return false;

                    target = this;
                    break;
                }
                // Glyph of Prayer of Healing
                case 55680:
                {
                    triggered_spell_id = 56161;

                    SpellInfo const* GoPoH = sSpellMgr->GetSpellInfo(triggered_spell_id);
                    if (!GoPoH)
                        return false;

                    int32 tickcount = GoPoH->GetMaxDuration() / GoPoH->Effects[EFFECT_0].Amplitude;
                    basepoints0 = CalculatePct(int32(damage), triggerAmount) / tickcount;
                    basepoints0 += victim->GetRemainingPeriodicAmount(GetGUID(), triggered_spell_id, SPELL_AURA_PERIODIC_HEAL);
                    break;
                }
                // Oracle Healing Bonus ("Garments of the Oracle" set)
                case 26169:
                {
                    // heal amount
                    basepoints0 = int32(CalculatePct(damage, 10));
                    target = this;
                    triggered_spell_id = 26170;
                    break;
                }
                // Frozen Shadoweave (Shadow's Embrace set) warning! its not only priest set
                case 39372:
                {
                    if (!procSpell || (procSpell->GetSchoolMask() & (SPELL_SCHOOL_MASK_FROST | SPELL_SCHOOL_MASK_SHADOW)) == 0)
                        return false;

                    // heal amount
                    basepoints0 = CalculatePct(int32(damage), triggerAmount);
                    target = this;
                    triggered_spell_id = 39373;
                    break;
                }
                // Greater Heal (Vestments of Faith (Priest Tier 3) - 4 pieces bonus)
                case 28809:
                {
                    triggered_spell_id = 28810;
                    break;
                }
                // Priest T10 Healer 2P Bonus
                case 70770:
                    // Flash Heal
                    if (procSpell->SpellFamilyFlags[0] & 0x800)
                    {
                        triggered_spell_id = 70772;
                        SpellInfo const* blessHealing = sSpellMgr->GetSpellInfo(triggered_spell_id);
                        if (!blessHealing)
                            return false;
                        basepoints0 = int32(CalculatePct(damage, triggerAmount) / (blessHealing->GetMaxDuration() / blessHealing->Effects[0].Amplitude));
                    }
                    break;
                // Shadowy Apparition
                case 78202:
                case 78203:
                case 78204:
                    if (Aura* aur = GetAura(dummySpell->Id))
                    {
                        int32 chance = aur->GetEffect(0)->GetAmount();
                        if (isMoving())
                            chance *= 5;
                        if (effIndex !=0 || !procSpell || !roll_chance_i(chance))
                            return false;

                        std::list<Creature*> summons;
                        GetAllMinionsByEntry(summons, 46954);
                        if (summons.size() > 3)
                            return false;

                        int32 bp0 = 1;
                        CastCustomSpell(this, 87426, &bp0, NULL, NULL, true);

                        std::list<Creature*> new_summons;
                        GetAllMinionsByEntry(new_summons, 46954);

                        Unit* summon = NULL;
                        for (std::list<Creature*>::iterator new_itr = new_summons.begin(); new_itr != new_summons.end(); ++new_itr)
                        {
                            summon = NULL;
                            for (std::list<Creature*>::iterator itr = summons.begin(); itr != summons.end(); ++itr)
                                if ((*new_itr)->GetGUID() == (*itr)->GetGUID())
                                    summon = *new_itr;
                            if (!summon)
                            {
                                summon = *new_itr;
                                break;
                            }
                        }
                        if (summon)
                        {
                            //summon->m_FollowingRefManager.clearReferences();
                            CastSpell(summon, 87213, true);
                            summon->CastSpell(summon, 87427, true);
                            summon->GetAI()->AttackStart(victim);
                        }
                    }
                    break;
            }
            break;
        }
        case SPELLFAMILY_DRUID:
        {
            switch (dummySpell->Id)
            {
                // Sudden Eclipse - PVP Druid Set Bonus
                case 46832:
                {   
                    // disabled if has eclipse
                    if (HasAura(48518) || HasAura(48517))
                        return false;

                    triggered_spell_id = 95746;

                    if ((!HasAura(67484) && HasAura(67483)))
                    {
                        basepoints0 = 20;

                        if (!HasAura(67483))
                            CastSpell(this, 67483, true);
                    }
                    else
                    {
                        basepoints0 = -13;

                        if (!HasAura(67484))
                            CastSpell(this, 67484, true);
                    }

                    target = victim;
                    break;
                }
                // Item - Shaman T12 Enhancement 4P Bonus
                case 99213:
                    triggered_spell_id = 99212;
                    target = victim;
                    break;
                // Item - Rogue T12 2P Bonus
                case 99174:
                {
                    triggerAmount = 3;
                    basepoints0 = CalculatePct(damage, triggerAmount);
                    triggered_spell_id = 99173;
                    basepoints0 += victim->GetRemainingPeriodicAmount(GetGUID(), triggered_spell_id, SPELL_AURA_PERIODIC_DAMAGE);
                    break;
                }
                // Item  Druid T12 Restoration 4P Bonus
                case 99015:
                {
                    if (!victim || !victim->ToPlayer())
                        return false;
                    
                    Player* plr = victim->ToPlayer();
                    if (!plr)
                        return false;

                    
                    std::list<Player*> plrList;
                    Trinity::AnyFriendlyUnitInObjectRangeCheck check(this, this, 15.0f);
                    Trinity::PlayerListSearcher<Trinity::AnyFriendlyUnitInObjectRangeCheck> searcher(this, plrList, check);
                    VisitNearbyObject(15.0f, searcher);
                    if (plrList.empty())
                        return false;

                    plrList.remove(plr);

                    if (plrList.empty())
                        return false;

                    plrList.sort(Trinity::HealthPctOrderPred());
                    plrList.resize(1);

                    int32 bp0 = damage;

                    CastCustomSpell(plrList.front(), 99017, &bp0, 0, 0, true);
                    return true;
                }
                // Item - Druid T12 Feral 4P Bonus
                case 99009:
                {
                    if (GetTypeId() != TYPEID_PLAYER)
                        return false;

                    uint32 ui_chance = 20 * ToPlayer()->GetComboPoints();
                    if (!roll_chance_i(ui_chance))
                        return false;

                    if (Aura* aur = GetAura(50334))
                        aur->SetDuration(aur->GetDuration() + 2000);
                    break;
                }
                // Item  Druid T12 Feral 2P Bonus
                case 99001:
                    triggerAmount /= 2;
                    basepoints0 = int32(CalculatePct(damage, triggerAmount));
                    triggered_spell_id = 99002;
                    basepoints0 += victim->GetRemainingPeriodicAmount(GetGUID(), triggered_spell_id, SPELL_AURA_PERIODIC_DAMAGE);
                    target = victim;
                    break;
                // Nature's Ward
                case 33881:
                case 33882:
                {
                    if (!HealthBelowPct(50) && !this->HealthBelowPctDamaged(50, damage))
                        return false;

                    target = this;
                    triggered_spell_id = 774;

                    break;
                }
                // Empowered Touch
                case 33879:
                case 33880:
                {
                    triggered_spell_id = 88433;
                    target = victim;
                    break;
                }
                // Glyph of Healing Touch
                case 54825:
                {
                    ToPlayer()->SpellCooldownReduction(17116, 10000);
                    break;
                }
                // Glyph of Innervate
                case 54832:
                {
                    if (procSpell->SpellIconID != 62 || victim == this)
                        return false;

                    triggered_spell_id = 54833;
                    target = this;
                    break;
                }

                // Glyph of Starfire
                case 54845:
                {
                    triggered_spell_id = 54846;
                    break;
                }
                // Glyph of Bloodletting
                case 54815:
                {
                    if (!target)
                        return false;

                    // try to find spell Rip on the target
                    if (AuraEffect const* AurEff = target->GetAuraEffect(SPELL_AURA_PERIODIC_DAMAGE, SPELLFAMILY_DRUID, 0x00800000, 0x0, 0x0, GetGUID()))
                    {
                        Aura* aura = AurEff->GetBase();
                        // Rip's max duration, note: spells which modifies Rip's duration also counted
                        uint32 CountMin = aura->GetMaxDuration();

                        // just Rip's max duration without other spells
                        uint32 CountMax = AurEff->GetSpellInfo()->GetMaxDuration();

                        // add possible auras' and Glyph of Shred's max duration
                        CountMax += 3 * triggerAmount * IN_MILLISECONDS;      // Glyph of Bloodletting        -> +6 seconds
                        CountMax += HasAura(60141) ? 4 * IN_MILLISECONDS : 0; // Rip Duration/Lacerate Damage -> +4 seconds

                        // if min < max -> that means caster didn't cast 3 shred yet
                        // so set Rip's duration and max duration
                        if (CountMin < CountMax)
                        {
                            aura->SetDuration(aura->GetDuration() + triggerAmount * IN_MILLISECONDS);
                            aura->SetMaxDuration(CountMin + triggerAmount * IN_MILLISECONDS);
                            return true;
                        }
                    }
                    // if not found Rip
                    return false;
                }
                // Glyph of Starsurge
                case 62971:
                {
                    ToPlayer()->SpellCooldownReduction(48505, 5000);
                    return true;
                }
                // Leader of the Pack
                case 17007:
                {
                    if (GetTypeId() != TYPEID_PLAYER)
                        return false;

                    if (ToPlayer()->HasSpellCooldown(68285))
                        return false;
                    
                    if (triggerAmount <= 0)
                        return false;
                    
                    basepoints0 = int32(CountPctFromMaxHealth(4));
                    target = this;
                    triggered_spell_id = 34299;

                    int32 mana = CalculatePct(GetCreateMana(), triggerAmount);
                    
                    CastCustomSpell(this, 68285, &mana, NULL, NULL, true, castItem, triggeredByAura);
                    ToPlayer()->AddSpellCooldown(68285, 0, time(NULL) + 6);
                    break;
                }
                // Healing Touch (Dreamwalker Raiment set)
                case 28719:
                {
                    // mana back
                    basepoints0 = int32(CalculatePct(procSpell->ManaCost, 30));
                    target = this;
                    triggered_spell_id = 28742;
                    break;
                }
                // Healing Touch Refund (Idol of Longevity trinket)
                case 28847:
                {
                    target = this;
                    triggered_spell_id = 28848;
                    break;
                }
                // Mana Restore (Malorne Raiment set / Malorne Regalia set)
                case 37288:
                case 37295:
                {
                    target = this;
                    triggered_spell_id = 37238;
                    break;
                }
                // Druid Tier 6 Trinket
                case 40442:
                {
                    float  chance;

                    // Starfire
                    if (procSpell->SpellFamilyFlags[0] & 0x4)
                    {
                        triggered_spell_id = 40445;
                        chance = 25.0f;
                    }
                    // Rejuvenation
                    else if (procSpell->SpellFamilyFlags[0] & 0x10)
                    {
                        triggered_spell_id = 40446;
                        chance = 25.0f;
                    }
                    // Mangle (Bear) and Mangle (Cat)
                    else if (procSpell->SpellFamilyFlags[1] & 0x00000440)
                    {
                        triggered_spell_id = 40452;
                        chance = 40.0f;
                    }
                    else
                        return false;

                    if (!roll_chance_f(chance))
                        return false;

                    target = this;
                    break;
                }
                // Maim Interrupt
                case 44835:
                {
                    // Deadly Interrupt Effect
                    triggered_spell_id = 32747;
                    break;
                }
                // Glyph of Regrowth
                case 54743:
                {
                    if (GetHealthPct() < 50.0f)
                    {
                        if (Aura * aur = victim->GetAura(procSpell->Id, GetGUID()))
                            aur->RefreshDuration(false);
                    }

                    return true;
                }
                // Item - Druid T10 Balance 4P Bonus
                case 70723:
                {
                    // Wrath & Starfire
                    if ((procSpell->SpellFamilyFlags[0] & 0x5) && (procEx & PROC_EX_CRITICAL_HIT))
                    {
                        triggered_spell_id = 71023;
                        SpellInfo const* triggeredSpell = sSpellMgr->GetSpellInfo(triggered_spell_id);
                        if (!triggeredSpell)
                            return false;
                        basepoints0 = CalculatePct(int32(damage), triggerAmount) / (triggeredSpell->GetMaxDuration() / triggeredSpell->Effects[0].Amplitude);
                        // Add remaining ticks to damage done
                        basepoints0 += victim->GetRemainingPeriodicAmount(GetGUID(), triggered_spell_id, SPELL_AURA_PERIODIC_DAMAGE);
                    }
                    break;
                }
                // Item - Druid T10 Restoration 4P Bonus (Rejuvenation)
                case 70664:
                {
                    // Proc only from normal Rejuvenation
                    if (procSpell->SpellVisual[0] != 32)
                        return false;

                    Player* caster = ToPlayer();
                    if (!caster)
                        return false;
                    if (!caster->GetGroup() && victim == this)
                        return false;

                    CastCustomSpell(70691, SPELLVALUE_BASE_POINT0, damage, victim, true);
                    return true;
                }
            }
            // Living Seed
            if (dummySpell->SpellIconID == 2860)
            {
                triggered_spell_id = 48504;
                basepoints0 = CalculatePct(int32(damage), triggerAmount);
                break;
            }
            break;
        }
        case SPELLFAMILY_ROGUE:
        {
            switch (dummySpell->Id)
            {
                // Improved Expose Armor
                case 14168:
                case 14169:
                {
                    if (GetTypeId() != TYPEID_PLAYER)
                        break;

                    target = victim;
                    triggered_spell_id = 79128;
                    basepoints0 = ToPlayer()->GetComboPoints();
                    break;
                }
                // Honor Among Thieves
                case 51698:
                case 51700:
                case 51701:
                {
                    if (!ToPlayer() || ToPlayer()->HasSpellCooldown(51699))
                        return false;

                    if (Unit* caster = triggeredByAura->GetCaster())
                    {
                        if (caster->GetTypeId() != TYPEID_PLAYER)
                            return false;

                        if (caster->ToPlayer()->HasSpellCooldown(51699))
                            return false;

                        target = NULL;
                        if (caster->ToPlayer()->GetComboPoints())
                            if (Unit* target2 = ObjectAccessor::GetUnit(*caster, caster->ToPlayer()->GetComboTarget()))
                                target = target2;
                        if (!target)
                            if (Unit* target3 = caster->ToPlayer()->GetSelectedUnit())
                            {
                                if (target3->IsHostileTo(caster))
                                    target = target3;
                            }

                        caster->CastSpell(target? target: victim, 51699, true);

                        cooldown = 4;
                        if (AuraEffect const* auraEff = caster->GetAuraEffect(SPELL_AURA_ADD_FLAT_MODIFIER, SPELLFAMILY_ROGUE, 2903, 2))
                            cooldown += auraEff->GetAmount() / 1000;
                        caster->ToPlayer()->AddSpellCooldown(51699, 0, time(NULL) + cooldown);
                    }

                    break;
                }
                // Deadly Brew
                case 51625:
                case 51626:
                    triggered_spell_id =3409;
                    break;
                // Venomous Wounds
                case 79134:
                case 79133:
                {
                    if (effIndex != 0)
                        return false;

                    int32 energy = triggeredByAura->GetBase()->GetEffect(1)->GetAmount();
                    CastCustomSpell(this, 51637, &energy, 0, 0, true);

                    triggered_spell_id = 79136;

                    break;
                }
                // Revealing Strike
                case 84617:
                    return triggeredByAura->GetCaster() == victim;

                // Bandit's Guile
                case 84652:
                case 84653:
                case 84654:
                {
                    if (effIndex != 0)
                        return false;

                    static const uint32 spells[] = {84745, 84746, 84747};
                    int32 basepoints00 = 0; // damage modifier
                    AuraEffect* banditsGuile = 0;

                    // read last count
                    int32 count = 1; // Sinister Strike/Revealing Strike count
                    if (banditsGuile = victim->GetAuraEffect(84748, 1, GetGUID()))
                        count += triggeredByAura->GetBase()->GetEffect(2)->GetAmount();

                    if (!banditsGuile)
                    {
                        for (int x = 0; x < 3; ++x)
                            RemoveAurasDueToSpell(spells[x]);
                        count = 0;
                    }
                    else
                    {
                        // find Insight aura
                        int i = 0;
                        for (i = 0; i < 3; ++i)
                            if (AuraEffect* moderateInsight = GetAuraEffect(spells[i], 0))
                            {
                                basepoints00 = moderateInsight->GetAmount();
                                break;
                            }
                            // "upgrade" Insight aura
                            if (count >= 4)
                            {
                                if (i == 2)
                                    return false;
                                count = 0;
                                basepoints00 += 10;
                                if (i == 3)
                                    triggered_spell_id = spells[0];
                                else
                                {
                                    RemoveAurasDueToSpell(spells[i]);
                                    triggered_spell_id = spells[i+1];
                                }
                            }
                    }
                    triggeredByAura->GetBase()->GetEffect(2)->SetAmount(count);
                    CastCustomSpell(target, 84748, &basepoints00, &basepoints00, 0, true);
                    break;
                }
                // Restless Blades
                case 79095:
                case 79096:
                {
                    if (!ToPlayer())
                        return false;

                    if (procSpell->DmgClass == SPELL_DAMAGE_CLASS_MELEE && procSpell->AttributesEx & SPELL_ATTR1_REQ_COMBO_POINTS1 && victim != this)
                    {
                        // better not to use cycle here otherwise the time is not always reduced
                        uint32 reductionTime = triggerAmount * ToPlayer()->GetComboPoints();
                        // Sprint
                        ToPlayer()->ReduceSpellCooldown(2983, reductionTime);
                        // Adrenaline Rush
                        ToPlayer()->ReduceSpellCooldown(13750, reductionTime);
                        // Killing Spree
                        ToPlayer()->ReduceSpellCooldown(51690, reductionTime);
                        // Redirect
                        ToPlayer()->ReduceSpellCooldown(73981, reductionTime);
                        return true;
                    }
                    break;
                }
                // Glyph of Hemorrhage
                case 56807:
                {
                    triggered_spell_id = 89775;
                    SpellInfo const* hemorrhageDot = sSpellMgr->GetSpellInfo(triggered_spell_id);
                    int32 tickcount = hemorrhageDot->GetMaxDuration() / hemorrhageDot->Effects[0].Amplitude;
                    basepoints0 = triggerAmount * damage / tickcount / 100;
                    break;
                }
                // Main Gauche
                case 76806:
                {
                    if (!victim || effIndex != 0)
                        return false;

                    if (procSpell && !(procSpell->SpellFamilyFlags & dummySpell->Effects[0].SpellClassMask))
                        return false;

                    if (!roll_chance_i(triggeredByAura->GetAmount()))
                        return false;

                    triggered_spell_id = 86392;
                    break;
                }
                // Serrated Blades
                case 14171:
                case 14172:
                {
                     float chance = triggerAmount * ToPlayer()->GetComboPoints();
                     if (roll_chance_f(chance))
                     {
                         if (Aura* Rupture = victim->GetAura(1943, GetGUID()))
                             Rupture->RefreshDuration();
                     }
                     break;
                }
                case 32748: // Deadly Throw Interrupt
                    // Prevent cast Deadly Throw Interrupt on self from last effect (apply dummy) of Deadly Throw
                    if (this == victim)
                        return false;

                    triggered_spell_id = 32747;
                    break;
                case 57934: // Tricks of the Trade
                {
                    if (GetTypeId() != TYPEID_PLAYER)
                        return false;

                    Unit* redirectTarget = GetMisdirectionTarget();
                    RemoveAura(57934);

                    // Item - Rogue T12 4P Bonus
                    if (HasAura(99175))
                    {
                        uint32 spellIds[3] = {99186, 99187, 99188}; 
                        uint32 crIds[3] = {CR_HASTE_MELEE, CR_CRIT_MELEE, CR_MASTERY};
                        uint32 i = urand(0, 2);
                        int32 bp0 = int32(CalculatePct(GetUInt32Value(PLAYER_FIELD_COMBAT_RATING_1 + crIds[i]), 25));
                        CastCustomSpell(this, spellIds[i], &bp0, 0, 0, true);
                    }
                    // Item - Rogue T13 2P Bonus
                    if (HasAura(105849))
                        CastSpell(this, 105864, true);

                    if (!redirectTarget)
                        break;
                    CastSpell(this,59628,true);
                    CastSpell(redirectTarget,57933,true);
                    break;
                }
                // Glyph of Kick
                case 56805:
                {
                    ToPlayer()->SpellCooldownReduction(1766, triggerAmount);
                    return true;
                }
                // Cut to the Chase
                case 51664:
                case 51665:
                case 51667:
                    if (Aura* aur = GetAura(5171))
                    {
                        uint32 max_duration = aur->GetSpellInfo()->GetMaxDuration();
                        aur->SetMaxDuration(max_duration);
                        aur->SetDuration(max_duration, true);
                        return true;
                    }
                    return false;
            }
            break;
        }
        case SPELLFAMILY_HUNTER:
        {
            switch (dummySpell->SpellIconID)
            {
                case 267: // Improved Mend Pet
                {
                    if (!roll_chance_i(triggerAmount))
                        return false;

                    triggered_spell_id = 24406;
                    break;
                }
                case 2236: // Thrill of the Hunt
                {
                    if (!procSpell)
                        return false;

                    basepoints0 = CalculatePct(procSpell->ManaCost, triggerAmount);

                    if (basepoints0 <= 0)
                        return false;

                    target = this;
                    triggered_spell_id = 34720;
                    break;
                }
                // Crouching Tiger, Hidden Chimera
                case 4752:
                {
                    // custom cooldown processing case
                    if (!ToPlayer() || (cooldown && ToPlayer()->HasSpellCooldown(dummySpell->Id)))
                        return false;

                    // Reduct Disengage cooldown
                    if (procFlag & (PROC_FLAG_TAKEN_MELEE_AUTO_ATTACK | PROC_FLAG_TAKEN_SPELL_MELEE_DMG_CLASS))
                    {
                        if (AuraEffect const* aurEff = this->GetAuraEffect(SPELL_AURA_DUMMY, SPELLFAMILY_HUNTER, 4752, 0))
                            ToPlayer()->SpellCooldownReduction(781, aurEff->GetAmount());
                    }
                    // Reduct Deterrence cooldown
                    else if (procFlag & (PROC_FLAG_TAKEN_RANGED_AUTO_ATTACK | PROC_FLAG_TAKEN_SPELL_RANGED_DMG_CLASS | PROC_FLAG_TAKEN_SPELL_MAGIC_DMG_CLASS_NEG))
                    {
                        if (AuraEffect const* aurEff = this->GetAuraEffect(SPELL_AURA_DUMMY, SPELLFAMILY_HUNTER, 4752, 1))
                            ToPlayer()->SpellCooldownReduction(19263, aurEff->GetAmount());
                    }
                    ToPlayer()->AddSpellCooldown(dummySpell->Id, 0, time(NULL) + cooldown);
                    return true;
                }
                // Improved Steady Shot
                case 3409:
                {
                    if (procSpell->Id != 56641) // not steady shot
                    {
                        if (!(procEx & (PROC_EX_INTERNAL_TRIGGERED|PROC_EX_INTERNAL_CANT_PROC))) // shitty procs
                            triggeredByAura->GetBase()->SetCharges(0);
                        return false;
                    }

                    // wtf bug
                    if (this == target)
                        return false;

                    if (triggeredByAura->GetBase()->GetCharges() <= 1)
                    {
                        triggeredByAura->GetBase()->SetCharges(2);
                        return true;
                    }
                    triggeredByAura->GetBase()->SetCharges(0);
                    triggered_spell_id = 53220;
                    basepoints0 = triggerAmount;
                    target = this;
                    break;
                }
                // Marked for Death
                case 3524:
                {
                    // Get old Hunter's Mark
                    if (Aura * aura = target->GetAura(1130)) 
                    {
                        if (aura->GetDuration() > 15000)
                            return false;
                    }
                    triggered_spell_id = 88691;
                    break;
                }
            }

            switch (dummySpell->Id)
            {
                // Killing Streak
                case 82748:
                case 82749:
                {
                    if (effIndex != 0)
                        return false;

                    if (procEx & PROC_EX_CRITICAL_HIT)
                    {
                        if (triggeredByAura->GetBase()->GetCharges() <= 1)
                        {
                            triggeredByAura->GetBase()->SetCharges(2);
                            return true;
                        }
                    }
                    else
                    {
                        triggeredByAura->GetBase()->SetCharges(0);
                        return false;
                    }
                    triggeredByAura->GetBase()->SetCharges(0);
                    GetProcOwner()->CastSpell(GetProcOwner(), (dummySpell->Id == 82748 ? 94006 : 94007), true);
                    return true;
                }
                // Roar of Sacrifice
                case 53480:
                {
                    target = triggeredByAura->GetCaster();
                    if (!target || !damage)
                        return false;
                    basepoints0 = damage / 10;
                    triggered_spell_id = 67481;
                    break;
                }
                // Aspect of the Fox
                case 82661:     
                {
                    target = this;
                    basepoints0 = triggerAmount;

                    // One with Nature
                    if (AuraEffect * aurEff = GetAuraEffect(SPELL_AURA_DUMMY, SPELLFAMILY_HUNTER, 5080, 1))
                        basepoints0 += aurEff->GetAmount();

                    triggered_spell_id = 82716;
                    break;
                }
                // Misdirection
                case 34477:
                {
                    if (!GetMisdirectionTarget())
                        return false;
                    triggered_spell_id = 35079; // 4 sec buff on self
                    target = this;
                    break;
                }
                // Wild Quiver (Mastery Marksmanship Hunter)
                case 76659:
                {
                    if (!victim || effIndex != 0)
                        return false;

                    if (!roll_chance_i(triggerAmount))
                        return false;

                    // It mustn't be triggered from Scatter Shot
                    if (procSpell && procSpell->Id == 19503)
                        return false;

                    triggered_spell_id = 76663;
                    basepoints0 = 0;
                    break;
                }
                // Kill Command - Resistance is Futile! - energy refund
                case 82897:
                {
                    if (Unit * owner = GetProcOwner())
                        owner->CastSpell(owner, 86316, true);

                    return true;
                }
                // Sic 'Em! (Rank 1)
                case 83340:
                {
                    triggered_spell_id = 83359;
                    target = this;
                    break;
                }
                // Sic 'Em! (Rank 2)
                case 83356:
                {
                    triggered_spell_id = 89388;
                    target = this;
                    break;
                }
            }
            break;
        }
        case SPELLFAMILY_PALADIN:
        {
            switch (dummySpell->Id)
            {
                case 96887: // Variable Pulse Lightning Capacitor
                case 97119: // Variable Pulse Lightning Capacitor (Heroic)
                {
                    if (!victim)
                        return false;

                    if (Aura* aur = GetAura(96890))
                    {
                        uint8 stacks = aur->GetStackAmount();
                        if (roll_chance_i(15))
                        {
                            int32 bp0 = dummySpell->Effects[EFFECT_0].CalcValue() * stacks;
                            CastCustomSpell(victim, 96891, &bp0, 0, 0, true);
                            aur->Remove();
                            return true;
                        }
                    }
                    triggered_spell_id = 96890;
                    target = this;
                    break;
                }
                // Item - Paladin T12 Holy 4P Bonus
                case 99070:
                {
                    if (!victim || !victim->ToPlayer())
                        return false;
                    
                    Player* plr = victim->ToPlayer();
                    if (!plr)
                        return false;

                    
                    std::list<Player*> plrList;
                    Trinity::AnyFriendlyUnitInObjectRangeCheck check(this, this, 15.0f);
                    Trinity::PlayerListSearcher<Trinity::AnyFriendlyUnitInObjectRangeCheck> searcher(this, plrList, check);
                    VisitNearbyObject(15.0f, searcher);
                    if (plrList.empty())
                        return false;

                    plrList.remove(plr);

                    if (plrList.empty())
                        return false;

                    plrList.sort(Trinity::HealthPctOrderPred());
                    plrList.resize(1);

                    int32 bp0 = int32(CalculatePct(damage, 10));

                    CastCustomSpell(plrList.front(), 99017, &bp0, 0, 0, true);
                    return true;
                }
                // Illuminated Healing
                case 76669:
                {
                    if (!victim || !procSpell || effIndex != 0)
                        return false;

                    switch (procSpell->Id)
                    {
                        case 635:
                        case 85673:
                        case 19750:
                        case 82326:
                        case 20473:
                        case 25914:
                        case 82327:
                        case 86452:
                        {
                            basepoints0 = int32(damage * float(triggerAmount / 100.0f));
                            triggered_spell_id = 86273;

                            if (AuraEffect const* aurShield = victim->GetAuraEffect(triggered_spell_id, EFFECT_0, GetGUID()))
                                basepoints0 += aurShield->GetAmount();

                            int32 maxHealth = CountPctFromMaxHealth(33);
                            basepoints0 = std::min(basepoints0, maxHealth);

                            break;
                        }
                    }
                    break;
                }
                // Item - Paladin T12 Protection 2P Bonus
                case 99074:
                    basepoints0 = int32(CalculatePct(damage, triggerAmount));
                    triggered_spell_id = 99075;
                    target = victim;
                    break;
                // Item - Paladin T12 Retribution 2P Bonus
                case 99093:
                    if (!victim || GetGUID() == victim->GetGUID())
                        return false;
                    basepoints0 = int32(CalculatePct(damage, triggerAmount / 2)); // 2 ticks
                    triggered_spell_id = 99092;
                    target = victim;
                    break;
                // Ancient Crusader (player)
                case 86701:
                {
                    if (GetTypeId() != TYPEID_PLAYER)
                        return false;

                    //if caster has no guardian of ancient kings aura then remove dummy aura
                    if (!HasAura(86698))
                    {
                        RemoveAurasDueToSpell(86701);
                        return false;
                    }

                    CastSpell(this, 86700, true);
                    return true;
                }
                // Ancient Crusader (guardian)
                case 86703:
                {
                    if (!GetOwner() || GetOwner()->GetTypeId() != TYPEID_PLAYER)
                        return false;

                    GetOwner()->CastSpell(this, 86700, true);
                    return true;
                }
                // Ancient Healer
                case 86674:
                {
                    if (GetTypeId() != TYPEID_PLAYER)
                        return false;

                    // if caster has no guardian of ancient kings aura then remove dummy aura
                    if (!HasAura(86669))
                    {
                        RemoveAurasDueToSpell(86674);
                        return false;
                    }

                    // check for single target spell (TARGET_SINGLE_FRIEND, NO_TARGET)
                    if (!(procSpell->Effects[triggeredByAura->GetEffIndex()].TargetA.GetTarget() == TARGET_UNIT_TARGET_ALLY) &&
                        (procSpell->Effects[triggeredByAura->GetEffIndex()].TargetB.GetTarget() == 0))
                        return false;

                    // Need to get guardian but that's NOT WORK!!!
                    //if (Creature* pet = ObjectAccessor::GetCreatureOrPetOrVehicle(*this, GetPetGUID()))

                    std::list<Creature*> petlist;
                    GetCreatureListWithEntryInGrid(petlist, 46499, 100.0f);
                    if (!petlist.empty())
                        for (std::list<Creature*>::const_iterator itr = petlist.begin(); itr != petlist.end(); ++itr)
                        {
                            Unit* pPet = (*itr);
                            if (pPet->GetOwnerGUID() == GetGUID())
                            {
                                int32 bp0 = damage;
                                int32 bp1 = damage / 10;
                                pPet->CastCustomSpell(victim, 86678, &bp0, &bp1, NULL, true);
                            }
                        }
                    return true;
                }
                // Light's Beacon - Beacon of Light
                case 53651:
                {
                    // Get target of beacon of light
                    if (Unit* beaconTarget = triggeredByAura->GetBase()->GetCaster())
                    {
                        // do not proc when target of beacon of light is healed
                        if (!victim || beaconTarget->GetGUID() == GetGUID())
                            return false;

                        float healing = 1.0f;
                        if (procSpell->Id != 635)
                            healing = 0.5f;
                        // check if it was heal by paladin which casted this beacon of light
                        if (beaconTarget->GetAura(53563, victim->GetGUID()))
                        {
                            if (beaconTarget->IsWithinLOSInMap(victim))
                            {
                                basepoints0 = damage * healing;
                                victim->CastCustomSpell(beaconTarget, 53652, &basepoints0, 0, 0, true);
                                return true;
                            }
                        }
                    }
                    return false;
                }
                // Item - Collecting Mana, Tyrande's Favirite Doll
                case 92272:
                {
                    if (procSpell && procSpell->ManaCostPercentage)
                    {
                        const int32 maxmana = 4200;
                        int32 mana = int32(0.2f * CalculatePct(GetCreateMana(), procSpell->ManaCostPercentage));
                        if (AuraEffect* aurEff = GetAuraEffect(92596, EFFECT_0))
                        {
                            int32 oldamount = aurEff->GetAmount();
                            if (oldamount < maxmana)
                            {
                                int32 newamount = std::min(maxmana, (oldamount + mana));
                                aurEff->ChangeAmount(newamount);
                            }
                        }
                        else
                        {
                            int32 bp = std::min(mana, maxmana);
                            CastCustomSpell(this, 92596, &bp, 0, 0, true);
                        }
                    }
                    break;
                }
                // Guarded by the Light
                case 85646:
                {
                    int32 overheal = int32(damage + GetHealth() - GetMaxHealth());
                    if (overheal > 0)
                        CastCustomSpell(this, 88063, &overheal, 0, 0, true);
                    break;
                }
                // Seal of Righteousness
                case 20154:
                {
                    if (!victim || (GetGUID() == victim->GetGUID()) || effIndex != 0)
                        return false;

                    // Seal of Righteousness - Seals of Command
                    triggered_spell_id = HasAura(85126) ? 101423: 25742;

                    float ap = GetTotalAttackPowerValue(BASE_ATTACK);
                    int32 holy = this->SpellBaseDamageBonusDone(SPELL_SCHOOL_MASK_HOLY);
                    basepoints0 = int32((GetHastedAttackTime(BASE_ATTACK) / 1000.0f) * (0.011f * ap + 0.022f * holy));
                    break;
                }
                // Long Arm of The law
                case 87168:
                case 87172:
                {
                    if (roll_chance_f(triggerAmount) && !this->IsWithinDistInMap(victim, 15.0f))
                    {
                        target = this;
                        triggered_spell_id = 87173;
                        break;
                    }
                    break;
                }
                // Divine Purpose
                case 85117: // rank 1
                case 86172: // rank 2
                {
                    target = this;
                    triggered_spell_id = 90174;
                    break;
                }
                // Selfless Healer
                case 85803:
                case 85804:
                {
                    if (victim == this)
                        return false;

                    target = this;
                    basepoints0 = triggerAmount * GetCurrentSpell(CURRENT_GENERIC_SPELL)->GetPowerCost();
                    triggered_spell_id = 90811;
                    break;
                }
                // Holy Power (Redemption Armor set)
                case 28789:
                {
                    if (!victim)
                        return false;

                    // Set class defined buff
                    switch (victim->getClass())
                    {
                        case CLASS_PALADIN:
                        case CLASS_PRIEST:
                        case CLASS_SHAMAN:
                        case CLASS_DRUID:
                            triggered_spell_id = 28795;     // Increases the friendly target's mana regeneration by $s1 per 5 sec. for $d.
                            break;
                        case CLASS_MAGE:
                        case CLASS_WARLOCK:
                            triggered_spell_id = 28793;     // Increases the friendly target's spell damage and healing by up to $s1 for $d.
                            break;
                        case CLASS_HUNTER:
                        case CLASS_ROGUE:
                            triggered_spell_id = 28791;     // Increases the friendly target's attack power by $s1 for $d.
                            break;
                        case CLASS_WARRIOR:
                            triggered_spell_id = 28790;     // Increases the friendly target's armor
                            break;
                        default:
                            return false;
                    }
                    break;
                }
                // Glyph of Divinity
                case 54939:
                {
                    target = this;
                    triggered_spell_id = 54986;
                    basepoints0 = triggerAmount;
                    break;
                }
                // Seal of Truth
                case 31801:
                {
                    if (effIndex != 0)                       // effect 1, 2 used by seal unleashing code
                        return false;

                    if (this->GetGUID() == victim->GetGUID())
                        return false;

                    triggered_spell_id = 31803;

                    // On target with 5 stacks of Holy Vengeance direct damage is done
                    if (Aura* sealDoT = victim->GetAura(triggered_spell_id, GetGUID()))
                        if (sealDoT->GetStackAmount() == 5)
                            CastSpell(victim, 42463, true);
                    break;
                }
                // Paladin Tier 6 Trinket (Ashtongue Talisman of Zeal)
                case 40470:
                {
                    if (!procSpell)
                        return false;

                    float chance;

                    // Flash of light/Holy light
                    if (procSpell->SpellFamilyFlags[0] & 0xC0000000)
                    {
                        triggered_spell_id = 40471;
                        chance = 15.0f;
                    }
                    // Judgement (any)
                    else if (procSpell->GetSpellSpecific() == SPELL_SPECIFIC_JUDGEMENT)
                    {
                        triggered_spell_id = 40472;
                        chance = 50.0f;
                    }
                    else
                        return false;

                    if (!roll_chance_f(chance))
                        return false;

                    break;
                }
                // Item - Paladin T8 Holy 2P Bonus
                case 64890:
                {
                    triggered_spell_id = 64891;
                    basepoints0 = triggerAmount * damage / 300;
                    break;
                }
                case 71406: // Tiny Abomination in a Jar
                case 71545: // Tiny Abomination in a Jar (Heroic)
                {
                    if (!victim || !victim->isAlive())
                        return false;

                    CastSpell(this, 71432, true, NULL, triggeredByAura);

                    Aura const* dummy = GetAura(71432);
                    if (!dummy || dummy->GetStackAmount() < (dummySpell->Id == 71406 ? 8 : 7))
                        return false;

                    RemoveAurasDueToSpell(71432);
                    triggered_spell_id = 71433;  // default main hand attack
                    // roll if offhand
                    if (Player const* player = ToPlayer())
                        if (player->GetWeaponForAttack(OFF_ATTACK, true) && urand(0, 1))
                            triggered_spell_id = 71434;
                    target = victim;
                    break;
                }
                // Item - Icecrown 25 Normal Dagger Proc
                case 71880:
                {
                    switch (getPowerType())
                    {
                        case POWER_MANA:
                            triggered_spell_id = 71881;
                            break;
                        case POWER_RAGE:
                            triggered_spell_id = 71883;
                            break;
                        case POWER_ENERGY:
                            triggered_spell_id = 71882;
                            break;
                        case POWER_RUNIC_POWER:
                            triggered_spell_id = 71884;
                            break;
                        default:
                            return false;
                    }
                    break;
                }
                // Item - Icecrown 25 Heroic Dagger Proc
                case 71892:
                {
                    switch (getPowerType())
                    {
                        case POWER_MANA:
                            triggered_spell_id = 71888;
                            break;
                        case POWER_RAGE:
                            triggered_spell_id = 71886;
                            break;
                        case POWER_ENERGY:
                            triggered_spell_id = 71887;
                            break;
                        case POWER_RUNIC_POWER:
                            triggered_spell_id = 71885;
                            break;
                        default:
                            return false;
                    }
                    break;
                }
            }
            break;
        }
        case SPELLFAMILY_SHAMAN:
        {
            switch (dummySpell->Id)
            {
                // Resurgence
                case 16180:
                case 16196:
                {
                    // Check Water Shield
                    if (!HasAura(52127))
                        return false;

                    SpellInfo const* spellEnergize = sSpellMgr->GetSpellInfo(101033);
                    if (!spellEnergize)
                        return false;

                    int32 bp0 = spellEnergize->Effects[EFFECT_0].CalcValue(this);

                    switch (procSpell->Id)
                    {
                        case 8004: // Healing Surge
                        case 61295: // Riptide
                        case 73685: // Unleash life
                            bp0 *= 0.6f;
                            break;
                        case 1064: // Chain Heal
                            bp0 *= 0.33f;
                            break;
                    }
                    if (dummySpell->Id == 16180)
                        bp0 /= 2;

                    EnergizeBySpell(this, 101033, bp0, POWER_MANA);

                    break;
                }
                // Lava Surge
                case 77755:
                case 77756:
                {
                    triggered_spell_id = 77762;
                    target = this;
                    break;
                }
                // Feedback
                case 86183:
                case 86184:
                case 86185:
                {
                    if (GetTypeId() != TYPEID_PLAYER)
                        break;

                    int32 bp = -triggerAmount;

                    ToPlayer()->ReduceSpellCooldown(16166, bp);

                    break;
                }
                // Telluric Currents
                case 82984:
                case 82988:
                {
                    basepoints0 = CalculatePct(damage, triggerAmount);
                    target = this;
                    triggered_spell_id = 82987;
                    break;
                }
                // Searing Flame
                case 77655:
                case 77656:
                case 77657:
                {
                    target = victim;
                    triggered_spell_id = 77661;
                    basepoints0 = damage / 5;
                    break;
                }
                // Focused Insight
                case 77794:
                case 77795:
                case 77796:
                {
                    if (Aura* aura = GetAura(dummySpell->Id))
                    {
                        if (!procSpell)
                            break;

                        uint32 cost = CalculatePct(GetCreateMana(), procSpell->ManaCostPercentage);
                        int32 bp0 = int32(CalculatePct(cost, aura->GetEffect(EFFECT_0)->GetAmount()) * -1);
                        int32 bp1 = aura->GetEffect(EFFECT_1)->GetAmount();
                        CastCustomSpell(this, 77800, &bp0, &bp1, &bp1, true);
                    }
                    break;
                }
                // Tidal Waves
                case 51562:
                case 51563:
                case 51564:
                {
                    int32 bp0 = -triggerAmount;
                    int32 bp1 = triggerAmount;
                    CastCustomSpell(this, 53390, &bp0, &bp1, 0, true);
                    break;
                }
                // Totemic Power (The Earthshatterer set)
                case 28823:
                {
                    if (!victim)
                        return false;

                    // Set class defined buff
                    switch (victim->getClass())
                    {
                        case CLASS_PALADIN:
                        case CLASS_PRIEST:
                        case CLASS_SHAMAN:
                        case CLASS_DRUID:
                            triggered_spell_id = 28824;     // Increases the friendly target's mana regeneration by $s1 per 5 sec. for $d.
                            break;
                        case CLASS_MAGE:
                        case CLASS_WARLOCK:
                            triggered_spell_id = 28825;     // Increases the friendly target's spell damage and healing by up to $s1 for $d.
                            break;
                        case CLASS_HUNTER:
                        case CLASS_ROGUE:
                            triggered_spell_id = 28826;     // Increases the friendly target's attack power by $s1 for $d.
                            break;
                        case CLASS_WARRIOR:
                            triggered_spell_id = 28827;     // Increases the friendly target's armor
                            break;
                        default:
                            return false;
                    }
                    break;
                }
                // Lesser Healing Wave (Totem of Flowing Water Relic)
                case 28849:
                {
                    target = this;
                    triggered_spell_id = 28850;
                    break;
                }
                // Windfury Weapon (Passive) 1-8 Ranks
                case 33757:
                {
                    Player* player = ToPlayer();
                    if (!player || !castItem || !castItem->IsEquipped() || !victim || !victim->isAlive())
                        return false;

                    // custom cooldown processing case
                    if (cooldown && player->HasSpellCooldown(dummySpell->Id))
                        return false;

                    if (triggeredByAura->GetBase() && castItem->GetGUID() != triggeredByAura->GetBase()->GetCastItemGUID())
                        return false;

                    WeaponAttackType attType = WeaponAttackType(player->GetAttackBySlot(castItem->GetSlot()));
                    if ((attType != BASE_ATTACK && attType != OFF_ATTACK)
                        || (attType == BASE_ATTACK && procFlag & PROC_FLAG_DONE_OFFHAND_ATTACK)
                        || (attType == OFF_ATTACK && procFlag & PROC_FLAG_DONE_MAINHAND_ATTACK))
                         return false;

                    // Now compute real proc chance...
                    uint32 chance = 20;
                    player->ApplySpellMod(dummySpell->Id, SPELLMOD_CHANCE_OF_SUCCESS, chance);

                    Item* addWeapon = player->GetWeaponForAttack(attType == BASE_ATTACK ? OFF_ATTACK : BASE_ATTACK, true);
                    uint32 enchant_id_add = addWeapon ? addWeapon->GetEnchantmentId(EnchantmentSlot(TEMP_ENCHANTMENT_SLOT)) : 0;
                    SpellItemEnchantmentEntry const* pEnchant = sSpellItemEnchantmentStore.LookupEntry(enchant_id_add);
                    if (pEnchant && pEnchant->spellid[0] == dummySpell->Id)
                        chance += 14;

                    if (!roll_chance_i(chance))
                        return false;

                    // Now amount of extra power stored in 1 effect of Enchant spell
                    uint32 spellId = 8232;
                    SpellInfo const* windfurySpellInfo = sSpellMgr->GetSpellInfo(spellId);
                    if (!windfurySpellInfo)
                        return false;

                    int32 extra_attack_power = CalculateSpellDamage(victim, windfurySpellInfo, 1);

                    // Value gained from additional AP
                    basepoints0 = int32(extra_attack_power / 14.0f * GetAttackTime(attType) / 1000);

                    if (procFlag & PROC_FLAG_DONE_MAINHAND_ATTACK)
                        triggered_spell_id = 25504;

                    if (procFlag & PROC_FLAG_DONE_OFFHAND_ATTACK)
                        triggered_spell_id = 33750;

                    // apply cooldown before cast to prevent processing itself
                    if (cooldown)
                        player->AddSpellCooldown(dummySpell->Id, 0, time(NULL) + cooldown);

                    // attack three time
                    for (uint32 i = 0; i < 3; ++i)
                        CastCustomSpell(victim, triggered_spell_id, &basepoints0, NULL, NULL, true, castItem, triggeredByAura);

                    return true;
                }
                // Shaman Tier 6 Trinket
                case 40463:
                {
                    if (!procSpell)
                        return false;

                    float chance;
                    if (procSpell->SpellFamilyFlags[0] & 0x1)
                    {
                        triggered_spell_id = 40465;         // Lightning Bolt
                        chance = 15.0f;
                    }
                    else if (procSpell->SpellFamilyFlags[0] & 0x80)
                    {
                        triggered_spell_id = 40465;         // Lesser Healing Wave
                        chance = 10.0f;
                    }
                    else if (procSpell->SpellFamilyFlags[1] & 0x00000010)
                    {
                        triggered_spell_id = 40466;         // Stormstrike
                        chance = 50.0f;
                    }
                    else
                        return false;

                    if (!roll_chance_f(chance))
                        return false;

                    target = this;
                    break;
                }
                // Glyph of Healing Wave
                case 55440:
                {
                    // Not proc from self heals
                    if (this == victim)
                        return false;
                    basepoints0 = CalculatePct(int32(damage), triggerAmount);
                    target = this;
                    triggered_spell_id = 55533;
                    break;
                }
                // Spirit Hunt
                case 58877:
                {
                    // Cast on owner
                    target = GetOwner();
                    if (!target)
                        return false;
                    basepoints0 = CalculatePct(int32(damage), triggerAmount);
                    triggered_spell_id = 58879;
                    // Cast on spirit wolf
                    CastCustomSpell(this, triggered_spell_id, &basepoints0, NULL, NULL, true, NULL, triggeredByAura);
                    break;
                }
                // Shaman T8 Elemental 4P Bonus
                case 64928:
                {
                    basepoints0 = CalculatePct(int32(damage), triggerAmount);
                    triggered_spell_id = 64930;            // Electrified
                    break;
                }
                // Shaman T9 Elemental 4P Bonus
                case 67228:
                {
                    // Lava Burst
                    if (procSpell->SpellFamilyFlags[1] & 0x1000)
                    {
                        triggered_spell_id = 71824;
                        SpellInfo const* triggeredSpell = sSpellMgr->GetSpellInfo(triggered_spell_id);
                        if (!triggeredSpell)
                            return false;
                        basepoints0 = CalculatePct(int32(damage), triggerAmount) / (triggeredSpell->GetMaxDuration() / triggeredSpell->Effects[0].Amplitude);
                    }
                    break;
                }
                // Item - Shaman T10 Restoration 4P Bonus
                case 70808:
                {
                    // Chain Heal
                    if ((procSpell->SpellFamilyFlags[0] & 0x100) && (procEx & PROC_EX_CRITICAL_HIT))
                    {
                        triggered_spell_id = 70809;
                        SpellInfo const* triggeredSpell = sSpellMgr->GetSpellInfo(triggered_spell_id);
                        if (!triggeredSpell)
                            return false;
                        basepoints0 = CalculatePct(int32(damage), triggerAmount) / (triggeredSpell->GetMaxDuration() / triggeredSpell->Effects[0].Amplitude);
                        // Add remaining ticks to healing done
                        basepoints0 += GetRemainingPeriodicAmount(GetGUID(), triggered_spell_id, SPELL_AURA_PERIODIC_HEAL);
                    }
                    break;
                }
                // Item - Shaman T10 Elemental 2P Bonus
                case 70811:
                {
                    // Lightning Bolt & Chain Lightning
                    if (procSpell->SpellFamilyFlags[0] & 0x3)
                    {
                        if (ToPlayer()->HasSpellCooldown(16166))
                        {
                            uint32 newCooldownDelay = ToPlayer()->GetSpellCooldownDelay(16166);
                            if (newCooldownDelay < 3)
                                newCooldownDelay = 0;
                            else
                                newCooldownDelay -= 2;
                            ToPlayer()->AddSpellCooldown(16166, 0, uint32(time(NULL) + newCooldownDelay));

                            WorldPacket data(SMSG_MODIFY_COOLDOWN, 4+8+4);
                            data << uint32(16166);                  // Spell ID
                            data << uint64(GetGUID());              // Player GUID
                            data << int32(-2000);                   // Cooldown mod in milliseconds
                            ToPlayer()->GetSession()->SendPacket(&data);
                            return true;
                        }
                    }
                    return false;
                }
                // Elemental Overload (Shaman Elemental Mastery)
                case 77222:
                {
                    if (!procSpell || !target || GetTypeId() != TYPEID_PLAYER || effIndex != 0)
                        return false;

                    int32 chance = triggeredByAura->GetAmount();

                    // Chain Lightning has /3 chance to proc instead of other spells (kinda 8% for 25%)
                    if (procSpell->Id == 421)
                    {
                        chance = chance / 3;
                    }

                    if (!roll_chance_i(chance))
                        return false;

                    switch (procSpell->Id)
                    {
                        case 403: triggered_spell_id = 45284; break;
                        case 421:  triggered_spell_id = 45297; break;
                        case 51505: triggered_spell_id = 77451; break;
                    }

                    // Item - Shaman T13 Elemental 4P Bonus (Elemental Overload)
                    if (HasAura(105816))
                        CastSpell(this, 105821, true);

                    // Taming the Flames, Item - Shaman T12 Elemental 2P Bonus
                    if (HasAura(99204))
                        ToPlayer()->ReduceSpellCooldown(2894, 4000);

                    break;
                }
                // Item - Shaman T10 Elemental 4P Bonus
                case 70817:
                {
                    if (!target)
                        return false;
                    // try to find spell Flame Shock on the target
                    if (AuraEffect const* aurEff = target->GetAuraEffect(SPELL_AURA_PERIODIC_DAMAGE, SPELLFAMILY_SHAMAN, 0x10000000, 0x0, 0x0, GetGUID()))
                    {
                        Aura* flameShock  = aurEff->GetBase();
                        int32 maxDuration = flameShock->GetMaxDuration();
                        int32 newDuration = flameShock->GetDuration() + 2 * aurEff->GetSpellInfo()->Effects[EFFECT_1].Amplitude;

                        flameShock->SetDuration(newDuration);
                        // is it blizzlike to change max duration for FS?
                        if (newDuration > maxDuration)
                            flameShock->SetMaxDuration(newDuration);

                        return true;
                    }
                    // if not found Flame Shock
                    return false;
                }
                // Frozen Power
                case 63373:
                case 63374:
                {
                    if (!target)
                        return false;
                    if (GetDistance(target) < 15.0f)
                        return false;
                    float chance = (float)triggerAmount;
                    if (!roll_chance_f(chance))
                        return false;

                    triggered_spell_id = 63685;
                    break;
                }
                // Ancestral Awakening
                case 51556:
                case 51557:
                case 51558:
                {
                    triggered_spell_id = 52759;
                    basepoints0 = CalculatePct(int32(damage), triggerAmount);
                    target = this;
                    break;
                }
                // Earth Shield
                case 974:
                {
                    // 3.0.8: Now correctly uses the Shaman's own spell critical strike chance to determine the chance of a critical heal.
                    originalCaster = triggeredByAura->GetCasterGUID();
                    target = this;
                    basepoints0 = triggerAmount;

                    triggered_spell_id = 379;
                    break;
                }
                // Flametongue Weapon (Passive)
                case 10400:
                {
                    if (GetTypeId() != TYPEID_PLAYER  || !victim || !victim->isAlive() || !castItem || !castItem->IsEquipped())
                        return false;

                    WeaponAttackType attType = WeaponAttackType(Player::GetAttackBySlot(castItem->GetSlot()));
                    if ((attType != BASE_ATTACK && attType != OFF_ATTACK)
                        || (attType == BASE_ATTACK && procFlag & PROC_FLAG_DONE_OFFHAND_ATTACK)
                        || (attType == OFF_ATTACK && procFlag & PROC_FLAG_DONE_MAINHAND_ATTACK))
                        return false;

                    float fire_onhit = float(CalculatePct(dummySpell->Effects[EFFECT_0]. CalcValue(), 1.0f));

                    float add_spellpower = (float)(SpellBaseDamageBonusDone(SPELL_SCHOOL_MASK_FIRE)
                                         + victim->SpellBaseDamageBonusTaken(SPELL_SCHOOL_MASK_FIRE));

                    // 1.3speed = 5%, 2.6speed = 10%, 4.0 speed = 15%, so, 1.0speed = 3.84%
                    ApplyPct(add_spellpower, 3.84f);

                    // Enchant on Off-Hand and ready?
                    if (castItem->GetSlot() == EQUIPMENT_SLOT_OFFHAND && procFlag & PROC_FLAG_DONE_OFFHAND_ATTACK)
                    {
                        float BaseWeaponSpeed = GetAttackTime(OFF_ATTACK) / 1000.0f;

                        // Value1: add the tooltip damage by swingspeed + Value2: add spelldmg by swingspeed
                        basepoints0 = int32((fire_onhit * BaseWeaponSpeed) + (add_spellpower * BaseWeaponSpeed));
                        triggered_spell_id = 10444;
                    }

                    // Enchant on Main-Hand and ready?
                    else if (castItem->GetSlot() == EQUIPMENT_SLOT_MAINHAND && procFlag & PROC_FLAG_DONE_MAINHAND_ATTACK)
                    {
                        float BaseWeaponSpeed = GetAttackTime(BASE_ATTACK) / 1000.0f;

                        // Value1: add the tooltip damage by swingspeed +  Value2: add spelldmg by swingspeed
                        basepoints0 = int32((fire_onhit * BaseWeaponSpeed) + (add_spellpower * BaseWeaponSpeed));
                        triggered_spell_id = 10444;
                    }

                    // If not ready, we should  return, shouldn't we?!
                    else
                        return false;

                    CastCustomSpell(victim, triggered_spell_id, &basepoints0, NULL, NULL, true, castItem, triggeredByAura);
                    return true;
                }
                // Static Shock
                case 51525:
                case 51526:
                case 51527:
                {
                    // Lightning Shield
                    if (GetAuraEffect(SPELL_AURA_PROC_TRIGGER_SPELL, SPELLFAMILY_SHAMAN, 0x400, 0, 0))
                    {
                        uint32 spell = 26364;

                        // custom cooldown processing case
                        if (GetTypeId() == TYPEID_PLAYER && ToPlayer()->HasSpellCooldown(spell))
                            ToPlayer()->RemoveSpellCooldown(spell);

                        CastSpell(target, spell, true, castItem, triggeredByAura);
                        return true;
                    }
                    return false;
                }
            }
            break;
        }
        case SPELLFAMILY_DEATHKNIGHT:
        {
            switch (dummySpell->Id)
            {
                // Item - Death Knight T12 DPS 4P Bonus
                case 98996:
                    basepoints0 = int32(CalculatePct(damage, triggerAmount));
                    triggered_spell_id = 99000;
                    target = victim;
                    break;
                // Ebon Plaguebringer
                case 51099:
                case 51160:
                {
                    triggered_spell_id = 65142;
                    basepoints0 = triggerAmount;
                    break;
                }
                // Runic Empowerment
                case 81229:
                {
                    if (!ToPlayer())
                        return false;

                    // Runic Corruption
                    if (AuraEffect const* runicCorruption = GetDummyAuraEffect(SPELLFAMILY_DEATHKNIGHT, 4068, 0))
                    {
                        int32 basepoints0 = runicCorruption->GetAmount();
                        if (Aura* aur = GetAura(51460))
                            aur->SetDuration(aur->GetDuration() + 3000);
                        else
                            CastCustomSpell(this, 51460, &basepoints0, NULL, NULL, true);

                        // Item - Death Knight T13 DPS 4P Bonus
                        if (Aura * aura = GetAura(105646))
                        {
                            if (roll_chance_i(aura->GetSpellInfo()->Effects[1].BasePoints))
                                CastSpell(this, 105647, true);
                        }
                        return true;
                    }

                    // Item - Death Knight T13 DPS 4P Bonus
                    if (AuraEffect * auraEff = GetAuraEffect(105646, 0))
                    {
                        if (roll_chance_i(auraEff->GetAmount()))
                            CastSpell(this, 105647, true);
                    }

                    std::set<uint8> runes;
                    for (uint8 i = 0; i < MAX_RUNES; i++)
                        if (this->ToPlayer()->GetRuneCooldown(i) == this->ToPlayer()->GetRuneBaseCooldown(i))
                            runes.insert(i);
                    if (!runes.empty())
                    {
                        std::set<uint8>::iterator itr = runes.begin();
                        std::advance(itr, urand(0, runes.size()-1));
                        this->ToPlayer()->SetRuneCooldown((*itr), 0);
                        this->ToPlayer()->ResyncRunes(MAX_RUNES);
                    }
                    return true;
                }
                // Dancing Rune Weapon
                case 49028:
                {
                    // 1 dummy aura for dismiss rune blade
                    if (effIndex != 1)
                        return false;

                    Unit* pPet = NULL;
                    for (ControlList::const_iterator itr = m_Controlled.begin(); itr != m_Controlled.end(); ++itr) // Find Rune Weapon
                        if ((*itr)->GetEntry() == 27893)
                        {
                            pPet = *itr;
                            break;
                        }

                    if (pPet && pPet->getVictim() && damage && procSpell)
                    {
                        uint32 procDmg = damage / 2;
                        pPet->SendSpellNonMeleeDamageLog(pPet->getVictim(), procSpell->Id, procDmg, procSpell->GetSchoolMask(), 0, 0, false, 0, false);
                        pPet->DealDamage(pPet->getVictim(), procDmg, NULL, SPELL_DIRECT_DAMAGE, procSpell->GetSchoolMask(), procSpell, true);
                        return true;
                    }
                    else
                        return false;
                    return true; // Return true because triggered_spell_id is not exist in DBC, nothing to trigger
                }
                // Unholy Blight
                case 49194:
                {
                    triggered_spell_id = 50536;
                    SpellInfo const* unholyBlight = sSpellMgr->GetSpellInfo(triggered_spell_id);
                    if (!unholyBlight)
                        return false;

                    basepoints0 = CalculatePct(int32(damage), triggerAmount);
                    basepoints0 = basepoints0 / (unholyBlight->GetMaxDuration() / unholyBlight->Effects[0].Amplitude);
                    basepoints0 += victim->GetRemainingPeriodicAmount(GetGUID(), triggered_spell_id, SPELL_AURA_PERIODIC_DAMAGE);
                    break;
                }
                // Runic Power Back on Snare/Root
                case 61257:
                {
                    // only for spells and hit/crit (trigger start always) and not start from self casted spells
                    if (procSpell == 0 || !(procEx & (PROC_EX_NORMAL_HIT|PROC_EX_CRITICAL_HIT)) || this == victim)
                        return false;
                    // Need snare or root mechanic
                    if (!(procSpell->GetAllEffectsMechanicMask() & ((1<<MECHANIC_ROOT)|(1<<MECHANIC_SNARE))))
                        return false;
                    triggered_spell_id = 61258;
                    target = this;
                    break;
                }
                // Dark Simulacrum
                case 77606:
                {
                    basepoints0 = procSpell->Id;
                    Unit* caster = triggeredByAura->GetCaster();
                    victim = this;

                    if (!caster || !victim)
                        return false;

                    caster->removeSimulacrumTarget();

                    if (!procSpell->IsCanBeStolen() || !triggeredByAura)
                        return false;

                    if (Creature* targetCreature = victim->ToCreature())
                        if (!targetCreature->isCanGiveSpell(caster))
                            return false;

                    caster->setSimulacrumTarget(victim);

                    // Replacer
                    caster->CastCustomSpell(caster, 77616, &basepoints0, 0, 0, true);

                    // SpellPower
                    basepoints0 = victim->SpellBaseDamageBonusDone(SpellSchoolMask(procSpell->SchoolMask));
                    int32 basepoints1 = victim->SpellBaseHealingBonusDone(SpellSchoolMask(procSpell->SchoolMask));
                    caster->CastCustomSpell(caster, 94984, &basepoints0, &basepoints1, 0, true);
                    return true;
                }
            }
            // Blood-Caked Blade
            if (dummySpell->SpellIconID == 138)
            {
                if (!target || !target->isAlive())
                    return false;

                triggered_spell_id = dummySpell->Effects[effIndex].TriggerSpell;
                break;
            }
            // Butchery
            if (dummySpell->SpellIconID == 2664)
            {
                basepoints0 = triggerAmount;
                triggered_spell_id = 50163;
                target = this;
                break;
            }
            // Threat of Thassarian
            if (dummySpell->SpellIconID == 2023)
            {
                // Must Dual Wield
                if (!procSpell || !haveOffhandWeapon())
                    return false;
                // Chance as basepoints for dummy aura
                if (!roll_chance_i(triggerAmount))
                    return false;

                switch (procSpell->Id)
                {
                    case 49020: triggered_spell_id = 66198; break; // Obliterate
                    case 49143: triggered_spell_id = 66196; break; // Frost Strike
                    case 45462: triggered_spell_id = 66216; break; // Plague Strike
                    case 49998: triggered_spell_id = 66188; break; // Death Strike
                    case 56815: triggered_spell_id = 66217; break; // Rune Strike
                    case 45902: triggered_spell_id = 66215; break; // Blood Strike
                    default:
                        return false;
                }
                break;
            }
            break;
        }
        case SPELLFAMILY_POTION:
        {
            // alchemist's stone
            if (dummySpell->Id == 17619)
            {
                if (procSpell->SpellFamilyName == SPELLFAMILY_POTION)
                {
                    for (uint8 i = 0; i < MAX_SPELL_EFFECTS; i++)
                    {
                        if (procSpell->Effects[i].Effect == SPELL_EFFECT_HEAL)
                        {
                            triggered_spell_id = 21399;
                        }
                        else if (procSpell->Effects[i].Effect == SPELL_EFFECT_ENERGIZE)
                        {
                            triggered_spell_id = 21400;
                        }
                        else
                            continue;

                        basepoints0 = int32(CalculateSpellDamage(this, procSpell, i) * 0.4f);
                        CastCustomSpell(this, triggered_spell_id, &basepoints0, NULL, NULL, true, NULL, triggeredByAura);
                    }
                    return true;
                }
            }
            break;
        }
        case SPELLFAMILY_PET:
        {
            switch (dummySpell->SpellIconID)
            {
                // Guard Dog
                case 201:
                {
                    if (!victim)
                        return false;

                    /* Not exist in cataclysm 4.3.4 */
                    //triggered_spell_id = 54445;
                    target = this;
                    float addThreat = float(CalculatePct(procSpell->Effects[0].CalcValue(this), triggerAmount));
                    victim->AddThreat(this, addThreat);
                    break;
                }
                // Silverback
                case 1582:
                    triggered_spell_id = dummySpell->Id == 62765 ? 62801 : 62800;
                    target = this;
                    break;
            }
            break;
        }
        default:
            break;
    }

    // Vengeance tank mastery
    switch (dummySpell->Id)
    {
        case 84839:
        case 84840:
        case 93098:
        case 93099:
        {
            if (!victim || victim->GetTypeId() == TYPEID_PLAYER)
                return false;

            if (victim->GetOwner() && victim->GetOwner()->GetTypeId() == TYPEID_PLAYER)
                return false;

            if (GetTypeId() != TYPEID_PLAYER)
                return false;

            if (!isInCombat())
                return false;

            int32 aviableBasepoints = 0;
            int32 max_amount = 0;

            triggered_spell_id = 76691;
            
            if (Aura const* aur = GetAura(triggered_spell_id, GetGUID()))
            {
                aviableBasepoints += aur->GetEffect(EFFECT_0)->GetAmount();
                max_amount += aur->GetEffect(EFFECT_2)->GetAmount();
            }

            // The first melee attack taken by the tank generates Vengeance equal to 33% of the damage taken by that attack.
            if (!aviableBasepoints && (procFlag & (PROC_FLAG_TAKEN_MELEE_AUTO_ATTACK)))
                triggerAmount = 33;

            
            int32 cap = (GetCreateHealth() + GetStat(STAT_STAMINA) * 14) / 10;
            basepoints0 = int32(damage * triggerAmount / 100);
            basepoints0 += aviableBasepoints;
            basepoints0 = std::min(cap, basepoints0);

            // calculate max amount player's had durind the fight
            int32 basepoints1 = std::max(basepoints0, max_amount);

            CastCustomSpell(this, triggered_spell_id, &basepoints0, &basepoints0, &basepoints1, true, castItem,triggeredByAura, originalCaster );
            return true;
        }
        default:
            break;
    }

    // if not handled by custom case, get triggered spell from dummySpell proto
    if (!triggered_spell_id)
        triggered_spell_id = dummySpell->Effects[triggeredByAura->GetEffIndex()].TriggerSpell;

    // processed charge only counting case
    if (!triggered_spell_id)
        return true;

    SpellInfo const* triggerEntry = sSpellMgr->GetSpellInfo(triggered_spell_id);
    if (!triggerEntry)
        return false;

    if (cooldown_spell_id == 0)
        cooldown_spell_id = triggered_spell_id;

    if (cooldown && GetTypeId() == TYPEID_PLAYER && ToPlayer()->HasSpellCooldown(cooldown_spell_id))
        return false;

    if (basepoints0)
        CastCustomSpell(target, triggered_spell_id, &basepoints0, NULL, NULL, true, castItem, triggeredByAura, originalCaster);
    else
        CastSpell(target, triggered_spell_id, true, castItem, triggeredByAura, originalCaster);

    if (cooldown && GetTypeId() == TYPEID_PLAYER)
        ToPlayer()->AddSpellCooldown(cooldown_spell_id, 0, time(NULL) + cooldown);

    return true;
}

bool Unit::HandleModifierAuraProc(Unit* victim, uint32 damage, AuraEffect* triggeredByAura, SpellInfo const* procSpell, uint32 procFlag, uint32 procEx, uint32 cooldown)
{
    SpellInfo const* triggeredByAuraSpell = triggeredByAura->GetSpellInfo();

    Item* castItem = triggeredByAura->GetBase()->GetCastItemGUID() && GetTypeId() == TYPEID_PLAYER
        ? ToPlayer()->GetItemByGuid(triggeredByAura->GetBase()->GetCastItemGUID()) : NULL;

    uint32 triggered_spell_id = 0;
    Unit* target = victim;
    int32 triggerAmount = triggeredByAura->GetAmount();
    int32 basepoints0 = 0;

    switch(triggeredByAuraSpell->SpellFamilyName)
    {
        case SPELLFAMILY_DRUID:
        {
            switch(triggeredByAuraSpell->Id)
            {
                // Item - Druid T11 Feral 4P Bonus
                case 90165:
                {
                    target = this;
                    triggered_spell_id = 90166;
                    break;
                }
                // Harmony - Restoration Mastery
                case 77495:
                {
                    target = this;
                    basepoints0 = triggerAmount;
                    triggered_spell_id = 100977;
                    break;
                }
            }
            break;
        }
        case SPELLFAMILY_WARLOCK:
        {
            switch (triggeredByAuraSpell->Id)
            {
                // Impending doom
                case 85106:
                case 85107:
                case 85108:
                {
 	    if (!this || !this->ToPlayer()) // shit things morph over morph
	        break;
                    this->ToPlayer()->SpellCooldownReduction(47241, 15000);
                    break;
                }
                // Soul Burn
                case 74434:
                {
                    if (!procSpell)
                        return false;
                    
                    switch (procSpell->Id)
                    {
                        // Healthstone
                        case 6262:
                            CastSpell(this, 79437, true);
                            break;
                        // Searing Pain
                        case 5676:
                            CastSpell(this, 79440, true);
                            break;
                        // Demonic Circle: Teleport
                        case 48020:
                            CastSpell(this, 79438, true);
                            break;
                        // Seed of Corruption
                        case 27243:
                        {
                            if (ToPlayer()->HasSpell(86664))
                                CastSpell(this, 86664, true);                                
                            break;
                        }
                    }
                    return true;
                }
                default:
                    break;
            }
            break;
        }
        case SPELLFAMILY_DEATHKNIGHT:
        {
            switch (triggeredByAuraSpell->Id)
            {
                // Unholy Command
                case 49588:
                case 49589:
                    if (victim && victim->GetOwner() && victim->GetOwner()->GetGUID() == GetGUID())
                        return false;

                    triggered_spell_id = 90289;
                    break;
                default:
                    break;
            }
            break;
        }
    }

    // processed charge only counting case
    if (!triggered_spell_id)
        return true;

    SpellEntry const* triggerEntry = sSpellStore.LookupEntry(triggered_spell_id);
    if (!triggerEntry)
        return false;

    // default case
    if (!target || (target != this && !target->isAlive()))
        return false;

    if (cooldown && GetTypeId() == TYPEID_PLAYER && this->ToPlayer()->HasSpellCooldown(triggered_spell_id))
        return false;

    if (basepoints0)
        CastCustomSpell(target, triggered_spell_id, &basepoints0, NULL, NULL, true, castItem, triggeredByAura);
    else
        CastSpell(target, triggered_spell_id, true, castItem, triggeredByAura);

    if (cooldown && GetTypeId() == TYPEID_PLAYER)
        this->ToPlayer()->AddSpellCooldown(triggered_spell_id, 0, time(NULL) + cooldown);

    return true;
}

bool Unit::HandleObsModEnergyAuraProc(Unit* victim, uint32 /*damage*/, AuraEffect* triggeredByAura, SpellInfo const* /*procSpell*/, uint32 /*procFlag*/, uint32 /*procEx*/, uint32 cooldown)
{
    SpellInfo const* dummySpell = triggeredByAura->GetSpellInfo();
    //uint32 effIndex = triggeredByAura->GetEffIndex();
    //int32  triggerAmount = triggeredByAura->GetAmount();

    Item* castItem = triggeredByAura->GetBase()->GetCastItemGUID() && GetTypeId() == TYPEID_PLAYER
        ? ToPlayer()->GetItemByGuid(triggeredByAura->GetBase()->GetCastItemGUID()) : NULL;

    uint32 triggered_spell_id = 0;
    Unit* target = victim;
    int32 basepoints0 = 0;

    // processed charge only counting case
    if (!triggered_spell_id)
        return true;

    SpellInfo const* triggerEntry = sSpellMgr->GetSpellInfo(triggered_spell_id);

    // Try handle unknown trigger spells
    if (!triggerEntry)
        return false;

    if (cooldown && GetTypeId() == TYPEID_PLAYER && ToPlayer()->HasSpellCooldown(triggered_spell_id))
        return false;
    if (basepoints0)
        CastCustomSpell(target, triggered_spell_id, &basepoints0, NULL, NULL, true, castItem, triggeredByAura);
    else
        CastSpell(target, triggered_spell_id, true, castItem, triggeredByAura);

    if (cooldown && GetTypeId() == TYPEID_PLAYER)
        ToPlayer()->AddSpellCooldown(triggered_spell_id, 0, time(NULL) + cooldown);
    return true;
}
bool Unit::HandleModDamagePctTakenAuraProc(Unit* victim, uint32 /*damage*/, AuraEffect* triggeredByAura, SpellInfo const* /*procSpell*/, uint32 /*procFlag*/, uint32 /*procEx*/, uint32 cooldown)
{
    SpellInfo const* dummySpell = triggeredByAura->GetSpellInfo();
    //uint32 effIndex = triggeredByAura->GetEffIndex();
    //int32  triggerAmount = triggeredByAura->GetAmount();

    Item* castItem = triggeredByAura->GetBase()->GetCastItemGUID() && GetTypeId() == TYPEID_PLAYER
        ? ToPlayer()->GetItemByGuid(triggeredByAura->GetBase()->GetCastItemGUID()) : NULL;

    uint32 triggered_spell_id = 0;
    Unit* target = victim;
    int32 basepoints0 = 0;

    /*
    switch (dummySpell->SpellFamilyName)
    {

    }
    */

    // processed charge only counting case
    if (!triggered_spell_id)
        return true;

    SpellInfo const* triggerEntry = sSpellMgr->GetSpellInfo(triggered_spell_id);

    if (!triggerEntry)
        return false;

    if (cooldown && GetTypeId() == TYPEID_PLAYER && ToPlayer()->HasSpellCooldown(triggered_spell_id))
        return false;

    if (basepoints0)
        CastCustomSpell(target, triggered_spell_id, &basepoints0, NULL, NULL, true, castItem, triggeredByAura);
    else
        CastSpell(target, triggered_spell_id, true, castItem, triggeredByAura);

    if (cooldown && GetTypeId() == TYPEID_PLAYER)
        ToPlayer()->AddSpellCooldown(triggered_spell_id, 0, time(NULL) + cooldown);

    return true;
}

// Used in case when access to whole aura is needed
// All procs should be handled like this...
bool Unit::HandleAuraProc(Unit* victim, uint32 damage, uint32 absorb, Aura* triggeredByAura, SpellInfo const* procSpell, uint32 /*procFlag*/, uint32 procEx, uint32 cooldown, bool *handled)
{
    SpellInfo const* dummySpell = triggeredByAura->GetSpellInfo();

    switch (dummySpell->SpellFamilyName)
    {
        case SPELLFAMILY_GENERIC:
            switch (dummySpell->Id)
            {
                // Nevermelting Ice Crystal
                case 71564:
                    RemoveAuraFromStack(71564);
                    *handled = true;
                    break;
                // Gaseous Bloat
                case 70672:
                case 72455:
                case 72832:
                case 72833:
                {
                    *handled = true;
                    uint32 stack = triggeredByAura->GetStackAmount();
                    int32 const mod = (GetMap()->GetSpawnMode() & 1) ? 1500 : 1250;
                    int32 dmg = 0;
                    for (uint8 i = 1; i < stack; ++i)
                        dmg += mod * stack;
                    if (Unit* caster = triggeredByAura->GetCaster())
                        caster->CastCustomSpell(70701, SPELLVALUE_BASE_POINT0, dmg);
                    break;
                }
                // Ball of Flames Proc
                case 71756:
                case 72782:
                case 72783:
                case 72784:
                    RemoveAuraFromStack(dummySpell->Id);
                    *handled = true;
                    break;
                // Discerning Eye of the Beast
                case 59915:
                {
                    CastSpell(this, 59914, true);   // 59914 already has correct basepoints in DBC, no need for custom bp
                    *handled = true;
                    break;
                }
                // Swift Hand of Justice
                case 59906:
                {
                    int32 bp0 = CalculatePct(GetMaxHealth(), dummySpell->Effects[EFFECT_0]. CalcValue());
                    CastCustomSpell(this, 59913, &bp0, NULL, NULL, true);
                    *handled = true;
                    break;
                }
            }

            break;
        case SPELLFAMILY_PALADIN:
        {
            // Repentance aura drop
            if (dummySpell->Id == 20066)
            {
                *handled = true;
                if (procSpell && procSpell->SpellFamilyName == SPELLFAMILY_PALADIN &&
                    procSpell->SpellFamilyFlags[0] == 0x20000000 && procSpell->SpellFamilyFlags[1] == 0x00000800)
                    return false;
                return true;
            }
            break;
        }
        case SPELLFAMILY_MAGE:
        {
            switch (dummySpell->Id)
            {
                // Arcane Missiles!
                case 79684:
                {
                    // Hot Streak
                    if (HasAura(44445))
                        *handled = true;

                    // Brain Freeze
                    if (GetAuraEffect(SPELL_AURA_PROC_TRIGGER_SPELL, SPELLFAMILY_MAGE, 0, 0, 0x00008000, GetGUID()))
                        *handled = true;

                    // roll basepoints
                    if (!roll_chance_i(triggeredByAura->GetEffect(0)->GetAmount()))
                        *handled = true;

                    // Disable proc from Arcane Missiles
                    if (procSpell->Id == 7268)
                        *handled = true;

                    break;
                }
                // Empowered Fire
                case 31656:
                case 31657:
                case 31658:
                {
                    *handled = true;

                    SpellInfo const* spInfo = sSpellMgr->GetSpellInfo(67545);
                    if (!spInfo)
                        return false;

                    int32 bp0 = int32(CalculatePct(GetCreateMana(), spInfo->Effects[0].CalcValue()));
                    CastCustomSpell(this, 67545, &bp0, NULL, NULL, true, NULL, triggeredByAura->GetEffect(EFFECT_0), GetGUID());
                    return true;
                }
            }
            break;
        }
        case SPELLFAMILY_DEATHKNIGHT:
        {
            switch (dummySpell->Id)
            {
                case 54637: // Blood of the North
                case 56835: // Reaping
                case 50034: // Blood Rites
                {
                    *handled = true;
                    if (GetTypeId() == TYPEID_PLAYER)
                    {
                        Player* plr = this->ToPlayer();
                        if (plr->getClass() != CLASS_DEATH_KNIGHT)
                            return false;

                        if (AuraEffect * aurEff = triggeredByAura->GetEffect(EFFECT_0))
                            aurEff->ResetPeriodic(true);

                        uint32 runesLeft;
                        switch (procSpell->Id)
                        {
                            case 45902: // Blood Strike
                            case 50842: // Pestilence
                                runesLeft = 1;
                                for (uint8 i=0; i < MAX_RUNES && runesLeft; ++i)
                                {
                                    if (plr->GetCurrentRune(i) == RUNE_DEATH
                                        || plr->GetBaseRune(i) != RUNE_BLOOD
                                        || plr->IsDeathRuneUsed(i))
                                        continue;

                                    if (plr->GetRuneCooldown(i) != plr->GetRuneBaseCooldown(i))
                                        continue;

                                    --runesLeft;
                                    plr->AddRuneBySpell(i, RUNE_DEATH, dummySpell->Id);
                                }
                                break;
                            case 49020: // Obliterate
                            case 49998: // Death Strike
                                runesLeft = 2;
                                for (uint8 i=0; i < MAX_RUNES && runesLeft; ++i)
                                {
                                    if (plr->GetCurrentRune(i) == RUNE_DEATH
                                        || plr->GetBaseRune(i) == RUNE_BLOOD
                                        || plr->IsDeathRuneUsed(i))
                                        continue;

                                    if (plr->GetRuneCooldown(i) != plr->GetRuneBaseCooldown(i))
                                        continue;

                                    --runesLeft;
                                    plr->AddRuneBySpell(i, RUNE_DEATH, dummySpell->Id);
                                }
                                break;
                            case 85948: // Festering Strike
                                runesLeft = 2;
                                for (uint8 i=0; i < MAX_RUNES && runesLeft; ++i)
                                {
                                    if (plr->GetCurrentRune(i) == RUNE_DEATH
                                        || plr->GetBaseRune(i) == RUNE_UNHOLY
                                        || plr->IsDeathRuneUsed(i))
                                        continue;

                                    if (plr->GetRuneCooldown(i) != plr->GetRuneBaseCooldown(i))
                                        continue;

                                    --runesLeft;
                                    plr->AddRuneBySpell(i, RUNE_DEATH, dummySpell->Id);
                                }
                                break;
                        }
                        return true;
                    }
                    return false;
                }
                // Bone Shield cooldown
                case 49222:
                {
                    *handled = true;
                    if (cooldown && GetTypeId() == TYPEID_PLAYER)
                    {
                        if (ToPlayer()->HasSpellCooldown(200000))
                            return false;
                        ToPlayer()->AddSpellCooldown(200000, 0, time(NULL) + cooldown);
                    }
                    return true;
                }
                // Hungering Cold aura drop
                case 49203:
                    *handled = true;
                    // Drop only in not disease case
                    if (procSpell && procSpell->Dispel == DISPEL_DISEASE)
                        return false;
                    return true;
            }
            break;
        }
        case SPELLFAMILY_WARRIOR:
        {
            switch (dummySpell->Id)
            {
                // Item - Warrior T10 Protection 4P Bonus
                case 70844:
                {
                    int32 basepoints0 = CalculatePct(GetMaxHealth(), dummySpell->Effects[EFFECT_1]. CalcValue());
                    CastCustomSpell(this, 70845, &basepoints0, NULL, NULL, true);
                    break;
                }
                // Recklessness
                case 1719:
                {
                    //! Possible hack alert
                    //! Don't drop charges on proc, they will be dropped on SpellMod removal
                    //! Before this change, it was dropping two charges per attack, one in ProcDamageAndSpellFor, and one in RemoveSpellMods.
                    //! The reason of this behaviour is Recklessness having three auras, 2 of them can not proc (isTriggeredAura array) but the other one can, making the whole spell proc.
                    *handled = true;
                    break;
                }
                default:
                    break;
            }
            break;
        }
        case SPELLFAMILY_WARLOCK:
            // Soulburn
            if (dummySpell->Id == 74434)
                if (procSpell->Id == 6353)
                    // Item - Warlock T13 4P Bonus (Soulburn)
                    if (HasAura(105787))
                        EnergizeBySpell(this, 74434, 1, POWER_SOUL_SHARDS);
            break;
    }
    return false;
}

bool Unit::HandleProcTriggerSpell(Unit* victim, uint32 damage, uint32 absorb, AuraEffect* triggeredByAura, SpellInfo const* procSpell, uint32 procFlags, uint32 procEx, uint32 cooldown)
{
    // Get triggered aura spell info
    SpellInfo const* auraSpellInfo = triggeredByAura->GetSpellInfo();

    // Basepoints of trigger aura
    int32 triggerAmount = triggeredByAura->GetAmount();

    // Set trigger spell id, target, custom basepoints
    uint32 trigger_spell_id = auraSpellInfo->Effects[triggeredByAura->GetEffIndex()].TriggerSpell;

    Unit*  target = NULL;
    int32  basepoints0 = 0;

    if (triggeredByAura->GetAuraType() == SPELL_AURA_PROC_TRIGGER_SPELL_WITH_VALUE)
        basepoints0 = triggerAmount;

    Item* castItem = triggeredByAura->GetBase()->GetCastItemGUID() && GetTypeId() == TYPEID_PLAYER
        ? ToPlayer()->GetItemByGuid(triggeredByAura->GetBase()->GetCastItemGUID()) : NULL;

    // Try handle unknown trigger spells
    if (sSpellMgr->GetSpellInfo(trigger_spell_id) == NULL)
    {
        switch (auraSpellInfo->SpellFamilyName)
        {
            case SPELLFAMILY_GENERIC:
                switch (auraSpellInfo->Id)
                {
                    case 102726: // Mark of Silence, Asira Dawnslayer, Hour of Twilight
                        if (Unit* pAsira = triggeredByAura->GetCaster())
                            pAsira->CastSpell(this, 103597, true);
                        return true;
                    case 99399: // Burning Wound, Ragnaros, Firelands
                    case 101238:
                    case 101239:
                    case 101240:
                    {
                        uint8 stacks = triggeredByAura->GetBase()->GetStackAmount();
                        int32 bp = urand(1950, 2050);
                        bp *= stacks;
                        CastCustomSpell(victim, 99400, &bp, NULL, NULL, true, NULL, triggeredByAura);
                        return true;
                    }
                    case 99296: // Combustible, Ragnaros (Firelands)
                    case 100282:
                    case 100283:
                    case 100284:
                        GetMotionMaster()->MovementExpired(false);
                        StopMoving();
                        if (victim)
                            CastSpell(victim, 99303, true);
                        if (Creature* pMeteor = ToCreature())
                            pMeteor->AI()->DoAction(4); // ACTION_COMBUSTION
                        break;
                    case 79900: // Unstable Shield, Electron
                    case 91447:
                    case 91448:
                    case 91449:
                        if (victim)
                            CastSpell(victim, 79911, true);
                        break;
                    case 99984: // Slightly Warm Pincers
                        trigger_spell_id = 99987;
                        target = victim;
                        break;
                    case 23780:             // Aegis of Preservation (Aegis of Preservation trinket)
                        trigger_spell_id = 23781;
                        break;
                    case 33896:             // Desperate Defense (Stonescythe Whelp, Stonescythe Alpha, Stonescythe Ambusher)
                        trigger_spell_id = 33898;
                        break;
                    case 43820:             // Charm of the Witch Doctor (Amani Charm of the Witch Doctor trinket)
                        // Pct value stored in dummy
                        basepoints0 = victim->GetCreateHealth() * auraSpellInfo->Effects[1].CalcValue() / 100;
                        target = victim;
                        break;
                    case 57345:             // Darkmoon Card: Greatness
                    {
                        float stat = 0.0f;
                        // strength
                        if (GetStat(STAT_STRENGTH) > stat) { trigger_spell_id = 60229;stat = GetStat(STAT_STRENGTH); }
                        // agility
                        if (GetStat(STAT_AGILITY)  > stat) { trigger_spell_id = 60233;stat = GetStat(STAT_AGILITY);  }
                        // intellect
                        if (GetStat(STAT_INTELLECT)> stat) { trigger_spell_id = 60234;stat = GetStat(STAT_INTELLECT);}
                        // spirit
                        if (GetStat(STAT_SPIRIT)   > stat) { trigger_spell_id = 60235;                               }
                        break;
                    }
                    case 64568:             // Blood Reserve
                    {
                        if (HealthBelowPctDamaged(35, damage))
                        {
                            CastCustomSpell(this, 64569, &triggerAmount, NULL, NULL, true);
                            RemoveAura(64568);
                        }
                        return false;
                    }
                    case 67702:             // Death's Choice, Item - Coliseum 25 Normal Melee Trinket
                    {
                        float stat = 0.0f;
                        // strength
                        if (GetStat(STAT_STRENGTH) > stat) { trigger_spell_id = 67708;stat = GetStat(STAT_STRENGTH); }
                        // agility
                        if (GetStat(STAT_AGILITY)  > stat) { trigger_spell_id = 67703;                               }
                        break;
                    }
                    case 67771:             // Death's Choice (heroic), Item - Coliseum 25 Heroic Melee Trinket
                    {
                        float stat = 0.0f;
                        // strength
                        if (GetStat(STAT_STRENGTH) > stat) { trigger_spell_id = 67773;stat = GetStat(STAT_STRENGTH); }
                        // agility
                        if (GetStat(STAT_AGILITY)  > stat) { trigger_spell_id = 67772;                               }
                        break;
                    }
                    // Mana Drain Trigger
                    case 27522:
                    case 40336:
                    {
                        // On successful melee or ranged attack gain $29471s1 mana and if possible drain $27526s1 mana from the target.
                        if (this && isAlive())
                            CastSpell(this, 29471, true, castItem, triggeredByAura);
                        if (victim && victim->isAlive())
                            CastSpell(victim, 27526, true, castItem, triggeredByAura);
                        return true;
                    }
                    // Evasive Maneuvers
                    case 50240:
                    {
                        // Remove a Evasive Charge
                        Aura* charge = GetAura(50241);
                        if (charge && charge->ModStackAmount(-1, AURA_REMOVE_BY_ENEMY_SPELL))
                            RemoveAurasDueToSpell(50240);
                        break;
                    }
                    // Reactive Barrier
                    case 86303:
                    case 86304:           
                        if (GetTypeId() != TYPEID_PLAYER || ToPlayer()->HasSpellCooldown(11426))
                            return false;

                        if (GetHealthPct() > 50)
                            return false;

                        if (!roll_chance_i(auraSpellInfo->ProcChance))
                            return false;

                        CastSpell(this, 86347, true);
                        CastSpell(this, 11426, TriggerCastFlags(0x72FFD));
                        break;
                }
                break;
            case SPELLFAMILY_MAGE:
                if (auraSpellInfo->SpellIconID == 2127)     // Blazing Speed
                {
                    switch (auraSpellInfo->Id)
                    {
                        case 31641:  // Rank 1
                        case 31642:  // Rank 2
                            trigger_spell_id = 31643;
                            break;
                        default:
                            return false;
                    }
                }
                break;
            case SPELLFAMILY_WARRIOR:
                if (auraSpellInfo->Id == 50421)             // Scent of Blood
                {
                    CastSpell(this, 50422, true);
                    RemoveAuraFromStack(auraSpellInfo->Id);
                    return false;
                }
                break;
            case SPELLFAMILY_PRIEST:
            {
                // Greater Heal Refund
                if (auraSpellInfo->Id == 37594)
                    trigger_spell_id = 37595;
                break;
            }
            case SPELLFAMILY_DRUID:
            {
                switch (auraSpellInfo->Id)
                {
                    // Druid Forms Trinket
                    case 37336:
                    {
                        switch (GetShapeshiftForm())
                        {
                            case FORM_NONE:     trigger_spell_id = 37344; break;
                            case FORM_CAT:      trigger_spell_id = 37341; break;
                            case FORM_BEAR:     trigger_spell_id = 37340; break;
                            case FORM_TREE:     trigger_spell_id = 37342; break;
                            case FORM_MOONKIN:  trigger_spell_id = 37343; break;
                            default:
                                return false;
                        }
                        break;
                    }
                    // Druid T9 Feral Relic (Lacerate, Swipe, Mangle, and Shred)
                    case 67353:
                    {
                        switch (GetShapeshiftForm())
                        {
                            case FORM_CAT:      trigger_spell_id = 67355; break;
                            case FORM_BEAR:     trigger_spell_id = 67354; break;
                            default:
                                return false;
                        }
                        break;
                    }
                    default:
                        break;
                }
                break;
            }
            case SPELLFAMILY_HUNTER:
            {
                if (auraSpellInfo->SpellIconID == 2225)     // Serpent Spread 1, 2
                {
                    switch (auraSpellInfo->Id)
                    {
                        case 87934:
                            trigger_spell_id = 88453;
                            break;
                        case 87935:
                            trigger_spell_id = 88466;
                            break;
                        default:
                            return false;
                    }
                    break;
                }
                if (auraSpellInfo->SpellIconID == 3247)     // Piercing Shots
                {
                    switch (auraSpellInfo->Id)
                    {
                        case 53234:  // Rank 1
                        case 53237:  // Rank 2
                        case 53238:  // Rank 3
                            trigger_spell_id = 63468;
                            break;
                        default:
                            return false;
                    }
                    SpellInfo const* TriggerPS = sSpellMgr->GetSpellInfo(trigger_spell_id);
                    if (!TriggerPS)
                        return false;

                    basepoints0 = CalculatePct(int32(damage), triggerAmount) / (TriggerPS->GetMaxDuration() / TriggerPS->Effects[0].Amplitude);
                    basepoints0 += victim->GetRemainingPeriodicAmount(GetGUID(), trigger_spell_id, SPELL_AURA_PERIODIC_DAMAGE);
                    break;
                }
                // Item - Hunter T9 4P Bonus
                if (auraSpellInfo->Id == 67151)
                {
                    trigger_spell_id = 68130;
                    target = this;
                    break;
                }
                break;
            }
            case SPELLFAMILY_PALADIN:
            {
                switch (auraSpellInfo->Id)
                {
                    // Healing Discount
                    case 37705:
                    {
                        trigger_spell_id = 37706;
                        target = this;
                        break;
                    }
                    // Soul Preserver
                    case 60510:
                    {
                        switch (getClass())
                        {
                            case CLASS_DRUID:
                                trigger_spell_id = 60512;
                                break;
                            case CLASS_PALADIN:
                                trigger_spell_id = 60513;
                                break;
                            case CLASS_PRIEST:
                                trigger_spell_id = 60514;
                                break;
                            case CLASS_SHAMAN:
                                trigger_spell_id = 60515;
                                break;
                        }

                        target = this;
                        break;
                    }
                    case 37657: // Lightning Capacitor
                    case 54841: // Thunder Capacitor
                    case 67712: // Item - Coliseum 25 Normal Caster Trinket
                    case 67758: // Item - Coliseum 25 Heroic Caster Trinket
                    {
                        if (!victim || !victim->isAlive() || GetTypeId() != TYPEID_PLAYER)
                            return false;

                        uint32 stack_spell_id = 0;
                        switch (auraSpellInfo->Id)
                        {
                            case 37657:
                                stack_spell_id = 37658;
                                trigger_spell_id = 37661;
                                break;
                            case 54841:
                                stack_spell_id = 54842;
                                trigger_spell_id = 54843;
                                break;
                            case 67712:
                                stack_spell_id = 67713;
                                trigger_spell_id = 67714;
                                break;
                            case 67758:
                                stack_spell_id = 67759;
                                trigger_spell_id = 67760;
                                break;
                        }

                        CastSpell(this, stack_spell_id, true, NULL, triggeredByAura);

                        Aura* dummy = GetAura(stack_spell_id);
                        if (!dummy || dummy->GetStackAmount() < triggerAmount)
                            return false;

                        RemoveAurasDueToSpell(stack_spell_id);
                        target = victim;
                        break;
                    }
                    // Hand of Light (Mastery Retribution Paladin)
                    case 76672:
                    {
                        if (!victim || !procSpell || (procSpell->Id != 35395 && procSpell->Id != 53385 && procSpell->Id != 85256))
                            return false;

                        if (this->GetGUID() == victim->GetGUID())
                            return false;

                        trigger_spell_id = 96172;
                        basepoints0 = int32((damage + absorb) / 100.0f * triggeredByAura->GetAmount());
                        if (HasAura(84963))
                            AddPct(basepoints0, 30);
                        target = victim;
                        break;
                    }
                    // Item - Paladin T13 Protection 2P Bonus (Judgement)
                    case 105800:
                    {
                        if (!damage)
                            return false;

                        basepoints0 = triggerAmount * damage / 100.0f;
                        trigger_spell_id = 105801;
                        break;
                    }
                    case 53695:
                    case 53696:
                    {
                        int32 bp = HasAura(53696) ? 20 : 10;
                        CastCustomSpell(victim, 68055, &bp, NULL, NULL, true, NULL, triggeredByAura);
                        return true;
                    }
                    default:
                        break;
                }
                break;
            }
            case SPELLFAMILY_SHAMAN:
            {
                switch (auraSpellInfo->Id)
                {
                    // Lightning Shield (The Ten Storms set)
                    case 23551:
                    {
                        trigger_spell_id = 23552;
                        target = victim;
                        break;
                    }
                    // Damage from Lightning Shield (The Ten Storms set)
                    case 23552:
                    {
                        trigger_spell_id = 27635;
                        break;
                    }
                    // Mana Surge (The Earthfury set)
                    case 23572:
                    {
                        if (!procSpell)
                            return false;
                        basepoints0 = int32(CalculatePct(procSpell->ManaCost, 35));
                        trigger_spell_id = 23571;
                        target = this;
                        break;
                    }
                    case 30881: // Nature's Guardian Rank 1
                    case 30883: // Nature's Guardian Rank 2
                    case 30884: // Nature's Guardian Rank 3
                    {
                        if (HealthBelowPct(30) || HealthBelowPctDamaged(30, damage))
                        {
                            basepoints0 = int32(CountPctFromMaxHealth(triggerAmount));
                            target = this;
                            trigger_spell_id = 31616;
                            if (victim && victim->isAlive())
                                victim->getThreatManager().modifyThreatPercent(this, -10);
                        }
                        else
                            return false;
                        break;
                    }
                }
                break;
            }
            case SPELLFAMILY_DEATHKNIGHT:
            {
                // Item - Death Knight T10 Melee 4P Bonus
                if (auraSpellInfo->Id == 70656)
                {
                    if (GetTypeId() != TYPEID_PLAYER || getClass() != CLASS_DEATH_KNIGHT)
                        return false;

                    for (uint8 i = 0; i < MAX_RUNES; ++i)
                        if (ToPlayer()->GetRuneCooldown(i) == 0)
                            return false;
                }
                break;
            }
            case SPELLFAMILY_ROGUE:
            {
                switch (auraSpellInfo->Id)
                {
                    // Rogue T10 2P bonus, should only proc on caster
                    case 70805:
                    {
                        if (victim != this)
                            return false;
                        break;
                    }
                    // Rogue T10 4P bonus, should proc on victim
                    case 70803:
                    {
                        target = victim;
                        break;
                    }
                }
                break;
            }
            default:
                 break;
        }
    }

    // All ok. Check current trigger spell
    SpellInfo const* triggerEntry = sSpellMgr->GetSpellInfo(trigger_spell_id);
    if (triggerEntry == NULL)
        return false;

    // not allow proc extra attack spell at extra attack
    if (m_extraAttacks && triggerEntry->HasEffect(SPELL_EFFECT_ADD_EXTRA_ATTACKS))
        return false;

    // Custom requirements (not listed in procEx) Warning! damage dealing after this
    // Custom triggered spells
    switch (auraSpellInfo->Id)
    {
        // Item - Death Knight T12 Blood 2P Bonus (wrong spellname, that's warrior item set)
        case 105907:
            if (!procSpell)
                return false;
            if (!(procSpell->Id == 12294 || (procSpell->Id == 23881 && roll_chance_i(50))))
                return false;
            break;
        // Item - Mage T13 2P Bonus (Haste)
        case 105788:
            if (!procSpell)
                return false;
            if (procSpell->Id != 30451 && !roll_chance_i(50))
                return false;
            break;
        // Savage Defence
        case 62600:
        {
            int32 chance = 50;
            
            // Item - Druid T13 Feral 2P Bonus (Savage Defense and Blood In The Water)
            if (procSpell && procSpell->Id == 33878 && HasAura(105725))
                chance = 100;
            
            if (!roll_chance_i(chance))
                return false;
            break;
        }
        // Vigilance
        case 50720:
        {   
            if (!victim)
                return false;

            target = triggeredByAura->GetCaster();

            // Add vengeance
            if (!target || target->GetTypeId() != TYPEID_PLAYER)
                break;

            if (victim->GetTypeId() == TYPEID_PLAYER || (victim->GetOwner() && victim->GetOwner()->GetTypeId() == TYPEID_PLAYER))
                break;

            if (!target->isInCombat())
                break;

            int32 aviableBasepoints = 0;
            int32 max_amount = 0;
                        
            if (Aura const* aur = target->GetAura(76691, target->GetGUID()))
            {
                aviableBasepoints += aur->GetEffect(EFFECT_0)->GetAmount();
                max_amount += aur->GetEffect(EFFECT_2)->GetAmount();
            }
   
            int32 cap = (target->GetCreateHealth() + target->GetStat(STAT_STAMINA) * 14) / 10;
            basepoints0 = int32((damage * triggerAmount) / 500); // 20% damage
            basepoints0 += aviableBasepoints;
            basepoints0 = std::min(cap, basepoints0);

            int32 basepoints1 = std::max(basepoints0, max_amount);

            target->CastCustomSpell(target, 76691, &basepoints0, &basepoints0, &basepoints1, true);

            break;
        }
        // Item - Dragon Soul Legendary Daggers
        case 109939:
        {
            if (!victim)
                return false;

            if (HasAura(109949))
                return false;

            if (Aura* aur = GetAura(109941))
            {
                uint8 stacks = aur->GetStackAmount();
                if (stacks >= 30)
                {
                    float chance = ((1.0f / (51.0f - stacks)) * 100);
                    if (roll_chance_f(chance))
                    {
                        CastSpell(victim, 109949, true);
                        aur->Remove();
                        return false;
                    }
                }
            }
            break;
        }
        // Fusing Vapors, Yor'sahj, Dragon Soul
        case 103968:
            if (GetHealthPct() > 50.0f)
                return false;
            break;
        // Embedded Blade, Mannoroth, Well of Eternity
        case 109542: 
            if (!victim)
                return false;
            target = victim;
            break;
        // Combat Potency
        case 35541:
        case 35550:
        case 35551:
            if (!procSpell & !(procFlags & PROC_FLAG_DONE_OFFHAND_ATTACK))
                return false;
            break;
        // Burnout, Alysrazor
        case 99432:
            if (!victim)
                return false;
            target = victim;
            break;
        // Molten Axe, Echo of Baine
        case 101836:
            if (!victim)
                return false;
            target = victim;
            break;
        // Molten Fists, Echo of Baine
        case 101866:
            if (!victim)
                return false;
            target = victim;
            break;
        // Fingers of Frost
        case 44543:
        case 44545:
        case 83074:
            if (Unit* original_caster = GetProcOwner())
                original_caster->CastSpell(original_caster, 44544, true);
            return true;
        case 324: // Lightning Shield
        case 30482: // Molten Armor
            if (victim && victim->isTotem())
                return false;
        break;
        // Invigoration
        case 53397:
            if (!procSpell)
                return false;

            if (procSpell->Id != 49966 && procSpell->Id != 16827 && procSpell->Id != 17253)
                return false;

            break;
        // Item - Hunter T12 2P Bonus
        case 99057:
            if (!victim || GetGUID() == victim->GetGUID())
                return false;
            break;
        // Nature's Grasp reapply handling
        case 16689:
            if (victim && victim->HasAura(19975))
                return false;
            break;
        // Glyph of Dark Succor
        case 96279:
            if (victim && victim->GetOwner() && victim->GetOwner()->GetGUID() == GetGUID())
                return false;
            break;
        // Tamed Pet Passive 07 (DND)
        case 20784:
        {
            if (!procSpell)
                return false;

            if (procSpell->Id != 49966 && procSpell->Id != 16827 && procSpell->Id != 17253)
                return false;

            if (Aura* aur = GetAura(trigger_spell_id))
                if (aur->GetStackAmount() >= 4)
                    if (Unit* owner = GetOwner())
                        owner->CastSpell(owner, 88843, true);
            break;
        }
        // Impact
        case 11103:
        case 12355:
        case 12358:
            // Can't proc from Molten Armor
            if (procSpell && procSpell->Id == 34913)
                return false;
            break;
        // Soul Leech
        case 30293:
        case 30295:
        {
            int32 bp = triggerAmount;
            CastCustomSpell(this, trigger_spell_id, &bp, &bp, 0, true);
            CastSpell(this, 57669, true, NULL, triggeredByAura);
            return true;
        }
        // Mana Feed
        case 91702:
        {
            if (Unit* owner = GetOwner())
            {
                if (AuraEffect const* aurEff = owner->GetAuraEffect(SPELL_AURA_DUMMY, SPELLFAMILY_WARLOCK, 1982, EFFECT_2))
                    basepoints0 = aurEff->GetAmount();
                else
                    return false;
            }
            else 
                return false;
            break;
        }
        // Lock'n'Load
        case 56342:
        case 56343:
        {
            if (procSpell->Id == 13797 || procSpell->Id == 13812 || procSpell->Id == 3674)
            {
                int32 _chance = 0;
                if (AuraEffect const* aurEff = GetAuraEffect(SPELL_AURA_DUMMY, SPELLFAMILY_HUNTER, 355, EFFECT_0))
                    _chance = aurEff->GetAmount();
                if (!_chance || !roll_chance_i(_chance))
                    return false;

                
            }
            if (GetTypeId() == TYPEID_PLAYER)
                ToPlayer()->RemoveSpellCooldown(53301, true);
            break;
        }
        // Item - Warrior T12 DPS 4P Bonus
        case 99238:
            // there are 3 Raging Blow spells, filter it
            if (!procSpell || !(procSpell->Id == 12294 || procSpell->Id == 96103))
                return false;
            break;
        case 90998: // Song of Sorrow, Sorrowsong
        case 91003: // Song of Sorrow, Sorrowsong (H)
        case 92180: // Item - Proc Armor, Leaden Despair
        case 92185: // Item - Proc Armor, Leaden Despair (H)
        case 92236: // Item - Proc Mastery Below 35%, Symbiotic Worm
        case 92356: // Item - Proc Mastery Below 35%, Symbiotic Worm (H)
        case 92234: // Item - Proc Dodge Below 35%, Bedrock Talisman
        case 96947: // Loom of Fate, Spediersilk Spindle
        case 97130: // Loom of Fate, Spediersilk Spindle (H)
        case 105552: // Item - Death Knight T12 Blood 2P Bonus
            if (!HealthBelowPct(35) && !HealthBelowPctDamaged(35, damage))
                return false;
            break;
        // Frostburn Formula
        case 96325:
            if (!victim || victim->getLevel() > 85)
                return true;
            break;
        // Rolling Thunder
        case 88756:
        case 88764:
        {
            if (!procSpell)
                return false;

            // Some spells has bad spellflags
            // So check spell id here
            if (procSpell->Id != 403 && procSpell->Id != 421 &&
                procSpell->Id != 45284 && procSpell->Id != 45297)
                return false;

            if (Aura* lightningShield = GetAura(324))
            {
                uint8 lsCharges = lightningShield->GetCharges();
                if (lsCharges < 9)
                {
                    lightningShield->SetCharges(lsCharges + 1);
                    lightningShield->RefreshDuration();
                }
                if (lsCharges > 7 && (HasAura(88766)))
                    CastSpell(this, 95774, true);
            }
            else 
                return false;
            break;
        }
        // Shadow Infusion
        case 48965:
        case 49571:
        case 49572:
        {
            if (!damage || GetTypeId() != TYPEID_PLAYER)
                return false;

            // Don't apply procs when pet is transformated
            if (Pet * pet = ToPlayer()->GetPet())
                if (pet->HasAura(63560))
                    return false;

            break;
        }
        // Will Of The Necropolis Ranks 1-3
        case 52284:
        case 81163:
        case 81164:
        {
            if (GetTypeId() != TYPEID_PLAYER)
                return false;

            if (HealthBelowPct(29) || (!HealthBelowPctDamaged(30, damage)) || this->ToPlayer()->HasSpellCooldown(trigger_spell_id))
                return false;

            this->ToPlayer()->RemoveSpellCooldown(48982,true); // Remove cooldown of rune tap
            CastSpell(this, 96171, true); // next rune tap wont cost runes
            cooldown = 45; // Can only happen once in 45 seconds
            break;
        }
        // Blood in the Water
        case 80318:
        case 80319:
            if (victim->GetHealthPct() > (HasAura(105725) ? 60 : triggerAmount))
                return false;
            break;
        // Crimson Scourge
        case 81141:
            if (victim && victim->HasAura(55078, this->GetGUID()))
                break;
            return false;
        // Masochism
        case 88994:
        case 88995:
            if (damage < CountPctFromMaxHealth(10))
                if (!procSpell || (procSpell && procSpell->Id != 32409))
                    return false;
            break;
        // Protector of the Innocent
        case 20138:
        case 20139:
        case 20140:
            if (!victim || (GetGUID() == victim->GetGUID()))
                return false;
            break;
        // Seals of Command
        case 85126:
            if (!victim || (GetGUID() == victim->GetGUID()))
                return false;

            if (!HasAura(31801) && !HasAura(20154) && !HasAura(20164))
                return false;
            
            break;
        // Arcane Concentration
        case 11213:
        case 12574:
        case 12575:
            if (!damage && !absorb)
                return false;
            break;
        // Seal of Insight
        case 20165:
        {
            if (!victim)
                return false;

            if (GetGUID() == victim->GetGUID())
                return false;
            
            break;
        }
        // Blessed Life
        case 31828:
        case 31829:
            if (GetTypeId() == TYPEID_PLAYER)
            {
                if (ToPlayer()->HasSpellCooldown(26022) ||
                    ToPlayer()->HasSpellCooldown(26023))
                    return false;
            }
            break;
        // Tower of Radiance
        case 84800:
        case 85511:
        case 85512:
            if (!victim->HasAura(53563))
                return false;
            target = this;
            trigger_spell_id = 88852;
            break;
        // Die by the Sword
        case 81913:
        case 81914:
            if (!(HealthBelowPct(20) || HealthBelowPctDamaged(20, damage)))
                return false;
            break;
        // Incite
        case 50685:
        case 50686:
        case 50687:
            if (HasAura(86627))
                return false;
            break;
        // Slaughter
        case 84583:
        case 84587:
        case 84588:
            if (Aura* aur = victim->GetAura(94009, GetGUID()))
                aur->RefreshDuration();
            break;
        // Impending Victory Rank 1
        // Impending Victory Rank 2
        case 80128:
        case 80129:
            if (!victim->HealthBelowPct(20))
                return false;
            break;
        // Empowered Imp
        case 54278:
            target = GetCharmerOrOwnerOrSelf();
            break;
        // Shield Specialization
        case 12298:
        case 12724:
        case 12725:
        {
            if (procEx & PROC_EX_REFLECT)
                basepoints0 = triggerEntry->Effects[0].BasePoints * 4.0f;
            break;
        }
        // Improved Hamstring
        case 12289:
        case 12668:
            return false;
        // Deep Wounds
        case 12834:
        case 12849:
        case 12867:
        {
            if (GetTypeId() != TYPEID_PLAYER)
                return false;

            if (AuraEffect * eff = victim->GetAuraEffect(12721, 0))
                basepoints0 = eff->GetAmount() * eff->GetBase()->GetDuration() / eff->GetBase()->GetMaxDuration();

            // now compute approximate weapon damage by formula from wowwiki.com
            Item* item = NULL;
            if (procFlags & PROC_FLAG_DONE_OFFHAND_ATTACK)
                item = ToPlayer()->GetUseableItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_OFFHAND);
            else
                item = ToPlayer()->GetUseableItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_MAINHAND);

            // dunno if it's really needed but will prevent any possible crashes
            if (!item)
                return false;

            float multiplier = 0.16f;
            if (auraSpellInfo->Id == 12849)
                multiplier = 0.32f;
            else if (auraSpellInfo->Id == 12867)
                multiplier = 0.48f;

            ItemTemplate const* weapon = item->GetTemplate();

            float weaponDPS = weapon->DPS;
            float attackPower = GetTotalAttackPowerValue(BASE_ATTACK) / 14.0f;
            float weaponSpeed = float(weapon->Delay) / 1000.0f;
            basepoints0 += int32((multiplier * (weaponDPS + attackPower) * weaponSpeed)) / 6;
            trigger_spell_id = 12721;
            break;
        }
        // Efflorescence
        case 34151:
        case 81274:
        case 81275:
        {
            basepoints0 = int32(CalculatePct(damage, triggerAmount));
            target = victim;
            trigger_spell_id = 81262;
            break;
        }
        // Persistent Shield (Scarab Brooch trinket)
        // This spell originally trigger 13567 - Dummy Trigger (vs dummy efect)
        case 26467:
        {
            basepoints0 = int32(CalculatePct(damage, 15));
            target = victim;
            trigger_spell_id = 26470;
            break;
        }
        // Sacred Shield Proc
        case 85285:
        {
            if (!HealthBelowPct(30) || !HealthBelowPctDamaged(30, damage))
                return false;

            basepoints0 = int32(1 + 2.8 * GetTotalAttackPowerValue(BASE_ATTACK));
            target = this;
            trigger_spell_id = 96263;
            break;
        }
        // Unyielding Knights (item exploit 29108\29109)
        case 38164:
        {
            if (!victim || victim->GetEntry() != 19457)  // Proc only if your target is Grillok
                return false;
            break;
        }
        // Deflection
        case 52420:
        {
            if (!HealthBelowPctDamaged(35, damage))
                return false;
            break;
        }

        // Cheat Death
        case 28845:
        {
            // When your health drops below 20%
            if (HealthBelowPctDamaged(20, damage) || HealthBelowPct(20))
                return false;
            break;
        }
        // Greater Heal Refund (Avatar Raiment set)
        case 37594:
        {
            if (!victim || !victim->isAlive())
                return false;

            // Doesn't proc if target already has full health
            if (victim->IsFullHealth())
                return false;
            // If your Greater Heal brings the target to full health, you gain $37595s1 mana.
            if (victim->GetHealth() + damage < victim->GetMaxHealth())
                return false;
            break;
        }
        // Bonus Healing (Crystal Spire of Karabor mace)
        case 40971:
        {
            // If your target is below $s1% health
            if (!victim || !victim->isAlive() || victim->HealthAbovePct(triggerAmount))
                return false;
            break;
        }
        // Rapid Recuperation
        case 53228:
        case 53232:
        {
            // This effect only from Rapid Fire (ability cast)
            if (!(procSpell->SpellFamilyFlags[0] & 0x20))
                return false;
            break;
        }
        // Decimation
        case 63156:
        case 63158:
            // Can proc only if target has hp below 25%
            if (!victim || !victim->HealthBelowPct(auraSpellInfo->Effects[EFFECT_1].CalcValue()))
                return false;
            break;
        // Deathbringer Saurfang - Blood Beast's Blood Link
        case 72176:
            basepoints0 = 3;
            break;
        // Professor Putricide - Ooze Spell Tank Protection
        case 71770:
            if (victim)
                victim->CastSpell(victim, trigger_spell_id, true);    // EffectImplicitTarget is self
            return true;
        case 45057: // Evasive Maneuvers (Commendation of Kael`thas trinket)
        case 71634: // Item - Icecrown 25 Normal Tank Trinket 1
        case 71640: // Item - Icecrown 25 Heroic Tank Trinket 1
        case 75475: // Item - Chamber of Aspects 25 Normal Tank Trinket
        case 75481: // Item - Chamber of Aspects 25 Heroic Tank Trinket
        {
            // Procs only if damage takes health below $s1%
            if (!HealthBelowPctDamaged(triggerAmount, damage))
                return false;
            break;
        }
        // Focus Magic
        case 54646:
        {
            Unit* caster = triggeredByAura->GetCaster();
            if (!caster)
                return false;

            caster->CastSpell(caster, 54648, true);
            return true;
        }
        // Ancestral Healing 
        case 16176:
        case 16235:
        {
            if (!victim)
                return false;

            AuraEffect *aura_eff = victim->GetAuraEffect(105284, EFFECT_0);

            int32 basepoints_ = std::min((aura_eff ? aura_eff->GetAmount() : 0.0f) + 
                damage * triggeredByAura->GetBase()->GetEffect(1)->GetAmount() / 100.0f, 
                victim->GetMaxHealth()/10.0f);

            CastCustomSpell(victim, 105284, &basepoints_, 0, 0, true);

            if (!(procEx & PROC_EX_CRITICAL_HIT))
                return false;

            target = victim;
            break;
        }
        default:
            break;
    }

    // Custom basepoints/target for exist spell
    // dummy basepoints or other customs
    switch (trigger_spell_id)
    {
        case 92931: // Pandemic
        {
            if (!victim || victim->GetHealthPct() > 25.0f)
                return false;
            break;
        }
        case 91342: // Shadow Infusion
        {
            if (GetTypeId() != TYPEID_PLAYER)
                return false;

            if (Pet* pet = ToPlayer()->GetPet())
            {
                uint8 stackAmount = 0;
                if (Aura* aura = pet->GetAura(trigger_spell_id))
                    stackAmount = aura->GetStackAmount();

                if (stackAmount >= 4) // Apply Dark Transformation
                    CastSpell(this, 93426, true);
            }
            break;
        }
        // Power Word: Barrier
        case 81782:
        case 90785:
            if (Aura* aur = victim->GetAura(trigger_spell_id))
            {
                aur->SetDuration(aur->GetMaxDuration(), true);
                return false;
            }
            break;
        // Auras which should proc on area aura source (caster in this case):
        // Cast positive spell on enemy target
        case 7099:  // Curse of Mending
        case 39703: // Curse of Mending
        case 29494: // Temptation
        case 20233: // Improved Lay on Hands (cast on target)
        {
            target = victim;
            break;
        }
        // Finish movies that add combo
        case 14189: // Seal Fate (Netherblade set)
        case 14157: // Ruthlessness
        {
            if (!victim || victim == this)
                return false;
            // Need add combopoint AFTER finish movie (or they dropped in finish phase)
            break;
        }
        // Item - Druid T10 Balance 2P Bonus
        case 16870:
        {
            if (HasAura(70718))
                CastSpell(this, 70721, true);
            break;
        }
        // Enlightenment (trigger only from mana cost spells)
        case 35095:
        {
            if (!procSpell || procSpell->PowerType != POWER_MANA || (procSpell->ManaCost == 0 && procSpell->ManaCostPercentage == 0 && procSpell->ManaCostPerlevel == 0))
                return false;
            break;
        }
        case 46916:  // Slam! (Bloodsurge proc)
        case 52437:  // Sudden Death
        {
            // Item - Warrior T10 Melee 4P Bonus
            if (AuraEffect const* aurEff = GetAuraEffect(70847, 0))
            {
                if (!roll_chance_i(aurEff->GetAmount()))
                    break;
                CastSpell(this, 70849, true, castItem, triggeredByAura); // Extra Charge!
                CastSpell(this, 71072, true, castItem, triggeredByAura); // Slam GCD Reduced
                CastSpell(this, 71069, true, castItem, triggeredByAura); // Execute GCD Reduced
            }
            break;
        }
        // Sword and Board
        case 50227:
        {
            // Remove cooldown on Shield Slam
            if (GetTypeId() == TYPEID_PLAYER)
                ToPlayer()->RemoveSpellCategoryCooldown(971, true);
            break;
        }
        // Maelstrom Weapon
        case 53817:
        {
            // Visual
            if (Aura* aur = GetAura(53817))
                if (aur->GetStackAmount()  >= 4)
                    CastSpell(this, 60349, true);

            // Item - Shaman T10 Enhancement 4P Bonus
            if (AuraEffect const* aurEff = GetAuraEffect(70832, 0))
                if (Aura const* maelstrom = GetAura(53817))
                    if ((maelstrom->GetStackAmount() == maelstrom->GetSpellInfo()->StackAmount - 1) && roll_chance_i(aurEff->GetAmount()))
                        CastSpell(this, 70831, true, castItem, triggeredByAura);
            break;
        }
        // Glyph of Death's Embrace
        case 58679:
        {
            // Proc only from healing part of Death Coil. Check is essential as all Death Coil spells have 0x2000 mask in SpellFamilyFlags
            if (!procSpell || !(procSpell->SpellFamilyName == SPELLFAMILY_DEATHKNIGHT && procSpell->SpellFamilyFlags[0] == 0x80002000))
                return false;

            // proc only on your pet
            if (victim && victim->GetOwner() != this)
                return false;

            break;
        }
        // Glyph of Death Grip
        case 58628:
        {
            // remove cooldown of Death Grip
            if (GetTypeId() == TYPEID_PLAYER)
                ToPlayer()->RemoveSpellCooldown(49576, true);
            return true;
        }
        // Body and Soul
        case 64128:
        case 65081:
        {
            // Proc only from PW:S cast
            if (!(procSpell->SpellFamilyFlags[0] & 0x00000001))
                return false;
            break;
        }
        // Culling the Herd
        case 70893:
        {
            // check if we're doing a critical hit
            if (!(procSpell->SpellFamilyFlags[1] & 0x10000000) && (procEx != PROC_EX_CRITICAL_HIT))
                return false;
            // check if we're procced by Claw, Bite or Smack (need to use the spell icon ID to detect it)
            if (!(procSpell->SpellIconID == 262 || procSpell->SpellIconID == 1680 || procSpell->SpellIconID == 473))
                return false;
            break;
        }
        // Shadow's Fate (Shadowmourne questline)
        case 71169:
        {
            if (GetTypeId() != TYPEID_PLAYER)
                return false;

            Player* player = ToPlayer();
            if (player->GetQuestStatus(24749) == QUEST_STATUS_INCOMPLETE)       // Unholy Infusion
            {
                if (!player->HasAura(71516) || victim->GetEntry() != 36678)    // Shadow Infusion && Professor Putricide
                    return false;
            }
            else if (player->GetQuestStatus(24756) == QUEST_STATUS_INCOMPLETE)  // Blood Infusion
            {
                if (!player->HasAura(72154) || victim->GetEntry() != 37955)    // Thirst Quenched && Blood-Queen Lana'thel
                    return false;
            }
            else if (player->GetQuestStatus(24757) == QUEST_STATUS_INCOMPLETE)  // Frost Infusion
            {
                if (!player->HasAura(72290) || victim->GetEntry() != 36853)    // Frost-Imbued Blade && Sindragosa
                    return false;
            }
            else if (player->GetQuestStatus(24547) != QUEST_STATUS_INCOMPLETE)  // A Feast of Souls
                return false;

            if (victim->GetTypeId() != TYPEID_UNIT)
                return false;
            // critters are not allowed
            if (victim->GetCreatureType() == CREATURE_TYPE_CRITTER)
                return false;
            break;
        }
        // Death's Advance
        case 96268:
        {
            if (!ToPlayer())
                return false;
            if (!ToPlayer()->GetRuneCooldown(RUNE_UNHOLY*2) || !ToPlayer()->GetRuneCooldown(RUNE_UNHOLY*2+1))
                return false;
            break;
        }
        case 12880: // Enrage (Enrage)
        case 14201:
        case 14202:
        case 57518: // Enrage (Wrecking Crew)
        case 57519:
        case 57520:
        {
            if (HasAura(12292)) // Death Wish
                return false;
            break;
        }
        case 81141: // Crimson Scourge
        {
            // proc only on Blood Plague affected targets
            if (!victim || !victim->HasAura(55078, GetGUID()))
                return false;
            break;
        }
        case 64803: // Entrapment
        {
            // Snake trap double proc handling
            if (procSpell && procSpell->Id == 57879)
                return false;
            break;
        }
        case 81340: // Sudden Doom
        {
            // Death Knight T13 DPS 2P Bonus
            if (AuraEffect * eff = GetAuraEffect(105609, 0))
            {
                if (roll_chance_i(eff->GetAmount()))
                {
                    AddAuraWithModCharges(trigger_spell_id, this, 2);
                    return true;
                }
            }
            break;
        }
        case 59052: // Rime - Freezing Fog
        {
            // Death Knight T13 DPS 2P Bonus
            if (AuraEffect * eff = GetAuraEffect(105609, 0))
            {
                if (roll_chance_i(eff->GetAmount() * 2))
                {
                    AddAuraWithModCharges(trigger_spell_id, this, 2);
                    return true;
                }
            }
            break;
        }
        case 81585: // Chakra: Serenity Renew update proc
        {
            if (procSpell->IsTargetingArea())
                return false;
            break;
        }
    }

    if (cooldown && GetTypeId() == TYPEID_PLAYER && ToPlayer()->HasSpellCooldown(trigger_spell_id))
        return false;

    // try detect target manually if not set
    if (target == NULL)
        target = !(procFlags & (PROC_FLAG_DONE_SPELL_MAGIC_DMG_CLASS_POS | PROC_FLAG_DONE_SPELL_NONE_DMG_CLASS_POS)) && triggerEntry && triggerEntry->IsPositive() ? this : victim;

    if (basepoints0 != 0) // basepoints can be negative
        CastCustomSpell(target, trigger_spell_id, &basepoints0, NULL, NULL, true, castItem, triggeredByAura);
    else
        CastSpell(target, trigger_spell_id, true, castItem, triggeredByAura);

    if (cooldown && GetTypeId() == TYPEID_PLAYER)
        ToPlayer()->AddSpellCooldown(trigger_spell_id, 0, time(NULL) + cooldown);

    if (trigger_spell_id == 26364)
    {
        // Improved Lightning Shield - A11 4/5 set bonus
        if (HasAura(100956) && triggeredByAura->GetBase()->GetCharges() < ((HasAura(88756) || HasAura(88764)) ? 9 : 3))
        {
            triggeredByAura->GetBase()->SetCharges(triggeredByAura->GetBase()->GetCharges() + 1);
            return false;
        }

        // Glyph of Lightning Shield
        if (HasAura(55448) && triggeredByAura->GetBase()->GetCharges() <= 3)
            return false;
    }

    return true;
}

bool Unit::HandleOverrideClassScriptAuraProc(Unit* victim, uint32 /*damage*/, AuraEffect* triggeredByAura, SpellInfo const* /*procSpell*/, uint32 cooldown)
{
    int32 scriptId = triggeredByAura->GetMiscValue();

    if (!victim || !victim->isAlive())
        return false;

    Item* castItem = triggeredByAura->GetBase()->GetCastItemGUID() && GetTypeId() == TYPEID_PLAYER
        ? ToPlayer()->GetItemByGuid(triggeredByAura->GetBase()->GetCastItemGUID()) : NULL;

    uint32 triggered_spell_id = 0;

    switch (scriptId)
    {
        // Dreamwalker Raiment 2 pieces bonus
        case 4533:
        {
            // Chance 50%
            if (!roll_chance_i(50))
                return false;

            switch (victim->getPowerType())
            {
                case POWER_MANA:   triggered_spell_id = 28722; break;
                case POWER_RAGE:   triggered_spell_id = 28723; break;
                case POWER_ENERGY: triggered_spell_id = 28724; break;
                default:
                    return false;
            }
            break;
        }
        // Dreamwalker Raiment 6 pieces bonus
        // Blessing of the Claw
        case 4537:
            triggered_spell_id = 28750;
            break;
        // Ice Shards (Ranl 1)
        case 836:
            triggered_spell_id = 12484;
            break;
        // Ice Shards (Ranl 2)
        case 988:
            triggered_spell_id = 12485;
            break;
        default:
            break;
    }

    // not processed
    if (!triggered_spell_id)
        return false;

    // standard non-dummy case
    SpellInfo const* triggerEntry = sSpellMgr->GetSpellInfo(triggered_spell_id);

    if (!triggerEntry)
        return false;

    if (cooldown && GetTypeId() == TYPEID_PLAYER && ToPlayer()->HasSpellCooldown(triggered_spell_id))
        return false;

    CastSpell(victim, triggered_spell_id, true, castItem, triggeredByAura);

    if (cooldown && GetTypeId() == TYPEID_PLAYER)
        ToPlayer()->AddSpellCooldown(triggered_spell_id, 0, time(NULL) + cooldown);

    return true;
}

void Unit::setPowerType(Powers new_powertype)
{
    SetByteValue(UNIT_FIELD_BYTES_0, 3, new_powertype);

    if (GetTypeId() == TYPEID_PLAYER)
    {
        if (ToPlayer()->GetGroup())
            ToPlayer()->SetGroupUpdateFlag(GROUP_UPDATE_FLAG_POWER_TYPE);
    }
    else if (Pet* pet = ToCreature()->ToPet())
    {
        if (pet->isControlled())
        {
            Unit* owner = GetOwner();
            if (owner && (owner->GetTypeId() == TYPEID_PLAYER) && owner->ToPlayer()->GetGroup())
                owner->ToPlayer()->SetGroupUpdateFlag(GROUP_UPDATE_FLAG_PET_POWER_TYPE);
        }
    }

    switch (new_powertype)
    {
        default:
        case POWER_MANA:
            break;
        case POWER_RAGE:
            SetMaxPower(POWER_RAGE, GetCreatePowers(POWER_RAGE));
            SetPower(POWER_RAGE, 0);
            break;
        case POWER_FOCUS:
            SetMaxPower(POWER_FOCUS, GetCreatePowers(POWER_FOCUS));
            SetPower(POWER_FOCUS, GetCreatePowers(POWER_FOCUS));
            break;
        case POWER_ENERGY:
            SetMaxPower(POWER_ENERGY, GetCreatePowers(POWER_ENERGY));
            break;
    }
    UpdateMaxPower(new_powertype);
}

FactionTemplateEntry const* Unit::getFactionTemplateEntry() const
{
    FactionTemplateEntry const* entry = sFactionTemplateStore.LookupEntry(getFaction());
    if (!entry)
    {
        static uint64 guid = 0;                             // prevent repeating spam same faction problem

        if (GetGUID() != guid)
        {
            if (Player const* player = ToPlayer())
                sLog->outError(LOG_FILTER_UNITS, "Player %s has invalid faction (faction template id) #%u", player->GetName(), getFaction());
            else if (Creature const* creature = ToCreature())
                sLog->outError(LOG_FILTER_UNITS, "Creature (template id: %u) has invalid faction (faction template id) #%u", creature->GetCreatureTemplate()->Entry, getFaction());
            else
                sLog->outError(LOG_FILTER_UNITS, "Unit (name=%s, type=%u) has invalid faction (faction template id) #%u", GetName(), uint32(GetTypeId()), getFaction());

            guid = GetGUID();
        }
    }
    return entry;
}

// function based on function Unit::UnitReaction from 13850 client
ReputationRank Unit::GetReactionTo(Unit const* target) const
{
    // always friendly to self
    if (this == target)
        return REP_FRIENDLY;

    // always friendly to charmer or owner
    if (GetCharmerOrOwnerOrSelf() == target->GetCharmerOrOwnerOrSelf())
        return REP_FRIENDLY;

    if (HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PVP_ATTACKABLE) || target->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PVP_ATTACKABLE))
    {
            Player const* selfPlayerOwner = GetAffectingPlayer();
            Player const* targetPlayerOwner = target->GetAffectingPlayer();

            if (selfPlayerOwner && targetPlayerOwner)
            {
                // always friendly to other unit controlled by player, or to the player himself
                if (selfPlayerOwner == targetPlayerOwner)
                    return REP_FRIENDLY;

                // duel - always hostile to opponent
                if (selfPlayerOwner->duel && selfPlayerOwner->duel->opponent == targetPlayerOwner && selfPlayerOwner->duel->startTime != 0)
                    return REP_HOSTILE;

                // same group - checks dependant only on our faction - skip FFA_PVP for example
                if (selfPlayerOwner->IsInRaidWith(targetPlayerOwner))
                    return REP_FRIENDLY; // return true to allow config option AllowTwoSide.Interaction.Group to work
                    // however client seems to allow mixed group parties, because in 13850 client it works like:
                    // return GetFactionReactionTo(getFactionTemplateEntry(), target);

                // check FFA_PVP
                if (selfPlayerOwner->GetByteValue(UNIT_FIELD_BYTES_2, 1) & UNIT_BYTE2_FLAG_FFA_PVP
                    && targetPlayerOwner->GetByteValue(UNIT_FIELD_BYTES_2, 1) & UNIT_BYTE2_FLAG_FFA_PVP)
                    return REP_HOSTILE;
            }

            // check FFA_PVP
            if (GetByteValue(UNIT_FIELD_BYTES_2, 1) & UNIT_BYTE2_FLAG_FFA_PVP
                && target->GetByteValue(UNIT_FIELD_BYTES_2, 1) & UNIT_BYTE2_FLAG_FFA_PVP)
                return REP_HOSTILE;

            if (selfPlayerOwner)
            {
                if (FactionTemplateEntry const* targetFactionTemplateEntry = target->getFactionTemplateEntry())
                {
                    if (ReputationRank const* repRank = selfPlayerOwner->GetReputationMgr().GetForcedRankIfAny(targetFactionTemplateEntry))
                        return *repRank;
                    if (!selfPlayerOwner->HasFlag(UNIT_FIELD_FLAGS_2, UNIT_FLAG2_IGNORE_REPUTATION))
                    {
                        if (FactionEntry const* targetFactionEntry = sFactionStore.LookupEntry(targetFactionTemplateEntry->faction))
                        {
                            if (targetFactionEntry->CanHaveReputation())
                            {
                                // check contested flags
                                if (targetFactionTemplateEntry->factionFlags & FACTION_TEMPLATE_FLAG_CONTESTED_GUARD
                                    && selfPlayerOwner->HasFlag(PLAYER_FLAGS, PLAYER_FLAGS_CONTESTED_PVP))
                                    return REP_HOSTILE;

                                // if faction has reputation, hostile state depends only from AtWar state
                                if (selfPlayerOwner->GetReputationMgr().IsAtWar(targetFactionEntry))
                                    return REP_HOSTILE;
                                return REP_FRIENDLY;
                            }
                        }
                    }
                }
        }
    }
    // do checks dependant only on our faction
    return GetFactionReactionTo(getFactionTemplateEntry(), target);
}

ReputationRank Unit::GetFactionReactionTo(FactionTemplateEntry const* factionTemplateEntry, Unit const* target)
{
    // always neutral when no template entry found
    if (!factionTemplateEntry)
        return REP_NEUTRAL;

    FactionTemplateEntry const* targetFactionTemplateEntry = target->getFactionTemplateEntry();
    if (!targetFactionTemplateEntry)
        return REP_NEUTRAL;

    if (Player const* targetPlayerOwner = target->GetAffectingPlayer())
    {
        // check contested flags
        if (factionTemplateEntry->factionFlags & FACTION_TEMPLATE_FLAG_CONTESTED_GUARD
            && targetPlayerOwner->HasFlag(PLAYER_FLAGS, PLAYER_FLAGS_CONTESTED_PVP))
            return REP_HOSTILE;
        if (ReputationRank const* repRank = targetPlayerOwner->GetReputationMgr().GetForcedRankIfAny(factionTemplateEntry))
            return *repRank;
        if (!target->HasFlag(UNIT_FIELD_FLAGS_2, UNIT_FLAG2_IGNORE_REPUTATION))
        {
            if (FactionEntry const* factionEntry = sFactionStore.LookupEntry(factionTemplateEntry->faction))
            {
                if (factionEntry->CanHaveReputation())
                {
                    // CvP case - check reputation, don't allow state higher than neutral when at war
                    ReputationRank repRank = targetPlayerOwner->GetReputationMgr().GetRank(factionEntry);
                    if (targetPlayerOwner->GetReputationMgr().IsAtWar(factionEntry))
                        repRank = std::min(REP_NEUTRAL, repRank);
                    return repRank;
                }
            }
        }
    }

    // common faction based check
    if (factionTemplateEntry->IsHostileTo(*targetFactionTemplateEntry))
        return REP_HOSTILE;
    if (factionTemplateEntry->IsFriendlyTo(*targetFactionTemplateEntry))
        return REP_FRIENDLY;
    if (targetFactionTemplateEntry->IsFriendlyTo(*factionTemplateEntry))
        return REP_FRIENDLY;
    if (factionTemplateEntry->factionFlags & FACTION_TEMPLATE_FLAG_HOSTILE_BY_DEFAULT)
        return REP_HOSTILE;
    // neutral by default
    return REP_NEUTRAL;
}

bool Unit::IsHostileTo(Unit const* unit) const
{
    return GetReactionTo(unit) <= REP_HOSTILE;
}

bool Unit::IsFriendlyTo(Unit const* unit) const
{
    return GetReactionTo(unit) >= REP_FRIENDLY;
}

bool Unit::IsHostileToPlayers() const
{
    FactionTemplateEntry const* my_faction = getFactionTemplateEntry();
    if (!my_faction || !my_faction->faction)
        return false;

    FactionEntry const* raw_faction = sFactionStore.LookupEntry(my_faction->faction);
    if (raw_faction && raw_faction->reputationListID >= 0)
        return false;

    return my_faction->IsHostileToPlayers();
}

bool Unit::IsNeutralToAll() const
{
    FactionTemplateEntry const* my_faction = getFactionTemplateEntry();
    if (!my_faction || !my_faction->faction)
        return true;

    FactionEntry const* raw_faction = sFactionStore.LookupEntry(my_faction->faction);
    if (raw_faction && raw_faction->reputationListID >= 0)
        return false;

    return my_faction->IsNeutralToAll();
}

bool Unit::Attack(Unit* victim, bool meleeAttack)
{
    if (!victim || victim == this)
        return false;

    // dead units can neither attack nor be attacked
    if (!isAlive() || !victim->IsInWorld() || !victim->isAlive())
        return false;

    // player cannot attack in mount state
    if (GetTypeId() == TYPEID_PLAYER && IsMounted())
        return false;

    // nobody can attack GM in GM-mode
    if (victim->GetTypeId() == TYPEID_PLAYER)
    {
        if (victim->ToPlayer()->isGameMaster())
            return false;
    }
    else
    {
        if (!victim->ToCreature() || victim->ToCreature()->IsInEvadeMode())
            return false;
    }

    // remove SPELL_AURA_MOD_UNATTACKABLE at attack (in case non-interruptible spells stun aura applied also that not let attack)
    if (HasAuraType(SPELL_AURA_MOD_UNATTACKABLE))
        RemoveAurasByType(SPELL_AURA_MOD_UNATTACKABLE);

    if (m_attacking)
    {
        if (m_attacking == victim)
        {
            // switch to melee attack from ranged/magic
            if (meleeAttack)
            {
                if (!HasUnitState(UNIT_STATE_MELEE_ATTACKING))
                {
                    AddUnitState(UNIT_STATE_MELEE_ATTACKING);
                    SendMeleeAttackStart(victim);
                    return true;
                }
            }
            else if (HasUnitState(UNIT_STATE_MELEE_ATTACKING))
            {
                ClearUnitState(UNIT_STATE_MELEE_ATTACKING);
                SendMeleeAttackStop(victim);
                return true;
            }
            return false;
        }

        // switch target
        InterruptSpell(CURRENT_MELEE_SPELL);
        if (!meleeAttack)
            ClearUnitState(UNIT_STATE_MELEE_ATTACKING);
    }

    if (m_attacking)
        m_attacking->_removeAttacker(this);

    m_attacking = victim;
    m_attacking->_addAttacker(this);

    // Set our target
    SetTarget(victim->GetGUID());

    if (meleeAttack)
        AddUnitState(UNIT_STATE_MELEE_ATTACKING);

    // set position before any AI calls/assistance
    //if (GetTypeId() == TYPEID_UNIT)
    //    ToCreature()->SetCombatStartPosition(GetPositionX(), GetPositionY(), GetPositionZ());

    if (GetTypeId() == TYPEID_UNIT && !ToCreature()->isPet() && !ToCreature()->isBattlegroundVehicle())
    {
        // should not let player enter combat by right clicking target - doesn't helps
        SetInCombatWith(victim);
        if (victim->GetTypeId() == TYPEID_PLAYER)
            victim->SetInCombatWith(this);
        AddThreat(victim, 0.0f);

        ToCreature()->SendAIReaction(AI_REACTION_HOSTILE);
        ToCreature()->CallAssistance();
    }

    if (GetTypeId() == TYPEID_PLAYER)
        if (ToPlayer()->GetEmoteState())
            ToPlayer()->SetEmoteState(0);

    // delay offhand weapon attack to next attack time
    if (haveOffhandWeapon())
        resetAttackTimer(OFF_ATTACK);

    if (meleeAttack)
        SendMeleeAttackStart(victim);

    // Let the pet know we've started attacking someting. Handles melee attacks only
    // Spells such as auto-shot and others handled in WorldSession::HandleCastSpellOpcode
    if (this->GetTypeId() == TYPEID_PLAYER)
    {
        Pet* playerPet = this->ToPlayer()->GetPet();

        if (playerPet && playerPet->isAlive())
            playerPet->AI()->OwnerAttacked(victim);

        for (std::set<uint64>::iterator itr = m_SummonNoSlot.begin(); itr != m_SummonNoSlot.end(); ++itr)
             if (Creature* summon = GetMap()->GetCreature(*itr))
                 if (summon->IsAIEnabled)
                     summon->AI()->OwnerAttacked(victim);
    }

    return true;
}

bool Unit::AttackStop()
{
    if (!m_attacking)
        return false;

    Unit* victim = m_attacking;

    m_attacking->_removeAttacker(this);
    m_attacking = NULL;

    // Clear our target
    SetTarget(0);

    ClearUnitState(UNIT_STATE_MELEE_ATTACKING);

    InterruptSpell(CURRENT_MELEE_SPELL);

    // reset only at real combat stop
    if (Creature* creature = ToCreature())
    {
        creature->SetNoCallAssistance(false);

        if (creature->HasSearchedAssistance())
        {
            creature->SetNoSearchAssistance(false);
            UpdateSpeed(MOVE_RUN, false);
        }
    }

    SendMeleeAttackStop(victim);

    if (GetTypeId() == TYPEID_PLAYER)
        if (ToPlayer()->GetEmoteState())
            ToPlayer()->SetEmoteState(0);

    return true;
}

void Unit::CombatStop(bool includingCast)
{
    if (includingCast && IsNonMeleeSpellCasted(false))
        InterruptNonMeleeSpells(false);

    AttackStop();
    RemoveAllAttackers();
    if (GetTypeId() == TYPEID_PLAYER)
        ToPlayer()->SendAttackSwingCancelAttack();     // melee and ranged forced attack cancel
    ClearInCombat();
}

void Unit::CombatStopWithPets(bool includingCast)
{
    CombatStop(includingCast);

    for (ControlList::const_iterator itr = m_Controlled.begin(); itr != m_Controlled.end(); ++itr)
        (*itr)->CombatStop(includingCast);
}

bool Unit::isAttackingPlayer() const
{
    if (HasUnitState(UNIT_STATE_ATTACK_PLAYER))
        return true;

    for (ControlList::const_iterator itr = m_Controlled.begin(); itr != m_Controlled.end(); ++itr)
        if ((*itr)->isAttackingPlayer())
            return true;

    for (uint8 i = 0; i < MAX_SUMMON_SLOT; ++i)
        if (m_SummonSlot[i])
            if (Creature* summon = GetMap()->GetCreature(m_SummonSlot[i]))
                if (summon->isAttackingPlayer())
                    return true;

    return false;
}

void Unit::RemoveAllAttackers()
{
    while (!m_attackers.empty())
    {
        AttackerSet::iterator iter = m_attackers.begin();
        if (!(*iter)->AttackStop())
            m_attackers.erase(iter);
    }
}

void Unit::ModifyAuraState(AuraStateType flag, bool apply)
{
    if (apply)
    {
        bool hasflag = HasFlag(UNIT_FIELD_AURASTATE, 1 << (flag-1));
        if (hasflag)
            RemoveFlag(UNIT_FIELD_AURASTATE, 1 << (flag-1));
    
        SetFlag(UNIT_FIELD_AURASTATE, 1 << (flag-1));

        if (hasflag == false)
        {
            if (GetTypeId() == TYPEID_PLAYER)
            {
                if (getClass() == CLASS_DEATH_KNIGHT && flag == AURA_STATE_DEFENSE)
                {
                    if (!HasAura(PRESENCE_BLOOD))
                        CastSpell(this, 56817, true);
                }
                PlayerSpellMap const& sp_list = ToPlayer()->GetSpellMap();
                for (PlayerSpellMap::const_iterator itr = sp_list.begin(); itr != sp_list.end(); ++itr)
                {
                    if (itr->second->state == PLAYERSPELL_REMOVED || itr->second->disabled)
                        continue;

                    SpellInfo const* spellInfo = sSpellMgr->GetSpellInfo(itr->first);
                    if (!spellInfo || !spellInfo->IsPassive())
                        continue;

                    if (spellInfo->CasterAuraState == uint32(flag))
                        CastSpell(this, itr->first, true, NULL);
                }
            }
            else if (Pet* pet = ToCreature()->ToPet())
            {
                for (PetSpellMap::const_iterator itr = pet->m_spells.begin(); itr != pet->m_spells.end(); ++itr)
                {
                    if (itr->second.state == PETSPELL_REMOVED)
                        continue;

                    SpellInfo const* spellInfo = sSpellMgr->GetSpellInfo(itr->first);
                    if (!spellInfo || !spellInfo->IsPassive())
                        continue;

                    if (spellInfo->CasterAuraState == uint32(flag))
                        CastSpell(this, itr->first, true, NULL);
                }
            }
        }
    }
    else
    {
        if (HasFlag(UNIT_FIELD_AURASTATE, 1 << (flag-1)))
        {
            RemoveFlag(UNIT_FIELD_AURASTATE, 1 << (flag-1));

            if (flag != AURA_STATE_ENRAGE)                  // enrage aura state triggering continues auras
            {
                Unit::AuraApplicationMap& tAuras = GetAppliedAuras();
                for (Unit::AuraApplicationMap::iterator itr = tAuras.begin(); itr != tAuras.end();)
                {
                    SpellInfo const* spellProto = (*itr).second->GetBase()->GetSpellInfo();
                    if (spellProto->CasterAuraState == uint32(flag))
                        RemoveAura(itr);
                    else
                        ++itr;
                }
            }
        }
    }
}

uint32 Unit::BuildAuraStateUpdateForTarget(Unit* target) const
{
    uint32 auraStates = GetUInt32Value(UNIT_FIELD_AURASTATE) &~(PER_CASTER_AURA_STATE_MASK);
    for (AuraStateAurasMap::const_iterator itr = m_auraStateAuras.begin(); itr != m_auraStateAuras.end(); ++itr)
        if ((1<<(itr->first-1)) & PER_CASTER_AURA_STATE_MASK)
            if (itr->second->GetBase()->GetCasterGUID() == target->GetGUID())
                auraStates |= (1<<(itr->first-1));

    return auraStates;
}

bool Unit::HasAuraState(AuraStateType flag, SpellInfo const* spellProto, Unit const* Caster) const
{
    if (Caster)
    {
        if (spellProto)
        {
            AuraEffectList const& stateAuras = Caster->GetAuraEffectsByType(SPELL_AURA_ABILITY_IGNORE_AURASTATE);
            for (AuraEffectList::const_iterator j = stateAuras.begin(); j != stateAuras.end(); ++j)
                if ((*j)->IsAffectingSpell(spellProto))
                    return true;
        }
        // Check per caster aura state
        // If aura with aurastate by caster not found return false
        if ((1<<(flag-1)) & PER_CASTER_AURA_STATE_MASK)
        {
            AuraStateAurasMapBounds range = m_auraStateAuras.equal_range(flag);
            for (AuraStateAurasMap::const_iterator itr = range.first; itr != range.second; ++itr)
                if (itr->second->GetBase()->GetCasterGUID() == Caster->GetGUID())
                    return true;
            return false;
        }
    }

    return HasFlag(UNIT_FIELD_AURASTATE, 1<<(flag-1));
}

void Unit::SetOwnerGUID(uint64 owner)
{
    if (GetOwnerGUID() == owner)
        return;
    
    m_ownerGuid = owner;
    SetUInt64Value(UNIT_FIELD_SUMMONEDBY, owner);
    if (!owner)
        return;

    // Update owner dependent fields
    Player* player = ObjectAccessor::GetPlayer(*this, owner);
    if (!player || !player->HaveAtClient(this)) // if player cannot see this unit yet, he will receive needed data with create object
        return;

    SetFieldNotifyFlag(UF_FLAG_OWNER);

    UpdateData udata(GetMapId());
    WorldPacket packet;
    BuildValuesUpdateBlockForPlayer(&udata, player);
    if (udata.BuildPacket(&packet))
        player->SendDirectMessage(&packet);

    RemoveFieldNotifyFlag(UF_FLAG_OWNER);
}

Unit* Unit::GetOwner() const
{
    if (uint64 ownerid = GetOwnerGUID())
    {
        return ObjectAccessor::GetUnit(*this, ownerid);
    }
    return NULL;
}

Unit* Unit::GetCharmer() const
{
    if (uint64 charmerid = GetCharmerGUID())
        return ObjectAccessor::GetUnit(*this, charmerid);
    return NULL;
}

Player* Unit::GetCharmerOrOwnerPlayerOrPlayerItself() const
{
    uint64 guid = GetCharmerOrOwnerGUID();
    if (IS_PLAYER_GUID(guid))
        return ObjectAccessor::GetPlayer(*this, guid);

    return GetTypeId() == TYPEID_PLAYER ? (Player*)this : NULL;
}

Player* Unit::GetAffectingPlayer() const
{
    if (!GetCharmerOrOwnerGUID())
        return GetTypeId() == TYPEID_PLAYER ? (Player*)this : NULL;

    if (Unit* owner = GetCharmerOrOwner())
        return owner->GetCharmerOrOwnerPlayerOrPlayerItself();
    return NULL;
}

Minion *Unit::GetFirstMinion() const
{
    if (uint64 pet_guid = GetMinionGUID())
    {
        if (Creature* pet = ObjectAccessor::GetCreatureOrPetOrVehicle(*this, pet_guid))
            if (pet->HasUnitTypeMask(UNIT_MASK_MINION))
                return (Minion*)pet;

        sLog->outError(LOG_FILTER_UNITS, "Unit::GetFirstMinion: Minion %u not exist.", GUID_LOPART(pet_guid));
        const_cast<Unit*>(this)->SetMinionGUID(0);
    }

    return NULL;
}

Guardian* Unit::GetGuardianPet() const
{
    if (uint64 pet_guid = GetPetGUID())
    {
        if (Creature* pet = ObjectAccessor::GetCreatureOrPetOrVehicle(*this, pet_guid))
            if (pet->HasUnitTypeMask(UNIT_MASK_GUARDIAN))
                return (Guardian*)pet;

        sLog->outFatal(LOG_FILTER_UNITS, "Unit::GetGuardianPet: Guardian " UI64FMTD " not exist.", pet_guid);
        const_cast<Unit*>(this)->SetPetGUID(0);
    }

    return NULL;
}

Unit* Unit::GetCharm() const
{
    if (uint64 charm_guid = GetCharmGUID())
    {
        if (Unit* pet = ObjectAccessor::GetUnit(*this, charm_guid))
            return pet;

        sLog->outError(LOG_FILTER_UNITS, "Unit::GetCharm: Charmed creature %u not exist.", GUID_LOPART(charm_guid));
        const_cast<Unit*>(this)->SetUInt64Value(UNIT_FIELD_CHARM, 0);
    }

    return NULL;
}

void Unit::SetMinion(Minion *minion, bool apply, PetSlot slot)
{

    if (apply)
    {
        if (minion->GetOwnerGUID())
            return;

        minion->SetOwnerGUID(GetGUID());

        m_Controlled.insert(minion);

        if (GetTypeId() == TYPEID_PLAYER)
        {
            minion->m_ControlledByPlayer = true;
            minion->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PVP_ATTACKABLE);
        }

        // Can only have one pet. If a new one is summoned, dismiss the old one.
        if (minion->IsGuardianPet())
        {
            if (Guardian* oldPet = GetGuardianPet())
            {
                if (oldPet != minion && (oldPet->isPet() || minion->isPet() || oldPet->GetEntry() != minion->GetEntry()))
                {
                    // remove existing minion pet
                    if (oldPet->isPet())
                        ((Pet*)oldPet)->Remove(PET_SLOT_ACTUAL_PET_SLOT);
                    else
                        oldPet->UnSummon();
                    SetPetGUID(minion->GetGUID());
                    SetMinionGUID(0);
                }
            }
            else
            {
                SetPetGUID(minion->GetGUID());
                SetMinionGUID(0);
            }

            if (slot == PET_SLOT_UNK_SLOT)
            {
                if (minion->isPet() && minion->ToPet()->getPetType() == HUNTER_PET)
                    assert(false);

                slot = PET_SLOT_OTHER_PET;
            }

            if (GetTypeId() == TYPEID_PLAYER)
            {
                if (!minion->isHunterPet()) // If its not a hunter pet, well lets not try to use it for hunter then
                {
                    ToPlayer()->m_currentPetSlot = slot;
                    ToPlayer()->m_petSlotUsed = 3452816845; // the same as 100 so that the pet is only that and nothing more
                    // ToPlayer()->setPetSlotUsed(slot, true);
                }

                if (slot >= PET_SLOT_HUNTER_FIRST && slot <= PET_SLOT_HUNTER_LAST) // Always save thoose spots where hunter is correct
                {
                    ToPlayer()->m_currentPetSlot = slot;
                    ToPlayer()->setPetSlotUsed(slot, true);
                }
            }
        }

        if (minion->HasUnitTypeMask(UNIT_MASK_CONTROLABLE_GUARDIAN))
        {
            if (AddUInt64Value(UNIT_FIELD_SUMMON, minion->GetGUID()))
            {
            }
        }

        if (minion->m_Properties && minion->m_Properties->Type == SUMMON_TYPE_MINIPET)
        {
            SetCritterGUID(minion->GetGUID());
        }

        // PvP, FFAPvP
        minion->SetByteValue(UNIT_FIELD_BYTES_2, 1, GetByteValue(UNIT_FIELD_BYTES_2, 1));

        // FIXME: hack, speed must be set only at follow
        if (GetTypeId() == TYPEID_PLAYER && minion->isPet())
            for (uint8 i = 0; i < MAX_MOVE_TYPE; ++i)
                minion->SetSpeed(UnitMoveType(i), m_speed_rate[i], true);

        // Ghoul pets have energy instead of mana (is anywhere better place for this code?)
        if (minion->IsPetGhoul())
        {
            minion->setPowerType(POWER_ENERGY);
            minion->SetPower(POWER_ENERGY, 100);
        }

        if (GetTypeId() == TYPEID_PLAYER)
        {
            // Send infinity cooldown - client does that automatically but after relog cooldown needs to be set again
            SpellInfo const* spellInfo = sSpellMgr->GetSpellInfo(minion->GetUInt32Value(UNIT_CREATED_BY_SPELL));
            if (spellInfo && (spellInfo->Attributes & SPELL_ATTR0_DISABLED_WHILE_ACTIVE))
                ToPlayer()->AddSpellAndCategoryCooldowns(spellInfo, 0, NULL, true);
        }
    }
    else
    {
        if (minion->GetOwnerGUID() != GetGUID())
        {
            sLog->outFatal(LOG_FILTER_UNITS, "SetMinion: Minion %u is not the minion of owner %u", minion->GetEntry(), GetEntry());
            return;
        }

        m_Controlled.erase(minion);

        if (minion->m_Properties && minion->m_Properties->Type == SUMMON_TYPE_MINIPET)
        {
            if (GetCritterGUID() == minion->GetGUID())
                SetCritterGUID(0);
        }

        if (minion->IsGuardianPet())
        {
            if (GetPetGUID() == minion->GetGUID())
                SetPetGUID(0);
        }
        else if (minion->isTotem())
        {
            // All summoned by totem minions must disappear when it is removed.
        if (SpellInfo const* spInfo = sSpellMgr->GetSpellInfo(minion->ToTotem()->GetSpell()))
            for (int i = 0; i < MAX_SPELL_EFFECTS; ++i)
            {
                if (spInfo->Effects[i].Effect != SPELL_EFFECT_SUMMON)
                    continue;

                RemoveAllMinionsByEntry(spInfo->Effects[i].MiscValue);
            }
        }

        if (GetTypeId() == TYPEID_PLAYER)
        {
            SpellInfo const* spellInfo = sSpellMgr->GetSpellInfo(minion->GetUInt32Value(UNIT_CREATED_BY_SPELL));
            // Remove infinity cooldown
            if (spellInfo && (spellInfo->Attributes & SPELL_ATTR0_DISABLED_WHILE_ACTIVE))
                ToPlayer()->SendCooldownEvent(spellInfo);
        }

        //if (minion->HasUnitTypeMask(UNIT_MASK_GUARDIAN))
        {
            if (RemoveUInt64Value(UNIT_FIELD_SUMMON, minion->GetGUID()))
            {
                // Check if there is another minion
                for (ControlList::iterator itr = m_Controlled.begin(); itr != m_Controlled.end(); ++itr)
                {
                    // do not use this check, creature do not have charm guid
                    //if (GetCharmGUID() == (*itr)->GetGUID())
                    if (GetGUID() == (*itr)->GetCharmerGUID())
                        continue;

                    //ASSERT((*itr)->GetOwnerGUID() == GetGUID());
                    if ((*itr)->GetOwnerGUID() != GetGUID())
                    {
                        OutDebugInfo();
                        (*itr)->OutDebugInfo();
                        ASSERT(false);
                    }
                    ASSERT((*itr)->GetTypeId() == TYPEID_UNIT);

                    if (!(*itr)->HasUnitTypeMask(UNIT_MASK_CONTROLABLE_GUARDIAN))
                        continue;

                    if (AddUInt64Value(UNIT_FIELD_SUMMON, (*itr)->GetGUID()))
                    {
                        // show another pet bar if there is no charm bar
                        if (GetTypeId() == TYPEID_PLAYER && !GetCharmGUID())
                        {
                            if ((*itr)->isPet())
                                ToPlayer()->PetSpellInitialize();
                            else
                                ToPlayer()->CharmSpellInitialize();
                        }
                    }
                    break;
                }
            }
        }
    }
}

void Unit::GetAllMinionsByEntry(std::list<Creature*>& Minions, uint32 entry)
{
    for (Unit::ControlList::iterator itr = m_Controlled.begin(); itr != m_Controlled.end();)
    {
        Unit* unit = *itr;
        ++itr;
        if (unit->GetEntry() == entry && unit->GetTypeId() == TYPEID_UNIT
            && unit->ToCreature()->isSummon()) // minion, actually
            Minions.push_back(unit->ToCreature());
    }
}

void Unit::RemoveAllMinionsByEntry(uint32 entry)
{
    for (Unit::ControlList::iterator itr = m_Controlled.begin(); itr != m_Controlled.end();)
    {
        Unit* unit = *itr;
        ++itr;
        if (unit->GetEntry() == entry && unit->GetTypeId() == TYPEID_UNIT
            && unit->ToCreature()->isSummon()) // minion, actually
            unit->ToTempSummon()->UnSummon();
        // i think this is safe because i have never heard that a despawned minion will trigger a same minion
    }
}

void Unit::SetCharm(Unit* charm, bool apply)
{
    if (apply)
    {
        if (GetTypeId() == TYPEID_PLAYER)
        {
            if (!AddUInt64Value(UNIT_FIELD_CHARM, charm->GetGUID()))
                sLog->outFatal(LOG_FILTER_UNITS, "Player %s is trying to charm unit %u, but it already has a charmed unit " UI64FMTD "", GetName(), charm->GetEntry(), GetCharmGUID());

            charm->m_ControlledByPlayer = true;
            // TODO: maybe we can use this flag to check if controlled by player
            charm->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PVP_ATTACKABLE);
        }
        else
            charm->m_ControlledByPlayer = false;

        // PvP, FFAPvP
        charm->SetByteValue(UNIT_FIELD_BYTES_2, 1, GetByteValue(UNIT_FIELD_BYTES_2, 1));

        if (!charm->AddUInt64Value(UNIT_FIELD_CHARMEDBY, GetGUID()))
            sLog->outFatal(LOG_FILTER_UNITS, "Unit %u is being charmed, but it already has a charmer " UI64FMTD "", charm->GetEntry(), charm->GetCharmerGUID());
        else
            charm->m_charmerGuid = GetGUID();

        _isWalkingBeforeCharm = charm->IsWalking();
        if (_isWalkingBeforeCharm)
        {
            charm->SetWalk(false);
            charm->SendMovementFlagUpdate();
        }

        m_Controlled.insert(charm);
    }
    else
    {
        if (GetTypeId() == TYPEID_PLAYER)
        {
            if (!RemoveUInt64Value(UNIT_FIELD_CHARM, charm->GetGUID()))
                sLog->outFatal(LOG_FILTER_UNITS, "Player %s is trying to uncharm unit %u, but it has another charmed unit " UI64FMTD "", GetName(), charm->GetEntry(), GetCharmGUID());
        }

        if (!charm->RemoveUInt64Value(UNIT_FIELD_CHARMEDBY, GetGUID()))
            sLog->outFatal(LOG_FILTER_UNITS, "Unit %u is being uncharmed, but it has another charmer " UI64FMTD "", charm->GetEntry(), charm->GetCharmerGUID());
        else
            charm->m_charmerGuid = 0;

        if (charm->GetTypeId() == TYPEID_PLAYER)
        {
            charm->m_ControlledByPlayer = true;
            charm->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PVP_ATTACKABLE);
            charm->ToPlayer()->UpdatePvPState();
        }
        else if (Player* player = charm->GetCharmerOrOwnerPlayerOrPlayerItself())
        {
            charm->m_ControlledByPlayer = true;
            charm->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PVP_ATTACKABLE);
            charm->SetByteValue(UNIT_FIELD_BYTES_2, 1, player->GetByteValue(UNIT_FIELD_BYTES_2, 1));
        }
        else
        {
            charm->m_ControlledByPlayer = false;
            charm->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PVP_ATTACKABLE);
            charm->SetByteValue(UNIT_FIELD_BYTES_2, 1, 0);
        }

        if (charm->IsWalking() != _isWalkingBeforeCharm)
        {
            charm->SetWalk(_isWalkingBeforeCharm);
            charm->SendMovementFlagUpdate(true); // send packet to self, to update movement state on player.
        }

        if (charm->GetTypeId() == TYPEID_PLAYER
            || !charm->ToCreature()->HasUnitTypeMask(UNIT_MASK_MINION)
            || charm->GetOwnerGUID() != GetGUID())
            m_Controlled.erase(charm);
    }
}

int32 Unit::DealHeal(Unit* victim, uint32 addhealth)
{
    int32 gain = 0;

    if (victim->IsAIEnabled)
        victim->GetAI()->HealReceived(this, addhealth);

    if (IsAIEnabled)
        GetAI()->HealDone(victim, addhealth);

    if (addhealth)
        gain = victim->ModifyHealth(int32(addhealth));

    Unit* unit = this;

    if (GetTypeId() == TYPEID_UNIT && ToCreature()->isTotem())
        unit = GetOwner();

    if (Player* player = unit->ToPlayer())
    {
        if (Battleground* bg = player->GetBattleground())
            bg->UpdatePlayerScore(player, SCORE_HEALING_DONE, gain);

        // use the actual gain, as the overheal shall not be counted, skip gain 0 (it ignored anyway in to criteria)
        if (gain)
            player->UpdateAchievementCriteria(ACHIEVEMENT_CRITERIA_TYPE_HEALING_DONE, gain, 0, 0, victim);

        player->UpdateAchievementCriteria(ACHIEVEMENT_CRITERIA_TYPE_HIGHEST_HEAL_CASTED, addhealth);
    }

    if (Player* player = victim->ToPlayer())
    {
        player->UpdateAchievementCriteria(ACHIEVEMENT_CRITERIA_TYPE_TOTAL_HEALING_RECEIVED, gain);
        player->UpdateAchievementCriteria(ACHIEVEMENT_CRITERIA_TYPE_HIGHEST_HEALING_RECEIVED, addhealth);
    }

    return gain;
}

Unit* Unit::GetMagicHitRedirectTarget(Unit* victim, SpellInfo const* spellInfo)
{
    // Patch 1.2 notes: Spell Reflection no longer reflects abilities
    if (spellInfo->Attributes & SPELL_ATTR0_ABILITY || spellInfo->AttributesEx & SPELL_ATTR1_CANT_BE_REDIRECTED || spellInfo->Attributes & SPELL_ATTR0_UNAFFECTED_BY_INVULNERABILITY)
        return victim;

    Unit::AuraEffectList const& magnetAuras = victim->GetAuraEffectsByType(SPELL_AURA_SPELL_MAGNET);
    for (Unit::AuraEffectList::const_iterator itr = magnetAuras.begin(); itr != magnetAuras.end(); ++itr)
    {
        if (Unit* magnet = (*itr)->GetBase()->GetCaster())
            if (spellInfo->CheckExplicitTarget(this, magnet) == SPELL_CAST_OK
                && (IsWithinLOSInMap(magnet)
                || magnet->isTotem()))
            {
                return magnet;
            }
    }
    return victim;
}

Unit* Unit::GetMeleeHitRedirectTarget(Unit* victim, SpellInfo const* spellInfo)
{
    AuraEffectList const& hitTriggerAuras = victim->GetAuraEffectsByType(SPELL_AURA_ADD_CASTER_HIT_TRIGGER);
    for (AuraEffectList::const_iterator i = hitTriggerAuras.begin(); i != hitTriggerAuras.end(); ++i)
    {
        if (Unit* magnet = (*i)->GetBase()->GetCaster())
            if (_IsValidAttackTarget(magnet, spellInfo) && magnet->IsWithinLOSInMap(this)
                && (!spellInfo || (spellInfo->CheckExplicitTarget(this, magnet) == SPELL_CAST_OK
                && spellInfo->CheckTarget(this, magnet, false) == SPELL_CAST_OK)))
                if (roll_chance_i((*i)->GetAmount()))
                {
                    (*i)->GetBase()->DropCharge(AURA_REMOVE_BY_EXPIRE);
                    return magnet;
                }
    }
    return victim;
}

Unit* Unit::GetFirstControlled() const
{
    // Sequence: charmed, pet, other guardians
    Unit* unit = GetCharm();
    if (!unit)
        if (uint64 guid = GetMinionGUID())
            unit = ObjectAccessor::GetUnit(*this, guid);

    return unit;
}

void Unit::RemoveAllControlled()
{
    // possessed pet and vehicle
    if (GetTypeId() == TYPEID_PLAYER)
        ToPlayer()->StopCastingCharm();

    while (!m_Controlled.empty())
    {
        Unit* target = *m_Controlled.begin();
        m_Controlled.erase(m_Controlled.begin());
        if (target->GetCharmerGUID() == GetGUID())
            target->RemoveCharmAuras();
        else if (target->GetOwnerGUID() == GetGUID() && target->isSummon())
            target->ToTempSummon()->UnSummon();
        else
            sLog->outError(LOG_FILTER_UNITS, "Unit %u is trying to release unit %u which is neither charmed nor owned by it", GetEntry(), target->GetEntry());
    }
    if (GetPetGUID())
        sLog->outFatal(LOG_FILTER_UNITS, "Unit %u is not able to release its pet " UI64FMTD, GetEntry(), GetPetGUID());
    if (GetMinionGUID())
        sLog->outFatal(LOG_FILTER_UNITS, "Unit %u is not able to release its minion " UI64FMTD, GetEntry(), GetMinionGUID());
    if (GetCharmGUID())
        sLog->outFatal(LOG_FILTER_UNITS, "Unit %u is not able to release its charm " UI64FMTD, GetEntry(), GetCharmGUID());
}

Unit* Unit::GetNextRandomRaidMemberOrPet(float radius)
{
    Player* player = NULL;
    if (GetTypeId() == TYPEID_PLAYER)
        player = ToPlayer();
    // Should we enable this also for charmed units?
    else if (GetTypeId() == TYPEID_UNIT && ToCreature()->isPet())
        player = GetOwner()->ToPlayer();

    if (!player)
        return NULL;
    Group* group = player->GetGroup();
    // When there is no group check pet presence
    if (!group)
    {
        // We are pet now, return owner
        if (player != this)
            return IsWithinDistInMap(player, radius) ? player : NULL;
        Unit* pet = GetGuardianPet();
        // No pet, no group, nothing to return
        if (!pet)
            return NULL;
        // We are owner now, return pet
        return IsWithinDistInMap(pet, radius) ? pet : NULL;
    }

    std::vector<Unit*> nearMembers;
    // reserve place for players and pets because resizing vector every unit push is unefficient (vector is reallocated then)
    nearMembers.reserve(group->GetMembersCount() * 2);

    for (GroupReference* itr = group->GetFirstMember(); itr != NULL; itr = itr->next())
        if (Player* Target = itr->getSource())
        {
            // IsHostileTo check duel and controlled by enemy
            if (Target != this && Target->isAlive() && IsWithinDistInMap(Target, radius) && !IsHostileTo(Target))
                nearMembers.push_back(Target);

        // Push player's pet to vector
        if (Unit* pet = Target->GetGuardianPet())
            if (pet != this && pet->isAlive() && IsWithinDistInMap(pet, radius) && !IsHostileTo(pet))
                nearMembers.push_back(pet);
        }

    if (nearMembers.empty())
        return NULL;

    uint32 randTarget = urand(0, nearMembers.size()-1);
    return nearMembers[randTarget];
}

// only called in Player::SetSeer
// so move it to Player?
void Unit::AddPlayerToVision(Player* player)
{
    if (m_sharedVision.empty())
    {
        setActive(true);
        SetWorldObject(true);
    }
    m_sharedVision.push_back(player);
}

// only called in Player::SetSeer
void Unit::RemovePlayerFromVision(Player* player)
{
    m_sharedVision.remove(player);
    if (m_sharedVision.empty())
    {
        setActive(false);
        SetWorldObject(false);
    }
}

void Unit::RemoveBindSightAuras()
{
    RemoveAurasByType(SPELL_AURA_BIND_SIGHT);
}

void Unit::RemoveCharmAuras()
{
    RemoveAurasByType(SPELL_AURA_MOD_CHARM);
    RemoveAurasByType(SPELL_AURA_MOD_POSSESS_PET);
    RemoveAurasByType(SPELL_AURA_MOD_POSSESS);
    RemoveAurasByType(SPELL_AURA_AOE_CHARM);
}

void Unit::UnsummonAllTotems()
{
    for (uint8 i = 0; i < MAX_SUMMON_SLOT; ++i)
    {
        if (!m_SummonSlot[i])
            continue;

        if (Creature* OldTotem = GetMap()->GetCreature(m_SummonSlot[i]))
            if (OldTotem->isSummon())
                OldTotem->ToTempSummon()->UnSummon();
    }
}

void Unit::SendHealSpellLog(Unit* victim, uint32 SpellID, uint32 Damage, uint32 OverHeal, uint32 Absorb, bool critical)
{
    // we guess size
    WorldPacket data(SMSG_SPELLHEALLOG, 8+8+4+4+4+4+1+1);
    data.append(victim->GetPackGUID());
    data.append(GetPackGUID());
    data << uint32(SpellID);
    data << uint32(Damage);
    data << uint32(OverHeal);
    data << uint32(Absorb); // Absorb amount
    data << uint8(critical ? 1 : 0);
    data << uint8(0); // unused
    SendMessageToSet(&data, true);
}

int32 Unit::HealBySpell(Unit* victim, SpellInfo const* spellInfo, uint32 addHealth, bool critical)
{
    uint32 absorb = 0;
    // calculate heal absorb and reduce healing
    CalcHealAbsorb(victim, spellInfo, addHealth, absorb);

    int32 gain = DealHeal(victim, addHealth);
    SendHealSpellLog(victim, spellInfo->Id, addHealth, uint32(addHealth - gain), absorb, critical);
    return gain;
}

void Unit::SendEnergizeSpellLog(Unit* victim, uint32 spellID, uint32 damage, Powers powerType)
{
    WorldPacket data(SMSG_SPELLENERGIZELOG, (8+8+4+4+4+1));
    data.append(victim->GetPackGUID());
    data.append(GetPackGUID());
    data << uint32(spellID);
    data << uint32(powerType);
    data << uint32(damage);
    SendMessageToSet(&data, true);
}

void Unit::EnergizeBySpell(Unit* victim, uint32 spellID, int32 damage, Powers powerType)
{
    SendEnergizeSpellLog(victim, spellID, damage, powerType);
    // needs to be called after sending spell log
    victim->ModifyPower(powerType, damage);

    SpellInfo const* spellInfo = sSpellMgr->GetSpellInfo(spellID);
    victim->getHostileRefManager().threatAssist(this, float(damage) * 0.5f, spellInfo);
}

uint32 Unit::SpellDamageBonusDone(Unit* victim, SpellInfo const* spellProto, uint32 pdamage, DamageEffectType damagetype, uint32 stack)
{
    if (!spellProto || !victim || damagetype == DIRECT_DAMAGE)
        return pdamage;

    // Some spells don't benefit from done mods
    if (spellProto->HasAttribute(SPELL_ATTR3_NO_DONE_BONUS) || spellProto->HasAttribute(SPELL_ATTR6_NO_DONE_PCT_DAMAGE_MODS))
        return pdamage;

    // For totems get damage bonus from owner
    if (GetTypeId() == TYPEID_UNIT && ToCreature()->isTotem())
        if (Unit* owner = GetOwner())
            return owner->SpellDamageBonusDone(victim, spellProto, pdamage, damagetype);

    // Done total percent damage auras
    float DoneTotalMod = 1.0f;
    float ApCoeffMod = 1.0f;
    int32 DoneTotal = 0;
    bool AlreadyCalculated = false;

    // Pet damage?
    if (GetTypeId() == TYPEID_UNIT && !ToCreature()->isPet())
        DoneTotalMod *= ToCreature()->GetSpellDamageMod(ToCreature()->GetCreatureTemplate()->rank);

    // Some spells don't benefit from pct done mods
    if (!spellProto->IsRankOf(sSpellMgr->GetSpellInfo(12162)))
    {
        AuraEffectList const& mModDamagePercentDone = GetAuraEffectsByType(SPELL_AURA_MOD_DAMAGE_PERCENT_DONE);
        for (AuraEffectList::const_iterator i = mModDamagePercentDone.begin(); i != mModDamagePercentDone.end(); ++i)
        {
            if (spellProto->EquippedItemClass == -1 && (*i)->GetSpellInfo()->EquippedItemClass != -1)    //prevent apply mods from weapon specific case to non weapon specific spells (Example: thunder clap and two-handed weapon specialization)
                continue;

            if ((*i)->GetMiscValue() & spellProto->GetSchoolMask())
            {
                const SpellInfo* _info = (*i)->GetSpellInfo();
                if (_info->EquippedItemClass == -1)
                    AddPct(DoneTotalMod, (*i)->GetAmount());
                else if (!_info->HasAttribute(SPELL_ATTR5_SPECIAL_ITEM_CLASS_CHECK) && (_info->EquippedItemSubClassMask == 0))
                    AddPct(DoneTotalMod, (*i)->GetAmount());
                else if (ToPlayer() && ToPlayer()->HasItemFitToSpellRequirements(_info))
                    AddPct(DoneTotalMod, (*i)->GetAmount());
            }
        }
    }

    uint32 creatureTypeMask = victim->GetCreatureTypeMask();
    // Add flat bonus from spell damage versus
    DoneTotal += GetTotalAuraModifierByMiscMask(SPELL_AURA_MOD_FLAT_SPELL_DAMAGE_VERSUS, creatureTypeMask);
    AuraEffectList const& mDamageDoneVersus = GetAuraEffectsByType(SPELL_AURA_MOD_DAMAGE_DONE_VERSUS);
    for (AuraEffectList::const_iterator i = mDamageDoneVersus.begin(); i != mDamageDoneVersus.end(); ++i)
        if (creatureTypeMask & uint32((*i)->GetMiscValue()))
            AddPct(DoneTotalMod, (*i)->GetAmount());

    // bonus against aurastate
    AuraEffectList const& mDamageDoneVersusAurastate = GetAuraEffectsByType(SPELL_AURA_MOD_DAMAGE_DONE_VERSUS_AURASTATE);
    for (AuraEffectList::const_iterator i = mDamageDoneVersusAurastate.begin(); i != mDamageDoneVersusAurastate.end(); ++i)
        if (victim->HasAuraState(AuraStateType((*i)->GetMiscValue())))
            AddPct(DoneTotalMod, (*i)->GetAmount());

    // Add SPELL_AURA_MOD_DAMAGE_DONE_FOR_MECHANIC percent bonus
    AddPct(DoneTotalMod, GetTotalAuraModifierByMiscValue(SPELL_AURA_MOD_DAMAGE_DONE_FOR_MECHANIC, spellProto->Mechanic));

    // done scripted mod (take it from owner)
    Unit* owner = GetOwner() ? GetOwner() : this;
    AuraEffectList const& mOverrideClassScript= owner->GetAuraEffectsByType(SPELL_AURA_OVERRIDE_CLASS_SCRIPTS);
    for (AuraEffectList::const_iterator i = mOverrideClassScript.begin(); i != mOverrideClassScript.end(); ++i)
    {
        if (!(*i)->IsAffectingSpell(spellProto))
            continue;

        switch ((*i)->GetMiscValue())
        {
            case 4920:  // Molten Fury (Rank 1)
            case 4919:  // Molten Fury (Rank 2)
            case 12368: // Molten Fury (Rank 3)
            {
                if (victim->HasAuraState(AURA_STATE_HEALTHLESS_35_PERCENT, spellProto, this))
                    AddPct(DoneTotalMod, (*i)->GetAmount());
                break;
            }
            case 6917: // Death's Embrace damage effect
            case 6926:
            case 6928:
            {
                // Health at 25% or less (25% stored at effect 2 of the spell)
                int32 pct = 25;   // CalculateSpellDamage(this, (*i)->GetSpellInfo(), EFFECT_2); - doesn't work
                if (victim->HealthBelowPct(pct))
                    AddPct(DoneTotalMod, (*i)->GetAmount());
                break;
            }
            case 6916: // Death's Embrace heal effect
            case 6925:
            case 6927:
            {
                int32 pct = 25; // CalculateSpellDamage(this, (*i)->GetSpellInfo(), EFFECT_2); - doesn't work
                if (HealthBelowPct(pct))
                    AddPct(DoneTotalMod, (*i)->GetAmount());
                break;
            }
            // Soul Siphon
            case 4992:
            case 4993:
            {
                // effect 1 m_amount
                int32 maxPercent = (*i)->GetAmount();
                // effect 0 m_amount
                int32 stepPercent = CalculateSpellDamage(this, (*i)->GetSpellInfo(), 0);
                // count affliction effects and calc additional damage in percentage
                int32 modPercent = 0;
                AuraApplicationMap const& victimAuras = victim->GetAppliedAuras();
                for (AuraApplicationMap::const_iterator itr = victimAuras.begin(); itr != victimAuras.end(); ++itr)
                {
                    Aura const* aura = itr->second->GetBase();
                    SpellInfo const* spell = aura->GetSpellInfo();
                    if (spell->SpellFamilyName != SPELLFAMILY_WARLOCK || !(spell->SpellFamilyFlags[1] & 0x0004071B || spell->SpellFamilyFlags[0] & 0x8044C402))
                        continue;
                    modPercent += stepPercent * aura->GetStackAmount();
                    if (modPercent >= maxPercent)
                    {
                        modPercent = maxPercent;
                        break;
                    }
                }
                AddPct(DoneTotalMod, modPercent);
                break;
            }
            case 5481: // Starfire Bonus
            {
                if (victim->GetAuraEffect(SPELL_AURA_PERIODIC_DAMAGE, SPELLFAMILY_DRUID, 0x200002, 0, 0))
                    AddPct(DoneTotalMod, (*i)->GetAmount());
                break;
            }
            case 4418: // Increased Shock Damage
            case 4554: // Increased Lightning Damage
            case 4555: // Improved Moonfire
            case 5142: // Increased Lightning Damage
            case 5147: // Improved Consecration / Libram of Resurgence
            case 5148: // Idol of the Shooting Star
            case 6008: // Increased Lightning Damage
            case 8627: // Totem of Hex
            {
                DoneTotal += (*i)->GetAmount();
                break;
            }
        }
    }

    // Custom scripted damage
    switch (spellProto->SpellFamilyName)
    {
        case SPELLFAMILY_GENERIC:
        {
            switch (spellProto->Id)
            {
                case 98535: // Leaping Flames, Majordomo Staghelm
                case 98474: // Flame Scythe, Majordomo Staghelm
                case 100212:
                case 100213:
                case 100214:
                    // Fury, SPELL_AURA_276
                    if (Aura const* aur = GetAura(97235))
                        AddPct(DoneTotalMod, aur->GetStackAmount() * 8);
                    break;
            }
            break;
        }
        case SPELLFAMILY_ROGUE:
        {
            // Revealing Strike for direct damage abilities
            if (spellProto->AttributesEx & SPELL_ATTR1_REQ_COMBO_POINTS1 && damagetype != DOT)
                if (AuraEffect* aurEff = victim->GetAuraEffect(84617, 2, GetGUID()))
                {
                    DoneTotalMod *= (100.0f + aurEff->GetAmount()) / 100.0f;
                    victim->RemoveAura(aurEff->GetBase());
                }
            break;
        }
        case SPELLFAMILY_MAGE:
            // Ice Lance
            if (spellProto->SpellIconID == 186)
            {
                if (victim->HasAuraState(AURA_STATE_FROZEN, spellProto, this))
                {
                    DoneTotalMod *= 2.0f;
                    // Glyph of Ice Lance
                    if (owner->HasAura(56377) && victim->getLevel() > owner->getLevel())
                        DoneTotalMod *= 1.05f;
                }
                // Fingers of Frost
                if (HasAura(44544))
                    DoneTotalMod *= 1.25f;
                break;
            }
            // Torment the weak
            if (spellProto->GetSchoolMask() & SPELL_SCHOOL_MASK_ARCANE)
            {
                if (victim->HasAuraWithMechanic((1<<MECHANIC_SNARE)|(1<<MECHANIC_SLOW_ATTACK)))
                {
                    AuraEffectList const& mDumyAuras = GetAuraEffectsByType(SPELL_AURA_DUMMY);
                    for (AuraEffectList::const_iterator i = mDumyAuras.begin(); i != mDumyAuras.end(); ++i)
                    {
                        if ((*i)->GetSpellInfo()->SpellIconID == 2215)
                        {
                            AddPct(DoneTotalMod, (*i)->GetAmount());
                            break;
                        }
                    }
                }
            }
            // Shatter bonus damage for frostbolt
            if (spellProto->SpellFamilyFlags[0] & 0x20)
            {
                // Frost Specialization - additional 15% to frostbolt
                if (AuraEffect const* frostSpec = GetAuraEffect(SPELL_AURA_ADD_PCT_MODIFIER, SPELLFAMILY_MAGE, 2458, 2))
                    AddPct(DoneTotalMod, frostSpec->GetAmount());
                // Increase damage on frozen target
                if (victim->HasAuraState(AURA_STATE_FROZEN, spellProto, this))
                    if (AuraEffect const* Shatter = GetDummyAuraEffect(SPELLFAMILY_MAGE, 976, 1))
                        AddPct(DoneTotalMod, Shatter->GetAmount());
            }
            break;
        case SPELLFAMILY_PRIEST:
            switch (spellProto->Id)
            {
                // Smite
                case 585:
                {
                    // Glyph of Smite
                    if (AuraEffect* aurEff = GetAuraEffect(55692, 0))
                        if (victim->GetAuraEffect(SPELL_AURA_PERIODIC_DAMAGE, SPELLFAMILY_PRIEST, 0x100000, 0, 0, GetGUID()))
                            AddPct(DoneTotalMod, aurEff->GetAmount());
                    break;
                }
                // Triple damage for Shadow word: death
                case 32379:
                {
                    if (victim->GetHealthPct() < 25)
                    {
                        int32 mod = 300;
                        // Mind Melt
                        if (AuraEffect* aur = GetDummyAuraEffect(SPELLFAMILY_PRIEST, 3139, 0))
                            mod += aur->GetAmount();
                        AddPct(DoneTotalMod, mod);
                    }
                    break;
                }
                case 73510: // Mind Blast
                case 8092:  // Mind Spike
                {
                    // Damage with shadow orbs
                    if (Aura* sphere = GetAura(77487))
                    {
                        int32 mod = sphere->GetStackAmount() * 10;
                        int32 mastery = 0;
                        if (Aura* aur = GetAura(77486))
                            mastery = aur->GetEffect(0)->GetAmount();
                        mod += mastery * sphere->GetStackAmount();
                        AddPct(DoneTotalMod, mod);
                        int32 bp = 10 + mastery;
                        if (victim)
                            CastCustomSpell(victim, 95799, &bp, &bp, NULL, true);
                        RemoveAurasDueToSpell(77487);
                    }
                    break;
                }
            }
            break;
        case SPELLFAMILY_WARLOCK:
            switch (spellProto->Id)
            {
                case 29722: // Incinerate
                case 50796: // Chaos Bolt
                    if (victim->HasAuraState(AURA_STATE_CONFLAGRATE))
                    {
                        // Fire and Brimstone
                        if (AuraEffect const* aurEff = GetAuraEffect(SPELL_AURA_DUMMY, SPELLFAMILY_WARLOCK, 3173, EFFECT_0))
                            AddPct(DoneTotalMod, aurEff->GetAmount());

                        if (spellProto->Id == 29722)
                            AddPct(DoneTotalMod, 16.66f);
                    }
                    break;
                // Drain Soul - increased damage for targets under 25 % HP
                case 1120:
                    if (victim->HealthBelowPct(25))
                        DoneTotalMod *= 2;
                    break;
                default:
                    break;
            }
            break;
        case SPELLFAMILY_DEATHKNIGHT:
        {
            // Sigil of the Vengeful Heart
            if (spellProto->SpellFamilyFlags[0] & 0x2000)
            {
                if (AuraEffect* aurEff = GetAuraEffect(64962, EFFECT_1))
                    DoneTotal += aurEff->GetAmount();
            }
            // Icy Touch, Howling Blast
            else if ((spellProto->SpellFamilyFlags[0] & 0x00000002) || (spellProto->SpellFamilyFlags[1] & 0x2))
            {
                // Merciless Combat
                if (AuraEffect * eff = GetDummyAuraEffect(SPELLFAMILY_DEATHKNIGHT, 2656, 0))
                {
                    if (victim->HealthBelowPct(35))
                        AddPct(DoneTotalMod, eff->GetAmount());
                }
            }
            break;
        }
        case SPELLFAMILY_HUNTER:
            // Serpent Sting Damage
            if (spellProto->SpellFamilyFlags[0] & 0x4000 && damagetype == DOT)
            {
                AlreadyCalculated = true;
                WeaponAttackType attType = (spellProto->IsRangedWeaponSpell() && spellProto->DmgClass == SPELL_DAMAGE_CLASS_RANGED) ? RANGED_ATTACK : BASE_ATTACK;
                int32 APbonus = float(victim->GetMaxPositiveAuraModifier(attType == BASE_ATTACK ? SPELL_AURA_MELEE_ATTACK_POWER_ATTACKER_BONUS : SPELL_AURA_RANGED_ATTACK_POWER_ATTACKER_BONUS));
                switch (spellProto->Id)
                {
                    case 1978:
                        DoneTotal = int32(((GetTotalAttackPowerValue(RANGED_ATTACK) + APbonus) * 0.4f + (pdamage * 15 / 3)) / (spellProto->GetMaxDuration() / spellProto->Effects[0].Amplitude));
                        break;
                    case 88453:
                        DoneTotal = int32(((GetTotalAttackPowerValue(RANGED_ATTACK) + APbonus) * 0.16f + (pdamage * 6 / 3)) / (spellProto->GetMaxDuration() / spellProto->Effects[0].Amplitude));
                        break;
                    case 88466:
                        DoneTotal = int32(((GetTotalAttackPowerValue(RANGED_ATTACK) + APbonus) * 0.24f + (pdamage * 9 / 3)) / (spellProto->GetMaxDuration() / spellProto->Effects[0].Amplitude));
                        break;
                    default:
                        break;
                }
            }
        case SPELLFAMILY_PALADIN:
        {
            // Judgement of Truth
            if (spellProto->Id == 31804)
            {
                uint8 stacks = 0;
                if (Aura* aur = victim->GetAura(31803, GetGUID()))
                    stacks = aur->GetStackAmount();
                AddPct(DoneTotalMod, 20 * stacks);                
            }
            break;
        }
    }

    // Frostburn (Mage Frost Mastery)
    if (AuraEffect const* frostBurn = GetAuraEffect(76613, EFFECT_0))
    {
        if (victim->HasAuraState(AURA_STATE_FROZEN, spellProto, this))
            AddPct(DoneTotalMod, frostBurn->GetAmount());
    }

    // Mana Adept (Mage Arcane Mastery)
    if (AuraEffect const* manaAdept = GetAuraEffect(76547, EFFECT_0))
    {
        float manaPct = float(GetPower(POWER_MANA)) / float(GetMaxPower(POWER_MANA)) * 100.0f;
        float bonusDmg = manaPct * manaAdept->GetAmount() / 100.0f;
        AddPct(DoneTotalMod, bonusDmg);
    }

    // Done fixed damage bonus auras
    int32 DoneAdvertisedBenefit  = SpellBaseDamageBonusDone(spellProto->GetSchoolMask());    

    // Check for table values
    float coeff = 0;
    SpellBonusEntry const* bonus = sSpellMgr->GetSpellBonusData(spellProto->Id);
    if (bonus && !AlreadyCalculated)
    {
        if (damagetype == DOT)
        {
            coeff = bonus->dot_damage;
            if (bonus->ap_dot_bonus > 0)
            {
                WeaponAttackType attType = (spellProto->IsRangedWeaponSpell() && spellProto->DmgClass == SPELL_DAMAGE_CLASS_RANGED) ? RANGED_ATTACK : BASE_ATTACK;
                float APbonus = float(victim->GetMaxPositiveAuraModifier(attType == BASE_ATTACK ? SPELL_AURA_MELEE_ATTACK_POWER_ATTACKER_BONUS : SPELL_AURA_RANGED_ATTACK_POWER_ATTACKER_BONUS));
                APbonus += GetTotalAttackPowerValue(attType);
                DoneTotal += int32(bonus->ap_dot_bonus * stack * ApCoeffMod * APbonus);
            }
        }
        else
        {
            coeff = bonus->direct_damage;
            if (bonus->ap_bonus > 0)
            {
                WeaponAttackType attType = (spellProto->IsRangedWeaponSpell() && spellProto->DmgClass == SPELL_DAMAGE_CLASS_RANGED) ? RANGED_ATTACK : BASE_ATTACK;
                float APbonus = float(victim->GetMaxPositiveAuraModifier(attType == BASE_ATTACK ? SPELL_AURA_MELEE_ATTACK_POWER_ATTACKER_BONUS : SPELL_AURA_RANGED_ATTACK_POWER_ATTACKER_BONUS));
                APbonus += GetTotalAttackPowerValue(attType);
                DoneTotal += int32(bonus->ap_bonus * stack * ApCoeffMod * APbonus);
            }
        }
    }
    // Default calculation
    if (DoneAdvertisedBenefit)
    {
        if (!bonus || coeff < 0)
            coeff = CalculateDefaultCoefficient(spellProto, damagetype) * int32(stack);

        float factorMod = CalculateLevelPenalty(spellProto) * stack;

        if (Player* modOwner = GetSpellModOwner())
        {
            coeff *= 100.0f;
            modOwner->ApplySpellMod(spellProto->Id, SPELLMOD_BONUS_MULTIPLIER, coeff);
            coeff /= 100.0f;
        }
        DoneTotal += int32(DoneAdvertisedBenefit * coeff * factorMod);
    }

    if (AlreadyCalculated)
        pdamage = 0;

    float tmpDamage = (int32(pdamage) + DoneTotal) * DoneTotalMod;
    // apply spellmod to Done damage (flat and pct)
    if (Player* modOwner = GetSpellModOwner())
        modOwner->ApplySpellMod(spellProto->Id, damagetype == DOT ? SPELLMOD_DOT : SPELLMOD_DAMAGE, tmpDamage);

    return uint32(std::max(tmpDamage, 0.0f));
}

uint32 Unit::SpellDamageBonusTaken(Unit* caster, SpellInfo const* spellProto, uint32 pdamage, DamageEffectType damagetype, uint32 stack)
{
    if (!spellProto || damagetype == DIRECT_DAMAGE)
        return pdamage;

    // Some spells don't benefit from done mods
    if (spellProto->HasAttribute(SPELL_ATTR3_NO_DONE_BONUS))
        return pdamage;

    int32 TakenTotal = 0;
    float TakenTotalMod = 1.0f;
    float TakenTotalCasterMod = 0.0f;

    // get all auras from caster that allow the spell to ignore resistance (sanctified wrath)
    AuraEffectList const& IgnoreResistAuras = caster->GetAuraEffectsByType(SPELL_AURA_MOD_IGNORE_TARGET_RESIST);
    for (AuraEffectList::const_iterator i = IgnoreResistAuras.begin(); i != IgnoreResistAuras.end(); ++i)
    {
        if ((*i)->GetMiscValue() & spellProto->GetSchoolMask())
            TakenTotalCasterMod += (float((*i)->GetAmount()));
    }

    // from positive and negative SPELL_AURA_MOD_DAMAGE_PERCENT_TAKEN
    // multiplicative bonus, for example Dispersion + Shadowform (0.10*0.85=0.085)
    TakenTotalMod *= GetTotalAuraMultiplierByMiscMask(SPELL_AURA_MOD_DAMAGE_PERCENT_TAKEN, spellProto->GetSchoolMask());

    //.. taken pct: dummy auras
    /*AuraEffectList const& mDummyAuras = GetAuraEffectsByType(SPELL_AURA_DUMMY);
    for (AuraEffectList::const_iterator i = mDummyAuras.begin(); i != mDummyAuras.end(); ++i)
    {
        switch ((*i)->GetSpellInfo()->SpellIconID)
        {
        }
    }*/

    // From caster spells
    AuraEffectList const& mOwnerTaken = GetAuraEffectsByType(SPELL_AURA_MOD_DAMAGE_FROM_CASTER);
    for (AuraEffectList::const_iterator i = mOwnerTaken.begin(); i != mOwnerTaken.end(); ++i)
    {
        if ((*i)->GetCasterGUID() == caster->GetGUID())
        {
            switch ((*i)->GetId())
            {
                // Vendetta, should affect to all damage
                case 79140:
                    AddPct(TakenTotalMod, (*i)->GetAmount());
                    break;
                default:
                    if ((*i)->IsAffectingSpell(spellProto))
                        AddPct(TakenTotalMod, (*i)->GetAmount());
                    break;
            }
        }
    }

    // Mod damage from spell mechanic
    uint32 mechanicMask = spellProto->GetAllEffectsMechanicMask();
    
    // Shred - "Effects which increase Bleed damage also increase Shred damage"
    if (spellProto->Id == 6807)
        mechanicMask |= (1<<MECHANIC_BLEED);

    if (mechanicMask)
    {
        int32 maxval = 0;
        AuraEffectList const& mDamageDoneMechanic = GetAuraEffectsByType(SPELL_AURA_MOD_MECHANIC_DAMAGE_TAKEN_PERCENT);
        for (AuraEffectList::const_iterator i = mDamageDoneMechanic.begin(); i != mDamageDoneMechanic.end(); ++i)
            if (mechanicMask & uint32(1<<((*i)->GetMiscValue())))
                maxval = std::max(maxval, (*i)->GetAmount());
        
        if (maxval)
            AddPct(TakenTotalMod, maxval);
    }

    if (spellProto->IsTargetingArea())
    {
        int32 mult = GetTotalAuraModifierByMiscMask(SPELL_AURA_MOD_AOE_DAMAGE_AVOIDANCE, spellProto->SchoolMask);
        AddPct(TakenTotalMod, mult);
        if (caster->GetTypeId() == TYPEID_UNIT)
        {
            int32 u_mult = GetTotalAuraModifierByMiscMask(SPELL_AURA_MOD_CREATURE_AOE_DAMAGE_AVOIDANCE, spellProto->SchoolMask);
            AddPct(TakenTotalMod, u_mult);
        }
    }

    int32 TakenAdvertisedBenefit = SpellBaseDamageBonusTaken(spellProto->GetSchoolMask());

    // Check for table values
    float coeff = 0;
    SpellBonusEntry const* bonus = sSpellMgr->GetSpellBonusData(spellProto->Id);
    if (bonus)
        coeff = (damagetype == DOT) ? bonus->dot_damage : bonus->direct_damage;

    // Default calculation
    if (TakenAdvertisedBenefit)
    {
        if (!bonus || coeff < 0)
            coeff = CalculateDefaultCoefficient(spellProto, damagetype) * int32(stack);

        float factorMod = CalculateLevelPenalty(spellProto) * stack;
        // level penalty still applied on Taken bonus - is it blizzlike?
        if (Player* modOwner = GetSpellModOwner())
        {
            coeff *= 100.0f;
            modOwner->ApplySpellMod(spellProto->Id, SPELLMOD_BONUS_MULTIPLIER, coeff);
            coeff /= 100.0f;
        }
        TakenTotal+= int32(TakenAdvertisedBenefit * coeff * factorMod);
    }

    float tmpDamage = 0.0f;

    if (TakenTotalCasterMod)
    {
        if (TakenTotal < 0)
        {
            if (TakenTotalMod < 1)
                tmpDamage = ((float(CalculatePct(pdamage, TakenTotalCasterMod) + TakenTotal) * TakenTotalMod) + CalculatePct(pdamage, TakenTotalCasterMod));
            else
                tmpDamage = ((float(CalculatePct(pdamage, TakenTotalCasterMod) + TakenTotal) + CalculatePct(pdamage, TakenTotalCasterMod)) * TakenTotalMod);
        }
        else if (TakenTotalMod < 1)
            tmpDamage = ((CalculatePct(float(pdamage) + TakenTotal, TakenTotalCasterMod) * TakenTotalMod) + CalculatePct(float(pdamage) + TakenTotal, TakenTotalCasterMod));
    }
    if (!tmpDamage)
        tmpDamage = (float(pdamage) + TakenTotal) * TakenTotalMod;

    return uint32(std::max(tmpDamage, 0.0f));
}

int32 Unit::SpellBaseDamageBonusDone(SpellSchoolMask schoolMask)
{
    int32 DoneAdvertisedBenefit = 0;

    AuraEffectList const& mDamageDone = GetAuraEffectsByType(SPELL_AURA_MOD_DAMAGE_DONE);
    for (AuraEffectList::const_iterator i = mDamageDone.begin(); i != mDamageDone.end(); ++i)
        if (((*i)->GetMiscValue() & schoolMask) != 0 &&
        (*i)->GetSpellInfo()->EquippedItemClass == -1 &&
                                                            // -1 == any item class (not wand then)
        ((*i)->GetSpellInfo()->EquippedItemInventoryTypeMask == 0
        || (*i)->GetSpellInfo()->EquippedItemInventoryTypeMask == -1))
                                                            // 0 == any inventory type (not wand then)
            DoneAdvertisedBenefit += (*i)->GetAmount();

    if (GetTypeId() == TYPEID_PLAYER)
    {
        // Damage from attack power
        // Your spell power is now equal to $s1% of your attack power, and you no longer benefit from other sources of spell power
        int32 spPctFromAttackPower = 0;
        AuraEffectList const& mSetSpellPowerPctFromAP = GetAuraEffectsByType(SPELL_AURA_OVERRIDE_SPELL_POWER_BY_AP_PCT);
        for (AuraEffectList::const_iterator i = mSetSpellPowerPctFromAP.begin(); i != mSetSpellPowerPctFromAP.end(); ++i)
        {
            if ((*i)->GetMiscValue() & schoolMask)
                spPctFromAttackPower += CalculatePct<int>(int32(GetTotalAttackPowerValue(BASE_ATTACK)), (*i)->GetAmount());
        }

        if (spPctFromAttackPower > 0)
            return spPctFromAttackPower;

        // Base value
        DoneAdvertisedBenefit += ToPlayer()->GetBaseSpellPowerBonus();

        // Check if we are ever using mana - PaperDollFrame.lua
        if (GetPowerIndexByClass(POWER_MANA, getClass()) != MAX_POWERS)
            DoneAdvertisedBenefit += std::max(0, int32(GetStat(STAT_INTELLECT)) - 10);  // spellpower from intellect

        // Damage bonus from stats
        AuraEffectList const& mDamageDoneOfStatPercent = GetAuraEffectsByType(SPELL_AURA_MOD_SPELL_DAMAGE_OF_STAT_PERCENT);
        for (AuraEffectList::const_iterator i = mDamageDoneOfStatPercent.begin(); i != mDamageDoneOfStatPercent.end(); ++i)
        {
            if ((*i)->GetMiscValue() & schoolMask)
            {
                // stat used stored in miscValueB for this aura
                Stats usedStat = Stats((*i)->GetMiscValueB());
                DoneAdvertisedBenefit += int32(CalculatePct(GetStat(usedStat), (*i)->GetAmount()));
            }
        }
        // ... and attack power
        AuraEffectList const& mDamageDonebyAP = GetAuraEffectsByType(SPELL_AURA_MOD_SPELL_DAMAGE_OF_ATTACK_POWER);
        for (AuraEffectList::const_iterator i =mDamageDonebyAP.begin(); i != mDamageDonebyAP.end(); ++i)
            if ((*i)->GetMiscValue() & schoolMask)
                DoneAdvertisedBenefit += int32(CalculatePct(GetTotalAttackPowerValue(BASE_ATTACK), (*i)->GetAmount()));

        int32 spModPct = GetMaxPositiveAuraModifier(SPELL_AURA_MOD_SPELL_POWER_PCT);

        // Temporal Ruin Warlock 4P T13 bonus
        if (AuraEffect * eff = GetAuraEffect(105786, 0))
        {
            spModPct += eff->GetAmount();
        }

        DoneAdvertisedBenefit += DoneAdvertisedBenefit * spModPct / 100;
    }

    if (DoneAdvertisedBenefit < 0)
        DoneAdvertisedBenefit = 0;

    return DoneAdvertisedBenefit;
}

int32 Unit::SpellBaseDamageBonusTaken(SpellSchoolMask schoolMask)
{
    int32 TakenAdvertisedBenefit = 0;

    AuraEffectList const& mDamageTaken = GetAuraEffectsByType(SPELL_AURA_MOD_DAMAGE_TAKEN);
    for (AuraEffectList::const_iterator i = mDamageTaken.begin(); i != mDamageTaken.end(); ++i)
        if (((*i)->GetMiscValue() & schoolMask) != 0)
            TakenAdvertisedBenefit += (*i)->GetAmount();

    return TakenAdvertisedBenefit;
}

bool Unit::isSpellCrit(Unit* victim, SpellInfo const* spellProto, SpellSchoolMask schoolMask, WeaponAttackType attackType) const
{
    //! Mobs can't crit with spells. Player Totems can
    //! Fire Elemental (from totem) can too - but this part is a hack and needs more research
    if (IS_CREATURE_GUID(GetGUID()) || IS_VEHICLE_GUID(GetGUID())) 
        if (!((isTotem() || isGuardian()) && IS_PLAYER_GUID(GetOwnerGUID())))
            if (GetEntry() != 15438)
                return false;

    // not critting spell
    if ((spellProto->AttributesEx2 & SPELL_ATTR2_CANT_CRIT))
        return false;

    switch (spellProto->Id)
    {
        case 53353: // Chimera Shot - Healing can crit, other spells - not
        case 34428: // Victory Rush
            break;
        default:
            if (spellProto->HasEffect(SPELL_EFFECT_HEAL_PCT))
                return false;
            break;
    }

    float crit_chance = GetSpellCrit(victim, spellProto, schoolMask, attackType);

    if (crit_chance == 0.0f)
        return false;
    else if (crit_chance >= 100.0f)
        return true;

    if (roll_chance_f(crit_chance))
        return true;

    return false;
}

float Unit::GetSpellCrit(Unit* victim, SpellInfo const* spellProto, SpellSchoolMask schoolMask, WeaponAttackType attackType) const  
{
    float crit_chance = 0.0f;
    if (victim && !spellProto->IsPositive())
    {
        // Modify critical chance by victim SPELL_AURA_MOD_ATTACKER_SPELL_AND_WEAPON_CRIT_CHANCE
        crit_chance += victim->GetTotalAuraModifier(SPELL_AURA_MOD_ATTACKER_SPELL_AND_WEAPON_CRIT_CHANCE);
        if (crit_chance < -100.0f)
            return 0.0f;
    }

    switch (spellProto->DmgClass)
    {
        case SPELL_DAMAGE_CLASS_NONE:
            if (!spellProto->CanCritDamageClassNone())
                return 0.0;
        case SPELL_DAMAGE_CLASS_MAGIC:
        {
            // exeption is healthstone and heal potions
            if (spellProto->SpellIconID == 284 || spellProto->SpellFamilyName == SPELLFAMILY_POTION)
            {
                schoolMask = SPELL_SCHOOL_MASK_HOLY;
            }
            if (schoolMask & SPELL_SCHOOL_MASK_NORMAL)
                crit_chance = 0.0f;
            // For other schools
            else if (GetTypeId() == TYPEID_PLAYER)
                crit_chance = GetFloatValue(PLAYER_SPELL_CRIT_PERCENTAGE1 + GetFirstSchoolInMask(schoolMask));
            else
            {
                crit_chance = (float)m_baseSpellCritChance;
                crit_chance += GetTotalAuraModifierByMiscMask(SPELL_AURA_MOD_SPELL_CRIT_CHANCE_SCHOOL, schoolMask);
            }
            // taken
            if (victim)
            {
                if (!spellProto->IsPositive())
                {
                    // Modify critical chance by victim SPELL_AURA_MOD_ATTACKER_SPELL_CRIT_CHANCE
                    crit_chance += victim->GetTotalAuraModifierByMiscMask(SPELL_AURA_MOD_ATTACKER_SPELL_CRIT_CHANCE, schoolMask);
                }
                // Custom crit by class
                switch (spellProto->SpellFamilyName)
                {
                    case SPELLFAMILY_PRIEST:
                    {
                        // Renewed Hope
                        if (AuraEffect * eff = GetDummyAuraEffect(SPELLFAMILY_PRIEST, 329, 0))
                        {
                            if (eff->IsAffectingSpell(spellProto) && (victim->HasAura(6788) || victim->HasAura(77613, GetGUID())))
                                crit_chance += eff->GetAmount();
                        }
                        break;
                    }
                    case SPELLFAMILY_MAGE:
                        // Glyph of Fire Blast
                        if (spellProto->SpellFamilyFlags[0] == 0x2 && spellProto->SpellIconID == 12)
                            if (victim->HasAuraWithMechanic((1<<MECHANIC_STUN) | (1<<MECHANIC_KNOCKOUT)))
                                if (AuraEffect const* aurEff = GetAuraEffect(56369, EFFECT_0))
                                    crit_chance += aurEff->GetAmount();
                        break;
                    case SPELLFAMILY_DRUID:

                        // Starfire
                        if (spellProto->SpellFamilyFlags[0] & 0x4 && spellProto->SpellIconID == 1485)
                        {
                            // Improved Insect Swarm
                            if (AuraEffect const* aurEff = GetDummyAuraEffect(SPELLFAMILY_DRUID, 1771, 0))
                                if (victim->GetAuraEffect(SPELL_AURA_PERIODIC_DAMAGE, SPELLFAMILY_DRUID, 0x00000002, 0, 0))
                                    crit_chance += aurEff->GetAmount();
                           break;
                        }
                        break;
                    case SPELLFAMILY_ROGUE:
                        // Shiv-applied poisons can't crit
                        if (FindCurrentSpellBySpellId(5938))
                            crit_chance = 0.0f;
                        break;
                    case SPELLFAMILY_PALADIN:
                        // Last Word - Word of Glory
                        if (spellProto->Id == 85673)
                        {
                            if (AuraEffect *aur = GetDummyAuraEffect(SPELLFAMILY_PALADIN, 2139, 0))
                                if (victim->HealthBelowPct(35))
                                    crit_chance+=aur->GetAmount();
                            break;
                        }
                        // Exorcism
                        else if (spellProto->Id == 879)
                        {
                            if (victim->GetCreatureTypeMask() & CREATURE_TYPEMASK_DEMON_OR_UNDEAD)
                                return 100.0f;
                            break;
                        }
                        break;
                    case SPELLFAMILY_SHAMAN:
                        // Lava Burst
                        if (spellProto->Id == 51505 || spellProto->Id == 77451)
                        {
                            if (victim->GetAuraEffect(SPELL_AURA_PERIODIC_DAMAGE, SPELLFAMILY_SHAMAN, 0x10000000, 0, 0, GetGUID()))
                                if (victim->GetTotalAuraModifier(SPELL_AURA_MOD_ATTACKER_SPELL_AND_WEAPON_CRIT_CHANCE) > -100)
                                    return 100.0f;
                            break;
                        }
                        break;
                    case SPELLFAMILY_WARLOCK:
                    {
                        // Searing Pain - Soulburn
                        if (spellProto->Id == 5676 && HasAura(74434))
                            return 100.0f;
                        break;
                    }
                }
            }
            break;
        }
        case SPELL_DAMAGE_CLASS_MELEE:
            if (victim)
            {
                // Custom crit by class
                switch (spellProto->SpellFamilyName)
                {
                    case SPELLFAMILY_DRUID:
                        // Rend and Tear - bonus crit chance for Ferocious Bite on bleeding targets
                        if (spellProto->SpellFamilyFlags[0] & 0x00800000
                            && spellProto->SpellIconID == 1680
                            && victim->HasAuraState(AURA_STATE_BLEEDING))
                        {
                            if (AuraEffect const* rendAndTear = GetDummyAuraEffect(SPELLFAMILY_DRUID, 2859, 1))
                                crit_chance += rendAndTear->GetAmount();
                            break;
                        }
                        // Predatory Strikes - bonus critical strike chance for Ravage
                        if (spellProto->SpellFamilyFlags[0] & 0x00010000
                            && spellProto->SpellIconID == 1531)
                        {
                            if (AuraEffect * predatory = GetDummyAuraEffect(SPELLFAMILY_DRUID, 1563, 0))
                            {
                                 if (victim->HealthAbovePct(80))
                                    crit_chance += predatory->GetAmount();
                            }
                            break;
                        }
                        break;
                    case SPELLFAMILY_HUNTER:
                        // Improved Kill Command
                        if (spellProto->SpellFamilyFlags[1] & 0x00000800)
                        {
                            if (GetOwner())
                                if (AuraEffect const * aurEff = GetOwner()->GetDummyAuraEffect(SPELLFAMILY_HUNTER, 2221, 0))
                                    crit_chance+=aurEff->GetAmount();
                            // Spider's Bite and other
                            crit_chance += GetTotalAuraModifier(SPELL_AURA_MOD_WEAPON_CRIT_PERCENT);
                            break;
                        }
                        break;
                }
            }
        case SPELL_DAMAGE_CLASS_RANGED:
        {
            if (victim)
            {
                // Careful Aim: Steady Shot, Cobra Shot and Aimed Shot
                if (spellProto->SpellFamilyName == SPELLFAMILY_HUNTER && (spellProto->SpellFamilyFlags[1] & 0x00000001 ||
                    spellProto->SpellFamilyFlags[2] & 0x00400000 || spellProto->SpellFamilyFlags[0] & 0x00020000))
                    if (AuraEffect * aur = GetDummyAuraEffect(SPELLFAMILY_HUNTER, 2222, 0))
                        if (victim->GetHealthPct() > 90)
                            crit_chance += aur->GetAmount();

                crit_chance += GetUnitCriticalChance(attackType, victim);
                crit_chance += GetTotalAuraModifierByMiscMask(SPELL_AURA_MOD_SPELL_CRIT_CHANCE_SCHOOL, schoolMask);
            }
            break;
        }
        default:
            return 0.0f;
    }
    
    if (victim)
    {
        AuraEffectList const& critAuras = victim->GetAuraEffectsByType(SPELL_AURA_MOD_CRIT_CHANCE_FOR_CASTER);
        for (AuraEffectList::const_iterator i = critAuras.begin(); i != critAuras.end(); ++i)
            if ((*i)->GetCasterGUID() == GetGUID() && (*i)->IsAffectingSpell(spellProto))
                crit_chance += (*i)->GetAmount();

        // Mind Blast & Mind Spike debuff
        if (spellProto->Id == 8092)
            victim->RemoveAurasDueToSpell(87178, GetGUID());
    }

    // percent done
    // only players use intelligence for critical chance computations
    if (Player* modOwner = GetSpellModOwner())
        modOwner->ApplySpellMod(spellProto->Id, SPELLMOD_CRITICAL_CHANCE, crit_chance);

    // Curse of Gul'dan
    if (isPet() || isGuardian())
        if (Unit* owner = GetOwner())
            if (Aura* aur = victim->GetAura(86000, owner->GetGUID()))
                crit_chance += float(aur->GetEffect(0)->GetAmount());

    // Shatter (same misc values now)
    // Shatter should calculated last
    if (spellProto->DmgClass == SPELL_DAMAGE_CLASS_MAGIC && spellProto->SpellFamilyName == SPELLFAMILY_MAGE)
    {
        if (victim->HasAuraState(AURA_STATE_FROZEN, spellProto, this))
        {
            if (AuraEffect const* Shatter = GetDummyAuraEffect(SPELLFAMILY_MAGE, 976, 1))
                crit_chance *= (float(Shatter->GetAmount()) / 10.0f) + 1.0f;
        }
    }

    crit_chance = crit_chance > 0.0f ? crit_chance : 0.0f;
    return crit_chance;
}

uint32 Unit::SpellCriticalDamageBonus(SpellInfo const* spellProto, uint32 damage, Unit* /*victim*/)
{
    // Calculate critical bonus
    int32 crit_bonus = damage;
    float crit_mod = 0.0f;

    switch (spellProto->DmgClass)
    {
        case SPELL_DAMAGE_CLASS_MELEE:                      // for melee based spells is 100%
        case SPELL_DAMAGE_CLASS_RANGED:
        {
            // Serpent Sting & Black Arrow
            if (spellProto->Id == 1978 || spellProto->Id == 3674)
            {
                crit_bonus += damage / 2;
                break;
            }
            // TODO: write here full calculation for melee/ranged spells
            crit_bonus += damage;
            break;
        }
        case SPELL_DAMAGE_CLASS_MAGIC:
            switch (spellProto->Id)
            {
                case 85692: // Doom Bolt
                case 54049: // Shadow Bite
                case 3110:  // Firebolt
                case 7814:  // Lash of Pain
                case 51963: // Gargoyle Strike
                    crit_bonus += damage;
                    break;
                default:
                    crit_bonus += damage / 2;
                    break;
            }
            break;
        default:
            crit_bonus += damage / 2;                       // for spells is 50%
            break;
    }

    crit_mod += (GetTotalAuraMultiplierByMiscMask(SPELL_AURA_MOD_CRIT_DAMAGE_BONUS, spellProto->GetSchoolMask()) - 1.0f) * 100;

    if (crit_bonus != 0)
        AddPct(crit_bonus, crit_mod);

    crit_bonus -= damage;

    // adds additional damage to critBonus (from talents)
    if (Player* modOwner = GetSpellModOwner())
        modOwner->ApplySpellMod(spellProto->Id, SPELLMOD_CRIT_DAMAGE_BONUS, crit_bonus);

    crit_bonus += damage;

    return crit_bonus;
}

uint32 Unit::SpellCriticalHealingBonus(SpellInfo const* /*spellProto*/, uint32 damage, Unit* /*victim*/)
{
    // Calculate critical bonus
    int32 crit_bonus = damage;

    damage += crit_bonus;

    damage = int32(float(damage) * GetTotalAuraMultiplier(SPELL_AURA_MOD_CRITICAL_HEALING_AMOUNT));

    return damage;
}

uint32 Unit::SpellHealingBonusDone(Unit* victim, SpellInfo const* spellProto, uint32 healamount, DamageEffectType damagetype, uint32 stack)
{
    // For totems get healing bonus from owner (statue isn't totem in fact)
    if (GetTypeId() == TYPEID_UNIT && isTotem())
        if (Unit* owner = GetOwner())
            return owner->SpellHealingBonusDone(victim, spellProto, healamount, damagetype, stack);

    // No bonus healing for potion spells
    if (spellProto->SpellFamilyName == SPELLFAMILY_POTION)
        return healamount;

    switch (spellProto->Id)
    {
        case 64801: // Gift of the Earthmother
        case 81751: // Atonement
        case 91394: // Permafrost
        case 63544: // Divine Touch
            return healamount;
        case 6262: // Warlock Healthstone
            healamount = uint32(0.45f * GetCreateHealth());
            break;
        default:
            break;
    }

    float DoneTotalMod = 1.0f;
    int32 DoneTotal = 0;

    // Healing done percent
    AuraEffectList const& mHealingDonePct = GetAuraEffectsByType(SPELL_AURA_MOD_HEALING_DONE_PERCENT);
    for (AuraEffectList::const_iterator i = mHealingDonePct.begin(); i != mHealingDonePct.end(); ++i)
        AddPct(DoneTotalMod, (*i)->GetAmount());

    // Healing done percent on debuff
    AuraEffectList const& mHealingDonePctOnDebuff = GetAuraEffectsByType(SPELL_AURA_MOD_HEALING_DONE_ON_DEBUFF);
    for (AuraEffectList::const_iterator i = mHealingDonePctOnDebuff.begin(); i != mHealingDonePctOnDebuff.end(); ++i)
        AddPct(DoneTotalMod, (*i)->GetAmount());

    // done scripted mod (take it from owner)
    Unit* owner = GetOwner() ? GetOwner() : this;
    AuraEffectList const& mOverrideClassScript= owner->GetAuraEffectsByType(SPELL_AURA_OVERRIDE_CLASS_SCRIPTS);
    for (AuraEffectList::const_iterator i = mOverrideClassScript.begin(); i != mOverrideClassScript.end(); ++i)
    {
        if (!(*i)->IsAffectingSpell(spellProto))
            continue;
        switch ((*i)->GetMiscValue())
        {
            case 4415: // Increased Rejuvenation Healing
            case 4953:
            case 3736: // Hateful Totem of the Third Wind / Increased Lesser Healing Wave / LK Arena (4/5/6) Totem of the Third Wind / Savage Totem of the Third Wind
                DoneTotal += (*i)->GetAmount();
                break;
            case   21: // Test of Faith
            case 6935:
            case 6918:
                if (victim && victim->HealthBelowPct(50))
                    AddPct(DoneTotalMod, (*i)->GetAmount());
                break;
            default:
                break;
        }
    }

    // Done fixed damage bonus auras
    int32 DoneAdvertisedBenefit = SpellBaseHealingBonusDone(spellProto->GetSchoolMask());

    // Check for table values
    SpellBonusEntry const* bonus = sSpellMgr->GetSpellBonusData(spellProto->Id);
    float coeff = 0;
    float factorMod = 1.0f;
    if (bonus)
    {
        if (damagetype == DOT)
        {
            coeff = bonus->dot_damage;
            if (bonus->ap_dot_bonus > 0)
                DoneTotal += int32(bonus->ap_dot_bonus * stack * GetTotalAttackPowerValue(
                    (spellProto->IsRangedWeaponSpell() && spellProto->DmgClass !=SPELL_DAMAGE_CLASS_MELEE) ? RANGED_ATTACK : BASE_ATTACK));
        }
        else
        {
            coeff = bonus->direct_damage;
            if (bonus->ap_bonus > 0)
                DoneTotal += int32(bonus->ap_bonus * stack * GetTotalAttackPowerValue(
                    (spellProto->IsRangedWeaponSpell() && spellProto->DmgClass !=SPELL_DAMAGE_CLASS_MELEE) ? RANGED_ATTACK : BASE_ATTACK));
        }
    }
    else
    {
        // No bonus healing for SPELL_DAMAGE_CLASS_NONE class spells by default
        if (spellProto->DmgClass == SPELL_DAMAGE_CLASS_NONE)
            return healamount;
    }

    // Default calculation
    if (DoneAdvertisedBenefit)
    {
        if (!bonus || coeff < 0)
            coeff = CalculateDefaultCoefficient(spellProto, damagetype) * int32(stack) * 1.88f;  // As wowwiki says: C = (Cast Time / 3.5) * 1.88 (for healing spells)

        factorMod *= CalculateLevelPenalty(spellProto) * stack;

        if (Player* modOwner = GetSpellModOwner())
        {
            coeff *= 100.0f;
            modOwner->ApplySpellMod(spellProto->Id, SPELLMOD_BONUS_MULTIPLIER, coeff);
            coeff /= 100.0f;
        }

        DoneTotal += int32(DoneAdvertisedBenefit * coeff * factorMod);
    }

    for (uint8 i = 0; i < MAX_SPELL_EFFECTS; ++i)
    {
        switch (spellProto->Effects[i].ApplyAuraName)
        {
            // Bonus healing does not apply to these spells
            case SPELL_AURA_PERIODIC_LEECH:
            case SPELL_AURA_PERIODIC_HEALTH_FUNNEL:
                DoneTotal = 0;
                break;
        }
        if (spellProto->Effects[i].Effect == SPELL_EFFECT_HEALTH_LEECH)
            DoneTotal = 0;
    }

    // Earth Shield
    if (spellProto->Id == 379)
    {
        // Glyph of Earth Shield
        if (AuraEffect const* aur = GetAuraEffect(63279, EFFECT_0))
            AddPct(DoneTotalMod, aur->GetAmount());

        // Purification
        if (AuraEffect const* aurEff = GetAuraEffect(16213, EFFECT_0))
            AddPct(DoneTotalMod, aurEff->GetAmount());
    }

    // use float as more appropriate for negative values and percent applying
    float heal = float(int32(healamount) + DoneTotal) * DoneTotalMod;
    // apply spellmod to Done amount
    if (Player* modOwner = GetSpellModOwner())
        modOwner->ApplySpellMod(spellProto->Id, damagetype == DOT ? SPELLMOD_DOT : SPELLMOD_DAMAGE, heal);

    return uint32(std::max(heal, 0.0f));
}

uint32 Unit::SpellHealingBonusTaken(Unit* caster, SpellInfo const* spellProto, uint32 healamount, DamageEffectType damagetype, uint32 stack)
{
    switch (spellProto->Id)
    {
        case 64801: // Gift of the Earthmother
        case 81751: // Atonement
        case 91394: // Permafrost
        case 63544: // Divine Touch
            return healamount;
            break;
        default:
            break;
    }

    float TakenTotalMod = 1.0f;

    // Healing taken percent
    float minval = float(GetMaxNegativeAuraModifier(SPELL_AURA_MOD_HEALING_PCT));
    if (minval)
        AddPct(TakenTotalMod, minval);

    float maxval = float(GetMaxPositiveAuraModifier(SPELL_AURA_MOD_HEALING_PCT));
    if (maxval)
        AddPct(TakenTotalMod, maxval);

    if (spellProto->SpellFamilyName == SPELLFAMILY_SHAMAN)
    {
        // Nature's Blessing
        if (AuraEffect * aur = caster->GetDummyAuraEffect(SPELLFAMILY_SHAMAN, 2012, 0))
            if (aur->IsAffectingSpell(spellProto) && HasAura(974)) // earth shield
                AddPct(TakenTotalMod, aur->GetAmount());
    }

    // Tenacity increase healing % taken
    if (AuraEffect const* Tenacity = GetAuraEffect(58549, 0))
        AddPct(TakenTotalMod, Tenacity->GetAmount());

    // Healing Done
    int32 TakenTotal = 0;

    // Taken fixed damage bonus auras
    int32 TakenAdvertisedBenefit = SpellBaseHealingBonusTaken(spellProto->GetSchoolMask());

    // Nourish cast
    if (spellProto->SpellFamilyName == SPELLFAMILY_DRUID && spellProto->SpellFamilyFlags[1] & 0x2000000)
    {
        // Rejuvenation, Regrowth, Lifebloom, or Wild Growth
        if (GetAuraEffect(SPELL_AURA_PERIODIC_HEAL, SPELLFAMILY_DRUID, 0x50, 0x4000010, 0))
            // increase healing by 20%
            TakenTotalMod *= 1.2f;
    }

    // Check for table values
    SpellBonusEntry const* bonus = sSpellMgr->GetSpellBonusData(spellProto->Id);
    float coeff = 0;
    float factorMod = 1.0f;
    if (bonus)
        coeff = (damagetype == DOT) ? bonus->dot_damage : bonus->direct_damage;
    else
    {
        // No bonus healing for SPELL_DAMAGE_CLASS_NONE class spells by default
        if (spellProto->DmgClass == SPELL_DAMAGE_CLASS_NONE)
        {
            healamount = uint32(std::max((float(healamount) * TakenTotalMod), 0.0f));
            return healamount;
        }
    }

    // Default calculation
    if (TakenAdvertisedBenefit)
    {
        if (!bonus || coeff < 0)
            coeff = CalculateDefaultCoefficient(spellProto, damagetype) * int32(stack) * 1.88f;  // As wowwiki says: C = (Cast Time / 3.5) * 1.88 (for healing spells)

        factorMod *= CalculateLevelPenalty(spellProto) * int32(stack);
        if (Player* modOwner = GetSpellModOwner())
        {
            coeff *= 100.0f;
            modOwner->ApplySpellMod(spellProto->Id, SPELLMOD_BONUS_MULTIPLIER, coeff);
            coeff /= 100.0f;
        }

        TakenTotal += int32(TakenAdvertisedBenefit * coeff * factorMod);
    }

    AuraEffectList const& mHealingGet= GetAuraEffectsByType(SPELL_AURA_MOD_HEALING_RECEIVED);
    for (AuraEffectList::const_iterator i = mHealingGet.begin(); i != mHealingGet.end(); ++i)
        if (caster->GetGUID() == (*i)->GetCasterGUID() && (*i)->IsAffectingSpell(spellProto))
            AddPct(TakenTotalMod, (*i)->GetAmount());

    // Deep Healing, Mastery Shaman
    if (caster)
        if (spellProto->SpellFamilyName == SPELLFAMILY_SHAMAN
            || spellProto->Id == 52042) // Healing Stream Totem tick
        {
            if (Aura* deepHealing = caster->GetAura(77226))
            {
                float effectAmount = float(deepHealing->GetEffect(0)->GetAmount());
                float mod = 100.0f - GetHealthPct();
                if (mod < 1.0f)
                    mod = 1.0f;
                float amount_pct = mod * effectAmount / 100.0f;
                AddPct(TakenTotalMod, amount_pct);
            }
        }

    for (uint8 i = 0; i < MAX_SPELL_EFFECTS; ++i)
    {
        switch (spellProto->Effects[i].ApplyAuraName)
        {
            // Bonus healing does not apply to these spells
            case SPELL_AURA_PERIODIC_LEECH:
            case SPELL_AURA_PERIODIC_HEALTH_FUNNEL:
                TakenTotal = 0;
                break;
        }
        if (spellProto->Effects[i].Effect == SPELL_EFFECT_HEALTH_LEECH)
            TakenTotal = 0;
    }

    float heal = float(int32(healamount) + TakenTotal) * TakenTotalMod;

    return uint32(std::max(heal, 0.0f));
}

int32 Unit::SpellBaseHealingBonusDone(SpellSchoolMask schoolMask)
{
    int32 AdvertisedBenefit = 0;

    AuraEffectList const& mHealingDone = GetAuraEffectsByType(SPELL_AURA_MOD_HEALING_DONE);
    for (AuraEffectList::const_iterator i = mHealingDone.begin(); i != mHealingDone.end(); ++i)
        if (!(*i)->GetMiscValue() || ((*i)->GetMiscValue() & schoolMask) != 0)
            AdvertisedBenefit += (*i)->GetAmount();

    // Healing bonus of spirit, intellect and strength
    if (GetTypeId() == TYPEID_PLAYER)
    {
        // Healing from attack power
        // Your spell power is now equal to $s1% of your attack power, and you no longer benefit from other sources of spell power
        int32 spPctFromAttackPower = 0;
        AuraEffectList const& mSetSpellPowerPctFromAP = GetAuraEffectsByType(SPELL_AURA_OVERRIDE_SPELL_POWER_BY_AP_PCT);
        for (AuraEffectList::const_iterator i = mSetSpellPowerPctFromAP.begin(); i != mSetSpellPowerPctFromAP.end(); ++i)
        {
            if ((*i)->GetMiscValue() & schoolMask)
                spPctFromAttackPower += CalculatePct<int>(int32(GetTotalAttackPowerValue(BASE_ATTACK)), (*i)->GetAmount());
        }

        if (spPctFromAttackPower > 0)
            return spPctFromAttackPower;

        // Base value
        AdvertisedBenefit += ToPlayer()->GetBaseSpellPowerBonus();

        // Check if we are ever using mana - PaperDollFrame.lua
        if (GetPowerIndexByClass(POWER_MANA, getClass()) != MAX_POWERS)
            AdvertisedBenefit += std::max(0, int32(GetStat(STAT_INTELLECT)) - 10);  // spellpower from intellect

        // Healing bonus from stats
        AuraEffectList const& mHealingDoneOfStatPercent = GetAuraEffectsByType(SPELL_AURA_MOD_SPELL_HEALING_OF_STAT_PERCENT);
        for (AuraEffectList::const_iterator i = mHealingDoneOfStatPercent.begin(); i != mHealingDoneOfStatPercent.end(); ++i)
        {
            // stat used dependent from misc value (stat index)
            Stats usedStat = Stats((*i)->GetSpellInfo()->Effects[(*i)->GetEffIndex()].MiscValue);
            AdvertisedBenefit += int32(CalculatePct(GetStat(usedStat), (*i)->GetAmount()));
        }

        // ... and attack power
        AuraEffectList const& mHealingDonebyAP = GetAuraEffectsByType(SPELL_AURA_MOD_SPELL_HEALING_OF_ATTACK_POWER);
        for (AuraEffectList::const_iterator i = mHealingDonebyAP.begin(); i != mHealingDonebyAP.end(); ++i)
            if ((*i)->GetMiscValue() & schoolMask)
                AdvertisedBenefit += int32(CalculatePct(GetTotalAttackPowerValue(BASE_ATTACK), (*i)->GetAmount()));

        int32 spModPct = GetMaxPositiveAuraModifier(SPELL_AURA_MOD_SPELL_POWER_PCT);

        // Temporal Ruin Warlock 4P T13 bonus
        if (AuraEffect * eff = GetAuraEffect(105786, 0))
        {
            spModPct += eff->GetAmount();
        }

        AdvertisedBenefit += AdvertisedBenefit * spModPct / 100;
    }
    return AdvertisedBenefit;
}

int32 Unit::SpellBaseHealingBonusTaken(SpellSchoolMask schoolMask)
{
    int32 AdvertisedBenefit = 0;

    AuraEffectList const& mDamageTaken = GetAuraEffectsByType(SPELL_AURA_MOD_HEALING);
    for (AuraEffectList::const_iterator i = mDamageTaken.begin(); i != mDamageTaken.end(); ++i)
        if (((*i)->GetMiscValue() & schoolMask) != 0)
            AdvertisedBenefit += (*i)->GetAmount();

    return AdvertisedBenefit;
}

bool Unit::IsImmunedToDamage(SpellSchoolMask shoolMask)
{
    // If m_immuneToSchool type contain this school type, IMMUNE damage.
    SpellImmuneList const& schoolList = m_spellImmune[IMMUNITY_SCHOOL];
    for (SpellImmuneList::const_iterator itr = schoolList.begin(); itr != schoolList.end(); ++itr)
        if (itr->type & shoolMask)
            return true;

    // If m_immuneToDamage type contain magic, IMMUNE damage.
    SpellImmuneList const& damageList = m_spellImmune[IMMUNITY_DAMAGE];
    for (SpellImmuneList::const_iterator itr = damageList.begin(); itr != damageList.end(); ++itr)
        if (itr->type & shoolMask)
            return true;

    return false;
}

bool Unit::IsImmunedToDamage(SpellInfo const* spellInfo)
{
    if (spellInfo->Attributes & SPELL_ATTR0_UNAFFECTED_BY_INVULNERABILITY)
        return false;

    // Queen Azshara, Well of Eternity
    if (GetEntry() == 54853)
        if (spellInfo->HasEffect(SPELL_EFFECT_INTERRUPT_CAST))
            return false;

    uint32 shoolMask = spellInfo->GetSchoolMask();
    if (spellInfo->IsNeedToCheckSchoolImmune())
    {
        // If m_immuneToSchool type contain this school type, IMMUNE damage.
        SpellImmuneList const& schoolList = m_spellImmune[IMMUNITY_SCHOOL];
        for (SpellImmuneList::const_iterator itr = schoolList.begin(); itr != schoolList.end(); ++itr)
            if (itr->type & shoolMask && !spellInfo->CanPierceImmuneAura(sSpellMgr->GetSpellInfo(itr->spellId)))
                return true;
    }

    // If m_immuneToDamage type contain magic, IMMUNE damage.
    SpellImmuneList const& damageList = m_spellImmune[IMMUNITY_DAMAGE];
    for (SpellImmuneList::const_iterator itr = damageList.begin(); itr != damageList.end(); ++itr)
        if (itr->type & shoolMask)
            return true;

    return false;
}

bool Unit::IsImmunedToSpell(SpellInfo const* spellInfo)
{
    if (!spellInfo)
        return false;

    // Single spell immunity.
    SpellImmuneList const& idList = m_spellImmune[IMMUNITY_ID];
    for (SpellImmuneList::const_iterator itr = idList.begin(); itr != idList.end(); ++itr)
        if (itr->type == spellInfo->Id)
            return true;

    if (spellInfo->Attributes & SPELL_ATTR0_UNAFFECTED_BY_INVULNERABILITY)
        return false;

    if (spellInfo->Dispel)
    {
        SpellImmuneList const& dispelList = m_spellImmune[IMMUNITY_DISPEL];
        for (SpellImmuneList::const_iterator itr = dispelList.begin(); itr != dispelList.end(); ++itr)
            if (itr->type == spellInfo->Dispel)
                return true;
    }

    // Spells that don't have effectMechanics.
    if (spellInfo->Mechanic)
    {
        SpellImmuneList const& mechanicList = m_spellImmune[IMMUNITY_MECHANIC];
        for (SpellImmuneList::const_iterator itr = mechanicList.begin(); itr != mechanicList.end(); ++itr)
            if (itr->type == spellInfo->Mechanic)
                return true;
    }

    bool immuneToAllEffects = true;
    bool immuneVariableChanged = false;
    for (uint8 i = 0; i < MAX_SPELL_EFFECTS; ++i)
    {
        // State/effect immunities applied by aura expect full spell immunity
        // Ignore effects with mechanic, they are supposed to be checked separately
        if (!spellInfo->Effects[i].IsEffect())
            continue;
        immuneVariableChanged = true;
        if (!IsImmunedToSpellEffect(spellInfo, i))
        {
            immuneToAllEffects = false;
            break;
        }
    }

    if (immuneToAllEffects && immuneVariableChanged) //Return immune only if the target is immune to all spell effects.
        return true;

    if (spellInfo->IsNeedToCheckSchoolImmune())
    {
        SpellImmuneList const& schoolList = m_spellImmune[IMMUNITY_SCHOOL];
        for (SpellImmuneList::const_iterator itr = schoolList.begin(); itr != schoolList.end(); ++itr)
        {
            SpellInfo const* immuneSpellInfo = sSpellMgr->GetSpellInfo(itr->spellId);
            if ((itr->type & spellInfo->GetSchoolMask())
                && !(immuneSpellInfo && immuneSpellInfo->IsPositive() && spellInfo->IsPositive())
                && !spellInfo->CanPierceImmuneAura(immuneSpellInfo))
                return true;
        }
    }

    return false;
}

bool Unit::IsImmunedToSpellEffect(SpellInfo const* spellInfo, uint32 index) const
{
    if (!spellInfo || !spellInfo->Effects[index].IsEffect())
        return false;

    if (spellInfo->Attributes & SPELL_ATTR0_UNAFFECTED_BY_INVULNERABILITY)
        return false;

    // If m_immuneToEffect type contain this effect type, IMMUNE effect.
    uint32 effect = spellInfo->Effects[index].Effect;
    SpellImmuneList const& effectList = m_spellImmune[IMMUNITY_EFFECT];
    for (SpellImmuneList::const_iterator itr = effectList.begin(); itr != effectList.end(); ++itr)
        if (itr->type == effect)
            return true;

    if (uint32 mechanic = spellInfo->Effects[index].Mechanic)
    {
        SpellImmuneList const& mechanicList = m_spellImmune[IMMUNITY_MECHANIC];
        for (SpellImmuneList::const_iterator itr = mechanicList.begin(); itr != mechanicList.end(); ++itr)
            if (itr->type == mechanic)
                return true;
    }

    if (uint32 aura = spellInfo->Effects[index].ApplyAuraName)
    {
        SpellImmuneList const& list = m_spellImmune[IMMUNITY_STATE];
        for (SpellImmuneList::const_iterator itr = list.begin(); itr != list.end(); ++itr)
            if (itr->type == aura)
                if (!(spellInfo->AttributesEx3 & SPELL_ATTR3_IGNORE_HIT_RESULT))
                    return true;

        // Check for immune to application of harmful magical effects
        AuraEffectList const& immuneAuraApply = GetAuraEffectsByType(SPELL_AURA_MOD_IMMUNE_AURA_APPLY_SCHOOL);
        for (AuraEffectList::const_iterator iter = immuneAuraApply.begin(); iter != immuneAuraApply.end(); ++iter)
            if (((*iter)->GetMiscValue() & spellInfo->GetSchoolMask()) &&  // Check school
                !spellInfo->IsPositiveEffect(index) && !spellInfo->CanPierceImmuneAura((*iter)->GetSpellInfo()))                        // Harmful
                return true;
    }

    if (!spellInfo->IsPositiveEffect(index))
    {
        uint32 shoolMask = spellInfo->GetSchoolMask();

        // If m_immuneToSchool type contain this school type, IMMUNE damage.
        SpellImmuneList const& schoolList = m_spellImmune[IMMUNITY_SCHOOL];
        for (SpellImmuneList::const_iterator itr = schoolList.begin(); itr != schoolList.end(); ++itr)
            if (itr->type & shoolMask && !spellInfo->CanPierceImmuneAura(sSpellMgr->GetSpellInfo(itr->spellId)))
                return true;
    }

    return false;
}

uint32 Unit::MeleeDamageBonusDone(Unit* victim, uint32 pdamage, WeaponAttackType attType, SpellInfo const* spellProto)
{
    if (!victim || pdamage == 0)
        return 0;

    uint32 creatureTypeMask = victim->GetCreatureTypeMask();

    // Done fixed damage bonus auras
    int32 DoneFlatBenefit = 0;

    // ..done
    AuraEffectList const& mDamageDoneCreature = GetAuraEffectsByType(SPELL_AURA_MOD_DAMAGE_DONE_CREATURE);
    for (AuraEffectList::const_iterator i = mDamageDoneCreature.begin(); i != mDamageDoneCreature.end(); ++i)
        if (creatureTypeMask & uint32((*i)->GetMiscValue()))
            DoneFlatBenefit += (*i)->GetAmount();

    // ..done
    // SPELL_AURA_MOD_DAMAGE_DONE included in weapon damage

    // ..done (base at attack power for marked target and base at attack power for creature type)
    int32 APbonus = 0;

    if (attType == RANGED_ATTACK)
    {
        APbonus += victim->GetMaxPositiveAuraModifier(SPELL_AURA_RANGED_ATTACK_POWER_ATTACKER_BONUS);

        // ..done (base at attack power and creature type)
        AuraEffectList const& mCreatureAttackPower = GetAuraEffectsByType(SPELL_AURA_MOD_RANGED_ATTACK_POWER_VERSUS);
        for (AuraEffectList::const_iterator i = mCreatureAttackPower.begin(); i != mCreatureAttackPower.end(); ++i)
            if (creatureTypeMask & uint32((*i)->GetMiscValue()))
                APbonus += (*i)->GetAmount();
    }
    else
    {
        APbonus += victim->GetTotalAuraModifier(SPELL_AURA_MELEE_ATTACK_POWER_ATTACKER_BONUS);

        // ..done (base at attack power and creature type)
        AuraEffectList const& mCreatureAttackPower = GetAuraEffectsByType(SPELL_AURA_MOD_MELEE_ATTACK_POWER_VERSUS);
        for (AuraEffectList::const_iterator i = mCreatureAttackPower.begin(); i != mCreatureAttackPower.end(); ++i)
            if (creatureTypeMask & uint32((*i)->GetMiscValue()))
                APbonus += (*i)->GetAmount();
    }

    if (APbonus != 0)                                       // Can be negative
    {
        bool normalized = false;
        if (spellProto)
            for (uint8 i = 0; i < MAX_SPELL_EFFECTS; ++i)
                if (spellProto->Effects[i].Effect == SPELL_EFFECT_NORMALIZED_WEAPON_DMG)
                {
                    normalized = true;
                    break;
                }
        DoneFlatBenefit += int32(APbonus/14.0f * GetAPMultiplier(attType, normalized));
    }

    // Done total percent damage auras
    float DoneTotalMod = 1.0f;

    // Some spells don't benefit from pct done mods
    if (spellProto)
        if (!(spellProto->AttributesEx6 & SPELL_ATTR6_NO_DONE_PCT_DAMAGE_MODS) && !spellProto->IsRankOf(sSpellMgr->GetSpellInfo(12162)))
        {
            AuraEffectList const& mModDamagePercentDone = GetAuraEffectsByType(SPELL_AURA_MOD_DAMAGE_PERCENT_DONE);
            for (AuraEffectList::const_iterator i = mModDamagePercentDone.begin(); i != mModDamagePercentDone.end(); ++i)
            {
                if ((*i)->GetMiscValue() & spellProto->GetSchoolMask() && !(spellProto->GetSchoolMask() & SPELL_SCHOOL_MASK_NORMAL))
                {
                    if ((*i)->GetSpellInfo()->EquippedItemClass == -1)
                        AddPct(DoneTotalMod, (*i)->GetAmount());
                    else if (!((*i)->GetSpellInfo()->AttributesEx5 & SPELL_ATTR5_SPECIAL_ITEM_CLASS_CHECK) && ((*i)->GetSpellInfo()->EquippedItemSubClassMask == 0))
                        AddPct(DoneTotalMod, (*i)->GetAmount());
                    else if (ToPlayer() && ToPlayer()->HasItemFitToSpellRequirements((*i)->GetSpellInfo()))
                        AddPct(DoneTotalMod, (*i)->GetAmount());
                }
            }
        }

    AuraEffectList const& mDamageDoneVersus = GetAuraEffectsByType(SPELL_AURA_MOD_DAMAGE_DONE_VERSUS);
    for (AuraEffectList::const_iterator i = mDamageDoneVersus.begin(); i != mDamageDoneVersus.end(); ++i)
        if (creatureTypeMask & uint32((*i)->GetMiscValue()))
            AddPct(DoneTotalMod, (*i)->GetAmount());

    // bonus against aurastate
    AuraEffectList const& mDamageDoneVersusAurastate = GetAuraEffectsByType(SPELL_AURA_MOD_DAMAGE_DONE_VERSUS_AURASTATE);
    for (AuraEffectList::const_iterator i = mDamageDoneVersusAurastate.begin(); i != mDamageDoneVersusAurastate.end(); ++i)
        if (victim->HasAuraState(AuraStateType((*i)->GetMiscValue())))
            AddPct(DoneTotalMod, (*i)->GetAmount());

    // Add SPELL_AURA_MOD_DAMAGE_DONE_FOR_MECHANIC percent bonus
    if (spellProto)
        AddPct(DoneTotalMod, GetTotalAuraModifierByMiscValue(SPELL_AURA_MOD_DAMAGE_DONE_FOR_MECHANIC, spellProto->Mechanic));

    // Frost Strike - Merciless Combat
    if (spellProto && spellProto->SpellFamilyName == SPELLFAMILY_DEATHKNIGHT && spellProto->SpellFamilyFlags[1] & 0x4)
        if (victim->HealthBelowPct(35))
            if (AuraEffect const * eff = GetDummyAuraEffect(SPELLFAMILY_DEATHKNIGHT, 2656, EFFECT_0))
                AddPct(DoneTotalMod, eff->GetAmount());

    // done scripted mod (take it from owner)
    // Unit* owner = GetOwner() ? GetOwner() : this;
    // AuraEffectList const& mOverrideClassScript = owner->GetAuraEffectsByType(SPELL_AURA_OVERRIDE_CLASS_SCRIPTS);

    // SPELL_AURA_MOD_AUTOATTACK_DAMAGE
    if (!spellProto)
    {
        Item *weapon = 0;
        if (GetTypeId() == TYPEID_PLAYER)
            weapon = this->ToPlayer()->GetWeaponForAttack(BASE_ATTACK, true);

        AuraEffectList const &mDamageDoneAutoAttacks = GetAuraEffectsByType(SPELL_AURA_MOD_AUTOATTACK_DAMAGE);
        for (AuraEffectList::const_iterator i = mDamageDoneAutoAttacks.begin(); i != mDamageDoneAutoAttacks.end(); ++i)
            if (!weapon || weapon->IsFitToSpellRequirements((*i)->GetSpellInfo()))
                DoneTotalMod *= ((*i)->GetAmount()+100.0f)/100.0f;
    }

    float tmpDamage = float(int32(pdamage) + DoneFlatBenefit) * DoneTotalMod;

    // apply spellmod to Done damage
    if (spellProto)
        if (Player* modOwner = GetSpellModOwner())
            modOwner->ApplySpellMod(spellProto->Id, SPELLMOD_DAMAGE, tmpDamage);

    // bonus result can be negative
    return uint32(std::max(tmpDamage, 0.0f));
}

uint32 Unit::MeleeDamageBonusTaken(Unit* attacker, uint32 pdamage, WeaponAttackType attType, SpellInfo const* spellProto)
{
    if (pdamage == 0)
        return 0;

    int32 TakenFlatBenefit = 0;
    float TakenTotalCasterMod = 0.0f;

    // get all auras from caster that allow the spell to ignore resistance (sanctified wrath)
    SpellSchoolMask attackSchoolMask = spellProto ? spellProto->GetSchoolMask() : SPELL_SCHOOL_MASK_NORMAL;
    AuraEffectList const& IgnoreResistAuras = attacker->GetAuraEffectsByType(SPELL_AURA_MOD_IGNORE_TARGET_RESIST);
    for (AuraEffectList::const_iterator i = IgnoreResistAuras.begin(); i != IgnoreResistAuras.end(); ++i)
    {
        if ((*i)->GetMiscValue() & attackSchoolMask)
            TakenTotalCasterMod += (float((*i)->GetAmount()));
    }

    // ..taken
    AuraEffectList const& mDamageTaken = GetAuraEffectsByType(SPELL_AURA_MOD_DAMAGE_TAKEN);
    for (AuraEffectList::const_iterator i = mDamageTaken.begin(); i != mDamageTaken.end(); ++i)
        if ((*i)->GetMiscValue() & GetMeleeDamageSchoolMask())
            TakenFlatBenefit += (*i)->GetAmount();

    if (attType != RANGED_ATTACK)
        TakenFlatBenefit += GetTotalAuraModifier(SPELL_AURA_MOD_MELEE_DAMAGE_TAKEN);
    else
        TakenFlatBenefit += GetTotalAuraModifier(SPELL_AURA_MOD_RANGED_DAMAGE_TAKEN);

    // Taken total percent damage auras
    float TakenTotalMod = 1.0f;

    // ..taken
    TakenTotalMod *= GetTotalAuraMultiplierByMiscMask(SPELL_AURA_MOD_DAMAGE_PERCENT_TAKEN, 
    spellProto ? spellProto->GetSchoolMask() : GetMeleeDamageSchoolMask());

    // .. taken pct (special attacks)
    if (spellProto)
    {
        // From caster spells
        AuraEffectList const& mOwnerTaken = GetAuraEffectsByType(SPELL_AURA_MOD_DAMAGE_FROM_CASTER);
        for (AuraEffectList::const_iterator i = mOwnerTaken.begin(); i != mOwnerTaken.end(); ++i)
            if ((*i)->GetCasterGUID() == attacker->GetGUID())
            {
                switch ((*i)->GetId())
                {
                    // Vendetta, should affect to all damage
                    case 79140:
                        AddPct(TakenTotalMod, (*i)->GetAmount());
                        break;
                    default:
                        if ((*i)->IsAffectingSpell(spellProto))
                            AddPct(TakenTotalMod, (*i)->GetAmount());
                        break;
                }
            }

        // Mod damage from spell mechanic
        uint32 mechanicMask = spellProto->GetAllEffectsMechanicMask();

        // Shred - "Effects which increase Bleed damage also increase Shred damage"
        if (spellProto->Id == 5221)
            mechanicMask |= (1<<MECHANIC_BLEED);

        if (mechanicMask)
        {
            int32 maxval = 0;
            AuraEffectList const& mDamageDoneMechanic = GetAuraEffectsByType(SPELL_AURA_MOD_MECHANIC_DAMAGE_TAKEN_PERCENT);
            for (AuraEffectList::const_iterator i = mDamageDoneMechanic.begin(); i != mDamageDoneMechanic.end(); ++i)
                if (mechanicMask & uint32(1<<((*i)->GetMiscValue())))
                    maxval = std::max(maxval, (*i)->GetAmount());

            if (maxval)
                AddPct(TakenTotalMod, maxval); 
        }
    }
    else
    {
        AuraEffectList const& mOwnerTaken = GetAuraEffectsByType(SPELL_AURA_MOD_AUTOATTACK_DAMAGE_TARGET);
        for (AuraEffectList::const_iterator i = mOwnerTaken.begin(); i != mOwnerTaken.end(); ++i)   
        {
            if ((*i)->GetCaster() == attacker)
                AddPct(TakenTotalMod, (*i)->GetAmount());
        }
    }

    // .. taken pct: dummy auras
    /*AuraEffectList const& mDummyAuras = GetAuraEffectsByType(SPELL_AURA_DUMMY);
    for (AuraEffectList::const_iterator i = mDummyAuras.begin(); i != mDummyAuras.end(); ++i)
    {
        switch ((*i)->GetSpellInfo()->SpellIconID)
        {
        }
    }*/

    // .. taken pct: class scripts
    //*AuraEffectList const& mclassScritAuras = GetAuraEffectsByType(SPELL_AURA_OVERRIDE_CLASS_SCRIPTS);
    //for (AuraEffectList::const_iterator i = mclassScritAuras.begin(); i != mclassScritAuras.end(); ++i)
    //{
    //    switch ((*i)->GetMiscValue())
    //    {
    //    }
    //}*/

    if (attType != RANGED_ATTACK)
    {
        AuraEffectList const& mModMeleeDamageTakenPercent = GetAuraEffectsByType(SPELL_AURA_MOD_MELEE_DAMAGE_TAKEN_PCT);
        for (AuraEffectList::const_iterator i = mModMeleeDamageTakenPercent.begin(); i != mModMeleeDamageTakenPercent.end(); ++i)
            AddPct(TakenTotalMod, (*i)->GetAmount());
    }
    else
    {
        AuraEffectList const& mModRangedDamageTakenPercent = GetAuraEffectsByType(SPELL_AURA_MOD_RANGED_DAMAGE_TAKEN_PCT);
        for (AuraEffectList::const_iterator i = mModRangedDamageTakenPercent.begin(); i != mModRangedDamageTakenPercent.end(); ++i)
            AddPct(TakenTotalMod, (*i)->GetAmount());
    }
    
    if (spellProto && spellProto->IsTargetingArea())
    {
        int32 mult = GetTotalAuraModifierByMiscMask(SPELL_AURA_MOD_AOE_DAMAGE_AVOIDANCE, spellProto->SchoolMask);
        AddPct(TakenTotalMod, mult);
        if (attacker->GetTypeId() == TYPEID_UNIT)
        {
            int32 u_mult = GetTotalAuraModifierByMiscMask(SPELL_AURA_MOD_CREATURE_AOE_DAMAGE_AVOIDANCE, spellProto->SchoolMask);
            AddPct(TakenTotalMod, u_mult);
        }
    }

    float tmpDamage = 0.0f;

    if (TakenTotalCasterMod)
    {
        if (TakenFlatBenefit < 0)
        {
            if (TakenTotalMod < 1)
                tmpDamage = ((float(CalculatePct(pdamage, TakenTotalCasterMod) + TakenFlatBenefit) * TakenTotalMod) + CalculatePct(pdamage, TakenTotalCasterMod));
            else
                tmpDamage = ((float(CalculatePct(pdamage, TakenTotalCasterMod) + TakenFlatBenefit) + CalculatePct(pdamage, TakenTotalCasterMod)) * TakenTotalMod);
        }
        else if (TakenTotalMod < 1)
            tmpDamage = ((CalculatePct(float(pdamage) + TakenFlatBenefit, TakenTotalCasterMod) * TakenTotalMod) + CalculatePct(float(pdamage) + TakenFlatBenefit, TakenTotalCasterMod));
    }
    if (!tmpDamage)
        tmpDamage = (float(pdamage) + TakenFlatBenefit) * TakenTotalMod;

    // bonus result can be negative
    return uint32(std::max(tmpDamage, 0.0f));
}

void Unit::ApplyUberImmune(uint32 spellid, bool apply)
{
    if (apply)
        RemoveAurasWithMechanic(IMMUNE_TO_MOVEMENT_IMPAIRMENT_AND_LOSS_CONTROL_MASK, AURA_REMOVE_BY_DEFAULT, spellid);
    for (uint32 mech=MECHANIC_CHARM; mech!=MECHANIC_ENRAGED; ++mech)
    {
        if (mech == MECHANIC_DISARM)
            continue;
        if (1<<mech & IMMUNE_TO_MOVEMENT_IMPAIRMENT_AND_LOSS_CONTROL_MASK)
            ApplySpellImmune(spellid, IMMUNITY_MECHANIC, mech, apply);
    }
}

void Unit::ApplySpellImmune(uint32 spellId, uint32 op, uint32 type, bool apply)
{
    if (apply)
    {
        for (SpellImmuneList::iterator itr = m_spellImmune[op].begin(), next; itr != m_spellImmune[op].end(); itr = next)
        {
            next = itr; ++next;
            if (itr->type == type)
            {
                m_spellImmune[op].erase(itr);
                next = m_spellImmune[op].begin();
            }
        }
        SpellImmune Immune;
        Immune.spellId = spellId;
        Immune.type = type;
        m_spellImmune[op].push_back(Immune);
    }
    else
    {
        for (SpellImmuneList::iterator itr = m_spellImmune[op].begin(); itr != m_spellImmune[op].end(); ++itr)
        {
            if (itr->spellId == spellId && itr->type == type)
            {
                m_spellImmune[op].erase(itr);
                break;
            }
        }
    }
}

void Unit::ApplySpellDispelImmunity(const SpellInfo* spellProto, DispelType type, bool apply)
{
    ApplySpellImmune(spellProto->Id, IMMUNITY_DISPEL, type, apply);

    if (apply && spellProto->AttributesEx & SPELL_ATTR1_DISPEL_AURAS_ON_IMMUNITY)
    {
        // Create dispel mask by dispel type
        uint32 dispelMask = SpellInfo::GetDispelMask(type);
        // Dispel all existing auras vs current dispel type
        AuraApplicationMap& auras = GetAppliedAuras();
        for (AuraApplicationMap::iterator itr = auras.begin(); itr != auras.end();)
        {
            SpellInfo const* spell = itr->second->GetBase()->GetSpellInfo();
            if (spell->GetDispelMask() & dispelMask)
            {
                // Dispel aura
                RemoveAura(itr);
            }
            else
                ++itr;
        }
    }
}

float Unit::GetWeaponProcChance() const
{
    // normalized proc chance for weapon attack speed
    // (odd formula...)
    if (isAttackReady(BASE_ATTACK))
        return (GetAttackTime(BASE_ATTACK) * 1.8f / 1000.0f);
    else if (haveOffhandWeapon() && isAttackReady(OFF_ATTACK))
        return (GetAttackTime(OFF_ATTACK) * 1.6f / 1000.0f);
    return 0;
}

float Unit::GetPPMProcChance(uint32 WeaponSpeed, float PPM, const SpellInfo* spellProto) const
{
    // proc per minute chance calculation
    if (PPM <= 0)
        return 0.0f;

    // Apply chance modifer aura
    if (spellProto)
        if (Player* modOwner = GetSpellModOwner())
            modOwner->ApplySpellMod(spellProto->Id, SPELLMOD_PROC_PER_MINUTE, PPM);

    return floor((WeaponSpeed * PPM) / 600.0f);   // result is chance in percents (probability = Speed_in_sec * (PPM / 60))
}

void Unit::Mount(uint32 mount, uint32 VehicleId, uint32 creatureEntry)
{
    if (mount)
        SetUInt32Value(UNIT_FIELD_MOUNTDISPLAYID, mount);

    SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_MOUNT);

    if (Player* player = ToPlayer())
    {
        
        if (player->GetEmoteState())
            player->SetEmoteState(0);

        // mount as a vehicle
        if (VehicleId)
        {
            if (CreateVehicleKit(VehicleId, creatureEntry))
            {
                GetVehicleKit()->Reset();

                // Send others that we now have a vehicle
                WorldPacket data(SMSG_PLAYER_VEHICLE_DATA, GetPackGUID().size()+4);
                data.appendPackGUID(GetGUID());
                data << uint32(VehicleId);
                SendMessageToSet(&data, true);

                data.Initialize(SMSG_ON_CANCEL_EXPECTED_RIDE_VEHICLE_AURA, 0);
                player->GetSession()->SendPacket(&data);

                // mounts can also have accessories
                GetVehicleKit()->InstallAllAccessories(false);
            }
        }

        //player->SendMovementSetCollisionHeight(player->GetCollisionHeight(true));
    }

    RemoveAurasWithInterruptFlags(AURA_INTERRUPT_FLAG_MOUNT);
}

void Unit::Dismount()
{
    if (!IsMounted())
        return;

    SetUInt32Value(UNIT_FIELD_MOUNTDISPLAYID, 0);
    RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_MOUNT);

    //if (Player* thisPlayer = ToPlayer())
    //    thisPlayer->SendMovementSetCollisionHeight(thisPlayer->GetCollisionHeight(false));

    WorldPacket data(SMSG_DISMOUNT, 8);
    data.appendPackGUID(GetGUID());
    SendMessageToSet(&data, true);

    // dismount as a vehicle
    if (GetTypeId() == TYPEID_PLAYER && GetVehicleKit())
    {
        // Send other players that we are no longer a vehicle
        data.Initialize(SMSG_PLAYER_VEHICLE_DATA, 8+4);
        data.appendPackGUID(GetGUID());
        data << uint32(0);
        ToPlayer()->SendMessageToSet(&data, true);
        // Remove vehicle from player
        RemoveVehicleKit();
    }

    RemoveAurasWithInterruptFlags(AURA_INTERRUPT_FLAG_NOT_MOUNTED);

    // enable pet
    if (Player* player = ToPlayer())
    {
        if (Pet* pPet = player->GetPet())
        {
            if (pPet->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_STUNNED) && !pPet->HasUnitState(UNIT_STATE_STUNNED))
                pPet->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_STUNNED);
        }
        else
        {
            player->ResummonPetTemporaryUnSummonedIfAny();
        }
    }
    RemoveAurasByType(SPELL_AURA_MOUNTED);
}

void Unit::UpdateMount()
{
    MountCapabilityEntry const* newMount = NULL;
    AuraEffect* effect = NULL;

    // First get the mount type
    MountTypeEntry const* mountType = NULL;
    {
        AuraEffectList const& auras = GetAuraEffectsByType(SPELL_AURA_MOUNTED);
        for (AuraEffectList::const_reverse_iterator itr = auras.rbegin(); itr != auras.rend(); ++itr)
        {
            AuraEffect* aura = *itr;
            aura->GetMiscValueB();
            mountType = sMountTypeStore.LookupEntry(uint32(aura->GetMiscValueB()));
            if (mountType)
            {
                effect = aura;
                break;
            }
        }
    }

    if (mountType)
    {
        uint32 zoneId, topZoneId, areaId;
        GetZoneAndAreaId(zoneId, areaId);
        topZoneId = zoneId;
        while (true)
        {
            AreaTableEntry const* zone = GetAreaEntryByAreaID(topZoneId);
            if (!zone)
                break;
            if (zone->zone == 0)
                break;
            topZoneId = zone->zone;
        }

        uint32 ridingSkill = 5000;
        if (GetTypeId() == TYPEID_PLAYER)
            ridingSkill = ToPlayer()->GetSkillValue(SKILL_RIDING);

        // Find the currently allowed mount flags
        uint32 currentMountFlags;
        {
            AuraEffectList const& auras = GetAuraEffectsByType(SPELL_AURA_MOD_FLYING_RESTRICTIONS);
            if (!auras.empty())
            {
                currentMountFlags = 0;
                for (AuraEffectList::const_iterator itr = auras.begin(); itr != auras.end(); ++itr)
                    currentMountFlags |= (*itr)->GetMiscValue();
            }
            else
            {
                AreaTableEntry const* entry;
                entry = GetAreaEntryByAreaID(areaId);
                if (!entry)
                    entry = GetAreaEntryByAreaID(zoneId);

                if (entry)
                    currentMountFlags = entry->mountFlags;
            }
        }

        // Find the fitting mount
        for (uint32 i = MAX_MOUNT_CAPABILITIES-1; i < MAX_MOUNT_CAPABILITIES; --i)
        {
            uint32 id = mountType->MountCapability[i];
            if (!id)
                continue;

            MountCapabilityEntry const* mountCapability = sMountCapabilityStore.LookupEntry(id);
            if (!mountCapability)
                continue;

            if (ridingSkill < mountCapability->RequiredRidingSkill)
                continue;

            // Flags required to use this mount
            uint32 reqFlags = mountCapability->Flags;

            if (reqFlags&1 && !(currentMountFlags&1))
                continue;

            if (reqFlags&2 && !(currentMountFlags&2))
                continue;

            if (reqFlags&4 && !(currentMountFlags&4))
                continue;

            if (reqFlags&8 && !(currentMountFlags&8))
                continue;

            if (m_movementInfo.HavePitch)
            {
                if (!(reqFlags & MOUNT_FLAG_CAN_PITCH))
                    continue;
            }

            if (HasUnitMovementFlag(MOVEMENTFLAG_SWIMMING))
            {
                if (!(reqFlags & MOUNT_FLAG_CAN_SWIM))
                    continue;
            }

            //if (!(reqFlags & 3))
                //continue;

            if (mountCapability->RequiredMap != -1 && GetMapId() != uint32(mountCapability->RequiredMap))
                continue;

            if (mountCapability->RequiredArea && (mountCapability->RequiredArea != zoneId && mountCapability->RequiredArea != topZoneId && mountCapability->RequiredArea != areaId))
                continue;

            if (mountCapability->RequiredAura && !HasAura(mountCapability->RequiredAura))
                continue;

            if (mountCapability->RequiredSpell && (GetTypeId() != TYPEID_PLAYER || !ToPlayer()->HasSpell(mountCapability->RequiredSpell)))
                continue;

            newMount = mountCapability;
            break;
        }
    }

    if (_mount != newMount)
    {
        uint32 oldSpell = _mount ? _mount->SpeedModSpell : 0;
        bool oldFlyer = _mount ? (_mount->Flags & 2) : false;
        uint32 newSpell = newMount ? newMount->SpeedModSpell : 0;
        bool newFlyer = newMount ? (newMount->Flags & 2) : false;

        // This is required for displaying speeds on aura
        if (effect)
            effect->SetAmount(newMount ? newMount->Id : 0);

        if (oldSpell != newSpell)
        {
            if (oldSpell)
                RemoveAurasDueToSpell(oldSpell);

            if (newSpell)
                CastSpell(this, newSpell, true, NULL, effect);
        }

        if (oldFlyer != newFlyer)
            SetCanFly(newFlyer);

        //if (!(oldFlyer ^ newFlyer))
        //    SetCanFly(newFlyer);

        Player* player = ToPlayer();
        if (!player)
            player = m_movedPlayer;

        if (player)
            player->SendMovementCanFlyChange();

        _mount = newMount;
    }
}

void Unit::SendMountResult(MountResult error)
{
    ASSERT(uint32(error) < 0xB);
    ASSERT(ToPlayer());

    WorldPacket data(SMSG_MOUNTRESULT, 4);
    data << uint32(error);
    ToPlayer()->SendDirectMessage(&data);
}

void Unit::SetInCombatWith(Unit* enemy)
{
    Unit* eOwner = enemy->GetCharmerOrOwnerOrSelf();
    if (eOwner->IsPvP() || (eOwner->ToCreature() && eOwner->ToCreature()->isBattlegroundVehicle()))
    {
        SetInCombatState(true, enemy);
        return;
    }

    // check for duel
    if (eOwner->GetTypeId() == TYPEID_PLAYER && eOwner->ToPlayer()->duel)
    {
        Unit const* myOwner = GetCharmerOrOwnerOrSelf();
        if (((Player const*)eOwner)->duel->opponent == myOwner)
        {
            SetInCombatState(true, enemy);
            return;
        }
    }
    SetInCombatState(false, enemy);
}

void Unit::CombatStart(Unit* target, bool initialAggro)
{
    if (initialAggro)
    {
        if (!target->IsStandState())
            target->SetStandState(UNIT_STAND_STATE_STAND);

        if (!target->isInCombat() && target->GetTypeId() != TYPEID_PLAYER
            && !target->ToCreature()->HasReactState(REACT_PASSIVE) && target->ToCreature()->IsAIEnabled)
        {
            if (target->isPet())
                target->ToCreature()->AI()->AttackedBy(this); // PetAI has special handler before AttackStart()
            else
                target->ToCreature()->AI()->AttackStart(this);
        }

        SetInCombatWith(target);
        target->SetInCombatWith(this);
    }

    Unit* who = target->GetCharmerOrOwnerOrSelf();
    if (who->GetTypeId() == TYPEID_PLAYER)
        SetContestedPvP(who->ToPlayer());

    Player* me = GetCharmerOrOwnerPlayerOrPlayerItself();
    if (me && who->IsPvP()
        && (who->GetTypeId() != TYPEID_PLAYER
        || !me->duel || me->duel->opponent != who))
    {
        me->UpdatePvP(true);
        me->RemoveAurasWithInterruptFlags(AURA_INTERRUPT_FLAG_ENTER_PVP_COMBAT);
    }
}

void Unit::SetInCombatState(bool PvP, Unit* enemy, bool isControlled)
{
    // only alive units can be in combat
    if (!isAlive())
        return;

    if (PvP)
        m_CombatTimer = 5000;

    if (isInCombat() || HasUnitState(UNIT_STATE_EVADE))
        return;

    for (Unit::ControlList::iterator itr = m_Controlled.begin(); itr != m_Controlled.end(); ++itr)
        (*itr)->SetInCombatState(PvP, enemy, true);

    if (Creature* creature = ToCreature())
        if (IsAIEnabled && creature->AI()->IsPassived())
            return;

    SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IN_COMBAT);
    if (isControlled)
        SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PET_IN_COMBAT);

    if (Creature* creature = ToCreature())
    {
        // Set home position at place of engaging combat for escorted creatures
        if ((IsAIEnabled && creature->AI()->IsEscorted()) ||
            GetMotionMaster()->GetCurrentMovementGeneratorType() == WAYPOINT_MOTION_TYPE ||
            GetMotionMaster()->GetCurrentMovementGeneratorType() == POINT_MOTION_TYPE)
            creature->SetHomePosition(GetPositionX(), GetPositionY(), GetPositionZ(), GetOrientation());

        if (enemy)
        {
            if (IsAIEnabled)
            {
                creature->AI()->EnterCombat(enemy);
                RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PC); // unit has engaged in combat, remove immunity so players can fight back
            }
            if (creature->GetFormation())
                creature->GetFormation()->MemberAttackStart(creature, enemy);
        }

        if (isPet())
        {
            UpdateSpeed(MOVE_RUN, true);
            UpdateSpeed(MOVE_SWIM, true);
            UpdateSpeed(MOVE_FLIGHT, true);
        }

        if (!(creature->GetCreatureTemplate()->type_flags & CREATURE_TYPEFLAGS_MOUNTED_COMBAT))
            Dismount();
    }

    if (GetTypeId() == TYPEID_PLAYER && !ToPlayer()->IsInWorgenForm() && ToPlayer()->CanSwitch())
        ToPlayer()->SwitchToWorgenForm();

    for (Unit::ControlList::iterator itr = m_Controlled.begin(); itr != m_Controlled.end(); ++itr)
    {
        (*itr)->SetInCombatState(PvP, enemy);
        (*itr)->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PET_IN_COMBAT);
    }
}

void Unit::ClearInCombat()
{
    m_CombatTimer = 0;
    RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IN_COMBAT);

    // Player's state will be cleared in Player::UpdateContestedPvP
    if (Creature* creature = ToCreature())
    {
        if (creature->GetCreatureTemplate() && creature->GetCreatureTemplate()->unit_flags & UNIT_FLAG_IMMUNE_TO_PC)
            SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PC); // set immunity state to the one from db on evade

        ClearUnitState(UNIT_STATE_ATTACK_PLAYER);
        if (HasFlag(UNIT_DYNAMIC_FLAGS, UNIT_DYNFLAG_TAPPED))
            SetUInt32Value(UNIT_DYNAMIC_FLAGS, creature->GetCreatureTemplate()->dynamicflags);

        if (creature->isPet())
        {
            if (Unit* owner = GetOwner())
                for (uint8 i = 0; i < MAX_MOVE_TYPE; ++i)
                    if (owner->GetSpeedRate(UnitMoveType(i)) > GetSpeedRate(UnitMoveType(i)))
                        SetSpeed(UnitMoveType(i), owner->GetSpeedRate(UnitMoveType(i)), true);
        }
        else if (!isCharmed())
            return;
    }
    else if(Player* player = ToPlayer())
    {
        player->UpdatePotionCooldown();
        player->ResetHolyPowerTimer();
        sScriptMgr->OnPlayerLeaveCombat(player);
    }
    RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PET_IN_COMBAT);
}

bool Unit::isTargetableForAttack(bool checkFakeDeath) const
{
    if (!isAlive())
        return false;

    if (HasFlag(UNIT_FIELD_FLAGS,
        UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_NOT_SELECTABLE | UNIT_FLAG_IMMUNE_TO_PC))
        return false;

    if (GetTypeId() == TYPEID_PLAYER && ToPlayer()->isGameMaster())
        return false;

    return !HasUnitState(UNIT_STATE_UNATTACKABLE) && (!checkFakeDeath || !HasUnitState(UNIT_STATE_DIED));
}

bool Unit::IsValidAttackTarget(Unit const* target) const
{
    return _IsValidAttackTarget(target, NULL);
}

// function based on function Unit::CanAttack from 13850 client
bool Unit::_IsValidAttackTarget(Unit const* target, SpellInfo const* bySpell, WorldObject const* obj) const
{
    ASSERT(target);

    // can't attack self
    if (this == target)
        return false;

    // can't attack unattackable units or GMs
    if (target->HasUnitState(UNIT_STATE_UNATTACKABLE)
        || (target->GetTypeId() == TYPEID_PLAYER && target->ToPlayer()->isGameMaster()))
        return false;

    // can't attack own vehicle or passenger
    if (m_vehicle)
        if (IsOnVehicle(target) || m_vehicle->GetBase()->IsOnVehicle(target))
            return false;

    // can't attack invisible (ignore stealth for aoe spells) also if the area being looked at is from a spell use the dynamic object created instead of the casting unit.
    if ((!bySpell || !(bySpell->AttributesEx6 & SPELL_ATTR6_CAN_TARGET_INVISIBLE)) && (obj ? !obj->canSeeOrDetect(target, bySpell && bySpell->IsAffectingArea()) : !canSeeOrDetect(target, bySpell && bySpell->IsAffectingArea())))
        return false;

    // can't attack dead
    if ((!bySpell || !bySpell->IsAllowingDeadTarget()) && !target->isAlive())
       return false;

    // can't attack untargetable
    if ((!bySpell || !(bySpell->AttributesEx6 & SPELL_ATTR6_CAN_TARGET_UNTARGETABLE))
        && target->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE))
        return false;

    if (Player const* playerAttacker = ToPlayer())
    {
        if (playerAttacker->HasFlag(PLAYER_FLAGS, PLAYER_FLAGS_COMMENTATOR))
            return false;
    }
    // check flags
    if (target->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_TAXI_FLIGHT | UNIT_FLAG_NOT_ATTACKABLE_1 | UNIT_FLAG_UNK_16)
        || (!HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PVP_ATTACKABLE) && target->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_NPC))
        || (!target->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PVP_ATTACKABLE) && HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_NPC))
        || (HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PVP_ATTACKABLE) && target->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PC))
        // check if this is a world trigger cast - GOs are using world triggers to cast their spells, so we need to ignore their immunity flag here, this is a temp workaround, needs removal when go cast is implemented properly
        || (GetEntry() != WORLD_TRIGGER && target->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PVP_ATTACKABLE) && HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PC)))
        return false;

    // CvC case - can attack each other only when one of them is hostile
    if (!HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PVP_ATTACKABLE) && !target->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PVP_ATTACKABLE))
        return GetReactionTo(target) <= REP_HOSTILE || target->GetReactionTo(this) <= REP_HOSTILE;

    // PvP, PvC, CvP case
    // can't attack friendly targets
    if ( GetReactionTo(target) > REP_NEUTRAL
        || target->GetReactionTo(this) > REP_NEUTRAL)
        return false;

    // Not all neutral creatures can be attacked
    if (GetReactionTo(target) == REP_NEUTRAL &&
        target->GetReactionTo(this) == REP_NEUTRAL)
    {
        if  (!(target->GetTypeId() == TYPEID_PLAYER && GetTypeId() == TYPEID_PLAYER) &&
            !(target->GetTypeId() == TYPEID_UNIT && GetTypeId() == TYPEID_UNIT))
        {
            Player const* player = target->GetTypeId() == TYPEID_PLAYER ? target->ToPlayer() : ToPlayer();
            Unit const* creature = target->GetTypeId() == TYPEID_UNIT ? target : this;

            if (FactionTemplateEntry const* factionTemplate = creature->getFactionTemplateEntry())
            {
                if (!(player->GetReputationMgr().GetForcedRankIfAny(factionTemplate)))
                    if (FactionEntry const* factionEntry = sFactionStore.LookupEntry(factionTemplate->faction))
                        if (FactionState const* repState = player->GetReputationMgr().GetState(factionEntry))
                            if (!(repState->Flags & FACTION_FLAG_AT_WAR))
                                return false;

            }
        }
    }

    Creature const* creatureAttacker = ToCreature();
    if (creatureAttacker && creatureAttacker->GetCreatureTemplate()->type_flags & CREATURE_TYPEFLAGS_PARTY_MEMBER)
        return false;

    Player const* playerAffectingAttacker = HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PVP_ATTACKABLE) ? GetAffectingPlayer() : NULL;
    Player const* playerAffectingTarget = target->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PVP_ATTACKABLE) ? target->GetAffectingPlayer() : NULL;

    // check duel - before sanctuary checks
    if (playerAffectingAttacker && playerAffectingTarget)
        if (playerAffectingAttacker->duel && playerAffectingAttacker->duel->opponent == playerAffectingTarget && playerAffectingAttacker->duel->startTime != 0)
            return true;

    // PvP case - can't attack when attacker or target are in sanctuary
    // however, 13850 client doesn't allow to attack when one of the unit's has sanctuary flag and is pvp
    if (target->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PVP_ATTACKABLE) && HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PVP_ATTACKABLE)
        && ((target->GetByteValue(UNIT_FIELD_BYTES_2, 1) & UNIT_BYTE2_FLAG_SANCTUARY) || (GetByteValue(UNIT_FIELD_BYTES_2, 1) & UNIT_BYTE2_FLAG_SANCTUARY)))
        return false;

    // additional checks - only PvP case
    if (playerAffectingAttacker && playerAffectingTarget)
    {
        if (target->GetByteValue(UNIT_FIELD_BYTES_2, 1) & UNIT_BYTE2_FLAG_PVP)
            return true;

        if (GetByteValue(UNIT_FIELD_BYTES_2, 1) & UNIT_BYTE2_FLAG_FFA_PVP
            && target->GetByteValue(UNIT_FIELD_BYTES_2, 1) & UNIT_BYTE2_FLAG_FFA_PVP)
            return true;

        return (GetByteValue(UNIT_FIELD_BYTES_2, 1) & UNIT_BYTE2_FLAG_UNK1)
            || (target->GetByteValue(UNIT_FIELD_BYTES_2, 1) & UNIT_BYTE2_FLAG_UNK1);
    }
    return true;
}

bool Unit::IsValidAssistTarget(Unit const* target) const
{
    return _IsValidAssistTarget(target, NULL);
}

// function based on function Unit::CanAssist from 13850 client
bool Unit::_IsValidAssistTarget(Unit const* target, SpellInfo const* bySpell) const
{
    ASSERT(target);

    // can assist to self
    if (this == target)
        return true;

    // can't assist unattackable units or GMs
    if (target->HasUnitState(UNIT_STATE_UNATTACKABLE)
        || (target->GetTypeId() == TYPEID_PLAYER && target->ToPlayer()->isGameMaster()))
        return false;

    // can't assist own vehicle or passenger
    if (m_vehicle)
        if (IsOnVehicle(target) || m_vehicle->GetBase()->IsOnVehicle(target))
            return false;

    // can't assist invisible
    if ((!bySpell || !(bySpell->AttributesEx6 & SPELL_ATTR6_CAN_TARGET_INVISIBLE)) && !canSeeOrDetect(target, bySpell && bySpell->IsAffectingArea()))
        return false;

    // can't assist dead
    if ((!bySpell || !bySpell->IsAllowingDeadTarget()) && !target->isAlive())
       return false;

    // can't assist untargetable
    if ((!bySpell || !(bySpell->AttributesEx6 & SPELL_ATTR6_CAN_TARGET_UNTARGETABLE))
        && target->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE))
        return false;

    if (!bySpell || !(bySpell->AttributesEx6 & SPELL_ATTR6_UNK3))
    {
        if (HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PVP_ATTACKABLE))
        {
            if (target->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PC))
                return false;
        }
        else
        {
            if (target->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_NPC))
                return false;
        }
    }

    // can't assist non-friendly targets
    if (GetReactionTo(target) <= REP_NEUTRAL
        && target->GetReactionTo(this) <= REP_NEUTRAL
        && (!ToCreature() || !(ToCreature()->GetCreatureTemplate()->type_flags & CREATURE_TYPEFLAGS_PARTY_MEMBER)))
        return false;

    // PvP case
    if (target->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PVP_ATTACKABLE))
    {
        Player const* targetPlayerOwner = target->GetAffectingPlayer();
        if (HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PVP_ATTACKABLE))
        {
            Player const* selfPlayerOwner = GetAffectingPlayer();
            if (selfPlayerOwner && targetPlayerOwner)
            {
                // can't assist player which is dueling someone
                if (selfPlayerOwner != targetPlayerOwner
                    && targetPlayerOwner->duel)
                    return false;
            }
            // can't assist player in ffa_pvp zone from outside
            if ((target->GetByteValue(UNIT_FIELD_BYTES_2, 1) & UNIT_BYTE2_FLAG_FFA_PVP)
                && !(GetByteValue(UNIT_FIELD_BYTES_2, 1) & UNIT_BYTE2_FLAG_FFA_PVP))
                return false;
            // can't assist player out of sanctuary from sanctuary if has pvp enabled
            if (target->GetByteValue(UNIT_FIELD_BYTES_2, 1) & UNIT_BYTE2_FLAG_PVP)
                if ((GetByteValue(UNIT_FIELD_BYTES_2, 1) & UNIT_BYTE2_FLAG_SANCTUARY) && !(target->GetByteValue(UNIT_FIELD_BYTES_2, 1) & UNIT_BYTE2_FLAG_SANCTUARY))
                    return false;
        }
    }
    // PvC case - player can assist creature only if has specific type flags
    // !target->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PVP_ATTACKABLE) &&
    else if (HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PVP_ATTACKABLE)
        && (!bySpell || !(bySpell->AttributesEx6 & SPELL_ATTR6_UNK3))
        && !((target->GetByteValue(UNIT_FIELD_BYTES_2, 1) & UNIT_BYTE2_FLAG_PVP)))
    {
        if (Creature const* creatureTarget = target->ToCreature())
            return creatureTarget->GetCreatureTemplate()->type_flags & CREATURE_TYPEFLAGS_PARTY_MEMBER || creatureTarget->GetCreatureTemplate()->type_flags & CREATURE_TYPEFLAGS_AID_PLAYERS;
    }
    return true;
}

int32 Unit::ModifyHealth(int32 dVal)
{
    int32 gain = 0;

    if (dVal == 0)
        return 0;

    // Part of Evade mechanics. Only track health lost, not gained.
    if (dVal < 0 && GetTypeId() != TYPEID_PLAYER && !isPet())
        SetLastDamagedTime(time(NULL));

    int32 curHealth = (int32)GetHealth();

    int32 val = dVal + curHealth;
    if (val <= 0)
    {
        SetHealth(0);
        return -curHealth;
    }

    int32 maxHealth = (int32)GetMaxHealth();

    if (val < maxHealth)
    {
        SetHealth(val);
        gain = val - curHealth;
    }
    else if (curHealth != maxHealth)
    {
        SetHealth(maxHealth);
        gain = maxHealth - curHealth;
    }

    return gain;
}

int32 Unit::GetHealthGain(int32 dVal)
{
    int32 gain = 0;

    if (dVal == 0)
        return 0;

    int32 curHealth = (int32)GetHealth();

    int32 val = dVal + curHealth;
    if (val <= 0)
    {
        return -curHealth;
    }

    int32 maxHealth = (int32)GetMaxHealth();

    if (val < maxHealth)
        gain = dVal;
    else if (curHealth != maxHealth)
        gain = maxHealth - curHealth;

    return gain;
}

// returns negative amount on power reduction
int32 Unit::ModifyPower(Powers power, int32 dVal)
{
    int32 gain = 0;

    if (dVal == 0)
        return 0;

    // Modify POWER_RUNIC_POWER from SPELL_AURA_MOD_RUNE_POWER_REGEN
    // I have not found a better place for this
    if (power == POWER_RUNIC_POWER && dVal > 0)
    {
        int32 pct = GetTotalAuraModifier(SPELL_AURA_MOD_RUNE_POWER_REGEN);
        AddPct<int>(dVal, pct);
    }

    int32 curPower = GetPower(power);

    int32 val = dVal + curPower;
    if (val <= GetMinPower(power))
    {
        SetPower(power, GetMinPower(power));
        return -curPower;
    }

    int32 maxPower = GetMaxPower(power);

    if (val < maxPower)
    {
        SetPower(power, val);
        gain = val - curPower;
    }
    else if (curPower != maxPower)
    {
        SetPower(power, maxPower);
        gain = maxPower - curPower;
    }

    return gain;
}

// returns negative amount on power reduction
int32 Unit::ModifyPowerPct(Powers power, float pct, bool apply)
{
    float amount = (float)GetMaxPower(power);
    ApplyPercentModFloatVar(amount, pct, apply);

    return ModifyPower(power, (int32)amount - GetMaxPower(power));
}

bool Unit::IsAlwaysVisibleFor(WorldObject const* seer) const
{
    if (WorldObject::IsAlwaysVisibleFor(seer))
        return true;

    // Always seen by owner
    if (uint64 guid = GetCharmerOrOwnerGUID())
        if (seer->GetGUID() == guid)
            return true;

    if (Player const* seerPlayer = seer->ToPlayer())
        if (Unit* owner =  GetOwner())
            if (Player* ownerPlayer = owner->ToPlayer())
                if (ownerPlayer->IsGroupVisibleFor(seerPlayer))
                    return true;

    return false;
}

bool Unit::IsAlwaysDetectableFor(WorldObject const* seer) const
{
    if (WorldObject::IsAlwaysDetectableFor(seer))
        return true;

    return false;
}

void Unit::SetVisible(bool x)
{
    if (!x)
        m_serverSideVisibility.SetValue(SERVERSIDE_VISIBILITY_GM, SEC_GAMEMASTER);
    else
        m_serverSideVisibility.SetValue(SERVERSIDE_VISIBILITY_GM, SEC_PLAYER);

    UpdateObjectVisibility();
}

void Unit::UpdateSpeed(UnitMoveType mtype, bool forced)
{
    int32 main_speed_mod  = 0;
    float stack_bonus     = 1.0f;
    float non_stack_bonus = 1.0f;

    switch (mtype)
    {
        // Only apply debuffs
        case MOVE_FLIGHT_BACK:
        case MOVE_RUN_BACK:
        case MOVE_SWIM_BACK:
            break;
        case MOVE_WALK:
            return;
        case MOVE_RUN:
        {
            if (IsMounted()) // Use on mount auras
            {
                main_speed_mod  = GetMaxPositiveAuraModifier(SPELL_AURA_MOD_INCREASE_MOUNTED_SPEED);
                stack_bonus     = GetTotalAuraMultiplier(SPELL_AURA_MOD_MOUNTED_SPEED_ALWAYS);
                non_stack_bonus += GetMaxPositiveAuraModifier(SPELL_AURA_MOD_MOUNTED_SPEED_NOT_STACK) / 100.0f;
            }
            else
            {
                main_speed_mod  = GetMaxPositiveAuraModifier(SPELL_AURA_MOD_INCREASE_SPEED);
                stack_bonus     = GetTotalAuraMultiplier(SPELL_AURA_MOD_SPEED_ALWAYS);
                non_stack_bonus += GetMaxPositiveAuraModifier(SPELL_AURA_MOD_SPEED_NOT_STACK) / 100.0f;
            }
            break;
        }
        case MOVE_SWIM:
        {
            main_speed_mod  = GetMaxPositiveAuraModifier(SPELL_AURA_MOD_INCREASE_SWIM_SPEED);
            break;
        }
        case MOVE_FLIGHT:
        {
            if (GetTypeId() == TYPEID_UNIT && IsControlledByPlayer()) // not sure if good for pet
            {
                main_speed_mod  = GetMaxPositiveAuraModifier(SPELL_AURA_MOD_INCREASE_VEHICLE_FLIGHT_SPEED);
                stack_bonus     = GetTotalAuraMultiplier(SPELL_AURA_MOD_VEHICLE_SPEED_ALWAYS);

                // for some spells this mod is applied on vehicle owner
                int32 owner_speed_mod = 0;

                if (Unit* owner = GetCharmer())
                    owner_speed_mod = owner->GetMaxPositiveAuraModifier(SPELL_AURA_MOD_INCREASE_VEHICLE_FLIGHT_SPEED);

                main_speed_mod = std::max(main_speed_mod, owner_speed_mod);
            }
            else if (IsMounted())
            {
                main_speed_mod  = GetMaxPositiveAuraModifier(SPELL_AURA_MOD_INCREASE_MOUNTED_FLIGHT_SPEED);
                stack_bonus     = GetTotalAuraMultiplier(SPELL_AURA_MOD_MOUNTED_FLIGHT_SPEED_ALWAYS);
            }
            else             // Use not mount (shapeshift for example) auras (should stack)
                main_speed_mod  = GetTotalAuraModifier(SPELL_AURA_MOD_INCREASE_FLIGHT_SPEED) + GetTotalAuraModifier(SPELL_AURA_MOD_INCREASE_VEHICLE_FLIGHT_SPEED);

            non_stack_bonus += GetMaxPositiveAuraModifier(SPELL_AURA_MOD_FLIGHT_SPEED_NOT_STACK) / 100.0f;

            // Update speed for vehicle if available
            if (GetTypeId() == TYPEID_PLAYER && GetVehicle())
                GetVehicleBase()->UpdateSpeed(MOVE_FLIGHT, true);
            break;
        }
        case MOVE_TURN_RATE:
        case MOVE_PITCH_RATE:
            return;
        default:
            return;
    }

    // now we ready for speed calculation
    float speed = std::max(non_stack_bonus, stack_bonus);
    if (main_speed_mod)
        AddPct(speed, main_speed_mod);

    switch (mtype)
    {
        case MOVE_RUN:
        case MOVE_SWIM:
        case MOVE_FLIGHT:
        {
            // Set creature speed rate
            if (GetTypeId() == TYPEID_UNIT)
            {
                Unit* pOwner = GetCharmerOrOwner();
                if ((isPet() || isGuardian()) && !isInCombat() && pOwner) // Must check for owner or crash on "Tame Beast"
                {
                    // For every yard over 5, increase speed by 0.01
                    //  to help prevent pet from lagging behind and despawning
                    float dist = GetDistance(pOwner);
                    float base_rate = 1.00f; // base speed is 100% of owner speed
                    
                    if (dist < 5)
                        dist = 5;

                    float mult = base_rate + ((dist - 5) * 0.01f);

                    speed *= pOwner->GetSpeedRate(mtype) * mult; // pets derive speed from owner when not in combat
                }
                else
                    speed *= ToCreature()->GetCreatureTemplate()->speed_run;    // at this point, MOVE_WALK is never reached
            }
            break;
        }
        default:
            break;
    }

    // for creature case, we check explicit if mob searched for assistance
    if (GetTypeId() == TYPEID_UNIT)
    {
        if (ToCreature()->HasSearchedAssistance())
            speed *= 0.66f;                                 // best guessed value, so this will be 33% reduction. Based off initial speed, mob can then "run", "walk fast" or "walk".
    }

    // Apply strongest slow aura mod to speed
    int32 slow = GetMaxNegativeAuraModifier(SPELL_AURA_MOD_DECREASE_SPEED);

    if (int32 normalization = GetMaxPositiveAuraModifier(SPELL_AURA_USE_NORMAL_MOVEMENT_SPEED))
    {
        if (float max_speed = (normalization / (IsControlledByPlayer() ? playerBaseMoveSpeed[MOVE_RUN] : baseMoveSpeed[MOVE_RUN])))
            speed = max_speed;
    }
    
    AddPct(speed, slow);

    if (float minSpeedMod = (float)GetMaxPositiveAuraModifier(SPELL_AURA_MOD_MINIMUM_SPEED))
    {
        float min_speed = minSpeedMod / 100.0f;
        if (speed < min_speed)
            speed = min_speed;
    }
    

    SetSpeed(mtype, speed, forced);
}

void Unit::SetSpeed(UnitMoveType mtype, float rate, bool forced)
{
    if (fabs(rate) <= 0.00000023841858) // From client
        rate = 1.0f;

    // Update speed only on change
    bool clientSideOnly = m_speed_rate[mtype] == rate;

    m_speed_rate[mtype] = rate;

    if (!clientSideOnly)
        propagateSpeedChange();

    // Don't build packets because we've got noone to send
    // them to except self, and self is not created at client.
    if (!IsInWorld())
        return;

    WorldPacket data;
    ObjectGuid guid = GetGUID();
    if (!forced)
    {
        switch (mtype)
        {
            case MOVE_WALK:
                data.Initialize(SMSG_SPLINE_MOVE_SET_WALK_SPEED, 1 + 8 + 4);
                data.WriteByteMask(guid[0])
                    .WriteByteMask(guid[6])
                    .WriteByteMask(guid[7])
                    .WriteByteMask(guid[3])
                    .WriteByteMask(guid[5])
                    .WriteByteMask(guid[1])
                    .WriteByteMask(guid[2])
                    .WriteByteMask(guid[4])
                    .WriteByteSeq(guid[0])
                    .WriteByteSeq(guid[4])
                    .WriteByteSeq(guid[7])
                    .WriteByteSeq(guid[1])
                    .WriteByteSeq(guid[5])
                    .WriteByteSeq(guid[3]);
                data << float(GetSpeed(mtype));
                data
                    .WriteByteSeq(guid[2])
                    .WriteByteSeq(guid[5]);
                break;
            case MOVE_RUN:
                data.Initialize(SMSG_SPLINE_MOVE_SET_RUN_SPEED, 1 + 8 + 4);
                data.WriteByteMask(guid[4]);
                data.WriteByteMask(guid[0]);
                data.WriteByteMask(guid[5]);
                data.WriteByteMask(guid[7]);
                data.WriteByteMask(guid[6]);
                data.WriteByteMask(guid[3]);
                data.WriteByteMask(guid[1]);
                data.WriteByteMask(guid[2]);
                data.WriteByteSeq(guid[0]);
                data.WriteByteSeq(guid[7]);
                data.WriteByteSeq(guid[6]);
                data.WriteByteSeq(guid[5]);
                data.WriteByteSeq(guid[3]);
                data.WriteByteSeq(guid[4]);
                data << float(GetSpeed(mtype));
                data.WriteByteSeq(guid[2]);
                data.WriteByteSeq(guid[1]);
                break;
            case MOVE_RUN_BACK:
                data.Initialize(SMSG_SPLINE_MOVE_SET_RUN_BACK_SPEED, 1 + 8 + 4);
                data.WriteByteMask(guid[1]);
                data.WriteByteMask(guid[2]);
                data.WriteByteMask(guid[6]);
                data.WriteByteMask(guid[0]);
                data.WriteByteMask(guid[3]);
                data.WriteByteMask(guid[7]);
                data.WriteByteMask(guid[5]);
                data.WriteByteMask(guid[4]);
                data.WriteByteSeq(guid[1]);
                data << float(GetSpeed(mtype));
                data.WriteByteSeq(guid[2]);
                data.WriteByteSeq(guid[4]);
                data.WriteByteSeq(guid[0]);
                data.WriteByteSeq(guid[3]);
                data.WriteByteSeq(guid[6]);
                data.WriteByteSeq(guid[5]);
                data.WriteByteSeq(guid[7]);
                break;
            case MOVE_SWIM:
                data.Initialize(SMSG_SPLINE_MOVE_SET_SWIM_SPEED, 1 + 8 + 4);
                data.WriteByteMask(guid[4]);
                data.WriteByteMask(guid[2]);
                data.WriteByteMask(guid[5]);
                data.WriteByteMask(guid[0]);
                data.WriteByteMask(guid[7]);
                data.WriteByteMask(guid[6]);
                data.WriteByteMask(guid[3]);
                data.WriteByteMask(guid[1]);
                data.WriteByteSeq(guid[5]);
                data.WriteByteSeq(guid[6]);
                data.WriteByteSeq(guid[1]);
                data.WriteByteSeq(guid[0]);
                data.WriteByteSeq(guid[2]);
                data.WriteByteSeq(guid[4]);
                data << float(GetSpeed(mtype));
                data.WriteByteSeq(guid[7]);
                data.WriteByteSeq(guid[3]);
                break;
            case MOVE_SWIM_BACK:
                data.Initialize(SMSG_SPLINE_MOVE_SET_SWIM_BACK_SPEED, 1 + 8 + 4);
                data.WriteByteMask(guid[0]);
                data.WriteByteMask(guid[1]);
                data.WriteByteMask(guid[3]);
                data.WriteByteMask(guid[6]);
                data.WriteByteMask(guid[4]);
                data.WriteByteMask(guid[5]);
                data.WriteByteMask(guid[7]);
                data.WriteByteMask(guid[2]);
                data.WriteByteSeq(guid[5]);
                data.WriteByteSeq(guid[3]);
                data.WriteByteSeq(guid[1]);
                data.WriteByteSeq(guid[0]);
                data.WriteByteSeq(guid[7]);
                data.WriteByteSeq(guid[6]);
                data << float(GetSpeed(mtype));
                data.WriteByteSeq(guid[4]);
                data.WriteByteSeq(guid[2]);
                break;
            case MOVE_TURN_RATE:
                data.Initialize(SMSG_SPLINE_MOVE_SET_TURN_RATE, 1 + 8 + 4);
                data.WriteByteMask(guid[2]);
                data.WriteByteMask(guid[4]);
                data.WriteByteMask(guid[6]);
                data.WriteByteMask(guid[1]);
                data.WriteByteMask(guid[3]);
                data.WriteByteMask(guid[5]);
                data.WriteByteMask(guid[7]);
                data.WriteByteMask(guid[0]);

                data << float(GetSpeed(mtype));
                data.WriteByteSeq(guid[1]);
                data.WriteByteSeq(guid[5]);
                data.WriteByteSeq(guid[3]);
                data.WriteByteSeq(guid[2]);
                data.WriteByteSeq(guid[7]);
                data.WriteByteSeq(guid[4]);
                data.WriteByteSeq(guid[6]);
                data.WriteByteSeq(guid[0]);
                break;
            case MOVE_FLIGHT:
                data.Initialize(SMSG_SPLINE_MOVE_SET_FLIGHT_SPEED, 1 + 8 + 4);
                data.WriteByteMask(guid[7]);
                data.WriteByteMask(guid[4]);
                data.WriteByteMask(guid[0]);
                data.WriteByteMask(guid[1]);
                data.WriteByteMask(guid[3]);
                data.WriteByteMask(guid[6]);
                data.WriteByteMask(guid[5]);
                data.WriteByteMask(guid[2]);
                data.WriteByteSeq(guid[0]);
                data.WriteByteSeq(guid[5]);
                data.WriteByteSeq(guid[4]);
                data.WriteByteSeq(guid[7]);
                data.WriteByteSeq(guid[3]);
                data.WriteByteSeq(guid[2]);
                data.WriteByteSeq(guid[1]);
                data.WriteByteSeq(guid[6]);
                data << float(GetSpeed(mtype));
                break;
            case MOVE_FLIGHT_BACK:
                data.Initialize(SMSG_SPLINE_MOVE_SET_FLIGHT_BACK_SPEED, 1 + 8 + 4);
                data.WriteByteMask(guid[2]);
                data.WriteByteMask(guid[1]);
                data.WriteByteMask(guid[6]);
                data.WriteByteMask(guid[5]);
                data.WriteByteMask(guid[0]);
                data.WriteByteMask(guid[3]);
                data.WriteByteMask(guid[4]);
                data.WriteByteMask(guid[7]);
                data.WriteByteSeq(guid[5]);
                data << float(GetSpeed(mtype));
                data.WriteByteSeq(guid[6]);
                data.WriteByteSeq(guid[1]);
                data.WriteByteSeq(guid[0]);
                data.WriteByteSeq(guid[2]);
                data.WriteByteSeq(guid[3]);
                data.WriteByteSeq(guid[7]);
                data.WriteByteSeq(guid[4]);
                break;
            case MOVE_PITCH_RATE:
                data.Initialize(SMSG_SPLINE_MOVE_SET_PITCH_RATE, 1 + 8 + 4);
                data.WriteByteMask(guid[3]);
                data.WriteByteMask(guid[5]);
                data.WriteByteMask(guid[6]);
                data.WriteByteMask(guid[1]);
                data.WriteByteMask(guid[0]);
                data.WriteByteMask(guid[4]);
                data.WriteByteMask(guid[7]);
                data.WriteByteMask(guid[2]);
                data.WriteByteSeq(guid[1]);
                data.WriteByteSeq(guid[5]);
                data.WriteByteSeq(guid[7]);
                data.WriteByteSeq(guid[0]);
                data.WriteByteSeq(guid[6]);
                data.WriteByteSeq(guid[3]);
                data.WriteByteSeq(guid[2]);
                data << float(GetSpeed(mtype));
                data.WriteByteSeq(guid[4]);
                break;
            default:
                return;
        }

        SendMessageToSet(&data, true);
    }
    else
    {
        if (GetTypeId() == TYPEID_PLAYER)
        {
            // register forced speed changes for WorldSession::HandleForceSpeedChangeAck
            // and do it only for real sent packets and use run for run/mounted as client expected
            ++ToPlayer()->m_forced_speed_changes[mtype];

            if (!isInCombat())
                if (Pet* pet = ToPlayer()->GetPet())
                    pet->SetSpeed(mtype, m_speed_rate[mtype], forced);
        }

        switch (mtype)
        {
            case MOVE_WALK:
                data.Initialize(SMSG_MOVE_SET_WALK_SPEED, 1 + 8 + 4 + 4);
                data.WriteByteMask(guid[0]);
                data.WriteByteMask(guid[4]);
                data.WriteByteMask(guid[5]);
                data.WriteByteMask(guid[2]);
                data.WriteByteMask(guid[3]);
                data.WriteByteMask(guid[1]);
                data.WriteByteMask(guid[6]);
                data.WriteByteMask(guid[7]);
                data.WriteByteSeq(guid[6]);
                data.WriteByteSeq(guid[1]);
                data.WriteByteSeq(guid[5]);
                data << float(GetSpeed(mtype));
                data.WriteByteSeq(guid[2]);
                data << uint32(0);
                data.WriteByteSeq(guid[4]);
                data.WriteByteSeq(guid[0]);
                data.WriteByteSeq(guid[7]);
                data.WriteByteSeq(guid[3]);
                break;
            case MOVE_RUN:
                data.Initialize(SMSG_MOVE_SET_RUN_SPEED, 1 + 8 + 4 + 4);
                data.WriteByteMask(guid[6]);
                data.WriteByteMask(guid[1]);
                data.WriteByteMask(guid[5]);
                data.WriteByteMask(guid[2]);
                data.WriteByteMask(guid[7]);
                data.WriteByteMask(guid[0]);
                data.WriteByteMask(guid[3]);
                data.WriteByteMask(guid[4]);
                data.WriteByteSeq(guid[5]);
                data.WriteByteSeq(guid[3]);
                data.WriteByteSeq(guid[1]);
                data.WriteByteSeq(guid[4]);
                data << uint32(0);
                data << float(GetSpeed(mtype));
                data.WriteByteSeq(guid[6]);
                data.WriteByteSeq(guid[0]);
                data.WriteByteSeq(guid[7]);
                data.WriteByteSeq(guid[2]);
                break;
            case MOVE_RUN_BACK:
                data.Initialize(SMSG_MOVE_SET_RUN_BACK_SPEED, 1 + 8 + 4 + 4);
                data.WriteByteMask(guid[0]);
                data.WriteByteMask(guid[6]);
                data.WriteByteMask(guid[2]);
                data.WriteByteMask(guid[1]);
                data.WriteByteMask(guid[3]);
                data.WriteByteMask(guid[4]);
                data.WriteByteMask(guid[5]);
                data.WriteByteMask(guid[7]);
                data.WriteByteSeq(guid[5]);
                data << uint32(0);
                data << float(GetSpeed(mtype));
                data.WriteByteSeq(guid[0]);
                data.WriteByteSeq(guid[4]);
                data.WriteByteSeq(guid[7]);
                data.WriteByteSeq(guid[3]);
                data.WriteByteSeq(guid[1]);
                data.WriteByteSeq(guid[2]);
                data.WriteByteSeq(guid[6]);
                break;
            case MOVE_SWIM:
                data.Initialize(SMSG_MOVE_SET_SWIM_SPEED, 1 + 8 + 4 + 4);
                data.WriteByteMask(guid[5]);
                data.WriteByteMask(guid[4]);
                data.WriteByteMask(guid[7]);
                data.WriteByteMask(guid[3]);
                data.WriteByteMask(guid[2]);
                data.WriteByteMask(guid[0]);
                data.WriteByteMask(guid[1]);
                data.WriteByteMask(guid[6]);
                data.WriteByteSeq(guid[0]);
                data << uint32(0);
                data.WriteByteSeq(guid[6]);
                data.WriteByteSeq(guid[3]);
                data.WriteByteSeq(guid[5]);
                data.WriteByteSeq(guid[2]);
                data << float(GetSpeed(mtype));
                data.WriteByteSeq(guid[1]);
                data.WriteByteSeq(guid[7]);
                data.WriteByteSeq(guid[4]);
                break;
            case MOVE_SWIM_BACK:
                data.Initialize(SMSG_MOVE_SET_SWIM_BACK_SPEED, 1 + 8 + 4 + 4);
                data.WriteByteMask(guid[4]);
                data.WriteByteMask(guid[2]);
                data.WriteByteMask(guid[3]);
                data.WriteByteMask(guid[6]);
                data.WriteByteMask(guid[5]);
                data.WriteByteMask(guid[1]);
                data.WriteByteMask(guid[0]);
                data.WriteByteMask(guid[7]);
                data << uint32(0);
                data.WriteByteSeq(guid[0]);
                data.WriteByteSeq(guid[3]);
                data.WriteByteSeq(guid[4]);
                data.WriteByteSeq(guid[6]);
                data.WriteByteSeq(guid[5]);
                data.WriteByteSeq(guid[1]);
                data << float(GetSpeed(mtype));
                data.WriteByteSeq(guid[7]);
                data.WriteByteSeq(guid[2]);
                break;
            case MOVE_TURN_RATE:
                data.Initialize(SMSG_MOVE_SET_TURN_RATE, 1 + 8 + 4 + 4);
                data.WriteByteMask(guid[7]);
                data.WriteByteMask(guid[2]);
                data.WriteByteMask(guid[1]);
                data.WriteByteMask(guid[0]);
                data.WriteByteMask(guid[4]);
                data.WriteByteMask(guid[5]);
                data.WriteByteMask(guid[6]);
                data.WriteByteMask(guid[3]);
                data.WriteByteSeq(guid[5]);
                data.WriteByteSeq(guid[7]);
                data.WriteByteSeq(guid[2]);
                data << float(GetSpeed(mtype));
                data.WriteByteSeq(guid[3]);
                data.WriteByteSeq(guid[1]);
                data.WriteByteSeq(guid[0]);
                data << uint32(0);
                data.WriteByteSeq(guid[6]);
                data.WriteByteSeq(guid[4]);
                break;
            case MOVE_FLIGHT:
                data.Initialize(SMSG_MOVE_SET_FLIGHT_SPEED, 1 + 8 + 4 + 4);
                data.WriteByteMask(guid[0]);
                data.WriteByteMask(guid[5]);
                data.WriteByteMask(guid[1]);
                data.WriteByteMask(guid[6]);
                data.WriteByteMask(guid[3]);
                data.WriteByteMask(guid[2]);
                data.WriteByteMask(guid[7]);
                data.WriteByteMask(guid[4]);
                data.WriteByteSeq(guid[0]);
                data.WriteByteSeq(guid[1]);
                data.WriteByteSeq(guid[7]);
                data.WriteByteSeq(guid[5]);
                data << float(GetSpeed(mtype));
                data << uint32(0);
                data.WriteByteSeq(guid[2]);
                data.WriteByteSeq(guid[6]);
                data.WriteByteSeq(guid[3]);
                data.WriteByteSeq(guid[4]);
                break;
            case MOVE_FLIGHT_BACK:
                data.Initialize(SMSG_MOVE_SET_FLIGHT_BACK_SPEED, 1 + 8 + 4 + 4);
                data.WriteByteMask(guid[1]);
                data.WriteByteMask(guid[2]);
                data.WriteByteMask(guid[6]);
                data.WriteByteMask(guid[4]);
                data.WriteByteMask(guid[7]);
                data.WriteByteMask(guid[3]);
                data.WriteByteMask(guid[0]);
                data.WriteByteMask(guid[5]);
                data.WriteByteSeq(guid[3]);
                data << uint32(0);
                data.WriteByteSeq(guid[6]);
                data << float(GetSpeed(mtype));
                data.WriteByteSeq(guid[1]);
                data.WriteByteSeq(guid[2]);
                data.WriteByteSeq(guid[4]);
                data.WriteByteSeq(guid[0]);
                data.WriteByteSeq(guid[5]);
                data.WriteByteSeq(guid[7]);
                break;
            case MOVE_PITCH_RATE:
                data.Initialize(SMSG_MOVE_SET_PITCH_RATE, 1 + 8 + 4 + 4);
                data.WriteByteMask(guid[1]);
                data.WriteByteMask(guid[2]);
                data.WriteByteMask(guid[6]);
                data.WriteByteMask(guid[7]);
                data.WriteByteMask(guid[0]);
                data.WriteByteMask(guid[3]);
                data.WriteByteMask(guid[5]);
                data.WriteByteMask(guid[4]);
                data << float(GetSpeed(mtype));
                data.WriteByteSeq(guid[6]);
                data.WriteByteSeq(guid[4]);
                data.WriteByteSeq(guid[0]);
                data << uint32(0);
                data.WriteByteSeq(guid[1]);
                data.WriteByteSeq(guid[2]);
                data.WriteByteSeq(guid[7]);
                data.WriteByteSeq(guid[3]);
                data.WriteByteSeq(guid[5]);
                break;
            default:
                return;
        }
        SendMessageToSet(&data, true);
    }
}

void Unit::setDeathState(DeathState s)
{
    if (s != ALIVE && s != JUST_RESPAWNED)
    {
        CombatStop();
        DeleteThreatList();
        getHostileRefManager().deleteReferences();

        if (IsNonMeleeSpellCasted(false))
            InterruptNonMeleeSpells(false);

        UnsummonAllTotems();
        RemoveAllControlled();
        RemoveAllAurasOnDeath();
    }

    if (s == JUST_DIED)
    {
        ModifyAuraState(AURA_STATE_HEALTHLESS_20_PERCENT, false);
        ModifyAuraState(AURA_STATE_HEALTHLESS_35_PERCENT, false);
        // remove aurastates allowing special moves
        ClearAllReactives();
        ClearDiminishings();
        if (IsInWorld())
        {
            // Only clear MotionMaster for entities that exists in world
            // Avoids crashes in the following conditions :
            //  * Using 'call pet' on dead pets
            //  * Using 'call stabled pet'
            //  * Logging in with dead pets
            GetMotionMaster()->Clear(false);
            GetMotionMaster()->MoveIdle();
        }
        StopMoving();
        DisableSpline();
        // without this when removing IncreaseMaxHealth aura player may stuck with 1 hp
        // do not why since in IncreaseMaxHealth currenthealth is checked
        SetHealth(0);
        SetPower(getPowerType(), 0);

        // Warlock: Demonic Rebirth
        if (isPet())
        {
            if (Unit* owner = GetOwner())
            {
                if (!owner->HasAura(89140))
                {
                    if (AuraEffect const* aurEff = owner->GetAuraEffect(SPELL_AURA_DUMMY, SPELLFAMILY_WARLOCK, 1981, EFFECT_0))
                    {
                        int32 bp0 = -aurEff->GetAmount();
                        owner->CastCustomSpell(owner, 88448, &bp0, 0, 0, true);
                    }
                }
            }
        }
        // Druid: Fungal Growth
        switch (GetEntry())
        {
            case 1964:
            case 47649:
                if (Unit* owner = GetOwner())
                {
                    if (owner->GetTypeId() != TYPEID_PLAYER
                        || owner->ToPlayer()->HasSpellCooldown(81291)
                        || owner->ToPlayer()->HasSpellCooldown(81283))
                        break;

                    uint32 spellId = 0;
                    if (owner->HasAura(78788))
                        spellId = 81291;
                    else if (owner->HasAura(78789))
                        spellId = 81283;

                    if (spellId)
                        owner->ToPlayer()->AddSpellCooldown(spellId, 0, time(NULL) + sSpellMgr->GetSpellInfo(spellId)->RecoveryTime / 1000);
                }
                break;
        }

        // players in instance don't have ZoneScript, but they have InstanceScript
        if (ZoneScript* zoneScript = GetZoneScript() ? GetZoneScript() : (ZoneScript*)GetInstanceScript())
            zoneScript->OnUnitDeath(this);
    }
    else if (s == JUST_RESPAWNED)
        RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_SKINNABLE); // clear skinnable for creature and player (at battleground)

    m_deathState = s;
}

/*########################################
########                          ########
########       AGGRO SYSTEM       ########
########                          ########
########################################*/
bool Unit::CanHaveThreatList() const
{
    // only creatures can have threat list
    if (GetTypeId() != TYPEID_UNIT)
        return false;

    // only alive units can have threat list
    if (!isAlive() || isDying())
        return false;

    // totems can not have threat list
    if (ToCreature()->isTotem())
        return false;

    // vehicles can not have threat list
    //if (ToCreature()->IsVehicle())
    //    return false;

    if (ToCreature()->isBattlegroundVehicle())
        return false;

    // summons can not have a threat list, unless they are controlled by a creature
    if (HasUnitTypeMask(UNIT_MASK_MINION | UNIT_MASK_GUARDIAN | UNIT_MASK_CONTROLABLE_GUARDIAN) && IS_PLAYER_GUID(((Pet*)this)->GetOwnerGUID()))
        return false;

    return true;
}

//======================================================================

float Unit::ApplyTotalThreatModifier(float fThreat, SpellSchoolMask schoolMask)
{
    if (!HasAuraType(SPELL_AURA_MOD_THREAT) || fThreat < 0)
        return fThreat;

    SpellSchools school = GetFirstSchoolInMask(schoolMask);

    return fThreat * m_threatModifier[school];
}

//======================================================================

void Unit::AddThreat(Unit* victim, float fThreat, SpellSchoolMask schoolMask, SpellInfo const* threatSpell)
{
    // Only mobs can manage threat lists
    if (CanHaveThreatList())
        m_ThreatManager.addThreat(victim, fThreat, schoolMask, threatSpell);
}

//======================================================================

void Unit::DeleteThreatList()
{
    if (CanHaveThreatList() && !m_ThreatManager.isThreatListEmpty())
        SendClearThreatListOpcode();
    m_ThreatManager.clearReferences();
}

//======================================================================

void Unit::TauntApply(Unit* taunter)
{
    ASSERT(GetTypeId() == TYPEID_UNIT);

    if (!taunter || (taunter->GetTypeId() == TYPEID_PLAYER && taunter->ToPlayer()->isGameMaster()))
        return;

    if (!CanHaveThreatList())
        return;

    Creature* creature = ToCreature();

    if (creature->HasReactState(REACT_PASSIVE))
        return;

    Unit* target = getVictim();
    if (target && target == taunter)
        return;

    SetInFront(taunter);
    if (creature->IsAIEnabled)
        creature->AI()->AttackStart(taunter);

    //m_ThreatManager.tauntApply(taunter);
}

//======================================================================

void Unit::TauntFadeOut(Unit* taunter)
{
    ASSERT(GetTypeId() == TYPEID_UNIT);

    if (!taunter || (taunter->GetTypeId() == TYPEID_PLAYER && taunter->ToPlayer()->isGameMaster()))
        return;

    if (!CanHaveThreatList())
        return;

    Creature* creature = ToCreature();

    if (creature->HasReactState(REACT_PASSIVE))
        return;

    Unit* target = getVictim();
    if (!target || target != taunter)
        return;

    if (m_ThreatManager.isThreatListEmpty())
    {
        if (creature->IsAIEnabled)
            creature->AI()->EnterEvadeMode();
        return;
    }

    target = creature->SelectVictim();  // might have more taunt auras remaining

    if (target && target != taunter)
    {
        SetInFront(target);
        if (creature->IsAIEnabled)
            creature->AI()->AttackStart(target);
    }
}

//======================================================================

Unit* Creature::SelectVictim()
{
    // function provides main threat functionality
    // next-victim-selection algorithm and evade mode are called
    // threat list sorting etc.

    Unit* target = NULL;
    // First checking if we have some taunt on us
    AuraEffectList const& tauntAuras = GetAuraEffectsByType(SPELL_AURA_MOD_TAUNT);
    if (!tauntAuras.empty())
    {
        Unit* caster = tauntAuras.back()->GetCaster();

        // The last taunt aura caster is alive an we are happy to attack him
        if (caster && caster->isAlive())
            return getVictim();
        else if (tauntAuras.size() > 1)
        {
            // We do not have last taunt aura caster but we have more taunt auras,
            // so find first available target

            // Auras are pushed_back, last caster will be on the end
            AuraEffectList::const_iterator aura = --tauntAuras.end();
            do
            {
                --aura;
                caster = (*aura)->GetCaster();
                if (caster && canSeeOrDetect(caster, true) && IsValidAttackTarget(caster) && caster->isInAccessiblePlaceFor(ToCreature()))
                {
                    target = caster;
                    break;
                }
            } while (aura != tauntAuras.begin());
        }
        else
            target = getVictim();
    }

    if (CanHaveThreatList())
    {
        if (!target && !m_ThreatManager.isThreatListEmpty())
            // No taunt aura or taunt aura caster is dead standard target selection
            target = m_ThreatManager.getHostilTarget();
    }
    else if (!HasReactState(REACT_PASSIVE))
    {
        // We have player pet probably
        target = getAttackerForHelper();
        if (!target && isSummon())
        {
            if (Unit* owner = ToTempSummon()->GetOwner())
            {
                if (owner->isInCombat())
                    target = owner->getAttackerForHelper();
                if (!target)
                {
                    for (ControlList::const_iterator itr = owner->m_Controlled.begin(); itr != owner->m_Controlled.end(); ++itr)
                    {
                        if ((*itr)->isInCombat())
                        {
                            target = (*itr)->getAttackerForHelper();
                            if (target)
                                break;
                        }
                    }
                }
            }
        }
    }
    else
        return NULL;

    if (target && _IsTargetAcceptable(target) && canCreatureAttack(target))
    {
        SetInFront(target);
        return target;
    }

    // Case where mob is being kited.
    // Mob may not be in range to attack or may have dropped target. In any case,
    //  don't evade if damage received within the last 10 seconds
    // Does not apply to world bosses to prevent kiting to cities
    if (!isWorldBoss() && !GetInstanceId())
        if (time(NULL) - GetLastDamagedTime() <= MAX_AGGRO_RESET_TIME)
            return target;

    // last case when creature must not go to evade mode:
    // it in combat but attacker not make any damage and not enter to aggro radius to have record in threat list
    // for example at owner command to pet attack some far away creature
    // Note: creature does not have targeted movement generator but has attacker in this case
    for (AttackerSet::const_iterator itr = m_attackers.begin(); itr != m_attackers.end(); ++itr)
    {
        if ((*itr) && !canCreatureAttack(*itr) && (*itr)->GetTypeId() != TYPEID_PLAYER
        && !(*itr)->ToCreature()->HasUnitTypeMask(UNIT_MASK_CONTROLABLE_GUARDIAN))
            return NULL;
    }

    // TODO: a vehicle may eat some mob, so mob should not evade
    if (GetVehicle())
        return NULL;

    // search nearby enemy before enter evade mode
    if (HasReactState(REACT_AGGRESSIVE))
    {
        target = SelectNearestTargetInAttackDistance(m_CombatDistance ? m_CombatDistance : ATTACK_DISTANCE);

        if (target && _IsTargetAcceptable(target) && canCreatureAttack(target))
            return target;
    }

    Unit::AuraEffectList const& iAuras = GetAuraEffectsByType(SPELL_AURA_MOD_INVISIBILITY);
    if (!iAuras.empty())
    {
        for (Unit::AuraEffectList::const_iterator itr = iAuras.begin(); itr != iAuras.end(); ++itr)
        {
            if ((*itr)->GetBase()->IsPermanent())
            {
                AI()->EnterEvadeMode();
                break;
            }
        }
        return NULL;
    }

    // enter in evade mode in other case
    AI()->EnterEvadeMode();

    return NULL;
}

//======================================================================
//======================================================================
//======================================================================

float Unit::ApplyEffectModifiers(SpellInfo const* spellProto, uint8 effect_index, float value) const
{
    if (Player* modOwner = GetSpellModOwner())
    {
        modOwner->ApplySpellMod(spellProto->Id, SPELLMOD_ALL_EFFECTS, value);
        switch (effect_index)
        {
            case 0:
                modOwner->ApplySpellMod(spellProto->Id, SPELLMOD_EFFECT1, value);
                break;
            case 1:
                modOwner->ApplySpellMod(spellProto->Id, SPELLMOD_EFFECT2, value);
                break;
            case 2:
                modOwner->ApplySpellMod(spellProto->Id, SPELLMOD_EFFECT3, value);
                break;
        }
    }
    return value;
}

// function uses real base points (typically value - 1)
int32 Unit::CalculateSpellDamage(Unit const* target, SpellInfo const* spellProto, uint8 effect_index, int32 const* basePoints) const
{
    return spellProto->Effects[effect_index].CalcValue(this, basePoints, target);
}

int32 Unit::CalcSpellDuration(SpellInfo const* spellProto, Unit* target)
{
    uint8 comboPoints = m_movedPlayer ? m_movedPlayer->GetComboPoints() : 0;

    int32 minduration = spellProto->GetDuration();
    int32 maxduration = spellProto->GetMaxDuration();

    int32 duration;

    if (comboPoints && minduration != -1 && minduration != maxduration)
    {
        duration = minduration + int32((maxduration - minduration) * comboPoints / 5);
        if (target)
            if (AuraEffect* aurEff = target->GetAuraEffect(84617, 2, GetGUID()))
            {
                if (!(spellProto->SpellFamilyFlags[0] & 0x00100000))
                    duration *= (100.0f + aurEff->GetAmount()) / 100.0f;
            }
    }
    else
        duration = minduration;

    return duration;
}

int32 Unit::ModSpellDuration(SpellInfo const* spellProto, Unit const* target, int32 duration, bool positive, uint32 effectMask)
{
    // don't mod permanent auras duration
    if (duration < 0)
        return duration;

    // Channeled spells does not affected by modifer duration
    if (spellProto->HasAttribute(SPELL_ATTR1_CHANNELED_1))
        return duration;

    // cut duration only of negative effects
    if (!positive)
    {
        int32 mechanic = spellProto->GetSpellMechanicMaskByEffectMask(effectMask);

        int32 durationMod;
        int32 durationMod_always = 0;
        int32 durationMod_not_stack = 0;

        for (uint8 i = 1; i <= MECHANIC_ENRAGED; ++i)
        {
            if (!(mechanic & 1<<i))
                continue;
            // Find total mod value (negative bonus)
            int32 new_durationMod_always = target->GetTotalAuraModifierByMiscValue(SPELL_AURA_MECHANIC_DURATION_MOD, i);
            // Find max mod (negative bonus)
            int32 new_durationMod_not_stack = target->GetMaxNegativeAuraModifierByMiscValue(SPELL_AURA_MECHANIC_DURATION_MOD_NOT_STACK, i);
            // Check if mods applied before were weaker
            if (new_durationMod_always < durationMod_always)
                durationMod_always = new_durationMod_always;
            if (new_durationMod_not_stack < durationMod_not_stack)
                durationMod_not_stack = new_durationMod_not_stack;
        }

        // Select strongest negative mod
        if (durationMod_always > durationMod_not_stack)
            durationMod = durationMod_not_stack;
        else
            durationMod = durationMod_always;

        if (durationMod != 0)
            AddPct(duration, durationMod);

        // there are only negative mods currently
        durationMod_always = target->GetTotalAuraModifierByMiscValue(SPELL_AURA_MOD_AURA_DURATION_BY_DISPEL, spellProto->Dispel);
        durationMod_not_stack = target->GetMaxNegativeAuraModifierByMiscValue(SPELL_AURA_MOD_AURA_DURATION_BY_DISPEL_NOT_STACK, spellProto->Dispel);

        durationMod = 0;
        if (durationMod_always > durationMod_not_stack)
            durationMod += durationMod_not_stack;
        else
            durationMod += durationMod_always;

        // Glyph of Strangulate
        if (spellProto->SpellFamilyName == SPELLFAMILY_DEATHKNIGHT && spellProto->SpellFamilyFlags[0] & 0x00000200)
        {
            if (AuraEffect * aurEff = GetAuraEffect(58618, 0))
               if (target->IsNonMeleeSpellCasted(false, false, true, false, true))
                   duration += aurEff->GetAmount();
        }

        if (durationMod != 0)
            AddPct(duration, durationMod);
    }
    else
    {
        // else positive mods here, there are no currently
        // when there will be, change GetTotalAuraModifierByMiscValue to GetTotalPositiveAuraModifierByMiscValue

        // Mixology - duration boost
        if (target->GetTypeId() == TYPEID_PLAYER)
        {
            if (spellProto->SpellFamilyName == SPELLFAMILY_POTION && (
                sSpellMgr->IsSpellMemberOfSpellGroup(spellProto->Id, SPELL_GROUP_ELIXIR_BATTLE) ||
                sSpellMgr->IsSpellMemberOfSpellGroup(spellProto->Id, SPELL_GROUP_ELIXIR_GUARDIAN)))
            {
                if (target->HasAura(53042) && target->HasSpell(spellProto->Effects[0].TriggerSpell))
                    duration *= 2;
            }
        }

        // Item - Druid T13 Restoration 4P Bonus (Rejuvenation)
        if (AuraEffect * eff = GetAuraEffect(105770, 0))
        {
            if (roll_chance_i(eff->GetAmount()) && eff->IsAffectingSpell(spellProto))
                duration *= 2;
        } 
    }
    return std::max(duration, 0);
}

void Unit::ModSpellCastTime(SpellInfo const* spellProto, int32 & castTime, Spell* spell)
{
    if (!spellProto || castTime < 0)
        return;
    // called from caster
    if (Player* modOwner = GetSpellModOwner())
        modOwner->ApplySpellMod(spellProto->Id, SPELLMOD_CASTING_TIME, castTime, spell);

    if (!(spellProto->Attributes & (SPELL_ATTR0_ABILITY|SPELL_ATTR0_TRADESPELL)) && ((GetTypeId() == TYPEID_PLAYER && spellProto->SpellFamilyName) || GetTypeId() == TYPEID_UNIT))
        castTime = int32(float(castTime) * GetFloatValue(UNIT_MOD_CAST_SPEED));
    else if (spellProto->Attributes & SPELL_ATTR0_REQ_AMMO && !(spellProto->AttributesEx2 & SPELL_ATTR2_AUTOREPEAT_FLAG))
        castTime = int32(float(castTime) * m_modAttackSpeedPct[RANGED_ATTACK]);
    else 
    {
        spellProto->UpdateCastTimeForProfessionRacicalBoost(this, castTime);
    }
}

DiminishingLevels Unit::GetDiminishing(DiminishingGroup group)
{
    for (Diminishing::iterator i = m_Diminishing.begin(); i != m_Diminishing.end(); ++i)
    {
        if (i->DRGroup != group)
            continue;

        if (!i->hitCount)
            return DIMINISHING_LEVEL_1;

        if (!i->hitTime)
            return DIMINISHING_LEVEL_1;

        // If last spell was casted more than 15 seconds ago - reset the count.
        if (i->stack == 0 && getMSTimeDiff(i->hitTime, getMSTime()) > 15000)
        {
            i->hitCount = DIMINISHING_LEVEL_1;
            return DIMINISHING_LEVEL_1;
        }
        // or else increase the count.
        else
            return DiminishingLevels(i->hitCount);
    }
    return DIMINISHING_LEVEL_1;
}

void Unit::IncrDiminishing(DiminishingGroup group)
{
    // Checking for existing in the table
    for (Diminishing::iterator i = m_Diminishing.begin(); i != m_Diminishing.end(); ++i)
    {
        if (i->DRGroup != group)
            continue;
        if (int32(i->hitCount) < GetDiminishingReturnsMaxLevel(group))
            i->hitCount += 1;
        return;
    }
    m_Diminishing.push_back(DiminishingReturn(group, getMSTime(), DIMINISHING_LEVEL_2));
}

float Unit::ApplyDiminishingToDuration(DiminishingGroup group, int32 &duration, Unit* caster, DiminishingLevels Level, int32 limitduration)
{
    if (duration == -1 || group == DIMINISHING_NONE)
        return 1.0f;

    // test pet/charm masters instead pets/charmeds
    Unit const* targetOwner = GetCharmerOrOwner();
    Unit const* casterOwner = caster->GetCharmerOrOwner();

    // Duration of crowd control abilities on pvp target is limited by 10 sec. (2.2.0)
    if (limitduration > 0 && duration > limitduration)
    {
        Unit const* target = targetOwner ? targetOwner : this;
        Unit const* source = casterOwner ? casterOwner : caster;

        if ((target->GetTypeId() == TYPEID_PLAYER
            || ((Creature*)target)->GetCreatureTemplate()->flags_extra & CREATURE_FLAG_EXTRA_ALL_DIMINISH)
            && source->GetTypeId() == TYPEID_PLAYER)
            duration = limitduration;
    }

    float mod = 1.0f;

    if (group == DIMINISHING_TAUNT)
    {
        if (GetTypeId() == TYPEID_UNIT && (ToCreature()->GetCreatureTemplate()->flags_extra & CREATURE_FLAG_EXTRA_TAUNT_DIMINISH))
        {
            DiminishingLevels diminish = Level;
            switch (diminish)
            {
                case DIMINISHING_LEVEL_1: break;
                case DIMINISHING_LEVEL_2: mod = 0.65f; break;
                case DIMINISHING_LEVEL_3: mod = 0.4225f; break;
                case DIMINISHING_LEVEL_4: mod = 0.274625f; break;
                case DIMINISHING_LEVEL_TAUNT_IMMUNE: mod = 0.0f; break;
                default: break;
            }
        }
    }
    // Some diminishings applies to mobs too (for example, Stun)
     else if ((GetDiminishingReturnsGroupType(group) == DRTYPE_PLAYER
         && ((targetOwner ? (targetOwner->GetTypeId() == TYPEID_PLAYER) : (GetTypeId() == TYPEID_PLAYER))
         || (targetOwner ? (targetOwner->GetTypeId() == TYPEID_UNIT && ((Creature*)targetOwner)->GetCreatureTemplate()->flags_extra & CREATURE_FLAG_EXTRA_ALL_DIMINISH)
            : (GetTypeId() == TYPEID_UNIT && ToCreature()->GetCreatureTemplate()->flags_extra & CREATURE_FLAG_EXTRA_ALL_DIMINISH))))
         || GetDiminishingReturnsGroupType(group) == DRTYPE_ALL)
    {
        DiminishingLevels diminish = Level;
        switch (diminish)
        {
            case DIMINISHING_LEVEL_1: break;
            case DIMINISHING_LEVEL_2: mod = 0.5f; break;
            case DIMINISHING_LEVEL_3: mod = 0.25f; break;
            case DIMINISHING_LEVEL_IMMUNE: mod = 0.0f; break;
            default: break;
        }
    }

    duration = int32(duration * mod);
    return mod;
}

void Unit::ApplyDiminishingAura(DiminishingGroup group, bool apply)
{
    // Checking for existing in the table
    for (Diminishing::iterator i = m_Diminishing.begin(); i != m_Diminishing.end(); ++i)
    {
        if (i->DRGroup != group)
            continue;

        if (apply)
            i->stack += 1;
        else if (i->stack)
        {
            i->stack -= 1;
            // Remember time after last aura from group removed
            if (i->stack == 0)
                i->hitTime = getMSTime();
        }
        break;
    }
}

float Unit::GetSpellMaxRangeForTarget(Unit const* target, SpellInfo const* spellInfo) const
{
    if (!spellInfo->RangeEntry)
        return 0;
    if (spellInfo->RangeEntry->maxRangeFriend == spellInfo->RangeEntry->maxRangeHostile)
        return spellInfo->GetMaxRange();
    if (!target)
        return spellInfo->GetMaxRange();
    return spellInfo->GetMaxRange(!IsHostileTo(target));
}

float Unit::GetSpellMinRangeForTarget(Unit const* target, SpellInfo const* spellInfo) const
{
    if (!spellInfo->RangeEntry)
        return 0;
    if (spellInfo->RangeEntry->minRangeFriend == spellInfo->RangeEntry->minRangeHostile)
        return spellInfo->GetMinRange();
    return spellInfo->GetMinRange(!IsHostileTo(target));
}

Unit* Unit::GetUnit(WorldObject& object, uint64 guid)
{
    return ObjectAccessor::GetUnit(object, guid);
}

Player* Unit::GetPlayer(WorldObject& object, uint64 guid)
{
    return ObjectAccessor::GetPlayer(object, guid);
}

Creature* Unit::GetCreature(WorldObject& object, uint64 guid)
{
    return object.GetMap()->GetCreature(guid);
}

uint32 Unit::GetCreatureType() const
{
    if (GetTypeId() == TYPEID_PLAYER)
    {
        ShapeshiftForm form = GetShapeshiftForm();
        SpellShapeshiftFormEntry const* ssEntry = sSpellShapeshiftFormStore.LookupEntry(form);
        if (ssEntry && ssEntry->creatureType > 0)
            return ssEntry->creatureType;
        else
            return CREATURE_TYPE_HUMANOID;
    }
    else
        return ToCreature()->GetCreatureTemplate()->type;
}

/*#######################################
########                         ########
########       STAT SYSTEM       ########
########                         ########
#######################################*/

bool Unit::HandleStatModifier(UnitMods unitMod, UnitModifierType modifierType, float amount, bool apply)
{
    if (unitMod >= UNIT_MOD_END || modifierType >= MODIFIER_TYPE_END)
        return false;

    switch (modifierType)
    {
        case BASE_VALUE:
        case TOTAL_VALUE:
            m_auraModifiersGroup[unitMod][modifierType] += apply ? amount : -amount;
            break;
        case BASE_PCT:
        case TOTAL_PCT:
            ApplyPercentModFloatVar(m_auraModifiersGroup[unitMod][modifierType], amount, apply);
            break;
        default:
            break;
    }

    if (!CanModifyStats())
        return false;

    switch (unitMod)
    {
        case UNIT_MOD_STAT_STRENGTH:
        case UNIT_MOD_STAT_AGILITY:
        case UNIT_MOD_STAT_STAMINA:
        case UNIT_MOD_STAT_INTELLECT:
        case UNIT_MOD_STAT_SPIRIT:         UpdateStats(GetStatByAuraGroup(unitMod));  break;

        case UNIT_MOD_ARMOR:               UpdateArmor();           break;
        case UNIT_MOD_HEALTH:              UpdateMaxHealth();       break;

        case UNIT_MOD_MANA:
        case UNIT_MOD_RAGE:
        case UNIT_MOD_FOCUS:
        case UNIT_MOD_ENERGY:
        case UNIT_MOD_RUNE:
        case UNIT_MOD_RUNIC_POWER:
        case UNIT_MOD_ALTERNATE_POWER      :UpdateMaxPower(GetPowerTypeByAuraGroup(unitMod));          break;

        case UNIT_MOD_RESISTANCE_HOLY:
        case UNIT_MOD_RESISTANCE_FIRE:
        case UNIT_MOD_RESISTANCE_NATURE:
        case UNIT_MOD_RESISTANCE_FROST:
        case UNIT_MOD_RESISTANCE_SHADOW:
        case UNIT_MOD_RESISTANCE_ARCANE:   UpdateResistances(GetSpellSchoolByAuraGroup(unitMod));      break;

        case UNIT_MOD_ATTACK_POWER:        UpdateAttackPowerAndDamage();         break;
        case UNIT_MOD_ATTACK_POWER_RANGED: UpdateAttackPowerAndDamage(true);     break;

        case UNIT_MOD_DAMAGE_MAINHAND:     UpdateDamagePhysical(BASE_ATTACK);    break;
        case UNIT_MOD_DAMAGE_OFFHAND:      UpdateDamagePhysical(OFF_ATTACK);     break;
        case UNIT_MOD_DAMAGE_RANGED:       UpdateDamagePhysical(RANGED_ATTACK);  break;

        default:
            break;
    }

    return true;
}

float Unit::GetModifierValue(UnitMods unitMod, UnitModifierType modifierType) const
{
    if (unitMod >= UNIT_MOD_END || modifierType >= MODIFIER_TYPE_END)
        return 0.0f;

    if (modifierType == TOTAL_PCT && m_auraModifiersGroup[unitMod][modifierType] <= 0.0f)
        return 0.0f;

    return m_auraModifiersGroup[unitMod][modifierType];
}

float Unit::GetTotalStatValue(Stats stat) const
{
    UnitMods unitMod = UnitMods(UNIT_MOD_STAT_START + stat);

    if (m_auraModifiersGroup[unitMod][TOTAL_PCT] <= 0.0f)
        return 0.0f;

    // value = ((base_value * base_pct) + total_value) * total_pct
    float value  = m_auraModifiersGroup[unitMod][BASE_VALUE] + GetCreateStat(stat);
    value *= m_auraModifiersGroup[unitMod][BASE_PCT];
    value += m_auraModifiersGroup[unitMod][TOTAL_VALUE];
    value *= m_auraModifiersGroup[unitMod][TOTAL_PCT];

    return value;
}

float Unit::GetTotalAuraModValue(UnitMods unitMod) const
{
    if (unitMod >= UNIT_MOD_END)
        return 0.0f;

    if (m_auraModifiersGroup[unitMod][TOTAL_PCT] <= 0.0f)
        return 0.0f;

    float value = m_auraModifiersGroup[unitMod][BASE_VALUE];
    value *= m_auraModifiersGroup[unitMod][BASE_PCT];
    value += m_auraModifiersGroup[unitMod][TOTAL_VALUE];
    value *= m_auraModifiersGroup[unitMod][TOTAL_PCT];

    return value;
}

SpellSchools Unit::GetSpellSchoolByAuraGroup(UnitMods unitMod) const
{
    SpellSchools school = SPELL_SCHOOL_NORMAL;

    switch (unitMod)
    {
        case UNIT_MOD_RESISTANCE_HOLY:     school = SPELL_SCHOOL_HOLY;          break;
        case UNIT_MOD_RESISTANCE_FIRE:     school = SPELL_SCHOOL_FIRE;          break;
        case UNIT_MOD_RESISTANCE_NATURE:   school = SPELL_SCHOOL_NATURE;        break;
        case UNIT_MOD_RESISTANCE_FROST:    school = SPELL_SCHOOL_FROST;         break;
        case UNIT_MOD_RESISTANCE_SHADOW:   school = SPELL_SCHOOL_SHADOW;        break;
        case UNIT_MOD_RESISTANCE_ARCANE:   school = SPELL_SCHOOL_ARCANE;        break;

        default:
            break;
    }

    return school;
}

Stats Unit::GetStatByAuraGroup(UnitMods unitMod) const
{
    Stats stat = STAT_STRENGTH;

    switch (unitMod)
    {
        case UNIT_MOD_STAT_STRENGTH:    stat = STAT_STRENGTH;      break;
        case UNIT_MOD_STAT_AGILITY:     stat = STAT_AGILITY;       break;
        case UNIT_MOD_STAT_STAMINA:     stat = STAT_STAMINA;       break;
        case UNIT_MOD_STAT_INTELLECT:   stat = STAT_INTELLECT;     break;
        case UNIT_MOD_STAT_SPIRIT:      stat = STAT_SPIRIT;        break;

        default:
            break;
    }

    return stat;
}

Powers Unit::GetPowerTypeByAuraGroup(UnitMods unitMod) const
{
    switch (unitMod)
    {
        case UNIT_MOD_RAGE:        return POWER_RAGE;
        case UNIT_MOD_FOCUS:       return POWER_FOCUS;
        case UNIT_MOD_ENERGY:      return POWER_ENERGY;
        case UNIT_MOD_RUNE:        return POWER_RUNES;
        case UNIT_MOD_RUNIC_POWER: return POWER_RUNIC_POWER;
        default:
        case UNIT_MOD_MANA:        return POWER_MANA;
    }
}

float Unit::GetTotalAttackPowerValue(WeaponAttackType attType) const
{
    if (attType == RANGED_ATTACK)
    {
        int32 ap = GetInt32Value(UNIT_FIELD_RANGED_ATTACK_POWER);
        if (ap < 0)
            return 0.0f;
        return ap * (1.0f + GetFloatValue(UNIT_FIELD_RANGED_ATTACK_POWER_MULTIPLIER));
    }
    else
    {
        int32 ap = GetInt32Value(UNIT_FIELD_ATTACK_POWER);
        if (ap < 0)
            return 0.0f;
        return ap * (1.0f + GetFloatValue(UNIT_FIELD_ATTACK_POWER_MULTIPLIER));
    }
}

float Unit::GetWeaponDamageRange(WeaponAttackType attType, WeaponDamageRange type) const
{
    if (attType == OFF_ATTACK && !haveOffhandWeapon())
        return 0.0f;

    return m_weaponDamage[attType][type];
}

void Unit::SetLevel(uint8 lvl)
{
    SetUInt32Value(UNIT_FIELD_LEVEL, lvl);

    // group update
    if (GetTypeId() == TYPEID_PLAYER && ToPlayer()->GetGroup())
        ToPlayer()->SetGroupUpdateFlag(GROUP_UPDATE_FLAG_LEVEL);

    if (GetTypeId() == TYPEID_PLAYER)
        sWorld->UpdateCharacterNameDataLevel(ToPlayer()->GetGUIDLow(), lvl);
}

void Unit::SetHealth(uint32 val)
{
    if (getDeathState() == JUST_DIED)
        val = 0;
    else if (GetTypeId() == TYPEID_PLAYER && getDeathState() == DEAD)
        val = 1;
    else
    {
        uint32 maxHealth = GetMaxHealth();
        if (maxHealth < val)
            val = maxHealth;
    }

    SetUInt32Value(UNIT_FIELD_HEALTH, val);

    // group update
    if (Player* player = ToPlayer())
    {
        if (player->GetGroup())
            player->SetGroupUpdateFlag(GROUP_UPDATE_FLAG_CUR_HP);
    }
    else if (Pet* pet = ToCreature()->ToPet())
    {
        if (pet->isControlled())
        {
            Unit* owner = GetOwner();
            if (owner && (owner->GetTypeId() == TYPEID_PLAYER) && owner->ToPlayer()->GetGroup())
                owner->ToPlayer()->SetGroupUpdateFlag(GROUP_UPDATE_FLAG_PET_CUR_HP);
        }
    }
}

void Unit::SetMaxHealth(uint32 val)
{
    if (!val)
        val = 1;

    uint32 health = GetHealth();
    SetUInt32Value(UNIT_FIELD_MAXHEALTH, val);

    // group update
    if (GetTypeId() == TYPEID_PLAYER)
    {
        if (ToPlayer()->GetGroup())
            ToPlayer()->SetGroupUpdateFlag(GROUP_UPDATE_FLAG_MAX_HP);
    }
    else if (Pet* pet = ToCreature()->ToPet())
    {
        if (pet->isControlled())
        {
            Unit* owner = GetOwner();
            if (owner && (owner->GetTypeId() == TYPEID_PLAYER) && owner->ToPlayer()->GetGroup())
                owner->ToPlayer()->SetGroupUpdateFlag(GROUP_UPDATE_FLAG_PET_MAX_HP);
        }
    }

    if (val < health)
        SetHealth(val);
}

int32 Unit::GetPower(Powers power) const
{
    uint32 powerIndex = GetPowerIndexByClass(power, getClass());
    if (powerIndex == MAX_POWERS)
        return 0;

    return GetUInt32Value(UNIT_FIELD_POWER1 + powerIndex);
}

int32 Unit::GetMaxPower(Powers power) const
{
    uint32 powerIndex = GetPowerIndexByClass(power, getClass());
    if (powerIndex == MAX_POWERS)
        return 0;

    return GetInt32Value(UNIT_FIELD_MAXPOWER1 + powerIndex);
}

void Unit::SetPower(Powers power, int32 val)
{
    uint32 powerIndex = GetPowerIndexByClass(power, getClass());
    if (powerIndex == MAX_POWERS)
        return;

    int32 maxPower = int32(GetMaxPower(power));
    if (maxPower < val)
        val = maxPower;

    SetInt32Value(UNIT_FIELD_POWER1 + powerIndex, val);

    if (IsInWorld())
    {
        WorldPacket data(SMSG_POWER_UPDATE, 8 + 4 + 1 + 4);
        data.append(GetPackGUID());
        data << uint32(1); //power count
        data << uint8((power == POWER_ALTERNATE_POWER ? POWER_ALTERNATE_POWER : powerIndex));
        data << int32(val);
        SendMessageToSet(&data, GetTypeId() == TYPEID_PLAYER ? true : false);
    }

    // group update
    if (Player* player = ToPlayer())
    {
        if (player->GetGroup())
            player->SetGroupUpdateFlag(GROUP_UPDATE_FLAG_CUR_POWER);
    }
    else if (Pet* pet = ToCreature()->ToPet())
    {
        if (pet->isControlled())
        {
            Unit* owner = GetOwner();
            if (owner && (owner->GetTypeId() == TYPEID_PLAYER) && owner->ToPlayer()->GetGroup())
                owner->ToPlayer()->SetGroupUpdateFlag(GROUP_UPDATE_FLAG_PET_CUR_POWER);
        }
    }
}

void Unit::SetMaxPower(Powers power, int32 val)
{
    uint32 powerIndex = GetPowerIndexByClass(power, getClass());
    if (powerIndex == MAX_POWERS)
        return;

    int32 cur_power = GetPower(power);
    SetInt32Value(UNIT_FIELD_MAXPOWER1 + powerIndex, val);

    // group update
    if (GetTypeId() == TYPEID_PLAYER)
    {
        if (ToPlayer()->GetGroup())
            ToPlayer()->SetGroupUpdateFlag(GROUP_UPDATE_FLAG_MAX_POWER);
    }
    else if (Pet* pet = ToCreature()->ToPet())
    {
        if (pet->isControlled())
        {
            Unit* owner = GetOwner();
            if (owner && (owner->GetTypeId() == TYPEID_PLAYER) && owner->ToPlayer()->GetGroup())
                owner->ToPlayer()->SetGroupUpdateFlag(GROUP_UPDATE_FLAG_PET_MAX_POWER);
        }
    }

    if (val < cur_power)
        SetPower(power, val);
}

int32 Unit::GetCreatePowers(Powers power) const
{
    switch (power)
    {
        case POWER_MANA:
            return GetCreateMana();
        case POWER_RAGE:
           return 1000;
      case POWER_FOCUS:
           if (GetTypeId() == TYPEID_PLAYER && getClass() == CLASS_HUNTER)
               return 100;
            return (GetTypeId() == TYPEID_PLAYER || !((Creature const*)this)->isPet() || ((Pet const*)this)->getPetType() != HUNTER_PET ? 0 : 100);
       case POWER_ENERGY:
            return 100;
        case POWER_RUNIC_POWER:
            return 1000;
        case POWER_RUNES:
            return 0;
        case POWER_SOUL_SHARDS:
           return 3;
        case POWER_ECLIPSE:
            return (GetTypeId() == TYPEID_PLAYER && ((Player const*)this)->getClass() == CLASS_DRUID ? 100 : 0);
        case POWER_HOLY_POWER:
            return (GetTypeId() == TYPEID_PLAYER && ((Player const*)this)->getClass() == CLASS_PALADIN ? 3 : 0);
        case POWER_HEALTH:
            return 0;
        default:
            break;
    }

    return 0;
}

void Unit::AddToWorld()
{
    if (!IsInWorld())
    {
        WorldObject::AddToWorld();
    }
}

void Unit::RemoveFromWorld()
{
    // cleanup
    ASSERT(GetGUID());

    if (IsInWorld())
    {
        m_duringRemoveFromWorld = true;
        if (IsVehicle())
            RemoveVehicleKit();

        RemoveCharmAuras();
        RemoveBindSightAuras();
        RemoveNotOwnSingleTargetAuras();

        RemoveAllGameObjects();
        RemoveAllDynObjects();

        ExitVehicle();  // Remove applied auras with SPELL_AURA_CONTROL_VEHICLE
        UnsummonAllTotems();
        RemoveAllControlled();

        RemoveAreaAurasDueToLeaveWorld();

        if (GetCharmerGUID())
            ASSERT(false);

        if (Unit* owner = GetOwner())
        {
            if (owner->m_Controlled.find(this) != owner->m_Controlled.end())
                ASSERT(false);
        }

        WorldObject::RemoveFromWorld();
        m_duringRemoveFromWorld = false;
    }
}

void Unit::CleanupBeforeRemoveFromMap(bool finalCleanup)
{
    // This needs to be before RemoveFromWorld to make GetCaster() return a valid pointer on aura removal
    InterruptNonMeleeSpells(true);

    if (IsInWorld())
        RemoveFromWorld();

    ASSERT(GetGUID());

    // A unit may be in removelist and not in world, but it is still in grid
    // and may have some references during delete
    RemoveAllAuras();
    RemoveAllGameObjects();

    if (finalCleanup)
        m_cleanupDone = true;

    m_Events.KillAllEvents(false);                      // non-delatable (currently casted spells) will not deleted now but it will deleted at call in Map::RemoveAllObjectsInRemoveList
    CombatStop();
    ClearComboPointHolders();
    DeleteThreatList();
    getHostileRefManager().setOnlineOfflineState(false);
    GetMotionMaster()->Clear(false);                    // remove different non-standard movement generators.
}

void Unit::CleanupsBeforeDelete(bool finalCleanup)
{
    CleanupBeforeRemoveFromMap(finalCleanup);

    if (Creature* thisCreature = ToCreature())
        if (GetTransport())
            GetTransport()->RemovePassenger(thisCreature);
}

void Unit::UpdateCharmAI()
{
    if (GetTypeId() == TYPEID_PLAYER)
        return;

    if (i_disabledAI) // disabled AI must be primary AI
    {
        if (!isCharmed())
        {
            delete i_AI;
            i_AI = i_disabledAI;
            i_disabledAI = NULL;
        }
    }
    else
    {
        if (isCharmed())
        {
            i_disabledAI = i_AI;
            if (isPossessed() || IsVehicle())
                i_AI = new PossessedAI(ToCreature());
            else
                i_AI = new PetAI(ToCreature());
        }
    }
}

void Unit::EnableAI()
{
    if (GetTypeId() == TYPEID_PLAYER)
        return;

    ToCreature()->NeedChangeAI = false;
    ToCreature()->IsAIEnabled = true;

    if (i_disabledAI)
    {
        delete i_AI;
        i_AI = i_disabledAI;
        i_disabledAI = NULL;
    }
}

CharmInfo* Unit::InitCharmInfo()
{
    if (!m_charmInfo)
        m_charmInfo = new CharmInfo(this);

    return m_charmInfo;
}

void Unit::DeleteCharmInfo()
{
    if (!m_charmInfo)
        return;

    m_charmInfo->RestoreState();
    delete m_charmInfo;
    m_charmInfo = NULL;
}

CharmInfo::CharmInfo(Unit* unit)
: _unit(unit), _CommandState(COMMAND_FOLLOW), _petnumber(0), _barInit(false),
  _isCommandAttack(false), _isAtStay(false), _isFollowing(false), _isReturning(false),
  _stayX(0.0f), _stayY(0.0f), _stayZ(0.0f)
{
    for (uint8 i = 0; i < MAX_SPELL_CHARM; ++i)
        _charmspells[i].SetActionAndType(0, ACT_DISABLED);

    if (_unit->GetTypeId() == TYPEID_UNIT)
    {
        _oldReactState = _unit->ToCreature()->GetReactState();
        _unit->ToCreature()->SetReactState(REACT_PASSIVE);
    }
}

CharmInfo::~CharmInfo()
{
}

void CharmInfo::RestoreState()
{
    if (_unit->GetTypeId() == TYPEID_UNIT)
        if (Creature* creature = _unit->ToCreature())
            creature->SetReactState(_oldReactState);
}

void CharmInfo::InitPetActionBar()
{
    // the first 3 SpellOrActions are attack, follow and stay
    for (uint32 i = 0; i < ACTION_BAR_INDEX_PET_SPELL_START - ACTION_BAR_INDEX_START; ++i)
        SetActionBar(ACTION_BAR_INDEX_START + i, COMMAND_ATTACK - i, ACT_COMMAND);

    // middle 4 SpellOrActions are spells/special attacks/abilities
    for (uint32 i = 0; i < ACTION_BAR_INDEX_PET_SPELL_END-ACTION_BAR_INDEX_PET_SPELL_START; ++i)
        SetActionBar(ACTION_BAR_INDEX_PET_SPELL_START + i, 0, ACT_PASSIVE);

    // last 3 SpellOrActions are reactions
    for (uint32 i = 0; i < ACTION_BAR_INDEX_END - ACTION_BAR_INDEX_PET_SPELL_END; ++i)
        // Hack, aggressive react state must be replayced by assist react state (4.2+ changes)
        SetActionBar(ACTION_BAR_INDEX_PET_SPELL_END + i, (i == 2)? 3: COMMAND_ATTACK - i, ACT_REACTION);
}

void CharmInfo::InitEmptyActionBar(bool withAttack)
{
    if (withAttack)
        SetActionBar(ACTION_BAR_INDEX_START, COMMAND_ATTACK, ACT_COMMAND);
    else
        SetActionBar(ACTION_BAR_INDEX_START, 0, ACT_PASSIVE);
    for (uint32 x = ACTION_BAR_INDEX_START+1; x < ACTION_BAR_INDEX_END; ++x)
        SetActionBar(x, 0, ACT_PASSIVE);
}

void CharmInfo::InitPossessCreateSpells()
{
    InitEmptyActionBar();
    if (_unit->GetTypeId() == TYPEID_UNIT)
    {
        for (uint32 i = 0; i < CREATURE_MAX_SPELLS; ++i)
        {
            uint32 spellId = _unit->ToCreature()->m_spells[i];
            SpellInfo const* spellInfo = sSpellMgr->GetSpellInfo(spellId);
            if (spellInfo && !(spellInfo->Attributes & SPELL_ATTR0_CASTABLE_WHILE_DEAD))
            {
                if (spellInfo->IsPassive())
                    _unit->CastSpell(_unit, spellInfo, true);
                else
                    AddSpellToActionBar(spellInfo, ACT_PASSIVE);
            }
        }
    }
}

void CharmInfo::InitCharmCreateSpells()
{
    if (_unit->GetTypeId() == TYPEID_PLAYER)                // charmed players don't have spells
    {
        InitEmptyActionBar();
        return;
    }

    InitPetActionBar();

    for (uint32 x = 0; x < MAX_SPELL_CHARM; ++x)
    {
        uint32 spellId = _unit->ToCreature()->m_spells[x];
        SpellInfo const* spellInfo = sSpellMgr->GetSpellInfo(spellId);

        if (!spellInfo || spellInfo->Attributes & SPELL_ATTR0_CASTABLE_WHILE_DEAD)
        {
            _charmspells[x].SetActionAndType(spellId, ACT_DISABLED);
            continue;
        }

        if (spellInfo->IsPassive())
        {
            _unit->CastSpell(_unit, spellInfo, true);
            _charmspells[x].SetActionAndType(spellId, ACT_PASSIVE);
        }
        else
        {
            _charmspells[x].SetActionAndType(spellId, ACT_DISABLED);

            ActiveStates newstate = ACT_PASSIVE;

            if (!spellInfo->IsAutocastable())
                newstate = ACT_PASSIVE;
            else
            {
                if (spellInfo->NeedsExplicitUnitTarget())
                {
                    newstate = ACT_ENABLED;
                    ToggleCreatureAutocast(spellInfo, true);
                }
                else
                    newstate = ACT_DISABLED;
            }

            AddSpellToActionBar(spellInfo, newstate);
        }
    }
}

bool CharmInfo::AddSpellToActionBar(SpellInfo const* spellInfo, ActiveStates newstate)
{
    uint32 spell_id = spellInfo->Id;
    uint32 first_id = spellInfo->GetFirstRankSpell()->Id;

    // new spell rank can be already listed
    for (uint8 i = 0; i < MAX_UNIT_ACTION_BAR_INDEX; ++i)
    {
        if (uint32 action = PetActionBar[i].GetAction())
        {
            if (PetActionBar[i].IsActionBarForSpell() && sSpellMgr->GetFirstSpellInChain(action) == first_id)
            {
                PetActionBar[i].SetAction(spell_id);
                return true;
            }
        }
    }

    // or use empty slot in other case
    for (uint8 i = 0; i < MAX_UNIT_ACTION_BAR_INDEX; ++i)
    {
        if (!PetActionBar[i].GetAction() && PetActionBar[i].IsActionBarForSpell())
        {
            SetActionBar(i, spell_id, newstate == ACT_DECIDE ? spellInfo->IsAutocastable() ? ACT_DISABLED : ACT_PASSIVE : newstate);
            return true;
        }
    }
    return false;
}

bool CharmInfo::RemoveSpellFromActionBar(uint32 spell_id)
{
    uint32 first_id = sSpellMgr->GetFirstSpellInChain(spell_id);

    for (uint8 i = 0; i < MAX_UNIT_ACTION_BAR_INDEX; ++i)
    {
        if (uint32 action = PetActionBar[i].GetAction())
        {
            if (PetActionBar[i].IsActionBarForSpell() && sSpellMgr->GetFirstSpellInChain(action) == first_id)
            {
                SetActionBar(i, 0, ACT_PASSIVE);
                return true;
            }
        }
    }

    return false;
}

void CharmInfo::ToggleCreatureAutocast(SpellInfo const* spellInfo, bool apply)
{
    if (spellInfo->IsPassive())
        return;

    for (uint32 x = 0; x < MAX_SPELL_CHARM; ++x)
        if (spellInfo->Id == _charmspells[x].GetAction())
            _charmspells[x].SetType(apply ? ACT_ENABLED : ACT_DISABLED);
}

void CharmInfo::SetPetNumber(uint32 petnumber, bool statwindow)
{
    _petnumber = petnumber;
    if (statwindow)
        _unit->SetUInt32Value(UNIT_FIELD_PETNUMBER, _petnumber);
    else
        _unit->SetUInt32Value(UNIT_FIELD_PETNUMBER, 0);
}

void CharmInfo::LoadPetActionBar(const std::string& data)
{
    InitPetActionBar();

    Tokenizer tokens(data, ' ');

    if (tokens.size() != (ACTION_BAR_INDEX_END-ACTION_BAR_INDEX_START) * 2)
        return;                                             // non critical, will reset to default

    uint8 index = ACTION_BAR_INDEX_START;
    Tokenizer::const_iterator iter = tokens.begin();
    for (; index < ACTION_BAR_INDEX_END; ++iter, ++index)
    {
        // use unsigned cast to avoid sign negative format use at long-> ActiveStates (int) conversion
        ActiveStates type  = ActiveStates(atol(*iter));
        ++iter;
        uint32 action = uint32(atol(*iter));

        // Hack, aggressive react state must be replayced by assist react state (4.2+ changes)
        if (type == ACT_REACTION && action == 2)
            action = 3;

        PetActionBar[index].SetActionAndType(action, type);

        // check correctness
        if (PetActionBar[index].IsActionBarForSpell())
        {
            SpellInfo const* spelInfo = sSpellMgr->GetSpellInfo(PetActionBar[index].GetAction());
            if (!spelInfo)
                SetActionBar(index, 0, ACT_PASSIVE);
            else if (!spelInfo->IsAutocastable())
                SetActionBar(index, PetActionBar[index].GetAction(), ACT_PASSIVE);
        }
    }
}

void CharmInfo::BuildActionBar(WorldPacket* data)
{
    for (uint32 i = 0; i < MAX_UNIT_ACTION_BAR_INDEX; ++i)
        *data << uint32(PetActionBar[i].packedData);
}

void CharmInfo::SetSpellAutocast(SpellInfo const* spellInfo, bool state)
{
    for (uint8 i = 0; i < MAX_UNIT_ACTION_BAR_INDEX; ++i)
    {
        if (spellInfo->Id == PetActionBar[i].GetAction() && PetActionBar[i].IsActionBarForSpell())
        {
            PetActionBar[i].SetType(state ? ACT_ENABLED : ACT_DISABLED);
            break;
        }
    }
}

bool Unit::isFrozen() const
{
    return HasAuraState(AURA_STATE_FROZEN);
}

struct ProcTriggeredData
{
    ProcTriggeredData(Aura* _aura)
        : aura(_aura)
    {
        effMask = 0;
        spellProcEvent = NULL;
    }
    SpellProcEventEntry const* spellProcEvent;
    Aura* aura;
    uint32 effMask;
};

typedef std::list< ProcTriggeredData > ProcTriggeredList;

// List of auras that CAN be trigger but may not exist in spell_proc_event
// in most case need for drop charges
// in some types of aura need do additional check
// for example SPELL_AURA_MECHANIC_IMMUNITY - need check for mechanic
bool InitTriggerAuraData()
{
    for (uint16 i = 0; i < TOTAL_AURAS; ++i)
    {
        isTriggerAura[i] = false;
        isNonTriggerAura[i] = false;
        isAlwaysTriggeredAura[i] = false;
    }
    isTriggerAura[SPELL_AURA_PROC_ON_POWER_AMOUNT] = true;
    isTriggerAura[SPELL_AURA_DUMMY] = true;
    isTriggerAura[SPELL_AURA_MOD_CONFUSE] = true;
    isTriggerAura[SPELL_AURA_MOD_THREAT] = true;
    isTriggerAura[SPELL_AURA_MOD_STUN] = true; // Aura does not have charges but needs to be removed on trigger
    isTriggerAura[SPELL_AURA_MOD_DAMAGE_DONE] = true;
    isTriggerAura[SPELL_AURA_MOD_DAMAGE_TAKEN] = true;
    isTriggerAura[SPELL_AURA_MOD_RESISTANCE] = true;
    isTriggerAura[SPELL_AURA_MOD_STEALTH] = true;
    isTriggerAura[SPELL_AURA_MOD_FEAR] = true; // Aura does not have charges but needs to be removed on trigger
    isTriggerAura[SPELL_AURA_MOD_ROOT] = true;
    isTriggerAura[SPELL_AURA_TRANSFORM] = true;
    isTriggerAura[SPELL_AURA_REFLECT_SPELLS] = true;
    isTriggerAura[SPELL_AURA_DAMAGE_IMMUNITY] = true;
    isTriggerAura[SPELL_AURA_PROC_TRIGGER_SPELL] = true;
    isTriggerAura[SPELL_AURA_PROC_TRIGGER_DAMAGE] = true;
    isTriggerAura[SPELL_AURA_MOD_CASTING_SPEED_NOT_STACK] = true;
    isTriggerAura[SPELL_AURA_SCHOOL_ABSORB] = true; // Savage Defense untested
    isTriggerAura[SPELL_AURA_MOD_POWER_COST_SCHOOL_PCT] = true;
    isTriggerAura[SPELL_AURA_MOD_POWER_COST_SCHOOL] = true;
    isTriggerAura[SPELL_AURA_REFLECT_SPELLS_SCHOOL] = true;
    isTriggerAura[SPELL_AURA_MECHANIC_IMMUNITY] = true;
    isTriggerAura[SPELL_AURA_MOD_DAMAGE_PERCENT_TAKEN] = true;
    isTriggerAura[SPELL_AURA_SPELL_MAGNET] = true;
    isTriggerAura[SPELL_AURA_MOD_ATTACK_POWER] = true;
    isTriggerAura[SPELL_AURA_ADD_CASTER_HIT_TRIGGER] = true;
    isTriggerAura[SPELL_AURA_OVERRIDE_CLASS_SCRIPTS] = true;
    isTriggerAura[SPELL_AURA_MOD_MECHANIC_RESISTANCE] = true;
    isTriggerAura[SPELL_AURA_RANGED_ATTACK_POWER_ATTACKER_BONUS] = true;
    isTriggerAura[SPELL_AURA_MOD_MELEE_HASTE] = true;
    isTriggerAura[SPELL_AURA_MOD_MELEE_HASTE_3] = true;
    isTriggerAura[SPELL_AURA_MOD_ATTACKER_MELEE_HIT_CHANCE] = true;
    isTriggerAura[SPELL_AURA_RAID_PROC_FROM_CHARGE] = true;
    isTriggerAura[SPELL_AURA_RAID_PROC_FROM_CHARGE_WITH_VALUE] = true;
    isTriggerAura[SPELL_AURA_PROC_TRIGGER_SPELL_WITH_VALUE] = true;
    isTriggerAura[SPELL_AURA_MOD_DAMAGE_FROM_CASTER] = true;
    isTriggerAura[SPELL_AURA_MOD_SPELL_CRIT_CHANCE] = true;
    isTriggerAura[SPELL_AURA_ABILITY_IGNORE_AURASTATE] = true;
    isTriggerAura[SPELL_AURA_ADD_FLAT_MODIFIER] = true;
    isTriggerAura[SPELL_AURA_ADD_PCT_MODIFIER] = true;
    isTriggerAura[SPELL_AURA_PROC_TRIGGER_SPELL_COPY] = true;
    isTriggerAura[SPELL_AURA_OVERRIDE_ACTIONBAR_SPELLS_2] = true;
    isTriggerAura[SPELL_AURA_MELEE_SLOW] = true;
    isTriggerAura[SPELL_AURA_MOD_FLIGHT_SPEED_NOT_STACK] = true;

    isNonTriggerAura[SPELL_AURA_MOD_POWER_REGEN] = true;
    isNonTriggerAura[SPELL_AURA_REDUCE_PUSHBACK] = true;

    isAlwaysTriggeredAura[SPELL_AURA_OVERRIDE_CLASS_SCRIPTS] = true;
    isAlwaysTriggeredAura[SPELL_AURA_MOD_FEAR] = true;
    isAlwaysTriggeredAura[SPELL_AURA_MOD_ROOT] = true;
    isAlwaysTriggeredAura[SPELL_AURA_MOD_STUN] = true;
    isAlwaysTriggeredAura[SPELL_AURA_TRANSFORM] = true;
    isAlwaysTriggeredAura[SPELL_AURA_SPELL_MAGNET] = true;
    isAlwaysTriggeredAura[SPELL_AURA_SCHOOL_ABSORB] = true;
    isAlwaysTriggeredAura[SPELL_AURA_MOD_STEALTH] = true;

    return true;
}

uint32 createProcExtendMask(SpellNonMeleeDamage* damageInfo, SpellMissInfo missCondition)
{
    uint32 procEx = PROC_EX_NONE;
    // Check victim state
    if (missCondition != SPELL_MISS_NONE)
        switch (missCondition)
        {
            case SPELL_MISS_MISS:    procEx |= PROC_EX_MISS;   break;
            case SPELL_MISS_RESIST:  procEx |= PROC_EX_RESIST; break;
            case SPELL_MISS_DODGE:   procEx |= PROC_EX_DODGE;  break;
            case SPELL_MISS_PARRY:   procEx |= PROC_EX_PARRY;  break;
            case SPELL_MISS_BLOCK:   procEx |= PROC_EX_BLOCK;  break;
            case SPELL_MISS_EVADE:   procEx |= PROC_EX_EVADE;  break;
            case SPELL_MISS_IMMUNE:  procEx |= PROC_EX_IMMUNE; break;
            case SPELL_MISS_IMMUNE2: procEx |= PROC_EX_IMMUNE; break;
            case SPELL_MISS_DEFLECT: procEx |= PROC_EX_DEFLECT;break;
            case SPELL_MISS_ABSORB:  procEx |= PROC_EX_ABSORB; break;
            case SPELL_MISS_REFLECT: procEx |= PROC_EX_REFLECT;break;
            default:
                break;
        }
    else
    {
        // On block
        if (damageInfo->blocked)
            procEx|=PROC_EX_BLOCK;
        // On absorb
        if (damageInfo->absorb)
            procEx|=PROC_EX_ABSORB;
        // On crit
        if (damageInfo->HitInfo & SPELL_HIT_TYPE_CRIT)
            procEx|=PROC_EX_CRITICAL_HIT;
        else
            procEx|=PROC_EX_NORMAL_HIT;
    }
    return procEx;
}

void Unit::ProcDamageAndSpellFor(bool isVictim, Unit* target, uint32 procFlag, uint32 procExtra, WeaponAttackType attType, SpellInfo const* procSpell, uint32 damage, uint32 absorb, SpellInfo const* procAura)
{
    // Player is loaded now - do not allow passive spell casts to proc
    if (GetTypeId() == TYPEID_PLAYER && ToPlayer()->GetSession()->PlayerLoading())
        return;
    // For melee/ranged based attack need update skills and set some Aura states if victim present
    if (procFlag & MELEE_BASED_TRIGGER_MASK && target)
    {
        // If exist crit/parry/dodge/block need update aura state (for victim and attacker)
        if (procExtra & (PROC_EX_CRITICAL_HIT|PROC_EX_PARRY|PROC_EX_DODGE|PROC_EX_BLOCK))
        {
            // for victim
            if (isVictim)
            {
                // if victim and dodge attack
                if (procExtra & PROC_EX_DODGE)
                {
                    // Update AURA_STATE on dodge
                    if (getClass() != CLASS_ROGUE) // skip Rogue Riposte
                    {
                        ModifyAuraState(AURA_STATE_DEFENSE, true);
                        StartReactiveTimer(REACTIVE_DEFENSE);
                    }
                }
                // if victim and parry attack
                if (procExtra & PROC_EX_PARRY)
                {
                    // For Hunters only Counterattack (skip Mongoose bite)
                    if (getClass() == CLASS_HUNTER)
                    {
                        ModifyAuraState(AURA_STATE_HUNTER_PARRY, true);
                        StartReactiveTimer(REACTIVE_HUNTER_PARRY);
                    }
                    else
                    {
                        ModifyAuraState(AURA_STATE_DEFENSE, true);
                        StartReactiveTimer(REACTIVE_DEFENSE);
                    }
                }
                // if and victim block attack
                if (procExtra & PROC_EX_BLOCK)
                {
                    ModifyAuraState(AURA_STATE_DEFENSE, true);
                    StartReactiveTimer(REACTIVE_DEFENSE);
                }
            }
            else // For attacker
            {
                // Overpower on victim dodge
                if (procExtra & PROC_EX_DODGE && GetTypeId() == TYPEID_PLAYER && getClass() == CLASS_WARRIOR)
                {
                    ToPlayer()->AddComboPoints(target, 1);
                    StartReactiveTimer(REACTIVE_OVERPOWER);
                }
            }
        }
    }

    ProcTriggeredList procTriggered;
    Unit* caster = this;

    // Some pet spells should trigger owner talents
    if (procSpell && caster->GetTypeId() != TYPEID_PLAYER)
        if (procSpell->IsShouldProcOnOwner())
            caster = GetProcOwner();

    Unit* actor = isVictim ? target : this;
    Unit* actionTarget = !isVictim ? target : this;

    DamageInfo damageInfo = DamageInfo(actor, actionTarget, damage, procSpell, procSpell ? SpellSchoolMask(procSpell->SchoolMask) : SPELL_SCHOOL_MASK_NORMAL, SPELL_DIRECT_DAMAGE);
    HealInfo healInfo = HealInfo(actor, actionTarget, damage, procSpell, procSpell ? SpellSchoolMask(procSpell->SchoolMask) : SPELL_SCHOOL_MASK_NORMAL);
    ProcEventInfo eventInfo = ProcEventInfo(actor, actionTarget, target, procFlag, 0, 0, procExtra, NULL, &damageInfo, &healInfo);		
			
    // Fill procTriggered list
    for (AuraApplicationMap::const_iterator itr = caster->GetAppliedAuras().begin(); itr!= caster->GetAppliedAuras().end(); ++itr)
    {
        // Do not allow auras to proc from effect triggered by itself
        if (procAura && procAura->Id == itr->first)
            continue;

        AuraApplication* second = itr->second;
        Aura* aura = second->GetBase();

        ProcTriggeredData triggerData(second->GetBase());
        // Defensive procs are active on absorbs (so absorption effects are not a hindrance)
        bool active = damage || (procExtra & PROC_EX_BLOCK && isVictim);
        if (isVictim)
            procExtra &= ~PROC_EX_INTERNAL_REQ_FAMILY;

        SpellInfo const* spellProto = aura->GetSpellInfo();

        // fix for not gain enchant buffs for pets
        if (caster != this && (spellProto->SpellFamilyName == SPELLFAMILY_GENERIC || spellProto->SpellFamilyName == SPELLFAMILY_POTION))
            continue;

        // only auras that has triggered spell should proc from fully absorbed damage
        if (procExtra & PROC_EX_ABSORB && isVictim)
        {
            if (damage || spellProto->HasCustomAttribute(SPELL_ATTR0_CU_AURA_CC) || spellProto->Effects[EFFECT_0].TriggerSpell || spellProto->Effects[EFFECT_1].TriggerSpell || spellProto->Effects[EFFECT_2].TriggerSpell)
                active = true;
        }

        // do checks using conditions table
        ConditionList conditions = sConditionMgr->GetConditionsForNotGroupedEntry(CONDITION_SOURCE_TYPE_SPELL_PROC, spellProto->Id);
        ConditionSourceInfo condInfo = ConditionSourceInfo(eventInfo.GetActor(), eventInfo.GetActionTarget());
        if (!sConditionMgr->IsObjectMeetToConditions(condInfo, conditions))
            continue;

        // AuraScript Hook
        if (!triggerData.aura->CallScriptCheckProcHandlers(itr->second, eventInfo))
            continue;

            // Triggered spells not triggering additional spells
        bool triggered = !(spellProto->AttributesEx3 & SPELL_ATTR3_CAN_PROC_WITH_TRIGGERED) ?
            (procExtra & PROC_EX_INTERNAL_TRIGGERED && !(procFlag & PROC_FLAG_DONE_TRAP_ACTIVATION)) : false;

        for (uint8 i = 0; i < MAX_SPELL_EFFECTS; ++i)
        {
            if (second->HasEffect(i))
            {
                if (!IsTriggeredAtSpellProcEvent(target, spellProto, procSpell, procFlag, procExtra, attType, isVictim, active, triggerData.spellProcEvent, i))
                    continue;
                AuraEffect* aurEff = aura->GetEffect(i);
                // Skip this auras
                if (isNonTriggerAura[aurEff->GetAuraType()])
                    continue;
                // If not trigger by default and spellProcEvent == NULL - skip
                if (!isTriggerAura[aurEff->GetAuraType()] && triggerData.spellProcEvent == NULL)
                    continue;
                // Some spells must always trigger
                if (!triggered || isAlwaysTriggeredAura[aurEff->GetAuraType()])
                    triggerData.effMask |= 1<<i;
            }
        }
        if (triggerData.effMask)
            procTriggered.push_front(triggerData);
    }

    // Nothing found
    if (procTriggered.empty())
        return;

    // Note: must SetCantProc(false) before return
    if (procExtra & (PROC_EX_INTERNAL_TRIGGERED | PROC_EX_INTERNAL_CANT_PROC))
        SetCantProc(true);

    // Handle effects proceed this time
    for (ProcTriggeredList::const_iterator i = procTriggered.begin(); i != procTriggered.end(); ++i)
    {
        Aura* aura = i->aura;
        // look for aura in auras list, it may be removed while proc event processing
        if (aura->IsRemoved())
            continue;

        bool useCharges = aura->IsUsingCharges();
        // no more charges to use, prevent proc
        if (useCharges && !aura->GetCharges())
            continue;

        bool takeCharges = false;
        SpellInfo const* spellInfo = aura->GetSpellInfo();
        uint32 Id = aura->GetId();

        AuraApplication const* aurApp = i->aura->GetApplicationOfTarget(GetGUID());

        bool prepare = i->aura->CallScriptPrepareProcHandlers(aurApp, eventInfo);
		
        // For players set spell cooldown if need
        uint32 cooldown = 0;
        if (prepare && GetTypeId() == TYPEID_PLAYER && i->spellProcEvent && i->spellProcEvent->cooldown)
            cooldown = i->spellProcEvent->cooldown;

        // Note: must SetCantProc(false) before return
        if (spellInfo->AttributesEx3 & SPELL_ATTR3_DISABLE_PROC)
            SetCantProc(true);

        i->aura->CallScriptProcHandlers(aurApp, eventInfo);			

        if(procSpell)
        {
            bool procCheck = false;
            Unit* _checkTarget = this;
            if (std::vector<SpellPrcoCheck> const* spellCheck = sSpellMgr->GetSpellPrcoCheck(spellInfo->Id))
            {
                for (std::vector<SpellPrcoCheck>::const_iterator itr = spellCheck->begin(); itr != spellCheck->end(); ++itr)
                {
                    if (!(itr->effectmask & i->effMask))
                        continue;
                    if(itr->target == 1 && target)
                        _checkTarget = target;
                    if (itr->checkspell < 0)
                    {
                        if (-(itr->checkspell) == procSpell->Id)
                        {
                            if(itr->hastalent != 0)
                            {
                                if(itr->hastalent > 0 && _checkTarget->HasAura(itr->hastalent))
                                    procCheck = true;
                                else if(itr->hastalent < 0 && !_checkTarget->HasAura(-(itr->hastalent)))
                                    procCheck = true;
                                if(itr->chance != 0 && !roll_chance_i(itr->chance) && procCheck)
                                    procCheck = false;
                                if(!procCheck)
                                    continue;
                            }
                            else if(itr->chance != 0 && !roll_chance_i(itr->chance))
                                procCheck = true;
                            else
                                procCheck = true;
                            break;
                        }
                    }
                    else if (itr->checkspell == procSpell->Id)
                    {
                        if(itr->hastalent != 0)
                        {
                            if(itr->hastalent > 0 && _checkTarget->HasAura(itr->hastalent))
                            {
                                procCheck = false;
                                break;
                            }
                            else if(itr->hastalent < 0 && !_checkTarget->HasAura(-(itr->hastalent)))
                            {
                                procCheck = false;
                                break;
                            }
                            procCheck = true;
                            continue;
                        }
                        if(itr->chance != 0 && !roll_chance_i(itr->chance))
                        {
                            procCheck = true;
                            break;
                        }
                        procCheck = false;
                        break;
                    }
                    else if (itr->checkspell == 0)
                    {
                        procCheck = true;
                        if(itr->hastalent != 0)
                        {
                            if(itr->hastalent > 0 && _checkTarget->HasAura(itr->hastalent))
                            {
                                procCheck = false;
                                break;
                            }
                            else if(itr->hastalent < 0 && !_checkTarget->HasAura(-(itr->hastalent)))
                            {
                                procCheck = false;
                                break;
                            }
                        }
                        if(itr->chance != 0 && roll_chance_i(itr->chance))
                        {
                            procCheck = false;
                            break;
                        }
                    }
                    else
                        procCheck = true;
                }
            }
            if(procCheck)
                continue;
        }

        // This bool is needed till separate aura effect procs are still here
        bool handled = false;
        if (HandleAuraProc(target, damage, absorb, i->aura, procSpell, procFlag, procExtra, cooldown, &handled))
            takeCharges = true;

        if (!handled)
        {
            for (uint8 effIndex = 0; effIndex < MAX_SPELL_EFFECTS; ++effIndex)
            {
                if (!(i->effMask & (1<<effIndex)))
                    continue;

                AuraEffect* triggeredByAura = aura->GetEffect(effIndex);
                ASSERT(triggeredByAura);

                switch(triggeredByAura->GetId())
                {
                    case 76672: // Hand of Light (Mastery Retribution Paladin)
                    case 48965: // Shadow Infusion
                    case 49571:
                    case 49572:
                        break;
                    case 56807: //
                        damage += absorb;
                        break;
                    default:
                        break; 
                }

                bool prevented = i->aura->CallScriptEffectProcHandlers(triggeredByAura, aurApp, eventInfo);
                if (prevented)
                {
                    takeCharges = true;
                    continue;
                }
				
                switch (triggeredByAura->GetAuraType())
                {
                    case SPELL_AURA_PROC_TRIGGER_SPELL:
                    case SPELL_AURA_PROC_TRIGGER_SPELL_2:
                    {
                        // Don`t drop charge or add cooldown for not started trigger
                        if (HandleProcTriggerSpell(target, damage, absorb, triggeredByAura, procSpell, procFlag, procExtra, cooldown))
                            takeCharges = true;
                        break;
                    }
                    case SPELL_AURA_PROC_TRIGGER_DAMAGE:
                    {
                        // target has to be valid
                        if (!target)
                            break;

                        SpellNonMeleeDamage damageInfo(this, target, spellInfo->Id, spellInfo->SchoolMask);
                        uint32 newDamage = target->SpellDamageBonusTaken(this, spellInfo, triggeredByAura->GetAmount(), SPELL_DIRECT_DAMAGE);
                        CalculateSpellDamageTaken(&damageInfo, newDamage, spellInfo);
                        DealDamageMods(damageInfo.target, damageInfo.damage, &damageInfo.absorb);
                        SendSpellNonMeleeDamageLog(&damageInfo);
                        DealSpellDamage(&damageInfo, true);
                        takeCharges = true;
                        break;
                    }
                    case SPELL_AURA_MANA_SHIELD:
                    case SPELL_AURA_DUMMY:
                    case SPELL_AURA_PROC_TRIGGER_SPELL_COPY:
                    case SPELL_AURA_MOD_FLIGHT_SPEED_NOT_STACK:
                    {
                        if (HandleDummyAuraProc(target, damage, triggeredByAura, procSpell, procFlag, procExtra, cooldown))
                            takeCharges = true;
                        break;
                    }
                    case SPELL_AURA_PROC_ON_POWER_AMOUNT:
                    {
                        if (HandleAuraProcOnPowerAmount(target, damage, triggeredByAura, procSpell, procFlag, procExtra, cooldown))
                            takeCharges = true;
                        break;
                    }
                    case SPELL_AURA_OBS_MOD_POWER:
                        if (HandleObsModEnergyAuraProc(target, damage, triggeredByAura, procSpell, procFlag, procExtra, cooldown))
                            takeCharges = true;
                        break;
                    case SPELL_AURA_MOD_DAMAGE_PERCENT_TAKEN:
                        if (HandleModDamagePctTakenAuraProc(target, damage, triggeredByAura, procSpell, procFlag, procExtra, cooldown))
                            takeCharges = true;
                        break;
                    case SPELL_AURA_MOD_MELEE_HASTE:
                    case SPELL_AURA_MELEE_SLOW:
                    case SPELL_AURA_MOD_MELEE_HASTE_3:
                    {
                        if (HandleHasteAuraProc(target, damage, triggeredByAura, procSpell, procFlag, procExtra, cooldown))
                            takeCharges = true;
                        break;
                    }
                    case SPELL_AURA_MOD_POWER_REGEN_PERCENT:
                    {
                        if (HandleModPowerRegenAuraProc(target, damage, triggeredByAura, procSpell, procFlag, procExtra, cooldown))
                            takeCharges = true;
                        break;
                    }
                    case SPELL_AURA_OVERRIDE_CLASS_SCRIPTS:
                    {
                        if (HandleOverrideClassScriptAuraProc(target, damage, triggeredByAura, procSpell, cooldown))
                            takeCharges = true;
                        break;
                    }
                    case SPELL_AURA_RAID_PROC_FROM_CHARGE_WITH_VALUE:
                    {
                        HandleAuraRaidProcFromChargeWithValue(triggeredByAura);
                        takeCharges = true;
                        break;
                    }
                    case SPELL_AURA_RAID_PROC_FROM_CHARGE:
                    {
                        HandleAuraRaidProcFromCharge(triggeredByAura);
                        takeCharges = true;
                        break;
                    }
                    case SPELL_AURA_PROC_TRIGGER_SPELL_WITH_VALUE:
                    {
                        if (HandleProcTriggerSpell(target, damage, absorb, triggeredByAura, procSpell, procFlag, procExtra, cooldown))
                            takeCharges = true;
                        break;
                    }
                    case SPELL_AURA_MOD_CASTING_SPEED_NOT_STACK:
                        // Skip melee hits or instant cast spells
                        if (procSpell && procSpell->CalcCastTime() != 0)
                            takeCharges = true;
                        break;
                    case SPELL_AURA_REFLECT_SPELLS_SCHOOL:
                        // Skip Melee hits and spells ws wrong school
                        if (procSpell && (triggeredByAura->GetMiscValue() & procSpell->SchoolMask))         // School check
                            takeCharges = true;
                        break;
                    case SPELL_AURA_SPELL_MAGNET:
                        // Skip Melee hits and targets with magnet aura
                        if (procSpell && (triggeredByAura->GetBase()->GetUnitOwner()->ToUnit() == ToUnit()))         // Magnet
                            takeCharges = true;
                        break;
                    case SPELL_AURA_MOD_POWER_COST_SCHOOL_PCT:
                    case SPELL_AURA_MOD_POWER_COST_SCHOOL:
                        // Skip melee hits and spells ws wrong school or zero cost
                        if (procSpell &&
                            (procSpell->ManaCost != 0 || procSpell->ManaCostPercentage != 0) && // Cost check
                            (triggeredByAura->GetMiscValue() & procSpell->SchoolMask))          // School check
                            takeCharges = true;
                        break;
                    case SPELL_AURA_MECHANIC_IMMUNITY:
                        // Compare mechanic
                        if (procSpell && procSpell->Mechanic == uint32(triggeredByAura->GetMiscValue()))
                            takeCharges = true;
                        break;
                    case SPELL_AURA_MOD_MECHANIC_RESISTANCE:
                        // Compare mechanic
                        if (procSpell && procSpell->Mechanic == uint32(triggeredByAura->GetMiscValue()))
                            takeCharges = true;
                        break;
                    case SPELL_AURA_MOD_DAMAGE_FROM_CASTER:
                        // Compare casters
                        if (triggeredByAura->GetCasterGUID() == target->GetGUID())
                            takeCharges = true;
                        break;
                    case SPELL_AURA_MOD_SPELL_CRIT_CHANCE:
                        if (procSpell && HandleSpellCritChanceAuraProc(target, damage, triggeredByAura, procSpell, procFlag, procExtra, cooldown))
                            takeCharges = true;
                        break;
                    // CC Auras which use their amount amount to drop
                    // Are there any more auras which need this?
                    case SPELL_AURA_MOD_CONFUSE:
                    case SPELL_AURA_MOD_FEAR:
                    case SPELL_AURA_MOD_STUN:
                    case SPELL_AURA_MOD_ROOT:
                    case SPELL_AURA_TRANSFORM:
                    {
                        if (IsNoBreakingCC(isVictim, target, procFlag, procExtra, attType, procSpell, damage, absorb, procAura, spellInfo))
                            break;

                        damage += absorb;
                        // chargeable mods are breaking on hit
                        // Spell own direct damage at apply wont break the CC
                        if (procSpell && (procSpell->Id == triggeredByAura->GetId()))
                        {
                            Aura* aura = triggeredByAura->GetBase();
                            // called from spellcast, should not have ticked yet
                            if (aura->GetDuration() == aura->GetMaxDuration())
                                break;
                        }
                        if (useCharges)
                            takeCharges = true;
                        else if (isVictim && damage && (!procSpell || GetSpellMaxRangeForTarget(this, procSpell) < 100.0f))
                        {
                            int32 damageLeft = triggeredByAura->GetAmount();
                            // No damage left
                            if (damageLeft < int32(damage))
                                i->aura->Remove();
                            else
                                triggeredByAura->SetAmount(damageLeft - damage);
                        }
                        break;
                    }
                    case SPELL_AURA_OVERRIDE_ACTIONBAR_SPELLS:
                    {
                        if (triggeredByAura->GetId() == 74434   // Soulburn
                        && procSpell && procSpell->Id == 27243) // Seed of Corruption
                        {
                            if (ToPlayer()->HasSpell(86664))    // Soulburn - Seed of Corruption learned talent check
                                takeCharges = true;
                            else
                                takeCharges = false;
                            break;
                        }
                        takeCharges = true;
                        break;
                    }
                    case SPELL_AURA_OVERRIDE_ACTIONBAR_SPELLS_2:
                        takeCharges = procSpell->Id == triggeredByAura->GetAmount();
                        useCharges = takeCharges;
                        break;
                    case SPELL_AURA_ADD_FLAT_MODIFIER:
                    case SPELL_AURA_ADD_PCT_MODIFIER:
                        HandleModifierAuraProc(target, damage, triggeredByAura, procSpell, procFlag, procExtra, cooldown);
                        takeCharges = false;
                        break;
                    default:
                        // nothing do, just charges counter
                        takeCharges = true;
                        break;
                } // switch (triggeredByAura->GetAuraType())
                i->aura->CallScriptAfterEffectProcHandlers(triggeredByAura, aurApp, eventInfo);
            } // for (uint8 effIndex = 0; effIndex < MAX_SPELL_EFFECTS; ++effIndex)
        } // if (!handled)

        // Remove charge (aura can be removed by triggers)
        if (prepare && useCharges && takeCharges)
            i->aura->DropCharge();
			
        i->aura->CallScriptAfterProcHandlers(aurApp, eventInfo);

        if (spellInfo->AttributesEx3 & SPELL_ATTR3_DISABLE_PROC)
            SetCantProc(false);
    }

    // Cleanup proc requirements
    if (procExtra & (PROC_EX_INTERNAL_TRIGGERED | PROC_EX_INTERNAL_CANT_PROC))
        SetCantProc(false);
}

bool Unit::IsNoBreakingCC(bool isVictim, Unit* target, uint32 procFlag, uint32 procExtra, WeaponAttackType attType, SpellInfo const* procSpell, 
uint32 damage, uint32 absorb /* = 0 */, SpellInfo const* procAura /* = NULL */, SpellInfo const* spellInfo ) const
{
    // Dragon Breath & Living Bomb
    if (spellInfo->Category == 1215 && procSpell &&
        procSpell->SpellFamilyName == SPELLFAMILY_MAGE && procSpell->SpellFamilyFlags[1] == 0x00010000)
        return true;

    // Sanguinary Vein and Gouge
    if (spellInfo->Id == 1776 && target && (procFlag & PROC_FLAG_TAKEN_PERIODIC) && (procSpell && (procSpell->GetAllEffectsMechanicMask() & (1<<MECHANIC_BLEED))))
        if (AuraEffect * aur = target->GetDummyAuraEffect(SPELLFAMILY_GENERIC, 4821, 1))
            if (roll_chance_i(aur->GetAmount()))
                return true;

    // Main Gauche & Gouge
    if (procSpell && procSpell->Id == 86392 && spellInfo->Id == 1776)
        return true;

    return false;
}

void Unit::GetProcAurasTriggeredOnEvent(std::list<AuraApplication*>& aurasTriggeringProc, std::list<AuraApplication*>* procAuras, ProcEventInfo eventInfo)
{
    // use provided list of auras which can proc
    if (procAuras)
    {
        for (std::list<AuraApplication*>::iterator itr = procAuras->begin(); itr!= procAuras->end(); ++itr)
        {
            ASSERT((*itr)->GetTarget() == this);
            if (!(*itr)->GetRemoveMode())
                if ((*itr)->GetBase()->IsProcTriggeredOnEvent(*itr, eventInfo))
                {
                   (*itr)->GetBase()->PrepareProcToTrigger(*itr, eventInfo);
                    aurasTriggeringProc.push_back(*itr);
                }
        }
    }
    // or generate one on our own
    else
    {
        for (AuraApplicationMap::iterator itr = GetAppliedAuras().begin(); itr!= GetAppliedAuras().end(); ++itr)
        {
            if (itr->second->GetBase()->IsProcTriggeredOnEvent(itr->second, eventInfo))
            {
                itr->second->GetBase()->PrepareProcToTrigger(itr->second, eventInfo);
                aurasTriggeringProc.push_back(itr->second);
            }
        }
    }
}

void Unit::TriggerAurasProcOnEvent(CalcDamageInfo& damageInfo)
{
    DamageInfo dmgInfo = DamageInfo(damageInfo);
    TriggerAurasProcOnEvent(NULL, NULL, damageInfo.target, damageInfo.procAttacker, damageInfo.procVictim, 0, 0, damageInfo.procEx, NULL, &dmgInfo, NULL);
}

void Unit::TriggerAurasProcOnEvent(std::list<AuraApplication*>* myProcAuras, std::list<AuraApplication*>* targetProcAuras, Unit* actionTarget, uint32 typeMaskActor, uint32 typeMaskActionTarget, uint32 spellTypeMask, uint32 spellPhaseMask, uint32 hitMask, Spell* spell, DamageInfo* damageInfo, HealInfo* healInfo)
{
    // prepare data for self trigger
    ProcEventInfo myProcEventInfo = ProcEventInfo(this, actionTarget, actionTarget, typeMaskActor, spellTypeMask, spellPhaseMask, hitMask, spell, damageInfo, healInfo);
    std::list<AuraApplication*> myAurasTriggeringProc;
    GetProcAurasTriggeredOnEvent(myAurasTriggeringProc, myProcAuras, myProcEventInfo);

    // prepare data for target trigger
    ProcEventInfo targetProcEventInfo = ProcEventInfo(this, actionTarget, this, typeMaskActionTarget, spellTypeMask, spellPhaseMask, hitMask, spell, damageInfo, healInfo);
    std::list<AuraApplication*> targetAurasTriggeringProc;
    if (typeMaskActionTarget)
        GetProcAurasTriggeredOnEvent(targetAurasTriggeringProc, targetProcAuras, targetProcEventInfo);

    TriggerAurasProcOnEvent(myProcEventInfo, myAurasTriggeringProc);

    if (typeMaskActionTarget)
        TriggerAurasProcOnEvent(targetProcEventInfo, targetAurasTriggeringProc);
}

void Unit::TriggerAurasProcOnEvent(ProcEventInfo& eventInfo, std::list<AuraApplication*>& aurasTriggeringProc)
{
    for (std::list<AuraApplication*>::iterator itr = aurasTriggeringProc.begin(); itr != aurasTriggeringProc.end(); ++itr)
    {
        if (!(*itr)->GetRemoveMode())
            (*itr)->GetBase()->TriggerProcOnEvent(*itr, eventInfo);
    }
}

SpellSchoolMask Unit::GetMeleeDamageSchoolMask() const
{
    return SPELL_SCHOOL_MASK_NORMAL;
}

Player* Unit::GetSpellModOwner() const
{
    if (GetTypeId() == TYPEID_PLAYER)
        return (Player*)this;
    if (ToCreature()->isPet() || ToCreature()->isTotem())
    {
        Unit* owner = GetOwner();
        if (owner && owner->GetTypeId() == TYPEID_PLAYER)
            return (Player*)owner;
    }
    return NULL;
}

///----------Pet responses methods-----------------
void Unit::SendPetCastFail(uint32 spellid, SpellCastResult result)
{
    if (result == SPELL_CAST_OK)
        return;

    Unit* owner = GetCharmerOrOwner();
    if (!owner || owner->GetTypeId() != TYPEID_PLAYER)
        return;

    WorldPacket data(SMSG_PET_CAST_FAILED, 1 + 4 + 1);
    data << uint8(0);                                       // cast count
    data << uint32(spellid);
    data << uint8(result);
    owner->ToPlayer()->GetSession()->SendPacket(&data);
}

void Unit::SendPetActionFeedback(uint8 msg)
{
    Unit* owner = GetOwner();
    if (!owner || owner->GetTypeId() != TYPEID_PLAYER)
        return;

    WorldPacket data(SMSG_PET_ACTION_FEEDBACK, 1);
    data << uint8(msg);
    owner->ToPlayer()->GetSession()->SendPacket(&data);
}

void Unit::SendPetTalk(uint32 pettalk)
{
    Unit* owner = GetOwner();
    if (!owner || owner->GetTypeId() != TYPEID_PLAYER)
        return;

    WorldPacket data(SMSG_PET_ACTION_SOUND, 8 + 4);
    data << uint64(GetGUID());
    data << uint32(pettalk);
    owner->ToPlayer()->GetSession()->SendPacket(&data);
}

void Unit::SendPetAIReaction(uint64 guid)
{
    Unit* owner = GetOwner();
    if (!owner || owner->GetTypeId() != TYPEID_PLAYER)
        return;

    WorldPacket data(SMSG_AI_REACTION, 8 + 4);
    data << uint64(guid);
    data << uint32(AI_REACTION_HOSTILE);
    owner->ToPlayer()->GetSession()->SendPacket(&data);
}

///----------End of Pet responses methods----------

void Unit::StopMoving()
{
    ClearUnitState(UNIT_STATE_MOVING);

    // not need send any packets if not in world or not moving
    if (!IsInWorld() || movespline->Finalized())
        return;

    // Update position using old spline
    UpdateSplinePosition();
    Movement::MoveSplineInit(*this).Stop();
}

void Unit::SendMovementFlagUpdate(bool self /* = false */)
{
    WorldPacket data(SMSG_PLAYER_MOVE, 40);
    WriteMovementInfo(data);
    SendMessageToSet(&data, self);
}

bool Unit::IsSitState() const
{
    uint8 s = getStandState();
    return
        s == UNIT_STAND_STATE_SIT_CHAIR        || s == UNIT_STAND_STATE_SIT_LOW_CHAIR  ||
        s == UNIT_STAND_STATE_SIT_MEDIUM_CHAIR || s == UNIT_STAND_STATE_SIT_HIGH_CHAIR ||
        s == UNIT_STAND_STATE_SIT;
}

bool Unit::IsStandState() const
{
    uint8 s = getStandState();
    return !IsSitState() && s != UNIT_STAND_STATE_SLEEP && s != UNIT_STAND_STATE_KNEEL;
}

void Unit::SetStandState(uint8 state)
{
    SetByteValue(UNIT_FIELD_BYTES_1, 0, state);

    if (IsStandState())
       RemoveAurasWithInterruptFlags(AURA_INTERRUPT_FLAG_NOT_SEATED);

    if (GetTypeId() == TYPEID_PLAYER)
    {
        WorldPacket data(SMSG_STANDSTATE_UPDATE, 1);
        data << (uint8)state;
        ToPlayer()->GetSession()->SendPacket(&data);
    }
}

bool Unit::IsPolymorphed(bool forMount) const
{
    uint32 transformId = getTransForm();
    if (!transformId)
        return false;

    SpellInfo const* spellInfo = sSpellMgr->GetSpellInfo(transformId);
    if (!spellInfo)
        return false;

    return spellInfo->GetSpellSpecific() == SPELL_SPECIFIC_MAGE_POLYMORPH || (forMount ? spellInfo->Id == 51514: false);
}

void Unit::SetDisplayId(uint32 modelId)
{
    SetUInt32Value(UNIT_FIELD_DISPLAYID, modelId);

    if (GetTypeId() == TYPEID_UNIT && ToCreature()->isPet())
    {
        Pet* pet = ToPet();
        if (!pet->isControlled())
            return;
        Unit* owner = GetOwner();
        if (owner && (owner->GetTypeId() == TYPEID_PLAYER) && owner->ToPlayer()->GetGroup())
            owner->ToPlayer()->SetGroupUpdateFlag(GROUP_UPDATE_FLAG_PET_MODEL_ID);
    }
}

void Unit::RestoreDisplayId()
{
    AuraEffect* handledAura = NULL;
    // try to receive model from transform auras
    Unit::AuraEffectList const& transforms = GetAuraEffectsByType(SPELL_AURA_TRANSFORM);
    if (!transforms.empty())
    {
        // iterate over already applied transform auras - from newest to oldest
        for (Unit::AuraEffectList::const_reverse_iterator i = transforms.rbegin(); i != transforms.rend(); ++i)
        {
            if (AuraApplication const* aurApp = (*i)->GetBase()->GetApplicationOfTarget(GetGUID()))
            {
                if (!handledAura)
                    handledAura = (*i);
                // prefer negative auras
                if (!aurApp->IsPositive())
                {
                    handledAura = (*i);
                    break;
                }
            }
        }
    }
    // transform aura was found
    if (handledAura)
        handledAura->HandleEffect(this, AURA_EFFECT_HANDLE_SEND_FOR_CLIENT, true);
    // we've found shapeshift
    else if (uint32 modelId = GetModelForForm(GetShapeshiftForm()))
        SetDisplayId(modelId);
    // no auras found - set modelid to default
    else
        SetDisplayId(GetNativeDisplayId());
}

void Unit::ClearComboPointHolders()
{
    while (!m_ComboPointHolders.empty())
    {
        uint32 lowguid = *m_ComboPointHolders.begin();

        Player* player = ObjectAccessor::FindPlayer(MAKE_NEW_GUID(lowguid, 0, HIGHGUID_PLAYER));
        if (player && player->GetComboTarget() == GetGUID())         // recheck for safe
            player->ClearComboPoints();                        // remove also guid from m_ComboPointHolders;
        else
            m_ComboPointHolders.erase(lowguid);             // or remove manually
    }
}

void Unit::ClearAllReactives()
{
    for (uint8 i = 0; i < MAX_REACTIVE; ++i)
        m_reactiveTimer[i] = 0;

    if (HasAuraState(AURA_STATE_DEFENSE))
        ModifyAuraState(AURA_STATE_DEFENSE, false);
    if (getClass() == CLASS_HUNTER && HasAuraState(AURA_STATE_HUNTER_PARRY))
        ModifyAuraState(AURA_STATE_HUNTER_PARRY, false);
    if (getClass() == CLASS_WARRIOR && GetTypeId() == TYPEID_PLAYER)
        ToPlayer()->ClearComboPoints();
}

void Unit::UpdateReactives(uint32 p_time)
{
    for (uint8 i = 0; i < MAX_REACTIVE; ++i)
    {
        ReactiveType reactive = ReactiveType(i);

        if (!m_reactiveTimer[reactive])
            continue;

        if (m_reactiveTimer[reactive] <= p_time)
        {
            m_reactiveTimer[reactive] = 0;

            switch (reactive)
            {
                case REACTIVE_DEFENSE:
                    if (HasAuraState(AURA_STATE_DEFENSE))
                    {
                        if (getClass() == CLASS_DEATH_KNIGHT && HasAura(PRESENCE_BLOOD))
                            break;

                        RemoveAura(56817);
                        ModifyAuraState(AURA_STATE_DEFENSE, false);
                    }
                    break;
                case REACTIVE_HUNTER_PARRY:
                    if (getClass() == CLASS_HUNTER && HasAuraState(AURA_STATE_HUNTER_PARRY))
                        ModifyAuraState(AURA_STATE_HUNTER_PARRY, false);
                    break;
                case REACTIVE_OVERPOWER:
                    if (getClass() == CLASS_WARRIOR && GetTypeId() == TYPEID_PLAYER)
                        ToPlayer()->ClearComboPoints();
                    break;
                default:
                    break;
            }
        }
        else
        {
            m_reactiveTimer[reactive] -= p_time;
        }
    }
}

Unit* Unit::SelectNearbyTarget(Unit* exclude, float dist) const
{
    std::list<Unit*> targets;
    Trinity::AnyUnfriendlyUnitInObjectRangeCheck u_check(this, this, dist);
    Trinity::UnitListSearcher<Trinity::AnyUnfriendlyUnitInObjectRangeCheck> searcher(this, targets, u_check);
    VisitNearbyObject(dist, searcher);

    // remove current target
    if (getVictim())
        targets.remove(getVictim());

    if (exclude)
        targets.remove(exclude);

    // remove not LoS targets
    for (std::list<Unit*>::iterator tIter = targets.begin(); tIter != targets.end();)
    {
        if (!IsValidAttackTarget(*tIter) || !IsWithinLOSInMap(*tIter) || (*tIter)->isTotem() || (*tIter)->isSpiritService() || (*tIter)->GetCreatureType() == CREATURE_TYPE_CRITTER)
            targets.erase(tIter++);
        else
            ++tIter;
    }

    // no appropriate targets
    if (targets.empty())
        return NULL;

    // select random
    return Trinity::Containers::SelectRandomContainerElement(targets);
}

void Unit::ApplyAttackTimePercentMod(WeaponAttackType att, float val, bool apply)
{
    float remainingTimePct = (float)m_attackTimer[att] / (GetAttackTime(att) * m_modAttackSpeedPct[att]);
    if (val > 0)
    {
        ApplyPercentModFloatVar(m_modAttackSpeedPct[att], val, !apply);
        ApplyPercentModFloatValue(UNIT_FIELD_BASEATTACKTIME+att, val, !apply);

        if (GetTypeId() == TYPEID_PLAYER)
        {
            if (att == BASE_ATTACK)
                ApplyPercentModFloatValue(PLAYER_FIELD_MOD_HASTE, val, !apply);
            else if (att == RANGED_ATTACK)
                ApplyPercentModFloatValue(PLAYER_FIELD_MOD_RANGED_HASTE, val, !apply);
        }
    }
    else
    {
        ApplyPercentModFloatVar(m_modAttackSpeedPct[att], -val, apply);
        ApplyPercentModFloatValue(UNIT_FIELD_BASEATTACKTIME+att, -val, apply);

        if (GetTypeId() == TYPEID_PLAYER)
        {
            if (att == BASE_ATTACK)
                ApplyPercentModFloatValue(PLAYER_FIELD_MOD_HASTE, -val, apply);
            else if (att == RANGED_ATTACK)
                ApplyPercentModFloatValue(PLAYER_FIELD_MOD_RANGED_HASTE, -val, apply);
        }
    }

    if (GetTypeId() == TYPEID_PLAYER)
    {
        // Sanctity of Battle - update hacked spellmods
        AuraEffectList const& hasteCooldownAura = GetAuraEffectsByType(SPELL_AURA_MOD_SPELL_COOLDOWN_BY_HASTE);
        for (AuraEffectList::const_iterator i = hasteCooldownAura.begin(); i != hasteCooldownAura.end(); ++i)
            (*i)->RecalculateAmount(true);

        // Update pets attack speed
        if (Pet* pPet = ToPlayer()->GetPet())
            pPet->UpdateAttackSpeed();
    }

    m_attackTimer[att] = uint32(GetAttackTime(att) * m_modAttackSpeedPct[att] * remainingTimePct);
}

void Unit::ApplyCastTimePercentMod(float val, bool apply)
{
    if (val > 0)
    {
        ApplyPercentModFloatValue(UNIT_MOD_CAST_SPEED, val, !apply);
        ApplyPercentModFloatValue(UNIT_MOD_CAST_HASTE, val, !apply);
    }
    else
    {
        ApplyPercentModFloatValue(UNIT_MOD_CAST_SPEED, -val, apply);
        ApplyPercentModFloatValue(UNIT_MOD_CAST_HASTE, -val, apply);
    }
}

uint32 Unit::GetCastingTimeForBonus(SpellInfo const* spellProto, DamageEffectType damagetype, uint32 CastingTime) const
{
    // Not apply this to creature casted spells with casttime == 0
    if (CastingTime == 0 && GetTypeId() == TYPEID_UNIT && !ToCreature()->isPet())
        return 3500;

    if (CastingTime > 7000) CastingTime = 7000;
    if (CastingTime < 1500) CastingTime = 1500;

    if (damagetype == DOT && !spellProto->IsChanneled())
        CastingTime = 3500;

    int32 overTime    = 0;
    uint8 effects     = 0;
    bool DirectDamage = false;
    bool AreaEffect   = false;

    for (uint32 i = 0; i < MAX_SPELL_EFFECTS; i++)
    {
        switch (spellProto->Effects[i].Effect)
        {
            case SPELL_EFFECT_SCHOOL_DAMAGE:
            case SPELL_EFFECT_POWER_DRAIN:
            case SPELL_EFFECT_HEALTH_LEECH:
            case SPELL_EFFECT_ENVIRONMENTAL_DAMAGE:
            case SPELL_EFFECT_POWER_BURN:
            case SPELL_EFFECT_HEAL:
                DirectDamage = true;
                break;
            case SPELL_EFFECT_APPLY_AURA:
                switch (spellProto->Effects[i].ApplyAuraName)
                {
                    case SPELL_AURA_PERIODIC_DAMAGE:
                    case SPELL_AURA_PERIODIC_HEAL:
                    case SPELL_AURA_PERIODIC_LEECH:
                        if (spellProto->GetDuration())
                            overTime = spellProto->GetDuration();
                        break;
                    default:
                        // -5% per additional effect
                        ++effects;
                        break;
                }
            default:
                break;
        }

        if (spellProto->Effects[i].IsTargetingArea())
            AreaEffect = true;
    }

    // Combined Spells with Both Over Time and Direct Damage
    if (overTime > 0 && CastingTime > 0 && DirectDamage)
    {
        // mainly for DoTs which are 3500 here otherwise
        uint32 OriginalCastTime = spellProto->CalcCastTime();
        if (OriginalCastTime > 7000) OriginalCastTime = 7000;
        if (OriginalCastTime < 1500) OriginalCastTime = 1500;
        // Portion to Over Time
        float PtOT = (overTime / 15000.0f) / ((overTime / 15000.0f) + (OriginalCastTime / 3500.0f));

        if (damagetype == DOT)
            CastingTime = uint32(CastingTime * PtOT);
        else if (PtOT < 1.0f)
            CastingTime  = uint32(CastingTime * (1 - PtOT));
        else
            CastingTime = 0;
    }

    // Area Effect Spells receive only half of bonus
    if (AreaEffect)
        CastingTime /= 2;

    // 50% for damage and healing spells for leech spells from damage bonus and 0% from healing
    for (uint8 j = 0; j < MAX_SPELL_EFFECTS; ++j)
    {
        if (spellProto->Effects[j].Effect == SPELL_EFFECT_HEALTH_LEECH ||
            (spellProto->Effects[j].Effect == SPELL_EFFECT_APPLY_AURA && spellProto->Effects[j].ApplyAuraName == SPELL_AURA_PERIODIC_LEECH))
        {
            CastingTime /= 2;
            break;
        }
    }

    // -5% of total per any additional effect
    for (uint8 i = 0; i < effects; ++i)
        CastingTime *= 0.95f;

    return CastingTime;
}

void Unit::UpdateAuraForGroup(uint8 slot)
{
    if (slot >= MAX_AURAS)                        // slot not found, return
        return;
    if (Player* player = ToPlayer())
    {
        if (player->GetGroup())
        {
            player->SetGroupUpdateFlag(GROUP_UPDATE_FLAG_AURAS);
            player->SetAuraUpdateMaskForRaid(slot);
        }
    }
    else if (GetTypeId() == TYPEID_UNIT && ToCreature()->isPet())
    {
        Pet* pet = ((Pet*)this);
        if (pet->isControlled())
        {
            Unit* owner = GetOwner();
            if (owner && (owner->GetTypeId() == TYPEID_PLAYER) && owner->ToPlayer()->GetGroup())
            {
                owner->ToPlayer()->SetGroupUpdateFlag(GROUP_UPDATE_FLAG_PET_AURAS);
                pet->SetAuraUpdateMaskForRaid(slot);
            }
        }
    }
}

float Unit::CalculateDefaultCoefficient(SpellInfo const *spellInfo, DamageEffectType damagetype) const
{
    // Damage over Time spells bonus calculation
    float DotFactor = 1.0f;
    if (damagetype == DOT)
    {

        int32 DotDuration = spellInfo->GetDuration();
        if (!spellInfo->IsChanneled() && DotDuration > 0)
            DotFactor = DotDuration / 15000.0f;

        if (uint32 DotTicks = spellInfo->GetMaxTicks())
            DotFactor /= DotTicks;
    }

    int32 CastingTime = spellInfo->IsChanneled() ? spellInfo->GetDuration() : spellInfo->CalcCastTime();
    // Distribute Damage over multiple effects, reduce by AoE
    CastingTime = GetCastingTimeForBonus(spellInfo, damagetype, CastingTime);

    // As wowwiki says: C = (Cast Time / 3.5)
    return (CastingTime / 3500.0f) * DotFactor;
}

float Unit::GetAPMultiplier(WeaponAttackType attType, bool normalized)
{
    if (!normalized || GetTypeId() != TYPEID_PLAYER)
        return float(GetAttackTime(attType)) / 1000.0f;

    Item* Weapon = ToPlayer()->GetWeaponForAttack(attType, true);
    if (!Weapon)
        return 2.4f;                                         // fist attack

    switch (Weapon->GetTemplate()->InventoryType)
    {
        case INVTYPE_2HWEAPON:
            return 3.3f;
        case INVTYPE_RANGED:
        case INVTYPE_RANGEDRIGHT:
        case INVTYPE_THROWN:
            return 2.8f;
        case INVTYPE_WEAPON:
        case INVTYPE_WEAPONMAINHAND:
        case INVTYPE_WEAPONOFFHAND:
        default:
            return Weapon->GetTemplate()->SubClass == ITEM_SUBCLASS_WEAPON_DAGGER ? 1.7f : 2.4f;
    }
}

void Unit::SetContestedPvP(Player* attackedPlayer)
{
    Player* player = GetCharmerOrOwnerPlayerOrPlayerItself();

    if (!player || (attackedPlayer && (attackedPlayer == player || (player->duel && player->duel->opponent == attackedPlayer))))
        return;

    player->SetContestedPvPTimer(30000);
    if (!player->HasUnitState(UNIT_STATE_ATTACK_PLAYER))
    {
        player->AddUnitState(UNIT_STATE_ATTACK_PLAYER);
        player->SetFlag(PLAYER_FLAGS, PLAYER_FLAGS_CONTESTED_PVP);
        // call MoveInLineOfSight for nearby contested guards
        UpdateObjectVisibility();
    }
    if (!HasUnitState(UNIT_STATE_ATTACK_PLAYER))
    {
        AddUnitState(UNIT_STATE_ATTACK_PLAYER);
        // call MoveInLineOfSight for nearby contested guards
        UpdateObjectVisibility();
    }
}

void Unit::AddPetAura(PetAura const* petSpell)
{
    if (GetTypeId() != TYPEID_PLAYER)
        return;

    m_petAuras.insert(petSpell);
    if (Pet* pet = ToPlayer()->GetPet())
        pet->CastPetAura(petSpell);
}

void Unit::RemovePetAura(PetAura const* petSpell)
{
    if (GetTypeId() != TYPEID_PLAYER)
        return;

    m_petAuras.erase(petSpell);
    if (Pet* pet = ToPlayer()->GetPet())
        pet->RemoveAurasDueToSpell(petSpell->GetAura(pet->GetEntry()));
}

Pet* Unit::CreateTamedPetFrom(Creature* creatureTarget, uint32 spell_id)
{
    if (GetTypeId() != TYPEID_PLAYER)
        return NULL;

    Pet* pet = new Pet((Player*)this, HUNTER_PET);

    if (!pet->CreateBaseAtCreature(creatureTarget))
    {
        delete pet;
        return NULL;
    }

    uint8 level = creatureTarget->getLevel() + 5 < getLevel() ? (getLevel() - 5) : creatureTarget->getLevel();

    InitTamedPet(pet, level, spell_id);

    return pet;
}

Pet* Unit::CreateTamedPetFrom(uint32 creatureEntry, uint32 spell_id)
{
    if (GetTypeId() != TYPEID_PLAYER)
        return NULL;

    CreatureTemplate const* creatureInfo = sObjectMgr->GetCreatureTemplate(creatureEntry);
    if (!creatureInfo)
        return NULL;

    Pet* pet = new Pet((Player*)this, HUNTER_PET);

    if (!pet->CreateBaseAtCreatureInfo(creatureInfo, this) || !InitTamedPet(pet, getLevel(), spell_id))
    {
        delete pet;
        return NULL;
    }

    return pet;
}

bool Unit::InitTamedPet(Pet* pet, uint8 level, uint32 spell_id)
{
    pet->SetCreatorGUID(GetGUID());
    pet->setFaction(getFaction());
    pet->SetUInt32Value(UNIT_CREATED_BY_SPELL, spell_id);

    if (GetTypeId() == TYPEID_PLAYER)
        pet->SetUInt32Value(UNIT_FIELD_FLAGS, UNIT_FLAG_PVP_ATTACKABLE);

    if (!pet->InitStatsForLevel(level))
        return false;

    pet->GetCharmInfo()->SetPetNumber(sObjectMgr->GeneratePetNumber(), true);
    // this enables pet details window (Shift+P)
    pet->InitPetCreateSpells();
    //pet->InitLevelupSpellsForLevel();
    pet->SetFullHealth();
    return true;
}

bool Unit::IsTriggeredAtSpellProcEvent(Unit* victim, SpellInfo const* spellProto, SpellInfo const* procSpell, uint32 procFlag, uint32 procExtra, WeaponAttackType attType, bool isVictim, bool active, SpellProcEventEntry const* & spellProcEvent, uint8 effect)
{
    if(!spellProto)
        return false;

    // let the aura be handled by new proc system if it has new entry
    //if (sSpellMgr->GetSpellProcEntry(spellProto->Id))
        //return false;

    // Get proc Event Entry
    //spellProcEvent = sSpellMgr->GetSpellProcEvent(spellProto->Id);
    if (std::vector<SpellProcEventEntry> const* spellproc = sSpellMgr->GetSpellProcEvent(spellProto->Id))
    {
        for (std::vector<SpellProcEventEntry>::const_iterator itr = spellproc->begin(); itr != spellproc->end(); ++itr)
        {
            if (itr->effectMask & (1 << effect))
            {
                spellProcEvent = &(*itr);
                break;
            }
        }
    }

    // Get EventProcFlag
    uint32 EventProcFlag;
    if (spellProcEvent && spellProcEvent->procFlags) // if exist get custom spellProcEvent->procFlags
        EventProcFlag = spellProcEvent->procFlags;
    else
        EventProcFlag = spellProto->ProcFlags;       // else get from spell proto
    // Continue if no trigger exist
    if (!EventProcFlag)
        return false;

    // Additional checks for triggered spells (ignore trap casts)
    if (procExtra & PROC_EX_INTERNAL_TRIGGERED && !(procFlag & PROC_FLAG_DONE_TRAP_ACTIVATION))
    {
        if (!(spellProto->AttributesEx3 & SPELL_ATTR3_CAN_PROC_WITH_TRIGGERED))
            return false;
    }

    // Check spellProcEvent data requirements
    if (!sSpellMgr->IsSpellProcEventCanTriggeredBy(spellProcEvent, EventProcFlag, procSpell, procFlag, procExtra, active))
        return false;
    // In most cases req get honor or XP from kill
    if (EventProcFlag & PROC_FLAG_KILL && GetTypeId() == TYPEID_PLAYER)
    {
        bool allow = false;

        if (victim)
            allow = ToPlayer()->isHonorOrXPTarget(victim);

        // Shadow Word: Death - can trigger from every kill
        if (spellProto->Id == 32409)
            allow = true;

        if (victim && (victim->IsPetGuardianStuff() && !victim->isTotem()))
            allow = true;

        if (!allow)
            return false;
    }
    // Aura added by spell can`t trigger from self (prevent drop charges/do triggers)
    // But except periodic and kill triggers (can triggered from self)
    if (procSpell && procSpell->Id == spellProto->Id
        && !(spellProto->ProcFlags&(PROC_FLAG_TAKEN_PERIODIC | PROC_FLAG_KILL)))
        return false;

    // Check if current equipment allows aura to proc
    if (!isVictim && GetTypeId() == TYPEID_PLAYER)
    {
        Player* player = ToPlayer();
        if (spellProto->EquippedItemClass == ITEM_CLASS_WEAPON)
        {
            Item* item = NULL;
            if (attType == BASE_ATTACK)
                item = player->GetUseableItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_MAINHAND);
            else if (attType == OFF_ATTACK)
                item = player->GetUseableItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_OFFHAND);
            else
                item = player->GetUseableItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_RANGED);

            if (player->IsInFeralForm())
                return false;

            if (!item || item->IsBroken() || item->GetTemplate()->Class != ITEM_CLASS_WEAPON || !((1<<item->GetTemplate()->SubClass) & spellProto->EquippedItemSubClassMask))
                return false;
        }
        else if (spellProto->EquippedItemClass == ITEM_CLASS_ARMOR)
        {
            // Check if player is wearing shield
            Item* item = player->GetUseableItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_OFFHAND);
            if (!item || item->IsBroken() || item->GetTemplate()->Class != ITEM_CLASS_ARMOR || !((1<<item->GetTemplate()->SubClass) & spellProto->EquippedItemSubClassMask))
                return false;
        }
    }
    // Get chance from spell
    float chance = float(spellProto->ProcChance);
    // If in spellProcEvent exist custom chance, chance = spellProcEvent->customChance;
    if (spellProcEvent && spellProcEvent->customChance)
        chance = spellProcEvent->customChance;

    // Earthliving Weapon
    if (spellProto->SpellFamilyFlags[1] == 0x00100000 && spellProto->SpellFamilyName == SPELLFAMILY_SHAMAN)
        // Blessing of the Eternals
        if (victim && victim->HealthBelowPct(35))
        {
            if (ToPlayer()->HasAuraEffect(51554, EFFECT_1))
                chance += 40;
            else if (ToPlayer()->HasAuraEffect(51555, EFFECT_1))
                chance += 80;
        }

    // If PPM exist calculate chance from PPM
    if (spellProcEvent && spellProcEvent->ppmRate != 0)
    {
        if (!isVictim)
        {
            uint32 WeaponSpeed = GetAttackTime(attType);
            chance = GetPPMProcChance(WeaponSpeed, spellProcEvent->ppmRate, spellProto);
        }
        else
        {
            uint32 WeaponSpeed = victim->GetAttackTime(attType);
            chance = victim->GetPPMProcChance(WeaponSpeed, spellProcEvent->ppmRate, spellProto);
        }
    }
    // Apply chance modifer aura
    if (Player* modOwner = GetSpellModOwner())
    {
        modOwner->ApplySpellMod(spellProto->Id, SPELLMOD_CHANCE_OF_SUCCESS, chance);
    }
    return roll_chance_f(chance);
}

bool Unit::HandleAuraRaidProcFromChargeWithValue(AuraEffect* triggeredByAura)
{
    // aura can be deleted at casts
    SpellInfo const* spellProto = triggeredByAura->GetSpellInfo();
    int32 heal = triggeredByAura->GetAmount();
    uint64 caster_guid = triggeredByAura->GetCasterGUID();

    // Currently only Prayer of Mending
    if (!(spellProto->SpellFamilyName == SPELLFAMILY_PRIEST && spellProto->SpellFamilyFlags[1] & 0x20))
        return false;

    // jumps
    int32 jumps = triggeredByAura->GetBase()->GetCharges()-1;

    // current aura expire
    triggeredByAura->GetBase()->SetCharges(1);             // will removed at next charges decrease

    // next target selection
    if (jumps > 0)
    {
        if (Unit* caster = triggeredByAura->GetCaster())
        {
            float radius = triggeredByAura->GetSpellInfo()->Effects[triggeredByAura->GetEffIndex()].CalcRadius(caster, NULL, true);

            if (Unit* target = GetNextRandomRaidMemberOrPet(radius))
            {
                CastCustomSpell(target, spellProto->Id, &heal, NULL, NULL, true, NULL, triggeredByAura, caster_guid);
                CastCustomSpell(target, 41637, &heal, NULL, NULL, true, NULL, triggeredByAura, caster_guid);
                if (Aura* aura = target->GetAura(spellProto->Id, caster->GetGUID()))
                    aura->SetCharges(jumps);
            }
            if (Player * modOwner = caster->GetSpellModOwner())
            {
                // first jump - Glyph of Prayer of Mending
                if (jumps == 4)
                    if (AuraEffect * aur = modOwner->GetAuraEffect(55685, 0))
                        heal*=(100.0f+aur->GetAmount())/100.0f;

                modOwner->ApplySpellMod(spellProto->Id, SPELLMOD_RADIUS, radius, NULL);
            }
        }
    }

    // heal
    CastCustomSpell(this, 33110, &heal, NULL, NULL, true, NULL, NULL, caster_guid);
    return true;

}

bool Unit::HandleAuraRaidProcFromCharge(AuraEffect* triggeredByAura)
{
    // aura can be deleted at casts
    SpellInfo const* spellProto = triggeredByAura->GetSpellInfo();

    uint32 damageSpellId;
    switch (spellProto->Id)
    {
        case 57949:            // shiver
            damageSpellId = 57952;
            //animationSpellId = 57951; dummy effects for jump spell have unknown use (see also 41637)
            break;
        case 59978:            // shiver
            damageSpellId = 59979;
            break;
        case 43593:            // Cold Stare
            damageSpellId = 43594;
            break;
        default:
            return false;
    }

    uint64 caster_guid = triggeredByAura->GetCasterGUID();

    // jumps
    int32 jumps = triggeredByAura->GetBase()->GetCharges()-1;

    // current aura expire
    triggeredByAura->GetBase()->SetCharges(1);             // will removed at next charges decrease

    // next target selection
    if (jumps > 0)
    {
        if (Unit* caster = triggeredByAura->GetCaster())
        {
            float radius = triggeredByAura->GetSpellInfo()->Effects[triggeredByAura->GetEffIndex()].CalcRadius(caster, NULL, false);
            if (Unit* target= GetNextRandomRaidMemberOrPet(radius))
            {
                CastSpell(target, spellProto, true, NULL, triggeredByAura, caster_guid);
                if (Aura* aura = target->GetAura(spellProto->Id, caster->GetGUID()))
                    aura->SetCharges(jumps);
            }
        }
    }

    CastSpell(this, damageSpellId, true, NULL, triggeredByAura, caster_guid);

    return true;
}

void Unit::SendDurabilityLoss(Player* receiver, uint32 percent)
{
    WorldPacket data(SMSG_DURABILITY_DAMAGE_DEATH, 4);
    data << uint32(percent);
    receiver->GetSession()->SendPacket(&data);
}

void Unit::PlayOneShotAnimKit(uint32 id)
{
    WorldPacket data(SMSG_PLAY_ONE_SHOT_ANIM_KIT, 7+2);
    data.appendPackGUID(GetGUID());
    data << uint16(id);
    SendMessageToSet(&data, true);
}

void Unit::Kill(Unit* victim, bool durabilityLoss)
{
    // Prevent killing unit twice (and giving reward from kill twice)
    if (!victim->GetHealth())
        return;

    // Spirit of Redemption can't be killed twice
    if (victim->HasAura(27827))
        return;

    // find player: owner of controlled `this` or `this` itself maybe
    Player* player = GetCharmerOrOwnerPlayerOrPlayerItself();
    Creature* creature = victim->ToCreature();

    bool isRewardAllowed = true;
    if (creature)
    {
        isRewardAllowed = creature->IsDamageEnoughForLootingAndReward();
        if (!isRewardAllowed)
            creature->SetLootRecipient(NULL);
    }

    if (isRewardAllowed && creature && creature->GetLootRecipient())
        player = creature->GetLootRecipient();

    // Reward player, his pets, and group/raid members
    // call kill spell proc event (before real die and combat stop to triggering auras removed at death/combat stop)
    if (isRewardAllowed && player && player != victim)
    {
        WorldPacket data(SMSG_PARTYKILLLOG, (8+8)); // send event PARTY_KILL
        data << uint64(player->GetGUID()); // player with killing blow
        data << uint64(victim->GetGUID()); // victim

        Player* looter = player;

        if (Group* group = player->GetGroup())
        {
            group->BroadcastPacket(&data, group->GetMemberGroup(player->GetGUID()));

            if (creature)
            {
                group->UpdateLooterGuid(creature, true);
                if (group->GetLooterGuid())
                {
                    looter = ObjectAccessor::FindPlayer(group->GetLooterGuid());
                    if (looter)
                    {
                        creature->SetLootRecipient(looter);   // update creature loot recipient to the allowed looter.
                        group->SendLooter(creature, looter);
                    }
                    else
                        group->SendLooter(creature, NULL);
                }
                else
                    group->SendLooter(creature, NULL);

                group->UpdateLooterGuid(creature);
            }
        }
        else
        {
            player->SendDirectMessage(&data);

            if (creature)
            {
                WorldPacket data2(SMSG_LOOT_LIST, 8 + 1 + 1);
                data2 << uint64(creature->GetGUID());
                data2 << uint8(0); // unk1
                data2 << uint8(0); // no group looter
                player->SendMessageToSet(&data2, true);
            }
        }

        if (creature)
        {
            Loot* loot = &creature->loot;
            if (creature->lootForPickPocketed)
                creature->lootForPickPocketed = false;

            loot->clear();
            if (uint32 lootid = creature->GetCreatureTemplate()->lootid)
                loot->FillLoot(lootid, LootTemplates_Creature, looter, false, false, creature->GetLootMode());

            loot->generateMoneyLoot(creature->GetCreatureTemplate()->mingold, creature->GetCreatureTemplate()->maxgold);
        }

        player->RewardPlayerAndGroupAtKill(victim, false);
    }

    // Do KILL and KILLED procs. KILL proc is called only for the unit who landed the killing blow (and its owner - for pets and totems) regardless of who tapped the victim
    if (isPet() || isTotem())
        if (Unit* owner = GetOwner())
            owner->ProcDamageAndSpell(victim, PROC_FLAG_KILL, PROC_FLAG_NONE, PROC_EX_NONE, 0);

    if (victim->GetCreatureType() != CREATURE_TYPE_CRITTER)
    {
        if (victim->GetEntry() != 19833 && victim->GetEntry() != 19921) // snake trap can't trigger PROC_FLAG_KILL
            ProcDamageAndSpell(victim, PROC_FLAG_KILL, PROC_FLAG_KILLED, PROC_EX_NONE, 0);
    }

    // Proc auras on death - must be before aura/combat remove
    victim->ProcDamageAndSpell(NULL, PROC_FLAG_DEATH, PROC_FLAG_NONE, PROC_EX_NONE, 0, 0, BASE_ATTACK, 0);

    // update get killing blow achievements, must be done before setDeathState to be able to require auras on target
    // and before Spirit of Redemption as it also removes auras
    if (player)
        player->UpdateAchievementCriteria(ACHIEVEMENT_CRITERIA_TYPE_GET_KILLING_BLOWS, 1, 0, 0, victim);

    // if talent known but not triggered (check priest class for speedup check)
    bool spiritOfRedemption = false;
    if (victim->GetTypeId() == TYPEID_PLAYER && victim->getClass() == CLASS_PRIEST)
    {
        AuraEffectList const& dummyAuras = victim->GetAuraEffectsByType(SPELL_AURA_DUMMY);
        for (AuraEffectList::const_iterator itr = dummyAuras.begin(); itr != dummyAuras.end(); ++itr)
        {
            if ((*itr)->GetSpellInfo()->SpellIconID == 1654)
            {
                AuraEffect const* aurEff = *itr;
                // save value before aura remove
                uint32 ressSpellId = victim->GetUInt32Value(PLAYER_SELF_RES_SPELL);
                if (!ressSpellId)
                    ressSpellId = victim->ToPlayer()->GetResurrectionSpellId();
                // Remove all expected to remove at death auras (most important negative case like DoT or periodic triggers)
                victim->RemoveAllAurasOnDeath();
                // restore for use at real death
                victim->SetUInt32Value(PLAYER_SELF_RES_SPELL, ressSpellId);

                // FORM_SPIRITOFREDEMPTION and related auras
                victim->CastSpell(victim, 27827, true, NULL, aurEff);
                victim->CastSpell(victim, 27792, true);
                spiritOfRedemption = true;
                break;
            }
        }
    }

    if (victim->GetTypeId() == TYPEID_UNIT)
        victim->SetUInt32Value(UNIT_NPC_EMOTESTATE, 0);
    if (!spiritOfRedemption)
        victim->setDeathState(JUST_DIED);

    // Inform pets (if any) when player kills target)
    // MUST come after victim->setDeathState(JUST_DIED); or pet next target
    // selection will get stuck on same target and break pet react state
    if (player)
    {
        Pet* pet = player->GetPet();
        if (pet && pet->isAlive() && pet->isControlled())
            pet->AI()->KilledUnit(victim);
    }

    // 10% durability loss on death
    // clean InHateListOf
    if (Player* plrVictim = victim->ToPlayer())
    {
        // remember victim PvP death for corpse type and corpse reclaim delay
        // at original death (not at SpiritOfRedemtionTalent timeout)
        plrVictim->SetPvPDeath(player != NULL);

        // only if not player and not controlled by player pet. And not at BG
        if ((durabilityLoss && !player && !victim->ToPlayer()->InBattleground()) || (player && sWorld->getBoolConfig(CONFIG_DURABILITY_LOSS_IN_PVP)))
        {
            double baseLoss = sWorld->getRate(RATE_DURABILITY_LOSS_ON_DEATH);
            uint32 loss = uint32(baseLoss - (baseLoss * plrVictim->GetTotalAuraMultiplier(SPELL_AURA_MOD_DURABILITY_LOSS)));
            // Durability loss is calculated more accurately again for each item in Player::DurabilityLoss
            plrVictim->DurabilityLossAll(baseLoss, false);
            // durability lost message
            SendDurabilityLoss(plrVictim, loss);
        }
        // Call KilledUnit for creatures
        if (GetTypeId() == TYPEID_UNIT && IsAIEnabled)
            ToCreature()->AI()->KilledUnit(victim);

        // last damage from non duel opponent or opponent controlled creature
        if (plrVictim->duel)
        {
            plrVictim->duel->opponent->CombatStopWithPets(true);
            plrVictim->CombatStopWithPets(true);
            plrVictim->DuelComplete(DUEL_INTERRUPTED);
        }
    }
    else                                                // creature died
    {
        if (!creature->isPet())
        {
            creature->DeleteThreatList();
            CreatureTemplate const* cInfo = creature->GetCreatureTemplate();
            if (cInfo && (cInfo->lootid || cInfo->maxgold > 0))
                creature->SetFlag(UNIT_DYNAMIC_FLAGS, UNIT_DYNFLAG_LOOTABLE);
        }

        // Call KilledUnit for creatures, this needs to be called after the lootable flag is set
        if (GetTypeId() == TYPEID_UNIT && IsAIEnabled)
            ToCreature()->AI()->KilledUnit(victim);

        // Call creature just died function
        if (creature->IsAIEnabled)
            creature->AI()->JustDied(this);

        if (TempSummon* summon = creature->ToTempSummon())
            if (Unit* summoner = summon->GetSummoner())
                if (summoner->ToCreature() && summoner->IsAIEnabled)
                    summoner->ToCreature()->AI()->SummonedCreatureDies(creature, this);

        // Dungeon specific stuff, only applies to players killing creatures
        if (creature->GetInstanceId())
        {
            Map* instanceMap = creature->GetMap();
            Player* creditedPlayer = GetCharmerOrOwnerPlayerOrPlayerItself();
            // TODO: do instance binding anyway if the charmer/owner is offline

            if (instanceMap->IsDungeon() && creditedPlayer)
            {
                if (instanceMap->IsRaidOrHeroicDungeon())
                {
                    if (creature->GetCreatureTemplate()->flags_extra & CREATURE_FLAG_EXTRA_INSTANCE_BIND)
                        ((InstanceMap*)instanceMap)->PermBindAllPlayers(creditedPlayer);
                }
                else
                {
                    // the reset time is set but not added to the scheduler
                    // until the players leave the instance
                    time_t resettime = creature->GetRespawnTimeEx() + 2 * HOUR;
                    if (InstanceSave* save = sInstanceSaveMgr->GetInstanceSave(creature->GetInstanceId()))
                        if (save->GetResetTime() < resettime) save->SetResetTime(resettime);
                }
            }
        }
    }

    // outdoor pvp things, do these after setting the death state, else the player activity notify won't work... doh...
    // handle player kill only if not suicide (spirit of redemption for example)
    if (player && this != victim)
    {
        if (OutdoorPvP* pvp = player->GetOutdoorPvP())
            pvp->HandleKill(player, victim);

        if (Battlefield* bf = sBattlefieldMgr->GetBattlefieldToZoneId(player->GetZoneId()))
            bf->HandleKill(player, victim);
    }

    //if (victim->GetTypeId() == TYPEID_PLAYER)
    //    if (OutdoorPvP* pvp = victim->ToPlayer()->GetOutdoorPvP())
    //        pvp->HandlePlayerActivityChangedpVictim->ToPlayer();

    // battleground things (do this at the end, so the death state flag will be properly set to handle in the bg->handlekill)
    if (player && player->InBattleground())
    {
        if (Battleground* bg = player->GetBattleground())
        {
            if (victim->GetTypeId() == TYPEID_PLAYER)
                bg->HandleKillPlayer((Player*)victim, player);
            else
                bg->HandleKillUnit(victim->ToCreature(), player);
        }
    }

    // achievement stuff
    if (victim->GetTypeId() == TYPEID_PLAYER)
    {
        if (GetTypeId() == TYPEID_UNIT)
            victim->ToPlayer()->UpdateAchievementCriteria(ACHIEVEMENT_CRITERIA_TYPE_KILLED_BY_CREATURE, GetEntry());
        else if (GetTypeId() == TYPEID_PLAYER && victim != this)
            victim->ToPlayer()->UpdateAchievementCriteria(ACHIEVEMENT_CRITERIA_TYPE_KILLED_BY_PLAYER, 1, ToPlayer()->GetTeam());
    }

    // Hook for OnPVPKill Event
    if (Player* killerPlr = ToPlayer())
    {
        if (Player* killedPlr = victim->ToPlayer())
            sScriptMgr->OnPVPKill(killerPlr, killedPlr);
        else if (Creature* killedCre = victim->ToCreature())
            sScriptMgr->OnCreatureKill(killerPlr, killedCre);
    }
    else if (Creature* killerCre = ToCreature())
    {
        if (Player* killed = victim->ToPlayer())
            sScriptMgr->OnPlayerKilledByCreature(killerCre, killed);
    }
}

void Unit::SetControlled(bool apply, UnitState state)
{
    if (apply)
    {
        if (HasUnitState(state))
            return;

        AddUnitState(state);
        switch (state)
        {
            case UNIT_STATE_STUNNED:
                SetStunned(true);
                CastStop();
                break;
            case UNIT_STATE_ROOT:
                if (!HasUnitState(UNIT_STATE_STUNNED))
                    SetRooted(true);
                break;
            case UNIT_STATE_CONFUSED:
                if (!HasUnitState(UNIT_STATE_STUNNED))
                {
                    ClearUnitState(UNIT_STATE_MELEE_ATTACKING);
                    SendMeleeAttackStop();
                    // SendAutoRepeatCancel ?
                    SetConfused(true);
                    CastStop();
                }
                break;
            case UNIT_STATE_FLEEING:
                if (!HasUnitState(UNIT_STATE_STUNNED | UNIT_STATE_CONFUSED))
                {
                    ClearUnitState(UNIT_STATE_MELEE_ATTACKING);
                    SendMeleeAttackStop();
                    // SendAutoRepeatCancel ?
                    SetFeared(true);
                    CastStop();
                }
                break;
            default:
                break;
        }
    }
    else
    {
        switch (state)
        {
            case UNIT_STATE_STUNNED: if (HasAuraType(SPELL_AURA_MOD_STUN))    return;
                                    else    SetStunned(false);    break;
            case UNIT_STATE_ROOT:    if (HasAuraType(SPELL_AURA_MOD_ROOT) || GetVehicle())    return;
                                    else    SetRooted(false);     break;
            case UNIT_STATE_CONFUSED:if (HasAuraType(SPELL_AURA_MOD_CONFUSE)) return;
                                    else    SetConfused(false);   break;
            case UNIT_STATE_FLEEING: if (HasAuraType(SPELL_AURA_MOD_FEAR))    return;
                                    else    SetFeared(false);     break;
            default: return;
        }

        ClearUnitState(state);

        if (HasUnitState(UNIT_STATE_STUNNED))
            SetStunned(true);
        else
        {
            if (HasUnitState(UNIT_STATE_ROOT))
                SetRooted(true);

            if (HasUnitState(UNIT_STATE_CONFUSED))
                SetConfused(true);
            else if (HasUnitState(UNIT_STATE_FLEEING))
                SetFeared(true);
        }
    }
}

void Unit::SendMoveRoot(uint32 value)
{
    ObjectGuid guid = GetGUID();
    WorldPacket data(SMSG_MOVE_ROOT, 1 + 8 + 4);
    data.WriteByteMask(guid[2]);
    data.WriteByteMask(guid[7]);
    data.WriteByteMask(guid[6]);
    data.WriteByteMask(guid[0]);
    data.WriteByteMask(guid[5]);
    data.WriteByteMask(guid[4]);
    data.WriteByteMask(guid[1]);
    data.WriteByteMask(guid[3]);

    data.WriteByteSeq(guid[1]);
    data.WriteByteSeq(guid[0]);
    data.WriteByteSeq(guid[2]);
    data.WriteByteSeq(guid[5]);

    data << uint32(value);

    data.WriteByteSeq(guid[3]);
    data.WriteByteSeq(guid[4]);
    data.WriteByteSeq(guid[7]);
    data.WriteByteSeq(guid[6]);

    SendMessageToSet(&data, true);
}

void Unit::SendMoveUnroot(uint32 value)
{
    ObjectGuid guid = GetGUID();
    WorldPacket data(SMSG_MOVE_UNROOT, 1 + 8 + 4);
    data.WriteByteMask(guid[0]);
    data.WriteByteMask(guid[1]);
    data.WriteByteMask(guid[3]);
    data.WriteByteMask(guid[7]);
    data.WriteByteMask(guid[5]);
    data.WriteByteMask(guid[2]);
    data.WriteByteMask(guid[4]);
    data.WriteByteMask(guid[6]);

    data.WriteByteSeq(guid[3]);
    data.WriteByteSeq(guid[6]);
    data.WriteByteSeq(guid[1]);

    data << uint32(value);

    data.WriteByteSeq(guid[2]);
    data.WriteByteSeq(guid[0]);
    data.WriteByteSeq(guid[7]);
    data.WriteByteSeq(guid[4]);
    data.WriteByteSeq(guid[5]);

    SendMessageToSet(&data, true);
}

void Unit::SetStunned(bool apply)
{
    ObjectGuid guid = GetGUID();

    if (apply)
    {
        SetTarget(0);
        SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_STUNNED);

        // MOVEMENTFLAG_ROOT cannot be used in conjunction with MOVEMENTFLAG_MASK_MOVING (tested 3.3.5a)
        // this will freeze clients. That's why we remove MOVEMENTFLAG_MASK_MOVING before
        // setting MOVEMENTFLAG_ROOT
        RemoveUnitMovementFlag(MOVEMENTFLAG_MASK_MOVING);
        AddUnitMovementFlag(MOVEMENTFLAG_ROOT);

        // Creature specific
        if (GetTypeId() != TYPEID_PLAYER)
            ToCreature()->StopMoving();
        else
            SetStandState(UNIT_STAND_STATE_STAND);

        if (GetTypeId() == TYPEID_PLAYER)
            SendMoveRoot(0);
        else
        {
            WorldPacket data(SMSG_SPLINE_MOVE_ROOT, 8);
            {
                data
                    .WriteByteMask(guid[5])
                    .WriteByteMask(guid[4])
                    .WriteByteMask(guid[6])
                    .WriteByteMask(guid[1])
                    .WriteByteMask(guid[3])
                    .WriteByteMask(guid[7])
                    .WriteByteMask(guid[2])
                    .WriteByteMask(guid[0])
                    .WriteByteSeq(guid[2])
                    .WriteByteSeq(guid[1])
                    .WriteByteSeq(guid[7])
                    .WriteByteSeq(guid[3])
                    .WriteByteSeq(guid[5])
                    .WriteByteSeq(guid[0])
                    .WriteByteSeq(guid[6])
                    .WriteByteSeq(guid[4]);
            }
        }

        CastStop();
    }
    else
    {
        if (isAlive() && getVictim())
            SetTarget(getVictim()->GetGUID());

        // don't remove UNIT_FLAG_STUNNED for pet when owner is mounted (disabled pet's interface)
        Unit* owner = GetOwner();
        if (!owner || (owner->GetTypeId() == TYPEID_PLAYER && !owner->ToPlayer()->IsMounted()))
            RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_STUNNED);

        if (!HasUnitState(UNIT_STATE_ROOT))         // prevent moving if it also has root effect
        {
            if (GetTypeId() == TYPEID_PLAYER)
                SendMoveUnroot(0);
            else
            {
                WorldPacket data(SMSG_SPLINE_MOVE_UNROOT, 8);
                data
                    .WriteByteMask(guid[0])
                    .WriteByteMask(guid[1])
                    .WriteByteMask(guid[6])
                    .WriteByteMask(guid[5])
                    .WriteByteMask(guid[3])
                    .WriteByteMask(guid[2])
                    .WriteByteMask(guid[7])
                    .WriteByteMask(guid[4])
                    .WriteByteSeq(guid[6])
                    .WriteByteSeq(guid[3])
                    .WriteByteSeq(guid[1])
                    .WriteByteSeq(guid[5])
                    .WriteByteSeq(guid[2])
                    .WriteByteSeq(guid[0])
                    .WriteByteSeq(guid[7])
                    .WriteByteSeq(guid[4]);
                SendMessageToSet(&data, true);
            }

            RemoveUnitMovementFlag(MOVEMENTFLAG_ROOT);
        }
    }
}

void Unit::SetRooted(bool apply)
{
    ObjectGuid guid = GetGUID();

    if (apply)
    {
        if (m_rootTimes > 0) // blizzard internal check?
            m_rootTimes++;

        // MOVEMENTFLAG_ROOT cannot be used in conjunction with MOVEMENTFLAG_MASK_MOVING (tested 3.3.5a)
        // this will freeze clients. That's why we remove MOVEMENTFLAG_MASK_MOVING before
        // setting MOVEMENTFLAG_ROOT
        RemoveUnitMovementFlag(MOVEMENTFLAG_MASK_MOVING);
        AddUnitMovementFlag(MOVEMENTFLAG_ROOT);

        if (GetTypeId() == TYPEID_PLAYER)
            SendMoveRoot(m_rootTimes);
        else
        {

            WorldPacket data(SMSG_SPLINE_MOVE_ROOT, 8);
            {
                data
                    .WriteByteMask(guid[5])
                    .WriteByteMask(guid[4])
                    .WriteByteMask(guid[6])
                    .WriteByteMask(guid[1])
                    .WriteByteMask(guid[3])
                    .WriteByteMask(guid[7])
                    .WriteByteMask(guid[2])
                    .WriteByteMask(guid[0])
                    .WriteByteSeq(guid[2])
                    .WriteByteSeq(guid[1])
                    .WriteByteSeq(guid[7])
                    .WriteByteSeq(guid[3])
                    .WriteByteSeq(guid[5])
                    .WriteByteSeq(guid[0])
                    .WriteByteSeq(guid[6])
                    .WriteByteSeq(guid[4]);
            }
            SendMessageToSet(&data, true);
            StopMoving();
        }
    }
    else
    {
        if (!HasUnitState(UNIT_STATE_STUNNED))      // prevent moving if it also has stun effect
        {
            if (GetTypeId() == TYPEID_PLAYER)
                SendMoveUnroot(++m_rootTimes);
            else
            {
                WorldPacket data(SMSG_SPLINE_MOVE_UNROOT, 8);
                data
                    .WriteByteMask(guid[0])
                    .WriteByteMask(guid[1])
                    .WriteByteMask(guid[6])
                    .WriteByteMask(guid[5])
                    .WriteByteMask(guid[3])
                    .WriteByteMask(guid[2])
                    .WriteByteMask(guid[7])
                    .WriteByteMask(guid[4])
                    .WriteByteSeq(guid[6])
                    .WriteByteSeq(guid[3])
                    .WriteByteSeq(guid[1])
                    .WriteByteSeq(guid[5])
                    .WriteByteSeq(guid[2])
                    .WriteByteSeq(guid[0])
                    .WriteByteSeq(guid[7])
                    .WriteByteSeq(guid[4]);
                SendMessageToSet(&data, true);
            }

            RemoveUnitMovementFlag(MOVEMENTFLAG_ROOT);
        }
    }
}

void Unit::SetFeared(bool apply)
{
    if (apply)
    {
        SetTarget(0);

        Unit* caster = NULL;
        Unit::AuraEffectList const& fearAuras = GetAuraEffectsByType(SPELL_AURA_MOD_FEAR);
        if (!fearAuras.empty())
            caster = ObjectAccessor::GetUnit(*this, fearAuras.front()->GetCasterGUID());
        if (!caster)
            caster = getAttackerForHelper();
        GetMotionMaster()->MoveFleeing(caster, fearAuras.empty() ? sWorld->getIntConfig(CONFIG_CREATURE_FAMILY_FLEE_DELAY) : 0);             // caster == NULL processed in MoveFleeing
    }
    else
    {
        if (isAlive())
        {
            if (GetMotionMaster()->GetCurrentMovementGeneratorType() == FLEEING_MOTION_TYPE)
                GetMotionMaster()->MovementExpired();
            if (getVictim())
                SetTarget(getVictim()->GetGUID());
        }
    }

    if (GetTypeId() == TYPEID_PLAYER)
        ToPlayer()->SetClientControl(this, !apply);
}

void Unit::SetConfused(bool apply)
{
    if (apply)
    {
        SetTarget(0);
        GetMotionMaster()->MoveConfused();
    }
    else
    {
        if (isAlive())
        {
            if (GetMotionMaster()->GetCurrentMovementGeneratorType() == CONFUSED_MOTION_TYPE)
                GetMotionMaster()->MovementExpired();
            if (getVictim())
                SetTarget(getVictim()->GetGUID());
        }
    }

    if (GetTypeId() == TYPEID_PLAYER)
        ToPlayer()->SetClientControl(this, !apply);
}

bool Unit::SetCharmedBy(Unit* charmer, CharmType type, AuraApplication const* aurApp)
{
    if (!charmer)
        return false;

    // dismount players when charmed
    if (GetTypeId() == TYPEID_PLAYER)
        Dismount();

    ASSERT(type != CHARM_TYPE_POSSESS || charmer->GetTypeId() == TYPEID_PLAYER);
    ASSERT((type == CHARM_TYPE_VEHICLE) == IsVehicle());

    if (this == charmer)
    {
        sLog->outFatal(LOG_FILTER_UNITS, "Unit::SetCharmedBy: Unit %u (GUID %u) is trying to charm itself!", GetEntry(), GetGUIDLow());
        return false;
    }

    //if (HasUnitState(UNIT_STATE_UNATTACKABLE))
    //    return false;

    if (GetTypeId() == TYPEID_PLAYER && ToPlayer()->GetTransport())
    {
        sLog->outFatal(LOG_FILTER_UNITS, "Unit::SetCharmedBy: Player on transport is trying to charm %u (GUID %u)", GetEntry(), GetGUIDLow());
        return false;
    }

    // Already charmed
    if (GetCharmerGUID())
    {
        sLog->outFatal(LOG_FILTER_UNITS, "Unit::SetCharmedBy: %u (GUID %u) has already been charmed but %u (GUID %u) is trying to charm it!", GetEntry(), GetGUIDLow(), charmer->GetEntry(), charmer->GetGUIDLow());
        return false;
    }

    CastStop();
    CombatStop(); // TODO: CombatStop(true) may cause crash (interrupt spells)
    DeleteThreatList();

    // Charmer stop charming
    if (charmer->GetTypeId() == TYPEID_PLAYER)
    {
        charmer->ToPlayer()->StopCastingCharm();
        charmer->ToPlayer()->StopCastingBindSight();
    }

    // Charmed stop charming
    if (GetTypeId() == TYPEID_PLAYER)
    {
        ToPlayer()->StopCastingCharm();
        ToPlayer()->StopCastingBindSight();
    }

    // StopCastingCharm may remove a possessed pet?
    if (!IsInWorld())
    {
        sLog->outFatal(LOG_FILTER_UNITS, "Unit::SetCharmedBy: %u (GUID %u) is not in world but %u (GUID %u) is trying to charm it!", GetEntry(), GetGUIDLow(), charmer->GetEntry(), charmer->GetGUIDLow());
        return false;
    }

    // charm is set by aura, and aura effect remove handler was called during apply handler execution
    // prevent undefined behaviour
    if (aurApp && aurApp->GetRemoveMode())
        return false;

    // Set charmed
    Map* map = GetMap();
    if (!IsVehicle() || (IsVehicle() && map && !map->IsBattleground()))
        setFaction(charmer->getFaction());

    charmer->SetCharm(this, true);

    if (GetTypeId() == TYPEID_UNIT)
    {
        ToCreature()->AI()->OnCharmed(true);
        GetMotionMaster()->MoveIdle();
    }
    else
    {
        Player* player = ToPlayer();
        if (player->isAFK())
            player->ToggleAFK();
        player->SetClientControl(this, 0);
    }

    // charm is set by aura, and aura effect remove handler was called during apply handler execution
    // prevent undefined behaviour
    if (aurApp && aurApp->GetRemoveMode())
        return false;

    // Pets already have a properly initialized CharmInfo, don't overwrite it.
    if (type != CHARM_TYPE_VEHICLE && !GetCharmInfo())
    {
        InitCharmInfo();
        if (type == CHARM_TYPE_POSSESS)
            GetCharmInfo()->InitPossessCreateSpells();
        else
            GetCharmInfo()->InitCharmCreateSpells();
    }

    if (charmer->GetTypeId() == TYPEID_PLAYER)
    {
        switch (type)
        {
            case CHARM_TYPE_VEHICLE:
                SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PLAYER_CONTROLLED);
                charmer->ToPlayer()->SetClientControl(this, 1);
                charmer->ToPlayer()->SetMover(this);
                charmer->ToPlayer()->SetViewpoint(this, true);
                charmer->ToPlayer()->VehicleSpellInitialize();
                break;
            case CHARM_TYPE_POSSESS:
                AddUnitState(UNIT_STATE_POSSESSED);
                SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PLAYER_CONTROLLED);
                charmer->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_DISABLE_MOVE);
                charmer->ToPlayer()->SetClientControl(this, 1);
                charmer->ToPlayer()->SetMover(this);
                charmer->ToPlayer()->SetViewpoint(this, true);
                charmer->ToPlayer()->PossessSpellInitialize();
                break;
            case CHARM_TYPE_CHARM:
                if (GetTypeId() == TYPEID_UNIT && charmer->getClass() == CLASS_WARLOCK)
                {
                    CreatureTemplate const* cinfo = ToCreature()->GetCreatureTemplate();
                    if (cinfo && cinfo->type == CREATURE_TYPE_DEMON)
                    {
                        // to prevent client crash
                        SetByteValue(UNIT_FIELD_BYTES_0, 1, (uint8)CLASS_MAGE);

                        // just to enable stat window
                        if (GetCharmInfo())
                            GetCharmInfo()->SetPetNumber(sObjectMgr->GeneratePetNumber(), true);

                        // if charmed two demons the same session, the 2nd gets the 1st one's name
                        SetUInt32Value(UNIT_FIELD_PET_NAME_TIMESTAMP, uint32(time(NULL))); // cast can't be helped
                    }
                }
                charmer->ToPlayer()->CharmSpellInitialize();
                break;
            default:
            case CHARM_TYPE_CONVERT:
                break;
        }
    }
    return true;
}

void Unit::RemoveCharmedBy(Unit* charmer)
{
    if (!isCharmed())
        return;

    if (!charmer)
        charmer = GetCharmer();
    if (charmer != GetCharmer()) // one aura overrides another?
    {
//        sLog->outFatal(LOG_FILTER_UNITS, "Unit::RemoveCharmedBy: this: " UI64FMTD " true charmer: " UI64FMTD " false charmer: " UI64FMTD,
//            GetGUID(), GetCharmerGUID(), charmer->GetGUID());
//        ASSERT(false);
        return;
    }

    CharmType type;
    if (HasUnitState(UNIT_STATE_POSSESSED))
        type = CHARM_TYPE_POSSESS;
    else if (charmer && charmer->IsOnVehicle(this))
        type = CHARM_TYPE_VEHICLE;
    else
        type = CHARM_TYPE_CHARM;

    CastStop();
    CombatStop(); // TODO: CombatStop(true) may cause crash (interrupt spells)
    getHostileRefManager().deleteReferences();
    DeleteThreatList();
    Map* map = GetMap();
    if (!IsVehicle() || (IsVehicle() && map && !map->IsBattleground()))
        RestoreFaction();
    GetMotionMaster()->InitDefault();

    if (type == CHARM_TYPE_POSSESS)
    {
        ClearUnitState(UNIT_STATE_POSSESSED);
        RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PLAYER_CONTROLLED);
    }

    if (Creature* creature = ToCreature())
    {
        if (creature->AI())
            creature->AI()->OnCharmed(false);

        if (type != CHARM_TYPE_VEHICLE) // Vehicles' AI is never modified
        {
            creature->AIM_Initialize();

            if (creature->AI() && charmer && charmer->isAlive())
                creature->AI()->AttackStart(charmer);
        }
    }
    else
        ToPlayer()->SetClientControl(this, 1);

    // If charmer still exists
    if (!charmer)
        return;

    ASSERT(type != CHARM_TYPE_POSSESS || charmer->GetTypeId() == TYPEID_PLAYER);
    ASSERT(type != CHARM_TYPE_VEHICLE || (GetTypeId() == TYPEID_UNIT && IsVehicle()));

    charmer->SetCharm(this, false);

    if (charmer->GetTypeId() == TYPEID_PLAYER)
    {
        switch (type)
        {
            case CHARM_TYPE_VEHICLE:
                charmer->ToPlayer()->SetClientControl(charmer, 1);
                charmer->ToPlayer()->SetViewpoint(this, false);
                charmer->ToPlayer()->SetClientControl(this, 0);
                if (GetTypeId() == TYPEID_PLAYER)
                    ToPlayer()->SetMover(this);
                break;
            case CHARM_TYPE_POSSESS:
                ClearUnitState(UNIT_STATE_POSSESSED);
                charmer->ToPlayer()->SetClientControl(charmer, 1);
                charmer->ToPlayer()->SetViewpoint(this, false);
                charmer->ToPlayer()->SetClientControl(this, 0);
                charmer->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_DISABLE_MOVE);
                if (GetTypeId() == TYPEID_PLAYER)
                    ToPlayer()->SetClientControl(this, 1);
                break;
            case CHARM_TYPE_CHARM:
                if (GetTypeId() == TYPEID_UNIT && charmer->getClass() == CLASS_WARLOCK)
                {
                    CreatureTemplate const* cinfo = ToCreature()->GetCreatureTemplate();
                    if (cinfo && cinfo->type == CREATURE_TYPE_DEMON)
                    {
                        SetByteValue(UNIT_FIELD_BYTES_0, 1, uint8(cinfo->unit_class));
                        if (GetCharmInfo())
                            GetCharmInfo()->SetPetNumber(0, true);
                        else
                            sLog->outError(LOG_FILTER_UNITS, "Aura::HandleModCharm: target="UI64FMTD" with typeid=%d has a charm aura but no charm info!", GetGUID(), GetTypeId());
                    }
                }
                break;
            default:
            case CHARM_TYPE_CONVERT:
                break;
        }
    }

    // a guardian should always have charminfo
    if (charmer->GetTypeId() == TYPEID_PLAYER && this != charmer->GetFirstControlled())
        charmer->ToPlayer()->SendRemoveControlBar();
    else if (GetTypeId() == TYPEID_PLAYER || (GetTypeId() == TYPEID_UNIT && !ToCreature()->isGuardian()))
        DeleteCharmInfo();
}

void Unit::RestoreFaction()
{
    if (GetTypeId() == TYPEID_PLAYER)
        ToPlayer()->setFactionForRace(getRace());
    else
    {
        if (HasUnitTypeMask(UNIT_MASK_MINION))
        {
            if (Unit* owner = GetOwner())
            {
                setFaction(owner->getFaction());
                return;
            }
        }

        if (CreatureTemplate const* cinfo = ToCreature()->GetCreatureTemplate())  // normal creature
        {
            FactionTemplateEntry const* faction = getFactionTemplateEntry();
            setFaction((faction && faction->friendlyMask & 0x004) ? cinfo->faction_H : cinfo->faction_A);
        }
    }
}

bool Unit::CreateVehicleKit(uint32 id, uint32 creatureEntry)
{
    VehicleEntry const* vehInfo = sVehicleStore.LookupEntry(id);
    if (!vehInfo)
        return false;

    m_vehicleKit = new Vehicle(this, vehInfo, creatureEntry);
    m_updateFlag |= UPDATEFLAG_VEHICLE;
    m_unitTypeMask |= UNIT_MASK_VEHICLE;
    return true;
}

void Unit::RemoveVehicleKit()
{
    if (!m_vehicleKit)
        return;

    m_vehicleKit->Uninstall();
    delete m_vehicleKit;

    m_vehicleKit = NULL;

    m_updateFlag &= ~UPDATEFLAG_VEHICLE;
    m_unitTypeMask &= ~UNIT_MASK_VEHICLE;
    RemoveFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_SPELLCLICK);
    RemoveFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_PLAYER_VEHICLE);
}

Unit* Unit::GetVehicleBase() const
{
    return m_vehicle ? m_vehicle->GetBase() : NULL;
}

Creature* Unit::GetVehicleCreatureBase() const
{
    if (Unit* veh = GetVehicleBase())
        if (Creature* c = veh->ToCreature())
            return c;

    return NULL;
}

uint64 Unit::GetTransGUID() const
{
    if (GetVehicle())
        return GetVehicleBase()->GetGUID();
    if (GetTransport())
        return GetTransport()->GetGUID();

    return 0;
}

TransportBase* Unit::GetDirectTransport() const
{
    if (Vehicle* veh = GetVehicle())
        return veh;
    return GetTransport();
}

bool Unit::IsInPartyWith(Unit const* unit) const
{
    if (this == unit)
        return true;

    const Unit* u1 = GetCharmerOrOwnerOrSelf();
    const Unit* u2 = unit->GetCharmerOrOwnerOrSelf();
    if (u1 == u2)
        return true;

    if (u1->GetTypeId() == TYPEID_PLAYER && u2->GetTypeId() == TYPEID_PLAYER)
        return u1->ToPlayer()->IsInSameGroupWith(u2->ToPlayer());
    else if ((u2->GetTypeId() == TYPEID_PLAYER && u1->GetTypeId() == TYPEID_UNIT && u1->ToCreature()->GetCreatureTemplate()->type_flags & CREATURE_TYPEFLAGS_PARTY_MEMBER) ||
        (u1->GetTypeId() == TYPEID_PLAYER && u2->GetTypeId() == TYPEID_UNIT && u2->ToCreature()->GetCreatureTemplate()->type_flags & CREATURE_TYPEFLAGS_PARTY_MEMBER))
        return true;
    else
        return false;
}

bool Unit::IsInRaidWith(Unit const* unit) const
{
    if (this == unit)
        return true;

    const Unit* u1 = GetCharmerOrOwnerOrSelf();
    const Unit* u2 = unit->GetCharmerOrOwnerOrSelf();
    if (u1 == u2)
        return true;

    if (u1->GetTypeId() == TYPEID_PLAYER && u2->GetTypeId() == TYPEID_PLAYER)
        return u1->ToPlayer()->IsInSameRaidWith(u2->ToPlayer());
    else if ((u2->GetTypeId() == TYPEID_PLAYER && u1->GetTypeId() == TYPEID_UNIT && u1->ToCreature()->GetCreatureTemplate()->type_flags & CREATURE_TYPEFLAGS_PARTY_MEMBER) ||
            (u1->GetTypeId() == TYPEID_PLAYER && u2->GetTypeId() == TYPEID_UNIT && u2->ToCreature()->GetCreatureTemplate()->type_flags & CREATURE_TYPEFLAGS_PARTY_MEMBER))
        return true;
    else
        return false;
}

void Unit::GetPartyMembers(std::list<Unit*> &TagUnitMap)
{
    Unit* owner = GetCharmerOrOwnerOrSelf();
    Group* group = NULL;
    if (owner->GetTypeId() == TYPEID_PLAYER)
        group = owner->ToPlayer()->GetGroup();

    if (group)
    {
        uint8 subgroup = owner->ToPlayer()->GetSubGroup();

        for (GroupReference* itr = group->GetFirstMember(); itr != NULL; itr = itr->next())
        {
            Player* Target = itr->getSource();

            // IsHostileTo check duel and controlled by enemy
            if (Target && Target->GetSubGroup() == subgroup && !IsHostileTo(Target))
            {
                if (Target->isAlive() && IsInMap(Target))
                    TagUnitMap.push_back(Target);

                if (Guardian* pet = Target->GetGuardianPet())
                    if (pet->isAlive() && IsInMap(Target))
                        TagUnitMap.push_back(pet);
            }
        }
    }
    else
    {
        if (owner->isAlive() && (owner == this || IsInMap(owner)))
            TagUnitMap.push_back(owner);
        if (Guardian* pet = owner->GetGuardianPet())
            if (pet->isAlive() && (pet == this || IsInMap(pet)))
                TagUnitMap.push_back(pet);
    }
}

Aura* Unit::AddAura(uint32 spellId, Unit* target)
{
    if (!target)
        return NULL;

    SpellInfo const* spellInfo = sSpellMgr->GetSpellInfo(spellId);
    if (!spellInfo)
        return NULL;

    if (!target->isAlive() && !(spellInfo->Attributes & SPELL_ATTR0_PASSIVE) && !(spellInfo->AttributesEx2 & SPELL_ATTR2_CAN_TARGET_DEAD))
        return NULL;

    return AddAura(spellInfo, MAX_EFFECT_MASK, target);
}

Aura* Unit::AddAura(SpellInfo const* spellInfo, uint8 effMask, Unit* target)
{
    if (!spellInfo)
        return NULL;

    if (target->IsImmunedToSpell(spellInfo))
        return NULL;

    for (uint32 i = 0; i < MAX_SPELL_EFFECTS; ++i)
    {
        if (!(effMask & (1<<i)))
            continue;
        if (target->IsImmunedToSpellEffect(spellInfo, i))
            effMask &= ~(1<<i);
    }

    if (Aura* aura = Aura::TryRefreshStackOrCreate(spellInfo, effMask, target, this))
    {
        aura->ApplyForTargets();
        return aura;
    }
    return NULL;
}

Aura* Unit::AddAuraWithModCharges(uint32 spellid, Unit *target, int16 charges)
{
    Aura * aura = AddAura(spellid, target);
    if (aura)
        aura->SetModCharges(charges);
    return aura;
}

void Unit::SetAuraStack(uint32 spellId, Unit* target, uint32 stack)
{
    Aura* aura = target->GetAura(spellId, GetGUID());
    if (!aura)
        aura = AddAura(spellId, target);
    if (aura && stack)
        aura->SetStackAmount(stack);
}

void Unit::SendPlaySpellVisualKit(uint32 id, uint32 impact)
{
    ObjectGuid guid = GetGUID();

    WorldPacket data(SMSG_PLAY_SPELL_VISUAL_KIT, 4 + 4+ 4 + 8);
    data << uint32(0);
    data << uint32(id);     // SpellVisualKit.dbc index
    data << uint32(impact);
    data.WriteByteMask(guid[4]);
    data.WriteByteMask(guid[7]);
    data.WriteByteMask(guid[5]);
    data.WriteByteMask(guid[3]);
    data.WriteByteMask(guid[1]);
    data.WriteByteMask(guid[2]);
    data.WriteByteMask(guid[0]);
    data.WriteByteMask(guid[6]);
    data.WriteByteSeq(guid[0]);
    data.WriteByteSeq(guid[4]);
    data.WriteByteSeq(guid[1]);
    data.WriteByteSeq(guid[6]);
    data.WriteByteSeq(guid[7]);
    data.WriteByteSeq(guid[2]);
    data.WriteByteSeq(guid[3]);
    data.WriteByteSeq(guid[5]);
    SendMessageToSet(&data, true);
}

void Unit::SendPlaySpellVisual(uint32 id)
{

    Player* player = ToPlayer();
   
    if (!player)
        return;

    ObjectGuid guid = GetGUID();

    WorldPacket data(SMSG_PLAY_SPELL_VISUAL, 8 + 4);
    data << uint64(guid);
    data << uint32(id);

    player->GetSession()->SendPacket(&data);
}

void Unit::ApplyResilience(const Unit *pVictim, int32 *damage) const
{
    // player mounted on multi-passenger mount is also classified as vehicle
    if (IsVehicle() || (pVictim->IsVehicle() && pVictim->GetTypeId() != TYPEID_PLAYER))
        return;

    // Don't consider resilience if not in PvP - player or pet
    if (!GetCharmerOrOwnerPlayerOrPlayerItself())
        return;

    Unit const* target = NULL;
    if (pVictim->GetTypeId() == TYPEID_PLAYER)
        target = pVictim;
    else if (pVictim->GetTypeId() == TYPEID_UNIT && pVictim->GetOwner() && pVictim->GetOwner()->GetTypeId() == TYPEID_PLAYER)
        target = pVictim->GetOwner();

    if (!target)
        return;

    *damage -= target->GetDamageReduction(*damage);
}

// Melee based spells can be miss, parry or dodge on this step
// Crit or block - determined on damage calculation phase! (and can be both in some time)
float Unit::MeleeSpellMissChance(const Unit* victim, WeaponAttackType attType, uint32 spellId) const
{
    //calculate miss chance
    float missChance = victim->GetUnitMissChance(attType);

    // for example
    // | caster | target | miss 
    //    85        85      5
    //    85        86     5.5
    //    85        87      6
    //    85        88      8
    //    85        89      10

    if (victim->getLevel() > getLevel())
    {
        uint8 level_diff = victim->getLevel() - getLevel();
        if (level_diff <= 2)
        {
            missChance += 0.5f * level_diff;
        }
        else
        {
            missChance += ((0.5f + level_diff - 2) * 2);
        }
    }

    if (!spellId && haveOffhandWeapon())
        missChance += 19;

    // Calculate hit chance
    float hitChance = 100.0f;

    // Spellmod from SPELLMOD_RESIST_MISS_CHANCE
    if (spellId)
    {
        if (Player* modOwner = GetSpellModOwner())
            modOwner->ApplySpellMod(spellId, SPELLMOD_RESIST_MISS_CHANCE, hitChance);
    }

    missChance += hitChance - 100.0f;

    if (attType == RANGED_ATTACK)
        missChance -= m_modRangedHitChance;
    else
        missChance -= m_modMeleeHitChance;

    // Limit miss chance from 0 to 60%
    if (missChance < 0.0f)
        return 0.0f;
    if (missChance > 100.0f)
        return 100.0f;
    return missChance;
}

void Unit::SetPhaseMask(uint32 newPhaseMask, bool update)
{
    if (newPhaseMask == GetPhaseMask())
        return;

    if (IsInWorld())
    {
        RemoveNotOwnSingleTargetAuras(newPhaseMask);            // we can lost access to caster or target

        // modify hostile references for new phasemask, some special cases deal with hostile references themselves
        if (GetTypeId() == TYPEID_UNIT || (!ToPlayer()->isGameMaster() && !ToPlayer()->GetSession()->PlayerLogout()))
        {
            HostileRefManager& refManager = getHostileRefManager();
            HostileReference* ref = refManager.getFirst();

            while (ref)
            {
                if (Unit* unit = ref->getSource()->getOwner())
                    if (Creature* creature = unit->ToCreature())
                        refManager.setOnlineOfflineState(creature, creature->InSamePhase(newPhaseMask));

                ref = ref->next();
            }

            // modify threat lists for new phasemask
            if (GetTypeId() != TYPEID_PLAYER)
            {
                std::list<HostileReference*> threatList = getThreatManager().getThreatList();
                std::list<HostileReference*> offlineThreatList = getThreatManager().getOfflineThreatList();

                // merge expects sorted lists
                threatList.sort();
                offlineThreatList.sort();
                threatList.merge(offlineThreatList);

                for (std::list<HostileReference*>::const_iterator itr = threatList.begin(); itr != threatList.end(); ++itr)
                    if (Unit* unit = (*itr)->getTarget())
                        unit->getHostileRefManager().setOnlineOfflineState(ToCreature(), unit->InSamePhase(newPhaseMask));
            }
        }
    }

    WorldObject::SetPhaseMask(newPhaseMask, update);

    if (!IsInWorld())
        return;

    for (ControlList::const_iterator itr = m_Controlled.begin(); itr != m_Controlled.end(); ++itr)
        if ((*itr)->GetTypeId() == TYPEID_UNIT)
            (*itr)->SetPhaseMask(newPhaseMask, true);

    for (uint8 i = 0; i < MAX_SUMMON_SLOT; ++i)
        if (m_SummonSlot[i])
            if (Creature* summon = GetMap()->GetCreature(m_SummonSlot[i]))
                summon->SetPhaseMask(newPhaseMask, true);
}

class Unit::AINotifyTask : public BasicEvent
{
    Unit& m_owner;
public:
    explicit AINotifyTask(Unit * me) : m_owner(*me) {
        m_owner.m_VisibilityUpdScheduled = true;
    }

    ~AINotifyTask() {
        m_owner.m_VisibilityUpdScheduled = false;
    }

    virtual bool Execute(uint64 , uint32) 
    {
        Trinity::AIRelocationNotifier notifier(m_owner);
        m_owner.VisitNearbyObject(m_owner.GetVisibilityRange(), notifier);
        return true;
    }

    static void ScheduleAINotify(Unit* me)
    {
        if (!me->m_VisibilityUpdScheduled)
            me->m_Events.AddEvent(new AINotifyTask(me), me->m_Events.CalculateTime(World::Visibility_AINotifyDelay));
    }
};

class Unit::VisibilityUpdateTask : public BasicEvent
{
    Unit& m_owner;
public:
    explicit VisibilityUpdateTask(Unit * me) : m_owner(*me) {}

    virtual bool Execute(uint64 , uint32) 
    {
        UpdateVisibility(&m_owner);
        return true;
    }

    static void UpdateVisibility(Unit* me)
    {
        if (!me->m_sharedVision.empty())
            for (SharedVisionList::const_iterator it = me->m_sharedVision.begin();it!= me->m_sharedVision.end();)
            {
                Player * tmp = *it;
                ++it;
                tmp->UpdateVisibilityForPlayer();
            }
        if (me->isType(TYPEMASK_PLAYER))
            ((Player*)me)->UpdateVisibilityForPlayer();
        me->WorldObject::UpdateObjectVisibility(true);
    }
};

void Unit::OnRelocated()
{
    if (!m_lastVisibilityUpdPos.IsInDist(this, World::Visibility_RelocationLowerLimit)) {
        m_lastVisibilityUpdPos = *this;
        m_Events.AddEvent(new VisibilityUpdateTask(this), m_Events.CalculateTime(1));
    }
    AINotifyTask::ScheduleAINotify(this);
}

void Unit::UpdateObjectVisibility(bool forced)
{
    if (forced)
        VisibilityUpdateTask::UpdateVisibility(this);
    else
        m_Events.AddEvent(new VisibilityUpdateTask(this), m_Events.CalculateTime(1));
    AINotifyTask::ScheduleAINotify(this);
}

void Unit::SendMoveKnockBack(Player* player, float speedXY, float speedZ, float vcos, float vsin)
{
    /*Update movementInfo data*/
    {
        m_movementInfo.AddServerMovementFlag(SERVERMOVEFLAG_FALLDATA);
        m_movementInfo.AddServerMovementFlag(SERVERMOVEFLAG_FALLDIRECTION);

        m_movementInfo.fallTime = 0;
        m_movementInfo.j_cosAngle = vcos;
        m_movementInfo.j_sinAngle = vsin;
        m_movementInfo.j_xyspeed = speedXY;
        m_movementInfo.j_zspeed = speedZ;
    }

    ObjectGuid guid = GetGUID();
    WorldPacket data(SMSG_MOVE_KNOCK_BACK, (1+8+4+4+4+4+4));
    data.WriteByteMask(guid[0]);
    data.WriteByteMask(guid[3]);
    data.WriteByteMask(guid[6]);
    data.WriteByteMask(guid[7]);
    data.WriteByteMask(guid[2]);
    data.WriteByteMask(guid[5]);
    data.WriteByteMask(guid[1]);
    data.WriteByteMask(guid[4]);

    data.WriteByteSeq(guid[1]);

    data << float(m_movementInfo.j_sinAngle);
    data << uint32(0);

    data.WriteByteSeq(guid[6]);
    data.WriteByteSeq(guid[7]);

    data << float(m_movementInfo.j_xyspeed);

    data.WriteByteSeq(guid[4]);
    data.WriteByteSeq(guid[5]);
    data.WriteByteSeq(guid[3]);

    data << float(m_movementInfo.j_zspeed);
    data << float(m_movementInfo.j_cosAngle);

    data.WriteByteSeq(guid[2]);
    data.WriteByteSeq(guid[0]);

    player->GetSession()->SendPacket(&data);
}

void Unit::KnockbackFrom(float x, float y, float speedXY, float speedZ)
{
    Player* player = NULL;
    if (GetTypeId() == TYPEID_PLAYER)
        player = ToPlayer();
    else if (Unit* charmer = GetCharmer())
    {
        player = charmer->ToPlayer();
        if (player && player->m_mover != this)
            player = NULL;
    }

    if (!player)
        GetMotionMaster()->MoveKnockbackFrom(x, y, speedXY, speedZ);
    else
    {
        float vcos, vsin;
        GetSinCos(x, y, vsin, vcos);
        SendMoveKnockBack(player, speedXY, -speedZ, vcos, vsin);
    }
}

float Unit::GetCombatRatingReduction(CombatRating cr) const
{
    if (Player const* player = ToPlayer())
        return player->GetRatingBonusValue(cr);
    // Player's pet get resilience from owner
    else if (isPet() && GetOwner())
        if (Player* owner = GetOwner()->ToPlayer())
            return owner->GetRatingBonusValue(cr);

    return 0.0f;
}

uint32 Unit::GetCombatRatingDamageReduction(CombatRating cr, float rate, float cap, uint32 damage) const
{
    float percent = std::min(GetCombatRatingReduction(cr) * rate, cap);
    return CalculatePct(damage, percent);
}

uint32 Unit::GetModelForForm(ShapeshiftForm form) const
{
    if (GetTypeId() == TYPEID_PLAYER)
    {
        switch (form)
        {
            case FORM_CAT:
                // Hack for Druid of the Flame, Fandral's Flamescythe
                if (HasAura(99245))
                    return 38150;

                // Based on Hair color
                if (getRace() == RACE_NIGHTELF)
                {
                    uint8 hairColor = GetByteValue(PLAYER_BYTES, 3);
                    switch (hairColor)
                    {
                        case 7: // Violet
                        case 8:
                            return 29405;
                        case 3: // Light Blue
                            return 29406;
                        case 0: // Green
                        case 1: // Light Green
                        case 2: // Dark Green
                            return 29407;
                        case 4: // White
                            return 29408;
                        default: // original - Dark Blue
                            return 892;
                    }
                }
                else if (getRace() == RACE_TROLL)
                {
                    uint8 hairColor = GetByteValue(PLAYER_BYTES, 3);
                    switch (hairColor)
                    {
                        case 0: // Red
                        case 1:
                            return 33668;
                        case 2: // Yellow
                        case 3:
                            return 33667;
                        case 4: // Blue
                        case 5:
                        case 6:
                            return 33666;
                        case 7: // Purple
                        case 10:
                            return 33665;
                        default: // original - white
                            return 33669;
                    }
                }
                else if (getRace() == RACE_WORGEN)
                {
                    // Based on Skin color
                    uint8 skinColor = GetByteValue(PLAYER_BYTES, 0);
                    // Male
                    if (getGender() == GENDER_MALE)
                    {
                        switch (skinColor)
                        {
                            case 1: // Brown
                                return 33662;
                            case 2: // Black
                            case 7:
                                return 33661;
                            case 4: // yellow
                                return 33664;
                            case 3: // White
                            case 5:
                                return 33663;
                            default: // original - Gray
                                return 33660;
                        }
                    }
                    // Female
                    else
                    {
                        switch (skinColor)
                        {
                            case 5: // Brown
                            case 6:
                                return 33662;
                            case 7: // Black
                            case 8:
                                return 33661;
                            case 3: // yellow
                            case 4:
                                return 33664;
                            case 2: // White
                                return 33663;
                            default: // original - Gray
                                return 33660;
                        }
                    }
                }
                // Based on Skin color
                else if (getRace() == RACE_TAUREN)
                {
                    uint8 skinColor = GetByteValue(PLAYER_BYTES, 0);
                    // Male
                    if (getGender() == GENDER_MALE)
                    {
                        switch (skinColor)
                        {
                            case 12: // White
                            case 13:
                            case 14:
                            case 18: // Completly White
                                return 29409;
                            case 9: // Light Brown
                            case 10:
                            case 11:
                                return 29410;
                            case 6: // Brown
                            case 7:
                            case 8:
                                return 29411;
                            case 0: // Dark
                            case 1:
                            case 2:
                            case 3: // Dark Grey
                            case 4:
                            case 5:
                                return 29412;
                            default: // original - Grey
                                return 8571;
                        }
                    }
                    // Female
                    else
                    {
                        switch (skinColor)
                        {
                            case 10: // White
                                return 29409;
                            case 6: // Light Brown
                            case 7:
                                return 29410;
                            case 4: // Brown
                            case 5:
                                return 29411;
                            case 0: // Dark
                            case 1:
                            case 2:
                            case 3:
                                return 29412;
                            default: // original - Grey
                                return 8571;
                        }
                    }
                }
                else if (Player::TeamForRace(getRace()) == ALLIANCE)
                    return 892;
                else
                    return 8571;
            case FORM_BEAR:
                // Based on Hair color
                if (getRace() == RACE_NIGHTELF)
                {
                    uint8 hairColor = GetByteValue(PLAYER_BYTES, 3);
                    switch (hairColor)
                    {
                        case 0: // Green
                        case 1: // Light Green
                        case 2: // Dark Green
                            return 29413; // 29415?
                        case 6: // Dark Blue
                            return 29414;
                        case 4: // White
                            return 29416;
                        case 3: // Light Blue
                            return 29417;
                        default: // original - Violet
                            return 2281;
                    }
                }
                else if (getRace() == RACE_TROLL)
                {
                    uint8 hairColor = GetByteValue(PLAYER_BYTES, 3);
                    switch (hairColor)
                    {
                        case 0: // Red
                        case 1:
                            return 33657;
                        case 2: // Yellow
                        case 3:
                            return 33659;
                        case 7: // Purple
                        case 10:
                            return 33656;
                        case 8: // White
                        case 9:
                        case 11:
                        case 12:
                            return 33658;
                        default: // original - Blue
                            return 33655;
                    }
                }
                else if (getRace() == RACE_WORGEN)
                {
                    // Based on Skin color
                    uint8 skinColor = GetByteValue(PLAYER_BYTES, 0);
                    // Male
                    if (getGender() == GENDER_MALE)
                    {
                        switch (skinColor)
                        {
                            case 1: // Brown
                                return 33652;
                            case 2: // Black
                            case 7:
                                return 33651;
                            case 4: // Yellow
                                return 33653;
                            case 3: // White
                            case 5:
                                return 33654;
                            default: // original - Gray
                                return 33650;
                        }
                    }
                    // Female
                    else
                    {
                        switch (skinColor)
                        {
                            case 5: // Brown
                            case 6:
                                return 33652;
                            case 7: // Black
                            case 8:
                                return 33651;
                            case 3: // yellow
                            case 4:
                                return 33654;
                            case 2: // White
                                return 33653;
                            default: // original - Gray
                                return 33650;
                        }
                    }
                }
                // Based on Skin color
                else if (getRace() == RACE_TAUREN)
                {
                    uint8 skinColor = GetByteValue(PLAYER_BYTES, 0);
                    // Male
                    if (getGender() == GENDER_MALE)
                    {
                        switch (skinColor)
                        {
                            case 0: // Dark (Black)
                            case 1:
                            case 2:
                                return 29418;
                            case 3: // White
                            case 4:
                            case 5:
                            case 12:
                            case 13:
                            case 14:
                                return 29419;
                            case 9: // Light Brown/Grey
                            case 10:
                            case 11:
                            case 15:
                            case 16:
                            case 17:
                                return 29420;
                            case 18: // Completly White
                                return 29421;
                            default: // original - Brown
                                return 2289;
                        }
                    }
                    // Female
                    else
                    {
                        switch (skinColor)
                        {
                            case 0: // Dark (Black)
                            case 1:
                                return 29418;
                            case 2: // White
                            case 3:
                                return 29419;
                            case 6: // Light Brown/Grey
                            case 7:
                            case 8:
                            case 9:
                                return 29420;
                            case 10: // Completly White
                                return 29421;
                            default: // original - Brown
                                return 2289;
                        }
                    }
                }
                else if (Player::TeamForRace(getRace()) == ALLIANCE)
                    return 2281;
                else
                    return 2289;
            case FORM_FLIGHT:
                switch(getRace())
                {
                    case RACE_NIGHTELF:
                        return 20857;
                    case RACE_WORGEN:
                        return 37727;
                    case RACE_TROLL:
                        return 37728;
                    default: // RACE_TAUREN
                        return 20872;
                }
            case FORM_FLIGHT_EPIC:
                switch(getRace())
                {
                    case RACE_NIGHTELF:
                        return 21243;
                    case RACE_WORGEN:
                        return 37729;
                    case RACE_TROLL:
                        return 37730;
                    default: // RACE_TAUREN
                        return 21244;
                }
            case FORM_MOONKIN:
                switch(getRace())
                {
                    case RACE_NIGHTELF:
                        return 15374;
                    case RACE_TAUREN:
                        return 15375;
                    case RACE_WORGEN:
                        return 37173;
                    case RACE_TROLL:
                        return 37174;
                    default: // RACE_NIGHTELF
                        return 15374;
                }
                break;
            case FORM_TREE:
                if (HasAura(95212))
                    return 864;
                else
                    return 37165;
                break;
            case FORM_GHOSTWOLF:
                if (HasAura(58135)) // Glyph of the Arctic Wolf
                    return 27312;
                else
                    return 4613;
                break;
            default:
                break;
        }
    }

    uint32 modelid = 0;
    SpellShapeshiftFormEntry const* formEntry = sSpellShapeshiftFormStore.LookupEntry(form);
    if (formEntry && formEntry->modelID_A)
    {
        // Take the alliance modelid as default
        if (GetTypeId() != TYPEID_PLAYER)
            return formEntry->modelID_A;
        else
        {
            if (Player::TeamForRace(getRace()) == ALLIANCE)
                modelid = formEntry->modelID_A;
            else
                modelid = formEntry->modelID_H;

            // If the player is horde but there are no values for the horde modelid - take the alliance modelid
            if (!modelid && Player::TeamForRace(getRace()) == HORDE)
                modelid = formEntry->modelID_A;
        }
    }

    return modelid;
}

uint32 Unit::GetModelForTotem(PlayerTotemType totemType)
{
    switch (getRace())
    {
        case RACE_ORC:
        {
            switch (totemType)
            {
                case SUMMON_TYPE_TOTEM_FIRE:    // fire
                    return 30758;
                case SUMMON_TYPE_TOTEM_EARTH:   // earth
                    return 30757;
                case SUMMON_TYPE_TOTEM_WATER:   // water
                    return 30759;
                case SUMMON_TYPE_TOTEM_AIR:     // air
                    return 30756;
            }
            break;
        }
        case RACE_DWARF:
        {
            switch (totemType)
            {
                case SUMMON_TYPE_TOTEM_FIRE:    // fire
                    return 30754;
                case SUMMON_TYPE_TOTEM_EARTH:   // earth
                    return 30753;
                case SUMMON_TYPE_TOTEM_WATER:   // water
                    return 30755;
                case SUMMON_TYPE_TOTEM_AIR:     // air
                    return 30736;
            }
            break;
        }
        case RACE_TROLL:
        {
            switch (totemType)
            {
                case SUMMON_TYPE_TOTEM_FIRE:    // fire
                    return 30762;
                case SUMMON_TYPE_TOTEM_EARTH:   // earth
                    return 30761;
                case SUMMON_TYPE_TOTEM_WATER:   // water
                    return 30763;
                case SUMMON_TYPE_TOTEM_AIR:     // air
                    return 30760;
            }
            break;
        }
        case RACE_TAUREN:
        {
            switch (totemType)
            {
                case SUMMON_TYPE_TOTEM_FIRE:    // fire
                    return 4589;
                case SUMMON_TYPE_TOTEM_EARTH:   // earth
                    return 4588;
                case SUMMON_TYPE_TOTEM_WATER:   // water
                    return 4587;
                case SUMMON_TYPE_TOTEM_AIR:     // air
                    return 4590;
            }
            break;
        }
        case RACE_DRAENEI:
        {
            switch (totemType)
            {
                case SUMMON_TYPE_TOTEM_FIRE:    // fire
                    return 19074;
                case SUMMON_TYPE_TOTEM_EARTH:   // earth
                    return 19073;
                case SUMMON_TYPE_TOTEM_WATER:   // water
                    return 19075;
                case SUMMON_TYPE_TOTEM_AIR:     // air
                    return 19071;
            }
            break;
        }
        case RACE_GOBLIN:
        {
            switch (totemType)
            {
                case SUMMON_TYPE_TOTEM_FIRE:    // fire
                    return 30783;
                case SUMMON_TYPE_TOTEM_EARTH:   // earth
                    return 30782;
                case SUMMON_TYPE_TOTEM_WATER:   // water
                    return 30784;
                case SUMMON_TYPE_TOTEM_AIR:     // air
                    return 30781;
            }
            break;
        }
    }
    return 0;
}

void Unit::JumpTo(float speedXY, float speedZ, bool forward)
{
    float angle = forward ? 0 : M_PI;
    if (GetTypeId() == TYPEID_UNIT)
        GetMotionMaster()->MoveJumpTo(angle, speedXY, speedZ);
    else
    {
        float vcos = std::cos(angle+GetOrientation());
        float vsin = std::sin(angle+GetOrientation());
        SendMoveKnockBack(ToPlayer(), speedXY, -speedZ, vcos, vsin);
    }
}

void Unit::JumpTo(WorldObject* obj, float speedZ)
{
    float x, y, z;
    obj->GetContactPoint(this, x, y, z);
    float speedXY = GetExactDist2d(x, y) * 10.0f / speedZ;
    GetMotionMaster()->MoveJump(x, y, z, speedXY, speedZ);
}

bool Unit::HandleSpellClick(Unit* clicker, int8 seatId)
{
    bool result = false;
    uint32 spellClickEntry = GetVehicleKit() ? GetVehicleKit()->GetCreatureEntry() : GetEntry();
    SpellClickInfoMapBounds clickPair = sObjectMgr->GetSpellClickInfoMapBounds(spellClickEntry);
    for (SpellClickInfoContainer::const_iterator itr = clickPair.first; itr != clickPair.second; ++itr)
    {
        //! First check simple relations from clicker to clickee
        if (!itr->second.IsFitToRequirements(clicker, this))
            continue;

        //! Check database conditions
        ConditionList conds = sConditionMgr->GetConditionsForSpellClickEvent(spellClickEntry, itr->second.spellId);
        ConditionSourceInfo info = ConditionSourceInfo(clicker, this);
        if (!sConditionMgr->IsObjectMeetToConditions(info, conds))
            continue;

        Unit* caster = (itr->second.castFlags & NPC_CLICK_CAST_CASTER_CLICKER) ? clicker : this;
        Unit* target = (itr->second.castFlags & NPC_CLICK_CAST_TARGET_CLICKER) ? clicker : this;
        uint64 origCasterGUID = (itr->second.castFlags & NPC_CLICK_CAST_ORIG_CASTER_OWNER) ? GetOwnerGUID() : clicker->GetGUID();

        SpellInfo const* spellEntry = sSpellMgr->GetSpellInfo(itr->second.spellId);
        // if (!spellEntry) should be checked at npc_spellclick load

        if (seatId > -1)
        {
            uint8 i = 0;
            bool valid = false;
            while (i < MAX_SPELL_EFFECTS && !valid)
            {
                if (spellEntry->Effects[i].ApplyAuraName == SPELL_AURA_CONTROL_VEHICLE)
                {
                    valid = true;
                    break;
                }
                ++i;
            }

            if (!valid)
            {
                sLog->outError(LOG_FILTER_SQL, "Spell %u specified in npc_spellclick_spells is not a valid vehicle enter aura!", itr->second.spellId);
                continue;
            }

            if (IsInMap(caster))
                caster->CastCustomSpell(itr->second.spellId, SpellValueMod(SPELLVALUE_BASE_POINT0+i), seatId + 1, target, GetVehicleKit() ? TRIGGERED_IGNORE_CASTER_MOUNTED_OR_ON_VEHICLE : TRIGGERED_NONE, NULL, NULL, origCasterGUID);
            else    // This can happen during Player::_LoadAuras
            {
                int32 bp0 = seatId;
                Aura::TryRefreshStackOrCreate(spellEntry, MAX_EFFECT_MASK, this, clicker, &bp0, NULL, origCasterGUID);
            }
        }
        else
        {
            if (IsInMap(caster))
                caster->CastSpell(target, spellEntry, GetVehicleKit() ? TRIGGERED_IGNORE_CASTER_MOUNTED_OR_ON_VEHICLE : TRIGGERED_NONE, NULL, NULL, origCasterGUID);
            else
                Aura::TryRefreshStackOrCreate(spellEntry, MAX_EFFECT_MASK, this, clicker, NULL, NULL, origCasterGUID);
        }

        result = true;
    }

    if (result)
    {
        Creature* creature = ToCreature();
        if (creature && creature->IsAIEnabled)
            creature->AI()->OnSpellClick(clicker, result);
    }

    return result;
}

void Unit::EnterVehicle(Unit* base, int8 seatId, bool fullTriggered)
{
    CastCustomSpell(VEHICLE_SPELL_RIDE_HARDCODED, SPELLVALUE_BASE_POINT0, seatId + 1, base, fullTriggered ? TRIGGERED_FULL_MASK : TRIGGERED_IGNORE_CASTER_MOUNTED_OR_ON_VEHICLE);
}

void Unit::_EnterVehicle(Vehicle* vehicle, int8 seatId, AuraApplication const* aurApp)
{
    // Must be called only from aura handler
    if (!isAlive() || GetVehicleKit() == vehicle || vehicle->GetBase()->IsOnVehicle(this))
        return;

    if (m_vehicle)
    {
        if (m_vehicle == vehicle)
        {
            if (seatId >= 0 && seatId != GetTransSeat())
                ChangeSeat(seatId);

            return;
        }
        else
            ExitVehicle();
    }

    if (aurApp && aurApp->GetRemoveMode())
        return;

    if (Player* player = ToPlayer())
    {
        if (vehicle->GetBase()->GetTypeId() == TYPEID_PLAYER && player->isInCombat())
            return;

        InterruptNonMeleeSpells(false);
        player->StopCastingCharm();
        player->StopCastingBindSight();
        Dismount();
        RemoveAurasByType(SPELL_AURA_MOUNTED);

        // drop flag at invisible in bg
        if (Battleground* bg = player->GetBattleground())
            bg->EventPlayerDroppedFlag(player);

        WorldPacket data(SMSG_ON_CANCEL_EXPECTED_RIDE_VEHICLE_AURA, 0);
        player->GetSession()->SendPacket(&data);

        switch (vehicle->GetVehicleInfo()->m_ID)
        {
            case 533: // Bone Spike
            case 647: // Bone Spike
            case 648: // Bone Spike
                break;
            default:
                player->UnsummonPetTemporaryIfAny();
                break;
        }
    }

    ASSERT(!m_vehicle);
    m_vehicle = vehicle;

    if (!m_vehicle->AddPassenger(this, seatId))
    {
        m_vehicle = NULL;
        return;
    }
}

void Unit::ChangeSeat(int8 seatId, bool next)
{
    if (!m_vehicle)
        return;

    if (seatId < 0)
    {
        seatId = m_vehicle->GetNextEmptySeat(GetTransSeat(), next);
        if (seatId < 0)
            return;
    }
    else if (seatId == GetTransSeat() || !m_vehicle->HasEmptySeat(seatId))
        return;

    m_vehicle->RemovePassenger(this);
    if (!m_vehicle->AddPassenger(this, seatId))
        ASSERT(false);
}

void Unit::ExitVehicle(Position const* /*exitPosition*/)
{
    //! This function can be called at upper level code to initialize an exit from the passenger's side.
    if (!m_vehicle)
        return;

    GetVehicleBase()->RemoveAurasByType(SPELL_AURA_CONTROL_VEHICLE, GetGUID());
    //! The following call would not even be executed successfully as the
    //! SPELL_AURA_CONTROL_VEHICLE unapply handler already calls _ExitVehicle without
    //! specifying an exitposition. The subsequent call below would return on if (!m_vehicle).
    /*_ExitVehicle(exitPosition);*/
    //! To do:
    //! We need to allow SPELL_AURA_CONTROL_VEHICLE unapply handlers in spellscripts
    //! to specify exit coordinates and either store those per passenger, or we need to
    //! init spline movement based on those coordinates in unapply handlers, and
    //! relocate exiting passengers based on Unit::moveSpline data. Either way,
    //! Coming Soon(TM)
}

void Unit::_ExitVehicle(Position const* exitPosition)
{
    if (!m_vehicle)
        return;

    m_vehicle->RemovePassenger(this);

    bool playerOnTransport = false;
    Player* player = ToPlayer();
    if (player && player->GetTransport())
        playerOnTransport = true;

    // If player is on mouted duel and exits the mount should immediatly lose the duel
    if (player && player->duel && player->duel->isMounted)
        player->DuelComplete(DUEL_FLED);

    // This should be done before dismiss, because there may be some aura removal
    Vehicle* vehicle = m_vehicle;
    m_vehicle = NULL;

    SetControlled(false, UNIT_STATE_ROOT);      // SMSG_MOVE_FORCE_UNROOT, ~MOVEMENTFLAG_ROOT

    Position pos;
    if (!exitPosition)                          // Exit position not specified
        vehicle->GetBase()->GetPosition(&pos);  // This should use passenger's current position, leaving it as it is now
                                                // because we calculate positions incorrect (sometimes under map)
    else
        pos = *exitPosition;

    AddUnitState(UNIT_STATE_MOVE);

    if (player)
        player->SetFallInformation(0, GetPositionZ());
    else if (HasUnitMovementFlag(MOVEMENTFLAG_ROOT))
    {
        WorldPacket data(SMSG_SPLINE_MOVE_UNROOT, 8);
        ObjectGuid guid = GetGUID();
        data.WriteByteMask(guid[0]);
        data.WriteByteMask(guid[1]);
        data.WriteByteMask(guid[6]);
        data.WriteByteMask(guid[5]);
        data.WriteByteMask(guid[3]);
        data.WriteByteMask(guid[2]);
        data.WriteByteMask(guid[7]);
        data.WriteByteMask(guid[4]);
        data.WriteByteSeq(guid[6]);
        data.WriteByteSeq(guid[3]);
        data.WriteByteSeq(guid[1]);
        data.WriteByteSeq(guid[5]);
        data.WriteByteSeq(guid[2]);
        data.WriteByteSeq(guid[0]);
        data.WriteByteSeq(guid[7]);
        data.WriteByteSeq(guid[4]);
        SendMessageToSet(&data, false);
    }

    if (!playerOnTransport)
    {
        Movement::MoveSplineInit init(*this);
        init.MoveTo(pos.GetPositionX(), pos.GetPositionY(), pos.GetPositionZ());
        init.SetFacing(GetOrientation());
        init.SetTransportExit();
        init.Launch();
    }
    else
        player->Relocate(pos);

    //GetMotionMaster()->MoveFall();            // Enable this once passenger positions are calculater properly (see above)

    if (player)
        player->ResummonPetTemporaryUnSummonedIfAny();

    SendMovementFlagUpdate();

    if (vehicle->GetBase()->HasUnitTypeMask(UNIT_MASK_MINION))
        if (((Minion*)vehicle->GetBase())->GetOwner() == this)
            vehicle->Dismiss();

    if (HasUnitTypeMask(UNIT_MASK_ACCESSORY))
    {
        // Vehicle just died, we die too
        if (vehicle->GetBase()->getDeathState() == JUST_DIED)
            setDeathState(JUST_DIED);
        // If for other reason we as minion are exiting the vehicle (ejected, master dismounted) - unsummon
        else
            ToTempSummon()->UnSummon(2000); // Approximation
    }
}

void Unit::SetCanFly(bool apply)
{
    if (apply)
        AddUnitMovementFlag(MOVEMENTFLAG_CAN_FLY);
    else
        RemoveUnitMovementFlag(MOVEMENTFLAG_CAN_FLY);
}

void Unit::NearTeleportTo(float x, float y, float z, float orientation, bool casting /*= false*/)
{
    DisableSpline();
    if (GetTypeId() == TYPEID_PLAYER)
        ToPlayer()->TeleportTo(GetMapId(), x, y, z, orientation, TELE_TO_NOT_LEAVE_TRANSPORT | TELE_TO_NOT_LEAVE_COMBAT | TELE_TO_NOT_UNSUMMON_PET | (casting ? TELE_TO_SPELL : 0));
    else
    {
        //DestroyForNearbyPlayers();
        //SendMovementFlagUpdate();
        Position pos = {x, y, z, orientation};
        SendTeleportPacket(pos);
        UpdatePosition(x, y, z, orientation, true);
        UpdateObjectVisibility();
    }
}

void Unit::SendTeleportPacket(Position &pos)
{
    Position oldPos = {GetPositionX(), GetPositionY(), GetPositionZMinusOffset(), GetOrientation()};

    if (GetTypeId() == TYPEID_UNIT)
        Relocate(&pos);

    ObjectGuid guid = GetGUID();
    ObjectGuid transGuid = GetTransGUID();

    WorldPacket data(MSG_MOVE_TELEPORT, 38);
    data
        .WriteByteMask(guid[6])
        .WriteByteMask(guid[0])
        .WriteByteMask(guid[3])
        .WriteByteMask(guid[2])
        .WriteBit(false)
        .WriteBit(uint64(transGuid) != 0)
        .WriteByteMask(guid[1]);
    if (transGuid)
    {
        data
            .WriteByteMask(transGuid[1])
            .WriteByteMask(transGuid[3])
            .WriteByteMask(transGuid[2])
            .WriteByteMask(transGuid[5])
            .WriteByteMask(transGuid[0])
            .WriteByteMask(transGuid[7])
            .WriteByteMask(transGuid[6])
            .WriteByteMask(transGuid[4]);
    }
    data
        .WriteByteMask(guid[4])
        .WriteByteMask(guid[7])
        .WriteByteMask(guid[5]);

    if (transGuid)
    {
        data
            .WriteByteSeq(transGuid[6])
            .WriteByteSeq(transGuid[5])
            .WriteByteSeq(transGuid[1])
            .WriteByteSeq(transGuid[7])
            .WriteByteSeq(transGuid[0])
            .WriteByteSeq(transGuid[2])
            .WriteByteSeq(transGuid[4])
            .WriteByteSeq(transGuid[3]);
    }
    data << uint32(0);  // counter
    data
        .WriteByteSeq(guid[1])
        .WriteByteSeq(guid[2])
        .WriteByteSeq(guid[3])
        .WriteByteSeq(guid[5]);

    data << float(GetPositionX());
    data.WriteByteSeq(guid[4]);
    data << float(GetOrientation());
    data.WriteByteSeq(guid[7]);
    data << float(GetPositionZMinusOffset());
    data.WriteByteSeq(guid[0]);
    data.WriteByteSeq(guid[6]);
    data << float(GetPositionY());

    if (GetTypeId() == TYPEID_PLAYER)
        Relocate(&pos);
    else
        Relocate(&oldPos);
    SendMessageToSet(&data, true);
}

bool Unit::UpdatePosition(float x, float y, float z, float orientation, bool teleport)
{
    // prevent crash when a bad coord is sent by the client
    if (!Trinity::IsValidMapCoord(x, y, z, orientation))
        return false;

    bool turn = (GetOrientation() != orientation);
    bool relocated = (teleport || GetPositionX() != x || GetPositionY() != y || (!HasUnitMovementFlag(MOVEMENTFLAG_HOVER) && GetPositionZ() != z));

    if (turn)
        RemoveAurasWithInterruptFlags(AURA_INTERRUPT_FLAG_TURNING);

    if (relocated)
    {
        RemoveAurasWithInterruptFlags(AURA_INTERRUPT_FLAG_MOVE);

        // move and update visible state if need
        if (GetTypeId() == TYPEID_PLAYER)
            GetMap()->PlayerRelocation(ToPlayer(), x, y, z, orientation);
        else
        {
            GetMap()->CreatureRelocation(ToCreature(), x, y, z, orientation);
            // code block for underwater state update
            UpdateUnderwaterState(GetMap(), x, y, z);
        }
    }
    else if (turn)
        UpdateOrientation(orientation);

    return (relocated || turn);
}

//! Only server-side orientation update, does not broadcast to client
void Unit::UpdateOrientation(float orientation)
{
    SetOrientation(orientation);
    if (IsVehicle())
        GetVehicleKit()->RelocatePassengers();
}

//! Only server-side height update, does not broadcast to client
void Unit::UpdateHeight(float newZ)
{
    Relocate(GetPositionX(), GetPositionY(), newZ);
    if (IsVehicle())
        GetVehicleKit()->RelocatePassengers();
}

void Unit::SendThreatListUpdate()
{
    if (!getThreatManager().isThreatListEmpty())
    {
        uint32 count = getThreatManager().getThreatList().size();

        WorldPacket data(SMSG_THREAT_UPDATE, 8 + count * 8);
        data.append(GetPackGUID());
        data << uint32(count);
        std::list<HostileReference*>& tlist = getThreatManager().getThreatList();
        for (std::list<HostileReference*>::const_iterator itr = tlist.begin(); itr != tlist.end(); ++itr)
        {
            data.appendPackGUID((*itr)->getUnitGuid());
            data << uint32((*itr)->getThreat() * 100);
        }
        SendMessageToSet(&data, false);
    }
}

void Unit::SendChangeCurrentVictimOpcode(HostileReference* pHostileReference)
{
    if (!getThreatManager().isThreatListEmpty())
    {
        uint32 count = getThreatManager().getThreatList().size();

        WorldPacket data(SMSG_HIGHEST_THREAT_UPDATE, 8 + 8 + count * 8);
        data.append(GetPackGUID());
        data.appendPackGUID(pHostileReference->getUnitGuid());
        data << uint32(count);
        std::list<HostileReference*>& tlist = getThreatManager().getThreatList();
        for (std::list<HostileReference*>::const_iterator itr = tlist.begin(); itr != tlist.end(); ++itr)
        {
            data.appendPackGUID((*itr)->getUnitGuid());
            data << uint32((*itr)->getThreat());
        }
        SendMessageToSet(&data, false);
    }
}

void Unit::SendClearThreatListOpcode()
{
    WorldPacket data(SMSG_THREAT_CLEAR, 8);
    data.append(GetPackGUID());
    SendMessageToSet(&data, false);
}

void Unit::SendRemoveFromThreatListOpcode(HostileReference* pHostileReference)
{
    WorldPacket data(SMSG_THREAT_REMOVE, 8 + 8);
    data.append(GetPackGUID());
    data.appendPackGUID(pHostileReference->getUnitGuid());
    SendMessageToSet(&data, false);
}

// baseRage means damage taken when attacker = false
void Unit::RewardRage(uint32 baseRage, bool attacker)
{
    float addRage = baseRage;

    if (attacker)
    {
        // talent who gave more rage on attack
        addRage *= 1.0f + GetTotalAuraModifier(SPELL_AURA_MOD_RAGE_FROM_DAMAGE_DEALT) / 100.0f;

        // Sentinel - Protection Warrior Mastery
        if (AuraEffect * const sentiel = GetAuraEffect(29144, 1))
            if (getVictim() && (!getVictim()->getVictim() || (getVictim()->getVictim() && this != getVictim()->getVictim())))
                addRage *= float((sentiel->GetAmount() + 100.0f) / 100.0f);
    }
    else
    {
        addRage /= (GetCreateHealth()/35);

        // Berserker Rage effect
        if (HasAura(18499))
        {
            float mod = 2.0f;

            // Unshackled Fury (Mastery Fury Warrior)
            if (Aura* aur = GetAura(76856))
                mod += float(aur->GetEffect(0)->GetAmount() / 100.0f);

            addRage *= mod;
        }
    }

    ModifyPower(POWER_RAGE, uint32(addRage * 10));
}

void Unit::StopAttackFaction(uint32 faction_id)
{
    if (Unit* victim = getVictim())
    {
        if (victim->getFactionTemplateEntry()->faction == faction_id)
        {
            AttackStop();
            if (IsNonMeleeSpellCasted(false))
                InterruptNonMeleeSpells(false);

            // melee and ranged forced attack cancel
            if (GetTypeId() == TYPEID_PLAYER)
                ToPlayer()->SendAttackSwingCancelAttack();
        }
    }

    AttackerSet const& attackers = getAttackers();
    for (AttackerSet::const_iterator itr = attackers.begin(); itr != attackers.end();)
    {
        if ((*itr)->getFactionTemplateEntry()->faction == faction_id)
        {
            (*itr)->AttackStop();
            itr = attackers.begin();
        }
        else
            ++itr;
    }

    getHostileRefManager().deleteReferencesForFaction(faction_id);

    for (ControlList::const_iterator itr = m_Controlled.begin(); itr != m_Controlled.end(); ++itr)
            (*itr)->StopAttackFaction(faction_id);
}

void Unit::OutDebugInfo() const
{
    sLog->outError(LOG_FILTER_UNITS, "Unit::OutDebugInfo");
    sLog->outInfo(LOG_FILTER_UNITS, "GUID "UI64FMTD", entry %u, type %u, name %s", GetGUID(), GetEntry(), (uint32)GetTypeId(), GetName());
    sLog->outInfo(LOG_FILTER_UNITS, "OwnerGUID "UI64FMTD", MinionGUID "UI64FMTD", CharmerGUID "UI64FMTD", CharmedGUID "UI64FMTD, GetOwnerGUID(), GetMinionGUID(), GetCharmerGUID(), GetCharmGUID());
    sLog->outInfo(LOG_FILTER_UNITS, "In world %u, unit type mask %u", (uint32)(IsInWorld() ? 1 : 0), m_unitTypeMask);
    if (IsInWorld())
        sLog->outInfo(LOG_FILTER_UNITS, "Mapid %u", GetMapId());

    std::ostringstream o;
    o << "Summon Slot: ";
    for (uint32 i = 0; i < MAX_SUMMON_SLOT; ++i)
        o << m_SummonSlot[i] << ", ";

    sLog->outInfo(LOG_FILTER_UNITS, "%s", o.str().c_str());
    o.str("");

    o << "Controlled List: ";
    for (ControlList::const_iterator itr = m_Controlled.begin(); itr != m_Controlled.end(); ++itr)
        o << (*itr)->GetGUID() << ", ";
    sLog->outInfo(LOG_FILTER_UNITS, "%s", o.str().c_str());
    o.str("");

    o << "Aura List: ";
    for (AuraApplicationMap::const_iterator itr = m_appliedAuras.begin(); itr != m_appliedAuras.end(); ++itr)
        o << itr->first << ", ";
    sLog->outInfo(LOG_FILTER_UNITS, "%s", o.str().c_str());
    o.str("");

    if (IsVehicle())
    {
        o << "Passenger List: ";
        for (SeatMap::iterator itr = GetVehicleKit()->Seats.begin(); itr != GetVehicleKit()->Seats.end(); ++itr)
            if (Unit* passenger = ObjectAccessor::GetUnit(*GetVehicleBase(), itr->second.Passenger))
                o << passenger->GetGUID() << ", ";
        sLog->outInfo(LOG_FILTER_UNITS, "%s", o.str().c_str());
    }

    if (GetVehicle())
        sLog->outInfo(LOG_FILTER_UNITS, "On vehicle %u.", GetVehicleBase()->GetEntry());
}

uint32 Unit::GetRemainingPeriodicAmount(uint64 caster, uint32 spellId, AuraType auraType, uint8 effectIndex) const
{
    uint32 amount = 0;
    AuraEffectList const& periodicAuras = GetAuraEffectsByType(auraType);
    for (AuraEffectList::const_iterator i = periodicAuras.begin(); i != periodicAuras.end(); ++i)
    {
        if ((*i)->GetCasterGUID() != caster || (*i)->GetId() != spellId || (*i)->GetEffIndex() != effectIndex || !(*i)->GetTotalTicks())
            continue;
        amount += uint32(((*i)->GetAmount() * std::max<int32>((*i)->GetTotalTicks() - int32((*i)->GetTickNumber()), 0)) / (*i)->GetTotalTicks());
        break;
    }

    return amount;
}

void Unit::SendClearTarget()
{
    WorldPacket data(SMSG_BREAK_TARGET, GetPackGUID().size());
    data.append(GetPackGUID());
    SendMessageToSet(&data, false);
}

uint32 Unit::GetResistance(SpellSchoolMask mask) const
{
    int32 resist = -1;
    for (int i = SPELL_SCHOOL_NORMAL; i < MAX_SPELL_SCHOOL; ++i)
        if (mask & (1 << i) && (resist < 0 || resist > int32(GetResistance(SpellSchools(i)))))
            resist = int32(GetResistance(SpellSchools(i)));

    // resist value will never be negative here
    return uint32(resist);
}

void CharmInfo::SetIsCommandAttack(bool val)
{
    _isCommandAttack = val;
}

bool CharmInfo::IsCommandAttack()
{
    return _isCommandAttack;
}

void CharmInfo::SetIsCommandFollow(bool val)
{
    _isCommandFollow = val;
}

bool CharmInfo::IsCommandFollow()
{
    return _isCommandFollow;
}

void CharmInfo::SaveStayPosition()
{
    //! At this point a new spline destination is enabled because of Unit::StopMoving()
    G3D::Vector3 const stayPos = _unit->movespline->FinalDestination();
    _stayX = stayPos.x;
    _stayY = stayPos.y;
    _stayZ = stayPos.z;
}

void CharmInfo::GetStayPosition(float &x, float &y, float &z)
{
    x = _stayX;
    y = _stayY;
    z = _stayZ;
}

void CharmInfo::SetIsAtStay(bool val)
{
    _isAtStay = val;
}

bool CharmInfo::IsAtStay()
{
    return _isAtStay;
}

void CharmInfo::SetIsFollowing(bool val)
{
    _isFollowing = val;
}

bool CharmInfo::IsFollowing()
{
    return _isFollowing;
}

void CharmInfo::SetIsReturning(bool val)
{
    _isReturning = val;
}

bool CharmInfo::IsReturning()
{
    return _isReturning;
}

void Unit::OnEclipseBuff(bool sun)
{
    // Nature's Grace
    this->ToPlayer()->RemoveSpellCooldown(16886);

    // Euphoria
    SpellInfo const* euphoria_spell = NULL;
    if (HasAura(81062))
        euphoria_spell = sSpellMgr->GetSpellInfo(81062);
    else if (HasAura(81061))
        euphoria_spell = sSpellMgr->GetSpellInfo(81061);
    if (euphoria_spell)
    {
        int32 bp0 = euphoria_spell->Effects[2].BasePoints;
        CastCustomSpell(this, 81070, &bp0, NULL, NULL, true);
    }

    // Item - Druid T11 Balance 4P Bonus
    if (HasAura(90163))
    {
        for (int i = 0; i < 3; ++i)
            CastSpell(this, 90164, true);
    }
}

void Unit::SetInFront(Unit const* target)
{
    if (!HasUnitState(UNIT_STATE_CANNOT_TURN))
        SetOrientation(GetAngle(target));
}

void Unit::SetFacingTo(float ori)
{
    Movement::MoveSplineInit init(*this);
    init.MoveTo(GetPositionX(), GetPositionY(), GetPositionZMinusOffset());
    init.SetFacing(ori);
    init.Launch();
}

void Unit::SetFacingToObject(WorldObject* object)
{
    // never face when already moving
    if (!IsStopped())
        return;

    // TODO: figure out under what conditions creature will move towards object instead of facing it where it currently is.
    SetFacingTo(GetAngle(object));
}

bool Unit::SetWalk(bool enable)
{
    if (enable == IsWalking())
        return false;

    if (enable)
        AddUnitMovementFlag(MOVEMENTFLAG_WALKING);
    else
        RemoveUnitMovementFlag(MOVEMENTFLAG_WALKING);

    return true;
}

bool Unit::SetDisableGravity(bool disable, bool /*packetOnly = false*/)
{
    if (disable == IsLevitating())
        return false;

    if (disable)
        AddUnitMovementFlag(MOVEMENTFLAG_DISABLE_GRAVITY);
    else
        RemoveUnitMovementFlag(MOVEMENTFLAG_DISABLE_GRAVITY);

    return true;
}

bool Unit::SetHover(bool enable)
{
    if (enable == HasUnitMovementFlag(MOVEMENTFLAG_HOVER))
        return false;

    if (enable)
    {
        //! No need to check height on ascent
        AddUnitMovementFlag(MOVEMENTFLAG_HOVER);
        if (float hh = GetFloatValue(UNIT_FIELD_HOVERHEIGHT))
            UpdateHeight(GetPositionZ() + hh);
    }
    else
    {
        RemoveUnitMovementFlag(MOVEMENTFLAG_HOVER);
        if (float hh = GetFloatValue(UNIT_FIELD_HOVERHEIGHT))
        {
            float newZ = GetPositionZ() - hh;
            UpdateAllowedPositionZ(GetPositionX(), GetPositionY(), newZ);
            UpdateHeight(newZ);
        }
    }

    return true;
}

void Unit::SendMovementHover(bool apply)
{
    if (GetTypeId() == TYPEID_PLAYER)
        ToPlayer()->SendMovementSetHover(HasUnitMovementFlag(MOVEMENTFLAG_HOVER));

    ObjectGuid guid = GetGUID();

    WorldPacket data;
    if (apply)
    {
        data.Initialize(SMSG_SPLINE_MOVE_SET_HOVER, 1 + 8);
        data
            .WriteByteMask(guid[3])
            .WriteByteMask(guid[7])
            .WriteByteMask(guid[0])
            .WriteByteMask(guid[1])
            .WriteByteMask(guid[4])
            .WriteByteMask(guid[6])
            .WriteByteMask(guid[2])
            .WriteByteMask(guid[5])

            .WriteByteSeq(guid[2])
            .WriteByteSeq(guid[4])
            .WriteByteSeq(guid[3])
            .WriteByteSeq(guid[1])
            .WriteByteSeq(guid[7])
            .WriteByteSeq(guid[0])
            .WriteByteSeq(guid[5])
            .WriteByteSeq(guid[6]);
    }
    else
    {
        data.Initialize(SMSG_SPLINE_MOVE_UNSET_HOVER, 1 + 8);
        data
            .WriteByteMask(guid[6])
            .WriteByteMask(guid[7])
            .WriteByteMask(guid[4])
            .WriteByteMask(guid[0])
            .WriteByteMask(guid[3])
            .WriteByteMask(guid[1])
            .WriteByteMask(guid[5])
            .WriteByteMask(guid[2])

            .WriteByteSeq(guid[4])
            .WriteByteSeq(guid[5])
            .WriteByteSeq(guid[3])
            .WriteByteSeq(guid[0])
            .WriteByteSeq(guid[2])
            .WriteByteSeq(guid[7])
            .WriteByteSeq(guid[6])
            .WriteByteSeq(guid[1]);
    }

    SendMessageToSet(&data, false);
}

void Unit::SendMovementWaterWalking()
{
    if (GetTypeId() == TYPEID_PLAYER)
        ToPlayer()->SendMovementSetWaterWalking(HasUnitMovementFlag(MOVEMENTFLAG_WATERWALKING));

    ObjectGuid guid = GetGUID();
    WorldPacket data(SMSG_SPLINE_MOVE_WATER_WALK, 1 + 8);
    data
        .WriteByteMask(guid[6])
        .WriteByteMask(guid[1])
        .WriteByteMask(guid[4])
        .WriteByteMask(guid[2])
        .WriteByteMask(guid[3])
        .WriteByteMask(guid[7])
        .WriteByteMask(guid[5])
        .WriteByteMask(guid[0])

        .WriteByteSeq(guid[0])
        .WriteByteSeq(guid[6])
        .WriteByteSeq(guid[3])
        .WriteByteSeq(guid[7])
        .WriteByteSeq(guid[4])
        .WriteByteSeq(guid[2])
        .WriteByteSeq(guid[5])
        .WriteByteSeq(guid[1]);

    SendMessageToSet(&data, false);
}

void Unit::SendMovementFeatherFall()
{
    if (GetTypeId() == TYPEID_PLAYER)
        ToPlayer()->SendMovementSetFeatherFall(HasUnitMovementFlag(MOVEMENTFLAG_FALLING_SLOW));

    ObjectGuid guid = GetGUID();
    WorldPacket data(SMSG_SPLINE_MOVE_SET_FEATHER_FALL, 1 + 8);
    data
        .WriteByteMask(guid[3])
        .WriteByteMask(guid[2])
        .WriteByteMask(guid[7])
        .WriteByteMask(guid[5])
        .WriteByteMask(guid[4])
        .WriteByteMask(guid[6])
        .WriteByteMask(guid[1])
        .WriteByteMask(guid[0])

        .WriteByteSeq(guid[1])
        .WriteByteSeq(guid[4])
        .WriteByteSeq(guid[7])
        .WriteByteSeq(guid[6])
        .WriteByteSeq(guid[2])
        .WriteByteSeq(guid[0])
        .WriteByteSeq(guid[5])
        .WriteByteSeq(guid[3]);

    SendMessageToSet(&data, false);
}

void Unit::SendMovementCanFlyChange()
{
    /*!
        if ( a3->MoveFlags & MOVEMENTFLAG_CAN_FLY )
        {
            v4->MoveFlags |= 0x1000000u;
            result = 1;
        }
        else
        {
            if ( v4->MoveFlags & MOVEMENTFLAG_FLYING )
                CMovement::DisableFlying(v4);
            v4->MoveFlags &= 0xFEFFFFFFu;
            result = 1;
        }
    */

    if (GetTypeId() == TYPEID_PLAYER)
        ToPlayer()->SendMovementSetCanFly(CanFly());

    ObjectGuid guid = GetGUID();

    WorldPacket data;
    if (CanFly())
    {
        data.Initialize(SMSG_SPLINE_MOVE_SET_FLYING, 1 + 8);
        data
            .WriteByteMask(guid[0])
            .WriteByteMask(guid[4])
            .WriteByteMask(guid[1])
            .WriteByteMask(guid[6])
            .WriteByteMask(guid[7])
            .WriteByteMask(guid[2])
            .WriteByteMask(guid[3])
            .WriteByteMask(guid[5])

            .WriteByteSeq(guid[7])
            .WriteByteSeq(guid[0])
            .WriteByteSeq(guid[5])
            .WriteByteSeq(guid[6])
            .WriteByteSeq(guid[4])
            .WriteByteSeq(guid[1])
            .WriteByteSeq(guid[3])
            .WriteByteSeq(guid[2]);
    }
    else
    {
        data.Initialize(SMSG_SPLINE_MOVE_UNSET_FLYING, 1 + 8);
        data
            .WriteByteMask(guid[5])
            .WriteByteMask(guid[0])
            .WriteByteMask(guid[4])
            .WriteByteMask(guid[7])
            .WriteByteMask(guid[2])
            .WriteByteMask(guid[3])
            .WriteByteMask(guid[1])
            .WriteByteMask(guid[6])

            .WriteByteSeq(guid[7])
            .WriteByteSeq(guid[2])
            .WriteByteSeq(guid[3])
            .WriteByteSeq(guid[4])
            .WriteByteSeq(guid[5])
            .WriteByteSeq(guid[1])
            .WriteByteSeq(guid[6])
            .WriteByteSeq(guid[0]);
    }

    SendMessageToSet(&data, false);
}

bool Unit::IsSplineEnabled() const
{
    return movespline->Initialized() && !movespline->Finalized();
}

void Unit::FocusTarget(Spell const* focusSpell, uint64 target)
{
    // already focused
    if (_focusSpell)
        return;

    _focusSpell = focusSpell;
    SetUInt64Value(UNIT_FIELD_TARGET, target);
    if (focusSpell->GetSpellInfo()->AttributesEx5 & SPELL_ATTR5_DONT_TURN_DURING_CAST)
        AddUnitState(UNIT_STATE_ROTATING);
}

void Unit::ReleaseFocus(Spell const* focusSpell)
{
    // focused to something else
    if (focusSpell != _focusSpell)
        return;

    _focusSpell = NULL;
    if (Unit* victim = getVictim())
        SetUInt64Value(UNIT_FIELD_TARGET, victim->GetGUID());
    else
        SetUInt64Value(UNIT_FIELD_TARGET, 0);

    if (focusSpell->GetSpellInfo()->AttributesEx5 & SPELL_ATTR5_DONT_TURN_DURING_CAST)
        ClearUnitState(UNIT_STATE_ROTATING);
}

bool Unit::IsVisionObscured(Unit* victim, SpellInfo const* spellInfo) const
{
    Aura* victimAura = NULL;
    Unit* victimCaster = NULL;
    Aura* myAura = NULL;
    Unit* myCaster = NULL;

    AuraEffectList const& vAuras = victim->GetAuraEffectsByType(SPELL_AURA_INTERFERE_TARGETTING);
    for (AuraEffectList::const_iterator i = vAuras.begin(); i != vAuras.end(); ++i)
    {
        victimAura = (*i)->GetBase();
        victimCaster = victimAura->GetCaster();
        break;
    }

    AuraEffectList const& myAuras = GetAuraEffectsByType(SPELL_AURA_INTERFERE_TARGETTING);
    for (AuraEffectList::const_iterator i = myAuras.begin(); i != myAuras.end(); ++i)
    {
        myAura = (*i)->GetBase();
        myCaster = myAura->GetCaster();
        break;
    }

    if ((myAura != NULL && myCaster == NULL) || (victimAura != NULL && victimCaster == NULL))
        return false; // Failed auras, will result in crash

    // E.G. Victim is in smoke bomb, and I'm not
    // Spells fail unless I'm friendly to the caster of victim's smoke bomb
    if (victimAura != NULL && myAura == NULL)
    {
        if (IsFriendlyTo(victimCaster) || spellInfo->GetMaxRange(false) <= 5.0f)
            return false;
        else
            return true;
    }
    // Victim is not in smoke bomb, while I am
    // Spells fail if my smoke bomb aura's caster is my enemy
    else if (myAura != NULL && victimAura == NULL)
    {
        if (IsFriendlyTo(myCaster))
            return false;
        else
            return true;
    }
    // Victim and caster have aura with effect SPELL_AURA_INTERFERE_TARGETTING, but aura may be not smoke bomb.
    else if (myAura != NULL && victimAura != NULL)
    {
        for (uint8 i = 0; i < MAX_SPELL_EFFECTS; ++i)
        {
            if (AuraEffect* effect = myAura->GetEffect(i))
            {
                if (effect->GetAuraType() != SPELL_AURA_INTERFERE_TARGETTING)
                    continue;
                if (effect->GetAmount() && !IsFriendlyTo(victimCaster))
                    return true;
            }
        }
    }
    // Victim and caster have aura with effect SPELL_AURA_INTERFERE_TARGETTING, but aura may be not smoke bomb.
    else if (myAura != NULL && victimAura != NULL)
    {
        for (uint8 i = 0; i < MAX_SPELL_EFFECTS; ++i)
        {
            if (AuraEffect* effect = myAura->GetEffect(i))
            {
                if (effect->GetAuraType() != SPELL_AURA_INTERFERE_TARGETTING)
                    continue;
                if (effect->GetAmount() && !IsFriendlyTo(victimCaster))
                    return true;
            }
        }
    }
    return false;
}

Unit* Unit::GetProcOwner()
{
    if (ToPet() && ToPet()->GetOwner())
        return ToPet()->GetOwner();

    if (ToTempSummon() && ToTempSummon()->GetOwner())
        return ToTempSummon()->GetOwner();

    if (this->isGuardian())
        return GetCharmerOrOwner();

    return this;
}

void Unit::DoBuffPartyOrSingle(uint32 single_buff, uint32 party_buff, Unit *target)
{
    bool Continue = false;
    if (GetTypeId() == TYPEID_PLAYER)
    {
        uint32 player = 0;
        std::list<Unit*> PartyMembers;
        GetPartyMembers(PartyMembers);
        for(std::list<Unit*>::iterator itr = PartyMembers.begin(); itr != PartyMembers.end(); ++itr) // If caster is in party with a player
        {
            ++player;
            if (Continue == false && player > 1)
                Continue = true;
        }
    }
    if (Continue == true)
        CastSpell(target, party_buff, true);
    else
        CastSpell(target, single_buff, true);
}

void Unit::SpreadAura(uint32 spellId, float radius, bool positive, int8 count, Unit * except)
{
    UnitList targets;
    Trinity::AnyUnfriendlyAttackableVisibleUnitInObjectRangeCheck u_check(this, radius);
    Trinity::UnitListSearcher<Trinity::AnyUnfriendlyAttackableVisibleUnitInObjectRangeCheck> searcher(this, targets, u_check);
    this->VisitNearbyObject(radius, searcher);

    UnitList targets_limited; // list of targets that hasn't aura
    if (count == -1)
    {
        targets_limited = targets;
    }
    else
    {
        for (UnitList::iterator itr = targets.begin(); itr != targets.end(); )
        {
            Unit *unit_itr = *itr;
            if ((!except || except != unit_itr) && !unit_itr->HasAura(spellId))
            {
                targets_limited.push_back(unit_itr);
                itr = targets.erase(itr);

                // break if we have full needed list
                if (--count == 0)
                    break;

                continue;
            }

            ++itr;
        }
    }

    // apply aura
    for (UnitList::iterator itr = targets_limited.begin(); itr != targets_limited.end(); ++itr)
    {
        AddAura(spellId, *itr);
    }

    if (count > 0)
    {
        for (UnitList::iterator itr = targets.begin(); itr != targets.end(); ++itr)
        {
            Unit *unit_itr = *itr;
            if (unit_itr == except)
                continue;

            AddAura(spellId, *itr);

            if (--count == 0)
                break;
        }
    }
}

void Unit::RemoveBattlegroundStartingAuras()
{
    // remove auras with duration lower than 30s
    AuraApplicationMap &auraMap = GetAppliedAuras();
    for (Unit::AuraApplicationMap::iterator iter = auraMap.begin(); iter != auraMap.end();)
    {
        AuraApplication* aurApp = iter->second;
        Aura* aura = aurApp->GetBase();
        if (!aura->IsPermanent()
            && aura->GetDuration() <= 30*IN_MILLISECONDS
            && aurApp->IsPositive()
            && (!(aura->GetSpellInfo()->Attributes & SPELL_ATTR0_UNAFFECTED_BY_INVULNERABILITY))
            && (!aura->HasEffectType(SPELL_AURA_MOD_INVISIBILITY)))
            RemoveAura(iter);
        else
            ++iter;
    }
}



void Unit::ReadMovementInfo(WorldPacket& data, MovementInfo* mi)
{
    if (GetTypeId() != TYPEID_PLAYER)
        return;

    bool hasMovementFlags = false;
    bool hasMovementFlags2 = false;
    bool hasTimestamp = false;
    bool hasTransportData = false;

    MovementStatusElements* sequence = GetMovementStatusElementsSequence(data.GetOpcode());
    if (sequence == NULL)
    {
        sLog->outError(LOG_FILTER_NETWORKIO, "WorldSession::ReadMovementInfo: No movement sequence found for opcode 0x%04X", uint32(data.GetOpcode()));
        return;
    }

    ObjectGuid guid;
    ObjectGuid tguid;

    for (uint32 i = 0; i < MSE_COUNT; ++i)
    {
        MovementStatusElements element = sequence[i];
        if (element == MSEEnd)
            break;

        if (element >= MSEHasGuidByte0 && element <= MSEHasGuidByte7)
        {
            data.ReadByteMask(guid[element - MSEHasGuidByte0]);
            continue;
        }

        if (element >= MSEHasTransportGuidByte0 &&
            element <= MSEHasTransportGuidByte7)
        {
            if (hasTransportData)
                data.ReadByteMask(tguid[element - MSEHasTransportGuidByte0]);
            continue;
        }

        if (element >= MSEGuidByte0 && element <= MSEGuidByte7)
        {
            data.ReadByteSeq(guid[element - MSEGuidByte0]);
            continue;
        }

        if (element >= MSETransportGuidByte0 &&
            element <= MSETransportGuidByte7)
        {
            if (hasTransportData)
                data.ReadByteSeq(tguid[element - MSETransportGuidByte0]);
            continue;
        }

        switch (element)
        {
        case MSEHasMovementFlags:
            hasMovementFlags = !data.ReadBit();
            break;
        case MSEHasMovementFlags2:
            hasMovementFlags2 = !data.ReadBit();
            break;
        case MSEHasTimestamp:
            hasTimestamp = !data.ReadBit();
            break;
        case MSEHasOrientation:
            mi->pos.m_orientation = !data.ReadBit() ? 1.0f : 0.0f;
            break;
        case MSEHasTransportData:
            hasTransportData = data.ReadBit();
            break;
        case MSEHasTransportTime2:
            if (hasTransportData)
            {
                if (data.ReadBit())
                    mi->AddServerMovementFlag(SERVERMOVEFLAG_TRANSPORT_T2);
                else
                    mi->RemoveServerMovementFlag(SERVERMOVEFLAG_TRANSPORT_T2);
            }
            break;
        case MSEHasTransportTime3:
            if (hasTransportData)
            {
                if (data.ReadBit())
                    mi->AddServerMovementFlag(SERVERMOVEFLAG_TRANSPORT_T3);
                else
                    mi->RemoveServerMovementFlag(SERVERMOVEFLAG_TRANSPORT_T3);
            }
            break;
        case MSEHasPitch:
            mi->HavePitch = !data.ReadBit();
            break;
        case MSEHasFallData:
            if (data.ReadBit())
                mi->AddServerMovementFlag(SERVERMOVEFLAG_FALLDATA);
            else
                mi->RemoveServerMovementFlag(SERVERMOVEFLAG_FALLDATA);
            break;
        case MSEHasFallDirection:
            if (mi->HasServerMovementFlag(SERVERMOVEFLAG_FALLDATA))
            {
                if (data.ReadBit())
                    mi->AddServerMovementFlag(SERVERMOVEFLAG_FALLDIRECTION);
                else
                    mi->RemoveServerMovementFlag(SERVERMOVEFLAG_FALLDIRECTION);
            }                    
            break;
        case MSEHasSplineElevation:
            mi->HaveSplineElevation = !data.ReadBit();
            break;
        case MSEHasSpline:
            if (data.ReadBit())
                mi->AddServerMovementFlag(SERVERMOVEFLAG_SPLINE1);
            else
                mi->RemoveServerMovementFlag(SERVERMOVEFLAG_SPLINE1);
            break;
        case MSEHasSpline2:
            if (data.ReadBit())
                mi->AddServerMovementFlag(SERVERMOVEFLAG_SPLINE2);
            else
                mi->RemoveServerMovementFlag(SERVERMOVEFLAG_SPLINE2);
            break;
        case MSEMovementFlags:
            if (hasMovementFlags)
                mi->flags = data.ReadBits(30);
            break;
        case MSEMovementFlags2:
            if (hasMovementFlags2)
                mi->flags2 = data.ReadBits(12);
            break;
        case MSETimestamp:
            if (hasTimestamp)
                data >> mi->time;
            break;
        case MSEPositionX:
            data >> mi->pos.m_positionX;
            break;
        case MSEPositionY:
            data >> mi->pos.m_positionY;
            break;
        case MSEPositionZ:
            data >> mi->pos.m_positionZ;
            break;
        case MSEOrientation:
            if (mi->pos.HaveOrientation())
                data >> mi->pos.m_orientation;
            break;
        case MSETransportPositionX:
            if (hasTransportData)
                data >> mi->t_pos.m_positionX;
            break;
        case MSETransportPositionY:
            if (hasTransportData)
                data >> mi->t_pos.m_positionY;
            break;
        case MSETransportPositionZ:
            if (hasTransportData)
                data >> mi->t_pos.m_positionZ;
            break;
        case MSETransportOrientation:
            if (hasTransportData)
                data >> mi->t_pos.m_orientation;
            break;
        case MSETransportSeat:
            if (hasTransportData)
                data >> mi->t_seat;
            break;
        case MSETransportTime:
            if (hasTransportData)
                data >> mi->t_time;
            break;
        case MSETransportTime2:
            if (hasTransportData && mi->HasServerMovementFlag(SERVERMOVEFLAG_TRANSPORT_T2))
                data >> mi->t_time2;
            break;
        case MSETransportTime3:
            if (hasTransportData && mi->HasServerMovementFlag(SERVERMOVEFLAG_TRANSPORT_T3))
                data >> mi->t_time3;
            break;
        case MSEPitch:
            if (mi->HavePitch)
                data >> mi->pitch;
            break;
        case MSEFallTime:
            if (mi->HasServerMovementFlag(SERVERMOVEFLAG_FALLDATA))
                data >> mi->fallTime;
            break;
        case MSEFallVerticalSpeed:
            if (mi->HasServerMovementFlag(SERVERMOVEFLAG_FALLDATA))
                data >> mi->j_zspeed;
            break;
        case MSEFallCosAngle:
            if (mi->HasServerMovementFlag(SERVERMOVEFLAG_FALLDIRECTION))
                data >> mi->j_cosAngle;
            break;
        case MSEFallSinAngle:
            if (mi->HasServerMovementFlag(SERVERMOVEFLAG_FALLDIRECTION))
                data >> mi->j_sinAngle;
            break;
        case MSEFallHorizontalSpeed:
            if (mi->HasServerMovementFlag(SERVERMOVEFLAG_FALLDIRECTION))
                data >> mi->j_xyspeed;
            break;
        case MSESplineElevation:
            if (mi->HaveSplineElevation)
                data >> mi->splineElevation;
            break;
        case MSEZeroBit:
        case MSEOneBit:
            data.ReadBit();
            break;
        case MSEGenericDword0:
        case MSEGenericDword1:
        case MSEMovementCounter:
            data.read_skip<uint32>();
            break;
        case MSESpeed:
            data >> mi->speed;
            break;
        case MSE2Bits:
            data.ReadBits(2);
            break;
        default:
            ASSERT(false && "Incorrect sequence element detected at ReadMovementInfo");
            break;
        }
    }

    if (data.size() != data.rpos())
        sLog->outError(LOG_FILTER_GENERAL, "Error in ReadMovementInfo: Packet (opcode = %u) not fully readed! Size = %d, readed = %d, diff = %d", 
        uint32(data.GetOpcode()), data.size(), data.rpos(), data.size() - data.rpos());

    mi->guid = guid;
    mi->t_guid = tguid;

    if (hasTransportData && mi->pos.m_positionX != mi->t_pos.m_positionX)
        if (GetTransport())
            GetTransport()->UpdatePosition(mi);
}

void Unit::WriteMovementInfo(WorldPacket &data)
{
    MovementStatusElements* sequence = GetMovementStatusElementsSequence(data.GetOpcode());
    if (!sequence)
    {
        sLog->outError(LOG_FILTER_NETWORKIO, "WorldSession::WriteMovementInfo: No movement sequence found for opcode %s, 0x%04X", GetOpcodeNameForLogging(data.GetOpcode()).c_str(), uint32(data.GetOpcode()));
        return;
    }

    Unit* mover = GetCharmerGUID() ? GetCharmer() : this;
    if (Player const* player = ToPlayer())
        mover = player->m_mover;

    bool hasTransportData = mover->GetTransport() != NULL;
    bool hasTimestamp = GetTypeId() == TYPEID_PLAYER ? (mover->m_movementInfo.time != 0) : true;

    ObjectGuid guid = mover->GetGUID();
    ObjectGuid tguid = hasTransportData ? GetTransport()->GetGUID() : 0;

    mover->m_movementInfo.Normalize();

    for(uint32 i = 0; i < MSE_COUNT; ++i)
    {
        MovementStatusElements element = sequence[i];
        if (element == MSEEnd)
            break;

        if (element >= MSEHasGuidByte0 && element <= MSEHasGuidByte7)
        {
            data.WriteByteMask(guid[element - MSEHasGuidByte0]);
            continue;
        }

        if (element >= MSEHasTransportGuidByte0 &&
            element <= MSEHasTransportGuidByte7)
        {
            if (hasTransportData)
                data.WriteByteMask(tguid[element - MSEHasTransportGuidByte0]);
            continue;
        }

        if (element >= MSEGuidByte0 && element <= MSEGuidByte7)
        {
            data.WriteByteSeq(guid[element - MSEGuidByte0]);
            continue;
        }

        if (element >= MSETransportGuidByte0 &&
            element <= MSETransportGuidByte7)
        {
            if (hasTransportData)
                data.WriteByteSeq(tguid[element - MSETransportGuidByte0]);
            continue;
        }

        switch (element)
        {
            case MSEHasMovementFlags:
                data.WriteBit(mover->GetUnitMovementFlags() == 0);
                break;
            case MSEHasMovementFlags2:
                data.WriteBit(mover->GetExtraUnitMovementFlags() == 0);
                break;
            case MSEHasTimestamp:
                data.WriteBit(!hasTimestamp);
                break;
            case MSEHasOrientation:
                data.WriteBit(!mover->m_movementInfo.pos.HaveOrientation());
                break;
            case MSEHasTransportData:
                data.WriteBit(hasTransportData);
                break;
            case MSEHasTransportTime2:
                if (hasTransportData)
                    data.WriteBit(mover->m_movementInfo.HasServerMovementFlag(SERVERMOVEFLAG_TRANSPORT_T2));
                break;
            case MSEHasTransportTime3:
                if (hasTransportData)
                    data.WriteBit(mover->m_movementInfo.HasServerMovementFlag(SERVERMOVEFLAG_TRANSPORT_T3));
                break;
            case MSEHasPitch:
                data.WriteBit(!mover->m_movementInfo.HavePitch);
                break;
            case MSEHasFallData:
                data.WriteBit(mover->m_movementInfo.HasServerMovementFlag(SERVERMOVEFLAG_FALLDATA));
                break;
            case MSEHasFallDirection:
                if (mover->m_movementInfo.HasServerMovementFlag(SERVERMOVEFLAG_FALLDATA))
                    data.WriteBit(mover->m_movementInfo.HasServerMovementFlag(SERVERMOVEFLAG_FALLDIRECTION));
                break;
            case MSEHasSplineElevation:
                data.WriteBit(!mover->m_movementInfo.HaveSplineElevation);
                break;
            case MSEHasSpline:
                data.WriteBit(mover->m_movementInfo.HasServerMovementFlag(SERVERMOVEFLAG_SPLINE1));
                break;
            case MSEHasSpline2:
                data.WriteBit(mover->m_movementInfo.HasServerMovementFlag(SERVERMOVEFLAG_SPLINE2));
                break;
            case MSEMovementFlags:
                if (mover->GetUnitMovementFlags() != 0)
                    data.WriteBits(mover->GetUnitMovementFlags(), 30);
                break;
            case MSEMovementFlags2:
                if (mover->GetExtraUnitMovementFlags() != 0)
                    data.WriteBits(mover->GetExtraUnitMovementFlags(), 12);
                break;
            case MSETimestamp:
                if (hasTimestamp)
                    data << getMSTime()/*mover->m_movementInfo.time*/;
                break;
            case MSEPositionX:
                data << mover->m_movementInfo.pos.m_positionX;
                break;
            case MSEPositionY:
                data << mover->m_movementInfo.pos.m_positionY;
                break;
            case MSEPositionZ:
                data << mover->m_movementInfo.pos.m_positionZ;
                break;
            case MSEOrientation:
                if (mover->m_movementInfo.pos.HaveOrientation())
                    data << mover->m_movementInfo.pos.GetOrientation();
                break;
            case MSETransportPositionX:
                if (hasTransportData)
                    data << mover->m_movementInfo.t_pos.m_positionX;
                break;
            case MSETransportPositionY:
                if (hasTransportData)
                    data << mover->m_movementInfo.t_pos.m_positionY;
                break;
            case MSETransportPositionZ:
                if (hasTransportData)
                    data << mover->m_movementInfo.t_pos.m_positionZ;
                break;
            case MSETransportOrientation:
                if (hasTransportData)
                    data << mover->m_movementInfo.t_pos.GetOrientation();
                break;
            case MSETransportSeat:
                if (hasTransportData)
                    data << mover->m_movementInfo.t_seat;
                break;
            case MSETransportTime:
                if (hasTransportData)
                    data << mover->m_movementInfo.t_time;
                break;
            case MSETransportTime2:
                if (hasTransportData && mover->m_movementInfo.HasServerMovementFlag(SERVERMOVEFLAG_TRANSPORT_T2))
                    data << mover->m_movementInfo.t_time2;
                break;
            case MSETransportTime3:
                if (hasTransportData && mover->m_movementInfo.HasServerMovementFlag(SERVERMOVEFLAG_TRANSPORT_T3))
                    data << mover->m_movementInfo.t_time3;
                break;
            case MSEPitch:
                if (mover->m_movementInfo.HavePitch)
                    data << mover->m_movementInfo.pitch;
                break;
            case MSEFallTime:
                if (mover->m_movementInfo.HasServerMovementFlag(SERVERMOVEFLAG_FALLDATA))
                    data << mover->m_movementInfo.fallTime;
                break;
            case MSEFallVerticalSpeed:
                if (mover->m_movementInfo.HasServerMovementFlag(SERVERMOVEFLAG_FALLDATA))
                    data << mover->m_movementInfo.j_zspeed;
                break;
            case MSEFallCosAngle:
                if (mover->m_movementInfo.HasServerMovementFlag(SERVERMOVEFLAG_FALLDIRECTION))
                    data << mover->m_movementInfo.j_cosAngle;
                break;
            case MSEFallSinAngle:
                if (mover->m_movementInfo.HasServerMovementFlag(SERVERMOVEFLAG_FALLDIRECTION))
                    data << mover->m_movementInfo.j_sinAngle;
                break;
            case MSEFallHorizontalSpeed:
                if (mover->m_movementInfo.HasServerMovementFlag(SERVERMOVEFLAG_FALLDIRECTION))
                    data << mover->m_movementInfo.j_xyspeed;
                break;
            case MSESplineElevation:
                if (mover->m_movementInfo.HaveSplineElevation)
                    data << mover->m_movementInfo.splineElevation;
                break;
            case MSEZeroBit:
            case MSEOneBit:
                data.WriteBit(element == MSEOneBit);
                break;
            case MSEGenericDword0:
            case MSEGenericDword1:
                data << uint32(0); // counter
                break;
            case MSEMovementCounter:
                data << uint32(0); // counter
                break;
            case MSESpeed:
                data << mover->m_movementInfo.speed;
                break;
            case MSE2Bits:
                data.WriteBits(0, 2);
                break;
            default:
                ASSERT(false && "Incorrect sequence element detected at ReadMovementInfo");
                break;
        }
    }
}

void Unit::CastWithDelay(uint32 delay, Unit* victim, uint32 spellid, bool triggered, bool repeat)
{
    class CastDelayEvent : public BasicEvent
    {
    public:
        CastDelayEvent(Unit* _me, Unit* _victim, uint32 const& _spellId, bool const& _triggered, bool const& _repeat, uint32 const& _delay) :
            me(_me), victim(_victim), spellId(_spellId), triggered(_triggered), repeat(_repeat), delay(_delay) { }

        bool Execute(uint64 /*execTime*/, uint32 /*diff*/)
        {
            me->CastSpell(victim, spellId, triggered);
            if (repeat)
                me->CastWithDelay(delay, victim, spellId, triggered, repeat);
            return true;
        }

    private:
        Unit* me;
        Unit* victim;
        uint32 const spellId;
        uint32 const delay;
        bool const triggered;
        bool const repeat;
    };

    m_Events.AddEvent(new CastDelayEvent(this, victim, spellid, triggered, repeat, delay), m_Events.CalculateTime(delay));
}

void Unit::AddSpellCooldown(uint32 spellid, uint32 itemid, time_t end_time)
{
    SpellCooldown sc;
    sc.end = end_time;
    sc.itemid = itemid;
    m_spellCooldowns[spellid] = sc;
}
