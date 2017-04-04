#pragma once
#include "PluginSDK.h"

class Graves;
class Caitlyn;
class Twitch;
class Sivir;
class Tristana;
class Kogmaw;


class ChampionHandler
{
public:
	ChampionHandler();
	~ChampionHandler();

private:
	void			LoadEvents();
	void			UnloadEvents();
	void			_OnGameUpdate();
	void			_OnRealSpellCast(CastedSpell const& Args);
	void			_OnSpellCast(CastedSpell const& Args);
	bool			_OnPreCast(int Slot, IUnit* Target, Vec3* StartPosition, Vec3* EndPosition);
	void			_OnOrbwalkAttack(IUnit* Source, IUnit* Target);
	void			_OnRender();
	void			_BeforeAttack(IUnit* Target);
	void			_OnInterruptible(InterruptibleSpell const& Args);
	void			_OnGapCloser(GapCloserSpell const& Args);

	IMenu*			Menu;
	IMenuOption*	LoadChampPlugin;
	bool			PluginActive;

private:
	PLUGIN_EVENT(void) OnGameUpdate();
	PLUGIN_EVENT(void) OnRender();
	PLUGIN_EVENT(void) OnRealSpellCast(CastedSpell const& Args);
	PLUGIN_EVENT(void) OnSpellCast(CastedSpell const& Args);
	PLUGIN_EVENT(void) OnPreCast(int Slot, IUnit* Target, Vec3* StartPosition, Vec3* EndPosition);
	PLUGIN_EVENT(void) OnOrbwalkAttack(IUnit* Source, IUnit* Target);
	PLUGIN_EVENT(void) BeforeAttack(IUnit* Target);
	PLUGIN_EVENT(void) OnInterruptible(InterruptibleSpell const& Args);
	PLUGIN_EVENT(void) OnGapCloser(GapCloserSpell const& Args);
	Graves* GravesHandler;
	Caitlyn* CaitlynHandler;
	Twitch* TwitchHandler;
	Sivir* SivirHandler;
	Tristana* TristanaHandler;
	Kogmaw* KogmawHandler;
};

extern ChampionHandler* GPluginInstance3;