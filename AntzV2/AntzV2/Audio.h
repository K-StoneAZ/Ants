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
        bool isLoaded = false;
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
        return Load();
    }

    bool Load()
    {
        if (!m_initialized || m_path.empty())
            return false;

        bool allLoaded = true;

        for (const auto& info : soundInventory)
        {
            auto it = std::find_if(m_loadedSounds.begin(), m_loadedSounds.end(),
                [&info](const auto& loaded)
                {
                    return loaded->filename == info.fname;
                });

            if (it != m_loadedSounds.end())
                continue;

            auto loaded = std::make_unique<LoadSound>();
            
            loaded->filename = info.fname;
            loaded->isBackground = info.type;
            loaded->isLoaded = false;

            std::string filePath = m_path + info.fname;

            if (ma_sound_init_from_file(&m_engine, filePath.c_str(), 0,
                nullptr, nullptr, &loaded->sound) == MA_SUCCESS)
            {
                loaded->isLoaded = true;
            }
            else
            {
                allLoaded = false;
            }

            m_loadedSounds.push_back(std::move(loaded));
        }

        return allLoaded;
    }

    bool PlayFile(const char* fileName)
    {
        if (!m_initialized || !fileName)
            return false;

        auto it = std::find_if(m_loadedSounds.begin(), m_loadedSounds.end(),
            [fileName](const auto& loaded)
            {
                return loaded->filename == fileName;
            });

        if (it == m_loadedSounds.end() || !(*it)->isLoaded)
            return false;

        if (!m_effectsEnabled)
            return true;

        ma_sound_set_volume(&(*it)->sound, m_effectsVolume / 100.0f);

        return ma_sound_start(&(*it)->sound) == MA_SUCCESS;
    }

    bool Background(const char* fileName)
    {
        if (!m_initialized || !fileName)
            return false;

        auto it = std::find_if(m_loadedSounds.begin(), m_loadedSounds.end(),
            [fileName](const auto& loaded)
            {
                return loaded->filename == fileName;
            });

        if (it == m_loadedSounds.end() || !(*it)->isLoaded)
            return false;

        ma_sound_set_looping(&(*it)->sound, MA_TRUE);

        ma_sound_set_volume(&(*it)->sound, m_musicVolume / 100.0f);

        if (!m_titleScreen && !m_musicEnabled)
            return true;

        return ma_sound_start(&(*it)->sound) == MA_SUCCESS;
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

