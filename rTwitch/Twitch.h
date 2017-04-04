#pragma once
#include "PluginSDK.h"

class Twitch
{
public:
	Twitch(IMenu* Parent);
	~Twitch();

	int EnemiesInRange(IUnit* Source, float range);
	float CalcEDamage(IUnit* Target);

	void OnGameUpdate();
	void OnRender();
	void OnRealSpellCast(CastedSpell const& Args);
	void OnOrbwalkAttack(IUnit* Source, IUnit* Target);
	bool OnPreCast(int Slot, IUnit* Target, Vec3* StartPosition, Vec3* EndPosition);
	void Combo();

private:
	IMenu*			TwitchMenu;
	IMenuOption*	DrawReady;
	IMenuOption*	DrawQ;
	IMenuOption*	DrawW;
	IMenuOption*	DrawE;
	IMenuOption*	DrawR;
	IMenuOption*	ComboETypeOption;
	IMenuOption*	ComboWOption;
	IMenuOption*	SaveManaForE;
	IMenuOption*	ComboQOption;
	IMenuOption*	QRecall;
	IMenuOption*	WUnderTurret;
	IMenuOption*	EEnemyLeaving;
	IMenuOption*	EJungleKS;
	IMenuOption*	QInRange;

	std::vector<std::string> ComboEType;

private:
	ISpell2*		Q;
	ISpell2*		W;
	ISpell2*		E;
	ISpell2*		R;
	ISpell2*		RECALL;
	IUnit*			Hero;
};
