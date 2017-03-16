#include "ChampionHandler.h"
// CHAMPION LIST
#include "Graves.h"
#include "Caitlyn.h"
#include "Twitch.h"


ChampionHandler::ChampionHandler()
{
	Menu = GPluginSDK->AddMenu("[Rembrandt AIO] Champion Settings");

	if (strstr(GEntityList->Player()->ChampionName(), "Graves"))
		GravesHandler = new Graves(Menu);
	else if (strstr(GEntityList->Player()->ChampionName(), "Caitlyn"))
		CaitlynHandler = new Caitlyn(Menu);
	else if (strstr(GEntityList->Player()->ChampionName(), "Twitch"))
		TwitchHandler = new Twitch(Menu);

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
}

void ChampionHandler::_OnRender()
{
	if (strstr(GEntityList->Player()->ChampionName(), "Graves"))
		GravesHandler->OnRender();
	if (strstr(GEntityList->Player()->ChampionName(), "Caitlyn"))
		CaitlynHandler->OnRender();
	if (strstr(GEntityList->Player()->ChampionName(), "Twitch"))
		TwitchHandler->OnRender();
}

void ChampionHandler::_OnSpellCast(CastedSpell const& Args)
{
	if (strstr(GEntityList->Player()->ChampionName(), "Graves"))
		GravesHandler->OnSpellCast(Args);
	if (strstr(GEntityList->Player()->ChampionName(), "Caitlyn"))
		CaitlynHandler->OnSpellCast(Args);
	if (strstr(GEntityList->Player()->ChampionName(), "Twitch"))
		TwitchHandler->OnSpellCast(Args);
}

bool ChampionHandler::_OnPreCast(int Slot, IUnit* Target, Vec3* StartPosition, Vec3* EndPosition)
{
	//if (strstr(GEntityList->Player()->ChampionName(), "Graves"))
		//GravesHandler->OnPreCast(Slot, Target, StartPosition, EndPosition);
	if (strstr(GEntityList->Player()->ChampionName(), "Twitch"))
		return TwitchHandler->OnPreCast(Slot, Target, StartPosition, EndPosition);
	else
		return true;
}

void ChampionHandler::_OnOrbwalkAttack(IUnit* Source, IUnit* Target)
{
	if (strstr(GEntityList->Player()->ChampionName(), "Caitlyn"))
		CaitlynHandler->OnOrbwalkAttack(Source, Target);
	if (strstr(GEntityList->Player()->ChampionName(), "Twitch"))
		TwitchHandler->OnOrbwalkAttack(Source, Target);
}


void ChampionHandler::LoadEvents()
{
	GEventManager->AddEventHandler(kEventOnGameUpdate, OnGameUpdate);
	GEventManager->AddEventHandler(kEventOnSpellCast, OnSpellCast);
	GEventManager->AddEventHandler(kEventOnPreCast, OnPreCast);
	GEventManager->AddEventHandler(kEventOnRender, OnRender);
	GEventManager->AddEventHandler(kEventOrbwalkAfterAttack, OnOrbwalkAttack);
}

void ChampionHandler::UnloadEvents()
{
	GEventManager->RemoveEventHandler(kEventOnGameUpdate, OnGameUpdate);
	GEventManager->RemoveEventHandler(kEventOnSpellCast, OnSpellCast);
	GEventManager->RemoveEventHandler(kEventOnPreCast, OnPreCast);
	GEventManager->RemoveEventHandler(kEventOnRender, OnRender);
	GEventManager->RemoveEventHandler(kEventOrbwalkAfterAttack, OnOrbwalkAttack);
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
