#include "SilkSong.h"

#include <fstream>

#include "functions/Utils.h"
#include "SilkDecode.h"

using std::string;
using audio::SilkSong;

constexpr static uint32_t SAMPLING_RATE = 24000;

SilkSong::SilkSong(const std::string& path) {
    initialAL();
    loadFromSilk(path);
}

SilkSong::~SilkSong() {
    close();
}

void SilkSong::initialAL()
{
    alGenSources((ALuint)1, &source);
    //alSourcef(source, AL_PITCH, 1.0f);
    //alSourcef(source, AL_GAIN, 1.0f);
    //alSource3f(source, AL_POSITION, 0.0f, 0.0f, 0.0f);
    //alSource3f(source, AL_VELOCITY, 0.0f, 0.0f, 0.0f);
    alSourcei(source, AL_LOOPING, AL_FALSE);
    alDopplerVelocity(1.0f);
}

void SilkSong::loadFromSilk(const string& path)
{
    string silk = Utils::readBinaryFile(path);
    string pcm;
    if (audio::convert::silkToPcm(silk, pcm) == 0)
    {
        alGenBuffers((ALuint)1, &buffer);
        alBufferData(buffer, AL_FORMAT_MONO16, (const ALvoid*)pcm.c_str(), pcm.length(), SAMPLING_RATE);
        alSourcei(source, AL_BUFFER, buffer);
        loaded = true;
    }
}

void SilkSong::close() {
    alSourceStop(source);
    alDeleteSources(1, &source);
    if (loaded)
    {
        alDeleteBuffers(1, &buffer);
    }
}

void SilkSong::play() {
    alSourceRewind(source);
    alSourcePlay(source);
}

void SilkSong::stop() {
    alSourceStop(source);
}

bool SilkSong::isPlaying()
{
    ALenum state;
    alGetSourcei(source, AL_SOURCE_STATE, &state);
    return state == AL_PLAYING;
}