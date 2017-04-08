#pragma once
#include "PluginSDK.h"
#include "Champion.h"

class Graves: public virtual Champion
{
public:
	Graves(IMenu* Parent, IUnit* Hero);
	~Graves();

	int EnemiesInRange(IUnit* Source, float range);
	float CalcRDamage(IUnit* Target);
	
	void ClearStack();
	//virtual override
	void OnGameUpdate();
	void Combo();
	void OnRender();
	void OnSpellCast(CastedSpell const& Args);

private:
	IMenu*			GravesMenu;
	IMenuOption*	SemiManualMenuKey;
	IMenuOption*	UseWCombo;
	IMenuOption*	UseQJungle;
	IMenuOption*	UseWJungle;
	IMenuOption*	UseEJungle;
	IMenuOption*	QJungleMana;
	IMenuOption*	WJungleMana;
	IMenuOption*	EJungleMana;
	IMenuOption*	ComboQTypeOption;

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
	IMenuOption*	UseECombo;
	IMenuOption*	UseRCombo;

	IMenu*			ComboMenu;

	IMenu*			HarassMenu;

	IMenu*			LaneClearMenu;

	IMenu*			ExtraMenu;

	std::vector<std::string> ComboQType;

private:

	int				StackIndex;
	std::vector<std::string> Stack;
};