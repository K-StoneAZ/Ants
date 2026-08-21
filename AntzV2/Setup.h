#pragma once
#include <windows.h>
#include <vector>
#include <string>
#include <algorithm>
#include <random>
#include "GameTypes.h"
#include "TextBox.h"

extern int gWidth;
extern int gHeight;
extern std::mt19937 gRNG;
extern HINSTANCE hInst;
extern AppState gAppState;
static const wchar_t* gRobotNames[] = {
    L"R2-D2", L"C-3PO", L"HK-47", L"WALL-E",
    L"T-800", L"T-1000", L"ED-209", L"ASIMO",
    L"Optimus", L"Data", L"Bishop", L"Sonny",
    L"GERTY", L"HAL-9000"
};

const wchar_t* m_personaNames[] =
{
    L"Persona 333", L"Persona 111", L"Persona 222", L"Persona 232", L"Persona 313"
};

const COLORREF gLABELColor = RGB(0, 0, 255);// blue
const COLORREF gVALUEColor = RGB(120, 120, 180);// purple
const COLORREF gTITLEColor = RGB(255, 220, 80);// yellow
const COLORREF gSummaryColor = RGB(50, 200, 0);// green

//Layout Constants
const int ROW_HEIGHT = 45;
const int BUTTON_W = 40;
const int BUTTON_H = 30;
const int BUTTON_SPACING = 50;
const int SMALL_GAP = 10;
const int TOP_MARGIN = 120;
const int SUMMARY_OFFSET_X = 270;
const int LABEL_X = 100;

class Setup
{
private:
    HWND m_hWnd = nullptr;
    HFONT m_titleFont = nullptr;
    HFONT m_textFont = nullptr;
    HBITMAP m_hBackground = nullptr;
    GameConfig m_config;
    GameResult m_result;
    std::vector<PlayerConfig> m_players; // 0 player is reserved, Max 8 active players
    bool m_GameConfigComplete = false;
	bool m_PlayerConfigComplete = false;
    bool ready = false;
    int m_ActiveTextBox = -1;
    std::vector<TextBox> m_nameBoxes;
    std::vector<std::string> m_robotNamePool;
    size_t m_robotNameIndex = 0;
	size_t m_personaIndex = 0;
    RECT m_continueRect{ 420, 650, 670, 710 };
    RECT m_titleRect{ 0, 40, gWidth, 80 };
    RECT m_labelRect{ 100, 120, 360, 165 };
    RECT m_dataRect{ 360, 120, 400, 150 };  // defined position for the first button
	RECT m_typeRect{ 390, 75, 470, 120 };//Type column for player setup
	RECT m_nameRect{ 580, 75, 740, 120 };//Name column for player setup
	RECT m_personaRect{ 780, 75, 940, 120 };//Persona column for player setup
    RECT m_replayRect{ 275, 650, 425, 710 }; // replay button
	RECT m_newGameRect{ 475, 650, 625, 710 }; // new game button
	RECT m_exitRect{ 675, 650, 825, 710 }; // exit button

   void SetGameResult(const GameResult& result)
    {
        m_result = result;
	}
   void DefineFonts()
   {
       m_titleFont = CreateFont(
           TitleSize, 0, 0, 0,
           FW_BOLD, FALSE, FALSE, FALSE,
           DEFAULT_CHARSET, OUT_DEFAULT_PRECIS,
           CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY,
           DEFAULT_PITCH, L"Arial");

       m_textFont = CreateFont(
           TextSize, 0, 0, 0,
           FW_NORMAL, FALSE, FALSE, FALSE,
           DEFAULT_CHARSET, OUT_DEFAULT_PRECIS,
           CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY,
           DEFAULT_PITCH, L"Arial");
   }
   Persona GetPersona(int index)
   {
       Persona persona;

       switch (index)
       {
       case 1:
           persona.growth = Balanced;
           persona.attack = Balanced;
           persona.move = Balanced;
           break;

       case 2:
           persona.growth = Aggressive;
           persona.attack = Aggressive;
           persona.move = Aggressive;
           break;

       case 3:
           persona.growth = Defensive;
           persona.attack = Defensive;
           persona.move = Defensive;
           break;

       case 4:
           persona.growth = Defensive;
           persona.attack = Balanced;
           persona.move = Defensive;
           break;

       case 5:
           persona.growth = Balanced;
           persona.attack = Aggressive;
           persona.move = Balanced;
           break;

           // etc.
       }

       return persona;
   }
public:
    const GameConfig& GetConfig() const
    {
        return m_config;
    }
    const std::vector<PlayerConfig>& GetPlayers() const
    {
        return m_players;
    }
    bool IsReady() const
    {
        return ready;
	}
    void ClearReady() { ready = false; }
    bool ValidFieldSize()
    {
        int requiredCells = m_config.m_StartCells * m_config.m_ActivePlayers;
        int availableCells = m_config.m_FieldSize * m_config.m_FieldSize;
        if (requiredCells <= availableCells)
            return true;
        else if (m_config.m_FieldSize == 10 &&  requiredCells < 144)
        {
			m_config.m_FieldSize = 12;
        }
        else
        {
            m_config.m_FieldSize = 14;
        }
        if (requiredCells > 192)
        {
            return false;
		}
    	return true;
    }
    void DrawOptionButton(HDC hdc, int left, int top, int right, int bottom,
        bool selected, const wchar_t* text)
    {
        RECT rc = { left, top, right, bottom };

        if (selected)
        {
			HBRUSH brush = CreateSolidBrush(RGB(150, 200, 255));// light blue
            // Save the old brush
            HBRUSH oldBrush = (HBRUSH)SelectObject(hdc, brush);
            Rectangle(hdc, left, top, right, bottom);
            // Restore
            SelectObject(hdc, oldBrush);
            DeleteObject(brush);
        }
        else
        {
            Rectangle(hdc, left, top, right, bottom);
        }
        HFONT oldFont = (HFONT)SelectObject(hdc, m_textFont);
        DrawText(hdc, text, -1, &rc, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
        SelectObject(hdc, oldFont);
    }
    void RenderPlayerSetup(HDC hdc)
    {
        SetBkMode(hdc, TRANSPARENT);

        WCHAR buf[64];

        // TITLE
        HFONT previousFont = (HFONT)SelectObject(hdc, m_titleFont);
		SetTextColor(hdc, gTITLEColor);// yellow
        DrawText(hdc, L"PLAYER SETUP", -1, &m_titleRect,
            DT_CENTER | DT_VCENTER | DT_SINGLELINE);
        SelectObject(hdc, previousFont);

        // COLUMN HEADERS
		SetTextColor(hdc, gLABELColor);// blue
        previousFont = (HFONT)SelectObject(hdc, m_textFont);
        DrawText(hdc, L"Type", -1, &m_typeRect,
            DT_CENTER | DT_VCENTER | DT_SINGLELINE);


        RECT nameRect = m_nameRect;
        nameRect.top = TOP_MARGIN;
        nameRect.bottom = TOP_MARGIN + BUTTON_H;
        RECT personaRect = m_personaRect;
        personaRect.top = TOP_MARGIN;
        personaRect.bottom = TOP_MARGIN + BUTTON_H;

        DrawText(hdc, L"Name", -1, &m_nameRect,
            DT_CENTER | DT_VCENTER | DT_SINGLELINE);

        DrawText(hdc, L"Persona", -1, &m_personaRect,
            DT_CENTER | DT_VCENTER | DT_SINGLELINE);

        RECT labelRect = m_labelRect;

        for (size_t i = 1; i < m_players.size(); i++)
        {
            wsprintf(buf, L"Player %d", (int)i);
            TextOut(hdc, labelRect.left, labelRect.top,
                buf, lstrlen(buf));

			DrawOptionButton(hdc, m_typeRect.left, labelRect.top, m_typeRect.right,
                labelRect.top + BUTTON_H, m_players[i].m_isHuman,
                m_players[i].m_isHuman ? L"Human" : L"AI");

            if (i < m_nameBoxes.size())
            {
                m_nameBoxes[i].SetRect(nameRect);
                m_nameBoxes[i].Draw(hdc);
            }
            const wchar_t* personaText = L""; // default
            if (!m_players[i].m_personaName.empty())
            {
                personaText = m_players[i].m_personaName.c_str();
            }
            DrawOptionButton(hdc, personaRect.left, personaRect.top,
                personaRect.right, personaRect.bottom,
                false, personaText);

            labelRect.top += ROW_HEIGHT;
            labelRect.bottom += ROW_HEIGHT;

            nameRect.top += ROW_HEIGHT;
            nameRect.bottom += ROW_HEIGHT;
            personaRect.top += ROW_HEIGHT;
            personaRect.bottom += ROW_HEIGHT;
        }

        // Continue BUTTON
        SetTextColor(hdc, gVALUEColor);
        DrawOptionButton(hdc, m_continueRect.left, m_continueRect.top,
            m_continueRect.right, m_continueRect.bottom, false, L"Continue");

        SelectObject(hdc, previousFont);
    }

    void RenderSummary(HDC hdc)
    {
        SetBkMode(hdc, TRANSPARENT);
        WCHAR buf[32];
        // TITLE
        HFONT previousFont = (HFONT)SelectObject(hdc, m_titleFont);
        SetTextColor(hdc, gTITLEColor);
        DrawText(hdc, L"ANTS SETUP", -1, &m_titleRect,
            DT_CENTER | DT_VCENTER | DT_SINGLELINE);
        SelectObject(hdc, previousFont);

        RECT labelRect = m_typeRect;
        labelRect.left -= BUTTON_W * 2.25;
        labelRect.right += BUTTON_W * 2;
        labelRect.top = TOP_MARGIN;
        labelRect.bottom = TOP_MARGIN + ROW_HEIGHT;

        RECT valueRect = m_nameRect;
        valueRect.top = TOP_MARGIN;
        valueRect.bottom = TOP_MARGIN + ROW_HEIGHT;

        previousFont = (HFONT)SelectObject(hdc, m_textFont);
        SetTextColor(hdc, gSummaryColor);
        // FIELD SIZE
        DrawText(hdc, L"Field Size", -1, &labelRect, DT_LEFT | DT_VCENTER | DT_SINGLELINE);

        wsprintf(buf, L"%d", m_config.m_FieldSize);
        DrawText(hdc, buf, -1, &valueRect, DT_RIGHT | DT_VCENTER | DT_SINGLELINE);

        labelRect.top += ROW_HEIGHT;
        labelRect.bottom += ROW_HEIGHT;
        valueRect.top += ROW_HEIGHT;
        valueRect.bottom += ROW_HEIGHT;

        // PLAYER COUNT
        DrawText(hdc, L"Players", -1, &labelRect, DT_LEFT | DT_VCENTER | DT_SINGLELINE);

        wsprintf(buf, L"%d", m_config.m_ActivePlayers);
        DrawText(hdc, buf, -1, &valueRect, DT_RIGHT | DT_VCENTER | DT_SINGLELINE);

        labelRect.top += ROW_HEIGHT;
        labelRect.bottom += ROW_HEIGHT;
        valueRect.top += ROW_HEIGHT;
        valueRect.bottom += ROW_HEIGHT;

        // DIFFICULTY
        DrawText(hdc, L"Difficulty", -1, &labelRect, DT_LEFT | DT_VCENTER | DT_SINGLELINE);
        const wchar_t* diff =
            (m_config.m_Difficulty == 1) ? L"Easy" :
            (m_config.m_Difficulty == 2) ? L"Medium" : L"Hard";
        DrawText(hdc, diff, -1, &valueRect, DT_RIGHT | DT_VCENTER | DT_SINGLELINE);

        labelRect.top += ROW_HEIGHT;
        labelRect.bottom += ROW_HEIGHT;
        valueRect.top += ROW_HEIGHT;
        valueRect.bottom += ROW_HEIGHT;

        // START CELLS
        DrawText(hdc, L"Start Cells", -1, &labelRect, DT_LEFT | DT_VCENTER | DT_SINGLELINE);

        wsprintf(buf, L"%d", m_config.m_StartCells);
        DrawText(hdc, buf, -1, &valueRect, DT_RIGHT | DT_VCENTER | DT_SINGLELINE);

        labelRect.top += ROW_HEIGHT;
        labelRect.bottom += ROW_HEIGHT;
        valueRect.top += ROW_HEIGHT;
        valueRect.bottom += ROW_HEIGHT;

        // ATTACKS
        DrawText(hdc, L"Successful Attacks", -1, &labelRect, DT_LEFT | DT_VCENTER | DT_SINGLELINE);
        wsprintf(buf, L"%d", m_config.m_AttackPerTurn);
        DrawText(hdc, buf, -1, &valueRect, DT_RIGHT | DT_VCENTER | DT_SINGLELINE);

        int y = TOP_MARGIN + ROW_HEIGHT * 6;

        RECT playersTitleRect = m_titleRect;
        playersTitleRect.top = y;
        playersTitleRect.bottom = y + ROW_HEIGHT;

        SelectObject(hdc, previousFont);
        previousFont = (HFONT)SelectObject(hdc, m_titleFont);
        DrawText(hdc, L"Players:", -1, &playersTitleRect,
            DT_CENTER | DT_VCENTER | DT_SINGLELINE);
        SelectObject(hdc, previousFont);
        previousFont = (HFONT)SelectObject(hdc, m_textFont);

        y += ROW_HEIGHT;
        labelRect.top = y;
        labelRect.bottom = y + ROW_HEIGHT;
        valueRect.top = y;
        valueRect.bottom = y + ROW_HEIGHT;
        valueRect.right = valueRect.left + (labelRect.right - labelRect.left);

        for (size_t i = 1; i < m_players.size(); i++)
        {
            std::wstring name(
                m_players[i].m_PlayerName.begin(),
                m_players[i].m_PlayerName.end());

            if (m_players[i].m_isHuman)
            {
                wsprintf(buf, L"%d: %s (Human)",
                    (int)i, name.c_str());
            }
            else
            {
                wsprintf(buf, L"%d: %s (AI - %s)",
                    (int)i, name.c_str(),
                    m_players[i].m_personaName.c_str());
            }

            if (i % 2 == 1)
            {
                // left column
                DrawText(hdc, buf, -1, &labelRect, DT_LEFT | DT_VCENTER | DT_SINGLELINE);
            }
            else
            {
                // right column
                DrawText(hdc, buf, -1, &valueRect, DT_LEFT | DT_VCENTER | DT_SINGLELINE);
                y += ROW_HEIGHT;
                labelRect.top = y;
                labelRect.bottom = y + ROW_HEIGHT;
                valueRect.top = y;
                valueRect.bottom = y + ROW_HEIGHT;
            }
        }
		SetTextColor(hdc, gVALUEColor);
        DrawOptionButton(hdc, m_continueRect.left, m_continueRect.top,
            m_continueRect.right, m_continueRect.bottom, false, L"Continue");

        SelectObject(hdc, previousFont);
    }

    void GetExeDirectory(wchar_t* outPath, int size)
    {
        GetModuleFileNameW(nullptr, outPath, size);

        wchar_t* lastSlash = wcsrchr(outPath, L'\\');
        if (lastSlash)
            *(lastSlash + 1) = 0; // keep trailing slash
    }
    bool Initialize(HWND hWnd)
    {
        wchar_t path[MAX_PATH];
        GetExeDirectory(path, MAX_PATH);
        wcscat_s(path, L"SetupBackground.bmp");

        m_hBackground = (HBITMAP)LoadImage( nullptr, path,
                IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);

        DefineFonts();

        return (m_hBackground != nullptr);
    }
    void Shutdown()
    {
        if (m_titleFont)
        {
            DeleteObject(m_titleFont);
            m_titleFont = nullptr;
        }

        if (m_textFont)
        {
            DeleteObject(m_textFont);
            m_textFont = nullptr;
        }
        if (m_hBackground)
        {
            DeleteObject(m_hBackground);
            m_hBackground = nullptr;
        }
    }
    void RenderBackground(HDC hdc)
    {
        if (!m_hBackground)
            return;

        HDC memDC = CreateCompatibleDC(hdc);

        HBITMAP oldBitmap =
            (HBITMAP)SelectObject(memDC, m_hBackground);

        BITMAP bm;
        GetObject(m_hBackground, sizeof(bm), &bm);

        StretchBlt( hdc, 0, 0, gWidth, gHeight, memDC,
            0, 0, bm.bmWidth, bm.bmHeight, SRCCOPY);

        SelectObject(memDC, oldBitmap);
        DeleteDC(memDC);
    }
    void RenderSetup(HDC hdc)
    {
        RenderBackground(hdc);

        if (m_PlayerConfigComplete && m_GameConfigComplete)
        {
            RenderSummary(hdc);
            return;
        }
        if (m_GameConfigComplete)
        {
            RenderPlayerSetup(hdc);
            return;
        }

        SetBkMode(hdc, TRANSPARENT);

        WCHAR buf[32];

        // TITLE
        HFONT previousFont = (HFONT)SelectObject(hdc, m_titleFont);
        SetTextColor(hdc, gTITLEColor);
        DrawText(hdc, L"ANTS SETUP", -1, &m_titleRect,
            DT_CENTER | DT_VCENTER | DT_SINGLELINE);
        SelectObject(hdc, previousFont);
        previousFont = (HFONT)SelectObject(hdc, m_textFont);

        // FIELD SIZE
        SetTextColor(hdc, gLABELColor);
        DrawText(hdc, L"Field Size", -1, &m_labelRect,
            DT_LEFT | DT_VCENTER | DT_SINGLELINE);

        DrawOptionButton(hdc, m_dataRect.left, m_dataRect.top, m_dataRect.right,
            m_dataRect.bottom, m_config.m_FieldSize == 10, L"10");

        RECT dataRect = m_dataRect;

        dataRect.left += BUTTON_SPACING;
        dataRect.right += BUTTON_SPACING;

        DrawOptionButton(hdc, dataRect.left, dataRect.top, dataRect.right,
            dataRect.bottom, m_config.m_FieldSize == 12, L"12");

        dataRect.left += BUTTON_SPACING;
        dataRect.right += BUTTON_SPACING;

        DrawOptionButton(hdc, dataRect.left, dataRect.top, dataRect.right,
            dataRect.bottom, m_config.m_FieldSize == 14, L"14");

        // PLAYER COUNT
        RECT labelRect = m_labelRect;
        labelRect.top += ROW_HEIGHT;
        labelRect.bottom += ROW_HEIGHT;

        DrawText(hdc, L"Players", -1, &labelRect,
            DT_LEFT | DT_VCENTER | DT_SINGLELINE);

        dataRect = m_dataRect;
        dataRect.top += ROW_HEIGHT;
        dataRect.bottom += ROW_HEIGHT;

        DrawOptionButton(hdc, dataRect.left, dataRect.top, dataRect.right,
            dataRect.bottom, m_config.m_ActivePlayers == 2, L"2");

        dataRect.left += BUTTON_SPACING;
        dataRect.right += BUTTON_SPACING;

        DrawOptionButton(hdc, dataRect.left, dataRect.top, dataRect.right,
            dataRect.bottom, m_config.m_ActivePlayers == 3, L"3");

        dataRect.left += BUTTON_SPACING;
        dataRect.right += BUTTON_SPACING;

        DrawOptionButton(hdc, dataRect.left, dataRect.top, dataRect.right,
            dataRect.bottom, m_config.m_ActivePlayers == 4, L"4");

        dataRect.left += BUTTON_SPACING;
        dataRect.right += BUTTON_SPACING;

        DrawOptionButton(hdc, dataRect.left, dataRect.top, dataRect.right,
            dataRect.bottom, m_config.m_ActivePlayers == 5, L"5");

        dataRect.left += BUTTON_SPACING;
        dataRect.right += BUTTON_SPACING;

        DrawOptionButton(hdc, dataRect.left, dataRect.top, dataRect.right,
            dataRect.bottom, m_config.m_ActivePlayers == 6, L"6");

        dataRect.left += BUTTON_SPACING;
        dataRect.right += BUTTON_SPACING;

        DrawOptionButton(hdc, dataRect.left, dataRect.top, dataRect.right,
            dataRect.bottom, m_config.m_ActivePlayers == 7, L"7");

        dataRect.left += BUTTON_SPACING;
        dataRect.right += BUTTON_SPACING;

        DrawOptionButton(hdc, dataRect.left, dataRect.top, dataRect.right,
            dataRect.bottom, m_config.m_ActivePlayers == 8, L"8");

        // DIFFICULTY
        labelRect = m_labelRect;
        labelRect.top += ROW_HEIGHT * 2;
        labelRect.bottom += ROW_HEIGHT * 2;
        dataRect = m_dataRect;
        dataRect.top += ROW_HEIGHT * 2;
        dataRect.bottom += ROW_HEIGHT * 2;

        DrawText(hdc, L"Difficulty", -1, &labelRect,
            DT_LEFT | DT_VCENTER | DT_SINGLELINE);

        SetTextColor(hdc, gVALUEColor);
        const wchar_t* diff =
            (m_config.m_Difficulty == 1) ? L"Easy" :
            (m_config.m_Difficulty == 2) ? L"Medium" : L"Hard";

        DrawText(hdc, diff, -1, &dataRect, DT_LEFT | DT_VCENTER | DT_SINGLELINE);

        // START CELLS
        labelRect = m_labelRect;
        labelRect.top += ROW_HEIGHT * 3;
        labelRect.bottom += ROW_HEIGHT * 3;
        dataRect = m_dataRect;
        dataRect.top += ROW_HEIGHT * 3;
        dataRect.bottom += ROW_HEIGHT * 3;

        SetTextColor(hdc, gLABELColor);
        DrawText(hdc, L"Start Cells", -1, &labelRect,
            DT_LEFT | DT_VCENTER | DT_SINGLELINE);

        SetTextColor(hdc, gVALUEColor);
        wsprintf(buf, L"%d", m_config.m_StartCells);
        DrawText(hdc, buf, -1, &dataRect, DT_LEFT | DT_VCENTER | DT_SINGLELINE);

        dataRect.left += BUTTON_SPACING * 2;
        dataRect.right += BUTTON_SPACING * 2;
        DrawOptionButton(hdc, dataRect.left, dataRect.top, dataRect.right,
            dataRect.bottom, false, L"-");

        dataRect.left += BUTTON_SPACING;
        dataRect.right += BUTTON_SPACING;
        DrawOptionButton(hdc, dataRect.left, dataRect.top, dataRect.right,
            dataRect.bottom, false, L"+");

        // ATTACKS
        labelRect = m_labelRect;
        labelRect.top += ROW_HEIGHT * 4;
        labelRect.bottom += ROW_HEIGHT * 4;

        dataRect = m_dataRect;
        dataRect.top += ROW_HEIGHT * 4;
        dataRect.bottom += ROW_HEIGHT * 4;

        SetTextColor(hdc, gLABELColor);

        RECT lineRect = labelRect;
        lineRect.bottom = lineRect.top + ROW_HEIGHT / 2;

        DrawText(hdc, L"Successful", -1, &lineRect,
            DT_LEFT | DT_VCENTER | DT_SINGLELINE);

        lineRect.top = lineRect.bottom;
        lineRect.bottom = labelRect.bottom;

        DrawText(hdc, L"Attacks/Turn", -1, &lineRect,
            DT_LEFT | DT_VCENTER | DT_SINGLELINE);

        DrawOptionButton(hdc, dataRect.left, dataRect.top, dataRect.right,
            dataRect.bottom, m_config.m_AttackPerTurn == 2, L"2");

        dataRect.left += BUTTON_SPACING;
        dataRect.right += BUTTON_SPACING;

        DrawOptionButton(hdc, dataRect.left, dataRect.top, dataRect.right,
            dataRect.bottom, m_config.m_AttackPerTurn == 3, L"3");

        dataRect.left += BUTTON_SPACING;
        dataRect.right += BUTTON_SPACING;

        DrawOptionButton(hdc, dataRect.left, dataRect.top, dataRect.right,
            dataRect.bottom, m_config.m_AttackPerTurn == 4, L"4");

        dataRect.left += BUTTON_SPACING;
        dataRect.right += BUTTON_SPACING;

        DrawOptionButton(hdc, dataRect.left, dataRect.top, dataRect.right,
            dataRect.bottom, m_config.m_AttackPerTurn == 5, L"5");

        // Continue BUTTON
        SetTextColor(hdc, gVALUEColor);
        DrawOptionButton(hdc, m_continueRect.left, m_continueRect.top,
            m_continueRect.right, m_continueRect.bottom, false, L"Continue");

        SelectObject(hdc, previousFont);
    }

    void RenderWin(HDC hdc)
       {
            // Draw the existing setup background
            RenderBackground(hdc);
            SetBkMode(hdc, TRANSPARENT);
            WCHAR buf[64];

            // TITLE
            HFONT previousFont = (HFONT)SelectObject(hdc, m_titleFont);
            SetTextColor(hdc, gTITLEColor);

            DrawText(hdc, L"GAME OVER", -1, &m_titleRect,
                DT_CENTER | DT_VCENTER | DT_SINGLELINE);
           
            //Winner's Name
            RECT winnerRect = m_titleRect;
            winnerRect.top = TOP_MARGIN;
            winnerRect.bottom = TOP_MARGIN + ROW_HEIGHT;

            std::wstring winnerText(
                m_players[m_result.m_winner].m_PlayerName.begin(),
                m_players[m_result.m_winner].m_PlayerName.end());

            winnerText += L" Wins!";

            DrawText(hdc, winnerText.c_str(), -1, &winnerRect,
                DT_CENTER | DT_VCENTER | DT_SINGLELINE);
            SelectObject(hdc, previousFont);
            previousFont = (HFONT)SelectObject(hdc, m_textFont);

            // Win type
            RECT winTypeRect = winnerRect;
            winTypeRect.top += ROW_HEIGHT;
            winTypeRect.bottom += ROW_HEIGHT;

            const wchar_t* winType = L"";

            if (m_result.m_winType == WinType::Domination)
                winType = L"Domination Victory";
            else if (m_result.m_winType == WinType::Elimination)
                winType = L"Elimination Victory";

            SetTextColor(hdc, gSummaryColor);

            DrawText(hdc, winType, -1, &winTypeRect,
                DT_CENTER | DT_VCENTER | DT_SINGLELINE);

			//Game Setttings Summary
            RECT settingsTitleRect = m_titleRect;
            settingsTitleRect.top = TOP_MARGIN + ROW_HEIGHT * 2;
            settingsTitleRect.bottom = settingsTitleRect.top + ROW_HEIGHT;

            SetTextColor(hdc, gTITLEColor);
            SelectObject(hdc, previousFont);
            previousFont = (HFONT)SelectObject(hdc, m_titleFont);

            DrawText(hdc, L"GAME SETTINGS", -1, &settingsTitleRect,
                DT_CENTER | DT_VCENTER | DT_SINGLELINE);

            SelectObject(hdc, previousFont);
            previousFont = (HFONT)SelectObject(hdc, m_textFont);
            // GAME SETTINGS
            RECT labelRect = m_typeRect;
            labelRect.left -= BUTTON_W * 2;
            labelRect.right += BUTTON_W * 2;
            labelRect.top = settingsTitleRect.bottom;
            labelRect.bottom = labelRect.top + ROW_HEIGHT;

            RECT valueRect = m_nameRect;
            valueRect.top = labelRect.top;
            valueRect.bottom = labelRect.bottom;

            SetTextColor(hdc, gSummaryColor);

            // FIELD SIZE
            DrawText(hdc, L"Field Size", -1, &labelRect,
                DT_LEFT | DT_VCENTER | DT_SINGLELINE);

            wsprintf(buf, L"%d", m_config.m_FieldSize);

            DrawText(hdc, buf, -1, &labelRect,
                DT_RIGHT | DT_VCENTER | DT_SINGLELINE);

            // PLAYER COUNT

            DrawText(hdc, L"Players", -1, &valueRect,
                DT_LEFT | DT_VCENTER | DT_SINGLELINE);

            wsprintf(buf, L"%d", m_config.m_ActivePlayers);

            DrawText(hdc, buf, -1, &valueRect,
                DT_RIGHT | DT_VCENTER | DT_SINGLELINE);

            // DIFFICULTY
            labelRect.top += ROW_HEIGHT;
            labelRect.bottom += ROW_HEIGHT;
            valueRect.top += ROW_HEIGHT;
            valueRect.bottom += ROW_HEIGHT;

            DrawText(hdc, L"Difficulty", -1, &labelRect,
                DT_LEFT | DT_VCENTER | DT_SINGLELINE);

            const wchar_t* diff =
                (m_config.m_Difficulty == 1) ? L"Easy" :
                (m_config.m_Difficulty == 2) ? L"Medium" : L"Hard";

            DrawText(hdc, diff, -1, &labelRect,
                DT_RIGHT | DT_VCENTER | DT_SINGLELINE);

            // START CELLS

            DrawText(hdc, L"Start Cells", -1, &valueRect,
                DT_LEFT | DT_VCENTER | DT_SINGLELINE);

            wsprintf(buf, L"%d", m_config.m_StartCells);

            DrawText(hdc, buf, -1, &valueRect,
                DT_RIGHT | DT_VCENTER | DT_SINGLELINE);

            // SUCCESSFUL ATTACKS
            labelRect.top += ROW_HEIGHT;
            labelRect.bottom += ROW_HEIGHT;
            valueRect.top += ROW_HEIGHT;
            valueRect.bottom += ROW_HEIGHT;

            DrawText(hdc, L"Successful Attacks", -1, &labelRect,
                DT_CENTER | DT_VCENTER | DT_SINGLELINE);

            wsprintf(buf, L"%d", m_config.m_AttackPerTurn);

            DrawText(hdc, buf, -1, &valueRect,
                DT_CENTER | DT_VCENTER | DT_SINGLELINE);

            // PLAYERS TITLE

            int y = valueRect.bottom + SMALL_GAP;

            RECT playersTitleRect = m_titleRect;
            playersTitleRect.top = y;
            playersTitleRect.bottom = y + ROW_HEIGHT;

            SetTextColor(hdc, gTITLEColor);
            SelectObject(hdc, previousFont);
            previousFont = (HFONT)SelectObject(hdc, m_titleFont);

            DrawText(hdc, L"PLAYERS", -1, &playersTitleRect,
                DT_CENTER | DT_VCENTER | DT_SINGLELINE);

            // PLAYER LIST
            y += ROW_HEIGHT;

            labelRect.top = y;
            labelRect.bottom = y + ROW_HEIGHT;

            valueRect.top = y;
            valueRect.bottom = y + ROW_HEIGHT;

            SetTextColor(hdc, gSummaryColor);
            SelectObject(hdc, previousFont);
            previousFont = (HFONT)SelectObject(hdc, m_textFont);

            for (size_t i = 1; i < m_players.size(); i++)
            {
                std::wstring name(
                    m_players[i].m_PlayerName.begin(),
                    m_players[i].m_PlayerName.end());

                wsprintf(buf, L"%d: %s", (int)i, name.c_str());

                if (i % 2 == 1)
                {
                    DrawText(hdc, buf, -1, &labelRect,
                        DT_LEFT | DT_VCENTER | DT_SINGLELINE);
                    DrawText(hdc, m_players[i].m_isHuman ? L"Human" : L"AI",
                        -1, &labelRect, DT_RIGHT | DT_VCENTER | DT_SINGLELINE);
                }
                else
                {
                    DrawText(hdc, buf, -1, &valueRect,
                        DT_LEFT | DT_VCENTER | DT_SINGLELINE);
                    DrawText(hdc, m_players[i].m_isHuman ? L"Human" : L"AI",
                        -1, &valueRect, DT_RIGHT | DT_VCENTER | DT_SINGLELINE);

                    y += ROW_HEIGHT;

                    labelRect.top = y;
                    labelRect.bottom = y + ROW_HEIGHT;

                    valueRect.top = y;
                    valueRect.bottom = y + ROW_HEIGHT;
                }
            }

            // WIN SCREEN BUTTONS
            SetTextColor(hdc, gVALUEColor);

            DrawOptionButton(hdc, m_replayRect.left, m_replayRect.top,
                m_replayRect.right, m_replayRect.bottom,
                false, L"Replay");

            DrawOptionButton(hdc, m_newGameRect.left, m_newGameRect.top,
                m_newGameRect.right, m_newGameRect.bottom,
                false, L"New Game");

            DrawOptionButton(hdc, m_exitRect.left, m_exitRect.top,
                m_exitRect.right, m_exitRect.bottom,
                false, L"Exit");

            SelectObject(hdc, previousFont);
    }

    void HandleMouseClick(HWND hWnd, int x, int y)
    {
        if(ready == true) { ready = false;}

        if (gAppState == APP_WIN)
        {
            // REPLAY
            if (x >= m_replayRect.left && x <= m_replayRect.right &&
                y >= m_replayRect.top && y <= m_replayRect.bottom)
            {
                // Replay current game
                ready = true;
                gAppState = APP_GAME;
                return;
            }

            // NEW GAME
            if (x >= m_newGameRect.left && x <= m_newGameRect.right &&
                y >= m_newGameRect.top && y <= m_newGameRect.bottom)
            {
                // Return to Setup
                m_GameConfigComplete = false;
                m_PlayerConfigComplete = false;
                ready = false;
                gAppState = APP_SETUP;
                return;
            }

            // EXIT
            if (x >= m_exitRect.left && x <= m_exitRect.right &&
                y >= m_exitRect.top && y <= m_exitRect.bottom)
            {
                // Cleanup and exit
                ready = false;
                gAppState = APP_GAMEOVER;
                return;
            }
        }

        if (!m_GameConfigComplete)
        {
            // FIELD SIZE
            RECT dataRect = m_dataRect;

            if (x >= dataRect.left && x <= dataRect.right &&
                y >= dataRect.top && y <= dataRect.bottom)
            {
                m_config.m_FieldSize = 10;
            }
            dataRect.left += BUTTON_SPACING;
            dataRect.right += BUTTON_SPACING;

            if (x >= dataRect.left && x <= dataRect.right &&
                y >= dataRect.top && y <= dataRect.bottom)
            {
                m_config.m_FieldSize = 12;
            }
            dataRect.left += BUTTON_SPACING;
            dataRect.right += BUTTON_SPACING;

            if (x >= dataRect.left && x <= dataRect.right &&
                y >= dataRect.top && y <= dataRect.bottom)
            {
                m_config.m_FieldSize = 14;
            }

            // PLAYER COUNT
            dataRect = m_dataRect;
            dataRect.top += ROW_HEIGHT;
            dataRect.bottom += ROW_HEIGHT;

            if (x >= dataRect.left && x <= dataRect.right &&
                y >= dataRect.top && y <= dataRect.bottom)
            {
                m_config.m_ActivePlayers = 2;
            }
            dataRect.left += BUTTON_SPACING;
            dataRect.right += BUTTON_SPACING;

            if (x >= dataRect.left && x <= dataRect.right &&
                y >= dataRect.top && y <= dataRect.bottom)
            {
                m_config.m_ActivePlayers = 3;
            }
            dataRect.left += BUTTON_SPACING;
            dataRect.right += BUTTON_SPACING;

            if (x >= dataRect.left && x <= dataRect.right &&
                y >= dataRect.top && y <= dataRect.bottom)
            {
                m_config.m_ActivePlayers = 4;
            }
            dataRect.left += BUTTON_SPACING;
            dataRect.right += BUTTON_SPACING;

            if (x >= dataRect.left && x <= dataRect.right &&
                y >= dataRect.top && y <= dataRect.bottom)
            {
                m_config.m_ActivePlayers = 5;
            }
            dataRect.left += BUTTON_SPACING;
            dataRect.right += BUTTON_SPACING;

            if (x >= dataRect.left && x <= dataRect.right &&
                y >= dataRect.top && y <= dataRect.bottom)
            {
               m_config.m_ActivePlayers = 6;
            }
    
            dataRect.left += BUTTON_SPACING;
            dataRect.right += BUTTON_SPACING;

            if (x >= dataRect.left && x <= dataRect.right &&
                y >= dataRect.top && y <= dataRect.bottom)
            {
                m_config.m_ActivePlayers = 7;
            }
            dataRect.left += BUTTON_SPACING;
            dataRect.right += BUTTON_SPACING;

            if (x >= dataRect.left && x <= dataRect.right &&
                y >= dataRect.top && y <= dataRect.bottom)
            {
                m_config.m_ActivePlayers = 8;
            }

            // DIFFICULTY (cycle)
            dataRect = m_dataRect;
            dataRect.top += ROW_HEIGHT * 2;
            dataRect.bottom += ROW_HEIGHT * 2;

            if (x >= dataRect.left && x <= dataRect.right &&
                y >= dataRect.top && y <= dataRect.bottom)
            {
                m_config.m_Difficulty++;
                if (m_config.m_Difficulty > 3)
                    m_config.m_Difficulty = 1;
            }
            // START CELLS (- / +)
            dataRect = m_dataRect;
            dataRect.top += ROW_HEIGHT * 3;
            dataRect.bottom += ROW_HEIGHT * 3;
            dataRect.left += BUTTON_SPACING * 2;
            dataRect.right += BUTTON_SPACING * 2;

            if (x >= dataRect.left && x <= dataRect.right &&
                y >= dataRect.top && y <= dataRect.bottom)
            {
                if (m_config.m_StartCells > 15)
                    m_config.m_StartCells--;
            }

            dataRect.left += BUTTON_SPACING;
            dataRect.right += BUTTON_SPACING;

            if (x >= dataRect.left && x <= dataRect.right &&
                y >= dataRect.top && y <= dataRect.bottom)
            {
                if (m_config.m_StartCells < 21)
                    m_config.m_StartCells++;
            }
            // ATTACKS PER TURN 
            dataRect = m_dataRect;
            dataRect.top += ROW_HEIGHT * 4;
            dataRect.bottom += ROW_HEIGHT * 4;

            if (x >= dataRect.left && x <= dataRect.right &&
                y >= dataRect.top && y <= dataRect.bottom)
            {
                m_config.m_AttackPerTurn = 2;
            }
            dataRect.left += BUTTON_SPACING;
            dataRect.right += BUTTON_SPACING;

            if (x >= dataRect.left && x <= dataRect.right &&
                y >= dataRect.top && y <= dataRect.bottom)
            {
                m_config.m_AttackPerTurn = 3;
            }
            dataRect.left += BUTTON_SPACING;
            dataRect.right += BUTTON_SPACING;

            if (x >= dataRect.left && x <= dataRect.right &&
                y >= dataRect.top && y <= dataRect.bottom)
            {
                m_config.m_AttackPerTurn = 4;
            }
            dataRect.left += BUTTON_SPACING;
            dataRect.right += BUTTON_SPACING;

            if (x >= dataRect.left && x <= dataRect.right &&
                y >= dataRect.top && y <= dataRect.bottom)
            {
                m_config.m_AttackPerTurn = 5;
            }
        
            // Game Continue BUTTON
            if (x >= m_continueRect.left && x <= m_continueRect.right &&
                y >= m_continueRect.top && y <= m_continueRect.bottom)
            {
                if (!ValidFieldSize()) 
                {
                    MessageBox(hWnd, L"Field is too small for players and starting cells.",
                        L"Configuration Error", MB_OK | MB_ICONERROR);
                    InvalidateRect(hWnd, nullptr, FALSE);
                    return;
                }
                //resize edit and player vectors to match player size
                // Index 0 is reserved for the Game/System player.
                m_players.resize(m_config.m_ActivePlayers + 1);
                m_nameBoxes.resize(m_config.m_ActivePlayers + 1);
                int yStart = TOP_MARGIN;// to be removed

                m_GameConfigComplete = true;

                ApplyPlayerTypeRules();
            }
            InvalidateRect(hWnd, nullptr, FALSE);
        }
		else if (m_GameConfigComplete && !m_PlayerConfigComplete)
        {
            // PLAYER SETUP

            int yStart = TOP_MARGIN;

            for (size_t i = 1; i < m_players.size(); i++)
            {
                int yTop = yStart + (int)(i - 1) * ROW_HEIGHT;

                // TOGGLE HUMAN / AI
                if (x >= m_typeRect.left && x <= m_typeRect.right &&
                    y >= yTop && y <= yTop + BUTTON_H)
                {
                    m_players[i].m_isHuman = !m_players[i].m_isHuman;

                    ApplyPlayerTypeRules();
                    InvalidateRect(hWnd, nullptr, FALSE);
                    return;
                }
                // PERSONA
                if (!m_players[i].m_isHuman && x >= m_personaRect.left && x <= m_personaRect.right &&
                    y >= yTop && y <= yTop + BUTTON_H)
                {
                    m_personaIndex++;

                    if (m_personaIndex > 5)
                        m_personaIndex = 1;

                    m_players[i].m_persona = GetPersona((int)m_personaIndex);

                    const wchar_t* personaName =
                        m_personaNames[m_personaIndex - 1];

                    m_players[i].m_personaName =
                        std::wstring(personaName, personaName + wcslen(personaName));                    // For now, just record the selected persona number.
                    // Trait assignment comes later.


                    InvalidateRect(hWnd, nullptr, FALSE);
                    return;
                }
            }

            // Player Continue BUTTON
            if (x >= m_continueRect.left && x <= m_continueRect.right &&
                y >= m_continueRect.top && y <= m_continueRect.bottom)
            {
                // Save all player names before starting the game
                for (size_t i = 1; i < m_players.size(); i++)
                {
                    CommitTextBox((int)i);
                }

				m_PlayerConfigComplete = true;

                InvalidateRect(hWnd, nullptr, TRUE);
            }
        }
        else if (m_PlayerConfigComplete && m_GameConfigComplete)
        {
            // SUMMARY SCREEN

            if (x >= LABEL_X + SUMMARY_OFFSET_X && x <= gWidth - LABEL_X &&
                y >= TOP_MARGIN && y <= m_continueRect.top - 20)
            {
                m_GameConfigComplete = false;
                m_PlayerConfigComplete = false;

                InvalidateRect(hWnd, nullptr, TRUE);
                return;
            }

            if (x >= m_continueRect.left && x <= m_continueRect.right &&
                y >= m_continueRect.top && y <= m_continueRect.bottom)
            {
                ready = true;

                InvalidateRect(hWnd, nullptr, TRUE);
                return;
            }
        }
    }
    void OnMouseDown(int x, int y)
    {
        // Ignore until the player setup screen
        if (!m_GameConfigComplete)
            return;

        // Did we click on a textbox?
        int clicked = -1;

        for (size_t i = 1; i < m_nameBoxes.size(); i++)
        {
            if (m_nameBoxes[i].HitTest(x, y))
            {
                clicked = (int)i;
                break;
            }
        }

        // If focus is changing, commit the old textbox
        if (clicked != m_ActiveTextBox)
        {
            if (m_ActiveTextBox >= 1 &&
                m_ActiveTextBox < (int)m_nameBoxes.size())
            {
                CommitTextBox(m_ActiveTextBox);
                m_nameBoxes[m_ActiveTextBox].SetActive(false);
            }

            m_ActiveTextBox = clicked;

            if (m_ActiveTextBox >= 1 &&
                m_ActiveTextBox < (int)m_nameBoxes.size())
            {
                m_nameBoxes[m_ActiveTextBox].SetActive(true);
            }
        }
    }

    void OnChar(char ch)
    {
        if (m_ActiveTextBox >= 1 &&
            m_ActiveTextBox < (int)m_nameBoxes.size())
        {
            m_nameBoxes[m_ActiveTextBox].OnChar(ch);
        }
    }
    
    void ApplyPlayerTypeRules()
    {
        for (size_t i = 1; i < m_players.size(); i++)
        {
            if (i >= m_nameBoxes.size())
                continue;

            if (m_players[i].m_isHuman)
            {
                // HUMAN: no AI persona
                m_players[i].m_personaName = L"";
                m_players[i].m_persona = {};

                // HUMAN: textbox is editable
                m_nameBoxes[i].SetActive(false);
                // Note: active = focused, not enabled/disabled

                // If player already has a name, push it into UI once
                if (!m_players[i].m_PlayerName.empty())
                {
                    m_nameBoxes[i].SetText(m_players[i].m_PlayerName);
                }
            }
            else
            {
                // AI: auto-generate name if needed
                if (m_players[i].m_PlayerName.empty())
                {
                    m_players[i].m_PlayerName = GetRandomRobotName();
                }

                // force UI sync
                m_nameBoxes[i].SetText(m_players[i].m_PlayerName);
            }
        }
    }
    void InitRobotNamePool()
    {
        m_robotNamePool.clear();

        for (size_t i = 0; i < _countof(gRobotNames); i++)
        {
            std::wstring ws = gRobotNames[i];
            m_robotNamePool.push_back(std::string(ws.begin(), ws.end()));
        }

        std::shuffle( m_robotNamePool.begin(),
            m_robotNamePool.end(), gRNG );

        m_robotNameIndex = 0;
    }
    std::string GetRandomRobotName()
    {
        if (m_robotNamePool.empty())
            InitRobotNamePool();

        if (m_robotNameIndex >= m_robotNamePool.size())
            m_robotNameIndex = 0; // fallback safety (shouldn't happen in normal game)

        return m_robotNamePool[m_robotNameIndex++];
    }
    void CommitTextBox(int i)
    {
        if (i <= 0 || i >= (int)m_nameBoxes.size())
            return;

        m_players[i].m_PlayerName = m_nameBoxes[i].GetText();
    }
};

