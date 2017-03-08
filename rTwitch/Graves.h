#pragma once
#include "PluginSDK.h"

class Graves
{
public:
	Graves(IMenu* Parent);
	~Graves();

	int EnemiesInRange(IUnit* Source, float range);
	float CalcRDamage(IUnit* Target);

	void OnGameUpdate();
	void OnRender();
	void OnSpellCast(CastedSpell const& Args);
	void ClearStack();
	void Combo();

private:
	IMenu*			GravesMenu;
	IMenuOption*	ComboPercent;
private:
	ISpell2*		Q;
	ISpell2*		W;
	ISpell2*		E;
	ISpell2*		R;
	short			keystate;
	IUnit*			ComboTarget;
	int				StackIndex;
	bool			SemiManualKey;
	std::vector<std::string> Stack;
};