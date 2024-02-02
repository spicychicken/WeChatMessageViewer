#include "SilkDecode.h"

#include <sstream>

#include "silk/interface/SKP_Silk_SDK_API.h"
#include "silk/src/SKP_Silk_SigProc_FIX.h"

using std::string;
using std::istringstream;
using std::ostringstream;

/* Define codec specific settings should be moved to h file */
#define MAX_BYTES_PER_FRAME     1024
#define MAX_INPUT_FRAMES        5
#define MAX_FRAME_LENGTH        480
#define FRAME_LENGTH_MS         20
#define MAX_API_FS_KHZ          48
#define MAX_LBRR_DELAY          2

#ifdef _SYSTEM_IS_BIG_ENDIAN
/* Function to convert a little endian int16 to a */
/* big endian int16 or vica verca                 */
void swap_endian(
    SKP_int16       vec[],
    SKP_int         len
)
{
    SKP_int i;
    SKP_int16 tmp;
    SKP_uint8 *p1, *p2;

    for( i = 0; i < len; i++ ){
        tmp = vec[ i ];
        p1 = (SKP_uint8 *)&vec[ i ]; p2 = (SKP_uint8 *)&tmp;
        p1[ 0 ] = p2[ 1 ]; p1[ 1 ] = p2[ 0 ];
    }
}
#endif

#if (defined(_WIN32) || defined(_WINCE))
#include <windows.h>	/* timer */
#else    // Linux or Mac
#include <sys/time.h>
#endif

#ifdef _WIN32

unsigned long GetHighResolutionTime() /* O: time in usec*/
{
    /* Returns a time counter in microsec	*/
    /* the resolution is platform dependent */
    /* but is typically 1.62 us resolution  */
    LARGE_INTEGER lpPerformanceCount;
    LARGE_INTEGER lpFrequency;
    QueryPerformanceCounter(&lpPerformanceCount);
    QueryPerformanceFrequency(&lpFrequency);
    return (unsigned long)((1000000*(lpPerformanceCount.QuadPart)) / lpFrequency.QuadPart);
}
#else    // Linux or Mac
unsigned long GetHighResolutionTime() /* O: time in usec*/
{
    struct timeval tv;
    gettimeofday(&tv, 0);
    return((tv.tv_sec*1000000)+(tv.tv_usec));
}
#endif // _WIN32

/* Seed for the random number generator, which is used for simulating packet loss */
static SKP_int32 rand_seed = 1;

static string readstring(istringstream& iss, int len)
{
    char buffer[1024] = "\0";
    ostringstream oss;
    while (len > 0 && iss.good())
    {
        iss.read(buffer, len);
        oss.write(buffer, strlen(buffer));
        len -= 1024;
    }
    return oss.str();
}

int audio::convert::silkToPcm(const string& input, string& output)
{
    istringstream   iss(input);
    ostringstream   oss;

    /* Check Silk header */
    {
        char first = iss.get();
        string type = "!SILK_V3";
        if (first == '\x2')
        {
            type = "#!SILK_V3";
        }
        if (readstring(iss, type.length()) != type)
        {
            // wrong header
            return -1;
        }
    }

    // hard code here
    SKP_float loss_prob = 0.0f;
    SKP_int32 API_Fs_Hz = 0;

    SKP_SILK_SDK_DecControlStruct DecControl;
    /* Set the samplingrate that is requested for the output */
    if( API_Fs_Hz == 0 ) {
        DecControl.API_sampleRate = 24000;
    } else {
        DecControl.API_sampleRate = API_Fs_Hz;
    }

    /* Initialize to one frame per packet, for proper concealment before first packet arrives */
    DecControl.framesPerPacket = 1;

    SKP_int ret;
    SKP_int32 decSizeBytes;

    /* Create decoder */
    ret = SKP_Silk_SDK_Get_Decoder_Size(&decSizeBytes);

    void *psDec = malloc(decSizeBytes);

    /* Reset decoder */
    ret = SKP_Silk_SDK_InitDecoder(psDec);

    SKP_uint8 payload[MAX_BYTES_PER_FRAME * MAX_INPUT_FRAMES * (MAX_LBRR_DELAY + 1)];
    SKP_int16 nBytesPerPacket[MAX_LBRR_DELAY + 1], totBytes;
    SKP_int32 totPackets     = 0;
    SKP_int16 nBytes         = 0;
    unsigned long tottime    = 0;
    SKP_uint8* payloadEnd = payload;

    /* Simulate the jitter buffer holding MAX_FEC_DELAY packets */
    for (int i = 0; i < MAX_LBRR_DELAY; i++)
    {
        /* Read payload size */
        iss.read((char*)&nBytes, sizeof(nBytes));

#ifdef _SYSTEM_IS_BIG_ENDIAN
        swap_endian( &nBytes, 1 );
#endif
        /* Read payload */
        iss.read((char*)payloadEnd, nBytes * sizeof(SKP_uint8));

        if (iss.eof())
        {
            break;
        }

        nBytesPerPacket[ i ] = nBytes;
        payloadEnd          += nBytes;
        totPackets++;
    }

    SKP_int32 frames, lost;
    SKP_uint8 FECpayload[MAX_BYTES_PER_FRAME * MAX_INPUT_FRAMES], *payloadPtr;
    unsigned long starttime = 0;
    SKP_int16 nBytesFEC;
    SKP_uint8* payloadToDec = nullptr;
    SKP_int16 out[((FRAME_LENGTH_MS * MAX_API_FS_KHZ) << 1) * MAX_INPUT_FRAMES], *outPtr;
    SKP_int16 tot_len, len;
    SKP_int32 packetSize_ms = 0;

    while (iss.good())
    {
        /* Read payload size */
        iss.read((char*)&nBytes, sizeof(nBytes));

#ifdef _SYSTEM_IS_BIG_ENDIAN
        swap_endian( &nBytes, 1 );
#endif
        if (nBytes < 0 || iss.eof())
        {
            break;
        }

        /* Read payload */
        iss.read((char*)payloadEnd, nBytes * sizeof(SKP_uint8));
        if (iss.eof())
        {
            break;
        }

        /* Simulate losses */
        rand_seed = SKP_RAND(rand_seed);
        if ((((float)((rand_seed >> 16) + (1 << 15))) / 65535.0f >= (loss_prob / 100.0f)))
        {
            nBytesPerPacket[ MAX_LBRR_DELAY ] = nBytes;
            payloadEnd                       += nBytes;
        } else {
            nBytesPerPacket[ MAX_LBRR_DELAY ] = 0;
        }

        if (nBytesPerPacket[ 0 ] == 0)
        {
            /* Indicate lost packet */
            lost = 1;

            /* Packet loss. Search after FEC in next packets. Should be done in the jitter buffer */
            payloadPtr = payload;
            for (int i = 0; i < MAX_LBRR_DELAY; i++)
            {
                if (nBytesPerPacket[ i + 1 ] > 0)
                {
                    starttime = GetHighResolutionTime();
                    SKP_Silk_SDK_search_for_LBRR( payloadPtr, nBytesPerPacket[ i + 1 ], ( i + 1 ), FECpayload, &nBytesFEC );
                    tottime += GetHighResolutionTime() - starttime;
                    if( nBytesFEC > 0 ) {
                        payloadToDec = FECpayload;
                        nBytes = nBytesFEC;
                        lost = 0;
                        break;
                    }
                }
                payloadPtr += nBytesPerPacket[ i + 1 ];
            }
        } else {
            lost = 0;
            nBytes = nBytesPerPacket[ 0 ];
            payloadToDec = payload;
        }

        /* Silk decoder */
        outPtr = out;
        tot_len = 0;
        starttime = GetHighResolutionTime();

        if( lost == 0 ) {
            /* No Loss: Decode all frames in the packet */
            frames = 0;
            do {
                /* Decode 20 ms */
                ret = SKP_Silk_SDK_Decode( psDec, &DecControl, 0, payloadToDec, nBytes, outPtr, &len );
                if( ret ) {
                    // printf( "\nSKP_Silk_SDK_Decode returned %d", ret );
                }

                frames++;
                outPtr  += len;
                tot_len += len;
                if( frames > MAX_INPUT_FRAMES ) {
                    /* Hack for corrupt stream that could generate too many frames */
                    outPtr  = out;
                    tot_len = 0;
                    frames  = 0;
                }
                /* Until last 20 ms frame of packet has been decoded */
            } while( DecControl.moreInternalDecoderFrames );
        } else {
            /* Loss: Decode enough frames to cover one packet duration */
            for(int i = 0; i < DecControl.framesPerPacket; i++ ) {
                /* Generate 20 ms */
                ret = SKP_Silk_SDK_Decode( psDec, &DecControl, 1, payloadToDec, nBytes, outPtr, &len );
                if( ret ) {
                    // printf( "\nSKP_Silk_Decode returned %d", ret );
                }
                outPtr  += len;
                tot_len += len;
            }
        }

        packetSize_ms = tot_len / ( DecControl.API_sampleRate / 1000 );
        tottime += GetHighResolutionTime() - starttime;
        totPackets++;

        /* Write output to file */
#ifdef _SYSTEM_IS_BIG_ENDIAN
        swap_endian( out, tot_len );
#endif
        oss.write((const char*)out, tot_len * sizeof(SKP_int16));

        /* Update buffer */
        totBytes = 0;
        for (int i = 0; i < MAX_LBRR_DELAY; i++)
        {
            totBytes += nBytesPerPacket[ i + 1 ];
        }
        /* Check if the received totBytes is valid */
        if (totBytes < 0 || totBytes > sizeof(payload))
        {
            return -1;
        }
        SKP_memmove( payload, &payload[ nBytesPerPacket[ 0 ] ], totBytes * sizeof( SKP_uint8 ) );
        payloadEnd -= nBytesPerPacket[ 0 ];
        SKP_memmove( nBytesPerPacket, &nBytesPerPacket[ 1 ], MAX_LBRR_DELAY * sizeof( SKP_int16 ) );
    }

    /* Empty the recieve buffer */
    for(int k = 0; k < MAX_LBRR_DELAY; k++ ) {
        if( nBytesPerPacket[ 0 ] == 0 ) {
            /* Indicate lost packet */
            lost = 1;

            /* Packet loss. Search after FEC in next packets. Should be done in the jitter buffer */
            payloadPtr = payload;
            for(int i = 0; i < MAX_LBRR_DELAY; i++ ) {
                if( nBytesPerPacket[ i + 1 ] > 0 ) {
                    starttime = GetHighResolutionTime();
                    SKP_Silk_SDK_search_for_LBRR( payloadPtr, nBytesPerPacket[ i + 1 ], ( i + 1 ), FECpayload, &nBytesFEC );
                    tottime += GetHighResolutionTime() - starttime;
                    if( nBytesFEC > 0 ) {
                        payloadToDec = FECpayload;
                        nBytes = nBytesFEC;
                        lost = 0;
                        break;
                    }
                }
                payloadPtr += nBytesPerPacket[ i + 1 ];
            }
        } else {
            lost = 0;
            nBytes = nBytesPerPacket[ 0 ];
            payloadToDec = payload;
        }

        /* Silk decoder */
        outPtr  = out;
        tot_len = 0;
        starttime = GetHighResolutionTime();

        if( lost == 0 ) {
            /* No loss: Decode all frames in the packet */
            frames = 0;
            do {
                /* Decode 20 ms */
                ret = SKP_Silk_SDK_Decode( psDec, &DecControl, 0, payloadToDec, nBytes, outPtr, &len );
                if( ret ) {
                    // printf( "\nSKP_Silk_SDK_Decode returned %d", ret );
                }

                frames++;
                outPtr  += len;
                tot_len += len;
                if( frames > MAX_INPUT_FRAMES ) {
                    /* Hack for corrupt stream that could generate too many frames */
                    outPtr  = out;
                    tot_len = 0;
                    frames  = 0;
                }
            /* Until last 20 ms frame of packet has been decoded */
            } while( DecControl.moreInternalDecoderFrames );
        } else {
            /* Loss: Decode enough frames to cover one packet duration */

            /* Generate 20 ms */
            for(int i = 0; i < DecControl.framesPerPacket; i++ ) {
                ret = SKP_Silk_SDK_Decode( psDec, &DecControl, 1, payloadToDec, nBytes, outPtr, &len );
                if( ret ) {
                    // printf( "\nSKP_Silk_Decode returned %d", ret );
                }
                outPtr  += len;
                tot_len += len;
            }
        }

        packetSize_ms = tot_len / ( DecControl.API_sampleRate / 1000 );
        tottime += GetHighResolutionTime() - starttime;
        totPackets++;

        /* Write output to file */
#ifdef _SYSTEM_IS_BIG_ENDIAN
        swap_endian( out, tot_len );
#endif
        oss.write((const char*)out, tot_len * sizeof(SKP_int16));

        /* Update Buffer */
        totBytes = 0;
        for(int i = 0; i < MAX_LBRR_DELAY; i++ ) {
            totBytes += nBytesPerPacket[ i + 1 ];
        }

        /* Check if the received totBytes is valid */
        if (totBytes < 0 || totBytes > sizeof(payload))
        {
            return -1;
        }
        
        SKP_memmove( payload, &payload[ nBytesPerPacket[ 0 ] ], totBytes * sizeof( SKP_uint8 ) );
        payloadEnd -= nBytesPerPacket[ 0 ];
        SKP_memmove( nBytesPerPacket, &nBytesPerPacket[ 1 ], MAX_LBRR_DELAY * sizeof( SKP_int16 ) );
    }

    /* Free decoder */
    free( psDec );

    output = oss.str();

    return 0;
}
