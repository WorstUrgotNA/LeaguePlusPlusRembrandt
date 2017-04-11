#pragma once
#include "PluginSDK.h"
#include "Champion.h"

class Lucian : public virtual Champion
{
public:
	Lucian(IMenu* Parent, IUnit* Hero);
	~Lucian();

	bool CastE(IUnit* Target);
	void LaneClear();
	double CalcRDamage(IUnit* Target);

	void OnGameUpdate();
	void OnRender();
	void OnSpellCast(CastedSpell const& Args);
	void OnLevelUp(IUnit* Source, int NewLevel);

private:
	IMenu*			LucianMenu;

	IMenu*			DrawMenu;
	IMenuOption*	DrawReady;
	IMenuOption*	DrawQ;
	IMenuOption*	DrawW;
	IMenuOption*	DrawE;
	IMenuOption*	DrawR;
	IMenuOption*	QColor;
	IMenuOption*	WColor;
	IMenuOption*	EColor;
	IMenuOption*	RColor;
	IMenuOption*	DrawRDamage;
	IMenuOption*	DrawRDamageColor;
	IMenuOption*	DrawSpellFarm;

	IMenu*			ComboMenu;
	IMenuOption*	QInCombo;
	IMenuOption*	WInCombo;
	IMenuOption*	EInCombo;
	IMenuOption*	RInCombo;
	IMenuOption*	RLock;

	IMenu*			HarassMenu;
	IMenuOption*	HarrasQ;
	IMenuOption*	ExtendedQHarass;
	IMenuOption*	HarassQMana;

	IMenu*			LaneClearMenu;
	IMenuOption*	SpellFarmToggle;
	IMenuOption*	SpellFarmEnabled;
	IMenuOption*	QSpellFarm;
	IMenuOption*	MinimumMinionsToQ;
	IMenuOption*	EFarmMana;
	IMenuOption*	QFarmMana;
	IMenuOption*	WFarmMana;
	IMenuOption*	QJGMana;
	IMenuOption*	EJGMana;
	IMenuOption*	WJGMana;
	IMenuOption*	WSpellFarm;
	IMenuOption*	ESpellFarm;
	IMenuOption*	QInJGClear;
	IMenuOption*	WInJGClear;
	IMenuOption*	EInJGClear;


private:
	bool			SpellFarm;
	ISpell2*		QExtended;
};