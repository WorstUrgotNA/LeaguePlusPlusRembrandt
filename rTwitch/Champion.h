#pragma once

#include "PluginSDK.h"

enum Champions
{
	KOGMAW, CAITLYN, TWITCH, GRAVES, TRISTANA, SIVIR
};

class Champion
{
public:
	Champion(IMenu* Parent, IUnit* Hero):Hero(Hero), ParentMenu(Parent){}
	~Champion();
	IUnit * GetHero();
	IMenu* GetMenu();

	virtual void Combo() {};
	//events
	virtual void OnGameUpdate();
	virtual void OnRender();
	virtual void OnRealSpellCast(CastedSpell const& Args);
	virtual void OnSpellCast(CastedSpell const& Args);
	virtual bool OnPreCast(int Slot, IUnit* Target, Vec3* StartPosition, Vec3* EndPosition);
	virtual void OnOrbwalkAttack(IUnit* Source, IUnit* Target);
	virtual void BeforeAttack(IUnit* Target);
	virtual void OnInterruptible(InterruptibleSpell const& Args);
	virtual void OnGapCloser(GapCloserSpell const& Args);

protected :
	IUnit* Hero;
	IMenu* ParentMenu;
	ISpell2*		Q;
	ISpell2*		W;
	ISpell2*		E;
	ISpell2*		R;
	short			keystate;
	IUnit*			ComboTarget;
	bool			SemiManualKey;
};