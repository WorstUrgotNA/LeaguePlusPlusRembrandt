#pragma once
#include "PluginSDK.h"
#include "Champion.h"

class Tristana : public virtual Champion
{
public:
	Tristana(IMenu* Parent, IUnit* Hero);
	~Tristana();

	int EnemiesInRange(IUnit* Source, float range);

	void OnGameUpdate();
	void OnRender();
	void OnOrbwalkAttack(IUnit* Source, IUnit* Target);
	void OnSpellCast(CastedSpell const& Args);
	void BeforeAttack(IUnit* Target);
	void OnInterruptible(InterruptibleSpell const& Args);
	void OnGapCloser(GapCloserSpell const& Args);
	void Combo();
	float CalcRDamage(IUnit* Target);
	float CalcEDamage(IUnit* Target);

private:
	IMenu*			TristanaMenu;

	IMenuOption*	QinCombo;
	IMenuOption*	EinCombo;
	IMenuOption*	RinCombo;
	IMenuOption*	InteruptSpellCasts;
	IMenuOption*	RGapCloser;

	IMenuOption*	DrawReady;
	IMenuOption*	DrawW;
};
