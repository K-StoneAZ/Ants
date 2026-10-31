#pragma once
#include "miniaudio.h"

class Audio
{
private:
    ma_engine m_engine{};
    bool m_initialized = false;

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

    void Shutdown()
    {
        if (m_initialized)
        {
            ma_engine_uninit(&m_engine);
            m_initialized = false;
        }
    }

};

