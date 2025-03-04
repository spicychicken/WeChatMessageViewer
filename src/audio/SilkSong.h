#ifndef _AUDIO_SILK_SONG_H_
#define _AUDIO_SILK_SONG_H_

#include <string>

#include <al.h>
#include <alc.h>

namespace audio
{

class SilkSong
{
public:
    static SilkSong* fromPath(const std::string& path);
    static SilkSong* fromData(const std::string& data);

    ~SilkSong();

    void play();
    void stop();

    bool isPlaying();

protected:
    SilkSong(const std::string& data);

    void loadFromData(const std::string& data);
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