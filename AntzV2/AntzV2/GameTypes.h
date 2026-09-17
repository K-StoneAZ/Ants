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
    bool m_DebugField = false;
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

struct Battlefield
{
    struct ExCandidate
    {
        int Row = -1;
        int Col = -1;
        int Ants = 0;
        float attackRatio = 0.0;
        int tempTstr = 0;
        float tempScore = 0.0;
    };

	//Source cells
    int sRow = -1;
    int sCol = -1;
    int sAnts = 0;
    int sBase = 0;
    int sStr = 0;
    std::vector<ExCandidate> attacks;
	//Friendly non-source cells
    int fRow = -1; 
    int fCol = -1;
    int fAnts = 0;
    int fBase = 0;
    int fStr = 0;
	//Target cells
    int tOwner = -1;
    int tRow = -1;
    int tCol = -1;
    int tAnts = 0;
    int tBase = 0;
    std::vector<ExCandidate> sources;

};
// Debug starting field: row, col, owner
int DebugField[42][3] = {
    {0,3,2},{0,5,1},{0,7,1},{0,9,2},
    {1,0,1},{1,2,1},{1,6,2},
    {2,0,1},{2,1,1},{2,6,2},
    {3,1,1},{3,2,2},{3,3,1},{3,4,2},{3,5,2},{3,6,1},
    {4,1,2},{4,2,2},{4,9,1},
    {5,0,1},{5,1,2},{5,4,2},{5,6,2},
    {6,1,2},{6,2,2},{6,5,1},{6,6,1},
    {7,0,1},{7,3,1},{7,4,1},{7,8,1},
    {8,0,2},{8,3,1},{8,4,2},{8,5,2},{8,6,2},{8,9,2},
    {9,2,1},{9,3,2},{9,4,1},{9,5,2},{9,6,1}
};

const int TitleSize = 32;
const int TextSize = 20;
extern int gTurn;

