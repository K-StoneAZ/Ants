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
        bool type = false;
		bool loop = false;
    };

    const SoundInfo soundInventory[6] =
    {
        { "Error.wav",      false, false },
        { "Failed.wav",     false, false },
        { "Growth.wav",     false, false },
        { "Marching.wav",   false, false },
        { "Success.wav",    false, false },
        { "BattleRound.wav",false, true  },
        { "TitleMusic.wav", true, true  }
    };

    struct LoadSound
    {
        ma_sound sound{};
        std::string filename;
        bool type = false;
        bool loop = false;
        bool isLoaded = false;
    };
    std::vector<std::unique_ptr<LoadSound>> m_loadedSounds;

    ma_engine m_engine{};
    bool m_initialized = false;

    bool m_titleScreen = false;

    bool m_musicEnabled = true;
    bool m_effectsEnabled = true;

    int m_musicVolume = 70;
    int m_effectsVolume = 100;
    std::string m_path = "";

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
            loaded->type = info.type;
            loaded->loop = info.loop;
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

        LoadSound& loaded = *(*it);

        // Determine which application settings apply.
        bool enabled = loaded.type ? m_musicEnabled : m_effectsEnabled;

        // Title music always plays on the title screen.
        if (loaded.filename == "TitleMusic.wav" && m_titleScreen)
            enabled = true;

        if (!enabled)
            return true;

        // Apply the appropriate volume.
        int volume = loaded.type ? m_musicVolume : m_effectsVolume;
        if (loaded.filename == "TitleMusic.wav" && m_titleScreen)
            volume = 100;

        ma_sound_set_volume(
            &loaded.sound, volume / 100.0f);

        // Apply the inventory's looping setting.
        ma_sound_set_looping(
            &loaded.sound, loaded.loop ? MA_TRUE : MA_FALSE);

        // Don't restart a looping sound that's already playing.
        if (loaded.loop && ma_sound_is_playing(&loaded.sound))
            return true;

        return ma_sound_start(&loaded.sound) == MA_SUCCESS;
    }

    bool StopFile(const char* fileName)
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

        return ma_sound_stop(&(*it)->sound) == MA_SUCCESS;
    }

    void SetPreferences(bool musicEnabled, bool effectsEnabled,
        int musicVolume, int effectsVolume)
    {
        // Save current preferences for comparison.
        bool oldMusicEnabled = m_musicEnabled;
        bool oldEffectsEnabled = m_effectsEnabled;

        // Update preferences.
        m_musicEnabled = musicEnabled;
        m_effectsEnabled = effectsEnabled;

        m_musicVolume = (musicVolume < 0) ? 0 :
            (musicVolume > 100) ? 100 : musicVolume;

        m_effectsVolume = (effectsVolume < 0) ? 0 :
            (effectsVolume > 100) ? 100 : effectsVolume;

        // Stop currently playing sounds if their category is disabled.
        for (auto& loaded : m_loadedSounds)
        {
            if (!loaded->isLoaded || !ma_sound_is_playing(&loaded->sound))
                continue;

            bool enabled = loaded->type ? m_musicEnabled : m_effectsEnabled;

            // Title music is exempt from the music preference on the title screen.
            if (loaded->filename == "TitleMusic.wav" && m_titleScreen)
                enabled = true;

            if (!enabled)
            {
                ma_sound_stop(&loaded->sound);
                continue;
            }

            // Apply the updated volume to sounds that remain enabled.
            int volume = loaded->type ? m_musicVolume : m_effectsVolume;

            if (loaded->filename == "TitleMusic.wav" && m_titleScreen)
                volume = 100;

            ma_sound_set_volume(&loaded->sound, volume / 100.0f);
        }

        // Restart title/background music when music is re-enabled.
        if (!oldMusicEnabled && m_musicEnabled && !m_titleScreen)
            PlayFile("TitleMusic.wav");
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
    }

    void Shutdown()
    {
        for (auto& loaded : m_loadedSounds)
        {
            ma_sound_uninit(&loaded->sound);
        }

        m_loadedSounds.clear();

        if (m_initialized)
        {
            ma_engine_uninit(&m_engine);
            m_initialized = false;
        }
    }

};

