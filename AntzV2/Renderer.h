#pragma once
// Renderer.h  Win32 AntzV2 Game
#include <windows.h>
#include <string>
#include <vector>
#include "Field.h"
#include "GameTypes.h"
#include "TextBox.h"

class Renderer
{
private:

	HDC m_hdc = nullptr;
    HFONT m_hudTitleFont = nullptr;
    HFONT m_hudPlayerFont = nullptr;
    HFONT m_hudTextFont = nullptr;
	const int CLIENT_W = 1100;
	const int CLIENT_H = 900;

    const int FIELD_WIDTH = 890;
    const int FIELD_HEIGHT = 690;

    const int STATUS_WIDTH = 195;
    const int STATUS_HEIGHT = 690;

    const int HUD_WIDTH = 540;
    const int HUD_HEIGHT = 195;

    const int DIALOG_WIDTH = 545;
    const int DIALOG_HEIGHT = 195;

    const int BORDER_SIZE = 5;
    const int TITLE_HEIGHT = 100;

    RECT m_fieldRect{ 0,0,0,0 };
    RECT m_statusRect{ 0,0,0,0 };
    RECT m_hudRect{ 0,0,0,0 };
	RECT m_speedRect{ 0,0,0,0 };
    RECT m_dialogRect{ 0,0,0,0 };
    RECT m_antRect{ 0,0,0,0 };
    RECT m_errorRect{ 0,0,0,0 };
    RECT m_confirmRect{ 0,0,0,0 };
    RECT m_cancelRect{ 0,0,0,0 };
    RECT m_skipRect{ 0,0,0,0 };
    std::string currentPlayer = "";
    std::string phase = "";
    bool sourceSelected = false;
    bool targetSelected = false;
    int antsAvailable = 0;
    int antsCommitted = 0;
    int m_activePlayer = 1;
    bool m_battleActive = false;

    // Player colors
    COLORREF m_playerColors[9] =
    {
		RGB(200, 200, 200), // 0 dark gray (unowned)
        RGB(255, 80, 80),   // 1 Red
        RGB(80, 80, 255),   // 2 Blue
        RGB(80, 220, 80),   // 3 Green
        RGB(255, 220, 80),  // 4 Yellow
        RGB(220, 80, 220),  // 5 Purple
        RGB(80, 220, 220),  // 6 Cyan
        RGB(255, 150, 50),  // 7 Orange
        RGB(150, 150, 150)  // 8 Gray
    };
	COLORREF m_antColor1 = RGB(0, 0, 0);// Black
	COLORREF m_antColor2 = RGB(255, 255, 255);// White
    // Visual mode colors
    COLORREF m_growthColor = RGB(80, 255, 80);// Light Green
    COLORREF m_attackSourceColor = RGB(80, 80, 255);//Light Blue
    COLORREF m_attackTargetColor = RGB(255, 80, 80);//Light Red
    COLORREF m_moveSourceColor = RGB(80, 220, 220);// Cyan
    COLORREF m_moveTargetColor = RGB(200, 80, 255);//Purple

    RECT m_fieldContent = {};

    int m_rows = 0;
    int m_cols = 0;

    int m_cellWidth = 0;
    int m_cellHeight = 0;

    struct CellRect
    {
        int row;
        int col;
        RECT rect;
    };
    std::vector<CellRect> m_cellRects;

    std::string GetSpeedText(GameSpeed gameSpeed)
    {
        switch (gameSpeed)
        {
        case GameSpeed::Half:
            return "1/2x";

        case GameSpeed::Normal:
            return "1x";

        case GameSpeed::Fast:
            return "2x";

        case GameSpeed::Fastest:
            return "4x";
        }

        return "";
    }

public:
    Renderer() {}

    void Initialize(HDC hdc)
    {
        m_hdc = hdc;
        DefineLayout();
        DefineFont();
	}

    void DefineFont()
    {
        m_hudTitleFont = CreateFont( 32, 0, 0, 0,
            FW_BOLD, FALSE, FALSE, FALSE,
            DEFAULT_CHARSET, OUT_DEFAULT_PRECIS,
            CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY,
            DEFAULT_PITCH, L"Arial");

        m_hudPlayerFont = CreateFont( 22, 0, 0, 0,
            FW_BOLD, FALSE, FALSE, FALSE,
            DEFAULT_CHARSET, OUT_DEFAULT_PRECIS,
            CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY,
            DEFAULT_PITCH, L"Arial");

        m_hudTextFont = CreateFont( 18, 0, 0, 0,
            FW_NORMAL, FALSE, FALSE, FALSE,
            DEFAULT_CHARSET, OUT_DEFAULT_PRECIS,
            CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY,
            DEFAULT_PITCH, L"Arial");
    }

    COLORREF GetVisualColor(Field::VisualMode mode)
    {
        switch (mode)
        {
        case Field::V_HOVER:
            return m_antColor2;   // existing white

        case Field::V_GROWTH:
            return m_growthColor;

        case Field::V_ATTACK_SOURCE:
            return m_attackSourceColor;

        case Field::V_ATTACK_TARGET:
            return m_attackTargetColor;

        case Field::V_MOVE_SOURCE:
            return m_moveSourceColor;

        case Field::V_MOVE_TARGET:
            return m_moveTargetColor;

        case Field::V_NORMAL:
        default:
            return m_antColor1;   // existing black
        }
    }

    void SetBattleActive(bool active)
    {
        m_battleActive = active;
    }

    void DefineLayout()
    {
        m_fieldRect = { BORDER_SIZE, BORDER_SIZE, 
            BORDER_SIZE + FIELD_WIDTH, BORDER_SIZE + FIELD_HEIGHT };

        m_statusRect = { BORDER_SIZE + FIELD_WIDTH + BORDER_SIZE, BORDER_SIZE,
            BORDER_SIZE + FIELD_WIDTH + BORDER_SIZE + STATUS_WIDTH,
            BORDER_SIZE + STATUS_HEIGHT };

        m_hudRect = { BORDER_SIZE, BORDER_SIZE + FIELD_HEIGHT + BORDER_SIZE,
            BORDER_SIZE + HUD_WIDTH, BORDER_SIZE + FIELD_HEIGHT + BORDER_SIZE + HUD_HEIGHT };

        m_dialogRect = { BORDER_SIZE + HUD_WIDTH + BORDER_SIZE,
            BORDER_SIZE + FIELD_HEIGHT + BORDER_SIZE,
            BORDER_SIZE + HUD_WIDTH + BORDER_SIZE + DIALOG_WIDTH,
            BORDER_SIZE + FIELD_HEIGHT + BORDER_SIZE + DIALOG_HEIGHT };

    }
    void Render(const Field& m_field, const std::vector<PlayerData>& m_players,
        const DialogState& dialog, int m_activePlayer, TextBox& antBox,
        std::string& error, GameSpeed gameSpeed)
    {
        RenderField(m_field, m_players, m_activePlayer);
        RenderStatus(m_players);
        RenderHUD(m_players, dialog, m_activePlayer, gameSpeed);

        switch (dialog.m_mode)
        {
        case DIALOG_GROWTH:
            RenderGrowth(m_players, m_activePlayer, antBox, error);
            break;

        case DIALOG_ATTACK:

            if (m_battleActive)
            {
                RenderBattle(m_players, m_activePlayer);
            }

            RenderAttack(m_players, m_activePlayer, antBox, error);
  
            break;

        case DIALOG_MOVE:
            RenderMove(m_players, m_activePlayer, antBox, error);
            break;

        default:
            RenderDialog(dialog);
            break;
        }
    }

    void RenderCell(int row, int col, const Field::Cells& cell,
        int left, int top, int right, int bottom)
    {
        // Paint entire cell background
        RECT cellRect = { left, top, right, bottom };

        // Cell frame / visual mode border
        COLORREF borderColor = GetVisualColor(cell.visual);

        HBRUSH borderBrush = CreateSolidBrush(borderColor);
        FillRect(m_hdc, &cellRect, borderBrush);
        DeleteObject(borderBrush);

        // Fill inside area with player color
        RECT content = { left + BORDER_SIZE, top + BORDER_SIZE,
            right - BORDER_SIZE, bottom - BORDER_SIZE };

        HBRUSH playerBrush = CreateSolidBrush(m_playerColors[cell.owner]);
        FillRect(m_hdc, &content, playerBrush);
        DeleteObject(playerBrush);


        // Draw ant count
        SetTextColor(m_hdc, m_antColor1);
        SetBkMode(m_hdc, TRANSPARENT);

        WCHAR buf[16];
        wsprintf(buf, L"%d", cell.ants);

        DrawText(m_hdc, buf, -1, &content,
            DT_CENTER | DT_VCENTER | DT_SINGLELINE);
    }

    void RenderField(const Field& field, const std::vector<PlayerData>& m_players,
        int m_activePlayer)
    {

        RECT frame = m_fieldRect;

        RECT content = frame;

        int rows = field.getRows();
        int cols = field.getCols();

        int cellWidth = (content.right - content.left) / cols;
        int cellHeight = (content.bottom - content.top) / rows;

        m_fieldContent = content;

        m_rows = rows;
        m_cols = cols;

        m_cellWidth = cellWidth;
        m_cellHeight = cellHeight;

        m_cellRects.clear(); // Clear previous cell rects

        // Draw cells
        for (int r = 0; r < rows; r++)
        {
            for (int c = 0; c < cols; c++)
            {
                int left = content.left + c * cellWidth;
                int top = content.top + r * cellHeight;
                int right = left + cellWidth;
                int bottom = top + cellHeight;

                RECT cellRect = { left, top, right, bottom };
                m_cellRects.push_back({ r, c, cellRect });

                const auto& cell = field.GetCell(r, c);

                RenderCell(r, c, cell, left, top, right, bottom);
            }
        }
    }

    void RenderHUD(const std::vector<PlayerData>& players, const DialogState& dialog, 
        int thisPlayer, GameSpeed gameSpeed)
    {
        std::string phase = "";
        std::string currentPlayer = players[thisPlayer].m_playerName;
        if (dialog.m_mode == DIALOG_GROWTH) { phase = "Growth"; }
        else if (dialog.m_mode == DIALOG_ATTACK) { phase = "Attack"; }
        else if (dialog.m_mode == DIALOG_MOVE) { phase = "Movement"; }
        bool sourceSelected = players[thisPlayer].selectS;
        bool targetSelected = players[thisPlayer].selectT;
        int antsAvailable = players[thisPlayer].sant;
		int antsCommitted = players[thisPlayer].tant;
        // Frame
        RECT frame = m_hudRect;
        // Paint border area

        // Fill
        RECT content = {frame.left + BORDER_SIZE, frame.top + BORDER_SIZE,
             frame.right - BORDER_SIZE, frame.bottom - BORDER_SIZE };

        HBRUSH fill = CreateSolidBrush(RGB(40, 40, 80));   // Dark blue
        FillRect(m_hdc, &content, fill);
        DeleteObject(fill);

        // HUD text layout
        int centerX = frame.left + (HUD_WIDTH / 2);

        int titleY = frame.top + 10;
        int playerY = titleY + 45;
        int statusY = playerY + 45;
        int countY = statusY + 45;

        SetTextColor(m_hdc, m_antColor2);
        SetBkMode(m_hdc, TRANSPARENT);
        // Phase
        SelectObject(m_hdc, m_hudTitleFont);

        int speedWidth = 90;
        int speedHeight = 30;

        m_speedRect = { frame.right - speedWidth - 10, titleY + 5,
            frame.right - 10, titleY + 5 + speedHeight };

        RECT textRect = { frame.left, titleY, frame.right, titleY + 40 };

        DrawTextA( m_hdc, phase.c_str(), -1, &textRect,
            DT_CENTER | DT_VCENTER | DT_SINGLELINE);

        std::string speedText = GetSpeedText(gameSpeed);
        SelectObject(m_hdc, m_hudPlayerFont);

        DrawTextA(m_hdc, speedText.c_str(), -1, &m_speedRect,
            DT_CENTER | DT_VCENTER | DT_SINGLELINE);

        // Current Player
        SelectObject(m_hdc, m_hudPlayerFont);

        textRect.top = playerY;
        textRect.bottom = playerY + 30;

        std::string playerText = currentPlayer;

        DrawTextA( m_hdc, playerText.c_str(), -1, &textRect,
            DT_CENTER | DT_VCENTER | DT_SINGLELINE);

        // Source / Target
        SelectObject(m_hdc, m_hudTextFont);

        RECT leftRect = { frame.left + 15, statusY,
            centerX - 10, statusY + 25 };

        RECT rightRect = { centerX + 10, statusY,
            frame.right - 15, statusY + 25 };

        std::string sourceText = "Source Selected: " +
            std::string(sourceSelected ? "Yes" : "No");

        std::string targetText = "Target Selected: " +
            std::string(targetSelected ? "Yes" : "No");

        DrawTextA( m_hdc, sourceText.c_str(), -1,
            &leftRect, DT_LEFT | DT_VCENTER | DT_SINGLELINE);

        DrawTextA( m_hdc, targetText.c_str(), -1,
            &rightRect, DT_RIGHT | DT_VCENTER | DT_SINGLELINE);

        // Ant counts
        leftRect.top = countY;
        leftRect.bottom = countY + 25;

        rightRect.top = countY;
        rightRect.bottom = countY + 25;

        std::string availableText = "Ants Available: " + std::to_string(antsAvailable);

        std::string committedText = "Ants Committed: " + std::to_string(antsCommitted);

        DrawTextA( m_hdc, availableText.c_str(), -1,
            &leftRect, DT_LEFT | DT_VCENTER | DT_SINGLELINE);

        DrawTextA( m_hdc, committedText.c_str(), -1,
            &rightRect, DT_RIGHT | DT_VCENTER | DT_SINGLELINE);
    }

    void RenderStatus(const std::vector<PlayerData>& players)
    {
        constexpr int STATUS_MARGIN = 20;
        constexpr int STATUS_COUNT_WIDTH = 50;
        RECT frame = m_statusRect;

        // Fill background
        RECT content = frame;

        HBRUSH fill = CreateSolidBrush(RGB(40, 80, 40));   // Dark green
        FillRect(m_hdc, &content, fill);
        DeleteObject(fill);
        // Player rows
        int rowHeight = frame.bottom - frame.top;
        rowHeight /= static_cast<int>(players.size());

        int y = frame.top;

        for (size_t i = 0; i < players.size(); ++i)
        {
            const auto& player = players[i];
            RECT playerRect =
            { frame.left, y, frame.right,
                (i == players.size() - 1) ? frame.bottom : y + rowHeight};

            // Player background color
            HBRUSH playerBrush = CreateSolidBrush( m_playerColors[player.m_playerID]);

            FillRect(m_hdc, &playerRect, playerBrush);

            DeleteObject(playerBrush);

            // Choose text color
            COLORREF textColor = m_antColor2;

            if (player.m_playerID == 0 || player.m_playerID == 4 ||
                player.m_playerID == 6 || player.m_playerID == 7)
            { textColor = m_antColor1; }

            SetTextColor(m_hdc, textColor);
            SetBkMode(m_hdc, TRANSPARENT);

            // Draw player name
            RECT nameRect = playerRect;
            nameRect.left += STATUS_MARGIN;
            nameRect.right -= (STATUS_COUNT_WIDTH + STATUS_MARGIN);

            DrawTextA( m_hdc, player.m_playerName.c_str(), -1,
                &nameRect, DT_LEFT | DT_VCENTER | DT_SINGLELINE);

            // Draw cells owned
            char cells[16];
            sprintf_s(cells, "%d", player.m_cells_owned);

            RECT cellRect = playerRect;
            cellRect.left = frame.right - (STATUS_COUNT_WIDTH + STATUS_MARGIN);
            cellRect.right = frame.right - STATUS_MARGIN;

            DrawTextA( m_hdc, cells, -1, &cellRect,
                DT_RIGHT | DT_VCENTER | DT_SINGLELINE);

            y += rowHeight;
        }
    }

    void RenderBattle(const std::vector<PlayerData>& players, int thisPlayer)
    {
        RECT frame = m_statusRect;

        // Overall status pane background
        HBRUSH background = CreateSolidBrush(RGB(40, 80, 40));
        FillRect(m_hdc, &frame, background);
        DeleteObject(background);

        // Title pane
        RECT titleRect = { frame.left + BORDER_SIZE, frame.top + BORDER_SIZE,
        frame.right - BORDER_SIZE, frame.top + TITLE_HEIGHT };

        //HBRUSH titleBrush = CreateSolidBrush(RGB(60, 60, 60));
        //FillRect(m_hdc, &titleRect, titleBrush);
        //DeleteObject(titleBrush);

        SetTextColor(m_hdc, m_antColor2);
        SetBkMode(m_hdc, TRANSPARENT);

        SelectObject(m_hdc, m_hudTitleFont);

        DrawTextA(m_hdc, "Battle", -1, &titleRect,
            DT_CENTER | DT_VCENTER | DT_SINGLELINE);

        // Combat panes
        int paneTop = frame.top + TITLE_HEIGHT + BORDER_SIZE;
        int paneBottom = frame.bottom - BORDER_SIZE;

        int paneWidth = (frame.right - frame.left - (BORDER_SIZE * 3)) / 2;

        RECT attackerRect = { frame.left + BORDER_SIZE, paneTop,
            frame.left + BORDER_SIZE + paneWidth, paneBottom };

        RECT defenderRect = { attackerRect.right + BORDER_SIZE, paneTop,
            frame.right - BORDER_SIZE, paneBottom };

        // Attacker pane
        HBRUSH attackerBrush = CreateSolidBrush(m_attackSourceColor);
        FillRect(m_hdc, &attackerRect, attackerBrush);
        DeleteObject(attackerBrush);

        // Defender pane
        HBRUSH defenderBrush = CreateSolidBrush(m_attackTargetColor);
        FillRect(m_hdc, &defenderRect, defenderBrush);
        DeleteObject(defenderBrush);


        SetTextColor(m_hdc, m_antColor1);
        SetBkMode(m_hdc, TRANSPARENT);

        SelectObject(m_hdc, m_hudPlayerFont);

        RECT textRect = attackerRect;

        DrawTextA(m_hdc,
            players[thisPlayer].m_playerName.c_str(),
            -1,
            &textRect,
            DT_CENTER | DT_TOP | DT_SINGLELINE);

        textRect = defenderRect;

        DrawTextA(m_hdc,
            players[players[thisPlayer].tplayerID].m_playerName.c_str(),
            -1,
            &textRect,
            DT_CENTER | DT_TOP | DT_SINGLELINE);

        // Battle information
        SelectObject(m_hdc, m_hudTextFont);

        RECT attackerInfo = attackerRect;
        attackerInfo.top += 45;

        std::string attackText =
            "\nRoll: " + std::to_string(players[thisPlayer].attackRoll) +
            "\n\nAnts: " + std::to_string(players[thisPlayer].sant);

        DrawTextA(m_hdc,
            attackText.c_str(),
            -1,
            &attackerInfo,
            DT_CENTER | DT_WORDBREAK);


        RECT defenderInfo = defenderRect;
        defenderInfo.top += 45;

        std::string defenseText =
            "\nRoll: " + std::to_string(players[thisPlayer].defenseRoll) +
            "\n\nAnts: " + std::to_string(players[thisPlayer].tant);

        DrawTextA(m_hdc,
            defenseText.c_str(),
            -1,
            &defenderInfo,
            DT_CENTER | DT_WORDBREAK);

    }

    void RenderDialogFrame()
    {
        RECT frame = m_dialogRect;

        RECT content = frame;

        HBRUSH fill = CreateSolidBrush(RGB(80, 40, 40));
        FillRect(m_hdc, &content, fill);
        DeleteObject(fill);
    }

    void RenderGrowth( const std::vector<PlayerData>& players,
        int thisPlayer, TextBox& antBox, const std::string& error)
    {
        RECT frame = m_dialogRect;

        RECT content = frame;

        HBRUSH fill = CreateSolidBrush(RGB(80, 40, 40));
        FillRect(m_hdc, &content, fill);
        DeleteObject(fill);

        SetTextColor(m_hdc, m_antColor2);
        SetBkMode(m_hdc, TRANSPARENT);
        SelectObject(m_hdc, m_hudPlayerFont);

        RECT textRect = m_dialogRect;

        textRect.left += 15;
        textRect.right -= 15;

        int lineHeight = 30;
        int y = m_dialogRect.top + 10;
        // Player Name
        textRect.top = y;
        textRect.bottom = y + lineHeight;

        DrawTextA( m_hdc, players[thisPlayer].m_playerName.c_str(),
            -1, &textRect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);

        SelectObject(m_hdc, m_hudTextFont);
        
        y += 30;
        textRect.top = y;
        textRect.bottom = y + lineHeight;

        std::string text = "Distribute Growth";

        DrawTextA( m_hdc, text.c_str(), -1,
            &textRect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);

        y += 30;
        textRect.top = y;
        textRect.bottom = y + lineHeight;

        text = "Select Target cell.";

        DrawTextA(m_hdc, text.c_str(), -1,
            &textRect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);

        y += 30;
        textRect.top = y;
        textRect.bottom = y + lineHeight;

        text = "How many ants?";

        DrawTextA(m_hdc, text.c_str(), -1,
            &textRect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);

        // Ant input box
        int boxWidth = 45;
        int boxHeight = 20;

        int centerX = (m_dialogRect.left + m_dialogRect.right) / 2;

        int boxTop = y + (lineHeight - boxHeight) / 2;

        m_antRect = { centerX + 60, boxTop,
            centerX + 60 + boxWidth, boxTop + boxHeight };

        antBox.SetRect(m_antRect);
        SetTextColor(m_hdc, m_antColor1);// Black
        antBox.Draw(m_hdc);

        // Error Message
        int errorTop = m_antRect.bottom + 5;

        m_errorRect = { m_dialogRect.left + 15, errorTop,
            m_dialogRect.right - 15, errorTop + lineHeight };

        if (!error.empty())
        {
            SetTextColor(m_hdc, m_antColor2);// White
            SetBkMode(m_hdc, TRANSPARENT);

            DrawTextA( m_hdc, error.c_str(), -1,
                &m_errorRect, DT_CENTER | DT_VCENTER | DT_SINGLELINE );
        }

        //Buttons
        int buttonWidth = 80;
        int buttonHeight = 24;
        int buttonGap = 15;

        int buttonTop = m_errorRect.bottom + 5;

        int totalWidth = buttonWidth * 2 + buttonGap;

        int left = centerX - totalWidth / 2;

        m_confirmRect = { left, buttonTop, left + buttonWidth, buttonTop + buttonHeight };

        m_cancelRect = { left + buttonWidth + buttonGap, buttonTop,
            left + buttonWidth * 2 + buttonGap, buttonTop + buttonHeight };

        SetTextColor(m_hdc, m_antColor1); // Black
        
        // Confirm button
        Rectangle( m_hdc, m_confirmRect.left, m_confirmRect.top,
            m_confirmRect.right, m_confirmRect.bottom);

        // Cancel button
        Rectangle( m_hdc, m_cancelRect.left, m_cancelRect.top,
            m_cancelRect.right, m_cancelRect.bottom);
        HBRUSH brush = CreateSolidBrush(RGB(255, 0, 0));
        FillRect(m_hdc, &m_confirmRect, brush);
        DeleteObject(brush);

        DrawTextA( m_hdc, "OK", -1, &m_confirmRect,
            DT_CENTER | DT_VCENTER | DT_SINGLELINE);

        DrawTextA( m_hdc, "Cancel", -1, &m_cancelRect,
            DT_CENTER | DT_VCENTER | DT_SINGLELINE);
    }

    void RenderAttack( const std::vector<PlayerData>& players, int thisPlayer,
        TextBox& antBox, const std::string& error)
    {
        RECT frame = m_dialogRect;

        HBRUSH fill = CreateSolidBrush(RGB(40, 40, 80));
        FillRect(m_hdc, &frame, fill);
        DeleteObject(fill);

        SetTextColor(m_hdc, m_antColor2);
        SetBkMode(m_hdc, TRANSPARENT);

        SelectObject(m_hdc, m_hudPlayerFont);

        RECT textRect = frame;

        textRect.left += 15;
        textRect.right -= 15;

        int y = frame.top + 10;
        int lineHeight = 30;

        textRect.top = y;
        textRect.bottom = y + lineHeight;

        DrawTextA( m_hdc, players[thisPlayer].m_playerName.c_str(), -1,
            &textRect, DT_CENTER | DT_SINGLELINE);

        SelectObject(m_hdc, m_hudTextFont);

        y += lineHeight;

        textRect.top = y;
        textRect.bottom = y + lineHeight;

        DrawTextA( m_hdc, "Attack Phase", -1,
            &textRect, DT_CENTER | DT_SINGLELINE);

        y += lineHeight;

        textRect.top = y;
        textRect.bottom = y + lineHeight;

        DrawTextA( m_hdc, "Select source and target", -1,
            &textRect, DT_CENTER | DT_SINGLELINE);
       
        y += lineHeight;
        textRect.top = y;
        textRect.bottom = y + lineHeight;

        DrawTextA(m_hdc, "How many ants?", -1,
            &textRect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);

        // Ant input box
        int boxWidth = 45;
        int boxHeight = 20;

        int centerX = (m_dialogRect.left + m_dialogRect.right) / 2;

        int boxTop = y + (lineHeight - boxHeight) / 2;

        m_antRect = { centerX + 60, boxTop,
            centerX + 60 + boxWidth, boxTop + boxHeight };

        antBox.SetRect(m_antRect);
        SetTextColor(m_hdc, m_antColor1);// Black
        antBox.Draw(m_hdc);

        // Error Message
        int errorTop = m_antRect.bottom + 5;

        m_errorRect = { m_dialogRect.left + 15, errorTop,
            m_dialogRect.right - 15, errorTop + lineHeight };

        if (!error.empty())
        {
            SetTextColor(m_hdc, m_antColor2);// White
            SetBkMode(m_hdc, TRANSPARENT);

            DrawTextA(m_hdc, error.c_str(), -1,
                &m_errorRect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
        }

        //Buttons
        int buttonWidth = 80;
        int buttonHeight = 24;
        int buttonGap = 15;

        int buttonTop = m_errorRect.bottom + 5;

        int totalWidth = buttonWidth * 3 + buttonGap * 2;

        int left = centerX - totalWidth / 2;

        m_confirmRect = { left, buttonTop, left + buttonWidth, buttonTop + buttonHeight };

        m_cancelRect = { left + buttonWidth + buttonGap, buttonTop,
            left + buttonWidth * 2 + buttonGap, buttonTop + buttonHeight };

        m_skipRect = { left + (buttonWidth + buttonGap) * 2, buttonTop,
            left + buttonWidth * 3 + buttonGap * 2, buttonTop + buttonHeight };

        SetTextColor(m_hdc, m_antColor1); // Black

        // Confirm button
        Rectangle(m_hdc, m_confirmRect.left, m_confirmRect.top,
            m_confirmRect.right, m_confirmRect.bottom);

        // Cancel button
        Rectangle(m_hdc, m_cancelRect.left, m_cancelRect.top,
            m_cancelRect.right, m_cancelRect.bottom);

        // Skip button
        Rectangle(m_hdc, m_skipRect.left, m_skipRect.top,
            m_skipRect.right, m_skipRect.bottom);

        HBRUSH brush = CreateSolidBrush(RGB(255, 0, 0));
        FillRect(m_hdc, &m_confirmRect, brush);
        DeleteObject(brush);

        DrawTextA(m_hdc, "OK", -1, &m_confirmRect,
            DT_CENTER | DT_VCENTER | DT_SINGLELINE);

        DrawTextA(m_hdc, "Cancel", -1, &m_cancelRect,
            DT_CENTER | DT_VCENTER | DT_SINGLELINE);

        DrawTextA(m_hdc, "Skip", -1, &m_skipRect,
            DT_CENTER | DT_VCENTER | DT_SINGLELINE);
    }

    void RenderMove(const std::vector<PlayerData>& players, int thisPlayer,
        TextBox& antBox, const std::string& error)
    {
        RECT frame = m_dialogRect;

        HBRUSH fill = CreateSolidBrush(RGB(40, 40, 80));
        FillRect(m_hdc, &frame, fill);
        DeleteObject(fill);

        SetTextColor(m_hdc, m_antColor2);
        SetBkMode(m_hdc, TRANSPARENT);

        SelectObject(m_hdc, m_hudPlayerFont);

        RECT textRect = frame;

        textRect.left += 15;
        textRect.right -= 15;

        int y = frame.top + 10;
        int lineHeight = 30;

        textRect.top = y;
        textRect.bottom = y + lineHeight;

        DrawTextA(m_hdc, players[thisPlayer].m_playerName.c_str(), -1,
            &textRect, DT_CENTER | DT_SINGLELINE);

        SelectObject(m_hdc, m_hudTextFont);

        y += lineHeight;

        textRect.top = y;
        textRect.bottom = y + lineHeight;

        DrawTextA(m_hdc, "Movement Phase", -1,
            &textRect, DT_CENTER | DT_SINGLELINE);

        y += lineHeight;

        textRect.top = y;
        textRect.bottom = y + lineHeight;

        DrawTextA(m_hdc, "Select source and target", -1,
            &textRect, DT_CENTER | DT_SINGLELINE);

        y += lineHeight;
        textRect.top = y;
        textRect.bottom = y + lineHeight;

        DrawTextA(m_hdc, "How many ants?", -1,
            &textRect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);

        // Ant input box
        int boxWidth = 45;
        int boxHeight = 20;

        int centerX = (m_dialogRect.left + m_dialogRect.right) / 2;

        int boxTop = y + (lineHeight - boxHeight) / 2;

        m_antRect = { centerX + 60, boxTop,
            centerX + 60 + boxWidth, boxTop + boxHeight };

        antBox.SetRect(m_antRect);
        SetTextColor(m_hdc, m_antColor1);// Black
        antBox.Draw(m_hdc);

        // Error Message
        int errorTop = m_antRect.bottom + 5;

        m_errorRect = { m_dialogRect.left + 15, errorTop,
            m_dialogRect.right - 15, errorTop + lineHeight };

        if (!error.empty())
        {
            SetTextColor(m_hdc, m_antColor2);// White
            SetBkMode(m_hdc, TRANSPARENT);

            DrawTextA(m_hdc, error.c_str(), -1,
                &m_errorRect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
        }

        //Buttons
        int buttonWidth = 80;
        int buttonHeight = 24;
        int buttonGap = 15;

        int buttonTop = m_errorRect.bottom + 5;

        int totalWidth = buttonWidth * 3 + buttonGap * 2;

        int left = centerX - totalWidth / 2;

        m_confirmRect = { left, buttonTop, left + buttonWidth, buttonTop + buttonHeight };

        m_cancelRect = { left + buttonWidth + buttonGap, buttonTop,
            left + buttonWidth * 2 + buttonGap, buttonTop + buttonHeight };

        m_skipRect = { left + (buttonWidth + buttonGap) * 2, buttonTop,
            left + buttonWidth * 3 + buttonGap * 2, buttonTop + buttonHeight };

        SetTextColor(m_hdc, m_antColor1); // Black

        // Confirm button
        Rectangle(m_hdc, m_confirmRect.left, m_confirmRect.top,
            m_confirmRect.right, m_confirmRect.bottom);

        // Cancel button
        Rectangle(m_hdc, m_cancelRect.left, m_cancelRect.top,
            m_cancelRect.right, m_cancelRect.bottom);

        // Skip button
        Rectangle(m_hdc, m_skipRect.left, m_skipRect.top,
            m_skipRect.right, m_skipRect.bottom);

        HBRUSH brush = CreateSolidBrush(RGB(255, 0, 0));
        FillRect(m_hdc, &m_confirmRect, brush);
        DeleteObject(brush);

        DrawTextA(m_hdc, "OK", -1, &m_confirmRect,
            DT_CENTER | DT_VCENTER | DT_SINGLELINE);

        DrawTextA(m_hdc, "Cancel", -1, &m_cancelRect,
            DT_CENTER | DT_VCENTER | DT_SINGLELINE);

        DrawTextA(m_hdc, "Skip", -1, &m_skipRect,
            DT_CENTER | DT_VCENTER | DT_SINGLELINE);
    }

    void RenderDialog(const DialogState& dialog)
    {
        
        RECT frame = m_dialogRect;

        // Frame
        HPEN oldPen = (HPEN)SelectObject(m_hdc, GetStockObject(BLACK_PEN));
        HBRUSH oldBrush = (HBRUSH)SelectObject(m_hdc, GetStockObject(NULL_BRUSH));
        Rectangle(m_hdc, frame.left, frame.top, frame.right, frame.bottom);
        SelectObject(m_hdc, oldPen);
        SelectObject(m_hdc, oldBrush);
        // Fill
        RECT content = frame;
        HBRUSH fill = CreateSolidBrush(RGB(80, 40, 40));
        FillRect(m_hdc, &content, fill);
        DeleteObject(fill);

        // Dialog text
        SetTextColor(m_hdc, m_antColor2);
        SetBkMode(m_hdc, TRANSPARENT);

        RECT textRect = frame;

        textRect.left += 15;
        textRect.right -= 15;
        textRect.top += 15;
        textRect.bottom -= 15;


        DrawTextA( m_hdc, dialog.m_message.c_str(), -1,
            &textRect, DT_CENTER | DT_WORDBREAK);
    }

//Getters for field layout
    RECT GetField() const { return m_fieldRect; }
    RECT GetStatus() const { return m_statusRect; }
    RECT GetHUD() const { return m_hudRect; }
	RECT GetDialog() const { return m_dialogRect; }
    RECT GetAnt() const { return m_antRect; }
    RECT GetConfirm() const { return m_confirmRect; }
    RECT GetCancel() const { return m_cancelRect; }
    RECT GetSkip() const { return m_skipRect; }
    RECT GetSpeed() const { return m_speedRect; }

    const std::vector<CellRect>& GetCellRects() const { return m_cellRects; }

    ~Renderer()
    {
        if (m_hudTitleFont)
            DeleteObject(m_hudTitleFont);

        if (m_hudPlayerFont)
            DeleteObject(m_hudPlayerFont);

        if (m_hudTextFont)
            DeleteObject(m_hudTextFont);
    }
};

