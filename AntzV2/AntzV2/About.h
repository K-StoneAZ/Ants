#pragma once
#include "framework.h"
#include "AboutGame.h"

extern HBITMAP gSetupBackgroundBmp;
extern int gWidth;
extern int gHeight;


class About
{
private:
    HWND m_hWnd = nullptr;
    enum AboutPage
    {
        A_Menu,
        A_Rules,
        A_Controls,
        A_AI,
        A_HowTo,
        A_Game,
        A_Credits
    };
    AboutPage m_page = A_Menu;

    const wchar_t* m_menuItems[6] =
    {
        L"Rules",
        L"Controls",
        L"AI",
        L"How To",
        L"Game",
        L"Credits"
    };
    RECT m_menuRects[6] = {};
    enum HowToPage
    {
        H_Menu,
        H_Growth,
        H_Attack,
        H_Move,
        H_Defense,
        H_Strategy
    };
    HowToPage m_howToPage = H_Menu;
    const wchar_t* menuItems[5] =
    {
        L"Growth",
        L"Attack",
        L"Move",
        L"Defense",
        L"Strategy"
    };
    RECT m_howToRects[5] = {};
    struct HowToContent
    {
        const wchar_t* title;
        const wchar_t* paragraph1;
        const wchar_t* paragraph2;
        const wchar_t* paragraph3;
        const wchar_t* paragraph4;
    };

    AboutGame m_aboutGame;
    bool m_gameInit = false;
    HFONT m_textFont = nullptr;
	HFONT m_headerFont = nullptr;
	HFONT m_bodyFont = nullptr;
    const int TextSize = 22;
    const int HeaderSize = 20;
	const int BodySize = 18;
    int m_selectedItem = -1;
	int m_hoverItem = -1;
    bool m_visible = false;
    const COLORREF BODY_TEXT_COLOR = RGB(230, 210, 135);

    void DefineMenuRects()
    {
        RECT rc;
        GetClientRect(m_hWnd, &rc);

        const int menuWidth = 10 * TextSize;
        const int left = (rc.right - menuWidth) / 2;
        const int right = left + menuWidth;
        const int rowHeight = 35;
        const int totalHeight = rowHeight * 6;
        const int top = (rc.bottom - totalHeight) / 2;

        for (int i = 0; i < 6; ++i)
        {
            m_menuRects[i].left = left;
            m_menuRects[i].right = right;
            m_menuRects[i].top = top + i * rowHeight;
            m_menuRects[i].bottom = m_menuRects[i].top + rowHeight;
        }
    }

    void DefineFonts()
    {
        m_textFont = CreateFont(
            TextSize, 0, 0, 0,
            FW_NORMAL, FALSE, FALSE, FALSE,
            DEFAULT_CHARSET, OUT_DEFAULT_PRECIS,
            CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY,
            DEFAULT_PITCH, L"Arial");

        m_headerFont = CreateFont(
            HeaderSize, 0, 0, 0, FW_NORMAL,
            FALSE, FALSE, FALSE,
            DEFAULT_CHARSET, OUT_DEFAULT_PRECIS,
            CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY,
            DEFAULT_PITCH, L"Arial");

        m_bodyFont = CreateFont(
            BodySize, 0, 0, 0, FW_NORMAL,
            FALSE, FALSE, FALSE,
            DEFAULT_CHARSET, OUT_DEFAULT_PRECIS,
            CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY,
            DEFAULT_PITCH, L"Arial");
    }


    void Rules(HDC hdc)
    {
        SetBkMode(hdc, TRANSPARENT);

        HFONT oldFont = (HFONT)SelectObject(hdc, m_textFont);

        SetTextColor(hdc, RGB(255, 255, 255));

        // Page title
        SelectObject(hdc, m_headerFont);
        RECT titleRect = { 0, 115, gWidth, 150 };
        DrawText(hdc, L"RULES", -1, &titleRect, DT_CENTER | DT_SINGLELINE);

        // Content
        const int left = 180;
        const int right = gWidth - 180;

        int y = 180;

        SelectObject(hdc, m_headerFont);
        RECT rect = { left, y, right, y + 25 };
        DrawText(hdc, L"Objective", -1, &rect, DT_LEFT | DT_SINGLELINE);

        y += 28;
        SelectObject(hdc, m_bodyFont);
        rect = { left, y, right, y + 45 };
        DrawText(hdc,
            L"Control enough of the battlefield to win, or eliminate your opponents.",
            -1, &rect, DT_LEFT | DT_WORDBREAK);

        y += 75;

        SelectObject(hdc, m_headerFont);
        rect = { left, y, right, y + 25 };
        DrawText(hdc, L"The Battlefield", -1, &rect, DT_LEFT | DT_SINGLELINE);

        y += 28;
        SelectObject(hdc, m_bodyFont);
        rect = { left, y, right, y + 50 };
        DrawText(hdc,
            L"The battlefield is a grid of cells. Each cell contains ants and is either "
            L"unowned or controlled by a player.",
            -1, &rect, DT_LEFT | DT_WORDBREAK);

        y += 80;

        SelectObject(hdc, m_headerFont);
        rect = { left, y, right, y + 25 };
        DrawText(hdc, L"Your Turn", -1, &rect, DT_LEFT | DT_SINGLELINE);

        y += 28;
        SelectObject(hdc, m_bodyFont);
        rect = { left, y, right, y + 50 };
        DrawText(hdc,
            L"A turn proceeds through Growth, Attack, Move, and End. During Growth, "
            L"your territory produces additional ants.",
            -1, &rect, DT_LEFT | DT_WORDBREAK);

        y += 80;

        SelectObject(hdc, m_headerFont);
        rect = { left, y, right, y + 25 };
        DrawText(hdc, L"Attack and Capture", -1, &rect, DT_LEFT | DT_SINGLELINE);

        y += 28;
        SelectObject(hdc, m_bodyFont);
        rect = { left, y, right, y + 50 };
        DrawText(hdc,
            L"Attack adjacent enemy cells. Battles are resolved with attack and "
            L"defense rolls. A successful attack captures the target cell.",
            -1, &rect, DT_LEFT | DT_WORDBREAK);

        y += 80;

        SelectObject(hdc, m_headerFont);
        rect = { left, y, right, y + 25 };
        DrawText(hdc, L"Victory", -1, &rect, DT_LEFT | DT_SINGLELINE);

        y += 28;
        SelectObject(hdc, m_bodyFont);
        rect = { left, y, right, y + 50 };
        DrawText(hdc,
            L"The game ends when a player meets the selected victory condition: "
            L"Domination or Elimination.",
            -1, &rect, DT_LEFT | DT_WORDBREAK);

        SelectObject(hdc, oldFont);

    }

    void Controls(HDC hdc)
    {
        SetBkMode(hdc, TRANSPARENT);

        HFONT oldFont = (HFONT)SelectObject(hdc, m_textFont);

        SetTextColor(hdc, RGB(255, 255, 255));

        // Page title
        SelectObject(hdc, m_headerFont);
        RECT titleRect = { 0, 115, gWidth, 150 };
        DrawText(hdc, L"CONTROLS", -1, &titleRect, DT_CENTER | DT_SINGLELINE);

        // Content
        const int left = 450;
        const int right = gWidth - 350;

        int y = 180;

        SelectObject(hdc, m_headerFont);
        RECT rect = { left, y, right, y + 25 };
        DrawText(hdc, L"Setup", -1, &rect, DT_LEFT | DT_SINGLELINE);

        y += 28;
        SelectObject(hdc, m_bodyFont);
        rect = { left, y, right, y + 100 };
        DrawText(hdc,
            L"Mouse over to highlight.\n" L"Click to select or <Enter> to confirm.",
            -1, &rect, DT_LEFT | DT_WORDBREAK);

        y += 125;

        SelectObject(hdc, m_headerFont);
        rect = { left, y, right, y + 25 };
        DrawText(hdc, L"Gameplay", -1, &rect, DT_LEFT | DT_SINGLELINE);

        y += 28;
        SelectObject(hdc, m_bodyFont);
        rect = { left, y, right, y + 150 };
        DrawText(hdc,
            L"Mouse over to highlight.\n"
            L"Click to select.\n"
            L"Click or <Enter> to confirm.\n" L"Click or <Space> to skip.\n"
            L"Number keys to enter ant count.",
            -1, &rect, DT_LEFT | DT_WORDBREAK);

        y += 175;

        SelectObject(hdc, m_headerFont);
        rect = { left, y, right, y + 25 };
        DrawText(hdc, L"About", -1, &rect, DT_LEFT | DT_SINGLELINE);

        y += 28;
        SelectObject(hdc, m_bodyFont);
        rect = { left, y, right, y + 100 };
        DrawText(hdc,
            L"Mouse over to highlight.\n"
            L"Click or <Enter> to select.\n"
            L"Arrow keys to navigate and highlight.\n"
            L"Escape to return.",
            -1, &rect, DT_LEFT | DT_WORDBREAK);

        SelectObject(hdc, oldFont);
    }

    void AImenu(HDC hdc)
    {
        SetBkMode(hdc, TRANSPARENT);

        HFONT oldFont = (HFONT)SelectObject(hdc, m_textFont);

        SetTextColor(hdc, RGB(255, 255, 255));

        // Page title
        SelectObject(hdc, m_headerFont);
        RECT titleRect = { 0, 115, gWidth, 150 };
        DrawText(hdc, L"AI", -1, &titleRect, DT_CENTER | DT_SINGLELINE);

        // Content
        const int left = 350;
        const int right = gWidth - 250;

        int y = 180;

        SetTextColor(hdc, RGB(255, 215, 0));
        SelectObject(hdc, m_headerFont);
        RECT rect = { left, y, right, y + 25 };
        DrawText(hdc, L"Difficulty", -1, &rect, DT_LEFT | DT_SINGLELINE);
        SetTextColor(hdc, RGB(255, 255, 255));
        y += 30;

        SelectObject(hdc, m_bodyFont);

        rect = { left, y, right, y + 25 };
        DrawText(hdc, L"Easy", -1, &rect, DT_LEFT | DT_SINGLELINE);

        y += 25;
        rect = { left + 30, y, right, y + 30 };
        DrawText(hdc, L"Basic growth and attack choices.", -1, &rect,
            DT_LEFT | DT_SINGLELINE);

        y += 40;
        rect = { left, y, right, y + 25 };
        DrawText(hdc, L"Medium", -1, &rect, DT_LEFT | DT_SINGLELINE);

        y += 25;
        rect = { left + 30, y, right, y + 30 };
        DrawText(hdc, L"More strategic growth and attack choices.", -1, &rect,
            DT_LEFT | DT_SINGLELINE);

        y += 40;
        rect = { left, y, right, y + 25 };
        DrawText(hdc, L"Hard", -1, &rect, DT_LEFT | DT_SINGLELINE);

        y += 25;
        rect = { left + 30, y, right, y + 30 };
        DrawText(hdc, L"Advanced evaluation of growth and attack opportunities.",
            -1, &rect, DT_LEFT | DT_SINGLELINE);

        y += 55;

        SetTextColor(hdc, RGB(255, 215, 0));
        SelectObject(hdc, m_headerFont);
        rect = { left, y, right, y + 25 };
        DrawText(hdc, L"Personality", -1, &rect, DT_LEFT | DT_SINGLELINE);

        y += 30;

        SetTextColor(hdc, RGB(255, 255, 255));
        SelectObject(hdc, m_bodyFont);

        rect = { left, y, right, y + 25 };
        DrawText(hdc, L"Aggressive", -1, &rect, DT_LEFT | DT_SINGLELINE);

        y += 25;
        rect = { left + 30, y, right, y + 30 };
        DrawText(hdc, L"Favors offensive actions against other players.",
            -1, &rect, DT_LEFT | DT_SINGLELINE);

        y += 40;
        rect = { left, y, right, y + 25 };
        DrawText(hdc, L"Balanced", -1, &rect, DT_LEFT | DT_SINGLELINE);

        y += 25;
        rect = { left + 30, y, right, y + 30 };
        DrawText(hdc, L"Balances expansion, offense, and defense.",
            -1, &rect, DT_LEFT | DT_SINGLELINE);

        y += 40;
        rect = { left, y, right, y + 25 };
    }
    HowToContent m_howToContent[6] =
    {
        {},
        {
            L"GROWTH",
            L"At the beginning of your turn, you receive ants based on the territory "
            L"you control. You can place them on any of your cells, and you can divide "
            L"them between cells.",
            L"Consider which cells are most important to you. Cells near an opponent "
            L"may need extra strength, while a cell you plan to use for an attack or "
            L"movement may benefit from additional ants.",
            L"You do not have to place all of your growth in one cell. Look at the "
            L"board, consider what may happen next, and strengthen the positions that "
            L"matter to you most.", L""
        },
        {
            L"ATTACK",
        L"Attacking gives you a chance to capture territory from an opponent. "
        L"Before attacking, consider what the attack will accomplish and whether "
        L"the target is worth the ants you commit.",

        L"You can choose how many ants to send into an attack. You do not have to "
        L"commit every available ant, so consider how much strength you want to "
        L"leave behind.",

        L"A successful attack captures the target cell and can change the shape "
        L"of the board in your favor. A failed attack costs the ants you committed.",

        L"Look at the board, consider what you want to accomplish, and choose your "
        L"attacks carefully."
        },
        {
            L"MOVE",
        L"Moving lets you redistribute ants between your cells. You can move ants "
        L"from one of your cells to an adjacent cell that you also control.",

        L"You can choose how many ants to move, so consider how much strength you "
        L"want to leave in the source cell and how much you want to add to the "
        L"destination.",

        L"A move can prepare a cell for an attack, reinforce an important position, "
        L"or simply put your ants where they are more useful. Be careful not to move "
        L"ants so far from the action that they become difficult to use later.",

        L"Look at the board, consider what you want to accomplish, and move your "
        L"ants with a purpose."
        },
        {
            L"DEFENSE",
        L"Defense is about recognizing threats before they become problems. Pay "
        L"attention to where opponents are building strength and which of your "
        L"cells may become vulnerable.",

        L"Consider the importance of each position, the strength of nearby opponents, "
        L"and how easily you can reinforce a cell if it comes under pressure.",

        L"Not every threat requires a response. The board can change quickly, so "
        L"consider what is at risk and what you may need to protect.",

        L"Good defense is not simply keeping ants safe. It is keeping your territory "
        L"in a position where you can respond to what happens next."
        },
        {
            L"STRATEGY",
        L"There is no single way to play Antz. Some players prefer an aggressive "
        L"style, looking for opportunities to expand and attack. Others prefer a "
        L"defensive style, concentrating on protecting their territory and responding "
        L"to threats. A balanced style combines elements of both.",

        L"Your own style may develop over time. You may find that you naturally favor "
        L"one approach, or that your style changes as the game develops.",

        L"Whatever your style, pay attention to the board, consider what your opponents "
        L"are trying to accomplish, and think about how your choices affect what you "
        L"can do next.",

        L"The best strategy is the one that fits the way you want to play."
        }
    };


    void HowTo(HDC hdc)
    {
        SetBkMode(hdc, TRANSPARENT);
        HFONT oldFont = (HFONT)SelectObject(hdc, m_textFont);
        SetTextColor(hdc, RGB(255, 255, 255));
        // Page title
        RECT titleRect = { 0, 115, gWidth, 150 };
        DrawText(hdc, L"HOW TO", -1, &titleRect, DT_CENTER | DT_SINGLELINE);
        // Content

        int menuWidth = 10 * TextSize;
        int rowHeight = 35;
        int menuHeight = 5 * rowHeight;

        int left = (gWidth - menuWidth) / 2;
        int top = (gHeight - menuHeight) / 2;

        for (int i = 0; i < 5; ++i)
        {
            m_howToRects[i] = { left, top + (i * rowHeight),
                          left + menuWidth, top + ((i + 1) * rowHeight) };

            RECT rect = m_howToRects[i];

            if (i == m_selectedItem || i == m_hoverItem)
            {
                SetTextColor(hdc, RGB(0, 255, 255));
            }
            else
            {
                SetTextColor(hdc, RGB(255, 255, 255));
            }

            DrawText(hdc, menuItems[i], -1, &rect,
                DT_CENTER | DT_SINGLELINE | DT_VCENTER);
        }

        SelectObject(hdc, oldFont);
	}

    void HowTopage(HDC hdc)
    {
        const HowToContent& content = m_howToContent[m_howToPage];

        SetBkMode(hdc, TRANSPARENT);

        HFONT oldFont = (HFONT)SelectObject(hdc, m_textFont);

        SetTextColor(hdc, RGB(255, 255, 255));

        // Page title
        SelectObject(hdc, m_headerFont);
        RECT titleRect = { 0, 115, gWidth, 150 };
        DrawText(hdc, content.title, -1, &titleRect, DT_CENTER | DT_SINGLELINE);

        // Content
        const int left = 250;
        const int right = gWidth - 250;

        int y = 250;

        SelectObject(hdc, m_bodyFont);
        SetTextColor(hdc, BODY_TEXT_COLOR);
        RECT rect = { left, y, right, y + 80 };
        DrawText(hdc, content.paragraph1, -1, &rect, DT_LEFT | DT_WORDBREAK);

        y += 100;

        rect = { left, y, right, y + 100 };
        DrawText(hdc, content.paragraph2, -1, &rect, DT_LEFT | DT_WORDBREAK);

        y += 100;

        rect = { left, y, right, y + 100 };
        DrawText(hdc, content.paragraph3, -1, &rect, DT_LEFT | DT_WORDBREAK);

        y += 100;

        rect = { left, y, right, y + 100 };
        DrawText(hdc, content.paragraph4, -1, &rect, DT_LEFT | DT_WORDBREAK);

        SelectObject(hdc, oldFont);
   }

    void Credits(HDC hdc)
    {
        SetBkMode(hdc, TRANSPARENT);

        HFONT oldFont = (HFONT)SelectObject(hdc, m_textFont);

        SetTextColor(hdc, RGB(255, 255, 255));

        // Page title

        RECT titleRect = { 0, 115, gWidth, 150 };
        DrawText(hdc, L"Credits", -1, &titleRect, DT_CENTER | DT_SINGLELINE);

        // Credits
        SelectObject(hdc, m_headerFont);
        SetTextColor(hdc, BODY_TEXT_COLOR);

        RECT rect = { 350, 350, gWidth - 350, 600 };

        DrawText(
            hdc,
            L"Inspired by the 1990's Flash game Ants and the Board Game 'Risk', "
            L"with the goal of creating a simple, approachable game "
            L"of territory, growth, and strategy.\n\n"
            L"Created by\n"
            L"Kelly\n\n"
            L"AI assistance by\n"
            L"OpenAI",
            -1,
            &rect,
            DT_CENTER | DT_WORDBREAK
        );

        SelectObject(hdc, oldFont);
    }


    void RenderBackground(HDC hdc)
    {
        if (!gSetupBackgroundBmp)
            return;

        HDC memDC = CreateCompatibleDC(hdc);

        HBITMAP oldBitmap =
            (HBITMAP)SelectObject(memDC, gSetupBackgroundBmp);

        BITMAP bm;
        GetObject(gSetupBackgroundBmp, sizeof(bm), &bm);

        StretchBlt(
            hdc, 0, 0, gWidth, gHeight,
            memDC, 0, 0, bm.bmWidth, bm.bmHeight,
            SRCCOPY);

        SelectObject(memDC, oldBitmap);
        DeleteDC(memDC);
    }

public:
    bool Initialize(HWND hWnd)
    {
        m_hWnd = hWnd;
		DefineFonts();
        DefineMenuRects();
        return true;
    }

    void Show()
    {
        m_visible = true;
        m_page = A_Menu;
        m_selectedItem = -1;
        InvalidateRect(m_hWnd, nullptr, FALSE);
    }

    bool IsVisible() const
    {
        return m_visible;
    }

    bool IsGamePage() const
    {
        return m_visible && m_page == A_Game;
    }

    void Update()
    {
        if (IsGamePage())
        {
            m_aboutGame.Update();
        }
    }

    void StartAbout()
    {
        m_aboutGame.Start();
    }

    void StopAbout()
    {
        m_aboutGame.Stop();
    }


    void Paint(HDC hdc)
    {
        RenderBackground(hdc);

        HFONT oldFont = (HFONT)SelectObject(hdc, m_textFont);

        SetBkMode(hdc, TRANSPARENT);

        if (m_page == A_Rules)
        {
            Rules(hdc);
        }
        else if (m_page == A_Controls)
        {
            Controls(hdc);
        }
        else if (m_page == A_AI)
        {
            AImenu(hdc);
        }
        else if (m_page == A_HowTo)
        {
            if (m_howToPage != H_Menu)
            {
                HowTopage(hdc);
            }
            else
            {
                HowTo(hdc);
            }
        }
        else if (m_page == A_Game)
        {
            if (!m_gameInit)
            {
                m_aboutGame.Initialize(hdc);
                m_gameInit = true;
            }

            m_aboutGame.Render();
        }
        else if (m_page == A_Credits)
        {
            Credits(hdc);
        }
        else
        {
            for (int i = 0; i < 6; ++i)
            {
                RECT textRect = m_menuRects[i];

                if (i == m_selectedItem || i == m_hoverItem)
                {
                    SetTextColor(hdc, RGB(0, 255, 255)); // cyan
                }
                else
                {
                    SetTextColor(hdc, RGB(255, 255, 255)); // white
                }

                DrawText(
                    hdc,
                    m_menuItems[i],
                    -1,
                    &textRect,
                    DT_CENTER | DT_VCENTER | DT_SINGLELINE);
            }
        }
        SelectObject(hdc, oldFont);
    }

    bool HandleMouse(UINT message, WPARAM wParam, LPARAM lParam)
    {
        if (!m_visible)
        {
            return false;
        }
        if (m_page != A_Menu)
        {
            if (m_page == A_HowTo)
            {
                // allow How To mouse handling below
            }
            else if (message == WM_LBUTTONDOWN)
            {
                if (m_page == A_Game)
                {
                    StopAbout();
                }
                m_page = A_Menu;
                InvalidateRect(m_hWnd, nullptr, FALSE);
                return true;
            }
            else
            {
                return false;
            }
        }

        if (message == WM_MOUSEMOVE)
        {
            int x = LOWORD(lParam);
            int y = HIWORD(lParam);

            int hoverItem = -1;
            if (m_page == A_Menu)
            {
                for (int i = 0; i < 6; ++i)
                {
                    if (PtInRect(&m_menuRects[i], POINT{ x, y }))
                    {
                        hoverItem = i;
                        break;
                    }
                }
            }
            else if (m_page == A_HowTo)
            {
                for (int i = 0; i < 5; ++i)
                {
                    if (PtInRect(&m_howToRects[i], POINT{ x, y }))
                    {
                        hoverItem = i;
                        break;
                    }
                }
            }

            if (hoverItem != m_hoverItem)
            {
                m_hoverItem = hoverItem;
                InvalidateRect(m_hWnd, nullptr, FALSE);
            }
        }

        if (message == WM_LBUTTONDOWN)
        {
            int x = LOWORD(lParam);
            int y = HIWORD(lParam);

            if (m_page == A_HowTo && m_howToPage != H_Menu)
            {
                m_howToPage = H_Menu;
                m_selectedItem = -1;
                m_hoverItem = -1;
                InvalidateRect(m_hWnd, nullptr, FALSE);
                return true;
            }

            if (m_page == A_HowTo)
            {
                for (int i = 0; i < 5; ++i)
                {
                    if (PtInRect(&m_howToRects[i], POINT{ x, y }))
                    {
                        m_howToPage = static_cast<HowToPage>(i + 1);
                        InvalidateRect(m_hWnd, nullptr, FALSE);
                        return true;
                    }
                }
                m_page = A_Menu;
                m_selectedItem = -1;
                m_hoverItem = -1;
                InvalidateRect(m_hWnd, nullptr, FALSE);
                return true;
            }

            for (int i = 0; i < 6; ++i)
            {
                if (PtInRect(&m_menuRects[i], POINT{ x, y }))
                {
                    if (m_page == A_Game)
                    {
                        StopAbout();
                    }
                    m_page = static_cast<AboutPage>(i + 1);
                    if (m_page == A_Game)
                    {
                        StartAbout();
                    }
                    if (m_page == A_HowTo)
                    {
                        m_selectedItem = -1;
                        m_hoverItem = -1;
                    }
                    InvalidateRect(m_hWnd, nullptr, FALSE);
                    return true;
                }
            }
            if (m_page == A_Game)
            {
                StopAbout();
            }
            m_visible = false;
            InvalidateRect(m_hWnd, nullptr, FALSE);
            return true;
        }

        return false;
    }

    bool HandleKey(UINT message, WPARAM wParam, LPARAM lParam)
    {
        if (message != WM_KEYDOWN)
        {
            return false;
        }

        if (!m_visible)
        {
            return false;
        }

        if (wParam == VK_ESCAPE)
        {
            if (m_page == A_HowTo && m_howToPage != H_Menu)
            {
                m_howToPage = H_Menu;
                m_selectedItem = -1;
                m_hoverItem = -1;
                InvalidateRect(m_hWnd, nullptr, FALSE);
                return true;
            }

            if (m_page != A_Menu)
            {
                if (m_page == A_Game)
                {
                    StopAbout();
                }
                m_page = A_Menu;
                InvalidateRect(m_hWnd, nullptr, FALSE);
                return true;
            }

            m_visible = false;

            InvalidateRect(m_hWnd, nullptr, FALSE);
            return true;
        }

        if (m_page != A_Menu)
        {
            return false;
        }

        switch (wParam)
        {
        case VK_UP:
            m_selectedItem = (m_selectedItem + 5) % 6;
            InvalidateRect(m_hWnd, nullptr, FALSE);
            return true;

        case VK_DOWN:
            m_selectedItem = (m_selectedItem + 1) % 6;
            InvalidateRect(m_hWnd, nullptr, FALSE);
            return true;

        case VK_RETURN:
            m_page = static_cast<AboutPage>(m_selectedItem + 1);
            InvalidateRect(m_hWnd, nullptr, FALSE);
            return true;
        
        }

        return false;
    }

};

