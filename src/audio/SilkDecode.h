#ifndef _AUDIO_SILK_DECODE_H_
#define _AUDIO_SILK_DECODE_H_

#include <string>

namespace audio
{
namespace convert
{
    int silkToPcm(const std::string& input, std::string& output);
}
}


#endif