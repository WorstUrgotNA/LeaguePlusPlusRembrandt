#pragma once
#include "PluginSDK.h"

class Kogmaw
{
public:
	Kogmaw(IMenu* Parent);
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
	float Kogmaw::GetWRange();
	float GetRRange();
	bool SemiManualKey;
	short keystate;

private:
	ISpell2*		Q;
	ISpell2*		W;
	ISpell2*		E;
	ISpell2*		R;
	IUnit*			Hero;
};
