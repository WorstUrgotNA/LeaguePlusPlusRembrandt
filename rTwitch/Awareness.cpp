#include "Awareness.h"
#include "Gui.h"
#include "Tracker.h"
#include "GankDetection.h"

Awareness::Awareness(IMenu* Parent)
{
	Menu = Parent->AddMenu("Tracker PRO");
	EnableUi = Menu->CheckBox("Enable GUI Modules:", true);
	EnableTracking = Menu->CheckBox("Enable Tracking Modules:", true);
	EnableGanks = Menu->CheckBox("Enable Awareness Modules:", true);

	if (EnableUi->Enabled())
		Ui			= new Gui(Menu);
	if (EnableTracking->Enabled())
		Tracking	= new Tracker(Menu);
	if (EnableGanks->Enabled())
		Ganks		= new GankDetection(Menu);

	LoadEvents();
}

Awareness::~Awareness()
{

	UnloadEvents();
	Menu->Remove();

	delete Ui;
}

void Awareness::_OnGameUpdate()
{
	if (Tracking && EnableTracking->Enabled())
		Tracking->OnGameUpdate();
	if (Ganks && EnableGanks->Enabled())
		Ganks->OnGameUpdate();
}

void Awareness::_OnJungleNotify(JungleNotifyData* Args)
{
	if (Ganks && EnableGanks->Enabled())
		Ganks->OnJungleNotify(Args);
}

void Awareness::_OnRender()
{
	if (Ui && EnableUi->Enabled())
		Ui->OnRender();
	if (Tracking && EnableTracking->Enabled())
		Tracking->OnRender();
	if (Ganks && EnableGanks->Enabled())
		Ganks->OnRender();
	
}

void Awareness::_OnCreateObject(IUnit* Args)
{
	if (Ui && EnableUi->Enabled())
		Ui->OnCreateObject(Args);
	if (Tracking && EnableTracking->Enabled())
		Tracking->OnCreateObject(Args);
}

void Awareness::_OnDestroyObject(IUnit* Args)
{
	if (Ui && EnableUi->Enabled())
		Ui->OnDestroyObject(Args);
	if (Tracking && EnableTracking->Enabled())
		Tracking->OnDestroyObject(Args);
}

void Awareness::_OnUnitDeath(IUnit* Args)
{
	if (Tracking && EnableTracking->Enabled())
		Tracking->OnUnitDeath(Args);
}

void Awareness::_OnProcessSpell(CastedSpell const& Args)
{
	if (Tracking && EnableTracking->Enabled())
		Tracking->OnProcessSpell(Args);
}

void Awareness::_OnTeleport(OnTeleportArgs* Args)
{
	if (Ui && EnableUi->Enabled())
		Ui->OnTeleport(Args);
	if (Ganks && EnableGanks->Enabled())
		Ganks->OnTeleport(Args);
}


void Awareness::LoadEvents()
{
	GEventManager->AddEventHandler(kEventOnGameUpdate, OnGameUpdate);
	GEventManager->AddEventHandler(kEventOnRender, OnRender);
	GEventManager->AddEventHandler(kEventOnCreateObject, OnCreateObject);
	GEventManager->AddEventHandler(kEventOnDestroyObject, OnDestroyObject);
	GEventManager->AddEventHandler(kEventOnUnitDeath, OnUnitDeath);
	GEventManager->AddEventHandler(kEventOnSpellCast, OnProcessSpell);
	GEventManager->AddEventHandler(kEventOnJungleNotification, OnJungleNotify);
	GEventManager->AddEventHandler(kEventOnTeleport, OnTeleport);
}

void Awareness::UnloadEvents()
{
	GEventManager->RemoveEventHandler(kEventOnGameUpdate, OnGameUpdate);
	GEventManager->RemoveEventHandler(kEventOnRender, OnRender);
	GEventManager->RemoveEventHandler(kEventOnCreateObject, OnCreateObject);
	GEventManager->RemoveEventHandler(kEventOnDestroyObject, OnDestroyObject);
	GEventManager->RemoveEventHandler(kEventOnUnitDeath, OnUnitDeath);
	GEventManager->RemoveEventHandler(kEventOnSpellCast, OnProcessSpell);
	GEventManager->RemoveEventHandler(kEventOnJungleNotification, OnJungleNotify);
	GEventManager->RemoveEventHandler(kEventOnTeleport, OnTeleport);
}

PLUGIN_EVENTD(void) Awareness::OnTeleport(OnTeleportArgs* Args)
{
	GPluginInstance->_OnTeleport(Args);
}

PLUGIN_EVENTD(void) Awareness::OnGameUpdate()
{
	GPluginInstance->_OnGameUpdate();
}

PLUGIN_EVENTD(void) Awareness::OnJungleNotify(JungleNotifyData* Args)
{
	GPluginInstance->_OnJungleNotify(Args);
}
PLUGIN_EVENTD(void) Awareness::OnRender()
{
	GPluginInstance->_OnRender();
}

PLUGIN_EVENTD(void) Awareness::OnCreateObject(IUnit* Args)
{
	GPluginInstance->_OnCreateObject(Args);
}

PLUGIN_EVENTD(void) Awareness::OnDestroyObject(IUnit* Args)
{
	GPluginInstance->_OnDestroyObject(Args);
}

PLUGIN_EVENTD(void) Awareness::OnUnitDeath(IUnit* Args)
{
	GPluginInstance->_OnUnitDeath(Args);
}

PLUGIN_EVENTD(void) Awareness::OnProcessSpell(CastedSpell const& Args)
{
	GPluginInstance->_OnProcessSpell(Args);
}