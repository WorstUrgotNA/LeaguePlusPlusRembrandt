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
	bool	IsOnScreen;
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
	IMenuOption*	PingOnAll;
	//fow tracker
	IMenuOption* TrackJungler;
	IMenuOption* DrawJunglerTrackerPingLocal;
	IMenuOption* DrawJunglerTrackerPingGlobal;
	IMenuOption* DrawJunglerTrackerPingType;
	IMenuOption* PingInterval;
	//radar
	IMenuOption* EnableRadar;
	IMenuOption* CircleRadius;
	IMenuOption* DetectionRadius;
	IMenuOption* CircleQuality;

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
	void OnTeleport(OnTeleportArgs* Args);

private:
	void UpdateChampions();
	void GetHealthColor(GankHero* Hero, Vec4& Out);
	bool DoesTextureExist(std::string const& Filename, std::string& FullPath);
	void DrawCircleMinimap(Vec2 Position, float Radius, int Quality, Vec4 Color = Vec4(255, 255, 0, 255));
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
	ITexture*				OutlineCircle;
	Vec3						JGDisplayPos;
	float						JGDelay;
	float						LastPingTimeTracker;
	float						LastPingTime2;
	bool						FoWUpdated;
	bool						JGNUpdated;
};