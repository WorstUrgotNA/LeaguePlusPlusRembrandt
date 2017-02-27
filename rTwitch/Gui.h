#pragma once

#include "PluginSDK.h"

struct HeroUI
{
	IUnit*		Player;
	ITexture*	ChampionIcon;
	ITexture*	SummonerSpellIcons[2];
	ITexture*	SummonerSpellIconsSmall[2];
	ITexture*	SummonerSpellIconsCircle[2];
	ITexture*	SpellIcons[4];
	float		RespawnTime;
	bool		RespawnNotification;
	bool		UltimateNotification;
	char		ChampionName[64];
	bool		Valid;
};

struct UiFonts
{
	IFont* DeathFont;
	IFont* CooldownFont;
	IFont* SummonerCooldownFont3D;
	IFont* HudFont;
	IFont* HudFont2;
};

struct UiTextures
{
	ITexture* OutlineLeft;
	ITexture* OutlineRight;
	ITexture* EmptyLeft;
	ITexture* EmptyRight;
	ITexture* HUD1_icons;
	ITexture* HUD1_champ;
	ITexture* HUD1_bar;
	ITexture* HUD1_iconsleft;
	ITexture* HUD1_champleft;
	ITexture* HUD1_barleft;
	ITexture* HUD1_hp;
	ITexture* HUD1_mp;

	ITexture* SmallOutline;
	ITexture* UltimateReady;

	ITexture* Health;
	ITexture* Mana;
	ITexture* CDHud;
	ITexture* CDHudSelf;

	ITexture* UN_bg;
	ITexture* UN_r;

	ITexture* OKTW_circle;
	ITexture* OKTW_bar;
};

struct UiMenu
{
	IMenu* Owner;
	IMenu* GUI2D;
	IMenu* GUI3D;
	IMenuOption* Enabled;
	IMenuOption* Show2DHud;
	IMenuOption* Show3DHud;
	IMenuOption* ShowSelf;
	IMenuOption* ShowTeam;
	IMenuOption* ShowEnemies;
	IMenuOption* ShowSelf3D;
	IMenuOption* ShowTeam3D;
	IMenuOption* ShowEnemies3D;
	IMenuOption* TeamOnLeft;
	IMenuOption* NotifyOnRespawn;
	IMenuOption* NotifyOnUltimate;
	IMenuOption* MinimalisticHud;
	IMenuOption* XOffset;
	IMenuOption* YOffset;
	IMenuOption* RadiusOffset;
	IMenuOption* Resize;
	IMenuOption* DrawHPBarText;
	IMenuOption* DrawManaBarText;
	IMenuOption* HPBarTextStyle;
	IMenuOption* ManaBarTextStyle;
};

class Gui
{
public:
	Gui(IMenu* Parent);
	~Gui();

	void OnRender();
	void OnCreateObject(IUnit* Args);
	void OnDestroyObject(IUnit* Args);
	void OnD3DPostReset();

private:
	void GetLatestVersionNo();

	void LoadMenu();
	void LoadFonts();
	void LoadTextureIcons();
	void LoadHeroIcon(HeroUI* Ui);
	void LoadSpellIcons(HeroUI* Ui);
	bool DoesTextureExist(std::string const& Filename, std::string& FullPath);
	ITexture* CreateTextureEx(std::string const& Filename, std::string const& DownloadUrl);

	void RenderTeammates();
	void RenderEnemies();
	void RenderPlayer2DElementBig(HeroUI* Ui, Vec2 const& Position, bool Left);
	void RenderPlayer2DElementSmall(HeroUI* Ui, Vec2 const& Position);
	void RenderPlayer3DElement(HeroUI* Ui);

	void UpdateChampions();
	float GetRespawnTime(IUnit* Player);

private:
	UiMenu					Menu;
	std::vector<HeroUI*>	Teammates;
	std::vector<HeroUI*>	Enemies;
	std::vector<HeroUI*>	UiPool;
	UiFonts*				Fonts;
	UiTextures*				Textures;
	std::string				VersionNo;
	std::string				VersionOutput;
	Vec2					Resolution;
	bool					DownloadIsComplete;
	bool					ReSizeNeeded;
};
