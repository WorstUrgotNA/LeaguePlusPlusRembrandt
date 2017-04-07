#pragma once
#include "PluginSDK.h"

namespace Rembrandt
{
	auto DamageModifierFromMasteries(IUnit* Hero, IUnit* Target) -> double;
	auto DrawDamageOnChampionHPBar(IUnit* Hero, double Damage, const char *Text, Vec4 BarColor) -> void;
	auto DrawDamageOnChampionHPBar(IUnit* Hero, double Damage, Vec4 BarColor) -> void;
	auto LengthSqr(Vec3 v) -> float;
	auto IsInRange(Vec2 PositionA, Vec2 PositionB, float Range) -> bool;
	auto IsInRange(Vec3 PositionA, Vec3 PositionB, float Range) -> bool;
}

