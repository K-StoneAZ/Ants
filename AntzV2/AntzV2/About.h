#pragma once
#include "framework.h"

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

    HFONT m_textFont = nullptr;
    const int TextSize = 22;
    int m_selectedItem = 0;
	int m_hoverItem = -1;
    bool m_visible = false;

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
        m_selectedItem = 0;
        InvalidateRect(m_hWnd, nullptr, FALSE);
    }

    bool IsVisible() const
    {
        return m_visible;
    }

    void Paint(HDC hdc)
    {
        RenderBackground(hdc);

        HFONT oldFont = (HFONT)SelectObject(hdc, m_textFont);

        SetBkMode(hdc, TRANSPARENT);

        if (m_page != A_Menu)
        {
            // temporary page
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
            if (message == WM_LBUTTONDOWN)
                {
                    m_page = A_Menu;
                    InvalidateRect(m_hWnd, nullptr, FALSE);
                    return true;
                }            
             return false;
        }

        if (message == WM_MOUSEMOVE)
        {
            int x = LOWORD(lParam);
            int y = HIWORD(lParam);

            int hoverItem = -1;

            for (int i = 0; i < 6; ++i)
            {
                if (PtInRect(&m_menuRects[i], POINT{ x, y }))
                {
                    hoverItem = i;
                    break;
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

            for (int i = 0; i < 6; ++i)
            {
                if (PtInRect(&m_menuRects[i], POINT{ x, y }))
                {
                    m_page = static_cast<AboutPage>(i + 1);
                    InvalidateRect(m_hWnd, nullptr, FALSE);
                    return true;
                }
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
            if (m_page != A_Menu)
            {
                m_page = A_Menu;
            }
            else
            {
                m_visible = false;
            }

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

