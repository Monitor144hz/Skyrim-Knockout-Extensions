#include "knockout.h"


namespace KnockoutExtensions
{
    void KnockoutHandler::ApplyUnconscious(Actor *a_actor)
    {
        ReadLocker locker(actorLock);
        if (actorIDMap.count(a_actor->GetFormID()) > 0) { return; }
        locker.unlock();

        if (a_actor->IsInCombat())
        {
            a_actor->StopCombat();
        }
        
        NiPoint3 actorPos = a_actor->GetPosition();
        
        auto& rtd = a_actor->GetActorRuntimeData();
        ActorUtil::Physics::PushActorAway(a_actor->GetActorRuntimeData().currentProcess, a_actor, &actorPos, 0.00000001f);

        auto *avOwner = a_actor->AsActorValueOwner();
        if (!avOwner)
        {
            return;
        }
        rtd.boolFlags.set(Actor::BOOL_FLAGS::kDoNotShowOnStealthMeter);

        if (!a_actor->IsEssential())
        {
            rtd.boolBits.set(Actor::BOOL_BITS::kDead);
        }

        avOwner->SetActorValue(ActorValue::kParalysis, 1.0f);
        a_actor->SetActivationBlocked(false);
    }
    void KnockoutHandler::ApplyUnconscious(Actor *a_actor, Actor *a_causer)
    {
        ReadLocker locker(actorLock);
        if (actorIDMap.count(a_actor->GetFormID()) > 0) { return; }
        locker.unlock();

        if (a_actor->IsInCombat())
        {
            a_actor->StopCombat();
        }
        uint64_t witnessCount; 
        if (ActorUtil::Detection::GetHighestDetectionValue(a_causer, &witnessCount) > 0 && Settings::GetKnockoutIsCrime())
        {
            ActorUtil::Detection::SendAssaultAlarm(a_actor, a_causer, false);
        }
        
        auto& rtd = a_actor->GetActorRuntimeData();


        NiPoint3 actorPos = a_causer->GetPosition();
        ActorUtil::Physics::PushActorAway(a_actor->GetActorRuntimeData().currentProcess, a_actor, &actorPos, 0.00000001f);

        auto *avOwner = a_actor->AsActorValueOwner();
        if (!avOwner)
        {
            return;
        }

        avOwner->SetActorValue(ActorValue::kParalysis, 1.0f);
        // a_actor->AllowBleedoutDialogue(false);
        rtd.boolFlags.set(Actor::BOOL_FLAGS::kDoNotShowOnStealthMeter);
        rtd.boolBits.set(Actor::BOOL_BITS::kMurderAlarm);
        if (a_actor->IsHostileToActor(a_causer) && !a_actor->IsEssential())
        {
            rtd.boolBits.set(Actor::BOOL_BITS::kDead);
        }
        

    }
    void KnockoutHandler::RecoverUnconscious(Actor *a_actor)
    {
        auto *avOwner = a_actor->AsActorValueOwner();
        if (!avOwner)
        {
            return;
        }
        avOwner->SetActorValue(ActorValue::kParalysis, 0.0f);
        auto& rtd = a_actor->GetActorRuntimeData();
        rtd.boolFlags.reset(Actor::BOOL_FLAGS::kDoNotShowOnStealthMeter);
        rtd.boolBits.reset(Actor::BOOL_BITS::kMurderAlarm);
        rtd.boolBits.reset(Actor::BOOL_BITS::kDead);

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
        float exactHoursPassed = calendar->GetHoursPassed();
        
        WriteLocker locker(actorLock); 
        actorIDMap.emplace(a_actor->GetFormID(), exactHoursPassed);
    }



    void KnockoutHandler::UpdateTrackedActors()
    {
        WriteLocker locker(actorLock);

        auto* calendar = Calendar::GetSingleton(); 
        if (!calendar) { return; }
        float exactHoursPassed = calendar->GetHoursPassed();
        float duration = Settings::GetUnconsciousDuration();
        std::vector<FormID> markedFormIDs;
        for(auto& it : actorIDMap)
        {
            auto formID = it.first; 
            if (exactHoursPassed - it.second > duration)
            {
                markedFormIDs.emplace_back(formID);

                auto* actor = TESForm::LookupByID(formID)->As<Actor>();
                
                RecoverUnconscious(actor); 
                actor->SetLifeState(ACTOR_LIFE_STATE::kAlive);
            }
        }
        for (auto formID : markedFormIDs)
        {
            actorIDMap.erase(formID);
        }

    }

    void KnockoutHandler::UntrackActor(Actor* a_actor)
    {
        WriteLocker locker(actorLock);
        actorIDMap.erase(a_actor->GetFormID());
    }

        bool KnockoutHandler::GameSave(SKSE::SerializationInterface *serde)
    {
        assert(serde); 

        ReadLocker locker(actorLock);

        const std::size_t numActors = actorIDMap.size();

        if (!serde->WriteRecordData(numActors))
        {
            SKSE::log::error("Failed to save {} unconscious actors!", numActors); 
            return false; 
        }
        uint16_t actorCount = 0;
        for (auto& it : actorIDMap)
        {
            auto formID = it.first; 
            float time = it.second;
            if (!serde->WriteRecordData(formID))
            {
                SKSE::log::error("Failed to save actor FormID {}", formID); 
                return false; 
            }
            if (!serde->WriteRecordData(time))
            {
                SKSE::log::error("Failed to save timestamp {}", time); 
                return false; 
            }
            actorCount++; 
        }
        SKSE::log::info("{} unconscious actors saved", actorCount);
        return true; 
    }
    bool KnockoutHandler::GameLoad(SKSE::SerializationInterface *serde)
    {
        assert(serde);

        std::size_t size; 
        serde->ReadRecordData(size);
        WriteLocker locker(actorLock);

        actorIDMap.clear(); 

        RE::FormID formID; 
        float hoursPassed; 
        uint16_t actorCount = 0;
        for (std::size_t i = 0; i < size; i++) {
            serde->ReadRecordData(formID); 
            if (!serde->ResolveFormID(formID, formID))
            {
                SKSE::log::error("Failed to resolve formID {}", formID);
                continue; 
            }
            serde->ReadRecordData(hoursPassed); 
            actorIDMap.emplace(formID, hoursPassed);
            actorCount++;
        }
        SKSE::log::info("{} unconscious actors loaded", actorCount);

        return true; 
    }
    void KnockoutHandler::GameRevert(SKSE::SerializationInterface *serde)
    {
        WriteLocker locker(actorLock); 

        actorIDMap.clear();
    }

    bool KnockoutHandler::RecordSave(SKSE::SerializationInterface *serde, uint32_t a_type, uint32_t a_version)
    {
        if (!serde->OpenRecord(a_type, a_version))
        {
            SKSE::log::error("Failed to open cosave record!");
            return false; 
        }
        return GameSave(serde);
    }

    void KnockoutHandler::GameSaveCallback(SKSE::SerializationInterface *serde)
    {
        if (!RecordSave(serde, SerializeActor, SerializeVersion))
        {
            SKSE::log::critical("Failed to save unconscious actors!"); 
        }
        
        SKSE::log::info("Finished game cosave."); 
    }
    void KnockoutHandler::GameLoadCallback(SKSE::SerializationInterface *serde)
    {
        uint32_t type; 
        uint32_t version; 
        uint32_t length; 

        while (serde->GetNextRecordInfo(type, version, length))
        {
            if (version != SerializeVersion)
            {
                SKSE::log::critical("Loaded data is out of date! Read ({}), expected ({}) for type code ({})", version, SerializeCode, type);
            }
            if (type == SerializeActor)
            {
                if (!GameLoad(serde)) 
                {
                    SKSE::log::info("Failed to load unconscious actors!");
                }
            }
        }

    }
    void KnockoutHandler::GameRevertCallback(SKSE::SerializationInterface *serde)
    {
        GameRevert(serde);
    }
}