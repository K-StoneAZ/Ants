#pragma once
#include "miniaudio.h"
#include <memory>
#include <string>
#include <vector>
#include <algorithm>

class Audio
{
private:
    struct SoundInfo
    {
        std::string fname;
        bool type;
    };

    const SoundInfo soundInventory[6] =
    {
        { "Alarm01.wav",    false },
        { "Failed.wav",     false },
        { "Growth.wav",     false },
        { "Marching.wav",   false },
        { "Success.wav",    false },
        { "TitleMusic.wav", true  }
    };

    struct LoadSound
    {
        ma_sound sound{};
        std::string filename;
        bool isBackground = false;
    };
    std::vector<std::unique_ptr<LoadSound>> m_loadedSounds;

    ma_engine m_engine{};
    bool m_initialized = false;

    ma_sound m_background{};
    bool m_backgroundLoaded = false;
    bool m_titleScreen = false;

    bool m_musicEnabled = true;
    bool m_effectsEnabled = true;

    int m_musicVolume = 70;
    int m_effectsVolume = 100;
    std::string m_path = "";


    void UpdateBackground()
    {
        if (!m_backgroundLoaded)
            return;

        bool shouldPlay = m_titleScreen || m_musicEnabled;

        ma_sound_set_volume( &m_background, m_musicVolume / 100.0f);

        if (shouldPlay)
            ma_sound_start(&m_background);
        else
            ma_sound_stop(&m_background);
    }

public:
    bool Initialize(std::string path)
    {
		m_path = path;

        if (m_initialized)
            return true;

        if (ma_engine_init(nullptr, &m_engine) != MA_SUCCESS)
            return false;

        m_initialized = true;
        return true;
    }

    bool Load(const char* filePath, bool isBackground = false)
    {
        if (!m_initialized || !filePath)
            return false;

        auto it = std::find_if(m_loadedSounds.begin(), m_loadedSounds.end(),
            [filePath](const auto& loaded)
            {
                return loaded->filename == filePath;
            });

        if (it != m_loadedSounds.end())
            return true;

        auto loaded = std::make_unique<LoadSound>();

        if (ma_sound_init_from_file(&m_engine, filePath, 0,
            nullptr, nullptr, &loaded->sound) != MA_SUCCESS)
        {
            return false;
        }

        loaded->filename = filePath;
        loaded->isBackground = isBackground;

        m_loadedSounds.push_back(std::move(loaded));

        return true;
    }

/*    bool PlayFile(const char* filePath)
    {
        if (!m_initialized || !filePath)
            return false;

        auto active = std::make_unique<ActiveSound>();

        if (ma_sound_init_from_file(&m_engine, filePath, 0,
            nullptr, nullptr, &active->sound) != MA_SUCCESS)
        {
            return false;
        }
 
        active->filename = filePath;

        if (ma_sound_start(&active->sound) != MA_SUCCESS)
        {
            ma_sound_uninit(&active->sound);
            return false;
        }

        m_activeSounds.push_back(std::move(active));

        return true;
   }*/

    bool Background(const char* filePath)
    {
        if (!m_initialized || !filePath)
            return false;

        if (m_backgroundLoaded)
        {
            ma_sound_uninit(&m_background);
            m_backgroundLoaded = false;
        }

        if (ma_sound_init_from_file( &m_engine, filePath,
            MA_SOUND_FLAG_LOOPING, nullptr, nullptr,
            &m_background) != MA_SUCCESS)
        {
            return false;
        }

        m_backgroundLoaded = true;

        UpdateBackground();

        return true;
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

        UpdateBackground();
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

    void SetTitleScreen(bool active)
    {
        m_titleScreen = active;
        UpdateBackground();
    }

    void Shutdown()
    {
        for (auto& loaded : m_loadedSounds)
        {
            ma_sound_uninit(&loaded->sound);
        }

        m_loadedSounds.clear();

        if (m_backgroundLoaded)
        {
            ma_sound_uninit(&m_background);
            m_backgroundLoaded = false;
		}
        if (m_initialized)
        {
            ma_engine_uninit(&m_engine);
            m_initialized = false;
        }
    }

};

