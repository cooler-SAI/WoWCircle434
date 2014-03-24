#include "ScriptPCH.h"
#include "dragon_soul.h"


// send SendPlaySpellVisualKit from limbs
// 22445 + 0 before casting visual spell
// 22446 + 0 after visual spell
enum ScriptedTexts
{
};

enum Spells
{
    
    // Thrall
    SPELL_ASTRAL_RECALL             = 108537, 

    SPELL_LIMB_EMERGE_VISUAL        = 107991,

    SPELL_IDLE                      = 106187, // tail tentacle has it

    SPELL_TRIGGER_ASPECT_BUFFS      = 106943, // casted by deathwing 56173
    SPELL_SHARE_HEALTH              = 109547, // casted by deathwing 56173 on self ?
    SPELL_ASSAULT_ASPECTS           = 107018, // casted by deathwing 56173

    SPELL_CRASH                     = 109628, // casted by mutated tentacle and tail tentacle ?

    SPELL_ACHIEVEMENT               = 111533,

    // Jump Pad
    SPELL_CARRYING_WINDS_1          = 106663, // casted by player, from 1 to 2
    SPELL_CARRYING_WINDS_SCRIPT_1   = 106666, // casted by pad on player
    SPELL_CARRYING_WINDS_2          = 106668, // from 2 to 1
    SPELL_CARRYING_WINDS_SCRIPT_2   = 106669,
    SPELL_CARRYING_WINDS_3          = 106670, // from 2 to 3
    SPELL_CARRYING_WINDS_SCRIPT_3   = 106671,
    SPELL_CARRYING_WINDS_4          = 106672, // from 3 to 2
    SPELL_CARRYING_WINDS_SCRIPT_4   = 106673,
    SPELL_CARRYING_WINDS_5          = 106674, // from 3 to 4
    SPELL_CARRYING_WINDS_SCRIPT_5   = 106675,
    SPELL_CARRYING_WINDS_6          = 106676, // from 4 to 3
    SPELL_CARRYING_WINDS_SCRIPT_6   = 106677,

    SPELL_CARRYING_WINDS_DUMMY      = 106678, // visual ?

    SPELL_CARRYING_WINDS_SPEED_10   = 106664,
    SPELL_CARRYING_WINDS_SPEED_25   = 109963,
    SPELL_CARRYING_WINDS_SPEED_10H  = 109962,
    SPELL_CARRYING_WINDS_SPEED_25H  = 109961,
};

enum Adds
{
    NPC_ALEXSTRASZA_DRAGON  = 56099, // 1
    NPC_NOZDORMU_DRAGON     = 56102, // 2
    NPC_YSERA_DRAGON        = 56100, // 3
    NPC_KALECGOS_DRAGON     = 56101, // 4

    NPC_WING_TENTACLE       = 56168, // 1 & 4
    NPC_ARM_TENTACLE_1      = 56846, // 2
    NPC_ARM_TENTACLE_2      = 56167, // 3

    NPC_MUTATED_CORRUPTION  = 56471,
    NPC_BLISTERING_TENTACLE = 56188,

    NPC_DEATHWING           = 56173, // at the eye
    NPC_DEATHWING_1         = 57962, // invisible ?

    NPC_COSMETIC_TENTACLE   = 57693,
    NPC_TAIL_TENTACLE       = 56844,

    NPC_JUMP_PAD            = 56699,
};

const Position mutatedcorruptionPos[4] = 
{
    {-12107.4f, 12201.9f, -5.32397f, 5.16617f},
    {-12028.8f, 12265.6f, -6.27147f, 4.13643f},
    {-11993.3f, 12286.3f, -2.58115f, 5.91667f},
    {-12160.9f, 12057.0f, 2.47362f, 0.733038f}
};

const Position thrallPos = {-12128.3f, 12253.8f, 0.0451f, 0.0f}; // Thrall teleports here

const Position jumpPos[] = 
{
    {-12026.30f, 12223.31f, -6.05f, 0.0f}, // from 1 to 2
    {-11977.64f, 12268.25f, 1.39f, 0.0f}, // from 2 to 1
    {-12086.50f, 12167.23f, -2.64f, 0.0f}, // from 2 to 3
    {-12051.42f, 12218.58f, -5.93f, 0.0f}, // from 3 to 2
    {-12118.76f, 12079.09f, 2.40f, 0.0f}, // from 3 to 4
    {-12099.18f, 12147.16f, -2.64f, 0.0f} // from 4 to 3
};

class npc_madness_of_deathwing_jump_pad : public CreatureScript
{
    public:
        npc_madness_of_deathwing_jump_pad() : CreatureScript("npc_madness_of_deathwing_jump_pad") { }

        CreatureAI* GetAI(Creature* pCreature) const
        {
            return new npc_madness_of_deathwing_jump_padAI(pCreature);
        }

        struct npc_madness_of_deathwing_jump_padAI : public Scripted_NoMovementAI
        {
            npc_madness_of_deathwing_jump_padAI(Creature* pCreature) : Scripted_NoMovementAI(pCreature)
            {
                me->SetReactState(REACT_PASSIVE);
                checkTimer = 500;
                pos = 0;
                spellId = 0;
            }

            void Reset()
            {
                if (me->GetPositionY() >= 12270.0f && me->GetPositionY() <= 12275.0f)
                {
                    pos = 1;
                    spellId = SPELL_CARRYING_WINDS_SCRIPT_1;
                }
                else if (me->GetPositionY() >= 12225.0f && me->GetPositionY() <= 12230.0f)
                {
                    pos = 2;
                    spellId = SPELL_CARRYING_WINDS_SCRIPT_2;
                }
                else if (me->GetPositionY() >= 12210.0f && me->GetPositionY() <= 12215.0f)
                {
                    pos = 3;
                    spellId = SPELL_CARRYING_WINDS_SCRIPT_3;
                }
                else if (me->GetPositionY() >= 12162.0f && me->GetPositionY() <= 12167.0f)
                {
                    pos = 4;
                    spellId = SPELL_CARRYING_WINDS_SCRIPT_4;
                }
                else if (me->GetPositionY() >= 12150.0f && me->GetPositionY() <= 12155.0f)
                {
                    pos = 5;
                    spellId = SPELL_CARRYING_WINDS_SCRIPT_5;
                }
                else if (me->GetPositionY() >= 12080.9f && me->GetPositionY() <= 12085.0f)
                {
                    pos = 6;
                    spellId = SPELL_CARRYING_WINDS_SCRIPT_6;
                }
            }

            void UpdateAI(const uint32 diff)
            {
                if (!spellId)
                    return;

                if (checkTimer <= diff)
                {
                    std::list<Player*> players;
                    PlayerCheck check(me);
                    Trinity::PlayerListSearcher<PlayerCheck> searcher(me, players, check);
                    me->VisitNearbyObject(32.0f, searcher);
                    if (!players.empty())
                        for (std::list<Player*>::const_iterator itr = players.begin(); itr != players.end(); ++ itr)
                            DoCast((*itr), spellId, true);

                    checkTimer = 500;
                }
                else
                    checkTimer -= diff;
            }

        private:
            uint32 checkTimer;
            uint8 pos;
            uint32 spellId;

            class PlayerCheck
            {
                public:
                    PlayerCheck(WorldObject const* obj) : _obj(obj) {}
                    bool operator()(Player* u)
                    {
                        if (!u->isAlive())
                            return false;

                        if (!u->IsFalling() || _obj->GetPositionZ() < (u->GetPositionZ() + 3.0f))
                            return false;

                        if (!_obj->IsWithinDistInMap(u, 32.0f))
                            return false;

                        return true;
                    }

                private:
                    WorldObject const* _obj;
            };
        };
};

class spell_madness_of_deathwing_carrying_winds_script : public SpellScriptLoader
{
    public:
        spell_madness_of_deathwing_carrying_winds_script(const char* name, uint8 pos) : SpellScriptLoader(name), _pos(pos) { }

        class spell_madness_of_deathwing_carrying_winds_script_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_madness_of_deathwing_carrying_winds_script_SpellScript);

            void HandleDummy(SpellEffIndex /*effIndex*/)
            {
                if (!GetCaster() || !GetHitUnit())
                    return;

                uint32 spellId = SPELL_CARRYING_WINDS_1;
                switch (_pos)
                {
                    case 1: spellId = SPELL_CARRYING_WINDS_1; break;
                    case 2: spellId = SPELL_CARRYING_WINDS_2; break;
                    case 3: spellId = SPELL_CARRYING_WINDS_3; break;
                    case 4: spellId = SPELL_CARRYING_WINDS_4; break;
                    case 5: spellId = SPELL_CARRYING_WINDS_5; break;
                    case 6: spellId = SPELL_CARRYING_WINDS_6; break;
                    default: break;
                }

                GetHitUnit()->CastSpell(jumpPos[_pos - 1].GetPositionX(), jumpPos[_pos - 1].GetPositionY(), jumpPos[_pos - 1].GetPositionZ(), spellId, true);
               
            }

            void Register()
            {
                OnEffectHitTarget += SpellEffectFn(spell_madness_of_deathwing_carrying_winds_script_SpellScript::HandleDummy, EFFECT_0, SPELL_EFFECT_SCRIPT_EFFECT);
            }

        public:
            spell_madness_of_deathwing_carrying_winds_script_SpellScript(uint8 pos) : SpellScript(), _pos(pos) {} 

        private:
            uint8 _pos;
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_madness_of_deathwing_carrying_winds_script_SpellScript(_pos);
        }

    private:
        uint8 _pos;
};

void AddSC_madness_of_deathwing()
{
    new npc_madness_of_deathwing_jump_pad();
    new spell_madness_of_deathwing_carrying_winds_script("spell_madness_of_deathwing_carrying_winds_script_1", 1);
    new spell_madness_of_deathwing_carrying_winds_script("spell_madness_of_deathwing_carrying_winds_script_2", 2);
    new spell_madness_of_deathwing_carrying_winds_script("spell_madness_of_deathwing_carrying_winds_script_3", 3);
    new spell_madness_of_deathwing_carrying_winds_script("spell_madness_of_deathwing_carrying_winds_script_4", 4);
    new spell_madness_of_deathwing_carrying_winds_script("spell_madness_of_deathwing_carrying_winds_script_5", 5);
    new spell_madness_of_deathwing_carrying_winds_script("spell_madness_of_deathwing_carrying_winds_script_6", 6);
}