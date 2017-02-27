#include "Tracker.h"
#include <sstream>
#include <string>
#include <Shlwapi.h>
#include <algorithm>
#include <stdlib.h>

ITexture*	TrapDot;
ITexture*	PinkWard;
ITexture*	EnemyWard;
ITexture*	RC_On;


#pragma comment(lib, "shlwapi")

Tracker::Tracker(IMenu* Parent)
{
	//DragonStacks	= 0;
	Menu.Owner		= Parent->AddMenu("Wards");
	
	TrackerFont		= GRender->CreateFont("Tahoma", 18.f);

	TrackerFont->SetLocationFlags(kFontLocationCenter);
	TrackerFont->SetOutline(true);

	FoWUpdated = false;
	JGDelay = 0.f;
	LastPingTime = 0.f;
	LastPingTime2 = 0.f;

	TrapDot = GRender->CreateTextureFromFile("UtilityPRO/trapdot.png"); //load texture
	PinkWard = GRender->CreateTextureFromFile("UtilityPRO/True_Sight_icon.png"); //load texture
	EnemyWard = GRender->CreateTextureFromFile("UtilityPRO/EnemyWard.png"); //load texture
	RC_On = GRender->CreateTextureFromFile("UtilityPRO/RC_On.png");
	RC_On->SetColor(Vec4(255, 150, 150, 150));
	RC_On->Scale(GRender->ScreenSize().y / 1440.f);

	

	LoadMenu(Parent);
	LoadJungleCamps();
}

Tracker::~Tracker()
{

}

void Tracker::OnJungleNotify(JungleNotifyData* Args)
{
	FoWUpdated = true;
	JGDisplayPos = Args->Position;
	JGDelay = GGame->Time() + 5;
}

void Tracker::OnGameUpdate()
{
	for (auto& obj : HiddenObjects)
	{
		if (obj.IsValid && GGame->Time() >= obj.DespawnTime)
			obj.IsValid = false;
	}
	//clear junglenotifications vector if too much time has passed
	if (GGame->Time() - LastPingTime > 5)
		JungleNotifications.clear();


	if (FoWUpdated) //try to push notification position and time to vector
	{
		if (Menu.Jungle.TrackJungler->Enabled())
		{
			bool FalseAlarm = false;

			for (auto pUnit : GEntityList->GetAllUnits())
			{					
				float flDistance = (pUnit->GetPosition() - JGDisplayPos).Length();
				if (flDistance < 500)
			{
				//GRender->Notification(Vec4(255, 255, 255, 255), 5, "%s distance %f to ping", pUnit->GetObjectName(), flDistance);
				FalseAlarm = true;
				break;
				}				
			}

			if (!FalseAlarm && GGame->Time() - LastPingTime >= 2)
			{
				JungleNotification o;

				GGame->WorldToMinimap(JGDisplayPos, o.MinimapPosition);
				o.WorldPosition = JGDisplayPos;
				o.TriggerTime = JGDelay;

				JungleNotifications.push_back(o);
				//GRender->Notification(Vec4(255, 255, 255, 255), 5, "Size: %i", JungleNotifications.size());
				LastPingTime = GGame->Time();

				if (GGame->Time() - LastPingTime2 >= 5)
				{
					if (Menu.Jungle.DrawJunglerTrackerPingGlobal->Enabled()) //ping global
					{
						GGame->SendPing(kPingNormal, o.WorldPosition);
					}
					else if (Menu.Jungle.DrawJunglerTrackerPingLocal->Enabled())
					{
						GGame->ShowPing(kPingNormal, o.WorldPosition, true);
					}
					LastPingTime2 = GGame->Time();
				}
			}
			FoWUpdated = false;
		}
	}
}

void Tracker::OnProcessSpell(CastedSpell const& Args)
{
	if (!Args.AutoAttack_ && AddHiddenObject(Args) && Menu.TrackWards->Enabled())
		return;
}

void Tracker::StrTimeFormat(int Seconds) //convert seconds to 0:00 format
{
	std::stringstream parser;
	if (Seconds >= 60)
	{
		parser << ((Seconds % 3600) / 60);		             // minutes
		parser << ':';                                      // separator
		if ((Seconds % 60) < 10) parser << "0";               // trailing 0 if seconds < 10
		parser << (Seconds % 60);                          // seconds

		formattedTime = parser.str();
	}
	else
	{
		parser << Seconds;
		formattedTime = parser.str();
	}

}

void Tracker::OnRender()
{
	//Draw FoW Jungle Tracker
	if (Menu.Jungle.TrackJungler->Enabled())
	{
		for (auto obj : JungleNotifications)
		{
			if (obj.TriggerTime - GGame->Time() > 0)
			{
				GRender->DrawOutlinedCircle(obj.WorldPosition, Vec4(255, 0, 0, 255), 20.f); // draw a small circle around notification
				RC_On->Draw(obj.MinimapPosition.x - 15, obj.MinimapPosition.y - 15);
			}
		}
	}
	
	

	/*
	int j = 0;
	for (auto obj : JungleNotifications)
	{
		if ( && obj.TriggerTime + 2 - GGame->Time() >= 0)
		{
			GRender->DrawOutlinedCircle(obj.WorldPosition, Vec4(255, 0, 0, 255), 20.f); // draw a small circle around notification
		}
		else
		{
			JungleNotifications.erase(JungleNotifications.begin() + j);
		}
		j++;
	}*/
	

	if (Menu.TrackWards->Enabled() && Menu.Enabled->Enabled())
	{
		for (auto obj : HiddenObjects)
		{
			if (!obj.IsValid)
			{
				//wipe obj and go to next obj
				obj.WorldPosition = Vec3(0, 0, 0);
				continue;
			}
				

			Vec4 vecColor(255, 255, 0, 255);

			if (obj.Type == kHiddenPinkWard)
				vecColor = Vec4(255, 105, 180, 255);

			if (obj.Type == kHiddenTrap)
				vecColor = Vec4(255, 165, 0, 255);

			if (obj.Type == kHiddenBlueWard)
				vecColor = Vec4(0, 150, 255, 255);
			
			TrackerFont->SetColor(vecColor);

			if (obj.Type == kHiddenPinkWard) // CURRENT DRAW FOR PINK
			{
				GRender->DrawOutlinedCircle(obj.WorldPosition, vecColor, 20.f); // draw a small circle around ward
				PinkWard->Draw(obj.MinimapPosition.x - 10, obj.MinimapPosition.y - 10);

				if (Menu.ShowVisionRange->Enabled()) // draw ward range
				{
					vecColor.w = (50);
					GRender->DrawOutlinedCircle(obj.WorldPosition, vecColor, 900.f); // draw a circle around ward range
				}
			}
			else if (obj.Type == kWardCorpse)
			{
				vecColor = Vec4(255, 0, 0, 255);
				GRender->DrawOutlinedCircle(obj.WorldPosition, vecColor, 100.f); // draw a small circle around ward
			}
			else
			{
				Vec3 vecPosition = obj.WorldPosition;
				Vec2 vecScreen;

				if (GGame->Projection(vecPosition, &vecScreen) && obj.Type != kHiddenBlueWard)
				{
					TrackerFont->Render(vecScreen.x, vecScreen.y, "%i", static_cast<int>(obj.DespawnTime - GGame->Time())); // draw ward or trap timer on minimap
				}
				
				if (obj.Type == kHiddenWard)
				{
					GRender->DrawOutlinedCircle(obj.WorldPosition, vecColor, 20.f); // draw a small circle around ward
					EnemyWard->Draw(obj.MinimapPosition.x - 5, obj.MinimapPosition.y - 5);
					if (Menu.ShowVisionRange->Enabled()) // draw ward range
					{
						vecColor.w = (50);
						GRender->DrawOutlinedCircle(obj.WorldPosition, vecColor, 900.f); // draw a circle around ward range
					}
				}
				else if (obj.Type == kHiddenBlueWard)
				{
					GRender->DrawOutlinedCircle(obj.WorldPosition, vecColor, 20.f); // draw a small circle around ward
					EnemyWard->Draw(obj.MinimapPosition.x - 5, obj.MinimapPosition.y - 5);
					if (Menu.ShowVisionRange->Enabled()) // draw ward range
					{
						vecColor.w = (50);
						GRender->DrawOutlinedCircle(obj.WorldPosition, vecColor, 500.f); // draw a circle around ward range
					}
				}
				else if (obj.Type == kHiddenTrap)
				{
					GRender->DrawOutlinedCircle(obj.WorldPosition, vecColor, 125.f); // draw a small circle around ward
					TrapDot->Draw(obj.MinimapPosition.x - 2, obj.MinimapPosition.y - 2);
					if (Menu.ShowVisionRange->Enabled()) // draw ward range
					{
						vecColor.w = (50);
						GRender->DrawOutlinedCircle(obj.WorldPosition, vecColor, 450.f); // draw a circle around ward range
					}
				}
			}
			TrackerFont->SetColor(Vec4(255, 255, 255, 255));
		}
	}

	for (auto& camp : Camps)
	{
		if (!camp.IsDead)
			continue;

		if (camp.NextSpawnTime - GGame->Time() <= 0)
		{
			camp.IsDead		= false;
			camp.Notified	= false;
			continue;
		}

		if (!Menu.Jungle.TrackJungleTimers->Enabled())
			continue;

		if (!Menu.Jungle.TrackGromp->Enabled() && strstr(camp.Name.c_str(), "Gromp"))
			continue;
		if (!Menu.Jungle.TrackBlue->Enabled() && strstr(camp.Name.c_str(), "Blue"))
			continue;
		if (!Menu.Jungle.TrackWolves->Enabled() && strstr(camp.Name.c_str(), "Wolves"))
			continue;
		if (!Menu.Jungle.TrackRaptors->Enabled() && strstr(camp.Name.c_str(), "Chicken"))
			continue;
		if (!Menu.Jungle.TrackRed->Enabled() && strstr(camp.Name.c_str(), "Red"))
			continue;
		if (!Menu.Jungle.TrackKrugs->Enabled() && strstr(camp.Name.c_str(), "Krug"))
			continue;
		if (!Menu.Jungle.TrackCrabs->Enabled() && strstr(camp.Name.c_str(), "Crab"))
			continue;
		if (!Menu.Jungle.TrackDrag->Enabled() && strstr(camp.Name.c_str(), "Dragon"))
			continue;
		/*if (Menu.Jungle.ShowNotifications->Enabled())
		{
			if (!camp.Notified && static_cast<int>(camp.NextSpawnTime - GGame->Time()) <= 5)
			{
				GRender->Notification(Vec4(255, 255, 255, 255), 5, "Camp %s is respawning in 5 seconds", camp.Name.c_str());
				camp.Notified = true;
			}
		}*/

		StrTimeFormat(static_cast<int>(camp.NextSpawnTime - GGame->Time()));
		char * cstr = new char[formattedTime.length() + 2];
		strcpy(cstr, formattedTime.c_str());

		// cstr now contains a c-string copy of formattedTime

		if (Menu.Jungle.Show3DTimer->Enabled())
		{
			Vec2 vecScreen;
			if (GGame->Projection(camp.Position, &vecScreen))
				TrackerFont->Render(vecScreen.x, vecScreen.y, cstr); //"%s: %s", camp.Name.c_str(),
		}

		if (Menu.Jungle.ShowMinimapTimer->Enabled())
		{
			Vec2 vecMinimap;
			if (GGame->WorldToMinimap(camp.Position, vecMinimap))
				TrackerFont->Render(vecMinimap.x, vecMinimap.y, cstr); // "%i", static_cast<int>(camp.NextSpawnTime - GGame->Time()));
		}
		delete[] cstr;
	}
}

void Tracker::OnCreateObject(IUnit* Args)
{
	if (Args->UnitFlags() != FL_CREEP && Args->UnitFlags() != FL_HERO)
		return;

	if (strstr(Args->GetObjectName(), "Minion"))
		return;

	if (!Args->IsEnemy(GEntityList->Player()))
		return;

	if (strstr(Args->GetObjectName(), "WardCorpse"))
		UpdateHiddenObjects(Args);

	// Check for wards
	if (Args->GetObjectName() != nullptr)
	{		
		// VisionWard = sightstone ward
		if (!_stricmp(Args->GetObjectName(), "visionward")) 
		{
			auto& i = std::find_if(HiddenObjects.begin(), HiddenObjects.end(), [&](HiddenObject& Object)
			{
				if (!Object.IsValid)
					return false;

				Vec3 vecPosition = Args->GetPosition();
				float flDistance = (vecPosition - Object.WorldPosition).Length();

				return (flDistance < 100.f);
			});

			if (i == HiddenObjects.end())
			{
				HiddenObject o;

				o.WorldPosition = Args->GetPosition();
				o.IsValid = true;
				o.Type = kHiddenWard;
				o.DespawnTime = GGame->Time() + Args->GetMana();
				
				GGame->WorldToMinimap(o.WorldPosition, o.MinimapPosition);
				HiddenObjects.push_back(o);
			}
		}
		// SightWard = trinket yellow/blue ward
		else if (!_stricmp(Args->GetObjectName(), "sightward"))
		{
			auto& i = std::find_if(HiddenObjects.begin(), HiddenObjects.end(), [&](HiddenObject& Object)
			{
				if (!Object.IsValid)
					return false;

				Vec3 vecPosition = Args->GetPosition();
				float flDistance = (vecPosition - Object.WorldPosition).Length();

				return (flDistance < 100.f);
			});

			if (i == HiddenObjects.end())
			{
				HiddenObject o;

				o.WorldPosition = Args->GetPosition();
				o.IsValid = true;

				if (Args->GetMana() == 0.f)
				{
					o.Type = kHiddenBlueWard;
					o.DespawnTime = FLT_MAX;
				}
				else
				{
					o.Type = kHiddenWard;
					if (strstr(Args->GetObjectName(), "SightWard"))
					{
						o.DespawnTime = GGame->Time() + ((GEntityList->Player()->GetLevel() - 1) * 3.5) + 60;
					}
					else
					{
						o.DespawnTime = GGame->Time() + Args->GetMana();
					}
				}

				GGame->WorldToMinimap(o.WorldPosition, o.MinimapPosition);

				//GRender->Notification(Vec4(255, 255, 0, 255), 10, "Tracking Object %s", Args->GetObjectName());
				HiddenObjects.push_back(o);
			}
		}
		// JammerDevice = REAL vision ward
		else if (!_stricmp(Args->GetObjectName(), "JammerDevice"))
		{
			auto& i = std::find_if(HiddenObjects.begin(), HiddenObjects.end(), [&](HiddenObject& Object)
			{
				if (!Object.IsValid)
					return false;

				Vec3 vecPosition = Args->GetPosition();
				float flDistance = (vecPosition - Object.WorldPosition).Length();

				return (flDistance < 100.f);
			});

			if (i == HiddenObjects.end())
			{
				HiddenObject o;

				o.WorldPosition = Args->GetPosition();
				o.IsValid = true;
				o.Type = kHiddenPinkWard;
				o.DespawnTime = FLT_MAX;
				
				GGame->WorldToMinimap(o.WorldPosition, o.MinimapPosition);
				HiddenObjects.push_back(o);
			}
		}
		// Noxious Trap = teemo shroom
		else if (!_stricmp(Args->GetObjectName(), "Noxious Trap"))
		{
			auto& i = std::find_if(HiddenObjects.begin(), HiddenObjects.end(), [&](HiddenObject& Object)
			{
				if (!Object.IsValid)
					return false;

				Vec3 vecPosition = Args->GetPosition();
				float flDistance = (vecPosition - Object.WorldPosition).Length();

				return (flDistance < 100.f);
			});

			if (i == HiddenObjects.end())
			{
				HiddenObject o;

				o.WorldPosition = Args->GetPosition();
				o.IsValid = true;
				o.Type = kHiddenTrap;
				o.DespawnTime = GGame->Time() + 300;

				GGame->WorldToMinimap(o.WorldPosition, o.MinimapPosition);
				HiddenObjects.push_back(o);
			}
		}
		else
		{
			//GRender->Notification(Vec4(255, 255, 0, 255), 10, "Unidentified Object Created: %s", Args->GetObjectName());
		}
	}

	if (!Args->IsJungleCreep())
		return;

	// Check for jungle camps
	for (auto& camp : Camps)
	{
		if (!camp.IsTracking)
			continue;

		auto& i = std::find_if(camp.Mobs.begin(), camp.Mobs.end(), [&](JungleMob Mob) { return StrStrIA(Mob.Name.c_str(), Args->GetObjectName()) != nullptr; });

		if (i != camp.Mobs.end())
		{
			(*i).IsDead = false;
			camp.IsDead = false;
		}
	}
}

void Tracker::OnDestroyObject(IUnit* Args)
{
	
}

void Tracker::OnUnitDeath(IUnit* Args)
{
	int iFlags = Args->UnitFlags();

	if (iFlags != FL_CREEP && iFlags != FL_HERO)
		return;

	if (strstr(Args->GetObjectName(), "Minion"))
		return;
	if (!Args->IsEnemy(GEntityList->Player()))
		return;

	// Invalidate all destroyed wards
	if (strstr(Args->GetObjectName(), "Noxious"))
		UpdateHiddenObjects(Args);

	if (!Args->IsJungleCreep())
		return;

	// Update jungle timers that change throughout the game
	UpdateDragonAndBaron();

	// Update jungle camps
	for (auto& camp : Camps)
	{
		if (!camp.IsTracking)
			continue;

		auto& i = std::find_if(camp.Mobs.begin(), camp.Mobs.end(), [&](JungleMob Mob) { return StrStrIA(Args->GetObjectName(), Mob.Name.c_str()) != nullptr; });

		if (i != camp.Mobs.end())
		{
			(*i).IsDead = true;
			camp.IsDead = IsCampDead(camp);

			if (camp.IsDead)
				camp.NextSpawnTime = GGame->Time() + camp.CooldownTime;
		}
	}
}

void Tracker::UpdateHiddenObjects(IUnit* Args)
{
	if (Args->GetHealth() > 100.f)
		return;

	//if (!_stricmp(Args->GetObjectName(), "wardcorpse"))
		//return;

	for (auto& Object : HiddenObjects)
	{
		Vec3 vecPosition = Args->GetPosition();
		float flDistance = (vecPosition - Object.WorldPosition).Length();

		//GRender->Notification(Vec4(255, 255, 0, 255), 10, "flDistance: %f", flDistance);

		if (flDistance < 20)
			Object.IsValid = false;
		/*
		if (vecPosition == Object.WorldPosition)
			Object.IsValid = false;
		else if (Object.Type == kHiddenTrap && flDistance < 100)
			Object.IsValid = false;
		else if (flDistance < 400)
		{
			if (Object.Type == kHiddenPinkWard && !_stricmp(Args->GetObjectName(), "jammerdevice"))
				Object.IsValid = false;
			else if ((Object.Type == kHiddenMissile || Object.Type == kHiddenWard) && !_stricmp(Args->GetObjectName(), "sightward"))
				Object.IsValid = false;

			
		}*/
	}
}

void Tracker::UpdateDragonAndBaron()
{
	static bool baron_updated = false;
	static bool dragon_updated = false;

	// Handle Baron changes after 20 minutes
	if (!baron_updated)
	{
		if (GGame->Time() > 1200.f)
		{
			Camps[RiftIdx].IsTracking	= false;
			Camps[BaronIdx].IsTracking	= true;
			baron_updated				= true;
		}
	}

	// Update Dragon respawn timer after 35 minutes
	if (!dragon_updated && GGame->Time() > 2100.f)
	{
		Camps[DragonIdx].CooldownTime	= 600.f;
		dragon_updated					= true;
	}
}

void Tracker::LoadMenu(IMenu* Parent)
{
	Menu.Enabled = Menu.Owner->CheckBox("Enabled:", true);
	Menu.TrackWards = Menu.Owner->CheckBox("Draw Wards:", true);
	Menu.ShowVisionRange = Menu.Owner->CheckBox("Draw Vision Indicators:", true);

	IMenu* pJungleMenu = Parent->AddMenu("Jungle");

	Menu.Jungle.TrackJungleTimers = pJungleMenu->CheckBox("Enable Timers:", true);
	Menu.Jungle.Show3DTimer = pJungleMenu->CheckBox("Draw Timer on Camps:", true);
	Menu.Jungle.ShowMinimapTimer = pJungleMenu->CheckBox("Draw Minimap Timers:", false);	Menu.Jungle.TrackGromp = pJungleMenu->CheckBox("Track Gromp:", true);
	Menu.Jungle.TrackBlue = pJungleMenu->CheckBox("Track Blue Buff:", true);
	Menu.Jungle.TrackWolves = pJungleMenu->CheckBox("Track Wolves:", true);
	Menu.Jungle.TrackRaptors = pJungleMenu->CheckBox("Track Raptors:", true);
	Menu.Jungle.TrackRed = pJungleMenu->CheckBox("Track Red Buff:", true);
	Menu.Jungle.TrackKrugs = pJungleMenu->CheckBox("Track Krugs:", true);
	Menu.Jungle.TrackCrabs = pJungleMenu->CheckBox("Track Scuttle Crabs:", true);
	Menu.Jungle.TrackDrag = pJungleMenu->CheckBox("Track Dragon:", false);

	IMenu* pFoWJungleTrackerMenu = Parent->AddMenu("FoW Jungle Tracker");
	Menu.Jungle.TrackJungler = pFoWJungleTrackerMenu->CheckBox("Enabled:", true);
	Menu.Jungle.DrawJunglerTrackerPingLocal = pFoWJungleTrackerMenu->CheckBox("Ping Local (only you see):", true);
	Menu.Jungle.DrawJunglerTrackerPingGlobal = pFoWJungleTrackerMenu->CheckBox("Ping Global (everyone sees):", false);
	//Menu.Jungle.DrawJunglerTrackerPingType = pFoWJungleTrackerMenu->AddInteger("Ping Type:", 1, 3, 1);
	//Menu.Jungle.ShowNotifications = pJungleMenu->CheckBox("Show Notifications", true);
}

bool Tracker::AddHiddenObject(CastedSpell const& Args)
{
	if (Args.Target_ != nullptr || Args.Caster_ == nullptr)
		return false;


	if (!Args.Caster_->IsEnemy(GEntityList->Player()))
		return false;

	struct HiddenObjectDefinition
	{
		std::string SpellName;
		float ActiveTime;
		eHiddenObjectType Type;
	};

	//GRender->Notification(Vec4(255, 255, 0, 255), 10, "Spell name: %s", Args.Name_);

	static std::vector<HiddenObjectDefinition> Definitions =
	{
		{"trinketorblvl3",FLT_MAX, kHiddenBlueWard},
		{"jammerdevice",FLT_MAX, kHiddenPinkWard },
		{"itemghostward", 150.f, kHiddenWard},
		{"wrigglelantern", 180.f, kHiddenWard},
		{"sightward", 180.f, kHiddenWard},
		{"itemferalflare", 180.f, kHiddenWard},
		{"trinkettotemlvl1", 60.f, kHiddenWard},
		{"trinkettotemlvl2", 120.f, kHiddenWard},
		{"trinkettotemlvl3", 180.f, kHiddenWard},
		{"teemorcast", 300.f, kHiddenTrap},
		{"noxious trap", 300.f, kHiddenTrap},
		{"JackInTheBox", 100.f, kHiddenTrap},
		{"Jack In The Box", 100.f, kHiddenTrap}
	};

	HiddenObject object;
	memset(&object, 0, sizeof(object));

	for (auto obj : Definitions)
	{
		if (StrStrIA(Args.Name_, obj.SpellName.c_str()) != nullptr)
		{
			object.WorldPosition	= Args.EndPosition_;
			object.Type				= obj.Type;
			object.DespawnTime		= obj.ActiveTime;
			object.IsValid			= true;
			
			if (object.DespawnTime != FLT_MAX)
			{
				object.DespawnTime += GGame->Time();
				if (strstr(Args.Name_, "TrinketTotemLvl1"))
				{
					object.DespawnTime += ((3.5f * (Args.Caster_->GetLevel() - 1)) + 1);
				}
			}

			GGame->WorldToMinimap(object.WorldPosition, object.MinimapPosition);
			HiddenObjects.push_back(object);
		}
	}

	return (object.DespawnTime != 0.f);
}

bool Tracker::IsCampDead(JungleCamp const& Camp)
{
	for (auto mob : Camp.Mobs)
	{
		if (!mob.IsDead)
			return false;
	}

	return true;
}

void Tracker::LoadJungleCamps()
{
	/* Team Order */
	Camps.emplace_back(97.f, 300.f, Vec3(3800.99f, 52.18f, 7883.53f), "Blue", std::vector<std::string>({ "SRU_Blue1.1.1" })); //"SRU_BlueMini1.1.2", "SRU_BlueMini21.1.3" }));
	Camps.emplace_back(97.f, 150.f, Vec3(3849.95f, 52.46f, 6504.36f), "Wolves", std::vector<std::string>({ "SRU_Murkwolf2.1.1", "SRU_MurkwolfMini2.1.2", "SRU_MurkwolfMini2.1.3" }));
	Camps.emplace_back(97.f, 150.f, Vec3(6943.41f, 52.62f, 5422.61f), "Chicken", std::vector<std::string>({ "SRU_Razorbeak3.1.1", "SRU_RazorbeakMini3.1.2", "SRU_RazorbeakMini3.1.3", "SRU_RazorbeakMini3.1.4" }));
	Camps.emplace_back(97.f, 300.f, Vec3(7813.07f, 53.81f, 4051.33f), "Red", std::vector<std::string>({ "SRU_Red4.1.1" }));// , "SRU_RedMini4.1.2", "SRU_RedMini4.1.3"}));
	Camps.emplace_back(97.f, 150.f, Vec3(8370.58f, 51.09f, 2718.15f), "Krug", std::vector<std::string>({ "SRU_Krug5.1.1", "SRU_KrugMini5.1.2" }));
	Camps.emplace_back(107.f, 150.f, Vec3(2164.34f, 51.78f, 8383.02f), "Gromp", std::vector<std::string>({ "SRU_Gromp13.1.1" }));

	/* Team Chaos */
	Camps.emplace_back(97.f, 300.f, Vec3(10984.11f, 51.72f, 6960.31f), "Blue", std::vector<std::string>({ "SRU_Blue7.1.1" })); //"SRU_BlueMini7.1.2", "SRU_BlueMini27.1.3" }));
	Camps.emplace_back(97.f, 150.f, Vec3(10983.83f, 62.22f, 8328.73f), "Wolves", std::vector<std::string>({ "SRU_Murkwolf8.1.1", "SRU_MurkwolfMini8.1.2", "SRU_MurkwolfMini8.1.3" }));
	Camps.emplace_back(97.f, 150.f, Vec3(7852.38f, 52.30f, 9562.62f), "Chicken", std::vector<std::string>({ "SRU_Razorbeak9.1.1", "SRU_RazorbeakMini9.1.2", "SRU_RazorbeakMini9.1.3", "SRU_RazorbeakMini9.1.4" }));
	Camps.emplace_back(97.f, 300.f, Vec3(7139.29f, 56.38f, 10779.34f), "Red", std::vector<std::string>({ "SRU_Red10.1.1" })); //"SRU_RedMini10.1.2", "SRU_RedMini10.1.3" }));
	Camps.emplace_back(97.f, 150.f, Vec3(6476.17f, 56.48f, 12142.51f), "Krug", std::vector<std::string>({ "SRU_Krug11.1.1", "SRU_KrugMini11.1.2" }));
	Camps.emplace_back(107.f, 150.f, Vec3(12671.83f, 51.71f, 6306.60f), "Gromp", std::vector<std::string>({ "SRU_Gromp14.1.1" }));
	
	/* Neutral Bosses */
	Camps.emplace_back(150.f, 360.f, Vec3(9813.83f, -71.24f, 4360.19f), "Dragon", std::vector<std::string>({ "SRU_Dragon" }));
	Camps.emplace_back(1200.f, 420.f, Vec3(4993.14f, -71.24f, 10491.92f), "Baron", std::vector<std::string>({ "SRU_Baron12.1.1" }));
	Camps.emplace_back(240.f, 300.f, Vec3(4993.14f, -71.24f, 10491.92f), "Rift Herald", std::vector<std::string>({ "SRU_RiftHerald17.1.1" }));

	/* Neutral Wanderers */
	Camps.emplace_back(150.f, 180.f, Vec3(10647.70f, -62.81f, 5144.68f), "DCrab", std::vector<std::string>({ "SRU_Crab15.1.1" }));
	Camps.emplace_back(150.f, 180.f, Vec3(4285.04f, -67.60f, 9597.52f), "BCrab", std::vector<std::string>({ "SRU_Crab16.1.1" }));
	Camps.emplace_back(315.f, 385.f, Vec3(10586.f, -67.5, -67.5), "Health Plant", std::vector <std::string>({ "PlantHealth" }));
	Camps.emplace_back(315.f, 385.f, Vec3(4156.f, -66.f, -66.f), "Health Plant", std::vector<std::string>({ "PlantHealthMirrored" }));

	for (size_t i = 0; i < Camps.size(); i++)
	{
		auto camp = Camps.at(i);

		if (camp.Mobs.front().Name.find("Herald") != camp.Mobs.front().Name.npos)
			RiftIdx = i;

		if (camp.Mobs.front().Name.find("Dragon") != camp.Mobs.front().Name.npos)
			DragonIdx = i;

		if (camp.Mobs.front().Name.find("Baron") != camp.Mobs.front().Name.npos)
			BaronIdx = i;
	}

	Camps[RiftIdx].IsTracking	= false;
	Camps[BaronIdx].IsTracking	= false;
}