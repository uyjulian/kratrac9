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

#include <cstdio>
#include <cstring>
#include <climits>

#include "at9wave.h"
#include "kratrac9.h"

#include "tp_stub.h"

/* FORMAT_PCM */
#ifndef WAVE_FORMAT_PCM
# define WAVE_FORMAT_PCM 	(1)
#endif	/* WAVE_FORMAT_PCM */

/* FORMAT_IEEEFLOAT */
#ifndef WAVE_FORMAT_IEEEFLOAT
# define WAVE_FORMAT_IEEEFLOAT (3)
#endif	/* WAVE_FORMAT_IEEEFLOAT */

#ifndef WAVE_FORMAT_EXTENSIBLE
# define WAVE_FORMAT_EXTENSIBLE (0xfffe)
#endif	/* WAVE_FORMAT_EXTENSIBLE */


/* size of "fmt" chunk without "chunk type" and "chunk size" fields */
/* (may be sizeof(_pcmextHeader)) */
#define PCM_EXT_FMT_CHUNK_DATA_SIZE	(40)


#define AT9_HEADER_CB_SIZE	     (34)	/* be set to "cbSize" field */
#define SONY_ATRAC9_WAVEFORMAT_VERSION	(1)
#define SONY_ATRAC9_WAVEFORMAT_VERSION_BEX	(2)

/******************************************************************************
	for FORMAT_AT9 new RIFF-wave header(uses WAVE_FORMAT_EXTENSIBLE)
******************************************************************************/

#ifndef KSDATAFORMAT_SUBTYPE_PCM
static const GUID KSDATAFORMAT_SUBTYPE_PCM
= {0x00000001, 0x0000, 0x0010, {0x80, 0x00, 0x00, 0xaa, 0x00, 0x38, 0x9b, 0x71}};
/* 00000001-0000-0010-8000-00aa00389b71 */
#endif	/* KSDATAFORMAT_SUBTYPE_PCM */

#ifndef KSDATAFORMAT_SUBTYPE_IEEE_FLOAT
static const GUID KSDATAFORMAT_SUBTYPE_IEEE_FLOAT
= {0x00000003, 0x0000, 0x0010, {0x80, 0x00, 0x00, 0xaa, 0x00, 0x38, 0x9b, 0x71}};
/* 00000003-0000-0010-8000-00aa00389b71 */
#endif	/* KSDATAFORMAT_SUBTYPE_IEEE_FLOAT */

#ifndef KSDATAFORMAT_SUBTYPE_ATRAC9
static const GUID KSDATAFORMAT_SUBTYPE_ATRAC9 =
{ 0x47e142d2, 0x36ba, 0x4d8d, { 0x88, 0xfc, 0x61, 0x65, 0x4f, 0x8c, 0x83, 0x6c } };
/* {47E142D2-36BA-4d8d-88FC-61654F8C836C} */
#endif	/* KSDATAFORMAT_SUBTYPE_ATRAC9 */


/******************************************************************************
	common functinos
******************************************************************************/
static unsigned long _fgetLong(void *fp, const Atrac9FileCallbacks *_cb);
static unsigned short _fgetShort(void *fp, const Atrac9FileCallbacks *_cb);

/******************************************************************************
	parse wave file
******************************************************************************/
int parseWaveHeader(void *fp, FmtChunk *fmt, unsigned __int64 *total, int *encdelay, const Atrac9FileCallbacks *_cb)
{
    unsigned long	chunkLength;
    int 	format = SCE_ERROR_MAIN_UNKNOW_FORMAT;
    long long 	dataChunkStart = 0;
    unsigned short Samples;
    PcmHeader 	*pcmHeader;
    PcmExtHeader	*pcmextHeader;
	At9Header    *pat9header;
	int nSmplchunk = 0, loopcnt = 0;
	size_t readSize;

    do {
		if (_fgetLong(fp, _cb) != 0x46464952) { /* 'RIFF' */
			return SCE_ERROR_HEADER_FATAL_ERROR;
		}
		chunkLength = _fgetLong(fp, _cb) - 4; /* rLen, RIFF chunk length */
		if (chunkLength % 2 == 1) {
			chunkLength += 1; /* if chunkLength is odd, add padding data length */
		}
		if (_fgetLong(fp, _cb) == 0x45564157) { /* 'WAVE' */
			break;
		}
    } while (!_cb->seek(fp, chunkLength, SEEK_CUR));

    while (_cb->tell(fp) != EOF) {
		unsigned long chunkType = _fgetLong(fp, _cb);
		if (_cb->tell(fp) == EOF)
			break;
		chunkLength = _fgetLong(fp, _cb);
	
		if (chunkLength % 2 == 1)
			chunkLength += 1; /* if chunkLength is odd, add padding data length */
	
		switch (chunkType) {
		case 0x20746d66: /* 'fmt ' */
			format = _fgetShort(fp, _cb);	/* wFormatTag */
			switch (format) {
			case WAVE_FORMAT_PCM:	/* WAVE_FORMAT_PCM */
			case WAVE_FORMAT_IEEEFLOAT:	/* WAVE_FORMAT_IEEEFLOAT */
				pcmHeader = &fmt->pcmHeader;
				pcmHeader->wFormatTag 	   = format;
				pcmHeader->nChannels 	   = _fgetShort(fp, _cb);
				pcmHeader->nSamplesPerSec  = _fgetLong(fp, _cb);
				pcmHeader->nAvgBytesPerSec = _fgetLong(fp, _cb);
				pcmHeader->nBlockAlign	   = _fgetShort(fp, _cb);
				pcmHeader->wBitsPerSample  = _fgetShort(fp, _cb);

				/* rest is unknown data. just skip them */
				chunkLength -= PCM_FMT_CHUNK_DATA_SIZE;
				if (format == WAVE_FORMAT_IEEEFLOAT) {
					format = FORMAT_IEEEFLOAT;
				} else {
					format = FORMAT_PCM;
				}
				break;

			case WAVE_FORMAT_EXTENSIBLE: /* FORMAT_AT9 | EXTENSIBLE_PCM */
				pcmextHeader = &fmt->pcmextHeader;
				pcmextHeader->wFormatTag 	= format;
				pcmextHeader->nChannels 	= _fgetShort(fp, _cb);
				pcmextHeader->nSamplesPerSec 	= _fgetLong(fp, _cb);
				pcmextHeader->nAvgBytesPerSec 	= _fgetLong(fp, _cb);
				pcmextHeader->nBlockAlign 	= _fgetShort(fp, _cb);
				pcmextHeader->wBitsPerSample 	= _fgetShort(fp, _cb);
				pcmextHeader->cbSize 		= _fgetShort(fp, _cb);
				Samples 			= _fgetShort(fp, _cb);
				/* one of wValidBitsPerSample,wSamplesPerBlock or wReserved, */
				/* which will be found after "SubFormat" parsed */
				pcmextHeader->dwChannelMask 	= _fgetLong(fp, _cb);
				pcmextHeader->SubFormat.Data1 	= _fgetLong(fp, _cb);
				pcmextHeader->SubFormat.Data2 	= _fgetShort(fp, _cb);
				pcmextHeader->SubFormat.Data3 	= _fgetShort(fp, _cb);
				readSize = _cb->read(fp, pcmextHeader->SubFormat.Data4, 8);

				if (readSize != 8) {
					return SCE_ERROR_COMMON_FREAD_ERROR;
				}

				if (pcmextHeader->SubFormat == KSDATAFORMAT_SUBTYPE_ATRAC9) {
					pat9header = &fmt->at9Header;
					pat9header->Samples.wSamplesPerBlock 	= Samples;
					pat9header->dwVersionInfo 			= _fgetLong(fp, _cb);
					if ((pat9header->dwVersionInfo != SONY_ATRAC9_WAVEFORMAT_VERSION)
						&& (pat9header->dwVersionInfo != SONY_ATRAC9_WAVEFORMAT_VERSION_BEX)) {
						return SCE_ERROR_HEADER_VERSION;
					}
					readSize = _cb->read(fp, pat9header->configData, 4);
					if (readSize != 4) {
						return SCE_ERROR_COMMON_FREAD_ERROR;
					}
					readSize = _cb->read(fp, pat9header->Reserved, 4);

					if (readSize != 4) {
						return SCE_ERROR_COMMON_FREAD_ERROR;
					}
					/* rest is unknown data. just skip them */
					chunkLength -= AT9_FMT_CHUNK_DATA_SIZE;
					format = FORMAT_AT9;
				}
				else if (pcmextHeader->SubFormat == KSDATAFORMAT_SUBTYPE_PCM) {
					pcmextHeader->Samples.wValidBitsPerSample = Samples;
					chunkLength -= PCM_EXT_FMT_CHUNK_DATA_SIZE;
					format = FORMAT_PCM;
					/* rest is unknown data. just skip them */
				}
				else if (pcmextHeader->SubFormat == KSDATAFORMAT_SUBTYPE_IEEE_FLOAT) {
					pcmextHeader->Samples.wValidBitsPerSample = Samples;
					chunkLength -= PCM_EXT_FMT_CHUNK_DATA_SIZE;
					format = FORMAT_IEEEFLOAT;
					/* rest is unknown data. just skip them */
				}
				else {	/* unsupported SubFormat */
					return SCE_ERROR_HEADER_FATAL_ERROR;
				}
				break;
			default:	/* unsupported wFormatTag */
				return SCE_ERROR_HEADER_FATAL_ERROR;
			}
			break;
		case 0x74636166: /* 'fact' */
			*total = _fgetLong(fp, _cb);
			chunkLength -= 4;
			/* Extended fact Chunk*/
			if (chunkLength >= 8) {
				_fgetLong(fp, _cb);
				*encdelay = _fgetLong(fp, _cb);
				chunkLength -= 8;
			}
			break;

		case 0x61746164: /* 'data' */
			if ((format == FORMAT_PCM)
				|| format == FORMAT_IEEEFLOAT) {
				if (fmt->pcmHeader.wBitsPerSample != 0 && fmt->pcmHeader.nChannels != 0) {
					*total = chunkLength
						/ (fmt->pcmHeader.wBitsPerSample/8*fmt->pcmHeader.nChannels);
					goto seek_data;
				}
				return SCE_ERROR_HEADER_FATAL_ERROR;
			}
			if (dataChunkStart != 0)
				return SCE_ERROR_HEADER_FATAL_ERROR;
			dataChunkStart = _cb->tell(fp);
			if (dataChunkStart == -1){
				return SCE_ERROR_HEADER_FATAL_ERROR;
			}
			goto seek_data;

		case 0x6c706d73: /* 'smpl' */
			/* command line option is prior to the 'smpl' chunk */
			if (_cb->seek(fp, 28, SEEK_CUR)){ /* skip all the unused data */
				return SCE_ERROR_HEADER_FATAL_ERROR;
			}
			loopcnt = _fgetLong(fp, _cb);
			chunkLength -= 32;
			if (loopcnt != 0) { /* check Num Sample Loops */
				int loopstart, loopend, i;

				TVPAddLog(TJS_W("Atrac9: This plugin doesn't support built-in loop data."));
				TVPAddLog(TJS_W("Atrac9: The following loop is disregarded."));
				if (_cb->seek(fp, 4, SEEK_CUR)){/* skip all the unused data */
					return SCE_ERROR_HEADER_FATAL_ERROR; 
				}
				chunkLength -= 4;
				for (i = 0; i < loopcnt; i++) {
					if (_cb->seek(fp, 8, SEEK_CUR)){/* skip all the unused data */
						return SCE_ERROR_HEADER_FATAL_ERROR; 
					}
					loopstart = _fgetLong(fp, _cb);
					loopstart -= *encdelay;
					loopend = _fgetLong(fp, _cb);
					loopend -= *encdelay;
					if (_cb->seek(fp, 8, SEEK_CUR)){/* skip all the unused data */
						return SCE_ERROR_HEADER_FATAL_ERROR; 
					}
					chunkLength -= 24;

					tjs_char buf[256];
					swprintf_s(buf, TJS_W("Atrac9: [loopstart = %8d, loopend = %8d]"), loopstart, loopend);
					TVPAddLog(buf);
				}
			}
			nSmplchunk++;
			break;

		default:	/* unknown chunk */
			break;
		}
	
		/* skip remain of chunk */
		if (_cb->seek(fp, chunkLength, SEEK_CUR)){
			TVPAddLog(TJS_W("Atrac9: [ERROR] input file is illegal file or over 2G Byte."));
			return SCE_ERROR_HEADER_FATAL_ERROR;
		}
    }
   
seek_data:
    /* rewind to data chunk start point */
    if (_cb->seek(fp, dataChunkStart, SEEK_SET)){
		return SCE_ERROR_HEADER_FATAL_ERROR;
    }
	
    return format;
}

int
createPcmHeader(FILE *outfile,
	FmtChunk *pInWaveHdr,
	unsigned __int32 totalSamples,
	int nBytePerSample)
{
	PcmHeader *pcmHdr = &pInWaveHdr->pcmHeader;
	int nAvgBytesPerSec = pcmHdr->nSamplesPerSec * pcmHdr->nChannels * nBytePerSample;
	int nBlockAlign = pcmHdr->nChannels * nBytePerSample;
	unsigned __int32 dataSize = pcmHdr->nChannels * nBytePerSample * totalSamples;
	unsigned __int32 fileSize = PCM_HEADER_SIZE + dataSize;

	char chunk[4];
	long long_num;
	short short_num;

	/* make main chunk header */
	chunk[0] = 'R'; chunk[1] = 'I'; chunk[2] = 'F', chunk[3] = 'F';
	fwrite(chunk, sizeof(chunk), sizeof(char), outfile);
	long_num = fileSize - 8;
	fwrite(&long_num, 1, sizeof(long), outfile);

	/* make wave chunk */
	chunk[0] = 'W'; chunk[1] = 'A'; chunk[2] = 'V'; chunk[3] = 'E';
	fwrite(chunk, sizeof(chunk), sizeof(char), outfile);
	chunk[0] = 'f'; chunk[1] = 'm'; chunk[2] = 't'; chunk[3] = ' ';
	fwrite(chunk, sizeof(chunk), sizeof(char), outfile);

	long_num = PCM_FMT_CHUNK_DATA_SIZE;
	fwrite(&long_num, 1, sizeof(long), outfile);
	if ((nBytePerSample * CHAR_BIT) != ATRAC9_WORD_LENGTH_FLOAT) {
		short_num = WAVE_FORMAT_PCM;
	} else {
		short_num = WAVE_FORMAT_IEEEFLOAT;
	}
	fwrite(&short_num, 1, sizeof(short), outfile);
	short_num = pcmHdr->nChannels;
	fwrite(&short_num, 1, sizeof(short), outfile);
	long_num = pcmHdr->nSamplesPerSec;
	fwrite(&long_num, 1, sizeof(long), outfile);
	long_num = nAvgBytesPerSec;
	fwrite(&long_num, 1, sizeof(long), outfile);
	short_num = nBlockAlign;
	fwrite(&short_num, 1, sizeof(short), outfile);
	short_num = nBytePerSample * CHAR_BIT;
	fwrite(&short_num, 1, sizeof(short), outfile);

	/* make data header */
	chunk[0] = 'd'; chunk[1] = 'a'; chunk[2] = 't'; chunk[3] = 'a';
	fwrite(chunk, sizeof(chunk), sizeof(char), outfile);
	long_num = dataSize;
	fwrite(&long_num, 1, sizeof(long), outfile);

	return 0;
}

/* get 32-bit data from the FILE stream */
static unsigned long _fgetLong(void *fp, const Atrac9FileCallbacks *_cb)
{
    unsigned long ret;
	(*_cb->read)(fp, static_cast<void *>(&ret), 4);
    return ret;
}

/* get 16-bit data from the FILE stream */
static unsigned short _fgetShort(void *fp, const Atrac9FileCallbacks *_cb)
{
    unsigned short ret;
	(*_cb->read)(fp, static_cast<void *>(&ret), 2);
    return ret;
}
