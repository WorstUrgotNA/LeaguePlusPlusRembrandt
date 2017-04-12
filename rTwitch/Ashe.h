#pragma once
#include "PluginSDK.h"
#include "Champion.h"


class Ashe : public virtual Champion
{
public:
	Ashe(IMenu* Parent, IUnit* Hero);
	~Ashe();

	void Combo();
	void LaneClear();

	void OnGameUpdate();
	void OnRender();
	void OnSpellCast(CastedSpell const& Args);
	void OnExitVisible(IUnit* Source);

private:
	IMenu*			AsheMenu;

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

	IMenu*			ComboMenu;
	IMenuOption*	QInCombo;
	IMenuOption*	WInCombo;
	IMenuOption*	RInCombo;
	IMenuOption*	SemiManualMenuKey;
	IMenuOption*	RInComboRange;

	IMenu*			HarassMenu;
	IMenuOption*	WInHarass;
	IMenuOption*	WInHarassMana;

	IMenu*			LaneClearMenu;
	IMenuOption*	QInLaneClear;
	IMenuOption*	QInLaneClearMana;
	IMenuOption*	QInLaneClearNearbyUnits;

	IMenu*			ExtraMenu;
	IMenuOption*	EFow;

private:
	float LastAttackTime;
};