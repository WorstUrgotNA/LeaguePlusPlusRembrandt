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
};

struct GankMenu
{
	IMenu*			Parent;
	IMenuOption*	ShowPredictedMovementCircle;
	IMenuOption*	DrawEnemyRadar;
	IMenuOption*	ShowClicks;
	IMenuOption*	GankPingDistance;
	IMenuOption*	GankWaitTime;
	IMenuOption*	RenderGankDetectionCircle;
	IMenuOption*	EnemyTowerRangeEnabled;
	IMenuOption*	FriendlyTowerRangeEnabled;
	IMenuOption*	DrawJunglerTracker;

	std::map<int, IMenuOption*> ChampionsToPingOnGank;
};


class GankDetection
{
public:
	GankDetection(IMenu* Parent);
	~GankDetection();

	void OnGameUpdate();
	void OnRender();

private:
	void UpdateChampions();
	void GetHealthColor(GankHero* Hero, Vec4& Out);

private:
	GankMenu				Menu;
	IFont*					Tahoma13;
	IFont*					Tahoma13Bold;
	IFont*					Font1;
	std::vector<GankHero>	Heros;
	float					LastPingTime;
	IUnit*					IncomingGankUnit;
	float					ShowGankEndTime;
	ITexture*				ChampIcons[4];
};