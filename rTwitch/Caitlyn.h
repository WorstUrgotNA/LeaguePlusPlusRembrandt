#pragma once
#include "PluginSDK.h"

class Caitlyn
{
public:
	Caitlyn(IMenu* Parent);
	~Caitlyn();

	int EnemiesInRange(IUnit* Source, float range);
	int AlliesInRange(IUnit* Source, float range);
	float CalcRDamage(IUnit* Target);

	void OnGameUpdate();
	void OnRender();
	void OnSpellCast(CastedSpell const& Args);
	void OnOrbwalkAttack(IUnit* Source, IUnit* Target);
	void Combo();
	void LaneClear();

private:
	IMenu*			CaitlynMenu;
	IMenuOption*	SemiManualMenuKey;
	IMenuOption*	DrawReady;
	IMenuOption*	DrawQ;
	IMenuOption*	DrawW;
	IMenuOption*	DrawE;
	IMenuOption*	DrawR;
	IMenuOption*	WDelay;
	IMenuOption*	TrapEnemyCast;
	IMenuOption*	UltRange;
	IMenuOption*	EnemyToBlockR;
	IMenuOption*	LaneClearMana;
	IMenuOption*	LaneClearQ;
	std::vector<std::string> TrapEnemyCastType;

private:
	ISpell2*		Q;
	ISpell2*		W;
	ISpell2*		E;
	ISpell2*		R;
	short			keystate;
	IUnit*			ComboTarget;
	bool			SemiManualKey;
	bool			ComboTrap;
	float			LastTrapTime;
};