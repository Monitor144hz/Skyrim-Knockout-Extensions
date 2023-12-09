#include "hook.h"
#pragma once

namespace KnockoutExtensions
{

    void UnconsciousHook::SetLifeState(Actor *a_actor, ACTOR_LIFE_STATE a_lifeState)
    {
        if (!a_actor || a_lifeState != ACTOR_LIFE_STATE::kEssentialDown) { return _SetLifeState(a_actor, a_lifeState); }
        KnockoutHandler::ExtendUnconscious(a_actor); 
        _SetLifeState(a_actor, ACTOR_LIFE_STATE::kUnconcious);
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
        KnockoutHandler::ExtendUnconscious(target);
    }
}
