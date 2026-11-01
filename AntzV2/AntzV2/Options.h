#pragma once
#include <Windows.h>
#include <cstdio>
#include <sstream>

class Options
{
private:
    bool m_musicEnabled = true;
    bool m_effectsEnabled = true;

    int m_musicVolume = 70;
    int m_effectsVolume = 100;

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

        if (_wfopen_s(&file, filePath, L"r") != 0 || file == nullptr)
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


};

