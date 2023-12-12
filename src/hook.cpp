#include "hook.h"
#pragma once

namespace KnockoutExtensions
{



    void HitEventHook::ProcessHitEvent(TESObjectREFR *a_target, HitData *a_hitData)
    {
        if (!a_target || !a_hitData || a_target->GetFormType() != FormType::ActorCharacter || !a_hitData->flags.any(HitData::Flag::kBash)) { return _ProcessHitEvent(a_target, a_hitData); }
        
        auto* target = a_target->As<Actor>();
        if (target->IsPlayerRef()) { return; }
        auto* targetState = target->AsActorState();
        bool inBleedout = targetState ? targetState->GetLifeState() == ACTOR_LIFE_STATE::kBleedout : false;

        if (target->IsInCombat() && !inBleedout) { return _ProcessHitEvent(a_target, a_hitData); }

        auto* attacker = a_hitData->aggressor.get().get();
        if (!attacker) { return _ProcessHitEvent(a_target, a_hitData); }
        
        bool arg2;
        bool hasLOS = target->HasLineOfSight(attacker,arg2);


        if ((hasLOS || !Settings::GetTriggerBackBash()) && (!inBleedout || !Settings::GetTriggerBleedoutBash())) { return _ProcessHitEvent(a_target, a_hitData); }

        target->SetLifeState(ACTOR_LIFE_STATE::kUnconcious);
        KnockoutHandler::ApplyUnconscious(target, attacker);
        KnockoutHandler::TrackActor(target);
        a_hitData->totalDamage = 0.0f;

        a_hitData->flags.set(HitData::Flag::kSneakAttack);


        _ProcessHitEvent(a_target, a_hitData);
    }


    void MainUpdateHook::Update(RE::Actor *a_this, float a_delta)
    {
        if (framesElapsed < 60)
        {
            framesElapsed += 1; 
            _Update(a_this, a_delta);
        }

        framesElapsed = 0;
        KnockoutHandler::UpdateTrackedActors(); 
        _Update(a_this, a_delta);
    }


    bool BleedoutStateHook::SetLifeState(Actor *a_actor, ACTOR_LIFE_STATE a_lifeState)
    {
        if (!a_actor || a_lifeState != ACTOR_LIFE_STATE::kEssentialDown || !Settings::GetTriggerEssentialDown() || a_actor->IsPlayerRef())
        {
            return _SetLifeState(a_actor, a_lifeState);
        }

        KnockoutHandler::ApplyUnconscious(a_actor);

        bool ret = _SetLifeState(a_actor, ACTOR_LIFE_STATE::kUnconcious);
        KnockoutHandler::TrackActor(a_actor);
        return ret;
    }


    bool UnconsciousFuncHook::DisableUnconscious(Actor *a_actor, bool a_enable)
    {
        bool ret = _DisableUnconscious(a_actor, a_enable);
        if (!a_actor) { return ret; }

        KnockoutHandler::RecoverUnconscious(a_actor); 
        KnockoutHandler::UntrackActor(a_actor);

        return ret;
    }
    bool UnconsciousFuncHook::EnableUnconscious(Actor *a_actor, bool a_enable)
    {

        bool ret = _EnableUnconscious(a_actor, a_enable);
        if (!a_actor) { return ret; }

        KnockoutHandler::ApplyUnconscious(a_actor);
        KnockoutHandler::TrackActor(a_actor);

        return ret;
    }
}
