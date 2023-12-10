#pragma once 


#include "util.h"
#include "knockout.h"
using namespace RE; 
namespace KnockoutExtensions
{
class UnconsciousStateHook
{
    public: 

    static void Install()
    {
        auto& trampoline = SKSE::GetTrampoline(); 

        SKSE::AllocTrampoline(16);
        //	j	sub_1405E32A0+9	jmp     Actor__SetLifeState_1405EDEF0
        REL::Relocation<std::uintptr_t> target{ REL::RelocationID(36488, 0) , REL::Relocate(0x9, 0x9)};

        _SetLifeState = trampoline.write_call<5>(target.address(), SetLifeState);

        SKSE::log::info("Unconscious Hook Installed");
    }
    private: 

    static bool SetLifeState(Actor* a_actor, ACTOR_LIFE_STATE a_lifeState);
    
    static inline REL::Relocation<decltype(SetLifeState)> _SetLifeState;
    //	p	Actor__KillImpl_140603B30+6C4	call    Actor__SetLifeState_1405EDEF0
    //>(REL::RelocationID(37673, 38627))
};
class BleedoutStateHook
{
    public: 

    static void Install()
    {
            //Down	p	Actor__KillImpl_140603B30+6C4	call    Actor__SetLifeState_1405EDEF0
            //Up	p	sub_140296CA0+52C	call    sub_140614650
        auto& trampoline = SKSE::GetTrampoline(); 

        SKSE::AllocTrampoline(16);

        REL::Relocation<std::uintptr_t> target{ REL::RelocationID(36872, 19507), REL::Relocate(0x6C4, 0x52C)} ; 
        _SetLifeState = trampoline.write_call<5>(target.address(), SetLifeState);
    }
    private: 
    static bool SetLifeState(Actor* a_actor, ACTOR_LIFE_STATE a_lifeState); 
    static inline REL::Relocation<decltype(SetLifeState)> _SetLifeState;


};
class HitEventHook
{
    public: 
    static void Install()
    {
        auto& trampoline = SKSE::GetTrampoline(); 

        SKSE::AllocTrampoline(16);
        REL::Relocation<std::uintptr_t> target { REL::RelocationID(37673, 38627), REL::Relocate(0x3C0, 0x4A8) };

        _ProcessHitEvent = trampoline.write_call<5>(target.address(), ProcessHitEvent);

        SKSE::log::info("Hit Event Hook Installed");
    }
    private: 
    static void ProcessHitEvent(TESObjectREFR *a_target, HitData *a_hitData);
    static inline REL::Relocation<decltype(ProcessHitEvent)> _ProcessHitEvent;
};

class MainUpdateHook
{
    public: 

    static void Install()
    {

        REL::Relocation<std::uintptr_t> PlayerVtbl{RE::VTABLE_PlayerCharacter[0]};
        _Update = PlayerVtbl.write_vfunc(0xAD, Update);

        SKSE::log::info("Hook - Main Update Installed");
    }

     private:
        static void Update(RE::Actor *a_this, float a_delta);
        static inline REL::Relocation<decltype(Update)> _Update;
        
        static inline uint32_t framesElapsed = 0; 
};
}
