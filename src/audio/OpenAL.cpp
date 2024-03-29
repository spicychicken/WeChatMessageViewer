#include "OpenAL.h"

#include "SilkSong.h"

using audio::OpenAL;
using audio::SilkSong;

static OpenAL openalInitializer;

OpenAL::OpenAL() {
    m_Device = alcOpenDevice(nullptr);
    if (m_Device) {
        m_Context = alcCreateContext(m_Device, nullptr);
        if (m_Context) {
            alcMakeContextCurrent(m_Context);
        }
    }
}

OpenAL::~OpenAL() {
    alcMakeContextCurrent(nullptr);

    if (m_Context) {
        alcDestroyContext(m_Context);
    }
    if (m_Device) {
        alcCloseDevice(m_Device);
    }
}

static SilkSong* silksong = nullptr;

void OpenAL::singlePlaySilkFromPath(const std::string& filePath)
{
    if (silksong != nullptr)
    {
        delete silksong;
    }
    silksong = SilkSong::fromPath(filePath);
    silksong->play();
}

void OpenAL::singlePlaySilkFromData(const std::string& data)
{
    if (silksong != nullptr)
    {
        delete silksong;
    }
    silksong = SilkSong::fromData(data);
    silksong->play();
}

void OpenAL::stop()
{
    if (silksong)
    {
        silksong->stop();
        delete silksong;
        silksong = nullptr;
    }
}