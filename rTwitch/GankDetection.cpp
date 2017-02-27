#include "GankDetection.h"

GankDetection::GankDetection(IMenu* Parent)
{
	int i = 0;

	for (auto pUnit : GEntityList->GetAllHeros(false, true))
	{
		Heros.emplace_back(pUnit);

		std::string const& Filename = pUnit->ChampionName();
		ChampIcons[i] = GRender->CreateTextureFromFile(("UtilityPRO/" + Filename + ".png").c_str());
		ChampIcons[i]->SetColor(Vec4(100, 100, 100, 255));
		i++;
	}
		

	Menu.Parent							= Parent->AddMenu("Awareness");
	Menu.ShowPredictedMovementCircle	= Menu.Parent->CheckBox("FoW Movement Prediction:", true);
	Menu.DrawJunglerTracker = Menu.Parent->CheckBox("Track Enemy Jungler:", true);
	//Menu.DrawEnemyRadar					= Menu.Parent->CheckBox("Show Radar", false);
	Menu.ShowClicks						= Menu.Parent->CheckBox("Draw Enemy Clicks:", false);
	Menu.RenderGankDetectionCircle		= Menu.Parent->CheckBox("Draw Gank Detection Radius:", false);
	Menu.GankPingDistance				= Menu.Parent->AddFloat("Gank Detection Radius", 500, 5000, 3000);
	//Menu.GankWaitTime					= Menu.Parent->AddFloat("Gank Ping Wait Time (s)", 5, 15, 5);
	Menu.EnemyTowerRangeEnabled			= Menu.Parent->CheckBox("Draw Enemy Tower Ranges:", true);
	Menu.FriendlyTowerRangeEnabled		= Menu.Parent->CheckBox("Draw Friendly Tower Ranges:", true);

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

			float flTimeSinceVisible	= GGame->Time() - unit.LastHiddenTime;
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
}

void GankDetection::UpdateChampions()
{
	IUnit* pLocal = GEntityList->Player();
	Vec3 vecLocalPosition = pLocal->GetPosition();

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
			unit.LastVisibleTime = GGame->Time();
	}
}

void GankDetection::OnRender()
{
	IUnit* pLocal		= GEntityList->Player();
	Vec3 vecPosition	= pLocal->GetPosition();
	float flExtension	= 500.f;
	int i = -1;

	Vec2 vecScreen;

	
	
	if (Menu.EnemyTowerRangeEnabled->Enabled() || Menu.FriendlyTowerRangeEnabled->Enabled())
	{
		for (auto turret : GEntityList->GetAllTurrets(Menu.FriendlyTowerRangeEnabled->Enabled(), Menu.EnemyTowerRangeEnabled->Enabled()))
		{
			if (turret == nullptr || turret->IsDead() || turret->GetHealth() < 1.f || strstr(turret->GetObjectName(), "Shrine"))
				continue;

			float range = turret->BoundingRadius() + 775.f; //trstr(turret->GetObjectName(), "Chaos") ? turret->BoundingRadius() + 1100.f : 

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
			continue;

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
				if (GGame->Time() - unit.LastVisibleTime < 10.f)
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
							
							/*if (unit.IsJungle)
								Tahoma13Bold->Render(vecMinimap.x, vecMinimap.y, "[%i]", static_cast<int>(GGame->Time() - unit.LastVisibleTime));
							else
								Tahoma13->Render(vecMinimap.x, vecMinimap.y, "[%i]", static_cast<int>(GGame->Time() - unit.LastVisibleTime));*/

							GRender->DrawOutlinedCircle(vecMinimap, Vec4(255, 255, 0, 255), flRadius);
							ChampIcons[i]->SetColor(Vec4(100, 100, 100, 255));
							ChampIcons[i]->DrawCircle(vecMinimap.x, vecMinimap.y, 13);
							
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

		GRender->DrawOutlinedCircle(vecMyPosition, Vec4(255, 0, 50, 255), (vecMyPositionEnd - vecMyPosition).Length());
	}

	if (GGame->Time() < ShowGankEndTime && IncomingGankUnit != nullptr)
	{
		Vec2 vecMyPosition;
		Vec2 vecEnemyPosition;

		if (GGame->Projection(pLocal->GetPosition(), &vecMyPosition))
		{
			GGame->Projection(IncomingGankUnit->GetPosition(), &vecEnemyPosition);
			//GRender->DrawLine(vecMyPosition, vecEnemyPosition, Vec4(255, 0, 0, 255));
			
			Vec3 vecLocal		= pLocal->GetPosition();
			Vec3 vecEnemy		= IncomingGankUnit->GetPosition();
			Vec3 vecDir			= (vecEnemy - vecLocal).VectorNormalize();
			float flDistance	= (vecEnemy - vecLocal).Length();
			Vec3 vecHalf		= vecLocal + vecDir * (flDistance / 2);

			Vec2 vecCenterLine;
			GGame->Projection(vecHalf, &vecCenterLine);

			//Tahoma13Bold->Render(vecCenterLine.x, vecCenterLine.y, IncomingGankUnit->ChampionName());
			
			ChampIcons[i]->SetColor(Vec4(255, 255, 255, 255));
			ChampIcons[i]->DrawCircle(vecCenterLine.x, vecCenterLine.y, 30);
		}
	}
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