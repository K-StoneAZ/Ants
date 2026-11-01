#pragma once
#include <Windows.h>


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

    void Load(const char* filePath)
    {
        m_musicEnabled = GetPrivateProfileIntA(
            "Audio", "MusicEnabled", 1, filePath) != 0;

        m_effectsEnabled = GetPrivateProfileIntA(
            "Audio", "EffectsEnabled", 1, filePath) != 0;

        m_musicVolume = GetPrivateProfileIntA(
            "Audio", "MusicVolume", 70, filePath);

        m_effectsVolume = GetPrivateProfileIntA(
            "Audio", "EffectsVolume", 100, filePath);

        SetMusicVolume(m_musicVolume);
        SetEffectsVolume(m_effectsVolume);
    }


};

