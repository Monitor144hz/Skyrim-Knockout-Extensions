#include "knockout.h"


namespace KnockoutExtensions
{
    void KnockoutHandler::ApplyUnconscious(Actor *a_actor)
    {
        if (actorMap.count(a_actor) > 0) { return; }
        if (a_actor->IsInCombat())
        {
            a_actor->StopCombat();
        }
        
        NiPoint3 actorPos = a_actor->GetPosition();
        // ActorUtil::Physics::PushActorAway(PlayerCharacter::GetSingleton()->GetActorRuntimeData().currentProcess, a_actor, &actorPos, 0.00000001f);
        TrackActor(a_actor);

        auto *avOwner = a_actor->AsActorValueOwner();
        if (!avOwner)
        {
            return;
        }
        avOwner->SetActorValue(ActorValue::kParalysis, 1.0f);
    }
    void KnockoutHandler::RecoverUnconscious(Actor *a_actor)
    {
        auto *avOwner = a_actor->AsActorValueOwner();
        if (!avOwner)
        {
            return;
        }
        avOwner->SetActorValue(ActorValue::kParalysis, 0.0f);

        if (a_actor->Is3DLoaded()) { a_actor->Update3DModel(); }
    }
    void KnockoutHandler::TrackActor(Actor *a_actor)
    {
        auto *calendar = Calendar::GetSingleton();
        if (!calendar)
        {
            RecoverUnconscious(a_actor);
            return;
        }
        float exactHoursPassed = calendar->GetHoursPassed() + calendar->GetHour();
        actorMap.emplace(a_actor, exactHoursPassed);
    }
    void KnockoutHandler::UpdateTrackedActors()
    {
        auto* calendar = Calendar::GetSingleton(); 
        if (!calendar) { return; }
        float exactHoursPassed = calendar->GetHoursPassed() + calendar->GetHour();
        for(auto& it : actorMap)
        {
            if (exactHoursPassed - it.second > 8.0f)
            {
                auto* actor = it.first; 
                actorMap.erase(actor);
                RecoverUnconscious(actor); 
                actor->SetLifeState(ACTOR_LIFE_STATE::kAlive);
            }
        }
    }
}