#include "ChampionHandler.h"
// CHAMPION LIST
#include "Graves.h"
#include "Caitlyn.h"
#include "Twitch.h"
#include "Sivir.h"
#include "Tristana.h"
#include "Kogmaw.h"


ChampionHandler::ChampionHandler()
{
	Menu = GPluginSDK->AddMenu("[Rembrandt AIO] Champion Settings");

	if (strstr(GEntityList->Player()->ChampionName(), "Graves"))
		GravesHandler = new Graves(Menu);
	else if (strstr(GEntityList->Player()->ChampionName(), "Caitlyn"))
		CaitlynHandler = new Caitlyn(Menu);
	else if (strstr(GEntityList->Player()->ChampionName(), "Twitch"))
		TwitchHandler = new Twitch(Menu);
	else if (strstr(GEntityList->Player()->ChampionName(), "Sivir"))
		SivirHandler = new Sivir(Menu);
	else if (strstr(GEntityList->Player()->ChampionName(), "Tristana"))
		TristanaHandler = new Tristana(Menu);
	else if (strstr(GEntityList->Player()->ChampionName(), "KogMaw"))
		KogmawHandler = new Kogmaw(Menu);

	LoadEvents();
}

ChampionHandler::~ChampionHandler()
{
	UnloadEvents();
	Menu->Remove();
}

void ChampionHandler::_OnGameUpdate()
{
	if (strstr(GEntityList->Player()->ChampionName(), "Graves"))
		GravesHandler->OnGameUpdate();
	if (strstr(GEntityList->Player()->ChampionName(), "Caitlyn"))
		CaitlynHandler->OnGameUpdate();
	if (strstr(GEntityList->Player()->ChampionName(), "Twitch"))
		TwitchHandler->OnGameUpdate();
	if (strstr(GEntityList->Player()->ChampionName(), "Sivir"))
		SivirHandler->OnGameUpdate();
	if (strstr(GEntityList->Player()->ChampionName(), "Tristana"))
		TristanaHandler->OnGameUpdate();
	if (strstr(GEntityList->Player()->ChampionName(), "KogMaw"))
		KogmawHandler->OnGameUpdate();
}

void ChampionHandler::_OnRender()
{
	if (strstr(GEntityList->Player()->ChampionName(), "Graves"))
		GravesHandler->OnRender();
	if (strstr(GEntityList->Player()->ChampionName(), "Caitlyn"))
		CaitlynHandler->OnRender();
	if (strstr(GEntityList->Player()->ChampionName(), "Twitch"))
		TwitchHandler->OnRender();
	if (strstr(GEntityList->Player()->ChampionName(), "Sivir"))
		SivirHandler->OnRender();
	if (strstr(GEntityList->Player()->ChampionName(), "Tristana"))
		TristanaHandler->OnRender();
	if (strstr(GEntityList->Player()->ChampionName(), "KogMaw"))
		KogmawHandler->OnRender();
}

void ChampionHandler::_OnSpellCast(CastedSpell const& Args)
{
	if (strstr(GEntityList->Player()->ChampionName(), "Graves"))
		GravesHandler->OnSpellCast(Args);
	if (strstr(GEntityList->Player()->ChampionName(), "Caitlyn"))
		CaitlynHandler->OnSpellCast(Args);
	if (strstr(GEntityList->Player()->ChampionName(), "Twitch"))
		TwitchHandler->OnSpellCast(Args);
	if (strstr(GEntityList->Player()->ChampionName(), "Sivir"))
		SivirHandler->OnSpellCast(Args);
	if (strstr(GEntityList->Player()->ChampionName(), "Tristana"))
		TristanaHandler->OnSpellCast(Args);
	if (strstr(GEntityList->Player()->ChampionName(), "KogMaw"))
		KogmawHandler->OnSpellCast(Args);
}

bool ChampionHandler::_OnPreCast(int Slot, IUnit* Target, Vec3* StartPosition, Vec3* EndPosition)
{
	if (strstr(GEntityList->Player()->ChampionName(), "Twitch"))
		return TwitchHandler->OnPreCast(Slot, Target, StartPosition, EndPosition);
	else if (strstr(GEntityList->Player()->ChampionName(), "Caitlyn"))
		return CaitlynHandler->OnPreCast(Slot, Target, StartPosition, EndPosition);
	else
		return true;
}

void ChampionHandler::_OnOrbwalkAttack(IUnit* Source, IUnit* Target)
{
	if (strstr(GEntityList->Player()->ChampionName(), "Caitlyn"))
		CaitlynHandler->OnOrbwalkAttack(Source, Target);
	if (strstr(GEntityList->Player()->ChampionName(), "Twitch"))
		TwitchHandler->OnOrbwalkAttack(Source, Target);
	if (strstr(GEntityList->Player()->ChampionName(), "Sivir"))
		SivirHandler->OnOrbwalkAttack(Source, Target);
	if (strstr(GEntityList->Player()->ChampionName(), "Tristana"))
		TristanaHandler->OnOrbwalkAttack(Source, Target);
	if (strstr(GEntityList->Player()->ChampionName(), "KogMaw"))
		KogmawHandler->OnOrbwalkAttack(Source, Target);
}

void ChampionHandler::_BeforeAttack(IUnit* Target)
{
	if (strstr(GEntityList->Player()->ChampionName(), "Tristana"))
		TristanaHandler->BeforeAttack(Target);
	if (strstr(GEntityList->Player()->ChampionName(), "KogMaw"))
		KogmawHandler->BeforeAttack(Target);
}

void ChampionHandler::_OnInterruptible(InterruptibleSpell const& Args)
{
	if (strstr(GEntityList->Player()->ChampionName(), "Tristana"))
		TristanaHandler->OnInterruptible(Args);
}

void ChampionHandler::_OnGapCloser(GapCloserSpell const& Args)
{
	if (strstr(GEntityList->Player()->ChampionName(), "Tristana"))
		TristanaHandler->OnGapCloser(Args);
}

void ChampionHandler::_OnNewPath(IUnit* Source, std::vector<Vec3> const& Path)
{
	if (strstr(GEntityList->Player()->ChampionName(), "KogMaw"))
		KogmawHandler->OnNewPath(Source, Path);
}

void ChampionHandler::LoadEvents()
{
	GEventManager->AddEventHandler(kEventOnGameUpdate, OnGameUpdate);
	//GEventManager->AddEventHandler(kEventOnSpellCast, OnSpellCast);
	GEventManager->AddEventHandler(kEventOnDoCast, OnSpellCast);
	GEventManager->AddEventHandler(kEventOnPreCast, OnPreCast);
	GEventManager->AddEventHandler(kEventOnRender, OnRender);
	GEventManager->AddEventHandler(kEventOrbwalkAfterAttack, OnOrbwalkAttack);
	GEventManager->AddEventHandler(kEventOrbwalkBeforeAttack, BeforeAttack);
	GEventManager->AddEventHandler(kEventOnInterruptible, OnInterruptible);
	GEventManager->AddEventHandler(kEventOnGapCloser, OnGapCloser);
	GEventManager->AddEventHandler(kEventOnNewPath, OnNewPath);
}

void ChampionHandler::UnloadEvents()
{
	GEventManager->RemoveEventHandler(kEventOnGameUpdate, OnGameUpdate);
	GEventManager->RemoveEventHandler(kEventOnDoCast, OnSpellCast);
	GEventManager->RemoveEventHandler(kEventOnPreCast, OnPreCast);
	GEventManager->RemoveEventHandler(kEventOnRender, OnRender);
	GEventManager->RemoveEventHandler(kEventOrbwalkAfterAttack, OnOrbwalkAttack);
	GEventManager->RemoveEventHandler(kEventOrbwalkBeforeAttack, BeforeAttack);
	GEventManager->RemoveEventHandler(kEventOnInterruptible, OnInterruptible);
	GEventManager->RemoveEventHandler(kEventOnGapCloser, OnGapCloser);
	GEventManager->RemoveEventHandler(kEventOnNewPath, OnNewPath);
}

PLUGIN_EVENTD(void) ChampionHandler::OnGameUpdate()
{
	GPluginInstance3->_OnGameUpdate();
}

PLUGIN_EVENTD(void) ChampionHandler::OnRender()
{
	GPluginInstance3->_OnRender();
}

PLUGIN_EVENTD(void) ChampionHandler::OnSpellCast(CastedSpell const& Args)
{
	GPluginInstance3->_OnSpellCast(Args);
}
PLUGIN_EVENTD(void) ChampionHandler::OnPreCast(int Slot, IUnit* Target, Vec3* StartPosition, Vec3* EndPosition)
{
	GPluginInstance3->_OnPreCast(Slot, Target, StartPosition, EndPosition);
}

PLUGIN_EVENTD(void) ChampionHandler::OnOrbwalkAttack(IUnit* Source, IUnit* Target)
{
	GPluginInstance3->_OnOrbwalkAttack(Source, Target);
}

PLUGIN_EVENTD(void) ChampionHandler::BeforeAttack(IUnit* Target)
{
	GPluginInstance3->_BeforeAttack(Target);
}

PLUGIN_EVENTD(void) ChampionHandler::OnInterruptible(InterruptibleSpell const& Args)
{
	GPluginInstance3->_OnInterruptible(Args);
}

PLUGIN_EVENTD(void) ChampionHandler::OnGapCloser(GapCloserSpell const& Args)
{
	GPluginInstance3->_OnGapCloser(Args);
}

PLUGIN_EVENTD(void) ChampionHandler::OnNewPath(IUnit* Source, std::vector<Vec3> const& Path)
{
	GPluginInstance3->_OnNewPath(Source, Path);
}