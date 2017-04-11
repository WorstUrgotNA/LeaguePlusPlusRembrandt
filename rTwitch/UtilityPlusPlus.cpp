#include "PluginSDK.h"
#include "Color.h"
#include "string"
#include "windows.h"
#include "Gui.h"
#include "Awareness.h"
#include "ObjectTracker.h"
#include "Champion.h"
#include "Graves.h"
#include "Caitlyn.h"
#include "Twitch.h"
#include "Sivir.h"
#include "Tristana.h"
#include "Kogmaw.h"
#include "Lucian.h"



PluginSetup("Rembrandt [AIO]");

IMenu* MainMenu;
IMenu* AutoSmiteMenu;
IMenu* Defensives;
IMenu* SummonerHealMenu;
IMenu* SummonerBarrierMenu;
IMenu* SummonerIgniteMenu;
IMenu* CleanseMenu;
IMenu* Potions;
IMenu* DefensiveItems;
IMenu* FaceOfTheMountainMenu;
IMenu* RedemptionMenu;
IMenu* ZhonyasMenu;
IMenu* SeraphsMenu;
IMenu* LocketMenu;
IMenu* RanduinsMenu;
IMenu* OffensiveItems;
IMenu* GunbladeMenu;
IMenu* BotrkMenu;
IMenu* CutlassMenu;
IMenu* YoumuusMenu;
IMenu* GLP800Menu;
IMenu* AutoTrinketMenu;
IMenu* MikaelsMenu;
IMenu* CCFilter;
IMenu* SummonerTeller;

IMenu* AutoLevelMainMenu;
IMenu* ChampNameMenu;

IMenuOption*	LoadChampPlugin;

IMenuOption* EnableAutoLevelUp;
IMenuOption* ALUQ;
IMenuOption* ALUW;
IMenuOption* ALUE;
IMenuOption* ALUR;
IMenuOption* ALUOnlyR;
IMenuOption* ALUStartLevel;

IMenuOption* SummonerTellerKey;
IMenuOption* SummonerTellerEnabled;
IMenuOption* SmiteActive;
IMenuOption* SmiteKey;
IMenuOption* SmiteEnemies;
IMenuOption* SmiteBlue;
IMenuOption* SmiteRed;
IMenuOption* SmiteEnemies2Stacks;
IMenuOption* HealActive;
IMenuOption* HealPercent;
IMenuOption* HealTeamateActive;
IMenuOption* HealTeamatePercent;
IMenuOption* BarrierActive;
IMenuOption* BarrierPercent;
IMenuOption* CleanseActive;
IMenuOption* IgniteKSEnable;
IMenuOption* IgniteInCombo;
IMenuOption* PotionsEnabled;
IMenuOption* PotionsPercent;
IMenuOption* FaceOfTheMountainEnabled;
IMenuOption* FaceOfTheMountainPercent;
IMenuOption* RedemptionEnabled;
IMenuOption* RedemptionPercent;
IMenuOption* ZhonyasEnabled;
IMenuOption* SeraphsEnabled;
IMenuOption* SeraphsPercent;
IMenuOption* LocketEnabled;
IMenuOption* LocketPercent;
IMenuOption* RanduinsEnabled;
IMenuOption* GunbladeEnabled;
IMenuOption* BotrkEnabled;
IMenuOption* CutlassEnabled;
IMenuOption* YoumuusEnabled;
IMenuOption* GLP800Enabled;
IMenuOption* AutoUpgradeTrinket;
IMenuOption* CleanseCharm;
IMenuOption* CleanseDisarm;
IMenuOption* CleanseFear;
IMenuOption* CleanseFlee;
IMenuOption* CleansePolymorph;
IMenuOption* CleanseSnare;
IMenuOption* CleanseTaunt;
IMenuOption* CleanseStun;
IMenuOption* CleanseExhaust;
IMenuOption* CleanseSuppression;
IMenuOption* CleanseBlind;
IMenuOption* CleanseTeamate1;
IMenuOption* CleanseTeamate2;
IMenuOption* CleanseTeamate3;
IMenuOption* CleanseTeamate4;
IMenuOption* CleanseTeamate5;
IMenuOption* CleanseHumanizerDelay;
IMenuOption* DrawSmiteEnabled;
IMenuOption* CleanseDurationMin;
IMenuOption* TiamatEnabled;
IMenuOption* RavenousEnabled;
IMenuOption* TitanicEnabled;
IMenu* TiamatMenu;

IMenuOption* SkinID;
IMenuOption* SkinEnable;

IMenu* SkinMenu, *ChampMenu;

IUnit* CleanseTeamate01;
IUnit* CleanseTeamate02;
IUnit* CleanseTeamate03;
IUnit* CleanseTeamate04;
IUnit* CleanseTeamate05;
IUnit* MikaelsTargetToCast;

IUnit* Hero;

ISpell* HEAL;
ISpell* BARRIER;
ISpell* EXHAUST;
ISpell* CLEANSE;
ISpell2* SMITE;
ISpell* IGNITE;

IInventoryItem* QSS;
IInventoryItem* Scimitar;
IInventoryItem* Mikaels;

IInventoryItem* HealthPotion;
IInventoryItem* RefillablePotion;
IInventoryItem* HuntersPotion;
IInventoryItem* CorruptingPotion;
IInventoryItem* Biscuit;

IInventoryItem* FaceOfTheMountain;
IInventoryItem* Redemption;
IInventoryItem* Zhonyas;
IInventoryItem* Seraphs;
IInventoryItem* Locket;
IInventoryItem* Randuins;

IInventoryItem* Gunblade;
IInventoryItem* Botrk;
IInventoryItem* Cutlass;
IInventoryItem* Youmuus;
IInventoryItem* GLP800;
IInventoryItem* WardTrinket;
IInventoryItem* Tiamat;
IInventoryItem* Ravenous;
IInventoryItem* Titanic;

Vec3 JungleNotification;

Awareness* GPluginInstance = nullptr;
ObjectTracker* GPluginInstance2 = nullptr;
Champion * ChampHandler = nullptr;


std::string	formattedTime;

Vec2 AutoSmiteTextPos;

int DelayedSpellIndex = 0;
int HumanizeDelayCleanse;

short keystate;
short keystate2;
bool SummonerTellerKeyWasDown = false;
bool smiteKeyWasDown = false;
bool DelayedCleanse = false;
bool SkinNeedsReset = false;
bool PluginActive;
int CurrentSkinID;
float AfkTimer;



void LoadSpells()
{
	AutoSmiteTextPos = Vec2(0, 0);

	CurrentSkinID = Hero->GetSkinId();

	MikaelsTargetToCast = nullptr;
	auto PlayerSum1  = Hero->GetSpellName(kSummonerSlot1);
	auto PlayerSum2 = Hero->GetSpellName(kSummonerSlot2);

	if (strstr(PlayerSum1, "SummonerSmite")) { SMITE = GPluginSDK->CreateSpell2(kSummonerSlot1, kTargetCast, false, false, kCollidesWithNothing); }
	else if (!strcmp(PlayerSum1, "SummonerHeal")) { HEAL = GPluginSDK->CreateSpell(kSummonerSlot1, 850); }
	else if (strcmp(PlayerSum1, "SummonerBarrier") == 0) { BARRIER = GPluginSDK->CreateSpell(kSummonerSlot1, 0); }
	else if (strcmp(PlayerSum1, "SummonerExhaust") == 0) { EXHAUST = GPluginSDK->CreateSpell(kSummonerSlot1, 650); }
	else if (strcmp(PlayerSum1, "SummonerBoost") == 0) { CLEANSE = GPluginSDK->CreateSpell(kSummonerSlot1, 0); }
	else if (strcmp(PlayerSum1, "SummonerDot") == 0) { IGNITE = GPluginSDK->CreateSpell(kSummonerSlot1, 600); }

	if (strstr(PlayerSum2, "SummonerSmite")) { SMITE = GPluginSDK->CreateSpell2(kSummonerSlot2, kTargetCast, false, false, kCollidesWithNothing); }
	else if (strcmp(PlayerSum2, "SummonerHeal") == 0) { HEAL = GPluginSDK->CreateSpell(kSummonerSlot2, 850); }
	else if (strcmp(PlayerSum2, "SummonerBarrier") == 0) { BARRIER = GPluginSDK->CreateSpell(kSummonerSlot2, 0); }
	else if (strcmp(PlayerSum2, "SummonerExhaust") == 0) { EXHAUST = GPluginSDK->CreateSpell(kSummonerSlot2, 650); }
	else if (strcmp(PlayerSum2, "SummonerBoost") == 0) { CLEANSE = GPluginSDK->CreateSpell(kSummonerSlot2, 0); }
	else if (strcmp(PlayerSum2, "SummonerDot") == 0) { IGNITE = GPluginSDK->CreateSpell(kSummonerSlot2, 600); }

	JungleNotification = Vec3(0, 0, 0);

	//Cleansers
	QSS = GPluginSDK->CreateItemForId(3140, 0);
	Scimitar = GPluginSDK->CreateItemForId(3139, 0);
	Mikaels = GPluginSDK->CreateItemForId(3222, 600);
	//Potions
	HealthPotion = GPluginSDK->CreateItemForId(2003, 0);
	RefillablePotion = GPluginSDK->CreateItemForId(2031, 0);
	HuntersPotion = GPluginSDK->CreateItemForId(2032, 0);
	CorruptingPotion = GPluginSDK->CreateItemForId(2033, 0);
	Biscuit = GPluginSDK->CreateItemForId(2010, 0);
	//Defensives
	FaceOfTheMountain = GPluginSDK->CreateItemForId(3401, 600);
	Redemption = GPluginSDK->CreateItemForId(3107, 5500);
	Zhonyas = GPluginSDK->CreateItemForId(3157, 0);
	Seraphs = GPluginSDK->CreateItemForId(3040, 0);
	Locket = GPluginSDK->CreateItemForId(3190, 600);
	Randuins = GPluginSDK->CreateItemForId(3143, 400);
	//Offensive
	Gunblade = GPluginSDK->CreateItemForId(3146, 700);
	Botrk = GPluginSDK->CreateItemForId(3153, 550);
	Cutlass = GPluginSDK->CreateItemForId(3144, 550);
	Youmuus = GPluginSDK->CreateItemForId(3142, 0);
	GLP800 = GPluginSDK->CreateItemForId(3030, 800);
	Tiamat = GPluginSDK->CreateItemForId(3077, 400);
	Ravenous = GPluginSDK->CreateItemForId(3074, 400);
	Titanic = GPluginSDK->CreateItemForId(3748, 400);
	
}


float GetDistance(IUnit* source, IUnit* target)
{
	return (source->GetPosition() - target->GetPosition()).Length();
}

void AddTeamatesToCleanse()
{
	int indx = 0;
	auto Teamates = GEntityList->GetAllHeros(true, false);
	for (auto teamate : Teamates)
	{
		if (indx == 0)
		{
			indx++;
			CleanseTeamate1 = MikaelsMenu->CheckBox(teamate->ChampionName(), true);
			CleanseTeamate01 = teamate;
		}
		else if (indx == 1)
		{
			indx++;
			CleanseTeamate2 = MikaelsMenu->CheckBox(teamate->ChampionName(), true);
			CleanseTeamate02 = teamate;
		}
		else if (indx == 2)
		{
			indx++;
			CleanseTeamate3 = MikaelsMenu->CheckBox(teamate->ChampionName(), true);
			CleanseTeamate03 = teamate;
		}
		else if (indx == 3)
		{
			indx++;
			CleanseTeamate4 = MikaelsMenu->CheckBox(teamate->ChampionName(), true);
			CleanseTeamate04 = teamate;
		}
		else if (indx == 4)
		{
			CleanseTeamate5 = MikaelsMenu->CheckBox(teamate->ChampionName(), true);
			CleanseTeamate05 = teamate;
			return;
		}
	}
}

void DelayCast()
{
	if (HumanizeDelayCleanse > 0) { HumanizeDelayCleanse--; } //every game tick --

	if (DelayedSpellIndex == 1 && HumanizeDelayCleanse == 0) //SUMMONER CLEANSE
	{
		CLEANSE->CastOnPlayer();
		DelayedSpellIndex = 0;
	}
	if (DelayedSpellIndex == 2 && HumanizeDelayCleanse == 0) //QSS
	{
		QSS->CastOnPlayer();
		DelayedSpellIndex = 0;
	}
	if (DelayedSpellIndex == 3 && HumanizeDelayCleanse == 0) //SCIMITAR
	{
		Scimitar->CastOnPlayer();
		DelayedSpellIndex = 0;
	}
	if (DelayedSpellIndex == 4 && HumanizeDelayCleanse == 0) //MIKAELS
	{
		Mikaels->CastOnTarget(MikaelsTargetToCast);
		DelayedSpellIndex = 0;
		MikaelsTargetToCast = nullptr;
	}
}

int EnemiesInRange(IUnit* Source, float range)
{
	auto Targets = GEntityList->GetAllHeros(false, true);
	auto enemiesInRange = 0;

	for (auto target : Targets)
	{
		if (target != nullptr && !target->IsDead() && target->IsValidTarget())
		{
			auto flDistance = (target->GetPosition() - Source->GetPosition()).Length();
			if (flDistance < range)
			{
				enemiesInRange++;
			}
		}
	}
	return enemiesInRange;
}



void UseDefensives()
{
	//ITEMS
	if (FaceOfTheMountain->IsOwned() && FaceOfTheMountain->IsReady() && FaceOfTheMountainEnabled->Enabled() && !(Hero->IsDead()))
	{
		auto Teamates = GEntityList->GetAllHeros(true, false);
		for (IUnit* Teamate : Teamates)
		{
			if (!(Teamate->IsDead()) && FaceOfTheMountain->IsTargetInRange(Teamate) && Teamate->HealthPercent() <= FaceOfTheMountainPercent->GetFloat() && EnemiesInRange(Teamate, 600) > 0) { FaceOfTheMountain->CastOnTarget(Teamate); } //Cast on injured teamate
		}
	}

	if (Redemption->IsOwned() && Redemption->IsReady() && RedemptionEnabled->Enabled() && !(Hero->IsDead()))
	{
		auto Teamates = GEntityList->GetAllHeros(true, false);
		for (IUnit* Teamate : Teamates)
		{
			if (!(Teamate->IsDead()) && Redemption->IsTargetInRange(Teamate) && Teamate->HealthPercent() <= RedemptionPercent->GetFloat() && EnemiesInRange(Teamate, 700) > 0) { Redemption->CastOnPosition(Teamate->GetPosition()); } //Cast on injured teamate
		}
	}

	if (Seraphs->IsOwned() && Seraphs->IsReady() && SeraphsEnabled->Enabled() && !(Hero->IsDead()))
	{
		if (Hero->HealthPercent() <= SeraphsPercent->GetFloat() && EnemiesInRange(Hero, 700) > 0) { Seraphs->CastOnPlayer(); }

	}

	if (Locket->IsOwned() && Locket->IsReady() && LocketEnabled->Enabled() && !(Hero->IsDead()))
	{
		auto Teamates = GEntityList->GetAllHeros(true, false);
		for (IUnit* Teamate : Teamates)
		{
			if (!(Teamate->IsDead()) && Locket->IsTargetInRange(Teamate) && Teamate->HealthPercent() <= LocketPercent->GetFloat() && EnemiesInRange(Teamate, 600) > 0) { Locket->CastOnPlayer(); } //Cast on injured teamate
		}
	}


	//HEAL
	if (HEAL != nullptr && HEAL->IsReady() && HealActive->Enabled() && !(Hero->IsDead()))
	{
		if (Hero->HealthPercent() <= HealPercent->GetInteger() && EnemiesInRange(Hero, 600) > 0) { HEAL->CastOnPlayer(); } //Cast on self

		if (HealTeamateActive->Enabled())
		{
			auto Teamates = GEntityList->GetAllHeros(true, false);
			for (IUnit* Teamate : Teamates)
			{
				if (!(Teamate->IsDead()) && GetDistance(Hero, Teamate) <= HEAL->GetSpellRange() && Teamate->HealthPercent() <= HealTeamatePercent->GetInteger() && EnemiesInRange(Teamate, 600) > 0) { HEAL->CastOnUnit(Teamate); } //Cast on injured teamate
			}
		}
	}
	

	//BARRIER
	if (BARRIER != nullptr && BARRIER->IsReady() && BarrierActive->Enabled() && !(Hero->IsDead()) && Hero->HealthPercent() <= BarrierPercent->GetInteger() && EnemiesInRange(Hero, 500) > 0)
	{
		BARRIER->CastOnPlayer();
	}

	//Potions
	if (!(Hero->IsDead()) && PotionsEnabled->Enabled() && Hero->HealthPercent() <= PotionsPercent->GetFloat() && !GUtility->IsPositionInFountain(Hero->GetPosition()) &&
		!(Hero->HasBuff("RegenerationPotion") || Hero->HasBuff("ItemMiniRegenPotion") || Hero->HasBuff("ItemCrystalFlaskJungle") ||
			Hero->HasBuff("ItemCrystalFlask") || Hero->HasBuff("ItemDarkCrystalFlask")) )
	{
		if (HealthPotion->IsOwned() && HealthPotion->IsReady()) {
			HealthPotion->CastOnPlayer();
		}
		if (RefillablePotion->IsOwned() && RefillablePotion->IsReady()) {
			RefillablePotion->CastOnPlayer();
		}
		if (HuntersPotion->IsOwned() && HuntersPotion->IsReady()) {
			HuntersPotion->CastOnPlayer();
		}
		if (CorruptingPotion->IsOwned() && CorruptingPotion->IsReady()) {
			CorruptingPotion->CastOnPlayer();
		}
		if (Biscuit->IsOwned() && Biscuit->IsReady() && !GUtility->IsPositionInFountain(Hero->GetPosition())) {
			Biscuit->CastOnPlayer();
		}
	}
}


void CheckKeyPresses()
{
	keystate = GetAsyncKeyState(SmiteKey->GetInteger()); //smite key

	if (GUtility->IsLeagueWindowFocused() && !GGame->IsChatOpen())
	{
		if (keystate < 0) // If most-significant bit is set...
		{
			// key is down . . .
			if (smiteKeyWasDown == false)
			{
				//toggle smite
				if (SmiteActive->GetInteger() == 0) { SmiteActive->UpdateInteger(1); }
				else { SmiteActive->UpdateInteger(0); }
				smiteKeyWasDown = true;
			}
		}
		else
		{
			// key is up . . .
			smiteKeyWasDown = false;
		}
	}
}

int GetSmiteDamage(int PlayerLevel)
{
	switch (PlayerLevel)
	{
		case 1:
			return 390;
		case 2:
			return 410;
		case 3:
			return 430;
		case 4:
			return 450;
		case 5:
			return 480;
		case 6:
			return 510;
		case 7:
			return 540;
		case 8:
			return 570;
		case 9:
			return 600;
		case 10:
			return 640;
		case 11:
			return 680;
		case 12:
			return 720;
		case 13:
			return 760;
		case 14:
			return 800;
		case 15:
			return 850;
		case 16:
			return 900;
		case 17:
			return 950;
		case 18:
			return 1000;

	}
}

void AutoSmite() // AUTO SMITE PRO BY REMBRANDT
{	
	if (SMITE != nullptr && SMITE->IsReady() && SmiteActive->Enabled()) {
		
		for (IUnit* minion : GEntityList->GetAllMinions(false, false, true))
		{
			if (minion->IsValidTarget(Hero, 570) && minion->GetHealth() <= GetSmiteDamage(Hero->GetLevel()))
			{
				if ((strstr(minion->GetObjectName(), "Red") && SmiteRed->Enabled()) || (strstr(minion->GetObjectName(), "Blue") && SmiteBlue->Enabled()) || strstr(minion->GetObjectName(), "Dragon") || strstr(minion->GetObjectName(), "Rift") || strstr(minion->GetObjectName(), "Baron"))
				{
					if (SMITE->CastOnUnit(minion)) { return; }
				}
			}
		}
	}
}

bool TargetValidForMikaels(IUnit* Source)
{
	if (CleanseTeamate1 != nullptr && CleanseTeamate1->Enabled() && CleanseTeamate01->ChampionName() == Source->ChampionName())
	{
		return true;
	}
	if (CleanseTeamate2 != nullptr && CleanseTeamate2->Enabled() && CleanseTeamate02->ChampionName() == Source->ChampionName())
	{
		return true;
	}
	if (CleanseTeamate3 != nullptr && CleanseTeamate3->Enabled() && CleanseTeamate03->ChampionName() == Source->ChampionName())
	{
		return true;
	}
	if (CleanseTeamate4 != nullptr && CleanseTeamate4->Enabled() && CleanseTeamate04->ChampionName() == Source->ChampionName())
	{
		return true;
	}
	if (CleanseTeamate5 != nullptr && CleanseTeamate5->Enabled() && CleanseTeamate05->ChampionName() == Source->ChampionName())
	{
		return true;
	}
	return false;
}

void Combo()
{	
	//GRender->Notification(Vec4(255, 255, 255, 255), 0, "Smite Charges: %i", static_cast<int>(Hero->GetSpellTotalCooldown(kSummonerSlot1)));
	//Smite Enemies
	if (SMITE != nullptr && SMITE->IsReady() && SmiteEnemies->Enabled())
	{
		if ((SmiteEnemies2Stacks->Enabled() && Hero->GetSpellTotalCooldown(SMITE->GetSlot()) == 0) || !SmiteEnemies2Stacks->Enabled())
		{
			if (GTargetSelector->GetFocusedTarget() != nullptr && GTargetSelector->GetFocusedTarget()->IsValidTarget() && !(GTargetSelector->GetFocusedTarget()->IsDead()) && (Hero->GetPosition() - GTargetSelector->GetFocusedTarget()->GetPosition()).Length() < 500)
			{
				SMITE->CastOnTarget(GTargetSelector->GetFocusedTarget());
			}
			else
			{
				SMITE->CastOnTarget(GTargetSelector->FindTarget(QuickestKill, TrueDamage, 500));
			}
		}
	}
		
	//Ravenous
	if (RavenousEnabled->Enabled() && Ravenous->IsOwned() && Ravenous->IsReady() && !Hero->IsDead())
	{
		if (EnemiesInRange(Hero, 400) > 0) { Ravenous->CastOnPlayer(); }
	}
	//Tiamat
	if (TiamatEnabled->Enabled() && Tiamat->IsOwned() && Tiamat->IsReady() && !Hero->IsDead())
	{
		if (EnemiesInRange(Hero, 400) > 0) { Tiamat->CastOnPlayer(); }
	}
	//RANDUINS
	if (Randuins->IsOwned() && Randuins->IsReady() && RanduinsEnabled->Enabled() && !(Hero->IsDead()))
	{
		if (EnemiesInRange(Hero, 400) > 0) { Randuins->CastOnPlayer(); }
	}
	//Gunblade
	if (Gunblade->IsOwned() && Gunblade->IsReady() && GunbladeEnabled->Enabled() && !(Hero->IsDead()))
	{
		if (GTargetSelector->GetFocusedTarget() != nullptr && GTargetSelector->GetFocusedTarget()->IsValidTarget() && !(GTargetSelector->GetFocusedTarget()->IsDead()) && Gunblade->IsTargetInRange(GTargetSelector->GetFocusedTarget()))
		{
			Gunblade->CastOnTarget(GTargetSelector->GetFocusedTarget());
		}
		else
		{
			Gunblade->CastOnTarget(GTargetSelector->FindTarget(QuickestKill, SpellDamage, 700));
		}
	}
	//Botrk
	if (Botrk->IsOwned() && Botrk->IsReady() && BotrkEnabled->Enabled()  && !(Hero->IsDead()))
	{
		if (GTargetSelector->GetFocusedTarget() != nullptr && GTargetSelector->GetFocusedTarget()->IsValidTarget() && !(GTargetSelector->GetFocusedTarget()->IsDead()) && Botrk->IsTargetInRange(GTargetSelector->GetFocusedTarget()))
		{
			Botrk->CastOnTarget(GTargetSelector->GetFocusedTarget());
		}
		else
		{
			Botrk->CastOnTarget(GTargetSelector->FindTarget(QuickestKill, SpellDamage, 550));
		}
	}
	//Cutlass
	if (Cutlass->IsOwned() && Cutlass->IsReady() && CutlassEnabled->Enabled() && !(Hero->IsDead()))
	{
		if (GTargetSelector->GetFocusedTarget() != nullptr && GTargetSelector->GetFocusedTarget()->IsValidTarget() && !(GTargetSelector->GetFocusedTarget()->IsDead()) && Cutlass->IsTargetInRange(GTargetSelector->GetFocusedTarget()))
		{
			Cutlass->CastOnTarget(GTargetSelector->GetFocusedTarget());
		}
		else
		{
			Cutlass->CastOnTarget(GTargetSelector->FindTarget(QuickestKill, SpellDamage, 550));
		}
	}
	//Youmuus
	if (Youmuus->IsOwned() && Youmuus->IsReady() && YoumuusEnabled->Enabled() && !(Hero->IsDead()))
	{
		if (EnemiesInRange(Hero, 500) > 0)
		{
			Youmuus->CastOnPlayer();
		}
	}
	//GLP800
	if (GLP800->IsOwned() && GLP800->IsReady() && GLP800Enabled->Enabled() && !(Hero->IsDead()))
	{
		if (GTargetSelector->GetFocusedTarget() != nullptr && GTargetSelector->GetFocusedTarget()->IsValidTarget() && !(GTargetSelector->GetFocusedTarget()->IsDead()) && GLP800->IsTargetInRange(GTargetSelector->GetFocusedTarget()))
		{
			GLP800->CastOnTarget(GTargetSelector->GetFocusedTarget());
		}
		else
		{
			GLP800->CastOnTarget(GTargetSelector->FindTarget(QuickestKill, SpellDamage, 800));
		}
	}
	//IGNITE
	if (IGNITE != nullptr && (IgniteInCombo->Enabled() || IgniteKSEnable->Enabled()))
	{
		if (IGNITE->IsReady()) {
			auto BadDudes = GEntityList->GetAllHeros(false, true);
			for (auto BadDude : BadDudes)
			{
				//GRender->Notification(Vec4(255, 255, 255, 255), 5, "Damage: %d", GDamage->GetSummonerSpellDamage(GEntityList->Player(), BadDude, kSummonerSpellIgnite));
				if (BadDude != nullptr && !BadDude->IsDead() && GetDistance(Hero, BadDude) <= 600 && BadDude->GetHealth() <= GDamage->GetSummonerSpellDamage(Hero, BadDude, kSummonerSpellIgnite))
				{
					IGNITE->CastOnUnit(BadDude);
				}
			}
		}
	}
}

void AutoTrinket()
{
	if (Hero->GetLevel() < 9 || !GUtility->IsPositionInFountain(Hero->GetPosition())) { return; }
	if (AutoUpgradeTrinket->GetInteger() != 0)
	{
		if (AutoUpgradeTrinket->GetInteger() == 1) //sweeping lense
		{
			if(!Hero->HasItemId(3364))
				GGame->BuyItem(3364); //oracle alteration
		}
		if (AutoUpgradeTrinket->GetInteger() == 2) //ward
		{
			if (!Hero->HasItemId(3363))
			GGame->BuyItem(3363); //oracle alteration
		}
	}
}

 
void UpdateSkin()
{
	if (SkinEnable->Enabled())
	{
		//if (SkinID->GetInteger() != CurrentSkinID)
		//{
			Hero->SetSkinId(SkinID->GetInteger());
			//CurrentSkinID = SkinID->GetInteger();
			SkinNeedsReset = true;
		//}
	}
	else
	{
		if (SkinNeedsReset)
		{
			Hero->SetSkinId(Hero->GetSkinId());
			//CurrentSkinID = Hero->GetSkinId();
			SkinNeedsReset = false;
		}
	}
}
 

PLUGIN_EVENT(void) OnGameUpdate()
{
	UseDefensives();
	DelayCast();
	UpdateSkin();
	CheckKeyPresses();
	AutoSmite();
	AutoTrinket();

	PluginActive = LoadChampPlugin->Enabled();

	if (GOrbwalking->GetOrbwalkingMode() == kModeCombo) { Combo(); }

	ChampHandler->OnGameUpdate();
}

PLUGIN_EVENT(void) OnRender()
{
	if (SMITE != nullptr && DrawSmiteEnabled->Enabled())
	{
		if (GGame->Projection(Hero->GetPosition(), &AutoSmiteTextPos))
		{
			AutoSmiteTextPos.y += 100;
			GRender->DrawTextW(AutoSmiteTextPos, Vec4(255, 255, 255, 255), "%s", SmiteActive->Enabled() ? "AUTOSMITE ON" : "AUTOSMITE OFF");
		}
	}
	ChampHandler->OnRender();
}

PLUGIN_EVENT(void) OnRealSpellCast(CastedSpell const& Args)
{
	ChampHandler->OnRealSpellCast(Args);
}

PLUGIN_EVENT(void) OnSpellCast(CastedSpell const& Args)
{
	ChampHandler->OnSpellCast(Args);
}
PLUGIN_EVENT(bool) OnPreCast(int Slot, IUnit* Target, Vec3* StartPosition, Vec3* EndPosition)
{
	return ChampHandler->OnPreCast(Slot,Target, StartPosition, EndPosition);
}

PLUGIN_EVENT(void) OnOrbwalkAttack(IUnit* Source, IUnit* Target)
{
	ChampHandler->OnOrbwalkAttack(Source, Target);
}

PLUGIN_EVENT(void) BeforeAttack(IUnit* Target)
{
	ChampHandler->BeforeAttack(Target);
}

PLUGIN_EVENT(void) OnInterruptible(InterruptibleSpell const& Args)
{
	ChampHandler->OnInterruptible(Args);
}

PLUGIN_EVENT(void) OnGapCloser(GapCloserSpell const& Args)
{
	ChampHandler->OnGapCloser(Args);
}

PLUGIN_EVENT(void) OnOrbwalkAfterAttack(IUnit* Source, IUnit* Target)
{
	//Titanic
	if (TitanicEnabled->Enabled() && Titanic->IsOwned() && Titanic->IsReady() && !Hero->IsDead())
	{
		Titanic->CastOnPlayer();
	}
}

PLUGIN_EVENT(void) OnBuffAdd(IUnit* Source, void* BuffData)
{
	//GRender->Notification(Vec4(255, 255, 255, 255), 0, "%s", GBuffData->GetBuffName(BuffData));

	if (GBuffData->GetEndTime(BuffData) - GBuffData->GetStartTime(BuffData) >= CleanseDurationMin->GetFloat())
	{
		//CLEANSE
		if (CLEANSE != nullptr && CLEANSE->IsReady() && !(Hero->IsDead()))
		{
			if (Source == Hero && CleanseActive->Enabled() && !Source->IsDead())
			{
				if (Source->HasBuffOfType(BUFF_Charm) && CleanseCharm->Enabled())
				{
					if (HumanizeDelayCleanse == 0) { HumanizeDelayCleanse = CleanseHumanizerDelay->GetInteger(); }
					DelayedSpellIndex = 1;
				}
				else if (Source->HasBuffOfType(BUFF_Blind) && CleanseBlind->Enabled())
				{
					if (HumanizeDelayCleanse == 0) { HumanizeDelayCleanse = CleanseHumanizerDelay->GetInteger(); }
					DelayedSpellIndex = 1;
				}
				else if (Source->HasBuffOfType(BUFF_Disarm) && CleanseDisarm->Enabled())
				{
					if (HumanizeDelayCleanse == 0) { HumanizeDelayCleanse = CleanseHumanizerDelay->GetInteger(); }
					DelayedSpellIndex = 1;
				}
				else if (Source->HasBuffOfType(BUFF_Fear) && CleanseFear->Enabled())
				{
					if (HumanizeDelayCleanse == 0) { HumanizeDelayCleanse = CleanseHumanizerDelay->GetInteger(); }
					DelayedSpellIndex = 1;
				}
				else if (Source->HasBuffOfType(BUFF_Flee) && CleanseFlee->Enabled())
				{
					if (HumanizeDelayCleanse == 0) { HumanizeDelayCleanse = CleanseHumanizerDelay->GetInteger(); }
					DelayedSpellIndex = 1;
				}
				else if (Source->HasBuffOfType(BUFF_Polymorph) && CleansePolymorph->Enabled())
				{
					if (HumanizeDelayCleanse == 0) { HumanizeDelayCleanse = CleanseHumanizerDelay->GetInteger(); }
					DelayedSpellIndex = 1;
				}
				else if (Source->HasBuffOfType(BUFF_Snare) && CleanseSnare->Enabled())
				{
					if (HumanizeDelayCleanse == 0) { HumanizeDelayCleanse = CleanseHumanizerDelay->GetInteger(); }
					DelayedSpellIndex = 1;
				}
				else if (Source->HasBuffOfType(BUFF_Taunt) && CleanseTaunt->Enabled())
				{
					if (HumanizeDelayCleanse == 0) { HumanizeDelayCleanse = CleanseHumanizerDelay->GetInteger(); }
					DelayedSpellIndex = 1;
				}
				else if (Source->HasBuff("SummonerExhaust") && CleanseExhaust->Enabled())
				{
					if (HumanizeDelayCleanse == 0) { HumanizeDelayCleanse = CleanseHumanizerDelay->GetInteger(); }
					DelayedSpellIndex = 1;
				}
				else if (Source->HasBuffOfType(BUFF_Stun) && CleanseStun->Enabled())
				{
					if (HumanizeDelayCleanse == 0) { HumanizeDelayCleanse = CleanseHumanizerDelay->GetInteger(); }
					DelayedSpellIndex = 1;
				}
			}
		}

		//QSS
		if (Source == Hero && !Source->IsDead() && CleanseActive->Enabled() && (QSS->IsOwned() || Scimitar->IsOwned()))
		{
			if (Source->HasBuffOfType(BUFF_Charm) && CleanseCharm->Enabled())
			{
				if (QSS->IsOwned() && QSS->IsReady())
				{
					if (HumanizeDelayCleanse == 0) { HumanizeDelayCleanse = CleanseHumanizerDelay->GetInteger(); }
					DelayedSpellIndex = 2;
				}
				else if (Scimitar->IsOwned() && Scimitar->IsReady())
				{
					if (HumanizeDelayCleanse == 0) { HumanizeDelayCleanse = CleanseHumanizerDelay->GetInteger(); }
					DelayedSpellIndex = 3;
				}
			}
			else if (Source->HasBuffOfType(BUFF_Blind) && CleanseBlind->Enabled())
			{
				if (QSS->IsOwned() && QSS->IsReady())
				{
					if (HumanizeDelayCleanse == 0) { HumanizeDelayCleanse = CleanseHumanizerDelay->GetInteger(); }
					DelayedSpellIndex = 2;
				}
				else if (Scimitar->IsOwned() && Scimitar->IsReady())
				{
					if (HumanizeDelayCleanse == 0) { HumanizeDelayCleanse = CleanseHumanizerDelay->GetInteger(); }
					DelayedSpellIndex = 3;
				}
			}
			else if (Source->HasBuffOfType(BUFF_Disarm) && CleanseDisarm->Enabled())
			{
				if (QSS->IsOwned() && QSS->IsReady())
				{
					if (HumanizeDelayCleanse == 0) { HumanizeDelayCleanse = CleanseHumanizerDelay->GetInteger(); }
					DelayedSpellIndex = 2;
				}
				else if (Scimitar->IsOwned() && Scimitar->IsReady())
				{
					if (HumanizeDelayCleanse == 0) { HumanizeDelayCleanse = CleanseHumanizerDelay->GetInteger(); }
					DelayedSpellIndex = 3;
				}
			}
			else if (Source->HasBuffOfType(BUFF_Fear) && CleanseFear->Enabled())
			{
				if (QSS->IsOwned() && QSS->IsReady())
				{
					if (HumanizeDelayCleanse == 0) { HumanizeDelayCleanse = CleanseHumanizerDelay->GetInteger(); }
					DelayedSpellIndex = 2;
				}
				else if (Scimitar->IsOwned() && Scimitar->IsReady())
				{
					if (HumanizeDelayCleanse == 0) { HumanizeDelayCleanse = CleanseHumanizerDelay->GetInteger(); }
					DelayedSpellIndex = 3;
				}
			}
			else if (Source->HasBuffOfType(BUFF_Flee) && CleanseFlee->Enabled())
			{
				if (QSS->IsOwned() && QSS->IsReady())
				{
					if (HumanizeDelayCleanse == 0) { HumanizeDelayCleanse = CleanseHumanizerDelay->GetInteger(); }
					DelayedSpellIndex = 2;
				}
				else if (Scimitar->IsOwned() && Scimitar->IsReady())
				{
					if (HumanizeDelayCleanse == 0) { HumanizeDelayCleanse = CleanseHumanizerDelay->GetInteger(); }
					DelayedSpellIndex = 3;
				}
			}
			else if (Source->HasBuffOfType(BUFF_Polymorph) && CleansePolymorph->Enabled())
			{
				if (QSS->IsOwned() && QSS->IsReady())
				{
					if (HumanizeDelayCleanse == 0) { HumanizeDelayCleanse = CleanseHumanizerDelay->GetInteger(); }
					DelayedSpellIndex = 2;
				}
				else if (Scimitar->IsOwned() && Scimitar->IsReady())
				{
					if (HumanizeDelayCleanse == 0) { HumanizeDelayCleanse = CleanseHumanizerDelay->GetInteger(); }
					DelayedSpellIndex = 3;
				}
			}
			else if (Source->HasBuffOfType(BUFF_Snare) && CleanseSnare->Enabled())
			{
				if (QSS->IsOwned() && QSS->IsReady())
				{
					if (HumanizeDelayCleanse == 0) { HumanizeDelayCleanse = CleanseHumanizerDelay->GetInteger(); }
					DelayedSpellIndex = 2;
				}
				else if (Scimitar->IsOwned() && Scimitar->IsReady())
				{
					if (HumanizeDelayCleanse == 0) { HumanizeDelayCleanse = CleanseHumanizerDelay->GetInteger(); }
					DelayedSpellIndex = 3;
				}
			}
			else if (Source->HasBuffOfType(BUFF_Taunt) && CleanseTaunt->Enabled())
			{
				if (QSS->IsOwned() && QSS->IsReady())
				{
					if (HumanizeDelayCleanse == 0) { HumanizeDelayCleanse = CleanseHumanizerDelay->GetInteger(); }
					DelayedSpellIndex = 2;
				}
				else if (Scimitar->IsOwned() && Scimitar->IsReady())
				{
					if (HumanizeDelayCleanse == 0) { HumanizeDelayCleanse = CleanseHumanizerDelay->GetInteger(); }
					DelayedSpellIndex = 3;
				}
			}
			else if (Source->HasBuff("SummonerExhaust") && CleanseExhaust->Enabled())
			{
				if (QSS->IsOwned() && QSS->IsReady())
				{
					if (HumanizeDelayCleanse == 0) { HumanizeDelayCleanse = CleanseHumanizerDelay->GetInteger(); }
					DelayedSpellIndex = 2;
				}
				else if (Scimitar->IsOwned() && Scimitar->IsReady())
				{
					if (HumanizeDelayCleanse == 0) { HumanizeDelayCleanse = CleanseHumanizerDelay->GetInteger(); }
					DelayedSpellIndex = 3;
				}
			}
			else if (Source->HasBuffOfType(BUFF_Stun) && CleanseStun->Enabled())
			{
				if (QSS->IsOwned() && QSS->IsReady())
				{
					if (HumanizeDelayCleanse == 0) { HumanizeDelayCleanse = CleanseHumanizerDelay->GetInteger(); }
					DelayedSpellIndex = 2;
				}
				else if (Scimitar->IsOwned() && Scimitar->IsReady())
				{
					if (HumanizeDelayCleanse == 0) { HumanizeDelayCleanse = CleanseHumanizerDelay->GetInteger(); }
					DelayedSpellIndex = 3;
				}
			}
			else if (Source->HasBuffOfType(BUFF_Suppression) && CleanseSuppression->Enabled())
			{
				if (QSS->IsOwned() && QSS->IsReady())
				{
					if (HumanizeDelayCleanse == 0) { HumanizeDelayCleanse = CleanseHumanizerDelay->GetInteger(); }
					DelayedSpellIndex = 2;
				}
				else if (Scimitar->IsOwned() && Scimitar->IsReady())
				{
					if (HumanizeDelayCleanse == 0) { HumanizeDelayCleanse = CleanseHumanizerDelay->GetInteger(); }
					DelayedSpellIndex = 3;
				}
			}
		}

		//Mikael's
		if (Mikaels->IsOwned() && Mikaels->IsReady() && !(Hero->IsDead()) && TargetValidForMikaels(Source) && GetDistance(Hero, Source) <= 600 && !Source->IsEnemy(Source) && CleanseActive->Enabled() && !Source->IsDead())
		{
			if (Source->HasBuffOfType(BUFF_Charm) && CleanseCharm->Enabled())
			{
				if (HumanizeDelayCleanse == 0) { HumanizeDelayCleanse = CleanseHumanizerDelay->GetInteger(); }
				DelayedSpellIndex = 4;
				MikaelsTargetToCast = Source;
			}
			else if (Source->HasBuffOfType(BUFF_Blind) && CleanseBlind->Enabled())
			{
				if (HumanizeDelayCleanse == 0) { HumanizeDelayCleanse = CleanseHumanizerDelay->GetInteger(); }
				DelayedSpellIndex = 4;
				MikaelsTargetToCast = Source;
			}
			else if (Source->HasBuffOfType(BUFF_Disarm) && CleanseDisarm->Enabled())
			{
				if (HumanizeDelayCleanse == 0) { HumanizeDelayCleanse = CleanseHumanizerDelay->GetInteger(); }
				DelayedSpellIndex = 4;
				MikaelsTargetToCast = Source;
			}
			else if (Source->HasBuffOfType(BUFF_Fear) && CleanseFear->Enabled())
			{
				if (HumanizeDelayCleanse == 0) { HumanizeDelayCleanse = CleanseHumanizerDelay->GetInteger(); }
				DelayedSpellIndex = 4;
				MikaelsTargetToCast = Source;
			}
			else if (Source->HasBuffOfType(BUFF_Flee) && CleanseFlee->Enabled())
			{
				if (HumanizeDelayCleanse == 0) { HumanizeDelayCleanse = CleanseHumanizerDelay->GetInteger(); }
				DelayedSpellIndex = 4;
				MikaelsTargetToCast = Source;
			}
			else if (Source->HasBuffOfType(BUFF_Polymorph) && CleansePolymorph->Enabled())
			{
				if (HumanizeDelayCleanse == 0) { HumanizeDelayCleanse = CleanseHumanizerDelay->GetInteger(); }
				DelayedSpellIndex = 4;
				MikaelsTargetToCast = Source;
			}
			else if (Source->HasBuffOfType(BUFF_Snare) && CleanseSnare->Enabled())
			{
				if (HumanizeDelayCleanse == 0) { HumanizeDelayCleanse = CleanseHumanizerDelay->GetInteger(); }
				DelayedSpellIndex = 4;
				MikaelsTargetToCast = Source;
			}
			else if (Source->HasBuffOfType(BUFF_Taunt) && CleanseTaunt->Enabled())
			{
				if (HumanizeDelayCleanse == 0) { HumanizeDelayCleanse = CleanseHumanizerDelay->GetInteger(); }
				DelayedSpellIndex = 4;
				MikaelsTargetToCast = Source;
			}
			else if (Source->HasBuff("SummonerExhaust") && CleanseExhaust->Enabled())
			{
				if (HumanizeDelayCleanse == 0) { HumanizeDelayCleanse = CleanseHumanizerDelay->GetInteger(); }
				DelayedSpellIndex = 4;
				MikaelsTargetToCast = Source;
			}
			else if (Source->HasBuffOfType(BUFF_Stun) && CleanseStun->Enabled())
			{
				if (HumanizeDelayCleanse == 0) { HumanizeDelayCleanse = CleanseHumanizerDelay->GetInteger(); }
				DelayedSpellIndex = 4;
				MikaelsTargetToCast = Source;
			}
		}
	}
}


PLUGIN_EVENT(void) OnLevelUp(IUnit* Source, int NewLevel)
{
	ChampHandler->OnLevelUp(Source, NewLevel);

	if (EnableAutoLevelUp->Enabled() && Source == Hero && NewLevel >= ALUStartLevel->GetInteger()) //auto level
	{

		for (int i = 1; i <= 4; i++)
		{
			if (ALUR->GetInteger() == i) {
				Source->LevelUpSpell(kSlotR);
			}
			if (ALUQ->GetInteger() == i && !ALUOnlyR->Enabled()) {
				Source->LevelUpSpell(kSlotQ);
			}
			if (ALUW->GetInteger() == i && !ALUOnlyR->Enabled()) {
				Source->LevelUpSpell(kSlotW);
			}
			if (ALUE->GetInteger() == i && !ALUOnlyR->Enabled()) {
				Source->LevelUpSpell(kSlotE);
			}

		}
	}
}

PLUGIN_EVENT(void) OnPlayAnimation(IUnit* Source, std::string const Args)
{
	ChampHandler->OnPlayAnimation(Source, Args);
}

void  LoadEvents()
{
	GEventManager->AddEventHandler(kEventOrbwalkAfterAttack, OnOrbwalkAfterAttack);
	GEventManager->AddEventHandler(kEventOnGameUpdate, OnGameUpdate);
	GEventManager->AddEventHandler(kEventOnRender, OnRender);
	GEventManager->AddEventHandler(kEventOnBuffAdd, OnBuffAdd);
	GEventManager->AddEventHandler(kEventOnLevelUp, OnLevelUp);
	GEventManager->AddEventHandler(kEventOnSpellCast, OnRealSpellCast);
	GEventManager->AddEventHandler(kEventOnDoCast, OnSpellCast);
	GEventManager->AddEventHandler(kEventOnPreCast, OnPreCast);
	GEventManager->AddEventHandler(kEventOrbwalkAfterAttack, OnOrbwalkAttack);
	GEventManager->AddEventHandler(kEventOrbwalkBeforeAttack, BeforeAttack);
	GEventManager->AddEventHandler(kEventOnInterruptible, OnInterruptible);
	GEventManager->AddEventHandler(kEventOnGapCloser, OnGapCloser);
	GEventManager->AddEventHandler(kEventOnPlayAnimation, OnPlayAnimation);
}

void  UnloadEvents()
{
	GEventManager->RemoveEventHandler(kEventOrbwalkAfterAttack, OnOrbwalkAfterAttack);
	GEventManager->RemoveEventHandler(kEventOnBuffAdd, OnBuffAdd);
	GEventManager->RemoveEventHandler(kEventOnLevelUp, OnLevelUp);
	GEventManager->RemoveEventHandler(kEventOnGameUpdate, OnGameUpdate);
	GEventManager->RemoveEventHandler(kEventOnSpellCast, OnRealSpellCast);
	GEventManager->RemoveEventHandler(kEventOnDoCast, OnSpellCast);
	GEventManager->RemoveEventHandler(kEventOnPreCast, OnPreCast);
	GEventManager->RemoveEventHandler(kEventOnRender, OnRender);
	GEventManager->RemoveEventHandler(kEventOrbwalkAfterAttack, OnOrbwalkAttack);
	GEventManager->RemoveEventHandler(kEventOrbwalkBeforeAttack, BeforeAttack);
	GEventManager->RemoveEventHandler(kEventOnInterruptible, OnInterruptible);
	GEventManager->RemoveEventHandler(kEventOnGapCloser, OnGapCloser);
	GEventManager->RemoveEventHandler(kEventOnPlayAnimation, OnPlayAnimation);
}

// Called when plugin is first loaded
PLUGIN_API void OnLoad(IPluginSDK* PluginSDK)
{
	// Initializes global interfaces for core access
	PluginSDKSetup(PluginSDK);

	//GUtility->CreateDebugConsole();

	Hero = GEntityList->Player();

	//Initialize Menus
	MainMenu = GPluginSDK->AddMenu("Rembrandt AIO & Utility PRO++");

	Defensives = MainMenu->AddMenu("Activator PRO");

	GUtility->LogConsole("Debug before awareness active");

	GPluginInstance = new Awareness(MainMenu);

	GUtility->LogConsole("Awareness active");

	GPluginInstance2 = new ObjectTracker(MainMenu);

	CleanseMenu = Defensives->AddMenu("Cleanse / QSS / Mikaels");
	CleanseActive = CleanseMenu->CheckBox("Enabled: ", true);
	CleanseHumanizerDelay = CleanseMenu->AddInteger("Humanizer Delay in Ticks:", 0, 60, 0);
	CleanseDurationMin = CleanseMenu->AddFloat("Minimum Duration (secs) to Cleanse:", 0, 3, 1);
	CCFilter = CleanseMenu->AddMenu("CC Filter");
	MikaelsMenu = CleanseMenu->AddMenu("Mikaels Filter");
	AddTeamatesToCleanse();
	CleanseCharm = CCFilter->CheckBox("Use on Charm ", true);
	CleanseDisarm = CCFilter->CheckBox("Use on Disarm ", true);
	CleanseFear = CCFilter->CheckBox("Use on Fear ", true);
	CleanseFlee = CCFilter->CheckBox("Use on Flee ", true);
	CleansePolymorph = CCFilter->CheckBox("Use on Polymorph ", true);
	CleanseSnare = CCFilter->CheckBox("Use on Snare", true);
	CleanseStun = CCFilter->CheckBox("Use on Stun ", true);
	CleanseExhaust = CCFilter->CheckBox("Use on Exhaust ", true);
	CleanseSuppression = CCFilter->CheckBox("Use on Suppression ", true);
	CleanseBlind = CCFilter->CheckBox("Use on Blind ", true);
	CleanseTaunt = CCFilter->CheckBox("Use on Taunt ", true);

	DefensiveItems = Defensives->AddMenu("Defensive Items"); // DEFENSIVE ITEMS MENU
	FaceOfTheMountainMenu = DefensiveItems->AddMenu("Face of The Mountain");
	FaceOfTheMountainEnabled = FaceOfTheMountainMenu->CheckBox("Enabled:", true);
	FaceOfTheMountainPercent = FaceOfTheMountainMenu->AddFloat("Use at Health %:", 1, 99, 50);

	RedemptionMenu = DefensiveItems->AddMenu("Redemption");
	RedemptionEnabled = RedemptionMenu->CheckBox("Enabled:", true);
	RedemptionPercent = RedemptionMenu->AddFloat("Use at Health %:", 1, 99, 40);

	ZhonyasMenu = DefensiveItems->AddMenu("Zhonyas Hourglass");
	ZhonyasEnabled = ZhonyasMenu->CheckBox("COMING SOON WITH DAMAGE PREDICTION:", true);

	SeraphsMenu = DefensiveItems->AddMenu("Seraphs Embrace");
	SeraphsEnabled = SeraphsMenu->CheckBox("Enabled:", true);
	SeraphsPercent = SeraphsMenu->AddFloat("Use at Health %:", 1, 99, 50);

	LocketMenu = DefensiveItems->AddMenu("Locket of the Iron Solari");
	LocketEnabled = LocketMenu->CheckBox("Enabled:", true);
	LocketPercent = LocketMenu->AddFloat("Use at Health %:", 1, 99, 45);

	RanduinsMenu = DefensiveItems->AddMenu("Randuins Omen");
	RanduinsEnabled = RanduinsMenu->CheckBox("Enabled:", true);

	OffensiveItems = Defensives->AddMenu("Offensive Items");//OFFENSIVE ITEMS MENU

	TiamatMenu = OffensiveItems->AddMenu("Tiamat/Hydras");
	TiamatEnabled = TiamatMenu->CheckBox("Use Tiamat:", true);
	RavenousEnabled = TiamatMenu->CheckBox("Use Ravenous Hydra:", true);
	TitanicEnabled = TiamatMenu->CheckBox("Use Titanic Hydra:", true);
	GunbladeMenu = OffensiveItems->AddMenu("Hextech Gunblade");
	GunbladeEnabled = GunbladeMenu->CheckBox("Enabled in Combo:", true);
	BotrkMenu = OffensiveItems->AddMenu("Blade of the Ruined King");
	BotrkEnabled = BotrkMenu->CheckBox("Enabled in Combo:", true);
	CutlassMenu = OffensiveItems->AddMenu("Bilgewater Cutlass");
	CutlassEnabled = CutlassMenu->CheckBox("Enabled in Combo:", true);
	YoumuusMenu = OffensiveItems->AddMenu("Youmuus Ghostblade");
	YoumuusEnabled = YoumuusMenu->CheckBox("Enabled in Combo:", true);

	GLP800Menu = OffensiveItems->AddMenu("Hextech GLP-800");
	GLP800Enabled = GLP800Menu->CheckBox("Enabled in Combo:", true);

	SummonerHealMenu = Defensives->AddMenu("Summoner: Heal");
	HealActive = SummonerHealMenu->CheckBox("Enabled: ", true);
	HealPercent = SummonerHealMenu->AddInteger("Use at Health %: ", 1, 99, 30);
	HealTeamateActive = SummonerHealMenu->CheckBox("Use on Teamates: ", true);
	HealTeamatePercent = SummonerHealMenu->AddInteger("Use at Teamate Health %: ", 1, 99, 30);

	SummonerBarrierMenu = Defensives->AddMenu("Summoner: Barrier");
	BarrierActive = SummonerBarrierMenu->CheckBox("Enabled: ", true);
	BarrierPercent = SummonerBarrierMenu->AddInteger("Use at Health %: ", 1, 99, 30);

	AutoSmiteMenu = Defensives->AddMenu("Summoner: Smite");
	SmiteActive = AutoSmiteMenu->CheckBox("Smite Epic Monsters: ", true);
	SmiteRed = AutoSmiteMenu->CheckBox("Smite Red Buff:", true);
	SmiteBlue = AutoSmiteMenu->CheckBox("Smite Blue Buff:", true);
	SmiteKey = AutoSmiteMenu->AddKey("Toggle Key:", 77);
	DrawSmiteEnabled = AutoSmiteMenu->CheckBox("Draw Auto Smite Enabled:", true);
	SmiteEnemies = AutoSmiteMenu->CheckBox("Smite Enemy Champions in Combo:", true);
	SmiteEnemies2Stacks = AutoSmiteMenu->CheckBox("Only Smite Champions at 2 Stacks:", true);

	SummonerIgniteMenu = Defensives->AddMenu("Summoner: Ignite");
	IgniteKSEnable = SummonerIgniteMenu->CheckBox("Enable Ignite KS:", true);
	IgniteInCombo = SummonerIgniteMenu->CheckBox("Enable Ignite in Combo:", true);

	Potions = Defensives->AddMenu("Potions");
	PotionsEnabled = Potions->CheckBox("Use Potions: ", true);
	PotionsPercent = Potions->AddFloat("Drink at Health %: ", 1, 99, 60);

	AutoTrinketMenu = MainMenu->AddMenu("Auto Trinket");
	AutoUpgradeTrinket = AutoTrinketMenu->AddInteger("Auto Buy [0] None [1] Red [2] Blue:", 0, 2, 0);
	
	GUtility->LogConsole("Menus Loaded");

	std::string szMenuName = std::string(Hero->ChampionName());
	szMenuName += " Level Builder";

	AutoLevelMainMenu = MainMenu->AddMenu("Auto Level PRO");
	ChampNameMenu = AutoLevelMainMenu->AddMenu(szMenuName.c_str());

	EnableAutoLevelUp = ChampNameMenu->CheckBox("Enable: ", false);
	ALUOnlyR = ChampNameMenu->CheckBox("Only level R (Ultimate):", false);
	ALUR = ChampNameMenu->AddInteger("R: ", 1, 4, 1);
	ALUQ = ChampNameMenu->AddInteger("Q: ", 1, 4, 2);
	ALUW = ChampNameMenu->AddInteger("W: ", 1, 4, 3);
	ALUE = ChampNameMenu->AddInteger("E: ", 1, 4, 4);
	ALUStartLevel = ChampNameMenu->AddInteger("Start at level: ", 1, 16, 4);

	ChampMenu = GPluginSDK->AddMenu("Rembrandt AIO Champion Settings");
	LoadChampPlugin = ChampMenu->CheckBox("Use Rembrandt AIO Champion Plugins:", true);
	PluginActive = LoadChampPlugin->Enabled();	

	SkinMenu = MainMenu->AddMenu("Skin Changer");
	SkinEnable = SkinMenu->CheckBox("Enable:", false);
	SkinID = SkinMenu->AddInteger("Select Skin ID:", 1, 30, 1);
	LoadSpells();

	// LOAD CHAMP PLUGIN
	if (!PluginActive)
		ChampHandler = new Champion(ChampMenu, Hero);
	else if (strstr(Hero->ChampionName(), "Graves"))
		ChampHandler = new Graves(ChampMenu, Hero);
	else if (strstr(Hero->ChampionName(), "Lucian"))
		ChampHandler = new Lucian(ChampMenu, Hero);
	else if (strstr(Hero->ChampionName(), "Caitlyn"))
		ChampHandler = new Caitlyn(ChampMenu, Hero);
	else if (strstr(Hero->ChampionName(), "Twitch"))
		ChampHandler = new Twitch(ChampMenu, Hero);
	else if (strstr(Hero->ChampionName(), "Sivir"))
		ChampHandler = new Sivir(ChampMenu, Hero);
	else if (strstr(Hero->ChampionName(), "Tristana"))
		ChampHandler = new Tristana(ChampMenu, Hero);
	else if (strstr(Hero->ChampionName(), "KogMaw"))
		ChampHandler = new Kogmaw(ChampMenu, Hero);
	else
		ChampHandler = new Champion(ChampMenu, Hero);

	LoadEvents();
}

// Called when plugin is unloaded
PLUGIN_API void OnUnload()
{
	GUtility->LogConsole("--- UNLOADED ---");
	MainMenu->Remove();

	UnloadEvents();

	delete GPluginInstance;
	delete GPluginInstance2;
	delete ChampHandler;
}
