#pragma once 


#include "util.h"
#include "knockout.h"
using namespace RE; 
namespace KnockoutExtensions
{
class UnconsciousHook
{
    public: 

    static void InstallHook()
    {
        auto& trampoline = SKSE::GetTrampoline(); 

        SKSE::AllocTrampoline(16);
        REL::Relocation<std::uintptr_t> target{ REL::RelocationID(36872, 0), REL::Relocate(0x6C4,0) };

        _SetLifeState = trampoline.write_call<5>(target.address(), SetLifeState);

        SKSE::log::info("Unconscious Hook Installed");
    }
    private: 

    static void SetLifeState(Actor* a_actor, ACTOR_LIFE_STATE a_lifeState);

    static inline REL::Relocation<decltype(SetLifeState)> _SetLifeState;
    //	p	Actor__KillImpl_140603B30+6C4	call    Actor__SetLifeState_1405EDEF0
    //>(REL::RelocationID(37673, 38627))
    
    static void PushActorAway(AIProcess *a_process, Actor *a_target, NiPoint3 *a_origin, float a_magnitude)
    {
        using func_t = decltype(PushActorAway);
        REL::Relocation<func_t> func{REL::RelocationID(38858, 0)}; 
        return func(a_process, a_target, a_origin, a_magnitude);
    }
};

class HitEventHook
{
    public: 
    static void InstallHook()
    {
        auto& trampoline = SKSE::GetTrampoline(); 

        SKSE::AllocTrampoline(16);
        REL::Relocation<std::uintptr_t> target { REL::RelocationID(37673, 38627), REL::Relocate(0x3C0, 0x4A8, 0x3C0) };

        _ProcessHitEvent = trampoline.write_call<5>(target.address(), ProcessHitEvent);

        SKSE::log::info("Hit Event Hook Installed");
    }
    private: 
    static void ProcessHitEvent(TESObjectREFR *a_target, HitData *a_hitData);
    static inline REL::Relocation<decltype(ProcessHitEvent)> _ProcessHitEvent;
};
}
