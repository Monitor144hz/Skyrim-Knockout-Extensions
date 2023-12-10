#include "log.h"
#include "hook.h"

void OnDataLoaded()
{
   
}

void MessageHandler(SKSE::MessagingInterface::Message* a_msg)
{
	switch (a_msg->type) {
	case SKSE::MessagingInterface::kDataLoaded:
        
		break;
	case SKSE::MessagingInterface::kPostLoad:
		break;
	case SKSE::MessagingInterface::kPreLoadGame:
		break;
	case SKSE::MessagingInterface::kPostLoadGame:
        break;
	case SKSE::MessagingInterface::kNewGame:
		break;
	}
}
void InitializeSerialization()
{
	SKSE::log::trace("Initializing cosave serialization...");
	auto *serde = SKSE::GetSerializationInterface();
	serde->SetUniqueID(KnockoutExtensions::SerializeCode);
	serde->SetSaveCallback(KnockoutExtensions::KnockoutHandler::GameSaveCallback);
	serde->SetRevertCallback(KnockoutExtensions::KnockoutHandler::GameRevertCallback);
	serde->SetLoadCallback(KnockoutExtensions::KnockoutHandler::GameLoadCallback);
	SKSE::log::trace("Cosave serialization initialized.");
}


SKSEPluginLoad(const SKSE::LoadInterface *skse) {
    SKSE::Init(skse);
	SetupLog();


    auto messaging = SKSE::GetMessagingInterface();
	if (!messaging->RegisterListener("SKSE", MessageHandler)) {
		return false;
	}
	InitializeSerialization();
	// KnockoutExtensions::UnconsciousStateHook::Install();
	KnockoutExtensions::BleedoutStateHook::Install();
	KnockoutExtensions::MainUpdateHook::Install();
	KnockoutExtensions::HitEventHook::Install();
	
    return true;
}

