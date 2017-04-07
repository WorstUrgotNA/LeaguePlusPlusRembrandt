#pragma once
#include "PluginSDK.h"
#include "Champion.h"


class Caitlyn : public virtual Champion
{
public:
	Caitlyn(IMenu* Parent, IUnit* Hero);
	~Caitlyn();

	int EnemiesInRange(IUnit* Source, float range);
	int AlliesInRange(IUnit* Source, float range);
	float CalcSpellDamage(IUnit* Target, eSpellSlot Slot);

	void Combo();
	void LaneClear();

	void OnGameUpdate();
	void OnRender();
	void OnSpellCast(CastedSpell const& Args);
	void OnOrbwalkAttack(IUnit* Source, IUnit* Target);
	bool OnPreCast(int Slot, IUnit* Target, Vec3* StartPosition, Vec3* EndPosition);

private:
	IMenu*			CaitlynMenu;

	IMenu*			DrawMenu;

	IMenu*			ComboMenu;

	IMenu*			HarassMenu;

	IMenu*			LaneClearMenu;

	IMenu*			ExtraMenu;

	IMenuOption*	WAfterE;
	IMenuOption*	SafeQKS;
	IMenuOption*	RInCombo;
	IMenuOption*	TrapEnemyCastHarass;
	IMenuOption*	SafeQHarass;
	IMenuOption*	SafeQHarassMana;
	IMenuOption*	TrapImmobileCombo;
	IMenuOption*	QInCombo;
	IMenuOption*	ShortQDisableLevel;
	IMenuOption*	SemiManualMenuKey;
	IMenuOption*	DrawReady;
	IMenuOption*	QColor;
	IMenuOption*	WColor;
	IMenuOption*	EColor;
	IMenuOption*	RColor;
	IMenuOption*	DrawQ;
	IMenuOption*	DrawW;
	IMenuOption*	DrawE;
	IMenuOption*	DrawR;
	IMenuOption*	DrawRDamage;
	IMenuOption*	DrawRDamageColor;
	IMenuOption*	WDelay;
	IMenuOption*	TrapEnemyCast;
	IMenuOption*	UltRange;
	IMenuOption*	EnemyToBlockR;
	IMenuOption*	LaneClearMana;
	IMenuOption*	LaneClearQ;
	IMenuOption*	EToMouse;
	IMenuOption*	EWhenClose;
	IMenuOption*	EBeforeLevel;

	std::vector<std::string> TrapEnemyCastType;

private:

	bool			ComboTrap;
	bool			UseNet;
	bool			UseNetCombo;
	float			LastTrapTime;
};