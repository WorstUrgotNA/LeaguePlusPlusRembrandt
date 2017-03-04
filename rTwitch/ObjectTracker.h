#pragma once

#include "PluginSDK.h"

struct TimedObject{
	bool			IsMoving;
	float			EndTime;
	IUnit*			ObjectUnit;
	Vec3			StagnantPosition;
};

class ObjectTracker
{
public:
	ObjectTracker(IMenu* Parent);
	~ObjectTracker();
	void _OnProcessSpell(CastedSpell const& Args);
	void _OnBuffAdd(IUnit* Source, void* BuffData);
	void _OnRender();
	void _OnCreateObject(IUnit* Source);

private:
	void			LoadEvents();
	void			UnloadEvents();
	IMenu*			Menu;
	IMenuOption*	Enabled;
	IFont*			TrackerFont;
	std::vector<TimedObject>	TimedObjects;

private:
	PLUGIN_EVENT(void) OnProcessSpell(CastedSpell const& Args);
	PLUGIN_EVENT(void) OnCreateObject(IUnit* Source);
	PLUGIN_EVENT(void) OnBuffAdd(IUnit* Source, void* BuffData);
	PLUGIN_EVENT(void) OnRender();
}; 

extern ObjectTracker* GPluginInstance2;