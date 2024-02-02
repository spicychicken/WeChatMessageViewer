#ifndef _OPEN_AL_H_
#define _OPEN_AL_H_

#include <string>

#include <al.h>
#include <alc.h>

namespace audio
{

class OpenAL {
public:
    OpenAL();
    ~OpenAL();

    static void singlePlaySilk(const std::string& filePath);
    static void stop();

private:
    ALCdevice* m_Device = nullptr;
    ALCcontext* m_Context = nullptr;
};

}

#endif