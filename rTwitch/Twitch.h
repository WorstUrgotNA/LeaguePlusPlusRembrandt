#pragma once
#include "PluginSDK.h"
#include "Champion.h"

class Twitch : public virtual Champion
{
public:
	Twitch(IMenu* Parent, IUnit* Hero);
	~Twitch();

	int EnemiesInRange(IUnit* Source, float range);
	float CalcEDamage(IUnit* Target);

	void OnGameUpdate();
	void LaneClear();
	void OnRender();
	void OnRealSpellCast(CastedSpell const& Args);
	void OnOrbwalkAttack(IUnit* Source, IUnit* Target);
	bool OnPreCast(int Slot, IUnit* Target, Vec3* StartPosition, Vec3* EndPosition);

	void Combo();

private:
	IMenu*			TwitchMenu;

	IMenu*			DrawMenu;
	IMenuOption*	DrawReady;
	IMenuOption*	DrawW;
	IMenuOption*	DrawE;
	IMenuOption*	DrawR;
	IMenuOption*	WColor;
	IMenuOption*	EColor;
	IMenuOption*	RColor;
	IMenuOption*	DrawEDamage;
	IMenuOption*	DrawEDamageColor;

	IMenu*			ComboMenu;

	IMenu*			HarassMenu;

	IMenu*			LaneClearMenu;

	IMenu*			ExtraMenu;

	IMenuOption*	ComboECustomStacks;
	IMenuOption*	ComboETypeOption;
	IMenuOption*	ComboWOption;
	IMenuOption*	SaveManaForE;
	IMenuOption*	ComboQOption;
	IMenuOption*	QRecall;
	IMenuOption*	WUnderTurret;
	IMenuOption*	EEnemyLeaving;
	IMenuOption*	EJungleKS;
	IMenuOption*	QInRange;
	IMenuOption*	ComboR;
	IMenuOption*	HarassW;
	IMenuOption*	HarassWMana;
	IMenuOption*	HarassE;

	std::vector<std::string> ComboRType;
	std::vector<std::string> ComboWType;
	std::vector<std::string> ComboEType;
	std::vector<std::string> HarassEType;
};
