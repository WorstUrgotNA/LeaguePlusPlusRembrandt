#pragma once
#include "PluginSDK.h"
#include "Champion.h"

class Kogmaw : public virtual Champion
{
public:
	Kogmaw(IMenu* Parent, IUnit* Hero);
	~Kogmaw();
	
	void OnGameUpdate();
	void OnRender();
	void OnOrbwalkAttack(IUnit* Source, IUnit* Target);
	void OnSpellCast(CastedSpell const& Args);
	void BeforeAttack(IUnit* Target);

private:
	IMenu*			KogmawMenu;

	IMenuOption*	QinCombo;
	IMenuOption*	EinCombo;
	IMenuOption*	WinCombo;
	IMenuOption*	RinCombo;

	IMenuOption*	DrawReady;
	IMenuOption*	DrawQ;
	IMenuOption*	DrawW;
	IMenuOption*	DrawE;
	IMenuOption*	DrawR;

	IMenuOption*	RMaxStacks;
	IMenuOption*	RMaxStacksHarass;
	IMenuOption*	RHarassMana;
	IMenuOption*	ECastDistance;

	IMenuOption*	SemiManualMenuKey;

	void Combo();
	void OnWaveClear();
	float CalcRDamage(IUnit* Target);
	float GetWRange();
	float GetRRange();
};
