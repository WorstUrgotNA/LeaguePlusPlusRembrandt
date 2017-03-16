#pragma once

#include "PluginSDK.h"

class Gui;
class Tracker;
class GankDetection;

class Awareness
{
public:
	Awareness(IMenu* Parent);
	~Awareness();

	void _OnGameUpdate();
	void _OnRender();
	void _OnCreateObject(IUnit*);
	void _OnDestroyObject(IUnit*);
	void _OnUnitDeath(IUnit* Args);
	void _OnProcessSpell(CastedSpell const&);
	void _OnJungleNotify(JungleNotifyData* Args);

private:
	void LoadEvents();
	void UnloadEvents();

private:
	PLUGIN_EVENT(void) OnGameUpdate();
	PLUGIN_EVENT(void) OnRender();
	PLUGIN_EVENT(void) OnCreateObject(IUnit*);
	PLUGIN_EVENT(void) OnDestroyObject(IUnit*);
	PLUGIN_EVENT(void) OnUnitDeath(IUnit*);
	PLUGIN_EVENT(void) OnProcessSpell(CastedSpell const&);
	PLUGIN_EVENT(void) OnJungleNotify(JungleNotifyData* Args);

private:
	IMenu*			Menu;
	Gui*			Ui;
	Tracker*		Tracking;
	GankDetection*	Ganks;
};

extern Awareness* GPluginInstance;