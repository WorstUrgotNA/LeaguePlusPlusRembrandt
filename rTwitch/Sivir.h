#pragma once
#include "PluginSDK.h"
#include "Champion.h"

class Sivir : public virtual Champion
{
public:
	Sivir(IMenu* Parent, IUnit* Hero);
	~Sivir();

	int EnemiesInRange(IUnit* Source, float range);

	void OnGameUpdate();
	void OnRender();
	void OnOrbwalkAttack(IUnit* Source, IUnit* Target);
	void OnSpellCast(CastedSpell const& Args);
	void Combo();
	void LaneClear();
	void HandleKeyPress();

private:
	IMenu*			SivirMenu;
	IMenuOption*	QLaneClear;
	IMenuOption*	WLaneClear;
	IMenuOption*	SpellFarmToggle;
	IMenuOption*	FarmWithSpells;
	IMenuOption*	QHitChance;

	IMenuOption*	DrawReady;
	IMenuOption*	DrawQ;

	std::vector<std::string> HitChance;

private:
	bool			SpellFarm;
};
