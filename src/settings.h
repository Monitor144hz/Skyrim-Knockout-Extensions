#pragma once

#include "SimpleIni.h"

namespace KnockoutExtensions
{
    class Settings
    {
        public: 

        static void Load()
        {
            constexpr auto path = L"Data/SKSE/Plugins/Knockout_Extensions/Settings.ini";

            CSimpleIniA ini;
            ini.SetUnicode();

            if (const auto rc = ini.LoadFile(path); rc < 0)
            {
                SKSE::log::error("Failed to load settings.ini!");
                return;
            }
            knockoutTriggerOptions.backBash = ini.GetBoolValue("Triggers", "BackBash", true);
            knockoutTriggerOptions.bleedoutBash = ini.GetBoolValue("Triggers", "BleedoutBash", true);
            knockoutTriggerOptions.essentialDown = ini.GetBoolValue("Triggers", "EssentialDown", true);

            unconsciousStatusOptions.durationHours = static_cast<float>(ini.GetDoubleValue("Status", "UnconsciousDuration", 8.0));
            unconsciousStatusOptions.isCrime = ini.GetBoolValue("Status", "IsCrime", true);
        }

        [[nodiscard]] static bool GetTriggerEssentialDown() { return knockoutTriggerOptions.essentialDown; }

        [[nodiscard]] static bool GetTriggerBackBash() { return knockoutTriggerOptions.backBash; }

        [[nodiscard]] static bool GetTriggerBleedoutBash() { return knockoutTriggerOptions.backBash; }

        [[nodiscard]] static float GetUnconsciousDuration() { return unconsciousStatusOptions.durationHours; }

        [[nodiscard]] static bool GetKnockoutIsCrime() { return unconsciousStatusOptions.isCrime; }


        private:

        struct KnockoutTriggers
        {
            bool essentialDown = true;
            bool backBash = true;
            bool bleedoutBash = true;
        };

        struct UnconsciousStatus
        {
            float durationHours = 4.0f;
            bool isCrime = true;
        };


        static inline KnockoutTriggers knockoutTriggerOptions; 

        static inline UnconsciousStatus unconsciousStatusOptions; 



    };


}