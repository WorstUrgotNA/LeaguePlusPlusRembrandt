#pragma once
#include "PluginSDK.h"

class Caitlyn
{
public:
	Caitlyn(IMenu* Parent);
	~Caitlyn();

	int EnemiesInRange(IUnit* Source, float range);
	int AlliesInRange(IUnit* Source, float range);
	float CalcSpellDamage(IUnit* Target, eSpellSlot Slot);

	void OnGameUpdate();
	void OnRender();
	void OnSpellCast(CastedSpell const& Args);
	void OnOrbwalkAttack(IUnit* Source, IUnit* Target);
	bool OnPreCast(int Slot, IUnit* Target, Vec3* StartPosition, Vec3* EndPosition);
	void Combo();
	void LaneClear();

private:
	IMenu*			CaitlynMenu;
	IMenu*			ComboMenu;
	IMenu*			DrawMenu;
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
	IMenuOption*	DrawQ;
	IMenuOption*	QColor;
	IMenuOption*	WColor;
	IMenuOption*	EColor;
	IMenuOption*	RColor;
	IMenuOption*	DrawW;
	IMenuOption*	DrawE;
	IMenuOption*	DrawR;
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
	ISpell2*		Q;
	ISpell2*		W;
	ISpell2*		E;
	ISpell2*		R;
	short			keystate;
	IUnit*			ComboTarget;
	IUnit*			Hero;
	bool			SemiManualKey;
	bool			ComboTrap;
	bool			UseNet;
	bool			UseNetCombo;
	float			LastTrapTime;
};