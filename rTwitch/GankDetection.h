#pragma once

#include "PluginSDK.h"
#include <map>

struct GankHero
{
	GankHero(IUnit* _Player)
	{
		Player				= _Player;
		IsJungle			= Player->GetSpellSlot("SummonerSmite") != kSlotUnknown;
		IsVisible			= Player->IsVisible();
		LastVisibleTime		= GGame->Time();
	}

	IUnit*	Player;
	Vec3	Position;
	float	Distance;
	float	LastVisibleTime;
	float	LastHiddenTime;
	float	TotalTimeHidden;
	bool	IsVisible;
	bool	IsJungle;
	float	LastHiddenTimeGank;
	ITexture* ChampIcon;
};

struct JungleNotification
{
	Vec3				WorldPosition;
	Vec2				MinimapPosition;
	float				TriggerTime;
};

struct GankMenu
{
	IMenu*			Parent;
	IMenuOption*	ShowPredictedMovementCircle;
	IMenuOption*	ShowMovementCircle;
	IMenuOption*	LastSeen;
	IMenuOption*	IconDuration;
	IMenuOption*	DrawEnemyRadar;
	IMenuOption*	ShowClicks;
	IMenuOption*	GankPingDistance;
	IMenuOption*	GankWaitTime;
	IMenuOption*	RenderGankDetectionCircle;
	IMenuOption*	EnemyTowerRangeEnabled;
	IMenuOption*	FriendlyTowerRangeEnabled;
	IMenuOption*	DrawGankLine;
	IMenuOption*	DrawGankIcon;
	IMenuOption*	DrawJunglerTracker;
	//fow tracker
	IMenuOption* TrackJungler;
	IMenuOption* DrawJunglerTrackerPingLocal;
	IMenuOption* DrawJunglerTrackerPingGlobal;
	IMenuOption* DrawJunglerTrackerPingType;
	IMenuOption* PingInterval;

	std::map<int, IMenuOption*> ChampionsToPingOnGank;
};


class GankDetection
{
public:
	GankDetection(IMenu* Parent);
	~GankDetection();

	void OnGameUpdate();
	void OnRender();
	void OnJungleNotify(JungleNotifyData* Args);

private:
	void UpdateChampions();
	void GetHealthColor(GankHero* Hero, Vec4& Out);
	bool DoesTextureExist(std::string const& Filename, std::string& FullPath);
	ITexture* CreateTextureEx(std::string const& Filename);

private:
	GankMenu				Menu;
	IFont*					Tahoma13;
	IFont*					Tahoma13Bold;
	IFont*					Font1;
	std::vector<GankHero>	Heros;
	std::vector<JungleNotification>	JungleNotifications;
	float					LastPingTime;
	IUnit*					IncomingGankUnit;
	float					ShowGankEndTime;
	ITexture*				ChampIcons[4];
	Vec3						JGDisplayPos;
	float						JGDelay;
	float						LastPingTimeTracker;
	float						LastPingTime2;
	bool						FoWUpdated;
	bool						JGNUpdated;
};