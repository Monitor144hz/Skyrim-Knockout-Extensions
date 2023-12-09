#pragma once 
using namespace RE; 

namespace KnockoutExtensions
{
    class KnockoutHandler
    {
        public: 

            static void ExtendUnconscious(Actor* a_actor)
            {
                if (a_actor->IsInCombat()) { a_actor->StopCombat(); }
                NiPoint3 actorPos = a_actor->GetPosition();
                PushActorAway(a_actor->GetActorRuntimeData().currentProcess, a_actor, &actorPos, 0.00000001);
                auto* avOwner = a_actor->AsActorValueOwner(); 
                if (!avOwner) 
                { 
                    return;
                }
                avOwner->SetActorValue(ActorValue::kParalysis, 1.0f);
            }

            static void RecoverUnconscious(Actor* a_actor)
            {
                auto* avOwner = a_actor->AsActorValueOwner(); 
                if (!avOwner) 
                { 
                    return;
                }
                avOwner->SetActorValue(ActorValue::kParalysis, 0.0f);
            }
            



        private:
            static void PushActorAway(AIProcess *a_process, Actor *a_target, NiPoint3 *a_origin, float a_magnitude)
            {
                using func_t = decltype(PushActorAway);
                REL::Relocation<func_t> func{REL::RelocationID(38858, 0)};
                return func(a_process, a_target, a_origin, a_magnitude);
            }
    };
}