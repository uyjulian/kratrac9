/********************************************
	Author: Hintay <L.M. Works>
*********************************************/
/*  ATRAC9 plugin for TSS ( stands for TVP Sound System )
 *  FOR INTERNAL USE ONLY.
 */


#ifndef _KRATRAC9_H
#define _KRATRAC9_H

#ifdef USE_OPEN_SOURCE_LIBRARY
#include "../libatrac9/src/libatrac9.h"
#else
#include "include/libatrac9.h"
#endif

// For output rendered pcm data to wav file.
#define TEST_OUT_WAV 0
#define TEST_OUT_WAV_PATH "at9_pcm_debug.wav"

#define SCE_ERROR_MAIN_CANNOT_OPEN_INFILE        (0x80000000)
#define SCE_ERROR_MAIN_CANNOT_OPEN_OUTFILE       (0x80000001)
#define SCE_ERROR_MAIN_OPTION_ILLEGAL_BITRATE    (0x80000002)
#define SCE_ERROR_MAIN_OPTION_NUMBER_OF_FILES    (0x80000003)
#define SCE_ERROR_MAIN_OPTION_ILLEGAL_MODE       (0x80000004)
#define SCE_ERROR_MAIN_OPTION_ILLEGAL_NBANDS     (0x80000005)
#define SCE_ERROR_MAIN_OPTION_ILLEGAL_ISBAND     (0x80000006)
#define SCE_ERROR_MAIN_OPTION_ILLEGAL_GRADMODE   (0x80000007)
#define SCE_ERROR_MAIN_UNKNOW_FORMAT             (0x80000008)
#define SCE_ERROR_MAIN_OPTION_ILLEGAL_BEX        (0x80000009)

/* Encode Error Code */
#define SCE_ERROR_ENCODE_ILLEGAL_INPUT_FORMAT    (0x81000000)
#define SCE_ERROR_ENCODE_ILLEGAL_BITS_PER_SAMPLE (0x81000001)
#define SCE_ERROR_ENCODE_TOO_SHORT_FILE_SAMPLES  (0x81000002)
#define SCE_ERROR_ENCODE_NOT_SUPPORT_PARAMETER   (0x81000003)
#define SCE_ERROR_ENCODE_INTERNAL_ERROR          (0x81000100)
#define SCE_ERROR_ENCODE_CANNOT_GET_HANDLE       (0x81000101)

/* Decode Error Code */
#define SCE_ERROR_DECODE_ILLEGAL_INPUT_FORMAT    (0x82000000)
#define SCE_ERROR_DECODE_ILLEGAL_PARAM           (0x82000001)
#define SCE_ERROR_DECODE_NOT_SUPPORT_PARAMETER   (0x82000002)
#define SCE_ERROR_DECODE_INTERNAL_ERROR          (0x82000101)
#define SCE_ERROR_DECODE_CANNOT_GET_HANDLE       (0x81000102)

/* Common Error Code */
#define SCE_ERROR_COMMON_FWRITE_ERROR            (0x83000000)
#define SCE_ERROR_COMMON_FSEEK_ERROR             (0x83000001)
#define SCE_ERROR_COMMON_FTELL_ERROR             (0x83000002)
#define SCE_ERROR_COMMON_FREAD_ERROR			 (0x83000003)

/*J Header Error Code */
#define SCE_ERROR_HEADER_VERSION                 (0x84000000)
#define SCE_ERROR_HEADER_FATAL_ERROR             (0x84000FFF)


#ifdef USE_OPEN_SOURCE_LIBRARY
#define KrAt9GetHandle Atrac9GetHandle
#define KrAt9GetCodecInfo Atrac9GetCodecInfo
#define KrAt9ReleaseHandle Atrac9ReleaseHandle
#define KRATRAC9_CONFIG_DATA_SIZE ATRAC9_CONFIG_DATA_SIZE
#define KrAtrac9CodecInfo Atrac9CodecInfo
#define KrAt9DecInit(handle, pConfigData, wlength) Atrac9InitDecoder(handle, pConfigData)
#define KrAt9Decode(handle, pStreamBuffer, pNByteUsed, pPcmBuffer, offset, nsamples) Atrac9Decode(handle, pStreamBuffer, pPcmBuffer, pNByteUsed)
#else
#define KrAt9GetHandle sceAt9GetHandle
#define KrAt9GetCodecInfo sceAt9GetCodecInfo
#define KrAt9ReleaseHandle sceAt9ReleaseHandle
#define KRATRAC9_CONFIG_DATA_SIZE SCE_AT9_CONFIG_DATA_SIZE
#define KrAtrac9CodecInfo SceAt9CodecInfo
#define KrAt9DecInit sceAt9DecInit
#define KrAt9Decode sceAt9DecDecode
#endif

#define ATRAC9_CHANNEL_MONO            (1)
#define ATRAC9_CHANNEL_STEREO          (2)
#define ATRAC9_CHANNEL_CH4_0           (4)
#define ATRAC9_CHANNEL_CH5_1           (6)
#define ATRAC9_CHANNEL_CH7_1           (8)

#define ATRAC9_WORD_LENGTH_8BIT        (8)
#define ATRAC9_WORD_LENGTH_16BIT       (16)
#define ATRAC9_WORD_LENGTH_24BIT       (24)
#define ATRAC9_WORD_LENGTH_FLOAT       (32)

// Use 16 bit depth
#define KRATRAC9_PCM_WORD_LENGTH ATRAC9_WORD_LENGTH_16BIT
typedef short samples;


/******************************************************************************
	ATRAC9 file part
******************************************************************************/

typedef struct Atrac9FileCallbacks Atrac9FileCallbacks;

typedef int(*at9_read_func)(void *_stream, void *_ptr, int _nbytes);

typedef int(*at9_seek_func)(void *_stream, long long _offset, int _whence);

typedef long long(*at9_tell_func)(void *_stream);

typedef int(*at9_close_func)(void *_stream);

struct Atrac9FileCallbacks {
	/**Used to read data from the stream.
	This must not be <code>NULL</code>.*/
	at9_read_func  read;
	/**Used to seek in the stream.
	This may be <code>NULL</code> if seeking is not implemented.*/
	at9_seek_func  seek;
	/**Used to return the current read position in the stream.
	This may be <code>NULL</code> if seeking is not implemented.*/
	at9_tell_func  tell;
	/**Used to close the stream when the decoder is freed.
	This may be <code>NULL</code> to leave the stream open.*/
	at9_close_func close;
};

#endif /* _KRATRAC9_H */