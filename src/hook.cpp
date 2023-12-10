#include "hook.h"
#pragma once

namespace KnockoutExtensions
{

    bool UnconsciousStateHook::SetLifeState(Actor *a_actor, ACTOR_LIFE_STATE a_lifeState)
    {
        if (!a_actor) { return false; }
        
        bool wasUnconscious = a_actor->AsActorState()->GetLifeState() == ACTOR_LIFE_STATE::kUnconcious;
        bool isUnconscious = a_lifeState == ACTOR_LIFE_STATE::kUnconcious;

        if (wasUnconscious && !isUnconscious) 
        {
            KnockoutHandler::RecoverUnconscious(a_actor); 
        }
        else 
        if (isUnconscious && !wasUnconscious)
        {
            KnockoutHandler::ApplyUnconscious(a_actor); 
        }


        return true;
    }


    void HitEventHook::ProcessHitEvent(TESObjectREFR *a_target, HitData *a_hitData)
    {
        if (!a_target || !a_hitData || a_target->GetFormType() != FormType::ActorCharacter || !a_hitData->flags.any(HitData::Flag::kBash)) { return _ProcessHitEvent(a_target, a_hitData); }
        
        auto* target = a_target->As<Actor>();
        if (target->IsInCombat()) { return _ProcessHitEvent(a_target, a_hitData); }

        auto* attacker = a_hitData->aggressor.get().get();
        if (!attacker) { return _ProcessHitEvent(a_target, a_hitData); }

        bool hasLOS;
        ActorUtil::Detection::HasLOSViewCone(target, attacker, &hasLOS);

        if (hasLOS) { return _ProcessHitEvent(a_target, a_hitData); }

        target->SetLifeState(ACTOR_LIFE_STATE::kUnconcious);
        KnockoutHandler::ApplyUnconscious(target);
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
        if (!a_actor || a_lifeState != ACTOR_LIFE_STATE::kEssentialDown)
        {
            return _SetLifeState(a_actor, a_lifeState);
        }

        KnockoutHandler::ApplyUnconscious(a_actor);

        return _SetLifeState(a_actor, ACTOR_LIFE_STATE::kUnconcious);
    }
}
