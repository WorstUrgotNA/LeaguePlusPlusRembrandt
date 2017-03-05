#include "GankDetection.h"
#include <curl/curl.h>

ITexture*	RC_On;

GankDetection::GankDetection(IMenu* Parent)
{
	int i = 0;

	for (auto pUnit : GEntityList->GetAllHeros(false, true))
	{
		std::string const& Filename = pUnit->ChampionName();
		ChampIcons[i] = CreateTextureEx(Filename);
		ChampIcons[i]->SetColor(Vec4(100, 100, 100, 255));
		
		Heros.emplace_back(pUnit);
		Heros.back().ChampIcon = CreateTextureEx(Filename);
		i++;
	}
		// init jung fow trackr
	RC_On = CreateTextureEx("RC_On");
	RC_On->SetColor(Vec4(255, 150, 150, 150));
	RC_On->Scale(GRender->ScreenSize().y / 1440.f);

	JGNUpdated = false;
	FoWUpdated = false;
	JGDelay = 0.f;
	LastPingTimeTracker = 0.f;
	LastPingTime2 = 0.f;

	Menu.Parent							= Parent->AddMenu("Awareness");
	Menu.ShowPredictedMovementCircle	= Menu.Parent->CheckBox("Enable Minimap Awareness:", true);
	Menu.ShowMovementCircle = Menu.Parent->CheckBox("Minimap Predicted Movement Circle:", true);
	Menu.LastSeen = Menu.Parent->CheckBox("Minimap MIA Timer:", false);
	Menu.IconDuration = Menu.Parent->AddInteger("Minimap MIA Duration (sec):", 1, 200, 10);
	Menu.DrawJunglerTracker = Menu.Parent->CheckBox("Enemy Jungler MIA Box:", true);
	//Menu.DrawEnemyRadar					= Menu.Parent->CheckBox("Show Radar", false);
	Menu.ShowClicks						= Menu.Parent->CheckBox("Draw Enemy Clicks:", false);
	Menu.RenderGankDetectionCircle		= Menu.Parent->CheckBox("Draw Gank Detection Radius:", false);
	Menu.GankPingDistance				= Menu.Parent->AddFloat("Gank Detection Radius:", 500, 5000, 3000);
	Menu.DrawGankLine					= Menu.Parent->CheckBox("Draw Gank Detection Line:", true);
	Menu.DrawGankIcon					= Menu.Parent->CheckBox("Draw Ganker Icon:", true);
	//Menu.GankWaitTime					= Menu.Parent->AddFloat("Gank Ping Wait Time (s)", 5, 15, 5);
	Menu.EnemyTowerRangeEnabled			= Menu.Parent->CheckBox("Draw Enemy Tower Ranges:", true);
	Menu.FriendlyTowerRangeEnabled		= Menu.Parent->CheckBox("Draw Friendly Tower Ranges:", false);


	IMenu* pFoWJungleTrackerMenu = Parent->AddMenu("FoW Jungle Tracker");
	Menu.TrackJungler = pFoWJungleTrackerMenu->CheckBox("Enabled:", true);
	Menu.DrawJunglerTrackerPingLocal = pFoWJungleTrackerMenu->CheckBox("Ping Local (only you see):", true);
	//Menu.DrawJunglerTrackerPingGlobal = pFoWJungleTrackerMenu->CheckBox("Ping Global (everyone sees):", false);
	Menu.DrawJunglerTrackerPingType = pFoWJungleTrackerMenu->AddInteger("Ping Type:", 1, 3, 1);
	Menu.PingInterval = pFoWJungleTrackerMenu->AddInteger("Minimum Time Between Pings:", 0, 20, 5);

	//auto pHeroes = Menu.Parent->AddMenu("Ping on Gank (Champions)");

	for (auto pHero : GEntityList->GetAllHeros(false, true))
	{
		std::string szMenuName = std::string(pHero->ChampionName()) + ": Ping on Gank";

		if (pHero->GetSpellSlot("summonersmite") != kSlotUnknown)
			Menu.ChampionsToPingOnGank[pHero->GetNetworkId()] = Menu.Parent->CheckBox(szMenuName.c_str(), true);
		else
			Menu.ChampionsToPingOnGank[pHero->GetNetworkId()] = Menu.Parent->CheckBox(szMenuName.c_str(), false);
	}

	Font1 = GRender->CreateFont("Tahoma", 13.f, kFontWeightNormal);
	Font1->SetOutline(false);
	Font1->SetLocationFlags(kFontLocationCenterHorizontal);
	Tahoma13		= GRender->CreateFont("Tahoma", 13.f, kFontWeightNormal);
	Tahoma13Bold	= GRender->CreateFont("Tahoma", 13.f, kFontWeightBold);

	Tahoma13->SetLocationFlags(kFontLocationCenterHorizontal);
	Tahoma13Bold->SetLocationFlags(kFontLocationCenterHorizontal);

	Tahoma13->SetOutline(true);
	Tahoma13Bold->SetOutline(true);

	IncomingGankUnit	= nullptr;
	ShowGankEndTime		= 0.f;
	LastPingTime		= 0.f;
}

GankDetection::~GankDetection()
{

}

void GankDetection::OnJungleNotify(JungleNotifyData* Args)
{
	FoWUpdated = true;
	JGDisplayPos = Args->Position;
	JGDelay = GGame->Time() + 5;
}

void GankDetection::OnGameUpdate()
{
	UpdateChampions();

	// Ping on ganks
	if (GGame->Time() - LastPingTime >= 5)
	{
		for (auto unit : Heros)
		{
			// Only process visible and living players
			if (!unit.IsVisible || unit.Player->IsDead())
				continue;

			// Make sure they've been in the FoW at least 5 seconds
			if (unit.TotalTimeHidden < 5.f)
				continue;

			// Make sure this champion is setup for ping notifications
			if (!Menu.ChampionsToPingOnGank[unit.Player->GetNetworkId()]->Enabled())
				continue;

			float flTimeSinceVisible	= GGame->Time() - unit.LastHiddenTimeGank;
			float flDistance			= (GEntityList->Player()->GetPosition() - unit.Position).Length2D();

			if (flTimeSinceVisible > 0.1f && flTimeSinceVisible < 2.f && flDistance < Menu.GankPingDistance->GetFloat())
			{
				GGame->ShowPing(kPingFallback, GEntityList->Player(), true);

				LastPingTime		= GGame->Time();
				IncomingGankUnit	= unit.Player;
				ShowGankEndTime		= LastPingTime + 5;

				break;
			}
		}
	}

	//clear junglenotifications vector if too much time has passed
	if (GGame->Time() - LastPingTimeTracker > 5)
		JungleNotifications.clear();


	if (FoWUpdated) //try to push notification position and time to vector
	{
		if (Menu.TrackJungler->Enabled())
		{
			bool FalseAlarm = false;

			for (auto pUnit : GEntityList->GetAllUnits())
			{
				float flDistance = (pUnit->GetPosition() - JGDisplayPos).Length();
				if (flDistance < 500 || !GGame->WithinFogOfWar(JGDisplayPos))
				{
					//GRender->Notification(Vec4(255, 255, 255, 255), 5, "%s distance %f to ping", pUnit->GetObjectName(), flDistance);
					FalseAlarm = true;
					break;
				}
			}

			if (!FalseAlarm && GGame->Time() - LastPingTimeTracker >= 1)
			{
				JungleNotification o;

				GGame->WorldToMinimap(JGDisplayPos, o.MinimapPosition);
				o.WorldPosition = JGDisplayPos;
				o.TriggerTime = JGDelay;

				JungleNotifications.push_back(o);
				JGNUpdated = true;
				//GRender->Notification(Vec4(255, 255, 255, 255), 5, "Size: %i", JungleNotifications.size());
				LastPingTimeTracker = GGame->Time();

				if (GGame->Time() - LastPingTime2 >= Menu.PingInterval->GetInteger())
				{
					if (Menu.DrawJunglerTrackerPingLocal->Enabled())
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

void GankDetection::UpdateChampions()
{
	IUnit* pLocal = GEntityList->Player();
	Vec3 vecLocalPosition = pLocal->GetPosition();
	int NumVisible = 0;

	for (auto& unit : Heros)
	{
		if (!unit.IsVisible && unit.Player->IsVisible())
		{
			unit.LastHiddenTimeGank = GGame->Time();
		}

		if (unit.Player->IsVisible() || unit.Player->IsDead())
		{
			unit.Position = unit.Player->GetPosition();
			unit.LastVisibleTime = GGame->Time();
			NumVisible++;
		}
		else
		{
			unit.TotalTimeHidden = GGame->Time() - unit.LastVisibleTime;
		}
		if (GGame->Time() - unit.LastHiddenTime > 10)
			unit.Position = unit.Player->GetPosition();

		unit.Distance = (unit.Position - vecLocalPosition).Length();
		unit.IsVisible = unit.Player->IsVisible();

	}

	if (NumVisible == 4 && JGNUpdated)
	{
		for (auto& unit : Heros)
		{
			if (!unit.IsVisible)
			{
				unit.LastHiddenTime = GGame->Time();
				unit.LastVisibleTime = GGame->Time();
				unit.Position = JGDisplayPos;
				JGNUpdated = false;
				break;
			}
		}
	}
	/*
	for (auto& unit : Heros)
	{
		if (!unit.IsVisible && unit.Player->IsVisible())
		{
			unit.TotalTimeHidden = GGame->Time() - unit.LastVisibleTime;
			unit.LastHiddenTime = GGame->Time();
		}
		unit.Position = unit.Player->GetPosition();
		unit.Distance = (unit.Position - vecLocalPosition).Length();
		unit.IsVisible = unit.Player->IsVisible();

		if (unit.IsVisible)
		{
			unit.LastVisibleTime = GGame->Time();
		}
			
	}*/
}

void GankDetection::OnRender()
{
	IUnit* pLocal		= GEntityList->Player();
	Vec3 vecPosition	= pLocal->GetPosition();
	float flExtension	= 500.f;
	int i = -1;

	Vec2 vecScreen;

	//Draw FoW Jungle Tracker
	if (Menu.TrackJungler->Enabled())
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
	
	
	if (Menu.EnemyTowerRangeEnabled->Enabled() || Menu.FriendlyTowerRangeEnabled->Enabled())
	{
		for (auto turret : GEntityList->GetAllTurrets(Menu.FriendlyTowerRangeEnabled->Enabled(), Menu.EnemyTowerRangeEnabled->Enabled()))
		{
			if (turret == nullptr || turret->IsDead() || turret->GetHealth() < 1.f || strstr(turret->GetObjectName(), "Shrine"))
				continue;

			float range = turret->BoundingRadius() + 775.f; //strstr(turret->GetObjectName(), "Chaos") ? turret->BoundingRadius() + 1100.f : 

			if (turret->IsEnemy(pLocal))
				GRender->DrawOutlinedCircle(turret->GetPosition(), Vec4(255, 0, 0, 140), range);
			else
				GRender->DrawOutlinedCircle(turret->GetPosition(), Vec4(0, 255, 0, 140), range);
		}
	}

	for (auto unit : Heros)
	{
		i++;

		Vec2 vecScreen;
		Vec3 vecExtendedPosition = vecPosition + (unit.Position - vecPosition).VectorNormalize() * flExtension;


		if (!GGame->Projection(vecExtendedPosition, &vecScreen))
		{
			//continue;
		}
			

		vecScreen.y += 15.f;

		if (unit.Player->IsDead())
		{
			if (unit.IsJungle && Menu.DrawJunglerTracker->Enabled() && GGame->Projection(vecPosition, &vecScreen))
			{
				GRender->DrawFilledBox(Vec2(vecScreen.x, vecScreen.y + 125), Vec2(120.f, 15.f), Vec4(0, 117, 0, 255));
				GRender->DrawOutinedBox(Vec2(vecScreen.x, vecScreen.y + 125), Vec2(120.f, 15.f), 1, Vec4(0, 0, 0, 255));
				Font1->Render(vecScreen.x + 60, vecScreen.y + 127, "Jungler Dead");
			}
			continue;
		}
			

		

		

		//Vec4 vecColor;
		//GetHealthColor(&unit, vecColor);

		//Vec2 vecHealthBarSize(50.f, 8.f);

		// Draw Health Bar
		/*if (Menu.DrawEnemyRadar->Enabled())
		{
			GRender->DrawFilledBox(vecScreen, vecHealthBarSize, Vec4(255, 255, 255, 255));
			GRender->DrawFilledBox(vecScreen, Vec2(vecHealthBarSize.x * (unit.Player->HealthPercent() / 100.f), vecHealthBarSize.y), vecColor);
			GRender->DrawOutinedBox(vecScreen, vecHealthBarSize, 1.f, Vec4(0, 0, 0, 255));
		}*/

		if (unit.IsVisible)
		{
			//GRender->Notification(Vec4(255, 255, 255, 255), 0, "%s is visible", unit.Player->ChampionName());
			// Draw Path
			if (Menu.ShowClicks->Enabled())
			{
				auto vecWaypointList = unit.Player->GetWaypointList();

				if (vecWaypointList.size() > 1)
				{
					Vec3 vecFinalClick = vecWaypointList.back();

					Vec2 vecWaypointScreen, vecPlayerScreen;
					if (GGame->Projection(vecFinalClick, &vecWaypointScreen) && GGame->Projection(unit.Position, &vecPlayerScreen))
						GRender->DrawLine(vecPlayerScreen, vecWaypointScreen, Vec4(255, 255, 255, 255));
				}
			}

			if (unit.IsJungle && Menu.DrawJunglerTracker->Enabled() && GGame->Projection(vecPosition, &vecScreen))
			{
				float flDistance = (GEntityList->Player()->GetPosition() - unit.Position).Length2D();
				if (flDistance <= Menu.GankPingDistance->GetFloat())
				{
					GRender->DrawFilledBox(Vec2(vecScreen.x, vecScreen.y + 125), Vec2(120.f, 15.f), Vec4(200, 0, 0, 255));
					GRender->DrawOutinedBox(Vec2(vecScreen.x, vecScreen.y + 125), Vec2(120.f, 15.f), 1, Vec4(0, 0, 0, 255));
					Font1->Render(vecScreen.x + 60, vecScreen.y + 127, "Jungler Near!");
				}
				else
				{
					GRender->DrawFilledBox(Vec2(vecScreen.x, vecScreen.y + 125), Vec2(120.f, 15.f), Vec4(0, 117, 0, 255));
					GRender->DrawOutinedBox(Vec2(vecScreen.x, vecScreen.y + 125), Vec2(120.f, 15.f), 1, Vec4(0, 0, 0, 255));
					Font1->Render(vecScreen.x + 60, vecScreen.y + 127, "Jungler Visible");
				}
				// 
			}
			// Draw Name
			/*if (Menu.DrawEnemyRadar->Enabled())
			{
				if (unit.IsJungle)
					Tahoma13Bold->Render(vecScreen.x + vecHealthBarSize.x / 2, vecScreen.y - 15.f, unit.Player->ChampionName());
				else
					Tahoma13->Render(vecScreen.x + vecHealthBarSize.x / 2, vecScreen.y - 15.f, unit.Player->ChampionName());
			}*/
		}
		else
		{
			//GRender->Notification(Vec4(255, 255, 255, 255), 0, "%s is NOT visible", unit.Player->ChampionName());
			// Draw Name [MIA]
			/*if (Menu.DrawEnemyRadar->Enabled())
			{
				if (unit.IsJungle)
					Tahoma13Bold->Render(vecScreen.x + vecHealthBarSize.x / 2, vecScreen.y - 15.f, "MIA %s %i", unit.Player->ChampionName(), static_cast<int>(GGame->Time() - unit.LastVisibleTime));
				else
					Tahoma13->Render(vecScreen.x + vecHealthBarSize.x / 2, vecScreen.y - 15.f, "MIA %s %i", unit.Player->ChampionName(), static_cast<int>(GGame->Time() - unit.LastVisibleTime));
			}*/

			if (unit.IsJungle && Menu.DrawJunglerTracker->Enabled() && GGame->Projection(vecPosition, &vecScreen))
			{
				GRender->DrawFilledBox(Vec2(vecScreen.x, vecScreen.y + 125), Vec2(120.f, 15.f), Vec4(255, 168, 0, 255));
				GRender->DrawOutinedBox(Vec2(vecScreen.x, vecScreen.y + 125), Vec2(120.f, 15.f), 1, Vec4(0, 0, 0, 255));
				Font1->Render(vecScreen.x + 60, vecScreen.y + 127, "Jungler MIA: %i", static_cast<int>(GGame->Time() - unit.LastVisibleTime));
			}
			
			// Draw FoW Circle
			
			if (Menu.ShowPredictedMovementCircle->Enabled())
			{
				if (GGame->Time() - unit.LastVisibleTime < Menu.IconDuration->GetInteger())
				{
					Vec2 vecMinimap;
					
					if (GGame->WorldToMinimap(unit.Position, vecMinimap))
					{
						float flMaxDistance = unit.Player->MovementSpeed() * (GGame->Time() - unit.LastVisibleTime);
						Vec3 vecNewPosition = unit.Position + (vecPosition - unit.Position).VectorNormalize() * flMaxDistance;

						Vec2 vecMinimapNew;
						if (GGame->WorldToMinimap(vecNewPosition, vecMinimapNew))
						{
							float flRadius = (vecMinimapNew - vecMinimap).Length();
							
							
							unit.ChampIcon->SetColor(Vec4(200, 200, 200, 255));
							unit.ChampIcon->DrawCircle(vecMinimap.x, vecMinimap.y, 12);
							
							/*if (Menu.ShowMovementCircle->Enabled())
								GRender->DrawOutlinedCircle(vecMinimap, Vec4(255, 255, 0, 255), flRadius + 22);
							else
								GRender->DrawOutlinedCircle(vecMinimap, Vec4(255, 255, 0, 255), 22);
							*/
							if (Menu.LastSeen->Enabled())
								Tahoma13->Render(vecMinimap.x + 2, vecMinimap.y + 4, "[%i]", static_cast<int>(GGame->Time() - unit.LastVisibleTime));
						}
					}
				}
			}
		}

		flExtension += 100.f;
	}
	
	if (Menu.RenderGankDetectionCircle->Enabled())
	{
		Vec2 vecMyPosition;
		GGame->WorldToMinimap(pLocal->GetPosition(), vecMyPosition);

		Vec2 vecMyPositionEnd;
		GGame->WorldToMinimap(pLocal->GetPosition() + Vec3(1.f, 0.f, 0.f) * Menu.GankPingDistance->GetFloat(), vecMyPositionEnd);

		//GRender->DrawCircle(pLocal->GetPosition(), (vecMyPositionEnd - vecMyPosition).Length(), Vec4(255, 0, 50, 255), 1.f, false, false);
		GRender->DrawOutlinedCircle(vecMyPosition, Vec4(255, 0, 50, 255), (vecMyPositionEnd - vecMyPosition).Length());
	}

	if (GGame->Time() < ShowGankEndTime && IncomingGankUnit != nullptr)
	{
		Vec2 vecMyPosition;
		Vec2 vecEnemyPosition;

		if (GGame->Projection(pLocal->GetPosition(), &vecMyPosition))
		{
			GGame->Projection(IncomingGankUnit->GetPosition(), &vecEnemyPosition);
			if (Menu.DrawGankLine->Enabled())
				GRender->DrawLine(vecMyPosition, vecEnemyPosition, Vec4(255, 0, 0, 255));
			
			Vec3 vecLocal		= pLocal->GetPosition();
			Vec3 vecEnemy		= IncomingGankUnit->GetPosition();
			Vec3 vecDir			= (vecEnemy - vecLocal).VectorNormalize();
			float flDistance	= (vecEnemy - vecLocal).Length();
			Vec3 vecHalf		= vecLocal + vecDir * (flDistance / 2);

			Vec2 vecCenterLine;
			GGame->Projection(vecHalf, &vecCenterLine);

			//Tahoma13Bold->Render(vecCenterLine.x, vecCenterLine.y, IncomingGankUnit->ChampionName());
			if (Menu.DrawGankIcon->Enabled())
			{
				for (auto hero : Heros)
				{
					if (hero.Player->ChampionName() == IncomingGankUnit->ChampionName())
					{
						hero.ChampIcon->DrawCircle(vecCenterLine.x, vecCenterLine.y, 30);
						break;
					}
				}
			}
		}
	}
}

bool GankDetection::DoesTextureExist(std::string const& Filename, std::string& FullPath)
{
	std::string szFinalPath;
	GPluginSDK->GetBaseDirectory(szFinalPath);

	szFinalPath += "\\Textures\\" + Filename + ".png";
	FullPath = szFinalPath;

	HANDLE hFile = CreateFileA(szFinalPath.c_str(), GENERIC_READ, FILE_SHARE_READ, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);

	if (hFile != INVALID_HANDLE_VALUE)
	{
		CloseHandle(hFile);
		return true;
	}

	return false;
}

ITexture* GankDetection::CreateTextureEx(std::string const& Filename)
{


	std::string szFullPath;
	if (DoesTextureExist(Filename, szFullPath))
		return GRender->CreateTextureFromFile((Filename + ".png").c_str());
	/*
	std::string szImage;
	if (GPluginSDK->ReadFileFromURL(DownloadUrl, szImage))
	return GRender->CreateTextureFromMemory((uint8_t*)szImage.data(), szImage.length(), Filename.c_str());*/
	//GUtility->LogConsole("Could not find %s.png", Filename);

	CURL *curl;
	FILE *fp;
	CURLcode res;
	auto url = "https://raw.githubusercontent.com/Harmenszoon/LeaguePlusPlusRembrandt/master/Resources/" + Filename + ".png";
	curl = curl_easy_init();
	if (curl)
	{
		fp = fopen((szFullPath).c_str(), "wb");
		curl_easy_setopt(curl, CURLOPT_URL, url);
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, NULL);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, fp);
		res = curl_easy_perform(curl);
		curl_easy_cleanup(curl);
		fclose(fp);
	}

	if (DoesTextureExist(Filename, szFullPath))
		return GRender->CreateTextureFromFile((Filename + ".png").c_str());

	return nullptr;
}

void GankDetection::GetHealthColor(GankHero* Hero, Vec4& Out)
{
	float flHealthPercent = Hero->Player->HealthPercent();

	if (flHealthPercent >= 0.f)
		Out = Vec4(255, 0, 0, 255);

	if (flHealthPercent >= 25.f)
		Out = Vec4(255, 165, 0, 255);

	if (flHealthPercent >= 50.f)
		Out = Vec4(255, 255, 0, 255);

	if (flHealthPercent >= 75.f)
		Out = Vec4(0, 255, 0, 255);
}