#pragma once
#include "PluginSDK.h"

namespace Rembrandt
{
	auto DamageModifierFromMasteries(IUnit* Hero, IUnit* Target) -> double;
	auto DrawDamageOnChampionHPBar(IUnit* Hero, double Damage, const char *Text, Vec4 BarColor) -> void;
}

