#ifndef _AUDIO_SILK_SONG_H_
#define _AUDIO_SILK_SONG_H_

#include <string>

#include <al.h>
#include <alc.h>

namespace audio
{

class SilkSong {
public:
    SilkSong(const std::string& path);
    ~SilkSong();

    void play();
    void stop();

    bool isPlaying();

protected:
    void loadFromSilk(const std::string& path);
    void close();

private:
    void initialAL();

private:
    ALuint source = AL_NONE;
    ALuint buffer = AL_NONE;
    bool loaded = false;
};

}
#endif