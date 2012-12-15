/*
 * Copyright (C) 2008-2012 TrinityCore <http://www.trinitycore.org/>
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

enum CompanionEntries
{
    // Clockwork Rocket Bot
    COMPANION_CLOCKWORK_ROCKET_BOT      = 24968,
    COMPANION_BLUE_CLOCKWORK_ROCKET_BOT = 40295,
};

enum CompanionSpells
{
    // Clockwork Rocket Bot
    SPELL_ROCKET_BOT_ATTACK             = 45269,
};

uint32 const rocketBotsList[2] =
{
    COMPANION_CLOCKWORK_ROCKET_BOT,
    COMPANION_BLUE_CLOCKWORK_ROCKET_BOT
};

class npc_clockwork_rocket_bot : public CreatureScript
{
    public:
        npc_clockwork_rocket_bot() : CreatureScript("npc_clockwork_rocket_bot") { }

        struct npc_clockwork_rocket_botAI : public ScriptedAI
        {
            npc_clockwork_rocket_botAI(Creature* creature) : ScriptedAI(creature), _checkTimer(1000) 
            {
            }

            void UpdateAI(uint32 const diff)
            {
                if (_checkTimer <= diff)
                {
                    for (uint8 i = 0; i < 2; i++)
                    {
                        std::list<Creature*> rocketBots;
                        me->GetCreatureListWithEntryInGrid(rocketBots, rocketBotsList[i], 15.0f);
                        if (!rocketBots.empty())
                            for (std::list<Creature*>::const_iterator itr = rocketBots.begin(); itr != rocketBots.end(); ++itr)
                            {
                                Creature* rocketBot = *itr;
                                if (rocketBot && rocketBot != me)
                                    DoCast(rocketBot, SPELL_ROCKET_BOT_ATTACK);
                            }
                    }
                    _checkTimer = 5000;
                }
                else 
                    _checkTimer -= diff;
            }

        private:
              uint32 _checkTimer;
        };

        CreatureAI* GetAI(Creature* creature) const
        {
            return new npc_clockwork_rocket_botAI(creature);
        }
};

void AddSC_npc_companions()
{
    new npc_clockwork_rocket_bot();
}
