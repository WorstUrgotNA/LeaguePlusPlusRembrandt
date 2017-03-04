#pragma once

#include "PluginSDK.h"

enum eHiddenObjectType
{
	kHiddenMissile		= 0,
	kHiddenWard			= 1,
	kHiddenPinkWard		= 2,
	kHiddenTrap			= 3,
	kHiddenBlueWard		= 4,
	kWardCorpse			= 5
};

struct TrackerJungleMenu
{
	IMenuOption* TrackJungleTimers;
	IMenuOption* Show3DTimer;
	IMenuOption* ShowMinimapTimer;
	IMenuOption* ShowNotifications;
	IMenuOption* TrackGromp;
	IMenuOption* TrackBlue;
	IMenuOption* TrackWolves;
	IMenuOption* TrackRaptors;
	IMenuOption* TrackRed;
	IMenuOption* TrackKrugs;
	IMenuOption* TrackDrag;
	IMenuOption* TrackCrabs;
	
};

struct TrackerMenu
{
	IMenu*				Owner;
	IMenuOption*		Enabled;
	IMenuOption*		TrackWards;
	IMenuOption*		ShowVisionRange;
	TrackerJungleMenu	Jungle;
};

struct JungleMob
{
	JungleMob(std::string _Name, bool _IsDead)
	{
		Name	= _Name;
		IsDead	= _IsDead;
	}

	std::string Name;
	bool IsDead;
};

struct HiddenObject
{
	Vec3				WorldPosition;
	Vec2				MinimapPosition;
	eHiddenObjectType	Type;
	float				DespawnTime;
	bool				IsValid;
};


struct ClonedUnit
{
	ClonedUnit(IUnit* _RealPlayer, IUnit* _ClonedPlayer)
	{
		RealPlayer		= _RealPlayer;
		ClonedPlayer	= _ClonedPlayer;
		IsValid			= true;
	}

	IUnit*	RealPlayer;
	IUnit*	ClonedPlayer;
	bool	IsValid;
};

struct JungleCamp
{
	JungleCamp(float _NextSpawnTime, float _CooldownTime, Vec3 const& _Position, std::string const& _Name, std::vector<std::string> const& _Mobs)
	{
		Position		= _Position;
		Name			= _Name;
		CooldownTime	= _CooldownTime;
		NextSpawnTime	= _NextSpawnTime;
		IsTracking		= true;
		IsDead			= false;
		Notified		= false;

		for (auto s : _Mobs)
			Mobs.emplace_back(s, false);
	}

	Vec3						Position;
	std::string					Name;
	std::vector<JungleMob>		Mobs;
	float						CooldownTime;
	float						NextSpawnTime;
	bool						IsTracking;
	bool						IsDead;
	bool						Notified;
};

class Tracker
{
public:
	Tracker(IMenu* Parent);
	~Tracker();

	void OnGameUpdate();
	void OnRender();
	void OnCreateObject(IUnit* Args);
	void OnDestroyObject(IUnit* Args);
	void OnUnitDeath(IUnit* Args);
	void OnProcessSpell(CastedSpell const& Args);
	void StrTimeFormat(int Seconds);

private:
	void LoadMenu(IMenu* Parent);
	void LoadJungleCamps();

	bool AddHiddenObject(CastedSpell const& Args);
	bool IsCampDead(JungleCamp const& Camp);
	void UpdateDragonAndBaron();
	void UpdateHiddenObjects(IUnit* Args);
	bool DoesTextureExist(std::string const& Filename, std::string& FullPath);
	ITexture* CreateTextureEx(std::string const& Filename);

private:
	TrackerMenu					Menu;
	std::vector<JungleCamp>		Camps;
	std::vector<HiddenObject>	HiddenObjects;
	std::vector<ClonedUnit>		Clones;
	std::string					formattedTime;
	size_t						BaronIdx;
	size_t						RiftIdx;
	size_t						DragonIdx;
	int							DragonStacks;
	IFont*						TrackerFont;
	IFont*						MinimapText;
};