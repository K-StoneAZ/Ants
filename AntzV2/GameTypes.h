#pragma once
// GameTypes.h  Win32 AntzV2 Game
#include <string>
#include <vector>

enum AppState
{
    APP_TITLE,
    APP_SETUP,
    APP_GAME,
    APP_WIN,
    APP_GAMEOVER
};

enum Trait
{
    None,
    Aggressive,
    Defensive,
    Balanced
};

struct Persona
{
    Trait growth = None;
    Trait attack = None;
    Trait move = None;
};

struct PlayerConfig
{
    bool m_isHuman = true;
    std::string m_PlayerName = "";
    std::wstring m_personaName = L"";
    Persona m_persona;
};

struct GameConfig
{
    int m_FieldSize = 10;
    int m_ActivePlayers = 2;
    int m_Difficulty = 1;
    int m_StartCells = 15;
    int m_AttackPerTurn = 2;
};

enum class WinType
{
    Domination,
	Elimination
};

enum class GameSpeed
{
    Half,
    Normal,
    Fast,
    Fastest
};

struct GameResult
{
    bool m_gameOver = false;
    int m_winner = 0;
    WinType m_winType = WinType::Domination;
};

struct PlayerData {

	int m_playerID{ 0 };
	std::string m_playerName{ "" };
	int m_playertype{ 0 }; // 0 for reserved, 1 for human, 2 for AI
    Persona m_persona;

	int m_cells_owned{ 0 };
	int srow{ 0 }; // source row
	int scol{ 0 }; // source col
	int trow{ 0 }; // target row
	int tcol{ 0 }; // target col
	int sant{ 0 }; // source ants
	int tant{ 0 }; // target ants
    int tplayerID{ 0 }; // Target player id
    bool success = false; // AI attack success/fail
    bool selectS = false;  //Source Selected
    bool selectT = false;  //Target Selected
    int attackRoll = 0;
    int defenseRoll = 0;
    int battleRound = 0;
};

enum DialogMode
{
    DIALOG_NONE,
    DIALOG_MESSAGE,
    DIALOG_GROWTH,
	DIALOG_ATTACK,
	DIALOG_MOVE,
    DIALOG_CONFIRMATION,
    DIALOG_YESNO
};

struct DialogState
{
    DialogMode m_mode = DIALOG_MESSAGE;

    std::string m_message = "";

    // Used by text/number input
    std::string m_inputText = "";

    // Future button control
    bool m_showOK = false;
    bool m_showCancel = false;
    bool m_showYes = false;
    bool m_showNo = false;
};

enum class Timer
{
    Battle,
    AI
};

const int TitleSize = 32;
const int TextSize = 20;
