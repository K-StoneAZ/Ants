#pragma once
// Game.h  Win32 AntzV2 Game
#include <windows.h>
#include <string>
#include <vector>
#include "GameTypes.h"
#include "TextBox.h"
#include "Field.h"
#include "Renderer.h"
#include "AIcontrol.h"


using RepaintCallback = void(*)();
using TimerCallback = void(*)(Timer, GameSpeed);
using GameWinCallback = void(*)(const GameResult&);
class Game
{
private:

    enum TurnState
    {
        T_START,
        T_GROWTH,
        T_MOVE,
        T_ATTACK,
        T_END,
        T_WIN
    };

    TurnState m_turnState = T_START;

    enum ClickType
    {
        CLICK_NONE,
        CLICK_FIELD,
        CLICK_CONFIRM,
        CLICK_CANCEL,
        CLICK_SKIP
    };

    ClickType m_clicked = CLICK_NONE;
	GameConfig m_config;
    GameResult m_result;
    GameSpeed m_gameSpeed = GameSpeed::Normal;
    std::vector<PlayerConfig> m_playerConfig;
    std::vector<PlayerData> m_players;
    Field m_field;
	Renderer m_renderer;
    DialogState m_dialog;
    TextBox m_antBox;
    AIcontrol m_aiC;
    RepaintCallback m_repaint = nullptr;
    TimerCallback m_startTimer = nullptr;
    TimerCallback m_stopTimer = nullptr;
    GameWinCallback m_gameWin = nullptr;

    int m_hoverRow = -1;
    int m_hoverCol = -1;
    int m_selectedRow = -1;
    int m_selectedCol = -1;
    int m_activePlayer = 1;
	int m_winCount = 0;
    int m_successfulCount = 0;
    int m_aiStep = 0;// Which AI action/state is active
    int m_tickCount = 0;// Number of AI_TIMER callbacks
    int m_delay = 0;// Number of ticks required before advancing
    std::string m_error = "";
	 

    void CalculateGrowth(PlayerData& player)
    {
        player.sant = 1 + player.m_cells_owned / 3;
    }
    bool CheckWin()
    {
        int livingPlayers = 0;
        int livingPlayer = 0;

        for (int i = 1; i < m_players.size(); i++)
        {
            if (m_players[i].m_cells_owned >= m_winCount) 
            { m_result.m_winner = i; m_result.m_winType = WinType::Domination; return true; }

            if (m_players[i].m_cells_owned > 0) 
            { 
                livingPlayers++; 
                livingPlayer = i;
            }
        }

        if (livingPlayers == 1) 
        {
            m_result.m_winner = livingPlayer;
			m_result.m_winType = WinType::Elimination;
            return true; }

        return false;
    }

    bool AIWaitComplete()
    {
        m_tickCount++;

        if (m_tickCount >= m_delay)
        {
            m_tickCount = 0;
            return true;
        }

        return false;
    }

public:
    Game() : m_aiC(m_players) {}

    void Initialize(HDC hdc)
    {
        m_renderer.Initialize(hdc);
    }
    void SetRepaintCallback(RepaintCallback callback)
    {
        m_repaint = callback;
    }

    void RequestRepaint()
    {
        if (m_repaint){ m_repaint(); }
    }
    void SetTimerCallbacks( TimerCallback startTimer, TimerCallback stopTimer)
    {
        m_startTimer = startTimer;
        m_stopTimer = stopTimer;
    }
    void SetGameWinCallback(GameWinCallback callback)
    {
        m_gameWin = callback;
    }
    void StartTimer(Timer timer)
    {
        if (m_startTimer)
        {
            m_startTimer(timer, m_gameSpeed);
        }
    }
    void StopTimer(Timer timer)
    {
        if (m_stopTimer)
        {
            m_stopTimer(timer, m_gameSpeed);
        }
    }

    void ChangeSpeed()
    {
        switch (m_gameSpeed)
        {
        case GameSpeed::Half:
            m_gameSpeed = GameSpeed::Normal;
            break;

        case GameSpeed::Normal:
            m_gameSpeed = GameSpeed::Fast;
            break;

        case GameSpeed::Fast:
            m_gameSpeed = GameSpeed::Fastest;
            break;

        case GameSpeed::Fastest:
            m_gameSpeed = GameSpeed::Half;
            break;
        }
        RequestRepaint();
    }

    void NotifyGameWin()
    {
        if (m_gameWin)
        {
            m_gameWin(m_result);
        }
    }

    GameResult GetResult() const { return m_result; }

    void SetupGame(const GameConfig& config, const std::vector<PlayerConfig>& players)
    {
        m_config = config;
        m_playerConfig = players;

        m_dialog.m_mode = DIALOG_MESSAGE;
        m_dialog.m_message = "Testing";
        m_dialog.m_inputText.clear();
        m_dialog.m_showOK = false;
        m_dialog.m_showCancel = false;
        m_dialog.m_showYes = false;
        m_dialog.m_showNo = false;
        m_players.clear();

        // Set game defaults
        m_clicked = CLICK_NONE;
        m_hoverRow = -1;
        m_hoverCol = -1;
        m_selectedRow = -1;
        m_selectedCol = -1;
        m_activePlayer = 1;
        m_winCount = 0;
        m_successfulCount = 0;
        m_aiStep = 0;
        m_tickCount = 0;
        m_delay = 0;
        m_result = GameResult{};
        m_turnState = T_START;

        // Player 0 is always reserved
        PlayerData reserved;

        reserved.m_playerID = 0;
        reserved.m_playerName = "Unowned";
        reserved.m_playertype = 0;

        m_players.push_back(reserved);
        // Create active players
        for (int i = 1; i < players.size(); i++)
        {
            PlayerData player;

            player.m_playerID = i;
            player.m_playerName = players[i].m_PlayerName;
            player.m_persona = players[i].m_persona;

            if (players[i].m_isHuman)
            {
                player.m_playertype = 1;
            }
            else
            {
                player.m_playertype = 2;
            }

            player.m_cells_owned = 0;
            player.srow = 0;
            player.scol = 0;
            player.trow = 0;
            player.tcol = 0;
            player.sant = 0;
            player.tant = 0;
            player.selectS = false;
            player.selectT = false;

            m_players.push_back(player);

        }
        // Build the game board
        m_field.Initialize(m_config);
        int totalCells = m_config.m_FieldSize * m_config.m_FieldSize;
        m_winCount = (totalCells * 2) / 3;
        m_dialog.m_mode = DIALOG_MESSAGE;
        m_dialog.m_message = "Game Ready";
        m_error = "";

        RequestRepaint();
    }

void UpdateAIturn()
    {

    if (!AIWaitComplete()) { return; }

    PlayerData& player = m_players[m_activePlayer];

    switch (m_turnState)
    {
    case T_GROWTH:
    {
        switch (m_aiStep)
        {
        case 0:
            // Think

            m_aiC.Growth(m_field, m_activePlayer, player.sant);

            player.srow = m_aiC.GetSourceRow();
            player.scol = m_aiC.GetSourceCol();
            player.trow = m_aiC.GetTargetRow();
            player.tcol = m_aiC.GetTargetCol();
            player.tant = 0;

            player.selectS = true;
            player.selectT = true;

            m_field.SetVisualMode(
                player.trow,
                player.tcol,
                Field::V_GROWTH);

            RequestRepaint();

            m_aiStep++;
            m_tickCount = 0;
            m_delay = 3;   // wait ticks before showing amount
            return;

        case 1:
            // Show amount
            player.tant = m_aiC.GetTargetAnts();

            RequestRepaint();

            m_aiStep++;
            m_tickCount = 0;
            m_delay = 3;   // wait before applying growth
            return;

        case 2:
            // Apply growth
            ApplyGrowth(player.tant);

            RequestRepaint();

            m_aiStep++;
            m_tickCount = 0;
            m_delay = 2;   // wait before cleanup
            return;

        case 3:
            // Cleanup
            m_field.SetVisualMode(
                player.trow,
                player.tcol,
                Field::V_HOVER);

            player.selectT = false;
            player.selectS = false;
            player.success = false;
            m_aiStep = 0;
            m_tickCount = 0;
            m_delay = 4;

            m_dialog.m_mode = DIALOG_ATTACK;
            m_turnState = T_ATTACK;

            return;
        }
        return;
    }
    
    case T_ATTACK:
    {
        switch (m_aiStep)
        {
        case 0:

        if (player.success)
        {
            m_aiC.ExAttack(m_field, m_activePlayer);
			player.success = false;



            if (m_aiC.GetSourceRow() == -1)
            {

                // No more attacks available/Forced Skip
                m_field.ClearAllVisualModes();
				m_field.SetPlayerVisualMode(m_activePlayer);
				m_dialog.m_mode = DIALOG_MOVE;
                m_turnState = T_MOVE;
                m_aiStep = 0;
                m_tickCount = 0;
                m_delay = 4;
                return;
            }
 
        }
        else 
        {
            m_aiC.Attack(m_field, m_activePlayer);
        }

            player.srow = m_aiC.GetSourceRow();
            player.scol = m_aiC.GetSourceCol();
            player.selectS = true;
            player.sant = m_field.getAnts(player.srow, player.scol) - 1; // available ants.

            m_field.SetVisualMode(
                player.srow,
                player.scol,
                Field::V_ATTACK_SOURCE);

            RequestRepaint();

            m_aiStep++;
            m_tickCount = 0;
            m_delay = 3;   // wait ticks
            return;

        case 1:

            player.trow = m_aiC.GetTargetRow();
            player.tcol = m_aiC.GetTargetCol();
            player.selectT = true;

            m_field.SetVisualMode(
                player.trow,
                player.tcol,
                Field::V_ATTACK_TARGET);

            RequestRepaint();

            m_aiStep++;
            m_tickCount = 0;
            m_delay = 3;

            return;
        
        case 2:
            // committed ants
            player.tant = m_aiC.GetSourceAnts();

            RequestRepaint();

            m_aiStep++;
            m_tickCount = 0;
            m_delay = 2;

            return;

        case 3:
            // setup battle refresh HUD
            m_field.setAnts( player.srow, player.scol,
                m_field.getAnts(player.srow, player.scol) - player.tant);

            player.tplayerID = m_field.getOwner(player.trow, player.tcol);

            player.tant = m_field.getAnts(player.trow, player.tcol);

            m_aiStep++;
            m_tickCount = 0;
            m_delay = 3;

            return;

        case 4:
            // Execute the attack sequence.
            m_renderer.SetBattleActive(true);
            StopTimer(Timer::AI);
            StartTimer(Timer::Battle);

            m_aiStep = 0;
            m_tickCount = 0;
            m_delay = 3;
            return;
        }
        return;
    }
    case T_MOVE:
        switch (m_aiStep)
        {

        case 0:

            m_aiC.Move(m_field, m_activePlayer);

            if (m_aiC.GetSourceRow() == -1)
            {
                // Forced Skip
                m_turnState = T_END;
                m_aiStep = 0;
                m_tickCount = 0;
                m_delay = 4;
                return;
            }

            m_field.SetPlayerVisualMode(m_activePlayer);
            m_dialog.m_mode = DIALOG_MOVE;

            player.srow = m_aiC.GetSourceRow();
            player.scol = m_aiC.GetSourceCol();
            player.selectS = true;
            player.sant = m_aiC.GetSourceAnts(); // available ants.
            m_field.SetVisualMode( player.srow, player.scol,
                Field::V_MOVE_SOURCE);

            RequestRepaint();

            m_aiStep++;
            m_tickCount = 0;
            m_delay = 3;

            return;

        case 1:
            player.trow = m_aiC.GetTargetRow();
            player.tcol = m_aiC.GetTargetCol();
            player.selectT = true;

            m_field.SetVisualMode( player.trow, player.tcol,
                Field::V_MOVE_TARGET);

            RequestRepaint();

            m_aiStep++;
            m_tickCount = 0;
            m_delay = 3;

            return;

        case 2:
            player.tant = m_aiC.GetTargetAnts();

            RequestRepaint();

            m_aiStep++;
            m_tickCount = 0;
            m_delay = 2;

            return;

        case 3:
            ApplyMove(player.tant);
            m_field.ClearAllVisualModes();
            ClearHUDState();

            m_turnState = T_END;
            m_aiStep = 0;
            m_tickCount = 0;
            m_delay = 4;

            return;

        }

    case T_END:

		StopTimer(Timer::AI);
        // Finish the AI turn.
        // Return control to the normal game state machine.
        UpdateTurn();
        return;

    }
    }

void UpdateTurn()
    {
        switch (m_turnState)
        {
        case T_START:
            for (int i = 0; i < static_cast<int>(m_players.size()); i++)
            {
                m_players[i].m_cells_owned =
                    m_field.CountPlayerCells(i);
                m_field.setCellstr(i);
           }

            m_players[m_activePlayer].tant = 0;
            CalculateGrowth(m_players[m_activePlayer]);

            m_successfulCount = 0;
            m_players[m_activePlayer].selectS = true;
            m_players[m_activePlayer].selectT = false;
            m_field.SetPlayerVisualMode(m_activePlayer);

            m_turnState = T_GROWTH;
            m_dialog.m_mode = DIALOG_GROWTH;

            m_aiStep = 0;
            m_tickCount = 0;
            m_delay = 4;
            RequestRepaint();
           
            if (m_players[m_activePlayer].m_playertype == 2)
            {
                m_aiStep = 0;
                StartTimer(Timer::AI);
                return;
            }

            RECT antRect = m_renderer.GetAnt();
            m_antBox.SetRect( antRect.left, antRect.top,
                antRect.right - antRect.left, antRect.bottom - antRect.top);
            m_antBox.SetNumbersOnly(true);
            m_antBox.SetActive(true);
            return;
        case T_GROWTH:
            // Human waits for HandleGrowthClick()
            // AI handled by UpdateAITurn()

            if (m_players[m_activePlayer].sant <= 0)
            {
                m_turnState = T_ATTACK;
                UpdateTurn();
            }
            return;
        case T_ATTACK:

                m_dialog.m_mode = DIALOG_ATTACK;
                m_field.ClearAllVisualModes();
                m_field.SetPlayerVisualMode(m_activePlayer);
                m_antBox.SetText("");
                m_antBox.SetNumbersOnly(true);
                m_antBox.SetActive(true);

                RequestRepaint();
            return;
        case T_MOVE:

                m_dialog.m_mode = DIALOG_MOVE;

                m_field.ClearAllVisualModes();
                m_field.SetPlayerVisualMode(m_activePlayer);

                m_antBox.SetText("");
                m_antBox.SetNumbersOnly(true);
                m_antBox.SetActive(true);

                RequestRepaint();
            return;
        case T_END:
        {
                int tempPlayer = m_activePlayer;
           do
            {
                // Advance to the next player.
                if (m_activePlayer == m_players.size() - 1)
                {
                    m_activePlayer = 1;

                    // End of round.
                    if (CheckWin())
                    {
                        m_turnState = T_WIN;
                        break;
                    }
                }
                else { m_activePlayer++; }

                // Safety guard.
                if (m_activePlayer == tempPlayer)
                {
                    m_turnState = T_WIN;
                    break;
                }

            } while (m_players[m_activePlayer].m_cells_owned == 0);

            if (m_turnState == T_WIN)
            {
                RequestRepaint();
                UpdateTurn();
                return;
            }

            m_turnState = T_START;

            RequestRepaint();
            UpdateTurn();

            return;
        }
        case T_WIN:
        {
			m_result.m_gameOver = true;

            StopTimer(Timer::AI);
            StopTimer(Timer::Battle);

            m_field.ClearAllVisualModes();
			ClearHUDState();
            m_antBox.SetText("");
            m_antBox.SetActive(false);

            m_dialog.m_mode = DIALOG_NONE;

            if (m_gameWin)
            {
                m_gameWin(m_result);
            }
            return;
        }
        default:
            break;
        }
}

void ClearHUDState()
{
    m_players[m_activePlayer].sant = 0;
    m_players[m_activePlayer].tant = 0;

    m_players[m_activePlayer].selectS = false;
    m_players[m_activePlayer].selectT = false;

    m_players[m_activePlayer].srow = -1;
    m_players[m_activePlayer].scol = -1;
    m_players[m_activePlayer].trow = -1;
    m_players[m_activePlayer].tcol = -1;

    m_error.clear();
    m_antBox.SetText("");
    m_antBox.SetActive(false);
}

void ClearBattle()
{
    PlayerData& player = m_players[m_activePlayer];

    player.attackRoll = 0;
    player.defenseRoll = 0;
    player.battleRound = 0;
}

void ApplyGrowth(int amount)
{
    int row = m_players[m_activePlayer].trow;
    int col = m_players[m_activePlayer].tcol;

    int ants = m_field.getAnts(row, col);

        m_field.setAnts(row, col, ants + amount);

        int newAnts = m_field.getAnts(row, col);

        m_players[m_activePlayer].sant -= amount;

}

void UpdateAttack()
{
    PlayerData& player = m_players[m_activePlayer];

    // Unowned cell
    int bonus = player.sant - player.tant;

    if (m_field.getOwner(player.trow, player.tcol) == 0)
    {
        // Automatic capture
        m_field.setOwner(player.trow, player.tcol, m_activePlayer);
        m_field.setAnts(player.trow, player.tcol, player.sant);
        player.success = true;
        m_successfulCount++;
    }
    else
    {
        // One battle tick here
        player.attackRoll = 1 + (gRNG() % 100) + bonus;
            if (player.attackRoll > 100) { player.attackRoll = 100; }
        player.defenseRoll = 1 + (gRNG() % 100);

        //Battle

        if (player.attackRoll > player.defenseRoll)
        {
            player.tant--;
        }
        else
        {
            player.sant--;
        }

        RequestRepaint();

        // If battle ended:
        if (player.sant <= 0)
        {
            // attack failed
            StopTimer(Timer::Battle);
            m_renderer.SetBattleActive(false);
            m_dialog.m_mode = DIALOG_MOVE;
            m_turnState = T_MOVE;
            m_field.setAnts( player.trow, player.tcol, player.tant);
            ClearBattle();
            ClearHUDState();
            m_field.ClearAllVisualModes();
            player.success = false;

            if (player.m_playertype != 1) { StartTimer(Timer::AI); return; }
            UpdateTurn();
            return;
        }
        if (player.tant <= 0)
        {
            // attack succeeded
            // Capture target
            m_field.setOwner(player.trow, player.tcol, m_activePlayer);
            m_field.setAnts(player.trow, player.tcol, player.sant);
            player.success = true;
            m_successfulCount++;

        }
        else
        {
            return;
        }
    }
    // Successful attack handling

    StopTimer(Timer::Battle);

    if (m_successfulCount >= m_config.m_AttackPerTurn)
    {
        for (int i = 0; i < static_cast<int>(m_players.size()); i++)
        {
            m_players[i].m_cells_owned =
                m_field.CountPlayerCells(i);
            m_field.setCellstr(i);
        }

        ClearBattle();
        ClearHUDState();
        m_renderer.SetBattleActive(false);
        m_field.ClearAllVisualModes();
        m_dialog.m_mode = DIALOG_MOVE;
        m_turnState = T_MOVE;
        player.success = false;

        if (player.m_playertype != 1) { StartTimer(Timer::AI); return; }
        UpdateTurn();
    }
    else
    {
        for (int i = 0; i < static_cast<int>(m_players.size()); i++)
        {
            m_players[i].m_cells_owned =
                m_field.CountPlayerCells(i);
            m_field.setCellstr(i);
        }

        ClearBattle();
        ClearHUDState();
        m_field.SetPlayerVisualMode(m_activePlayer);
        m_dialog.m_mode = DIALOG_ATTACK;

        if (player.m_playertype != 1) { StartTimer(Timer::AI); return; }

        m_antBox.SetNumbersOnly(true);
        m_antBox.SetActive(true);
        m_field.SetPlayerVisualMode(m_activePlayer);
    }

    return;
}

void ApplyMove(int amount)
{
    PlayerData& player = m_players[m_activePlayer];

    m_field.setAnts(
        player.srow,
        player.scol,
        m_field.getAnts(player.srow, player.scol) - amount);

    m_field.setAnts(
        player.trow,
        player.tcol,
        m_field.getAnts(player.trow, player.tcol) + amount);
}

void HandleGrowthClick(ClickType click, int row = -1, int col = -1)
{
    if (click == CLICK_FIELD)
    {
        if (m_field.getOwner(row, col) != m_activePlayer)
        {
            m_error = "That cell is not yours.";
            RequestRepaint();

            return;
        }
        if (m_players[m_activePlayer].selectT)
        {
            m_field.SetVisualMode( m_players[m_activePlayer].trow,
                m_players[m_activePlayer].tcol, Field::V_HOVER);
        }

        m_error.clear();
        m_players[m_activePlayer].tant = 0;
        m_players[m_activePlayer].trow = row;
        m_players[m_activePlayer].tcol = col;
        m_players[m_activePlayer].selectT = true;
        m_field.SetVisualMode(row, col, Field::V_GROWTH);
        RequestRepaint();
        return;
    }
    else if (click == CLICK_CONFIRM)
    {

        if (!m_players[m_activePlayer].selectT)
        {
            m_error = "Select a target cell.";
            RequestRepaint();
            return;
        }
        std::string text = m_antBox.GetText();
        int amount = 0;
        if (!text.empty())
        {
            amount = std::stoi(text);
        }
        if (amount <= 0)
        {
            m_error = "Growth amount must be greater than zero.";
            RequestRepaint();
            return;
        }
        if (amount > m_players[m_activePlayer].sant)
        {
            m_error = "Not enough ants available.";
            RequestRepaint();
            return;
        }    

    // All validation passed
         m_error.clear();

        ApplyGrowth(amount);

            // Clear current selection
        m_field.SetVisualMode(m_players[m_activePlayer].trow,
            m_players[m_activePlayer].tcol, Field::V_HOVER);

        m_players[m_activePlayer].selectT = false;
        m_players[m_activePlayer].trow = -1;
        m_players[m_activePlayer].tcol = -1;

        m_antBox.SetText("");

        if (m_players[m_activePlayer].sant > 0)
        {
            // Continue growth phase
        }
        else
        {
            // Growth complete
            m_field.ClearAllVisualModes();
            ClearHUDState();
            m_turnState = T_ATTACK;
            UpdateTurn();
        }
    RequestRepaint();
    return;
        
    }
    else if (click == CLICK_CANCEL)
    {
        if (m_players[m_activePlayer].selectT)
        {
            m_field.SetVisualMode(m_players[m_activePlayer].trow,
                m_players[m_activePlayer].tcol, Field::V_HOVER);
        }

        m_antBox.SetText("");
        m_error.clear();
        m_players[m_activePlayer].selectT = false;
        m_players[m_activePlayer].trow = -1;
        m_players[m_activePlayer].tcol = -1;

        RequestRepaint();
    }
    return;
}

void HandleAttackClick(ClickType click, int row = -1, int col = -1)
{
    if (click == CLICK_FIELD)
    {
        PlayerData& player = m_players[m_activePlayer];

        // Selecting source
        if (!player.selectS)
        {
            if (m_field.getOwner(row, col) != m_activePlayer)
            {
                m_error = "That cell is not yours.";
                RequestRepaint();
                return;
            }

            if (m_field.getAnts(row, col) <= 1)
            {
                m_error = "Not enough ants to attack.";
                RequestRepaint();
                return;
            }

            player.srow = row;
            player.scol = col;
            player.sant = m_field.getAnts(row, col) - 1;   // ants available to send
            player.tant = 0;
            player.selectS = true;
            m_field.ClearPlayerVisualMode(m_activePlayer);
            m_field.SetVisualMode(row, col, Field::V_ATTACK_SOURCE);
            //Finding enemy cells
            Field::Area area = m_field.getArea(row, col);

            for (int r = area.rmin; r <= area.rmax; r++)
            {
                for (int c = area.cmin; c <= area.cmax; c++)
                {
                    if (r == row && c == col)
                        continue;

                    if (m_field.getOwner(r, c) == m_activePlayer)
                        continue;

                    m_field.SetVisualMode(r, c, Field::V_HOVER);
                }
            }

            m_error.clear();
            RequestRepaint();
            return;
        }
        // Selecting target
        if (player.selectS && !player.selectT)
        {
            if (row == player.srow && col == player.scol)
            {
                m_error = "Cannot attack yourself.";
                RequestRepaint();
                return;
            }
            if (m_field.GetCell(row, col).visual != Field::V_HOVER)
            {
                m_error = "Invalid Cell!";
                RequestRepaint();
                return;
            }

            player.trow = row;
            player.tcol = col;
            player.selectT = true;
            m_field.ClearVisualModeType(Field::V_HOVER);
            m_field.SetVisualMode(row, col,
                Field::V_ATTACK_TARGET);

            m_error.clear();
            RequestRepaint();
            return;
        }
    }

        if (click == CLICK_CONFIRM)
        {
            PlayerData& player = m_players[m_activePlayer];

            if (!player.selectS)
            {
                m_error = "Select an attack source.";
                RequestRepaint();
                return;
            }

            if (!player.selectT)
            {
                m_error = "Select an attack target.";
                RequestRepaint();
                return;
            }

            std::string text = m_antBox.GetText();

            int amount = 0;

            if (!text.empty())
            {
                amount = std::stoi(text);
            }

            if (amount <= 0)
            {
                m_error = "Attack amount must be greater than zero.";
                RequestRepaint();
                return;
            }

            if (amount > player.sant)
            {
                m_error = "Not enough ants available.";
                RequestRepaint();
                return;
            }

            m_field.setAnts( player.srow, player.scol,
                m_field.getAnts(player.srow, player.scol) - amount);

            player.sant = amount;

            player.tplayerID = m_field.getOwner(player.trow, player.tcol);
            player.tant = m_field.getAnts(player.trow, player.tcol);

            // Input no longer needed
            m_antBox.SetText("");
            m_antBox.SetActive(false);

            player.selectS = false;
            player.selectT = false;

            m_error.clear();

            StartTimer(Timer::Battle);
            m_renderer.SetBattleActive(true);
            RequestRepaint();
            return;
        }

        if (click == CLICK_CANCEL)
        {
            PlayerData& player = m_players[m_activePlayer];

            m_field.ClearAllVisualModes();
            m_field.SetPlayerVisualMode(m_activePlayer);
            player.selectS = false;
            player.selectT = false;

            player.srow = -1;
            player.scol = -1;
            player.trow = -1;
            player.tcol = -1;
            player.sant = 0;
            player.tant = 0;
            m_antBox.SetText("");
            m_error.clear();

            RequestRepaint();
            return;
        }

        if (click == CLICK_SKIP)
        {
            m_field.ClearAllVisualModes();
            ClearHUDState();
            ClearBattle();
            m_renderer.SetBattleActive(false);
            m_turnState = T_MOVE;

            UpdateTurn();
            return;
        }
    }

    void HandleMoveClick(ClickType click, int row = -1, int col = -1)
    {
        if (click == CLICK_FIELD)
        {
            PlayerData& player = m_players[m_activePlayer];

            // Selecting source
            if (!player.selectS)
            {
                if (m_field.getOwner(row, col) != m_activePlayer)
                {
                    m_error = "That cell is not yours.";
                    RequestRepaint();
                    return;
                }

                if (m_field.getAnts(row, col) <= 1)
                {
                    m_error = "Not enough ants to move.";
                    RequestRepaint();
                    return;
                }

                player.srow = row;
                player.scol = col;
                player.sant = m_field.getAnts(row, col) - 1;   // ants available to send
                player.tant = 0;
                player.selectS = true;
                m_field.ClearPlayerVisualMode(m_activePlayer);
                m_field.SetVisualMode(row, col, Field::V_MOVE_SOURCE);
                //Finding cells
                Field::Area area = m_field.getArea(row, col);

                for (int r = area.rmin; r <= area.rmax; r++)
                {
                    for (int c = area.cmin; c <= area.cmax; c++)
                    {
                        if (r == row && c == col)
                            continue;

                        if (m_field.getOwner(r, c) != m_activePlayer)
                            continue;

                        m_field.SetVisualMode(r, c, Field::V_HOVER);
                    }
                }

                m_error.clear();
                RequestRepaint();
                return;
            }
            // Selecting target
            if (player.selectS && !player.selectT)
            {
                if (row == player.srow && col == player.scol)
                {
                    m_error = "Cannot move to source cell.";
                    RequestRepaint();
                    return;
                }
                if (m_field.GetCell(row, col).visual != Field::V_HOVER)
                {
                    m_error = "Invalid Cell!";
                    RequestRepaint();
                    return;
                }

                player.trow = row;
                player.tcol = col;
                player.selectT = true;
                m_field.ClearVisualModeType(Field::V_HOVER);
                m_field.SetVisualMode(row, col,
                    Field::V_MOVE_TARGET);

                m_error.clear();
                RequestRepaint();
                return;
            }
        }

        if (click == CLICK_CONFIRM)
        {
            PlayerData& player = m_players[m_activePlayer];

            if (!player.selectS)
            {
                m_error = "Select movement source.";
                RequestRepaint();
                return;
            }

            if (!player.selectT)
            {
                m_error = "Select movement target.";
                RequestRepaint();
                return;
            }

            std::string text = m_antBox.GetText();

            int amount = 0;

            if (!text.empty())
            {
                amount = std::stoi(text);
            }

            if (amount <= 0)
            {
                m_error = "Move amount must be greater than zero.";
                RequestRepaint();
                return;
            }

            if (amount > player.sant)
            {
                m_error = "Not enough ants available.";
                RequestRepaint();
                return;
            }

            ApplyMove(amount);

            // Input no longer needed
            m_field.ClearAllVisualModes();
            ClearHUDState();
            m_turnState = T_END;

            UpdateTurn();
            return;
        }

        if (click == CLICK_CANCEL)
        {
            PlayerData& player = m_players[m_activePlayer];

            m_field.ClearAllVisualModes();
            m_field.SetPlayerVisualMode(m_activePlayer);
            player.selectS = false;
            player.selectT = false;

            player.srow = -1;
            player.scol = -1;
            player.trow = -1;
            player.tcol = -1;
            player.sant = 0;
            player.tant = 0;
            m_antBox.SetText("");
            m_error.clear();

            RequestRepaint();
            return;
        }

        if (click == CLICK_SKIP)
        {
            m_field.ClearAllVisualModes();
            ClearHUDState();
            m_turnState = T_END;

            UpdateTurn();
            return;
        }
    }

    void OnMouseDown(int x, int y)
    {
        POINT pt = { x, y };

        // Check speed button
        RECT speed = m_renderer.GetSpeed();

        if (PtInRect(&speed, pt))
        {
            ChangeSpeed();
            return;
        }

       if (m_players[m_activePlayer].m_playertype == 2)
            return;

        // Check field cells first
        const auto& cells = m_renderer.GetCellRects();

        for (const auto& cell : cells)
        {
            if (PtInRect(&cell.rect, pt))
            {
                m_selectedRow = cell.row;
                m_selectedCol = cell.col;

                if (m_turnState == T_GROWTH)
                {
                    m_clicked = CLICK_FIELD;
                    HandleGrowthClick(m_clicked, cell.row, cell.col);
                }
                else if (m_turnState == T_ATTACK)
                {
                    m_clicked = CLICK_FIELD;
                    HandleAttackClick(m_clicked, cell.row, cell.col);
                }
                else if (m_turnState == T_MOVE)
                {
                    m_clicked = CLICK_FIELD;
                    HandleMoveClick(m_clicked, cell.row, cell.col);
                }
                // TODO:

				return;
            }
        }

        // Check Dialog area
        RECT dialog = m_renderer.GetDialog();

        if (PtInRect(&dialog, pt))
        {

            if (m_turnState == T_GROWTH)
            {
                RECT confirm = m_renderer.GetConfirm();
                RECT cancel = m_renderer.GetCancel();

                if (PtInRect(&confirm, pt))
                {
                    m_clicked = CLICK_CONFIRM;
                    HandleGrowthClick(m_clicked);
                    return;                    
                }
                if (PtInRect(&cancel, pt)) 
                {
                    m_clicked = CLICK_CANCEL;
                    HandleGrowthClick(m_clicked);
                    return;
                }
            }
            else if (m_turnState == T_ATTACK)
            {
                RECT confirm = m_renderer.GetConfirm();
                RECT cancel = m_renderer.GetCancel();
                RECT skip = m_renderer.GetSkip();

                if (PtInRect(&confirm, pt))
                {
                    m_clicked = CLICK_CONFIRM;
                    HandleAttackClick(m_clicked);
                    return;
                }

                if (PtInRect(&cancel, pt))
                {
                    m_clicked = CLICK_CANCEL;
                    HandleAttackClick(m_clicked);
                    return;
                }

                if (PtInRect(&skip, pt))
                {
                    m_clicked = CLICK_SKIP;
                    HandleAttackClick(m_clicked);
                    return;
                }
            }
            else if (m_turnState == T_MOVE)
            {
                RECT confirm = m_renderer.GetConfirm();
                RECT cancel = m_renderer.GetCancel();
                RECT skip = m_renderer.GetSkip();

                if (PtInRect(&confirm, pt))
                {
                    m_clicked = CLICK_CONFIRM;
                    HandleMoveClick(m_clicked);
                    return;
                }

                if (PtInRect(&cancel, pt))
                {
                    m_clicked = CLICK_CANCEL;
                    HandleMoveClick(m_clicked);
                    return;
                }

                if (PtInRect(&skip, pt))
                {
                    m_clicked = CLICK_SKIP;
                    HandleMoveClick(m_clicked);
                    return;
                }
            }
            // Todo;
            return;
        }
    }

void OnChar(char ch)
    {
    if (m_players[m_activePlayer].m_playertype == 2)
        return;

    if (ch == '\r')   // Enter
    {
        if (m_turnState == T_GROWTH)
            HandleGrowthClick(CLICK_CONFIRM);
        else if (m_turnState == T_ATTACK)
            HandleAttackClick(CLICK_CONFIRM);
        else if (m_turnState == T_MOVE)
            HandleMoveClick(CLICK_CONFIRM);

        return;
    }

	if (ch == ' ')     // Space
    {
        if (m_turnState == T_ATTACK)
            HandleAttackClick(CLICK_SKIP);
        else if (m_turnState == T_MOVE)
            HandleMoveClick(CLICK_SKIP);

        return;
    }

        if (m_turnState == T_GROWTH ||
            m_turnState == T_ATTACK ||
            m_turnState == T_MOVE)
        {
            m_antBox.OnChar(ch);

            std::string text = m_antBox.GetText();

            if (!text.empty())
            {
                m_players[m_activePlayer].tant = std::stoi(text);
            }
            else { m_players[m_activePlayer].tant = 0; }

            RequestRepaint();
        }
    }

 void HandleMouseMove(int x, int y)
    {
     if (m_players[m_activePlayer].m_playertype == 2)
         return;

        POINT pt = { x, y };
        m_hoverRow = -1;
        m_hoverCol = -1;
        // Check field cells first
        const auto& cells = m_renderer.GetCellRects();
        for (const auto& cell : cells)
        {
            if (PtInRect(&cell.rect, pt))
            {
                m_hoverRow = cell.row;
                m_hoverCol = cell.col;

                return;
            }
        }
    }

 int GetPlayer() const
    {
        return m_activePlayer;
    }

 void Render()
    {
        m_renderer.Render(m_field, m_players, m_dialog, GetPlayer(), m_antBox, m_error, m_gameSpeed);
    }
};
