#pragma once
#include "PluginSDK.h"

class Graves;

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
	IMenu*			Menu;

private:
	PLUGIN_EVENT(void) OnGameUpdate();
	PLUGIN_EVENT(void) OnSpellCast(CastedSpell const& Args);
	Graves* GravesHandler;
};

extern ChampionHandler* GPluginInstance3;