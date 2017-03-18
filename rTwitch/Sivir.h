#pragma once
#include "PluginSDK.h"

class Sivir
{
public:
	Sivir(IMenu* Parent);
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
	ISpell2*		Q;
	ISpell2*		W;
	ISpell2*		E;
	ISpell2*		R;
	IUnit*			Hero;

	short			keystate;
	bool			SpellFarm;
};
