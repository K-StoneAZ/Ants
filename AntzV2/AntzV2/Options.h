#pragma once

#include "framework.h"
#include <cstdio>
#include "Audio.h"

extern HBITMAP gSetupBackgroundBmp;
extern int gWidth;
extern int gHeight;
extern Audio gAudio;

class Options
{
private:
    // Saved application preferences.
    bool m_musicEnabled = true;
    bool m_effectsEnabled = true;

    int m_musicVolume = 70;
    int m_effectsVolume = 100;

    // Temporary values used while the Options screen is open.
    bool m_tempMusicEnabled = true;
    bool m_tempEffectsEnabled = true;

    int m_tempMusicVolume = 70;
    int m_tempEffectsVolume = 100;

    HWND m_hWnd = nullptr;
    bool m_visible = false;
    bool m_draggingMusic = false;
    bool m_draggingEffects = false;

    const wchar_t* m_filePath = nullptr;

    HFONT m_textFont = nullptr;
    HFONT m_headerFont = nullptr;

    RECT m_musicCheckRect = {};
    RECT m_effectsCheckRect = {};

    RECT m_musicTrackRect = {};
    RECT m_effectsTrackRect = {};

    RECT m_okRect = {};
    RECT m_cancelRect = {};

    const COLORREF HIGHLIGHT_COLOR = RGB(0, 255, 255);
    const COLORREF TEXT_COLOR = RGB(255, 255, 255);
    const COLORREF BODY_TEXT_COLOR = RGB(230, 210, 135);

    void DefineFonts()
    {
        if (m_textFont)
            return;

        m_textFont = CreateFont(
            22, 0, 0, 0, FW_NORMAL,
            FALSE, FALSE, FALSE,
            DEFAULT_CHARSET, OUT_DEFAULT_PRECIS,
            CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY,
            DEFAULT_PITCH, L"Arial");

        m_headerFont = CreateFont(
            28, 0, 0, 0, FW_NORMAL,
            FALSE, FALSE, FALSE,
            DEFAULT_CHARSET, OUT_DEFAULT_PRECIS,
            CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY,
            DEFAULT_PITCH, L"Arial");
    }

    void DefineRects()
    {
        const int centerX = gWidth / 2;

        m_musicCheckRect =
        {
            centerX - 250, 270,
            centerX - 220, 300
        };

        m_effectsCheckRect =
        {
            centerX - 250, 440,
            centerX - 220, 470
        };

        m_musicTrackRect =
        {
            centerX - 100, 340,
            centerX + 200, 352
        };

        m_effectsTrackRect =
        {
            centerX - 100, 510,
            centerX + 200, 522
        };

        m_okRect =
        {
            centerX - 150, 650,
            centerX - 20, 700
        };

        m_cancelRect =
        {
            centerX + 20, 650,
            centerX + 150, 700
        };
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

    void DrawCheckbox(HDC hdc, const RECT& rect, bool checked)
    {
        Rectangle(
            hdc,
            rect.left,
            rect.top,
            rect.right,
            rect.bottom);

        if (checked)
        {
            MoveToEx(hdc, rect.left + 5, rect.top + 15, nullptr);
            LineTo(hdc, rect.left + 12, rect.bottom - 5);
            LineTo(hdc, rect.right - 4, rect.top + 5);
        }
    }

    void DrawSlider(
        HDC hdc,
        const RECT& track,
        int value)
    {
        // Track
        RECT trackRect = track;
        FillRect(hdc, &trackRect,
            (HBRUSH)GetStockObject(WHITE_BRUSH));

        // Filled portion
        int thumbX = track.left +
            ((track.right - track.left) * value / 100);

        RECT filledRect =
        {
            track.left,
            track.top,
            thumbX,
            track.bottom
        };

        HBRUSH fillBrush = CreateSolidBrush(HIGHLIGHT_COLOR);
        FillRect(hdc, &filledRect, fillBrush);
        DeleteObject(fillBrush);

        // Slider thumb
        RECT thumbRect =
        {
            thumbX - 7,
            track.top - 7,
            thumbX + 7,
            track.bottom + 7
        };

        HBRUSH thumbBrush =
            CreateSolidBrush(RGB(255, 255, 255));

        FillRect(hdc, &thumbRect, thumbBrush);
        DeleteObject(thumbBrush);
    }

    int GetSliderValue(const RECT& track, int mouseX)
    {
        int width = track.right - track.left;

        if (mouseX < track.left)
            mouseX = track.left;

        if (mouseX > track.right)
            mouseX = track.right;

        return ((mouseX - track.left) * 100) / width;
    }

    void UpdateMusicSlider(int mouseX)
    {
        m_tempMusicVolume =
            GetSliderValue(m_musicTrackRect, mouseX);
    }

    void UpdateEffectsSlider(int mouseX)
    {
        m_tempEffectsVolume =
            GetSliderValue(m_effectsTrackRect, mouseX);
    }

    void CommitChanges()
    {
        m_musicEnabled = m_tempMusicEnabled;
        m_effectsEnabled = m_tempEffectsEnabled;

        SetMusicVolume(m_tempMusicVolume);
        SetEffectsVolume(m_tempEffectsVolume);

        gAudio.SetPreferences( m_musicEnabled, m_effectsEnabled,
            m_musicVolume, m_effectsVolume);

        if (m_filePath)
            Save(m_filePath);

        Close();
    }

    void Close()
    {
        m_visible = false;
        m_draggingMusic = false;
        m_draggingEffects = false;

        ReleaseCapture();
        InvalidateRect(m_hWnd, nullptr, FALSE);
    }

public:

    bool IsMusicEnabled() const
    {
        return m_musicEnabled;
    }

    void SetMusicEnabled(bool enabled)
    {
        m_musicEnabled = enabled;
    }

    bool AreEffectsEnabled() const
    {
        return m_effectsEnabled;
    }

    void SetEffectsEnabled(bool enabled)
    {
        m_effectsEnabled = enabled;
    }

    int GetMusicVolume() const
    {
        return m_musicVolume;
    }

    void SetMusicVolume(int volume)
    {
        if (volume < 0)
            volume = 0;

        if (volume > 100)
            volume = 100;

        m_musicVolume = volume;
    }

    int GetEffectsVolume() const
    {
        return m_effectsVolume;
    }

    void SetEffectsVolume(int volume)
    {
        if (volume < 0)
            volume = 0;

        if (volume > 100)
            volume = 100;

        m_effectsVolume = volume;
    }

    void Load(const wchar_t* filePath)
    {
        FILE* file = nullptr;

        if (_wfopen_s(&file, filePath, L"r") != 0 ||
            file == nullptr)
            return;

        int value = 0;

        if (fscanf_s(file, "%d", &value) == 1)
            m_musicEnabled = (value != 0);

        if (fscanf_s(file, "%d", &value) == 1)
            m_effectsEnabled = (value != 0);

        if (fscanf_s(file, "%d", &value) == 1)
            SetMusicVolume(value);

        if (fscanf_s(file, "%d", &value) == 1)
            SetEffectsVolume(value);

        fclose(file);
    }

    void Save(const wchar_t* filePath)
    {
        FILE* file = nullptr;

        if (_wfopen_s(&file, filePath, L"w") != 0 ||
            file == nullptr)
            return;

        fprintf(file, "%d\n", m_musicEnabled ? 1 : 0);
        fprintf(file, "%d\n", m_effectsEnabled ? 1 : 0);
        fprintf(file, "%d\n", m_musicVolume);
        fprintf(file, "%d\n", m_effectsVolume);

        fclose(file);
    }

    void Show(HWND hWnd, const wchar_t* filePath)
    {
        m_hWnd = hWnd;
        m_filePath = filePath;

        DefineFonts();
        DefineRects();

        // Start with the currently saved values.
        m_tempMusicEnabled = m_musicEnabled;
        m_tempEffectsEnabled = m_effectsEnabled;

        m_tempMusicVolume = m_musicVolume;
        m_tempEffectsVolume = m_effectsVolume;

        m_draggingMusic = false;
        m_draggingEffects = false;

        m_visible = true;

        InvalidateRect(m_hWnd, nullptr, FALSE);
    }

    bool IsVisible() const
    {
        return m_visible;
    }

    void Paint(HDC hdc)
    {
        if (!m_visible)
            return;

        RenderBackground(hdc);

        SetBkMode(hdc, TRANSPARENT);

        HFONT oldFont = (HFONT)SelectObject(hdc, m_textFont);

        // Screen title
        SelectObject(hdc, m_headerFont);
        SetTextColor(hdc, TEXT_COLOR);

        RECT titleRect = { 0, 115, gWidth, 155 };

        DrawText(
            hdc,
            L"APPLICATION OPTIONS",
            -1,
            &titleRect,
            DT_CENTER | DT_SINGLELINE);

        SelectObject(hdc, m_textFont);

        // Background Music
        SetTextColor(hdc, BODY_TEXT_COLOR);

        RECT musicLabel = { gWidth / 2 - 210, 265,
                            gWidth / 2 + 200, 305 };

        DrawText(
            hdc,
            L"Background Music",
            -1,
            &musicLabel,
            DT_LEFT | DT_VCENTER | DT_SINGLELINE);

        SetTextColor(hdc, TEXT_COLOR);

        DrawCheckbox(hdc, m_musicCheckRect, m_tempMusicEnabled);

        DrawSlider(hdc, m_musicTrackRect, m_tempMusicVolume);

        RECT musicValue =
        {
            m_musicTrackRect.right + 20,
            325,
            m_musicTrackRect.right + 80,
            365
        };

        wchar_t valueText[16];
        swprintf_s(valueText, L"%d%%", m_tempMusicVolume);

        DrawText(
            hdc,
            valueText,
            -1,
            &musicValue,
            DT_LEFT | DT_VCENTER | DT_SINGLELINE);

        // Sound Effects
        SetTextColor(hdc, BODY_TEXT_COLOR);

        RECT effectsLabel = { gWidth / 2 - 210, 435,
                              gWidth / 2 + 200, 475 };

        DrawText(
            hdc,
            L"Sound Effects",
            -1,
            &effectsLabel,
            DT_LEFT | DT_VCENTER | DT_SINGLELINE);

        SetTextColor(hdc, TEXT_COLOR);

        DrawCheckbox(hdc, m_effectsCheckRect, m_tempEffectsEnabled);

        DrawSlider(hdc, m_effectsTrackRect, m_tempEffectsVolume);

        RECT effectsValue =
        {
            m_effectsTrackRect.right + 20,
            495,
            m_effectsTrackRect.right + 80,
            535
        };

        swprintf_s(valueText, L"%d%%", m_tempEffectsVolume);

        DrawText(
            hdc,
            valueText,
            -1,
            &effectsValue,
            DT_LEFT | DT_VCENTER | DT_SINGLELINE);

        // OK and Cancel
        SetTextColor(hdc, HIGHLIGHT_COLOR);

        RECT okText = m_okRect;
        DrawText(
            hdc,
            L"OK",
            -1,
            &okText,
            DT_CENTER | DT_VCENTER | DT_SINGLELINE);

        RECT cancelText = m_cancelRect;
        DrawText(
            hdc,
            L"Cancel",
            -1,
            &cancelText,
            DT_CENTER | DT_VCENTER | DT_SINGLELINE);

        SelectObject(hdc, oldFont);
    }

    bool HandleMouse(UINT message, WPARAM wParam, LPARAM lParam)
    {
        if (!m_visible)
            return false;

        int x = LOWORD(lParam);
        int y = HIWORD(lParam);

        POINT pt = { x, y };

        if (message == WM_MOUSEMOVE)
        {
            if (m_draggingMusic)
            {
                UpdateMusicSlider(x);
                InvalidateRect(m_hWnd, nullptr, FALSE);
                return true;
            }

            if (m_draggingEffects)
            {
                UpdateEffectsSlider(x);
                InvalidateRect(m_hWnd, nullptr, FALSE);
                return true;
            }

            return true;
        }

        if (message == WM_LBUTTONUP)
        {
            if (m_draggingMusic || m_draggingEffects)
            {
                m_draggingMusic = false;
                m_draggingEffects = false;

                ReleaseCapture();
                InvalidateRect(m_hWnd, nullptr, FALSE);
                return true;
            }

            return true;
        }

        if (message != WM_LBUTTONDOWN)
            return false;

        if (PtInRect(&m_musicCheckRect, pt))
        {
            m_tempMusicEnabled = !m_tempMusicEnabled;
        }
        else if (PtInRect(&m_effectsCheckRect, pt))
        {
            m_tempEffectsEnabled = !m_tempEffectsEnabled;
        }
        else if (PtInRect(&m_musicTrackRect, pt))
        {
            m_draggingMusic = true;
            UpdateMusicSlider(x);
            SetCapture(m_hWnd);
        }
        else if (PtInRect(&m_effectsTrackRect, pt))
        {
            m_draggingEffects = true;
            UpdateEffectsSlider(x);
            SetCapture(m_hWnd);
        }
        else if (PtInRect(&m_okRect, pt))
        {
            CommitChanges();
            return true;
        }
        else if (PtInRect(&m_cancelRect, pt))
        {
            Close();
            return true;
        }

        InvalidateRect(m_hWnd, nullptr, FALSE);
        return true;
    }

    bool HandleKey(UINT message, WPARAM wParam, LPARAM lParam)
    {
        if (!m_visible || message != WM_KEYDOWN)
            return false;

        if (wParam == VK_ESCAPE)
        {
            Close();
            return true;
        }

        return true;
    }
};

