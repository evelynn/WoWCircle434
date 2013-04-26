#include "ScriptPCH.h"
#include "Vehicle.h"
#include "firelands.h"
#include "boss_lord_rhyolith.h"

enum ScriptedTexts
{
    SAY_AGGRO   = 0,
    SAY_ARMOR   = 1,
    SAY_DEATH   = 2,
    SAY_KILL    = 3,
    SAY_LAVA    = 4,
    SAY_STOMP   = 5,
    SAY_TRANS   = 6,
};

enum Spells
{
    // Lord Rhyolith
    SPELL_OBSIDIAN_ARMOR                    = 98632,
    SPELL_CONCLUSIVE_STOMP                  = 97282,
    SPELL_VOLCANIC_BIRTH                    = 98010,
    SPELL_FLAME_OF_LIFE                     = 98045,
    SPELL_HEATED_VOLCANO                    = 98493,
    SPELL_LAVA_LINE                         = 100650,
    SPELL_DRINK_MAGMA                       = 98034,
    SPELL_SUPERHEATED                       = 101304,
    SPELL_MOLTEN_SPEW                       = 98043,
    SPELL_SUMMON_FRAGMENT_OF_RHYOLITH       = 98135,
    SPELL_SUMMON_FRAGMENT_OF_RHYOLITH_SUM   = 98136,
    SPELL_SUMMON_FRAGMENT_OF_RHYOLITH_SUM_H = 100392,
    SPELL_SUMMON_SPARK_OF_RHYOLITH          = 98553,
    SPELL_SUMMON_SPARK_OF_RHYOLITH_SUM      = 98552,
    SPELL_IMMOLATION                        = 99846,
    SPELL_BURNING_FEET                      = 98837,
    SPELL_RIDE_VEHICLE                      = 98843,
    SPELL_BALANCE_BAR                       = 98226,
    SPELL_MOLTEN_ARMOR                      = 98255,

    // Volcano
    SPELL_EXPLODE                           = 97719, // 46419 ?
    SPELL_VOLCANO_SMOKE                     = 97699,
    SPELL_ERUPTION                          = 98264,
    SPELL_LAVA_STRIKE                       = 98491,
    SPELL_ERUPTION_DMG                      = 98492,

    // Crater
    SPELL_MAGMA_FLOW                        = 97225, // aura
    SPELL_MAGMA_FLOW_AREA                   = 97230,
    SPELL_MAGMA_FLOW_DMG                    = 97234,
    SPELL_MAGMA                             = 98472,
    SPELL_LAVA_TUBE                         = 98265,

    // Spark of Rhyolith
    SPELL_IMMOLATION_SPARK                  = 98597,
    SPELL_INFERNAL_RAGE                     = 98596,

    // Fragment of Rhyolith
    SPELL_MELTDOWN                          = 98646,

    // Liquid Obsidian
    SPELL_FUSE                              = 99875,
};

enum Adds
{
    NPC_SPARK_OF_RHYOLITH       = 53211,
    NPC_FRAGMENT_OF_RHYOLITH    = 52620,
    NPC_CRATER                  = 52866,
    NPC_LIQUID_OBSIDIAN         = 52619,
    NPC_VOLCANO                 = 52582,
    NPC_LEFT_FOOT               = 52577, 
    NPC_RIGHT_FOOT              = 53087, 
    NPC_RHYOLITH_2              = 53772,
    NPC_MOVEMENT_CONTROLLER     = 52659,
};

enum Events
{
    EVENT_CHECK_MOVE        = 1,
    EVENT_CONCLUSIVE_STOMP  = 2,
    EVENT_ACTIVATE_VOLCANO  = 3,
    EVENT_CHECK_RHYOLITH    = 4, // check if the boss is near for adds
    EVENT_MAGMA_FLOW        = 5, // tube
    EVENT_MAGMA_FLOW_1      = 6, // spawn lines
    EVENT_MAGMA_FLOW_2      = 7, // despawn lines
    EVENT_START_MOVE        = 8, // start moving for adds
    EVENT_INFERNAL_RAGE     = 9,
    EVENT_SUPERHEATED       = 10,
    EVENT_FRAGMENT          = 11,
    EVENT_SPARK             = 12,
    EVENT_ERUPTION          = 13,
};

enum Others
{
    DATA_PHASE                      = 1,
    DATA_HITS                       = 2,
    ACTION_ADD_MOLTEN_ARMOR         = 3,
    ACTION_REMOVE_MOLTEN_ARMOR      = 4,
    ACTION_ADD_OBSIDIAN_ARMOR       = 5,
    ACTION_REMOVE_OBSIDIAN_ARMOR    = 6,
};

class boss_lord_rhyolith : public CreatureScript
{
    public:
        boss_lord_rhyolith() : CreatureScript("boss_lord_rhyolith") { }

        CreatureAI* GetAI(Creature* pCreature) const
        {
            return new boss_lord_rhyolithAI(pCreature);
        }

        struct boss_lord_rhyolithAI : public BossAI
        {
            boss_lord_rhyolithAI(Creature* pCreature) : BossAI(pCreature, DATA_RHYOLITH)
            {
                me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
                me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_GRIP, true);
                me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_STUN, true);
                me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
                me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_ROOT, true);
                me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FREEZE, true);
                me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
                me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
                me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_SAPPED, true);
                me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_CHARM, true);
                me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_DISORIENTED, true);
                me->ApplySpellImmune(0, IMMUNITY_STATE, SPELL_AURA_MOD_CONFUSE, true);
                me->setActive(true);
                me->SetSpeed(MOVE_RUN, 0.4f, true);
                me->SetSpeed(MOVE_WALK, 0.4f, true);
                pController = NULL;
                pRightFoot = NULL;
                pLeftFoot = NULL;
                curMove = 0;
                phase = 0;
                bAchieve = true;
                players_count = 0;
            }

            void InitializeAI()
            {
                if (!instance || static_cast<InstanceMap*>(me->GetMap())->GetScriptId() != sObjectMgr->GetScriptId(FLScriptName))
                    me->IsAIEnabled = false;
                else if (!me->isDead())
                    Reset();
            }

            bool AllowAchieve()
            {
                return bAchieve;
            }

            uint32 GetData(uint32 type)
            {
                if (type == DATA_PHASE)
                    return phase;
                return 0;
            }

            void Reset()
            {
                _Reset();
                
                me->UpdateEntry(NPC_RHYOLITH);
                me->SetHealth(me->GetMaxHealth());
                me->SetReactState(REACT_PASSIVE);

                curMove = 0;
                bAchieve = true;
                phase = 0;
                players_count = 0;

                instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_BALANCE_BAR);
                instance->SetData(DATA_RHYOLITH_HEALTH_SHARED, me->GetMaxHealth());

                //int32 bp0 = 2;
                //int32 bp1 = 1;
                //if (pRightFoot = me->SummonCreature(NPC_RIGHT_FOOT, me->GetPositionX(), me->GetPositionY(), me->GetPositionZ()))
                //    pRightFoot->CastCustomSpell(me, SPELL_RIDE_VEHICLE, &bp0, NULL, NULL, true);
                //if (pLeftFoot = me->SummonCreature(NPC_LEFT_FOOT, me->GetPositionX(), me->GetPositionY(), me->GetPositionZ()))
                //    pLeftFoot->CastCustomSpell(me, SPELL_RIDE_VEHICLE, &bp1, NULL, NULL, true);
            }

            void EnterEvadeMode()
            {
                instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_BALANCE_BAR);
                BossAI::EnterEvadeMode();
            }

            void DamageTaken(Unit* /*who*/, uint32 &damage)
            {
                if (phase == 0)
                    damage = 0;
            }

            void EnterCombat(Unit* attacker)
            {
                Talk(SAY_AGGRO);
                
                curMove = 0;
                phase = 0;
                bAchieve = true;
                players_count = instance->instance->GetPlayers().getSize();

                pController = me->SummonCreature(NPC_MOVEMENT_CONTROLLER, movePos[curMove]);
                pRightFoot = me->FindNearestCreature(NPC_RIGHT_FOOT, 100.0f);
                pLeftFoot = me->FindNearestCreature(NPC_LEFT_FOOT, 100.0f);

                if (pController)
                {
                    me->SetSpeed(MOVE_RUN, 0.4f, true);
                    me->SetSpeed(MOVE_WALK, 0.4f, true);
                    me->SetWalk(true);
                    me->GetMotionMaster()->MoveFollow(pController, 0.0f, 0.0f);
                }
                instance->SetData(DATA_RHYOLITH_HEALTH_SHARED, me->GetMaxHealth());

                events.ScheduleEvent(EVENT_CHECK_MOVE, 1000);
                events.ScheduleEvent(EVENT_CONCLUSIVE_STOMP, 10000);
                events.ScheduleEvent(EVENT_ACTIVATE_VOLCANO, urand(25000, 30000));
                events.ScheduleEvent(EVENT_FRAGMENT, urand(25000, 30000));
                events.ScheduleEvent(EVENT_SUPERHEATED, (IsHeroic() ? 5 * MINUTE * IN_MILLISECONDS : 6 * MINUTE * IN_MILLISECONDS));

                DoCastAOE(SPELL_BALANCE_BAR, true);

                DoZoneInCombat();
                instance->SetBossState(DATA_RHYOLITH, IN_PROGRESS);
            }

            void JustDied(Unit* /*killer*/)
            {
                _JustDied();
                instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_BALANCE_BAR);
                Talk(SAY_DEATH);
            }
            
            void JustReachedHome()
            {
                me->GetVehicleKit()->InstallAllAccessories(false);
                instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_BALANCE_BAR);
            }

            void DoAction(const int32 action)
            {
                switch(action)
                {
                    case ACTION_ADD_MOLTEN_ARMOR:
                        me->CastSpell(me, SPELL_MOLTEN_ARMOR, true);
                        if (pLeftFoot)
                            pLeftFoot->CastSpell(pLeftFoot, SPELL_MOLTEN_ARMOR, true);
                        if (pRightFoot)
                            pRightFoot->CastSpell(pRightFoot, SPELL_MOLTEN_ARMOR, true);
                        break;
                    case ACTION_REMOVE_MOLTEN_ARMOR:
                        me->RemoveAuraFromStack(SPELL_MOLTEN_ARMOR);
                        if (pLeftFoot)
                            pLeftFoot->RemoveAuraFromStack(SPELL_MOLTEN_ARMOR);
                        if (pRightFoot)
                            pRightFoot->RemoveAuraFromStack(SPELL_MOLTEN_ARMOR);
                        break;
                    case ACTION_ADD_OBSIDIAN_ARMOR:
                        if (pLeftFoot)
                            pLeftFoot->CastSpell(pLeftFoot, SPELL_OBSIDIAN_ARMOR, true);
                        if (pRightFoot)
                            pRightFoot->CastSpell(pRightFoot, SPELL_OBSIDIAN_ARMOR, true);
                        break;
                    case ACTION_REMOVE_OBSIDIAN_ARMOR:
                        if (pLeftFoot)
                            pLeftFoot->RemoveAuraFromStack(SPELL_OBSIDIAN_ARMOR, 0, AURA_REMOVE_BY_DEFAULT, 10);
                        if (pRightFoot)
                            pRightFoot->RemoveAuraFromStack(SPELL_OBSIDIAN_ARMOR, 0, AURA_REMOVE_BY_DEFAULT, 10);
                        break;
                }
            }

            void KilledUnit(Unit* who)
            {
                if (who->GetTypeId() == TYPEID_PLAYER)
                    Talk(SAY_KILL);
            }

            void UpdateAI(const uint32 diff)
            {
                if (!UpdateVictim())
                    return;

                if ((instance->GetData(DATA_RHYOLITH_HEALTH_SHARED) != 0) && (phase == 0))
                    me->SetHealth(instance->GetData(DATA_RHYOLITH_HEALTH_SHARED));

                if (me->HealthBelowPct(25) && phase == 0)
                {
                    phase = 1;
                    me->StopMoving();

                    me->UpdateEntry(NPC_RHYOLITH_2);

                    summons.DespawnEntry(NPC_VOLCANO);
                    summons.DespawnEntry(NPC_LIQUID_OBSIDIAN);

                    Talk(SAY_TRANS);
                    instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_ERUPTION_DMG);
                    instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_BALANCE_BAR);
                    events.CancelEvent(EVENT_FRAGMENT);
                    events.CancelEvent(EVENT_SPARK);
                    events.CancelEvent(EVENT_CHECK_MOVE);
                    events.CancelEvent(EVENT_ACTIVATE_VOLCANO);
                    events.CancelEvent(EVENT_SUPERHEATED);

                    if (pController)
                    {
                        pController->DespawnOrUnsummon();
                        pController = NULL;
                    }
                    if (pLeftFoot)
                    {
                        pLeftFoot->DespawnOrUnsummon();
                        pLeftFoot = NULL;
                    }
                    if (pRightFoot)
                    {
                        pRightFoot->DespawnOrUnsummon();
                        pRightFoot = NULL;
                    }

                    me->SetSpeed(MOVE_RUN, 1.0f, true);
                    me->SetSpeed(MOVE_WALK, 1.0f, true);
                    me->SetWalk(false);
                    
                    me->SetHealth(instance->GetData(DATA_RHYOLITH_HEALTH_SHARED));
                    DoCast(me, SPELL_IMMOLATION, true);
                    
                    events.ScheduleEvent(EVENT_START_MOVE, 2000);
                    return;
                }

                events.Update(diff);

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                while (uint32 eventId = events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_CHECK_MOVE:
                        {
                            if (pController && phase == 0)
                            {
                                if (me->GetDistance2d(pController) <= 0.5f)
                                {
                                    me->StopMoving();
                                    me->CastSpell(me, SPELL_DRINK_MAGMA);
                                    events.Reset();
                                    events.ScheduleEvent(EVENT_CHECK_MOVE, 8000);
                                    return;
                                }
                            }
                            if (pController && pLeftFoot && pRightFoot)
                            {
                                int32 l_dmg = pLeftFoot->AI()->GetData(DATA_HITS);
                                int32 r_dmg = pRightFoot->AI()->GetData(DATA_HITS);

                                if (!l_dmg && !r_dmg)
                                {
                                    me->RemoveAura(SPELL_BURNING_FEET);
                                    if (pLeftFoot)
                                        pLeftFoot->RemoveAura(SPELL_BURNING_FEET);
                                    if (pRightFoot)
                                        pRightFoot->RemoveAura(SPELL_BURNING_FEET);
                                    instance->DoSetAlternatePowerOnPlayers(25);
                                    events.ScheduleEvent(EVENT_CHECK_MOVE, 1000);
                                    return;
                                }

                                int32 i = NormalizeMove(curMove, CalculateNextMove(curMove, l_dmg, r_dmg));
                                if (i < 0)
                                    bAchieve = false;

                                if (i != curMove)
                                {
                                    me->RemoveAura(SPELL_BURNING_FEET);
                                    if (pLeftFoot)
                                        pLeftFoot->RemoveAura(SPELL_BURNING_FEET);
                                    if (pRightFoot)
                                        pRightFoot->RemoveAura(SPELL_BURNING_FEET);
                                    curMove = i;
                                    pController->NearTeleportTo(movePos[curMove].GetPositionX(), movePos[curMove].GetPositionY(), movePos[curMove].GetPositionZ(), 0.0f);
                                }
                                else
                                {
                                    DoCast(me, SPELL_BURNING_FEET, true);
                                    if (pLeftFoot)
                                        pLeftFoot->CastSpell(pLeftFoot, SPELL_BURNING_FEET, true);
                                    if (pRightFoot)
                                        pRightFoot->CastSpell(pRightFoot, SPELL_BURNING_FEET, true);
                                }
                            }
                            events.ScheduleEvent(EVENT_CHECK_MOVE, 1000);
                            break;
                        }
                        case EVENT_CONCLUSIVE_STOMP:
                            Talk(SAY_STOMP);
                            DoCastAOE(SPELL_CONCLUSIVE_STOMP);
                            events.ScheduleEvent(EVENT_CONCLUSIVE_STOMP, urand(35000, 40000));
                            break;
                        case EVENT_ACTIVATE_VOLCANO:
                        {
                            std::list<Creature*> volcanos;
                            Creature* pTarget = NULL;
                            me->GetCreatureListWithEntryInGrid(volcanos, NPC_VOLCANO, 300.0f);
                            
                            if (!volcanos.empty())
                                volcanos.remove_if(AuraCheck(SPELL_ERUPTION));

                            if (!volcanos.empty())
                            {
                                std::list<Creature*> volcanos_1;
                                for (std::list<Creature*>::const_iterator itr = volcanos.begin(); itr != volcanos.end(); ++itr)
                                {
                                    if (me->HasInArc(M_PI / 2, (*itr)))
                                        volcanos_1.push_back((*itr));
                                }
                                pTarget = Trinity::Containers::SelectRandomContainerElement((volcanos_1.empty() ? volcanos : volcanos_1));
                            }
                            if (pTarget)
                            {
                                Talk(SAY_LAVA);
                                DoCast(pTarget, SPELL_HEATED_VOLCANO, true);
                            }
                            events.ScheduleEvent(EVENT_ACTIVATE_VOLCANO, urand(45000, 50000));
                            break;
                        }
                        case EVENT_SPARK:
                        {
                            uint32 i = urand(0, _MAX_VOLCANO - 1);
                            me->CastSpell(volcanoPos[i].GetPositionX(), volcanoPos[i].GetPositionY(), volcanoPos[i].GetPositionZ(), SPELL_SUMMON_SPARK_OF_RHYOLITH, true);
                            events.ScheduleEvent(EVENT_FRAGMENT, 30000);
                            break;
                        }
                        case EVENT_FRAGMENT:
                        {
                            std::set<uint8> posList;
                            uint8 max_size = 5;

                            while (posList.size() < max_size)
                            {
                                uint8 i = urand(0, _MAX_VOLCANO - 1);
                                
                                if (posList.find(i) == posList.end())
                                    posList.insert(i);
                            }

                            for (std::set<uint8>::const_iterator itr = posList.begin(); itr != posList.end(); ++itr)
                                me->CastSpell(volcanoPos[(*itr)].GetPositionX(), volcanoPos[(*itr)].GetPositionY(), volcanoPos[(*itr)].GetPositionZ(), SPELL_SUMMON_FRAGMENT_OF_RHYOLITH, true);
                            
                            events.ScheduleEvent(EVENT_SPARK, 30000);
                            break;
                        }
                        case EVENT_SUPERHEATED:
                            DoCast(me, SPELL_SUPERHEATED, true);
                            events.ScheduleEvent(EVENT_SUPERHEATED, 10000);
                            break;
                        case EVENT_START_MOVE:
                            me->SetReactState(REACT_AGGRESSIVE);
                            me->GetMotionMaster()->MoveChase(me->getVictim());
                            break;
                        default:
                            break;
                    }
                }

                DoMeleeAttackIfReady();
            }
        private:
            Creature* pController;
            Creature* pRightFoot;
            Creature* pLeftFoot;
            int32 curMove;
            uint8 phase;
            bool bAchieve;
            uint8 players_count;

            int32 CalculateNextMove(int32 cur, int32 left, int32 right)
            {
                int32 diff = right - left;
                
                if (diff == 0)
                    return cur;
                else if (diff > 0 && diff < 3)
                    return cur;
                else if (diff < 0 && diff > -3)
                    return cur;

                diff /= 3;
                if (diff > 6)
                    diff = 6;
                else if (diff < -6)
                    diff = -6;

                float step = 4.1666666f;
                int32 energy = int32((diff + 6) * step);
                if (energy < 0)
                    energy = 0;
                if (energy > 50)
                    energy = 50;

                instance->DoSetAlternatePowerOnPlayers(energy);

                return int32(diff); 
            }

            int32 NormalizeMove(int32 cur, int32 diff)
            {
                int32 new_move = cur + diff;
                if (new_move >= 0 && new_move < 192)
                    return new_move;
                else if (new_move < 0)
                    return (191 + new_move);
                else if (new_move >= 192)
                    return (new_move - 191);
                return cur;
            }

            class AuraCheck
            {
                public:
                    AuraCheck(uint32 spellId) : _spellId(spellId) {}
        
                    bool operator()(Creature* volcano)
                    {
                       return (volcano->HasAura(_spellId));
                    }
                private:
                    uint32 _spellId;
            };
        };
};

class npc_lord_rhyolith_right_foot : public CreatureScript
{
    public:
        npc_lord_rhyolith_right_foot() : CreatureScript("npc_lord_rhyolith_right_foot") { }

        CreatureAI* GetAI(Creature* pCreature) const
        {
            return new npc_lord_rhyolith_right_footAI(pCreature);
        }

        struct npc_lord_rhyolith_right_footAI : public Scripted_NoMovementAI
        {
            npc_lord_rhyolith_right_footAI(Creature* pCreature) : Scripted_NoMovementAI(pCreature)
            {
                pInstance = me->GetInstanceScript();
                me->SetReactState(REACT_PASSIVE);
                me->CastCustomSpell(SPELL_OBSIDIAN_ARMOR, SPELLVALUE_AURA_STACK, 80, me, true);
                memset(m_hits, 0, sizeof(m_hits));
                hitsTimer = 1000;
            }

            void Reset()
            {
                memset(m_hits, 0, sizeof(m_hits));
                hitsTimer = 1000;
            }

            void EnterCombat(Unit* who)
            {
                if (Creature* pRhyolith = me->FindNearestCreature(NPC_RHYOLITH, 300.0f))
                    DoZoneInCombat(pRhyolith);
                if (Creature* pLeftFoot = me->FindNearestCreature(NPC_LEFT_FOOT, 300.0f))
                    DoZoneInCombat(pLeftFoot);
            }

            uint32 GetData(uint32 type)
            {
                if (type == DATA_HITS)
                    return GetHits();
                return 0;
            }

            void DamageTaken(Unit* who, uint32 &damage)
            {
                if (!me || !me->isAlive())
                    return;

                if (who->GetGUID() == me->GetGUID())
                    return;

                m_hits[0]++;

                if (pInstance)
                    pInstance->SetData(DATA_RHYOLITH_HEALTH_SHARED, me->GetHealth() > damage ? me->GetHealth() - damage : 0);
            }

            void UpdateAI(const uint32 diff)
            {
                if (!UpdateVictim())
                    return;

                hitsTimer  -= diff;

                if (hitsTimer <= 0)
                {
                     ResetHits();
                     hitsTimer = 1000;
                }

                if (pInstance)
                    if (pInstance->GetData(DATA_RHYOLITH_HEALTH_SHARED) != 0)
                        me->SetHealth(pInstance->GetData(DATA_RHYOLITH_HEALTH_SHARED));
            }
        private:
            InstanceScript* pInstance;
            uint32 m_hits[3];
            int32 hitsTimer;

            void ResetHits()
            {
                for (uint8 i = 3 - 1;  i > 0; i--)
                    m_hits[i] = m_hits[i - 1];

                m_hits[0] = 0;
            }

            uint32 GetHits()
            {
                uint32 value = 0;

                for (uint8 i = 0; i < 3; ++i)
                     value += m_hits[i];

                if (value < 0)
                    return 0;

                return value;
            }
        };
};

class npc_lord_rhyolith_left_foot : public CreatureScript
{
    public:
        npc_lord_rhyolith_left_foot() : CreatureScript("npc_lord_rhyolith_left_foot") { }

        CreatureAI* GetAI(Creature* pCreature) const
        {
            return new npc_lord_rhyolith_left_footAI(pCreature);
        }

        struct npc_lord_rhyolith_left_footAI : public Scripted_NoMovementAI
        {
            npc_lord_rhyolith_left_footAI(Creature* pCreature) : Scripted_NoMovementAI(pCreature)
            {
                pInstance = me->GetInstanceScript();
                me->SetReactState(REACT_PASSIVE);
                me->CastCustomSpell(SPELL_OBSIDIAN_ARMOR, SPELLVALUE_AURA_STACK, 80, me, true);
                memset(m_hits, 0, sizeof(m_hits));
                hitsTimer = 1000;
            }

            void Reset()
            {
                memset(m_hits, 0, sizeof(m_hits));
                hitsTimer = 1000;
            }

            void EnterCombat(Unit* who)
            {
                if (Creature* pRhyolith = me->FindNearestCreature(NPC_RHYOLITH, 300.0f))
                    DoZoneInCombat(pRhyolith);
                if (Creature* pRightFoot = me->FindNearestCreature(NPC_RIGHT_FOOT, 300.0f))
                    DoZoneInCombat(pRightFoot);
            }

            uint32 GetData(uint32 type)
            {
                if (type == DATA_HITS)
                    return GetHits();
                return 0;
            }

            void DamageTaken(Unit* who, uint32 &damage)
            {
                if (!me || !me->isAlive())
                    return;

                if (who->GetGUID() == me->GetGUID())
                    return;

                m_hits[0]++;

                if (pInstance)
                    pInstance->SetData(DATA_RHYOLITH_HEALTH_SHARED, me->GetHealth() > damage ? me->GetHealth() - damage : 0);
            }

            void UpdateAI(const uint32 diff)
            {
                if (!UpdateVictim())
                    return;

                hitsTimer  -= diff;

                if (hitsTimer <= 0)
                {
                     ResetHits();
                     hitsTimer = 1000;
                }

                if (pInstance)
                    if (pInstance->GetData(DATA_RHYOLITH_HEALTH_SHARED) != 0)
                        me->SetHealth(pInstance->GetData(DATA_RHYOLITH_HEALTH_SHARED));
            }
        private:
            InstanceScript* pInstance;
            uint32 m_hits[3];
            int32 hitsTimer;

            void ResetHits()
            {
                for (uint8 i = 3 - 1;  i > 0; i--)
                    m_hits[i] = m_hits[i - 1];

                m_hits[0] = 0;
            }

            uint32 GetHits()
            {
                uint32 value = 0;

                for (uint8 i = 0; i < 3; ++i)
                     value += m_hits[i];

                if (value < 0)
                    return 0;

                return value;
            }
        };
};

class npc_lord_rhyolith_volcano : public CreatureScript
{
    public:
        npc_lord_rhyolith_volcano() : CreatureScript("npc_lord_rhyolith_volcano") { }

        CreatureAI* GetAI(Creature* pCreature) const
        {
            return new npc_lord_rhyolith_volcanoAI(pCreature);
        }

        struct npc_lord_rhyolith_volcanoAI : public Scripted_NoMovementAI
        {
            npc_lord_rhyolith_volcanoAI(Creature* pCreature) : Scripted_NoMovementAI(pCreature)
            {
                me->SetReactState(REACT_PASSIVE);
                me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE | UNIT_FLAG_NON_ATTACKABLE);
            }

            void Reset()
            {
                events.Reset();
            }

            void EnterCombat(Unit* /*who*/)
            {
                DoCast(me, SPELL_VOLCANO_SMOKE, true);
                events.ScheduleEvent(EVENT_CHECK_RHYOLITH, 3000);
            }

            void SpellHit(Unit* /*who*/, const SpellInfo* spellInfo)
            {
                if (spellInfo->Id == SPELL_HEATED_VOLCANO)
                {
                    me->RemoveAura(SPELL_VOLCANO_SMOKE);
                    events.ScheduleEvent(EVENT_ERUPTION, 3000);
                }
            }

            void UpdateAI(const uint32 diff)
            {
                if (!UpdateVictim())
                    return;

                events.Update(diff);

                while (uint32 eventId = events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_ERUPTION:
                            DoCast(me, SPELL_ERUPTION, true);
                            break;
                        case EVENT_CHECK_RHYOLITH:
                            if (Creature* pRhyolith = me->FindNearestCreature(NPC_RHYOLITH, 5.0f, true))
                            {
                                if (pRhyolith->IsInEvadeMode() || (pRhyolith->AI()->GetData(DATA_PHASE) != 0))
                                    return;

                                pRhyolith->AI()->DoAction(ACTION_ADD_MOLTEN_ARMOR);
                                pRhyolith->AI()->DoAction(ACTION_REMOVE_OBSIDIAN_ARMOR);
                                pRhyolith->SummonCreature(NPC_CRATER, me->GetPositionX(), me->GetPositionY(), me->GetPositionZ(), 0.0f, TEMPSUMMON_TIMED_DESPAWN, 60000);
                                if (IsHeroic())
                                {
                                    std::set<uint8> posList;
                                    uint8 max_size = 5;

                                    while (posList.size() < max_size)
                                    {
                                        uint8 i = urand(0, 191);
                                        if (pRhyolith->GetDistance2d(movePos[i].GetPositionX(), movePos[i].GetPositionY()) <= 10.0f)
                                            continue;

                                        if (posList.find(i) == posList.end())
                                            posList.insert(i);
                                    }

                                    for (std::set<uint8>::const_iterator itr = posList.begin(); itr != posList.end(); ++itr)
                                        pRhyolith->SummonCreature(NPC_LIQUID_OBSIDIAN, movePos[(*itr)]);
                                }
                                
                                me->DespawnOrUnsummon(500);
                            }
                            else
                                events.ScheduleEvent(EVENT_CHECK_RHYOLITH, 1000);
                            break;
                    }
                }
            }
        private:
            EventMap events;
        };
};

class npc_lord_rhyolith_crater : public CreatureScript
{
    public:
        npc_lord_rhyolith_crater() : CreatureScript("npc_lord_rhyolith_crater") { }

        CreatureAI* GetAI(Creature* pCreature) const
        {
            return new npc_lord_rhyolith_craterAI(pCreature);
        }

        struct npc_lord_rhyolith_craterAI : public Scripted_NoMovementAI
        {
            npc_lord_rhyolith_craterAI(Creature* pCreature) : Scripted_NoMovementAI(pCreature)
            {
                me->SetReactState(REACT_PASSIVE);
                me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE | UNIT_FLAG_NON_ATTACKABLE);
            }

            void Reset()
            {
                events.Reset();
            }

            void EnterCombat(Unit* /*who*/)
            {
                DoCast(me, SPELL_EXPLODE, true);
                DoCast(me, SPELL_MAGMA, true);
                events.ScheduleEvent(EVENT_MAGMA_FLOW, 20000);
            }

            void UpdateAI(const uint32 diff)
            {
                if (!UpdateVictim())
                    return;
                
                events.Update(diff);

                while (uint32 eventId = events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_MAGMA_FLOW:
                            DoCast(me, SPELL_LAVA_TUBE, true);
                            events.ScheduleEvent(EVENT_MAGMA_FLOW_1, 3000);
                            break;
                        case EVENT_MAGMA_FLOW_1:
                            DoCast(me, SPELL_MAGMA_FLOW, true);
                            events.ScheduleEvent(EVENT_MAGMA_FLOW_2, 15000);
                            break;
                        case EVENT_MAGMA_FLOW_2:
                            me->RemoveAura(SPELL_LAVA_TUBE);
                            me->RemoveAllDynObjects();
                            break;
                    }
                }
            }
        private:
            EventMap events;
        };
};

class npc_lord_rhyolith_liquid_obsidian : public CreatureScript
{
    public:
        npc_lord_rhyolith_liquid_obsidian() : CreatureScript("npc_lord_rhyolith_liquid_obsidian") { }

        CreatureAI* GetAI(Creature* pCreature) const
        {
            return new npc_lord_rhyolith_liquid_obsidianAI(pCreature);
        }

        struct npc_lord_rhyolith_liquid_obsidianAI : public ScriptedAI
        {
            npc_lord_rhyolith_liquid_obsidianAI(Creature* pCreature) : ScriptedAI(pCreature)
            {
                me->SetReactState(REACT_PASSIVE);
                me->SetSpeed(MOVE_RUN, 0.3f, true);
                me->SetSpeed(MOVE_WALK, 0.3f, true);
            }

            void Reset()
            {
                events.Reset();
            }

            void EnterCombat(Unit* /*who*/)
            {
                events.ScheduleEvent(EVENT_START_MOVE, 2000);
            }

            void UpdateAI(const uint32 diff)
            {
                if (!UpdateVictim())
                    return;
                
                events.Update(diff);

                while (uint32 eventId = events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_START_MOVE:
                            if (Creature* pRhyolith = me->FindNearestCreature(NPC_RHYOLITH, 300.0f))
                            {
                                me->GetMotionMaster()->MoveFollow(pRhyolith, 0.0f, 0.0f);
                                events.ScheduleEvent(EVENT_CHECK_RHYOLITH, 1000);
                            }
                            break;
                        case EVENT_CHECK_RHYOLITH:
                            if (Creature* pRhyolith = me->FindNearestCreature(NPC_RHYOLITH, 0.5f, true))
                                me->CastSpell(pRhyolith, SPELL_FUSE, true);
                            else
                                events.ScheduleEvent(EVENT_CHECK_RHYOLITH, 1000);
                            break;
                    }
                }
            }
        private:
            EventMap events;
        };
};

class npc_lord_rhyolith_spark_of_rhyolith : public CreatureScript
{
    public:
        npc_lord_rhyolith_spark_of_rhyolith() : CreatureScript("npc_lord_rhyolith_spark_of_rhyolith") { }

        CreatureAI* GetAI(Creature* pCreature) const
        {
            return new npc_lord_rhyolith_spark_of_rhyolithAI(pCreature);
        }

        struct npc_lord_rhyolith_spark_of_rhyolithAI : public ScriptedAI
        {
            npc_lord_rhyolith_spark_of_rhyolithAI(Creature* pCreature) : ScriptedAI(pCreature)
            {
                me->SetReactState(REACT_PASSIVE);
            }

            void Reset()
            {
                events.Reset();
            }

            void EnterCombat(Unit* /*who*/)
            {
                events.ScheduleEvent(EVENT_START_MOVE, 2000);
            }

            void UpdateAI(const uint32 diff)
            {
                if (!UpdateVictim())
                    return;
                
                events.Update(diff);

                while (uint32 eventId = events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_START_MOVE:
                            DoCast(me, SPELL_IMMOLATION_SPARK, true);
                            me->SetReactState(REACT_AGGRESSIVE);
                            me->GetMotionMaster()->MoveChase(me->getVictim());
                            events.ScheduleEvent(EVENT_INFERNAL_RAGE, 5000);
                            break;
                        case EVENT_INFERNAL_RAGE:
                            DoCast(me, SPELL_INFERNAL_RAGE, true);
                            events.ScheduleEvent(EVENT_INFERNAL_RAGE, 5000);
                            break;
                    }
                }
            }
        private:
            EventMap events;
        };
};

class npc_lord_rhyolith_fragment_of_rhyolith : public CreatureScript
{
    public:
        npc_lord_rhyolith_fragment_of_rhyolith() : CreatureScript("npc_lord_rhyolith_fragment_of_rhyolith") { }

        CreatureAI* GetAI(Creature* pCreature) const
        {
            return new npc_lord_rhyolith_fragment_of_rhyolithAI(pCreature);
        }

        struct npc_lord_rhyolith_fragment_of_rhyolithAI : public ScriptedAI
        {
            npc_lord_rhyolith_fragment_of_rhyolithAI(Creature* pCreature) : ScriptedAI(pCreature)
            {
                me->SetReactState(REACT_PASSIVE);
            }

            void Reset()
            {
                events.Reset();
            }

            void JustDied(Unit* /*who*/)
            {
                if (Creature* pRhyolith = me->FindNearestCreature(NPC_RHYOLITH, 5.0f))
                    pRhyolith->AI()->DoAction(ACTION_REMOVE_MOLTEN_ARMOR);
            }

            void EnterCombat(Unit* /*who*/)
            {
                events.ScheduleEvent(EVENT_START_MOVE, 2000);
            }

            void MovementInform(uint32 type, uint32 data)
            {
                if (type == POINT_MOTION_TYPE)
                    if (data == EVENT_CHARGE)
                        me->Kill(me);
            }

            void UpdateAI(const uint32 diff)
            {
                if (!UpdateVictim())
                    return;
                
                events.Update(diff);

                while (uint32 eventId = events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_START_MOVE:
                            DoCast(me, SPELL_MELTDOWN, true);
                            me->SetReactState(REACT_AGGRESSIVE);
                            me->GetMotionMaster()->MoveChase(me->getVictim());
                            break;
                    }
                }
            }
        private:
            EventMap events;
        };
};

class spell_lord_rhyolith_conclusive_stomp : public SpellScriptLoader
{
    public:
        spell_lord_rhyolith_conclusive_stomp() : SpellScriptLoader("spell_lord_rhyolith_conclusive_stomp") { }


        class spell_lord_rhyolith_conclusive_stomp_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_lord_rhyolith_conclusive_stomp_SpellScript);

            void HandleDummy(SpellEffIndex effIndex)
            {
                if (!GetCaster())
                    return;

                if (Creature* pCreature = GetCaster()->ToCreature())
                    if (pCreature->AI()->GetData(DATA_PHASE) != 1)
                    {
                        std::set<uint8> posList;
                        uint8 max_size = urand(2, 3);

                        while (posList.size() < max_size)
                        {
                            uint8 i = urand(0, 31);
                            if (GetCaster()->GetDistance2d(volcanoPos[i].GetPositionX(), volcanoPos[i].GetPositionY()) <= 5.0f)
                                continue;

                            if (posList.find(i) == posList.end())
                                posList.insert(i);
                        }

                        for (std::set<uint8>::const_iterator itr = posList.begin(); itr != posList.end(); ++itr)
                            GetCaster()->CastSpell(volcanoPos[(*itr)].GetPositionX(), volcanoPos[(*itr)].GetPositionY(), volcanoPos[(*itr)].GetPositionZ(), SPELL_VOLCANIC_BIRTH, true);
                    }
            }

            void Register()
            {
                OnEffectHitTarget += SpellEffectFn(spell_lord_rhyolith_conclusive_stomp_SpellScript::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_lord_rhyolith_conclusive_stomp_SpellScript();
        }
};

class spell_lord_rhyolith_magma_flow : public SpellScriptLoader
{
    public:
        spell_lord_rhyolith_magma_flow() : SpellScriptLoader("spell_lord_rhyolith_magma_flow") { }

        class spell_lord_rhyolith_magma_flow_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_lord_rhyolith_magma_flow_AuraScript);
            
            bool Load()
            {
                count = 0;
                add = true;
                return true;
            }

            void PeriodicTick(AuraEffect const* aurEff)
            {
                if (!GetCaster())
                    return;

                count++;

                for (float a = 0; a <= 2 * M_PI; a += M_PI / 2)
                {
                    Position pos;
                    GetCaster()->GetNearPosition(pos, 1.0f * count, a + frand(-0.05f, 0.05f));
                    GetCaster()->CastSpell(pos.GetPositionX(), pos.GetPositionY(), pos.GetPositionZ(), SPELL_MAGMA_FLOW_AREA, true);
                }
            }

            void Register()
            {
                OnEffectPeriodic += AuraEffectPeriodicFn(spell_lord_rhyolith_magma_flow_AuraScript::PeriodicTick, EFFECT_0, SPELL_AURA_PERIODIC_DUMMY);
            }

        private:
            uint8 count;
            bool add;
        };

        AuraScript* GetAuraScript() const
        {
            return new spell_lord_rhyolith_magma_flow_AuraScript();
        }
};

class spell_lord_rhyolith_fuse : public SpellScriptLoader
{
    public:
        spell_lord_rhyolith_fuse() : SpellScriptLoader("spell_lord_rhyolith_fuse") { }


        class spell_lord_rhyolith_fuse_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_lord_rhyolith_fuse_SpellScript);

            void HandleScript(SpellEffIndex effIndex)
            {
                if (!GetCaster() || !GetHitUnit())
                    return;

                if (Creature* pRhyolith = GetHitUnit()->ToCreature())
                    if (pRhyolith->AI()->GetData(DATA_PHASE) == 0)
                        pRhyolith->AI()->DoAction(ACTION_ADD_OBSIDIAN_ARMOR);
            }

            void Register()
            {
                OnEffectHitTarget += SpellEffectFn(spell_lord_rhyolith_fuse_SpellScript::HandleScript, EFFECT_0, SPELL_EFFECT_SCRIPT_EFFECT);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_lord_rhyolith_fuse_SpellScript();
        }
};

class spell_lord_rhyolith_drink_magma : public SpellScriptLoader
{
    public:
        spell_lord_rhyolith_drink_magma() : SpellScriptLoader("spell_lord_rhyolith_drink_magma") { }


        class spell_lord_rhyolith_drink_magma_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_lord_rhyolith_drink_magma_SpellScript);

            void HandleDummy()
            {
                if (!GetCaster())
                    return;

                GetCaster()->CastSpell(GetCaster(), SPELL_MOLTEN_SPEW, true);
            }

            void Register()
            {
                AfterCast += SpellCastFn(spell_lord_rhyolith_drink_magma_SpellScript::HandleDummy);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_lord_rhyolith_drink_magma_SpellScript();
        }
};

class spell_lord_rhyolith_lava_strike : public SpellScriptLoader
{
    public:
        spell_lord_rhyolith_lava_strike() : SpellScriptLoader("spell_lord_rhyolith_lava_strike") { }


        class spell_lord_rhyolith_lava_strike_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_lord_rhyolith_lava_strike_SpellScript);

            void FilterTargets(std::list<WorldObject*>& targets)
            {
                if (!GetCaster())
                    return;

                uint32 max_size = (GetCaster()->GetMap()->Is25ManRaid() ? 6 : 3);
                if (!targets.empty())
                    Trinity::Containers::RandomResizeList(targets, max_size);
            }

            void HandleDummy(SpellEffIndex effIndex)
            {
                if (!GetCaster() || !GetHitUnit())
                    return; 

                GetCaster()->CastSpell(GetHitUnit(), SPELL_LAVA_STRIKE, true);
            }

            void Register()
            {
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_lord_rhyolith_lava_strike_SpellScript::FilterTargets, EFFECT_0,TARGET_UNIT_DEST_AREA_ENEMY);
                OnEffectHitTarget += SpellEffectFn(spell_lord_rhyolith_lava_strike_SpellScript::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_lord_rhyolith_lava_strike_SpellScript();
        }
};

class achievement_not_an_ambi_turner : public AchievementCriteriaScript
{
    public:
        achievement_not_an_ambi_turner() : AchievementCriteriaScript("achievement_not_an_ambi_turner") { }

        bool OnCheck(Player* source, Unit* target)
        {
            if (!target)
                return false;

            if (boss_lord_rhyolith::boss_lord_rhyolithAI* lord_rhyolithAI = CAST_AI(boss_lord_rhyolith::boss_lord_rhyolithAI, target->GetAI()))
                return lord_rhyolithAI->AllowAchieve();

            return false;
        }
};

void AddSC_boss_lord_rhyolith()
{
    new boss_lord_rhyolith();
    new npc_lord_rhyolith_left_foot();
    new npc_lord_rhyolith_right_foot();
    new npc_lord_rhyolith_volcano();
    new npc_lord_rhyolith_crater();
    new npc_lord_rhyolith_spark_of_rhyolith();
    new npc_lord_rhyolith_fragment_of_rhyolith();
    new npc_lord_rhyolith_liquid_obsidian();
    new spell_lord_rhyolith_conclusive_stomp();
    new spell_lord_rhyolith_magma_flow();
    new spell_lord_rhyolith_fuse();
    new spell_lord_rhyolith_drink_magma();
    new spell_lord_rhyolith_lava_strike();
    new achievement_not_an_ambi_turner();
}