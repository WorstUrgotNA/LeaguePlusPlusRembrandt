#include "ChampionHandler.h"
// CHAMPION LIST
#include "Graves.h"


ChampionHandler::ChampionHandler()
{
	Menu = GPluginSDK->AddMenu("Champion Settings");

	//if (strstr(GEntityList->Player()->ChampionName(), "Graves"))
		//GravesHandler = new Graves(Menu);

	LoadEvents();
}

ChampionHandler::~ChampionHandler()
{
	UnloadEvents();
	Menu->Remove();
}

void ChampionHandler::_OnGameUpdate()
{
	//if (strstr(GEntityList->Player()->ChampionName(), "Graves"))
		//GravesHandler->OnGameUpdate();
}

void ChampionHandler::_OnSpellCast(CastedSpell const& Args)
{
	//if (strstr(GEntityList->Player()->ChampionName(), "Graves"))
		//GravesHandler->OnSpellCast(Args);
}

void ChampionHandler::LoadEvents()
{
	GEventManager->AddEventHandler(kEventOnGameUpdate, OnGameUpdate);
	GEventManager->AddEventHandler(kEventOnSpellCast, OnSpellCast);
}

void ChampionHandler::UnloadEvents()
{
	GEventManager->RemoveEventHandler(kEventOnGameUpdate, OnGameUpdate);
	GEventManager->RemoveEventHandler(kEventOnSpellCast, OnSpellCast);
}

PLUGIN_EVENTD(void) ChampionHandler::OnGameUpdate()
{
	GPluginInstance3->_OnGameUpdate();
}

PLUGIN_EVENTD(void) ChampionHandler::OnSpellCast(CastedSpell const& Args)
{
	GPluginInstance3->_OnSpellCast(Args);
}