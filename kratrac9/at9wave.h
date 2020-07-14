/********************************************
	(C) Copyright 2009, 2010, 2011, 2012, 2013, 2014 Sony Corporation
	All Rights Reserved.
*********************************************/
/* SIE CONFIDENTIAL
 ATRAC9(TM) DLL version 4.0.0.0
 *
 *      Copyright (C) 2014 Sony Interactive Entertainment Inc.
 *                        All Rights Reserved.
 *
 */

#ifndef _AT9WAVE_H_
#define _AT9WAVE_H_

#include <stdio.h>
#if 0
#include <Windows.h>
#endif
#include "kratrac9.h"

typedef struct _GUID {
  unsigned long  Data1;
  unsigned short Data2;
  unsigned short Data3;
  unsigned char  Data4[8];
} GUID;

/* Definition Of Format */
#define FORMAT_PCM  (1)
#define FORMAT_IEEEFLOAT (3)
#define FORMAT_AT9	(9)

#define PCM_FMT_CHUNK_DATA_SIZE  (16)
#define AT9_FMT_CHUNK_DATA_SIZE  (52)
#define AT9_FACT_CHUNK_DATA_SIZE (12)

#define PCM_FMT_CHUNK_SIZE       (8+PCM_FMT_CHUNK_DATA_SIZE)
#define AT9_FMT_CHUNK_SIZE       (8+AT9_FMT_CHUNK_DATA_SIZE)
#define AT9_FACT_CHUNK_SIZE      (8+AT9_FACT_CHUNK_DATA_SIZE)

#define PCM_HEADER_SIZE          (12+PCM_FMT_CHUNK_SIZE+8)
#define AT9_HEADER_SIZE          (12+AT9_FMT_CHUNK_SIZE + AT9_FACT_CHUNK_SIZE+8)

/* Flag bit definition of a WAVE format */
/* http://msdn.microsoft.com/ja-jp/library/cc371561.aspx */
#define AT9_CHMAP_FLGBIT_FL  (0x1)
#define AT9_CHMAP_FLGBIT_FR  (0x2)
#define AT9_CHMAP_FLGBIT_FC  (0x4)
#define AT9_CHMAP_FLGBIT_LFE (0x8)
#define AT9_CHMAP_FLGBIT_BL  (0x10)
#define AT9_CHMAP_FLGBIT_BR  (0x20)
#define AT9_CHMAP_FLGBIT_BC  (0x100)
#define AT9_CHMAP_FLGBIT_SL  (0x200)
#define AT9_CHMAP_FLGBIT_SR  (0x400)

/* Channel Mapping definition of a XAudio2 */
/* http://msdn.microsoft.com/ja-jp/library/bb694506(v=vs.85).aspx */
#define AT9_CHMAP_MONO   (AT9_CHMAP_FLGBIT_FC)
#define AT9_CHMAP_STEREO (AT9_CHMAP_FLGBIT_FL | AT9_CHMAP_FLGBIT_FR)
#define AT9_CHMAP_3_0CH  (AT9_CHMAP_FLGBIT_FL | AT9_CHMAP_FLGBIT_FR | AT9_CHMAP_FLGBIT_LFE)
#define AT9_CHMAP_4_0CH  (AT9_CHMAP_FLGBIT_FL | AT9_CHMAP_FLGBIT_FR | AT9_CHMAP_FLGBIT_BL | AT9_CHMAP_FLGBIT_BR)
#define AT9_CHMAP_5_0CH  (AT9_CHMAP_FLGBIT_FL | AT9_CHMAP_FLGBIT_FR | AT9_CHMAP_FLGBIT_FC | AT9_CHMAP_FLGBIT_SL | AT9_CHMAP_FLGBIT_SR)
#define AT9_CHMAP_5_1CH  (AT9_CHMAP_FLGBIT_FL | AT9_CHMAP_FLGBIT_FR | AT9_CHMAP_FLGBIT_FC | AT9_CHMAP_FLGBIT_LFE | AT9_CHMAP_FLGBIT_SL | AT9_CHMAP_FLGBIT_SR)
#define AT9_CHMAP_7_0CH  (AT9_CHMAP_FLGBIT_FL | AT9_CHMAP_FLGBIT_FR | AT9_CHMAP_FLGBIT_FC | AT9_CHMAP_FLGBIT_LFE | AT9_CHMAP_FLGBIT_SL | AT9_CHMAP_FLGBIT_SR | AT9_CHMAP_FLGBIT_BC)
#define AT9_CHMAP_7_1CH  (AT9_CHMAP_FLGBIT_FL | AT9_CHMAP_FLGBIT_FR | AT9_CHMAP_FLGBIT_FC | AT9_CHMAP_FLGBIT_LFE | AT9_CHMAP_FLGBIT_SL | AT9_CHMAP_FLGBIT_SR | AT9_CHMAP_FLGBIT_BL | AT9_CHMAP_FLGBIT_BR)

/* Version check parameter */
#define AT9_MULTICHVERSION_PARAM_LF (1)
#define AT9_MULTICHVERSION_PARAM_FF (2)
#define AT9_MULTICHVERSION_PARAM_CF (0)

/******************************************************************************
	FORMAT_PCM file header part
******************************************************************************/
typedef struct {
 /* general waveform format structure */
    unsigned short	wFormatTag;	/* always 1 = FORMAT_PCM-Code */
    unsigned short	nChannels;	/* 1 = Mono, 2 = Stereo */
    unsigned long	nSamplesPerSec;	/* Sampling Freq */
    unsigned long	nAvgBytesPerSec;/* Data per sec */
    unsigned short	nBlockAlign;	/* block alignment,
					   1=8 bit, 2=16bit (mono)
					   2=8 bit, 4=16 bit (stereo) */
 /* specific waveform format structure for FORMAT_PCM data */
    unsigned short	wBitsPerSample;	/* bits per sample, 8, 12, 16 */
} PcmHeader;

/*typedef struct 
{
    unsigned long 	Data1;
    unsigned short 	Data2;
    unsigned short 	Data3;
    unsigned char 	Data4[8];
} GUID;*/

typedef struct PcmExtHeader {
    unsigned short	wFormatTag;			/* Compression Format ID: WAVE_FORMAT_EXTENSIBLE	*/
    unsigned short 	nChannels;			/* Number of Channels					*/
    unsigned long	nSamplesPerSec;			/* Sampling Frequency [Hz]     				*/ 
    unsigned long	nAvgBytesPerSec;		/* Average Number of Bytes per Sec.			*/
    unsigned short	nBlockAlign;			/* Number of Block Alignment Bytes       		*/
    unsigned short	wBitsPerSample;			/* Quantization Bit Number for Audio Sample		*/
    unsigned short	cbSize;				/* Size of the Extended Header that follows		*/
    union {
	unsigned short	wValidBitsPerSample;	/* Bits of Precision for Each Audio Sample		*/
	unsigned short	wSamplesPerBlock;      	/* The Number of Audio Samples in One Compressed Block	*/
	unsigned short	wReserved;		/* Reserved						*/
    }	Samples;
    unsigned long		dwChannelMask;			/* The Mapping of Channels to spatial location		*/
    GUID 	SubFormat;
} PcmExtHeader;


/******************************************************************************
	ATRAC file header part
******************************************************************************/
/* ATRAC-X in WAVEFORMATEXTENSIBLE */


typedef struct {
 /* general waveform format extensible structure(WAVEFORMATEXTENSIBLE) */
    unsigned short	wFormatTag;	/* WAVE_FORMAT_EXTENSIBLE */
    unsigned short 	nChannels;	/* Number of Channels */
    unsigned long	nSamplesPerSec;	/* Sampling Frequency [Hz] */
    unsigned long	nAvgBytesPerSec;/* Average Bytes [bytes/sec]*/
    unsigned short	nBlockAlign;	/* SuperframeLength */
    unsigned short	wBitsPerSample;	/* fixed number (0 for compression format) */
    unsigned short	cbSize;		/* fixed number (34 for AT-X) */
    union {
	unsigned short	wValidBitsPerSample;	/* (unused)*/
	unsigned short	wSamplesPerBlock;      	/* SuperframeSample */
	unsigned short	wReserved;		/* (unused)*/
    }	Samples;
    unsigned long	dwChannelMask;	/* The Mapping of Channels to Spatial Location		*/
    GUID            SubFormat;
    /* extra information area */
    unsigned long	dwVersionInfo;	/* Version Information of the ATRAC-X WAVE Header	*/
	unsigned char	configData[KRATRAC9_CONFIG_DATA_SIZE];	/* FORMAT_AT9 config_data 	*/
    unsigned char	Reserved[4];	/* FORMAT_AT9 reserved (0)	*/
} At9Header;

/* fmt chunk definition(without 'chunk type' and 'chunk size' fields) */
typedef union {
    PcmHeader    pcmHeader;
    PcmExtHeader pcmextHeader;
	At9Header    at9Header;
} FmtChunk;

typedef struct ATRAC9File {
	Atrac9FileCallbacks callbacks;

	tjs_uint64 totallen;
	tjs_uint64 current_sample;

	int encdelay;
	long long dataChunkStart;
	FmtChunk inWaveHeader;
	HANDLE_ATRAC9 decHandle;
	KrAtrac9CodecInfo codecInfo;
	unsigned char * data_buffer;

#if TEST_OUT_WAV
	FILE *outfile;
#endif

	samples * sample_buffer;
	size_t samples_filled; /* number of samples in the buffer */
	size_t samples_used; /* number of samples extracted from the buffer */

	unsigned int samples_to_discard;
} ATRAC9File;

/******************************************************************************
	Function Prototype Declare
******************************************************************************/
int parseWaveHeader(void *fp, FmtChunk *fmt, tjs_uint64 *total, int *encdelay, const Atrac9FileCallbacks *_cb);

int createPcmHeader(FILE     *outfile,
	FmtChunk			*pInWaveHdr,
	tjs_uint32	totalSamples,
	int					nBytePerSample);

#endif /* _AT9WAVE_H_ */

