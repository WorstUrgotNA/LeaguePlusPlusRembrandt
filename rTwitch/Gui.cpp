#include <string>
#include "Gui.h"


int LocalVersion = 2;
float LogoDuration;

Gui::Gui(IMenu* Parent)
{
	Menu.Owner = Parent->AddMenu("HUD");

	ReSizeNeeded = true;

	GetLatestVersionNo();
	LoadFonts();
	LoadTextureIcons();
	LoadMenu();
	LogoDuration = GGame->Time() + 5;

}

Gui::~Gui()
{
}

void Gui::OnD3DPostReset()
{
		ReSizeNeeded = true;
}

void Gui::OnRender()
{
	GUtility->LogConsole("Begin Gui OnRender");
	Resolution = GRender->ScreenSize();
	GUtility->LogConsole("Resize");
	
	
	Textures->UHud->Scale(Resolution.y / 1440.f);
	Vec2 LogoSpot = Vec2(Resolution.x / 2 - Textures->UHud->GetSize().x / 2, Resolution.y / 2 - Textures->UHud->GetSize().y / 2);
		if (LogoDuration - GGame->Time() > 0)
			Textures->UHud->Draw(LogoSpot.x, LogoSpot.y);

	if (Menu.Enabled->Enabled())
	{
		UpdateChampions();
		GUtility->LogConsole("UpdateChampions");
		RenderTeammates();
		GUtility->LogConsole("UpdateTeammates");
		RenderEnemies();
		GUtility->LogConsole("UpdateEnemies");
	}
	ReSizeNeeded = false;
	GUtility->LogConsole("Gui OnRender Complete");
}

void Gui::OnCreateObject(IUnit* Args)
{
	if (Args->IsHero() && Args->GetMaxHealth() < 9000)
	{
		auto pszName = Args->ChampionName();

		for (auto pGui : UiPool)
		{
			if (!_stricmp(pszName, pGui->ChampionName))
			{
				pGui->Player = Args;
				pGui->Valid = true;
			}
		}
	}
}

void Gui::OnDestroyObject(IUnit* Args)
{
	for (auto pGui : UiPool)
	{
		if (pGui->Player == Args)
			pGui->Valid = false;
	}
}

void Gui::RenderPlayer2DElementNostalgic(HeroUI* Ui, Vec2 StartingTestPos)
{
	float ScreenRatio = Resolution.y / 1080.f;
	

	auto pOKTW_bar = Textures->OKTW_bar;
	auto pOKTW_circle = Textures->OKTW_circle;
	auto Sum1 = Ui->SummonerSpellIconsCircle[0];
	auto Sum2 = Ui->SummonerSpellIconsCircle[1];
	//resize
	pOKTW_bar->Scale(ScreenRatio);
	pOKTW_circle->Scale(ScreenRatio);
	Sum1->Scale(ScreenRatio);
	Sum2->Scale(ScreenRatio);

	Ui->ChampionIcon->DrawCircle(StartingTestPos.x + 30 * ScreenRatio, StartingTestPos.y + 34 * ScreenRatio, 29 * ScreenRatio);
	pOKTW_circle->Draw(StartingTestPos.x, StartingTestPos.y);
	pOKTW_bar->Draw(StartingTestPos.x, StartingTestPos.y);

	Sum1->Draw(StartingTestPos.x - 5 * ScreenRatio, StartingTestPos.y - 20 * ScreenRatio);
	Sum2->Draw(StartingTestPos.x + 25 * ScreenRatio, StartingTestPos.y - 20 * ScreenRatio);
	Ui->SpellIcons[kSlotR]->DrawCircle(StartingTestPos.x + 29 * ScreenRatio, StartingTestPos.y -28 * ScreenRatio, 15 * ScreenRatio);

	//MANA BAR
	if (Ui->Player->GetMaxMana() > 200 && !Ui->Player->IsDead())
	{
		try
		{
			GRender->DrawFilledBox(Vec2(StartingTestPos.x + 54 * ScreenRatio, StartingTestPos.y + 67 * ScreenRatio), Vec2(-(46.f * ((100.f - Ui->Player->ManaPercent()) / 100)) * ScreenRatio, 6 * ScreenRatio), Vec4(0, 0, 0, 200));
		}
		catch (const std::exception&)
		{
			GUtility->LogConsole("couldnt draw mana bar");
		}
	}
	else
	{
		GRender->DrawFilledBox(Vec2(StartingTestPos.x + 54 * ScreenRatio, StartingTestPos.y + 67 * ScreenRatio), Vec2(-(46.f * ScreenRatio), 6 * ScreenRatio), Vec4(0, 0, 0, 200));
	}

	//HEALTH BAR
	if (!Ui->Player->IsDead())
	{
		try
		{
			GRender->DrawFilledBox(Vec2(StartingTestPos.x + 54 * ScreenRatio, StartingTestPos.y + 57 * ScreenRatio), Vec2(-(46.f * ((100.f - Ui->Player->HealthPercent()) / 100)) * ScreenRatio, 8 * ScreenRatio), Vec4(0, 0, 0, 200));
		}
		catch (const std::exception&)
		{
			GUtility->LogConsole("couldnt draw mana bar");
		}
	}
	else
	{
		GRender->DrawFilledBox(Vec2(StartingTestPos.x + 54 * ScreenRatio, StartingTestPos.y + 57 * ScreenRatio), Vec2(-(46.f * ((100.f - Ui->Player->HealthPercent()) / 100)) * ScreenRatio, 8 * ScreenRatio), Vec4(0, 0, 0, 200));
	}
	
	//timers
	float flCooldown1 = Ui->Player->GetSpellRemainingCooldown(kSummonerSlot1);
	float flCooldown2 = Ui->Player->GetSpellRemainingCooldown(kSummonerSlot2);
	float flCooldown3 = Ui->Player->GetSpellLevel(kSlotR) > 0 ? Ui->Player->GetSpellRemainingCooldown(kSlotR) : 0.f;
	
	if (flCooldown1 > 0)
	{
		flCooldown1 += 1.f;
		Vec2 vecSpell1CD = StartingTestPos + Ui->SummonerSpellIconsCircle[0]->GetSize() / 2;
		Fonts->HudFont->Render(StartingTestPos.x + (Sum1->GetSize().x / 2 -5) * ScreenRatio, StartingTestPos.y + (Sum1->GetSize().y / 2 - 20) * ScreenRatio, "%i", static_cast<int>(flCooldown1));
	}
	
	if (flCooldown2 > 0)
	{
		flCooldown2 += 1.f;
		Vec2 vecSpell2CD = StartingTestPos + Ui->SummonerSpellIconsCircle[1]->GetSize() / 2;
		Fonts->HudFont->Render(StartingTestPos.x + (Sum2->GetSize().x / 2 + 25) * ScreenRatio, StartingTestPos.y + (Sum2->GetSize().y / 2 -20) * ScreenRatio, "%i", static_cast<int>(flCooldown2));
	}
	
	if (flCooldown3 > 0)
	{
		flCooldown3 += 1.f;
		Vec2 vecUltiCD = StartingTestPos;
		Fonts->HudFont->Render(StartingTestPos.x + 29 * ScreenRatio, StartingTestPos.y - 27 * ScreenRatio, "%i", static_cast<int>(flCooldown3));
	}
}

void Gui::RenderPlayer2DElementBig(HeroUI* Ui, Vec2 const& Position, bool Left)
{
	float ScreenRatio = Resolution.y / 1440.f;


	Vec2 vecHealthOffset	= Left ? Vec2(9, 79) : Vec2(17, 79);
	Vec2 vecManaOffset		= Left ? Vec2(9, 90) : Vec2(17, 90);
	Vec2 vecChampionOffset	= Left ? Vec2(43, 6) : Vec2(13, 6);
	Vec2 vecSpell1Offset	= Left ? Vec2(6, 6) : Vec2(82, 7);
	Vec2 vecSpell2Offset	= Left ? Vec2(6, 40) : Vec2(82, 40);
	Vec2 vecUltimateOffset	= Left ? Vec2(106, 20) : Vec2(18, 20);
	Vec2 vecChampionSize	= Ui->ChampionIcon->GetSize();
	Vec2 vecLevelPosition	= Position + vecChampionOffset;
	Vec2 HPScaledOffset = Vec2(Position.x - 150, Position.y);
	
	Vec2 LeftOffsetBase = Left ? Vec2(0, 0) : Vec2(150 * ScreenRatio, 0);
	GUtility->LogConsole("Init Variable Set 1");
	HPScaledOffset.x -= 150;

	auto pEmptyIcon		= Left ? Textures->EmptyLeft : Textures->EmptyRight;
	auto pOutlineIcon	= Left ? Textures->OutlineLeft : Textures->OutlineRight;

	auto pHUD1_bar = Left ? Textures->HUD1_barleft : Textures->HUD1_bar;
	auto pHUD1_champ = Left ? Textures->HUD1_champleft : Textures->HUD1_champ;
	auto pHUD1_icons = Left ? Textures->HUD1_iconsleft : Textures->HUD1_icons;
	auto pHUD1_hp = Textures->HUD1_hp;
	auto pHUD1_mp = Textures->HUD1_mp;
	auto Sum1 = Ui->SummonerSpellIconsCircle[0];
	auto Sum2 = Ui->SummonerSpellIconsCircle[1];
	GUtility->LogConsole("Init Textures");
	//RESIZE TO FIT SCREEN
	
	pHUD1_bar->Scale(ScreenRatio);
	pHUD1_champ->Scale(ScreenRatio);
	pHUD1_icons->Scale(ScreenRatio);
	pHUD1_hp->Scale(ScreenRatio);
	pHUD1_mp->Scale(ScreenRatio);
	Sum1->Scale(ScreenRatio);
	Sum2->Scale(ScreenRatio);
	
	GUtility->LogConsole("Resize Textures");
	if (Left)
		vecLevelPosition += vecChampionSize - Vec2(8, 5) - Vec2(26, 26);
	else
		vecLevelPosition += Vec2(0, vecChampionSize.y) + Vec2(3, -4) - Vec2(0, 26);

	//pHUD1_hp->Scale(Ui->Player->HealthPercent() / 100.f, 1.f);
	//pHUD1_mp->Scale(Ui->Player->ManaPercent() / 100.f, 1.f);

	if (!Left)
	{
		//Vec2 barSize = pHUD1_bar->Scale;
		//float ScaledSizeAroo = (2 * barSize.x + 2 * barSize.y) * ScreenRatio;
		
		GUtility->LogConsole("Drawing Right Side Bar");
		pHUD1_bar->Draw(Position.x - LeftOffsetBase.x, Position.y);
		{
			//draw hp bars
			GUtility->LogConsole("Drawing HP Bars");
			pHUD1_hp->Draw(Position.x - LeftOffsetBase.x, Position.y);
			GUtility->LogConsole("1");
			GRender->DrawFilledBox(Vec2(HPScaledOffset.x + 295, HPScaledOffset.y + 50 * ScreenRatio), Vec2(-(133.f * ((100.f - Ui->Player->HealthPercent()) / 100)) * ScreenRatio, 16.f * ScreenRatio), Vec4(0, 0, 0, 225));
			GUtility->LogConsole("2");
			if (Ui->Player->GetMaxMana() > 200 && !Ui->Player->IsDead())
			{
				pHUD1_mp->Draw(Position.x - LeftOffsetBase.x, Position.y);
				try
				{
					GRender->DrawFilledBox(Vec2(HPScaledOffset.x + 295, HPScaledOffset.y + 67 * ScreenRatio), Vec2(-(133.f * ((100.f - Ui->Player->ManaPercent()) / 100)) * ScreenRatio, 11 * ScreenRatio), Vec4(0, 0, 0, 200));
				}
				catch (const std::exception&)
				{
					GUtility->LogConsole("couldnt draw mana bar");
				}
			}
			//draw champ icon
			GUtility->LogConsole("Drawing Champ Icon");
			pHUD1_champ->Draw(Position.x - LeftOffsetBase.x, Position.y );
			Ui->ChampionIcon->DrawCircle(Position.x + 33 * ScreenRatio, Position.y + 63 * ScreenRatio, 37 * ScreenRatio);
			//draw icons
			GUtility->LogConsole("Drawing Summoner Icons");
			Sum1->Draw(Position.x + 62 * ScreenRatio, Position.y + 43 * ScreenRatio);
			Sum2->Draw(Position.x + 48 * ScreenRatio, Position.y + 77 * ScreenRatio);
			Ui->SpellIcons[kSlotR]->DrawCircle(Position.x + 68 * ScreenRatio, Position.y + 29 * ScreenRatio, 17 * ScreenRatio);
			pHUD1_icons->Draw(Position.x - LeftOffsetBase.x, Position.y);

			//HP BAR TEXT
			GUtility->LogConsole("Setting HP bar text");
			if (Menu.DrawHPBarText->Enabled())
			{
				switch (Menu.HPBarTextStyle->GetInteger())
				{
				case 1:
					Fonts->HudFont2->Render(Position.x + (-300 + 221) * ScreenRatio, HPScaledOffset.y + 54 * ScreenRatio, "%i ", static_cast<int>(Ui->Player->GetHealth()));
					break;
				case 2:
					Fonts->HudFont2->Render(Position.x  + (- 300 + 209) * ScreenRatio, HPScaledOffset.y + 54 * ScreenRatio, "%i/%i ", static_cast<int>(Ui->Player->GetHealth()), static_cast<int>(Ui->Player->GetMaxHealth()));
					break;
				case 3:
					Fonts->HudFont2->Render(Position.x + (-300 + 221) * ScreenRatio, HPScaledOffset.y + 54 * ScreenRatio, "%i%%", static_cast<int>(Ui->Player->HealthPercent()));
					break;
				}
			}
			//MP BAR TEXT
			GUtility->LogConsole("Setting MP Bar Text");
			if (Menu.DrawManaBarText->Enabled() && Ui->Player->GetMaxMana() > 200)
			{
				switch (Menu.ManaBarTextStyle->GetInteger())
				{
				case 1:
					Fonts->HudFont2->Render(Position.x + (-300 + 221) * ScreenRatio, HPScaledOffset.y + 68 * ScreenRatio, "%i ", static_cast<int>(Ui->Player->GetMana()));
					break;
				case 2:
					Fonts->HudFont2->Render(Position.x + (-300 + 209) * ScreenRatio, HPScaledOffset.y + 68 * ScreenRatio, "%i/%i ", static_cast<int>(Ui->Player->GetMana()), static_cast<int>(Ui->Player->GetMaxMana()));
					break;
				case 3:
					Fonts->HudFont2->Render(Position.x + (-300 + 221) * ScreenRatio, HPScaledOffset.y + 68 * ScreenRatio, "%i%%", static_cast<int>(Ui->Player->ManaPercent()));
					break;
				}
			}
			GUtility->LogConsole("Init Vars again");
			float flCooldown1 = Ui->Player->GetSpellRemainingCooldown(kSummonerSlot1);
			float flCooldown2 = Ui->Player->GetSpellRemainingCooldown(kSummonerSlot2);
			float flCooldown3 = Ui->Player->GetSpellLevel(kSlotR) > 0 ? Ui->Player->GetSpellRemainingCooldown(kSlotR) : 0.f;

			if (flCooldown1 > 0)
			{
				flCooldown1 += 1.f;
				Vec2 vecSpell1CD = Position + vecSpell1Offset + Ui->SummonerSpellIconsCircle[0]->GetSize() / 2;
				Fonts->HudFont->Render(Position.x + (Sum1->GetSize().x / 2 + 63) * ScreenRatio, Position.y + (Sum1->GetSize().y / 2 + 42) * ScreenRatio, "%i", static_cast<int>(flCooldown1));
			}

			if (flCooldown2 > 0)
			{
				flCooldown2 += 1.f;
				Vec2 vecSpell2CD = Position + vecSpell2Offset + Ui->SummonerSpellIconsCircle[1]->GetSize() / 2;
				Fonts->HudFont->Render(Position.x + (Sum2->GetSize().x / 2 + 49) * ScreenRatio , Position.y + (vecSpell2Offset.y + Sum2->GetSize().y / 2 + 36) * ScreenRatio, "%i", static_cast<int>(flCooldown2));
			}

			if (flCooldown3 > 0)
			{
				flCooldown3 += 1.f;
				Vec2 vecUltiCD = Position + vecUltimateOffset;
				Fonts->HudFont->Render(Position.x + (vecUltimateOffset.x + 52) * ScreenRatio, Position.y + (vecUltimateOffset.y + 9) * ScreenRatio, "%i", static_cast<int>(flCooldown3));
			}

			if (Ui->RespawnTime > 0)
			{
				Vec2 vecChampionCenter = Position + vecChampionOffset + vecChampionSize / 2;
				Fonts->HudFont->Render(Position.x + (vecChampionOffset.x + vecChampionSize.x / 2 - 14) * ScreenRatio, Position.y + (vecChampionOffset.y + vecChampionSize.y / 2 + 19) * ScreenRatio, "%i", static_cast<int>(Ui->RespawnTime - GGame->Time()));
			}
			GUtility->LogConsole("RIGHT SIDE COMPLETE");
			//Fonts->HudFont->Render(vecLevelPosition.x + 13, vecLevelPosition.y + 13, "%i", Ui->Player->GetLevel());
		}
	}
	else
	{
		GUtility->LogConsole("Drawing Left Side Bar");
		pHUD1_bar->Draw(Position.x - LeftOffsetBase.x, Position.y);
		{
			//draw hp bars
			pHUD1_hp->Draw((Position.x + 102) * ScreenRatio, Position.y);
			GRender->DrawFilledBox(Vec2((HPScaledOffset.x + 546) * ScreenRatio, HPScaledOffset.y + 50 * ScreenRatio), Vec2((-(133.f * ((100.f - Ui->Player->HealthPercent()) / 100))) * ScreenRatio, 16.f * ScreenRatio), Vec4(0, 0, 0, 225));
			if (Ui->Player->GetMaxMana() > 200 && !Ui->Player->IsDead())
			{
				pHUD1_mp->Draw((Position.x + 102) * ScreenRatio, (Position.y));
				try
				{
					GRender->DrawFilledBox(Vec2((HPScaledOffset.x + 546) * ScreenRatio, HPScaledOffset.y + 67 * ScreenRatio), Vec2((-(133.f * ((100.f - Ui->Player->ManaPercent()) / 100))) * ScreenRatio, 11 * ScreenRatio), Vec4(0, 0, 0, 200));
				}
				catch (const std::exception&)
				{
					
				}
			}
			
			//draw champ icon
			pHUD1_champ->Draw((Position.x) * ScreenRatio, Position.y);
			Ui->ChampionIcon->DrawCircle((Position.x + 75) * ScreenRatio, Position.y + 63 * ScreenRatio, 37 * ScreenRatio);
			//draw icons
			Sum1->Draw((Position.x + 7) * ScreenRatio, Position.y + 43 * ScreenRatio);
			Sum2->Draw((Position.x + 21) * ScreenRatio, Position.y + 77 * ScreenRatio);
			Ui->SpellIcons[kSlotR]->DrawCircle((Position.x + 42) * ScreenRatio, Position.y + 29 * ScreenRatio, 17 * ScreenRatio);
			pHUD1_icons->Draw(Position.x, Position.y);

			//HP BAR TEXT
			if (Menu.DrawHPBarText->Enabled())
			{
				switch (Menu.HPBarTextStyle->GetInteger())
				{
				case 1:
					Fonts->HudFont2->Render((HPScaledOffset.x + 467) * ScreenRatio, HPScaledOffset.y + 54 * ScreenRatio, "%i ", static_cast<int>(Ui->Player->GetHealth()));
					break;
				case 2:
					Fonts->HudFont2->Render((HPScaledOffset.x + 457) * ScreenRatio, HPScaledOffset.y + 54 * ScreenRatio, "%i/%i ", static_cast<int>(Ui->Player->GetHealth()), static_cast<int>(Ui->Player->GetMaxHealth()));
					break;
				case 3:
					Fonts->HudFont2->Render((HPScaledOffset.x + 467) * ScreenRatio, HPScaledOffset.y + 54 * ScreenRatio, "%i%%", static_cast<int>(Ui->Player->HealthPercent()));
					break;
				}
			}
			//MP BAR TEXT
			if (Menu.DrawManaBarText->Enabled() && Ui->Player->GetMaxMana() > 200)
			{
				switch (Menu.ManaBarTextStyle->GetInteger())
				{
				case 1:
					Fonts->HudFont2->Render((HPScaledOffset.x + 467) * ScreenRatio, HPScaledOffset.y + 68 * ScreenRatio, "%i ", static_cast<int>(Ui->Player->GetMana()));
					break;
				case 2:
					Fonts->HudFont2->Render((HPScaledOffset.x + 457) * ScreenRatio, HPScaledOffset.y + 68 * ScreenRatio, "%i/%i ", static_cast<int>(Ui->Player->GetMana()), static_cast<int>(Ui->Player->GetMaxMana()));
					break;
				case 3:
					Fonts->HudFont2->Render((HPScaledOffset.x + 467) * ScreenRatio, HPScaledOffset.y + 68 * ScreenRatio, "%i%%", static_cast<int>(Ui->Player->ManaPercent()));
					break;
				}
			}

			float flCooldown1 = Ui->Player->GetSpellRemainingCooldown(kSummonerSlot1);
			float flCooldown2 = Ui->Player->GetSpellRemainingCooldown(kSummonerSlot2);
			float flCooldown3 = Ui->Player->GetSpellLevel(kSlotR) > 0 ? Ui->Player->GetSpellRemainingCooldown(kSlotR) : 0.f;

			if (flCooldown1 > 0)
			{
				flCooldown1 += 1.f;
				Vec2 vecSpell1CD = Position + vecSpell1Offset + Sum1->GetSize() / 2;
				Fonts->HudFont->Render(vecSpell1CD.x * ScreenRatio, Position.y + (vecSpell1Offset.y + Sum1->GetSize().y / 2 + 35) * ScreenRatio, "%i", static_cast<int>(flCooldown1));
			}

			if (flCooldown2 > 0)
			{
				flCooldown2 += 1.f;
				Vec2 vecSpell2CD = Position + vecSpell2Offset + Sum2->GetSize() / 2;
				Fonts->HudFont->Render((vecSpell2CD.x + 14) * ScreenRatio, Position.y + (vecSpell2Offset.y + Sum2->GetSize().y / 2 + 36) * ScreenRatio, "%i", static_cast<int>(flCooldown2));
			}

			if (flCooldown3 > 0)
			{
				flCooldown3 += 1.f;
				Vec2 vecUltiCD = Position + vecUltimateOffset;
				Fonts->HudFont->Render((vecUltiCD.x - 64) * ScreenRatio, Position.y + (vecUltimateOffset.y + 9) * ScreenRatio, "%i", static_cast<int>(flCooldown3));
			}

			if (Ui->RespawnTime > 0)
			{
				Vec2 vecChampionCenter = Position + vecChampionOffset + vecChampionSize / 2;
				Fonts->HudFont->Render(vecChampionCenter.x * ScreenRatio, Position.y + (vecChampionOffset.y + vecChampionSize.y / 2 + 19) * ScreenRatio, "%i", static_cast<int>(Ui->RespawnTime - GGame->Time()));
			}

			//Fonts->HudFont->Render(vecLevelPosition.x + 13, vecLevelPosition.y + 13, "%i", Ui->Player->GetLevel());
		}
	}
}

void Gui::RenderPlayer2DElementSmall(HeroUI* Ui, Vec2 const& Position)
{

	Vec2 vecHealthOffset	= Position + Vec2(13, 62);
	Vec2 vecManaOffset		= Position + Vec2(13, 71);
	Vec2 vecChampionOffset	= Position + Vec2(12, 10);
	Vec2 vecSpell1Offset	= Position + Vec2(63, 8);
	Vec2 vecSpell2Offset	= Position + Vec2(63, 34);
	Vec2 vecUltimateOffset	= Position + Vec2(2, 2);

	Ui->ChampionIcon->SetScaleBySize(Vec2(43, 43));
	Ui->SummonerSpellIcons[0]->SetScaleBySize(Vec2(22, 22));
	Ui->SummonerSpellIcons[1]->SetScaleBySize(Vec2(22, 22));

	Ui->SummonerSpellIcons[0]->Draw(vecSpell1Offset.x, vecSpell1Offset.y);
	Ui->SummonerSpellIcons[1]->Draw(vecSpell2Offset.x, vecSpell2Offset.y);
	Ui->ChampionIcon->Draw(vecChampionOffset.x, vecChampionOffset.y);

	Textures->SmallOutline->Draw(Position.x, Position.y);

	if (Ui->Player->GetSpellLevel(kSlotR) > 0 && Ui->Player->GetSpellRemainingCooldown(kSlotR) == 0.f)
		Textures->UltimateReady->Draw(vecUltimateOffset.x, vecUltimateOffset.y);

	GRender->DrawFilledBox(vecHealthOffset, Vec2(74.5f * (Ui->Player->HealthPercent() / 100.f), 7.f), Vec4(0, 255, 0, 255));
	GRender->DrawFilledBox(vecManaOffset, Vec2(74.5f * (Ui->Player->ManaPercent() / 100.f), 7.f), Vec4(0, 0, 255, 255));

	Ui->ChampionIcon->Scale(1.f);
	Ui->SummonerSpellIcons[0]->Scale(1.f);
	Ui->SummonerSpellIcons[1]->Scale(1.f);
}

void Gui::RenderPlayer3DElement(HeroUI* Ui)
{

	Vec2 vecHealthBar;
	if (!Ui->Player->GetHPBarPosition(vecHealthBar) || !Ui->Player->IsVisible() || Ui->Player->IsDead())
	return;

	IUnit* pLocal = GEntityList->Player();
	bool bIsLocal = Ui->Player == GEntityList->Player();
	bool bIsEnemy = pLocal->IsEnemy(Ui->Player);
	Vec2 vecOffset = Vec2(bIsLocal ? 10.f : -8.f, bIsEnemy ? 17.f : (bIsLocal ? 6.f : 14.f));
	float SumCooldown1 = Ui->Player->GetSpellRemainingCooldown(kSummonerSlot1);
	float SumCooldown2 = Ui->Player->GetSpellRemainingCooldown(kSummonerSlot2);


	vecHealthBar += vecOffset;

	if (bIsLocal)
	{
		Textures->CDHudSelf->Draw(vecHealthBar.x, vecHealthBar.y);
		Ui->SummonerSpellIconsSmall[0]->Draw(vecHealthBar.x + 130, vecHealthBar.y + 2);
		Ui->SummonerSpellIconsSmall[1]->Draw(vecHealthBar.x + 152, vecHealthBar.y + 2);

		if (Ui->Player->GetSpellRemainingCooldown(kSummonerSlot1) > 0.f)
			Fonts->SummonerCooldownFont3D->Render(vecHealthBar.x + 142, vecHealthBar.y + 11, "%i", static_cast<int>(Ui->Player->GetSpellRemainingCooldown(kSummonerSlot1) + 1));
		if (Ui->Player->GetSpellRemainingCooldown(kSummonerSlot2) > 0.f)
			Fonts->SummonerCooldownFont3D->Render(vecHealthBar.x + 164, vecHealthBar.y + 11, "%i", static_cast<int>(Ui->Player->GetSpellRemainingCooldown(kSummonerSlot2) + 1));
	}
	else
	{
		vecHealthBar.x -= 34;
		vecHealthBar.y -= 9;
		Textures->CDHud->Draw(vecHealthBar.x, vecHealthBar.y);
		Ui->SummonerSpellIconsSmall[0]->Draw(vecHealthBar.x + 3, vecHealthBar.y + 2);
		Ui->SummonerSpellIconsSmall[1]->Draw(vecHealthBar.x + 25, vecHealthBar.y + 2);

		if (Ui->Player->GetSpellRemainingCooldown(kSummonerSlot1) > 0.f)
			Fonts->SummonerCooldownFont3D->Render(vecHealthBar.x + 14, vecHealthBar.y + 11, "%i", static_cast<int>(Ui->Player->GetSpellRemainingCooldown(kSummonerSlot1) + 1));
		if (Ui->Player->GetSpellRemainingCooldown(kSummonerSlot2) > 0.f)
			Fonts->SummonerCooldownFont3D->Render(vecHealthBar.x + 36, vecHealthBar.y + 11, "%i", static_cast<int>(Ui->Player->GetSpellRemainingCooldown(kSummonerSlot2) + 1));
	}

	Vec2 vecSpell = Vec2(20, 20);
	Vec2 vecSize = Vec2(22, 2);
	float flSkip = 27.f;

	if (bIsLocal)
	vecSpell = Vec2(24, 19);

	vecHealthBar.y++;

	for (auto i = 0; i < 4; i++)
	{
		Vec4 vecColor = Vec4(8, 131, 32, 200);
		float flCooldown = Ui->Player->GetSpellRemainingCooldown(i);
		float flWidth = vecSize.x;

		if (flCooldown > 0.f && Ui->Player->GetSpellLevel(i) > 0)
		{
			flWidth *= (Ui->Player->GetSpellTotalCooldown(i) - (flCooldown + 1)) / Ui->Player->GetSpellTotalCooldown(i);
			vecColor = Vec4(215, 139, 32, 200);
		}

		if (bIsLocal)
		{
			if (Ui->Player->GetSpellLevel(i) != 0)
				GRender->DrawFilledBox(vecHealthBar + vecSpell, Vec2(flWidth, vecSize.y), vecColor);

			// Needs to account for ammo CD as well
			if (flCooldown > 0.f && Ui->Player->GetSpellLevel(i) > 0)
				Fonts->CooldownFont->Render(vecHealthBar.x + vecSpell.x + (vecSize.x + 3) / 2, vecHealthBar.y + vecSpell.y + vecSize.y + 10, "%i", static_cast<int>(Ui->Player->GetSpellRemainingCooldown(i) + 1));
		}
		else
		{
			Vec2 BoxOffset = Vec2(32, -1);
			if (Ui->Player->GetSpellLevel(i) != 0)
				GRender->DrawFilledBox(vecHealthBar + vecSpell + BoxOffset, Vec2(flWidth, vecSize.y), vecColor);

			// Needs to account for ammo CD as well
			if (flCooldown > 0.f && Ui->Player->GetSpellLevel(i) > 0)
				Fonts->CooldownFont->Render(vecHealthBar.x + 32 + vecSpell.x + (vecSize.x + 3) / 2, vecHealthBar.y + vecSpell.y + vecSize.y + 9, "%i", static_cast<int>(Ui->Player->GetSpellRemainingCooldown(i) + 1));
		}
		vecSpell.x += flSkip;
	}
}

void Gui::RenderTeammates()
{
	GUtility->LogConsole("Rendering Teamates");

	if (Teammates.size() == 0)
		return;

	bool bLeftSide = Menu.TeamOnLeft->Enabled() ? true : false;
	Vec2 vecSize = bLeftSide ? Textures->OutlineLeft->GetSize() : Textures->OutlineRight->GetSize();
	GUtility->LogConsole("First Initialization Sequence Complete");
	if (Menu.HUDType->GetInteger() == 2)
		vecSize = Textures->SmallOutline->GetSize();
	else
		vecSize *= (Resolution.y / 1440);
	GUtility->LogConsole("Second Initialization Sequence Complete");
	float flPadding = 5 * (Resolution.y / 1440);
	float flLeft = bLeftSide ? 0 : Resolution.x - vecSize.x;
	float flTop = Menu.HUDType->GetInteger() == 2 ? 125.f : 140.f;
	Vec2 vecPosition = Vec2(flLeft, flTop);
	GUtility->LogConsole("Third Initialization Sequence Complete");
	for (auto pGui : Teammates)
	{
		if (!pGui->Valid)
			continue;
		
		if (Menu.Show2DHud->Enabled())
		{
			GUtility->LogConsole("Building 2D HUD in Loop");
			if (pGui->Player == GEntityList->Player()) // pGui is player
			{
				if (Menu.ShowSelf->Enabled())
				{
					if (Menu.HUDType->GetInteger() == 2)
						RenderPlayer2DElementSmall(pGui, vecPosition);
					else if (Menu.HUDType->GetInteger() == 0)
					{
						GUtility->LogConsole("RenderPlayer2DElementBig for SELF");
						RenderPlayer2DElementBig(pGui, vecPosition, bLeftSide);
					}
						
				}
			}
			else //pGui is teamate
			{
				if (Menu.ShowTeam->Enabled())
				{
					if (Menu.HUDType->GetInteger() == 2)
						RenderPlayer2DElementSmall(pGui, vecPosition);
					else if (Menu.HUDType->GetInteger() == 0)
					{
						GUtility->LogConsole("RenderPlayer2DElementBig for TEAMATE");
						RenderPlayer2DElementBig(pGui, vecPosition, bLeftSide);
					}
						
				}
			}			
		}
		

		if (Menu.Show3DHud->Enabled())
		{
			GUtility->LogConsole("Building 3D HUD in Loop");
			if (pGui->Player == GEntityList->Player()) // pGui is player
			{
				if (Menu.ShowSelf3D->Enabled())
				{
					RenderPlayer3DElement(pGui);
				}
			}
			else //pGui is teamate
			{
				if (Menu.ShowTeam3D->Enabled())
				{
					RenderPlayer3DElement(pGui);
				}
			}
		}
			

		vecPosition.y += vecSize.y + flPadding;
	}
}

void Gui::RenderEnemies()
{
	if (Enemies.size() == 0)
		return;

	bool bLeftSide		= Menu.TeamOnLeft->Enabled() ? false : true;
	Vec2 vecSize		= bLeftSide ? Textures->OutlineLeft->GetSize() : Textures->OutlineRight->GetSize();

	if (Menu.HUDType->GetInteger() == 2)
		vecSize = Textures->SmallOutline->GetSize();
	else
		vecSize *= (Resolution.y / 1440);

	float flPadding		= 5 * (Resolution.y / 1440);
	float flLeft		= bLeftSide ? 0 : Resolution.x - vecSize.x;
	float flTop			= Menu.HUDType->GetInteger() == 2 ? 125.f : 140.f;
	Vec2 vecPosition	= Vec2(flLeft, flTop);

	Vec2 StartingTestPos = Vec2(Resolution.x - 323 * (Resolution.y / 1080.f), Resolution.y - 451 * (Resolution.y / 1080.f));

	for (auto pGui : Enemies)
	{
		if (!pGui->Valid)
			continue;

		if (Menu.Show2DHud->Enabled() && Menu.ShowEnemies->Enabled())
		{
			if (Menu.HUDType->GetInteger() == 2)
				RenderPlayer2DElementSmall(pGui, vecPosition);
			else if (Menu.HUDType->GetInteger() == 1)
			{
				if (!pGui->Player->IsDead())
				{
					RenderPlayer2DElementNostalgic(pGui, StartingTestPos);
					StartingTestPos.x += 65 * (Resolution.y / 1080.f);
				}
			}
			else
				RenderPlayer2DElementBig(pGui, vecPosition, bLeftSide);
		}

		if (Menu.Show3DHud->Enabled() && Menu.ShowEnemies3D->Enabled())
			RenderPlayer3DElement(pGui);
		
		
		vecPosition.y += vecSize.y + flPadding;
	}
}

void Gui::UpdateChampions()
{
	for (auto pGui : UiPool)
	{
		if (!pGui->Valid)
			continue;



		auto pPlayer = pGui->Player;
		
		if (pPlayer->ChampionName() == GEntityList->Player()->ChampionName() && Menu.TestNotifyOnUltimate->Enabled()) //test notification
		{
			Vec2 UN_bgWidth = Textures->UN_bg->GetSize();
			Vec2 UN_pos = Vec2(0, 75 + Menu.UltimateYOffset->GetFloat());
			UN_pos.x = (Resolution.x / 2 - UN_bgWidth.x / 2) + Menu.UltimateXOffset->GetFloat();
			auto rTexture = pGui->SpellIcons[3];
			auto rChamp = pGui->ChampionIcon;
			float flCooldown = 0;
			float Opacity = 255;


			rTexture->SetColor(Vec4(255, 255, 255, Opacity));
			rChamp->SetColor(Vec4(255, 255, 255, Opacity));
			Textures->UN_bg->SetColor(Vec4(255, 255, 255, Opacity));
			Textures->UN_r->SetColor(Vec4(255, 255, 255, Opacity));

			if (Resolution.y < 1080.f)
			{
				rTexture->Scale(Resolution.y / 1080.f);
				rChamp->Scale(Resolution.y / 1080.f);
				Textures->UN_bg->Scale(Resolution.y / 1080.f);
				Textures->UN_r->Scale(Resolution.y / 1080.f);
			}

			Textures->UN_bg->Draw(UN_pos.x, UN_pos.y - flCooldown * 15);
			rTexture->Draw(UN_pos.x + 4, UN_pos.y + 3 - flCooldown * 15);
			rChamp->Draw(UN_pos.x + 156, UN_pos.y - flCooldown * 15);
			Textures->UN_r->Draw(UN_pos.x, UN_pos.y - flCooldown * 15);
		}


		if (pPlayer->IsEnemy(GEntityList->Player()) && Menu.NotifyOnUltimate->Enabled() && pPlayer->GetSpellLevel(kSlotR) != 0 && Menu.ChampionsToNotifyOnR[pPlayer->GetNetworkId()]->Enabled() &&
			pPlayer->GetSpellRemainingCooldown(kSlotR) > 0 && pPlayer->GetSpellRemainingCooldown(kSlotR) < 4 && pPlayer->GetSpellTotalCooldown(kSlotR) > 30.f)
		{
			Vec2 UN_bgWidth = Textures->UN_bg->GetSize();
			Vec2 UN_pos = Vec2(0, 75 + Menu.UltimateYOffset->GetFloat());
			UN_pos.x = (Resolution.x / 2 - UN_bgWidth.x / 2) + Menu.UltimateXOffset->GetFloat();
			auto rTexture = pGui->SpellIcons[3];
			auto rChamp = pGui->ChampionIcon;
			float flCooldown = pPlayer->GetSpellRemainingCooldown(kSlotR);
			float Opacity = 255 * (flCooldown / 4);
			

			rTexture->SetColor(Vec4(255, 255, 255, Opacity));
			rChamp->SetColor(Vec4(255, 255, 255, Opacity));
			Textures->UN_bg->SetColor(Vec4(255, 255, 255, Opacity));
			Textures->UN_r->SetColor(Vec4(255, 255, 255, Opacity));

			if (Resolution.y < 1080.f)
			{
				rTexture->Scale(Resolution.y / 1080.f);
				rChamp->Scale(Resolution.y / 1080.f);
				Textures->UN_bg->Scale(Resolution.y / 1080.f);
				Textures->UN_r->Scale(Resolution.y / 1080.f);
			}

			Textures->UN_bg->Draw(UN_pos.x, UN_pos.y - flCooldown * 15);
			rTexture->Draw(UN_pos.x + 4, UN_pos.y + 3 - flCooldown * 15);
			rChamp->Draw(UN_pos.x + 156, UN_pos.y - flCooldown * 15);
			Textures->UN_r->Draw(UN_pos.x, UN_pos.y - flCooldown * 15);
		}
		

		for (auto i = 0; i < 4; i++)
		{
			if (pGui->SpellIcons[i] == nullptr)
				continue;

			/*if (Menu.NotifyOnUltimate->Enabled() && i == kSlotR)
			{
				if (pPlayer->GetSpellLevel(i) == 0)// || pPlayer->GetSpellRemainingCooldown(i) == 0.f || pPlayer->GetSpellTotalCooldown(i) < 30.f)
					pGui->UltimateNotification = false;

				if (!pGui->UltimateNotification && pPlayer->GetSpellLevel(i) > 0)
				{
					float flCooldown = pPlayer->GetSpellRemainingCooldown(i);

					if (flCooldown < 5.f && flCooldown > 0.f)
					{
						pGui->UltimateNotification = true;

						if (Menu.NotifyOnUltimate->Enabled())
						{
							

							//GRender->Notification(Vec4(255, 255, 255, 255), 5, "%s ultimate returning in 5 seconds", pPlayer->ChampionName());
						}
							

					}
				}
			}*/

			if (pPlayer->GetSpellLevel(i) == 0 || pPlayer->GetSpellRemainingCooldown(i) > 0.f)
				pGui->SpellIcons[i]->SetColor(Vec4(100, 100, 100, 255));
			else
				pGui->SpellIcons[i]->SetColor(Vec4(255, 255, 255, 255));
		}

		if (pPlayer->IsDead())
		{
			if (pGui->RespawnTime == 0.f)
				pGui->RespawnTime = GetRespawnTime(pPlayer) + 1.f;

			if (!pGui->RespawnNotification && pGui->RespawnTime - GGame->Time() <= 5.f)
			{
				pGui->RespawnNotification = true;

				//if (Menu.NotifyOnRespawn->Enabled())
					//GRender->Notification(Vec4(255, 255, 255, 255), 5, "%s is respawning in 5 seconds", pPlayer->ChampionName());
			}
		}
		else
		{
			pGui->RespawnTime			= 0.f;
			pGui->RespawnNotification	= false;
		}

		pGui->ChampionIcon->SetColor(pPlayer->IsDead() ? Vec4(100, 100, 100, 255) : Vec4(255, 255, 255, 255));
		pGui->SummonerSpellIcons[0]->SetColor(pPlayer->GetSpellRemainingCooldown(kSummonerSlot1) > 0.f ? Vec4(100, 100, 100, 255) : Vec4(255, 255, 255, 255));
		pGui->SummonerSpellIcons[1]->SetColor(pPlayer->GetSpellRemainingCooldown(kSummonerSlot2) > 0.f ? Vec4(100, 100, 100, 255) : Vec4(255, 255, 255, 255));
		pGui->SummonerSpellIconsCircle[0]->SetColor(pPlayer->GetSpellRemainingCooldown(kSummonerSlot1) > 0.f ? Vec4(100, 100, 100, 255) : Vec4(255, 255, 255, 255));
		pGui->SummonerSpellIconsCircle[1]->SetColor(pPlayer->GetSpellRemainingCooldown(kSummonerSlot2) > 0.f ? Vec4(100, 100, 100, 255) : Vec4(255, 255, 255, 255));
		pGui->SummonerSpellIconsSmall[0]->SetColor(pPlayer->GetSpellRemainingCooldown(kSummonerSlot1) > 0.f ? Vec4(100, 100, 100, 255) : Vec4(255, 255, 255, 255));
		pGui->SummonerSpellIconsSmall[1]->SetColor(pPlayer->GetSpellRemainingCooldown(kSummonerSlot2) > 0.f ? Vec4(100, 100, 100, 255) : Vec4(255, 255, 255, 255));
	}
}

void Gui::LoadMenu()
{
	HUDTypesVector = { "Rembrandt", "Nostalgic", "Classic" };
	Menu.Enabled = Menu.Owner->CheckBox("Enable/Disable all HUD:", true);

	Menu.GUI2D = Menu.Owner->AddMenu("2D GUI Options");
	Menu.Show2DHud = Menu.GUI2D->CheckBox("Draw 2D HUD:", true);
	//Menu.MinimalisticHud = Menu.GUI2D->CheckBox("Old Style UI", false);
	Menu.HUDType = Menu.GUI2D->AddSelection("GUI Style:", 0, HUDTypesVector);
	Menu.TeamOnLeft = Menu.GUI2D->CheckBox("Flip Sides:", true);
	Menu.ShowSelf = Menu.GUI2D->CheckBox("Draw My UI:", true);
	Menu.ShowTeam = Menu.GUI2D->CheckBox("Draw Team UI:", true);
	Menu.ShowEnemies = Menu.GUI2D->CheckBox("Draw Enemy UI:", true);
	Menu.DrawHPBarText = Menu.GUI2D->CheckBox("Draw Text on Health Bar:", true);
	Menu.DrawManaBarText = Menu.GUI2D->CheckBox("Draw Text on Mana Bar:", true);
	Menu.HPBarTextStyle = Menu.GUI2D->AddInteger("Health Bar Text Style:", 1, 3, 2);
	Menu.ManaBarTextStyle = Menu.GUI2D->AddInteger("Mana Bar Text Style:", 1, 3, 1);;

	Menu.GUI3D = Menu.Owner->AddMenu("3D GUI Options");
	Menu.Show3DHud = Menu.GUI3D->CheckBox("Draw 3D HUD", true);
	Menu.ShowSelf3D = Menu.GUI3D->CheckBox("Draw My UI:", true);
	Menu.ShowTeam3D = Menu.GUI3D->CheckBox("Draw Team UI:", true);
	Menu.ShowEnemies3D = Menu.GUI3D->CheckBox("Draw Enemy UI:", true);
	
	auto pHeroes = Menu.Owner->AddMenu("Ultimate Cooldown Notifications");
	Menu.NotifyOnUltimate = pHeroes->CheckBox("Enable:", true);
	Menu.TestNotifyOnUltimate = pHeroes->CheckBox("Test/Sample Notification:", false);
	Menu.UltimateXOffset = pHeroes->AddFloat("X offset:", -4000, 4000, 0);
	Menu.UltimateYOffset = pHeroes->AddFloat("Y offset:", -4000, 4000, 0);

	for (auto pHero : GEntityList->GetAllHeros(false, true))
	{
		std::string szMenuName = std::string(pHero->ChampionName()) + ":";
		Menu.ChampionsToNotifyOnR[pHero->GetNetworkId()] = pHeroes->CheckBox(szMenuName.c_str(), true);
	}

	
	//Menu.NotifyOnRespawn = Menu.Owner->CheckBox("Notify on Respawn", false);
	//Menu.XOffset = Menu.Owner->AddFloat("X offset:", -200, 200, 0);
	//Menu.YOffset = Menu.Owner->AddFloat("Y offset:", -200, 200, 0);
	//Menu.RadiusOffset = Menu.Owner->AddFloat("Radius:", -200, 200, 0);
	//Menu.Resize = Menu.Owner->AddFloat("Resize bonus:", 0, 200, 75);
}

void Gui::LoadFonts()
{
	Resolution = GRender->ScreenSize();
	float ScreenRatio = Resolution.y / 1440.f;

	Fonts = new UiFonts;

	Fonts->DeathFont		= GRender->CreateFont("Tahoma", 18.f, kFontWeightBold);
	Fonts->SummonerCooldownFont3D = GRender->CreateFont("Tahoma", 10.f, kFontWeightBold);
	Fonts->CooldownFont		= GRender->CreateFont("Tahoma", 12.f, kFontWeightBold);
	Fonts->HudFont			= GRender->CreateFont("Tahoma", 11.f * ScreenRatio + 1.f, kFontWeightBold);
	Fonts->HudFont2			= GRender->CreateFont("Tahoma", 11.f * ScreenRatio + 1.f, kFontWeightBold);

	Fonts->HudFont2->SetOutline(false);

	Fonts->CooldownFont->SetLocationFlags(kFontLocationCenter);
	Fonts->CooldownFont->SetOutline(true);

	Fonts->SummonerCooldownFont3D->SetLocationFlags(kFontLocationCenter);
	Fonts->SummonerCooldownFont3D->SetOutline(true);

	Fonts->HudFont->SetLocationFlags(kFontLocationCenter);
	Fonts->HudFont->SetOutline(true);
}

void Gui::LoadTextureIcons()
{
	Textures = new UiTextures;

	//Logo
	Textures->UHud = CreateTextureEx("UHud");
	Textures->UHud->SetColor(Vec4(255, 255, 255, 150));

	// 2D Hud
	Textures->OutlineLeft	= CreateTextureEx("LeftHud");
	Textures->OutlineRight	= CreateTextureEx("RightHUD");
	Textures->EmptyLeft		= CreateTextureEx("LeftEmpty");
	Textures->EmptyRight	= CreateTextureEx("RightEmpty");
	Textures->Health		= CreateTextureEx("HP");
	Textures->Mana			= CreateTextureEx("MP");
	Textures->HUD1_bar		= CreateTextureEx("HUD1_bar");
	Textures->HUD1_champ		= CreateTextureEx("HUD1_champ");
	Textures->HUD1_icons		= CreateTextureEx("HUD1_icons");
	Textures->HUD1_barleft = CreateTextureEx("HUD1_barleft");
	Textures->HUD1_champleft = CreateTextureEx("HUD1_champleft");
	Textures->HUD1_iconsleft = CreateTextureEx("HUD1_iconsleft");
	Textures->HUD1_hp = CreateTextureEx("HUD1_hp");
	Textures->HUD1_mp = CreateTextureEx("HUD1_mp");
	Textures->OKTW_circle = CreateTextureEx("OKTW_circle");
	Textures->OKTW_bar = CreateTextureEx("OKTW_bar");

	//Ult Notification
	Textures->UN_bg = CreateTextureEx("UN_bg");
	Textures->UN_r = CreateTextureEx("UN_r");

	// 2D Hud Small
	Textures->SmallOutline	= CreateTextureEx("SB_HUD");
	Textures->UltimateReady = CreateTextureEx("SB_Ultimate");
	
	// 3D Hud
	Textures->CDHud			= CreateTextureEx("CD_Hud");
	Textures->CDHudSelf		= CreateTextureEx("CD_HudSelf");

	for (auto pPlayer : GEntityList->GetAllHeros(true, true))
	{
		if (pPlayer->GetMaxHealth() < 9000)
		{
			HeroUI* lpGui = new HeroUI();

			lpGui->Player = pPlayer;
			lpGui->RespawnTime = 0.f;
			lpGui->Valid = true;

			strcpy_s(lpGui->ChampionName, pPlayer->ChampionName());

			LoadHeroIcon(lpGui);
			LoadSpellIcons(lpGui);

			if (pPlayer->GetTeam() == GEntityList->Player()->GetTeam())
				Teammates.push_back(lpGui);
			else
				Enemies.push_back(lpGui);

			UiPool.push_back(lpGui);
		}
	}
}

void Gui::LoadHeroIcon(HeroUI* Ui)
{
	//Ui->ChampionIcon = CreateTextureEx(Ui->Player->ChampionName(), "ddragon.leagueoflegends.com/cdn/" + VersionNo + "/img/champion/" + Ui->Player->ChampionName() + ".png");
	Ui->ChampionIcon = CreateTextureEx(Ui->Player->ChampionName());

	if (Ui->ChampionIcon)
		Ui->ChampionIcon->Resize(69);
}

void Gui::LoadSpellIcons(HeroUI* Ui)
{
	// Champion Spells
	for (auto i = 0; i < 4; i++)
	{
		const char* pszSpellName = Ui->Player->GetSpellName(i);

		if (pszSpellName == nullptr)
		{
			Ui->SpellIcons[i] = nullptr;
			continue;
		}
		Ui->SpellIcons[i] = nullptr;
		if (i == kSlotR)
			Ui->SpellIcons[i] = CreateTextureEx(pszSpellName);
			

		//if (Ui->SpellIcons[i])
			//Ui->SpellIcons[i]->Resize(38);
	}


	// Summoner Spells
	for (auto i = 0; i < 2; i++)
	{
		const char* pszSpellName = Ui->Player->GetSpellName(i + 4);
		std::string pszSpellNameSmall;
		std::string pszSpellNameSS;
		pszSpellNameSmall += pszSpellName;
		pszSpellNameSmall += "Small";
		pszSpellNameSS += pszSpellName;
		pszSpellNameSS += "SS";

		Ui->SummonerSpellIcons[i]		= CreateTextureEx(pszSpellName);
		Ui->SummonerSpellIconsSmall[i]	= CreateTextureEx(pszSpellNameSS);
		Ui->SummonerSpellIconsCircle[i] = CreateTextureEx(pszSpellNameSmall);

	}
}

bool Gui::DoesTextureExist(std::string const& Filename, std::string& FullPath)
{
	std::string szFinalPath;
	GPluginSDK->GetBaseDirectory(szFinalPath);

	szFinalPath		+= "\\Textures\\" + Filename + ".png";
	FullPath		= szFinalPath;

	HANDLE hFile = CreateFileA(szFinalPath.c_str(), GENERIC_READ, FILE_SHARE_READ, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
	
	if (hFile != INVALID_HANDLE_VALUE)
	{
		CloseHandle(hFile);
		return true;
	}
	
	return false;
}

ITexture* Gui::CreateTextureEx(std::string const& Filename)
{
	

	std::string szFullPath;
	if (DoesTextureExist(Filename, szFullPath))
		return GRender->CreateTextureFromFile((Filename + ".png").c_str());
	/*
	std::string szImage;
	if (GPluginSDK->ReadFileFromURL(DownloadUrl, szImage))
		return GRender->CreateTextureFromMemory((uint8_t*)szImage.data(), szImage.length(), Filename.c_str());*/
	//GUtility->LogConsole("Could not find %s.png", Filename);

	auto DownloadUrl = "https://raw.githubusercontent.com/Harmenszoon/LeaguePlusPlusRembrandt/master/Resources/" + Filename + ".png";
	std::string szImage;
	if (GPluginSDK->ReadFileFromURL(DownloadUrl, szImage))
		return GRender->CreateTextureFromMemory((uint8_t*)szImage.data(), szImage.length(), Filename.c_str());

	//if (DoesTextureExist(Filename, szFullPath))
		//return GRender->CreateTextureFromFile((Filename + ".png").c_str());

	return nullptr;
}

void Gui::GetLatestVersionNo()
{
	VersionNo = "";
	/*
	std::string szJson;
	if (GPluginSDK->ReadFileFromURL("ddragon.leagueoflegends.com/api/versions.json", szJson))
	{
		size_t n = szJson.find("\"");

		if (n != szJson.npos)
			n++;

		szJson = szJson.substr(n, szJson.length() - n);
		n = szJson.find("\"");

		if (n != szJson.npos)
			VersionNo = szJson.substr(0, n);
	}*/
	GGame->PrintChat("<b><font color=\"#f8a101\">Utility PRO<b><font color=\"#FFFFFF\">++</font></b> Loaded</font></b>");
	GGame->PrintChat("<font color=\"#f8a101\">checking for updates...");

	std::string szData;
	if (GPluginSDK->ReadFileFromURL("rembrandt.000webhostapp.com/version.txt", szData))
	{
		auto n = szData.find("LatestVersion = ");

		if (n != szData.npos)
		{
			n += 15;
			szData = szData.substr(n, szData.size() - n);

		}

		auto k = szData.find(";");

		if (k != szData.npos)
			szData = szData.substr(0, k);

		auto dwGitVersion = atoi(szData.c_str());


		if (dwGitVersion != LocalVersion)
			GGame->PrintChat("<b><font color=\"#ce0505\">OUT OF DATE: Please download updates from forum!<b>");
		else
			GGame->PrintChat("<b><font color=\"#26cd05\">You have the latest version.<b>");
	}


	
}

// http://leagueoflegends.wikia.com/wiki/Death
float Gui::GetRespawnTime(IUnit* Player)
{
	// Level × 2.5 + 7.5 = your Base Respawn Wait time (BRW)
	float flWait = static_cast<float>(Player->GetLevel()) * 2.5f + 7.5f;
	float flMinutes = GGame->Time() / 60.f;

	// After minute 15, the wait time is increase by multiplying BRW as follows:
	// Total respawn time = BRW + ((BRW / 100) × (current minutes - 15) × 2 × 0.425)
	if (flMinutes >= 15.f && flMinutes < 30.f)
		flWait += ((flWait / 100.f) * (flMinutes - 15) * 2 * 0.425f);

	// After minute 30, the wait time is as follows:
	// Total respawn time = BRW + ((BRW / 100) × (current minutes - 15) × 2 × 0.425) + ((BRW / 100) × (current minutes - 30) × 2 × 0.30)
	if (flMinutes >= 30.f && flMinutes < 45.f)
		flWait += ((flWait / 100.f) * (flWait - 15) * 2 * 0.425f) + ((flWait / 100.f) * (flMinutes - 30.f) * 2 * 0.30f);

	// After minute 45, the wait time is as follows:
	// Total respawn time = BRW + ((BRW / 100) × (current minutes - 15) × 2 × 0.425) + ((BRW / 100) × (current minutes - 30) × 2 × 0.30) + ((BRW / 100) × (current minutes - 45) × 2 × 1.45)
	if (flMinutes >= 45.f)
		flWait += ((flWait / 100.f) * (flMinutes - 15.f) * 2 * 0.425f) + ((flWait / 100.f) * (flMinutes - 30) * 2 * 0.30f) + ((flWait / 100.f) * (flMinutes - 45.f) * 2 * 1.45f);

	// After minute 53.5 minutes, the respawn time is maxed out at 150% BRW.
	if (flMinutes >= 53.5)
		flWait *= 1.5;

	return flWait + GGame->Time();
}