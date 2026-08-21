#pragma once
//TextBox.h Win32 AntzV2 Game
#include <windows.h>
#include <string>

class TextBox
{
private:

    RECT m_rect{};

    std::string m_text;

    bool m_active = false;

    bool m_numbersOnly = false;

    int m_maxLength = 20;

public:

    TextBox() = default;
    TextBox(int x, int y, int w, int h)
    {
        SetRect(x, y, w, h);
    }

    void SetRect(int x, int y, int w, int h)
    {
        m_rect.left = x; m_rect.top = y;
        m_rect.right = x + w; m_rect.bottom = y + h;
    }
    void SetRect(const RECT& rect)
    {
        m_rect = rect;
    }
    void SetActive(bool active){ m_active = active;}

    void SetText(const std::string& text)
    {
        m_text = text;

        if ((int)m_text.size() > m_maxLength)
            m_text = m_text.substr(0, m_maxLength);
    }

    void SetNumbersOnly(bool numbersOnly) { m_numbersOnly = numbersOnly; }

    const std::string& GetText() const { return m_text; }

    bool IsActive() const { return m_active; }

    void OnChar(char ch)
    {
        if (!m_active)
            return;
        // backspace
        if (ch == 8)
        {
            if (!m_text.empty())
                m_text.pop_back();
            return;
        }
        // ignore enter
        if (ch == 13)
            return;
        // numeric filter
        if (m_numbersOnly)
        {
            if (ch < '0' || ch > '9')
                return;
        }
        // max length
        if ((int)m_text.size() >= m_maxLength)
            return;
        m_text.push_back(ch);
    }

    void OnKeyDown(UINT key)
    {
        //Reserved for special keys 
    }

    bool HitTest(int x, int y) const {
        return (x >= m_rect.left && x <= m_rect.right &&
            y >= m_rect.top && y <= m_rect.bottom);
    }
    void Draw(HDC hdc)
    {
        HBRUSH bg = CreateSolidBrush(
            m_active ? RGB(200, 220, 255) : RGB(240, 240, 240));
        HGDIOBJ old = SelectObject(hdc, bg);

        FillRect(hdc, &m_rect, bg);
        SelectObject(hdc, old);
        DeleteObject(bg);

        Rectangle(hdc, m_rect.left, m_rect.top, m_rect.right, m_rect.bottom);

        SetBkMode(hdc, TRANSPARENT);

        DrawTextA(
            hdc,
            m_text.c_str(),
            -1,
            &m_rect,
            DT_CENTER | DT_VCENTER | DT_SINGLELINE
        );

        if (m_active)
        {
            SIZE textSize{};

            GetTextExtentPoint32A( hdc, m_text.c_str(), (int)m_text.size(), &textSize );

            int caretX = m_rect.left + ((m_rect.right - m_rect.left) - textSize.cx) / 2 +
                textSize.cx;

            int caretY1 = m_rect.top + 4;
            int caretY2 = m_rect.bottom - 4;

            MoveToEx(hdc, caretX, caretY1, nullptr);
            LineTo(hdc, caretX, caretY2);
        }
    }
};

