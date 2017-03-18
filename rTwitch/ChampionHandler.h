#pragma once
#include "PluginSDK.h"

class Graves;
class Caitlyn;
class Twitch;
class Sivir;
class Tristana;

class ChampionHandler
{
public:
	ChampionHandler();
	~ChampionHandler();

private:
	void			LoadEvents();
	void			UnloadEvents();
	void			_OnGameUpdate();
	void			_OnSpellCast(CastedSpell const& Args);
	bool			_OnPreCast(int Slot, IUnit* Target, Vec3* StartPosition, Vec3* EndPosition);
	void			_OnOrbwalkAttack(IUnit* Source, IUnit* Target);
	void			_OnRender();
	void			_BeforeAttack(IUnit* Target);
	IMenu*			Menu;

private:
	PLUGIN_EVENT(void) OnGameUpdate();
	PLUGIN_EVENT(void) OnRender();
	PLUGIN_EVENT(void) OnSpellCast(CastedSpell const& Args);
	PLUGIN_EVENT(void) OnPreCast(int Slot, IUnit* Target, Vec3* StartPosition, Vec3* EndPosition);
	PLUGIN_EVENT(void) OnOrbwalkAttack(IUnit* Source, IUnit* Target);
	PLUGIN_EVENT(void) ChampionHandler::BeforeAttack(IUnit* Target);
	Graves* GravesHandler;
	Caitlyn* CaitlynHandler;
	Twitch* TwitchHandler;
	Sivir* SivirHandler;
	Tristana* TristanaHandler;
};

extern ChampionHandler* GPluginInstance3;