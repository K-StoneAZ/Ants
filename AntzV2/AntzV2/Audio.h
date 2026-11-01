#pragma once
#include "miniaudio.h"

class Audio
{
private:
    ma_engine m_engine{};
    bool m_initialized = false;
    bool m_musicEnabled = true;
    bool m_effectsEnabled = true;

    int m_musicVolume = 70;
    int m_effectsVolume = 100;

public:
    bool Initialize()
    {
        if (m_initialized)
            return true;

        if (ma_engine_init(nullptr, &m_engine) != MA_SUCCESS)
            return false;

        m_initialized = true;
        return true;
    }

    bool PlayFile(const char* filePath)
    {
        if (!m_initialized)
            return false;

        return ma_engine_play_sound(&m_engine, filePath, nullptr) == MA_SUCCESS;
    }

    void SetPreferences( bool musicEnabled, bool effectsEnabled,
        int musicVolume, int effectsVolume)
    {
        m_musicEnabled = musicEnabled;
        m_effectsEnabled = effectsEnabled;

        m_musicVolume = (musicVolume < 0) ? 0 :
            (musicVolume > 100) ? 100 : musicVolume;

        m_effectsVolume = (effectsVolume < 0) ? 0 :
            (effectsVolume > 100) ? 100 : effectsVolume;
    }
    bool MusicEnabled() const
    {
        return m_musicEnabled;
    }

    bool EffectsEnabled() const
    {
        return m_effectsEnabled;
    }

    int GetMusicVolume() const
    {
        return m_musicVolume;
    }

    int GetEffectsVolume() const
    {
        return m_effectsVolume;
    }

    void Shutdown()
    {
        if (m_initialized)
        {
            ma_engine_uninit(&m_engine);
            m_initialized = false;
        }
    }

};

