#pragma once
#include "PluginSDK.h"

class Tristana
{
public:
	Tristana(IMenu* Parent);
	~Tristana();

	int EnemiesInRange(IUnit* Source, float range);

	void OnGameUpdate();
	void OnRender();
	void OnOrbwalkAttack(IUnit* Source, IUnit* Target);
	void OnSpellCast(CastedSpell const& Args);
	void BeforeAttack(IUnit* Target);
	void Combo();
	float CalcRDamage(IUnit* Target);
	float CalcEDamage(IUnit* Target);

private:
	IMenu*			TristanaMenu;

	IMenuOption*	DrawReady;
	IMenuOption*	DrawW;

private:
	ISpell2*		Q;
	ISpell2*		W;
	ISpell2*		E;
	ISpell2*		R;
	IUnit*			Hero;
};
