#pragma once 
#include "util.h"

using namespace RE; 

namespace KnockoutExtensions
{
    class KnockoutHandler
    {
        public: 

            static void ApplyUnconscious(Actor* a_actor);

            static void RecoverUnconscious(Actor* a_actor);

            static void UpdateTrackedActors();
        private: 

            static inline std::unordered_map<Actor*, float> actorMap; 

            static void TrackActor(Actor* a_actor);

            static void PushActorAway(AIProcess *a_process, Actor *a_target, NiPoint3 *a_origin, float a_magnitude)
            {
                if (!a_process->InHighProcess())
                {
                    auto *actor = a_process->GetUserData();
                    if (!actor || !actor->MoveToHigh())
                    {
                        return;
                    }
                }
                using func_t = decltype(PushActorAway);
                REL::Relocation<func_t> func{REL::RelocationID(38858, 0)};
                func(a_process, a_target, a_origin, a_magnitude);
            }
    };
}