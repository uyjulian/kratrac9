#ifndef _SCE_ATRAC9_LIBATRAC9_H
#define _SCE_ATRAC9_LIBATRAC9_H

#ifdef	__cplusplus
extern "C" {
#endif	/* __cplusplus */


#ifdef _WIN32
#define DLLEXPORT 		__declspec(dllexport)
#else /* _WIN32 */
#define DLLEXPORT 		__attribute__((visibility("default")))
#endif /* _WIN32 */


#define SCE_AT9_SUCCESS (0x00000000)

#define SCE_AT9_ERROR_INVALID_HANDLE                  (0x8F630000)
#define SCE_AT9_ERROR_INVALID_SAMPLING_RATE           (0x8F630001)
#define SCE_AT9_ERROR_INVALID_CHANNEL                 (0x8F630002)
#define SCE_AT9_ERROR_INVALID_FRAMES_IN_SUPER_FRAME   (0x8F630003)
#define SCE_AT9_ERROR_INVALID_WORD_LENGTH             (0x8F630004)
#define SCE_AT9_ERROR_INVALID_BANDS                   (0x8F630005)
#define SCE_AT9_ERROR_INVALID_ISBANDS                 (0x8F630006)
#define SCE_AT9_ERROR_INVALID_GRADMODE                (0x8F630007)
#define SCE_AT9_ERROR_NOT_SUPPORTED_PARAM             (0x8F630008)
#define SCE_AT9_ERROR_INVALID_CONFIG_DATA             (0x8F630009)
#define SCE_AT9_ERROR_INVALID_INPUT_SAMPLES           (0x8F63000A)
#define SCE_AT9_ERROR_INVALID_OFFSET_SAMPLES          (0x8F63000B)
#define SCE_AT9_ERROR_INVALID_OUTPUT_SAMPLES          (0x8F63000C)
#define SCE_AT9_ERROR_NOT_INITIALIZED				  (0x8F63000D)
#define SCE_AT9_ERROR_INVALID_POINTER                 (0x8F63000F)
#define SCE_AT9_ERROR_INVALID_DUAL_MODE               (0x8F630010)
#define SCE_AT9_ERROR_INVALID_SLC                     (0x8F630011)
#define SCE_AT9_ERROR_INVALID_BEX                     (0x8F630012)
#define SCE_AT9_ERROR_ALREADY_SET_ENC_PARAM           (0x8F630020)
#define SCE_AT9_ERROR_ALREADY_INITIALIZED             (0x8F630022)
#define SCE_AT9_ERROR_SHORTAGE_OF_BYTES               (0x8F630400)
#define SCE_AT9_ERROR_INTERNAL_ERROR                  (0x8F630401)




#define SCE_AT9_CONFIG_DATA_SIZE        (4)

#define	SCE_AT9_PARAM_UNSET		        (-255)

#define SCE_AT9_CHANNEL_MONO            (1)
#define SCE_AT9_CHANNEL_STEREO          (2)
#define SCE_AT9_CHANNEL_CH4_0           (4)
#define SCE_AT9_CHANNEL_CH5_1           (6)
#define SCE_AT9_CHANNEL_CH7_1           (8)

/* The definition of SamplingRate */
#define SCE_AT9_SAMPLING_RATE_48KHZ     (48000)
#define SCE_AT9_SAMPLING_RATE_44_1KHZ   (44100)
#define SCE_AT9_SAMPLING_RATE_32KHZ     (32000)
#define SCE_AT9_SAMPLING_RATE_24KHZ     (24000)
#define SCE_AT9_SAMPLING_RATE_22_05KHZ  (22050)
#define SCE_AT9_SAMPLING_RATE_16KHZ     (16000)
#define SCE_AT9_SAMPLING_RATE_12KHZ     (12000)
#define SCE_AT9_SAMPLING_RATE_11_025KHZ (11025)

#define SCE_AT9_1FRAME_IN_SUPER_FRAME    (1)
#define SCE_AT9_4FRAMES_IN_SUPER_FRAME   (4)

#define SCE_AT9_WORD_LENGTH_8BIT        (8)
#define SCE_AT9_WORD_LENGTH_16BIT       (16)
#define SCE_AT9_WORD_LENGTH_24BIT       (24)
#define SCE_AT9_WORD_LENGTH_FLOAT       (32)

#define SCE_AT9_MAX_CHANNELS            (8)

#define SCE_AT9_MAX_SUPER_FRAME_SIZE    (2048)

#define SCE_AT9_MAX_FRAME_SAMPLES       (256)
#define SCE_AT9_MAX_SUPER_FRAME_SAMPLES  (1024)

#define SCE_AT9_ENCODER_DELAY_SAMPLES   (0)
#define SCE_AT9_DECODER_DELAY_SAMPLES   (0)

#define SCE_AT9_DECODER_INPUT_PADDING   (2)

typedef void* HANDLE_ATRAC9;

typedef struct {
	int channels;
	int dualMode;
	int samplingRate;
	int superframeSize;
	int framesInSuperframe;
	int wlength;
} SceAt9EncInitParam;

typedef struct {
	int nbands;
	int isband;
	int gradMode;
} SceAt9EncExtParam;



typedef struct {
	int channels;
	int channelConfigIndex;
	int samplingRate;
	int superframeSize;
	int framesInSuperframe;
	int frameSamples;
	int wlength;
	unsigned char configData[SCE_AT9_CONFIG_DATA_SIZE];
} SceAt9CodecInfo;

typedef struct {
	int errorCode;
	int detailError;
	int coreError;
} SceAt9InternalErrorInfo;


DLLEXPORT HANDLE_ATRAC9 sceAt9GetHandle(void);

DLLEXPORT void sceAt9ReleaseHandle(HANDLE_ATRAC9 handle);

DLLEXPORT int sceAt9EncSetEncParam(HANDLE_ATRAC9 handle,
									int wbandFlag,
									int bexFlag,
									int reserved1,
									int slcFlag);

DLLEXPORT int sceAt9EncInit(HANDLE_ATRAC9 handle,
							const SceAt9EncInitParam *pInitParam,
							const SceAt9EncExtParam  *pExtParam);

DLLEXPORT int sceAt9DecInit(HANDLE_ATRAC9 handle,
							unsigned char *pConfigData,
							int wlength);

DLLEXPORT int sceAt9EncEncode(HANDLE_ATRAC9 handle,
								const void *pPcmBuffer,
								int pcmSamples,
								unsigned char *pOutputBuffer,
								int *pOutputBytes);

DLLEXPORT int sceAt9EncFlush(HANDLE_ATRAC9 handle,
								unsigned char *pOutputBuffer,
								int *pOutputBytes,
								int *pTerminateFlag);

DLLEXPORT int sceAt9DecDecode(HANDLE_ATRAC9 handle,
								const unsigned char *pStreamBuffer,
								int *pNByteUsed,
								void *pPcmBuffer,
								const int offset,
								const int nsamples);

DLLEXPORT int sceAt9GetCodecInfo(HANDLE_ATRAC9 handle,
									SceAt9CodecInfo *pCodecInfo);

DLLEXPORT int sceAt9GetInternalErrorInfo(HANDLE_ATRAC9 handle,
											SceAt9InternalErrorInfo *pInfo);

DLLEXPORT int sceAt9GetDefaultExtParam(const SceAt9EncInitParam *pInitParam,
										SceAt9EncExtParam  *pExtParam);

DLLEXPORT int sceAt9GetVersion(void);



#ifdef	__cplusplus
}
#endif	/* __cplusplus */







#endif /* _SCE_ATRAC9_LIBATRAC9_H */
