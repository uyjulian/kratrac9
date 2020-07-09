//---------------------------------------------------------------------------
// ATRAC9 plugin for TSS ( stands for TVP Sound System )
// FOR INTERNAL USE ONLY.
//
// Author: Hintay <L.M. Works>
// This plugin just for test.
// 
// Please use open source configuration in production if you don't want to get into trouble with the law.
// libatrac9 in open source version: https://github.com/Thealexbarney/LibAtrac9
//---------------------------------------------------------------------------

#define _cdecl
#define __int64 int64_t

#define _CRT_SECURE_NO_WARNINGS

#include "kratrac9.h"
#include "at9wave.h"

#if 0
#include <Windows.h>
#endif
#include <cstring>
#include <sstream>
#include <cstdlib>
#if 0
#include "tp_stub.h"
#include "tvpsnd.h" // TSS sound system interface definitions
#endif
#include "ncbind/ncbind.hpp"
#include "WaveIntf.h"
#include "istream_compat.h"

//---------------------------------------------------------------------------
#if 0
int WINAPI DllEntryPoint(HINSTANCE hinst, unsigned long reason, void* lpReserved)
{
	return 1;
}
#endif
//---------------------------------------------------------------------------
#if 0
void strcpy_limit(LPWSTR dest, LPWSTR src, int n)
{
	// string copy with limitation
	// this will add a null terminater at destination buffer
	wcsncpy(dest, src, n-1);
	dest[n-1] = '\0';
}
#endif
//---------------------------------------------------------------------------
#if 0
ITSSStorageProvider *StorageProvider = nullptr;
#endif
//---------------------------------------------------------------------------
#if 0
class Atrac9Module : public ITSSModule // module interface
{
	ULONG RefCount; // reference count

public:
	Atrac9Module();
	virtual ~Atrac9Module();

	// IUnknown
	HRESULT __stdcall QueryInterface(REFIID iid, void ** ppvObject) override;
	ULONG __stdcall AddRef() override;
	ULONG __stdcall Release() override;
	
	// ITSSModule
	HRESULT __stdcall GetModuleCopyright(LPWSTR buffer, unsigned long buflen ) override;
	HRESULT __stdcall GetModuleDescription(LPWSTR buffer, unsigned long buflen ) override;
	HRESULT __stdcall GetSupportExts(unsigned long index, LPWSTR mediashortname, LPWSTR buf, unsigned long buflen ) override;
	HRESULT __stdcall GetMediaInfo(LPWSTR url, ITSSMediaBaseInfo ** info ) override;
	HRESULT __stdcall GetMediaSupport(LPWSTR url ) override;
	HRESULT __stdcall GetMediaInstance(LPWSTR url, IUnknown ** instance ) override;
};
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
class Atrac9WaveDecoder : public ITSSWaveDecoder // decoder interface
{
	ULONG RefCount; // refernce count
	bool InputFileInit; // whether InputFile is inited
	ATRAC9File* InputFile; // OggOpusFile instance
	IStream *InputStream; // input stream
	TSSWaveFormat Format{}; // output PCM format


public:
	Atrac9WaveDecoder();
	virtual ~Atrac9WaveDecoder();

	// IUnkown
	HRESULT __stdcall QueryInterface(REFIID iid, void ** ppvObject) override;
	ULONG __stdcall AddRef() override;
	ULONG __stdcall Release() override;

	// ITSSWaveDecoder
	HRESULT __stdcall GetFormat(TSSWaveFormat *format) override;
	HRESULT __stdcall Render(void *buf, unsigned long bufsamplelen,
            unsigned long *rendered, unsigned long *status) override;
	HRESULT __stdcall SetPosition(unsigned __int64 samplepos) override;

	// others
	HRESULT SetStream(IStream *stream, LPWSTR url);

private:
	int static _cdecl read_func(void *stream, void *ptr, int nbytes);
	int static _cdecl seek_func(void *stream, long long offset, int whence);
	int static _cdecl close_func(void *stream);
	long long static _cdecl tell_func(void *stream);
};
#endif
class Atrac9WaveDecoder : public tTVPWaveDecoder
{
	bool InputFileInit; // whether InputFile is inited
	ATRAC9File* InputFile; // OggOpusFile instance
	IStream *InputStream; // input stream
	tTVPWaveFormat Format; // output PCM format

public:
    Atrac9WaveDecoder();
    ~Atrac9WaveDecoder();

public:
    // ITSSWaveDecoder
    virtual void GetFormat(tTVPWaveFormat & format);
    virtual bool Render(void *buf, tjs_uint bufsamplelen, tjs_uint& rendered);
    virtual bool SetPosition(tjs_uint64 samplepos);

    bool SetStream(IStream *stream, const ttstr & url);

    bool Open(const ttstr & url);
    bool ReadBlock(int , int );

private:
	int static _cdecl read_func(void *stream, void *ptr, int nbytes);
	int static _cdecl seek_func(void *stream, long long offset, int whence);
	int static _cdecl close_func(void *stream);
	long long static _cdecl tell_func(void *stream);
};

class Atrac9WaveDecoderCreator : public tTVPWaveDecoderCreator
{
public:
    tTVPWaveDecoder * Create(const ttstr & storagename, const ttstr & extension) {
    	IStream *stream;
		stream = TVPCreateIStream(storagename, TJS_BS_READ);
		if(!stream)
		{
			return nullptr;
		}
		Atrac9WaveDecoder * decoder = new Atrac9WaveDecoder();
		if(!decoder->SetStream(stream, storagename))
		{
			// error; stream may not be a vorbis stream
			delete decoder;
			stream->Release();
			return nullptr;
		}

        return decoder;
    }
};
#if 0
//---------------------------------------------------------------------------
// Atrac9Module implementation ##############################################
//---------------------------------------------------------------------------
Atrac9Module::Atrac9Module()
{
	// Atrac9Module constructor
	RefCount = 1;
}
//---------------------------------------------------------------------------
Atrac9Module::~Atrac9Module() = default;
//---------------------------------------------------------------------------
HRESULT __stdcall Atrac9Module::QueryInterface(REFIID iid, void ** ppvObject)
{
	// IUnknown::QueryInterface

	if(!ppvObject) return E_INVALIDARG;

	*ppvObject= nullptr;
	if(!memcmp(&iid,&IID_IUnknown,16))
		*ppvObject=static_cast<IUnknown*>(this);
	else if(!memcmp(&iid,&IID_ITSSModule,16))
		*ppvObject=static_cast<ITSSModule*>(this);

	if(*ppvObject)
	{
		AddRef();
		return S_OK;
	}
	return E_NOINTERFACE;
}
//---------------------------------------------------------------------------
ULONG __stdcall Atrac9Module::AddRef()
{
	return ++RefCount;
}
//---------------------------------------------------------------------------
ULONG __stdcall Atrac9Module::Release()
{
	if(RefCount == 1)
	{
		delete this;
		return 0;
	}
	return --RefCount;
}
//---------------------------------------------------------------------------
HRESULT __stdcall Atrac9Module::GetModuleCopyright(LPWSTR buffer, unsigned long buflen)
{
	// return module copyright information
	strcpy_limit(buffer, L"ATRAC9(TM) Plug-in for TVP Sound System (C) 2018 L.M. Works", buflen);
	return S_OK;
}
//---------------------------------------------------------------------------
HRESULT __stdcall Atrac9Module::GetModuleDescription(LPWSTR buffer, unsigned long buflen )
{
	// return module description
	strcpy_limit(buffer, L"ATRAC9(TM) (*.at9) decoder", buflen);
	return S_OK;
}
//---------------------------------------------------------------------------
HRESULT __stdcall Atrac9Module::GetSupportExts(unsigned long index, LPWSTR mediashortname,
												LPWSTR buf, unsigned long buflen )
{
	// return supported file extensios
	if(index >= 1) return S_FALSE;
	wcscpy(mediashortname, L"ATRAC9(TM) Stream Format");
	strcpy_limit(buf, L".at9", buflen);
	return S_OK;
}
//---------------------------------------------------------------------------
HRESULT __stdcall Atrac9Module::GetMediaInfo(LPWSTR url, ITSSMediaBaseInfo ** info )
{
	// return media information interface
	return E_NOTIMPL; // not implemented
}
//---------------------------------------------------------------------------
HRESULT __stdcall Atrac9Module::GetMediaSupport(LPWSTR url )
{
	// return media support interface
	return E_NOTIMPL; // not implemented
}
//---------------------------------------------------------------------------
HRESULT __stdcall Atrac9Module::GetMediaInstance(LPWSTR url, IUnknown ** instance )
{
	// retrieve input stream interface
	IStream *stream;
	HRESULT hr = StorageProvider->GetStreamForRead(url, reinterpret_cast<IUnknown**>(&stream));
	if(FAILED(hr)) return hr;

	// create Atrac9 decoder
	Atrac9WaveDecoder * decoder = new Atrac9WaveDecoder();
	hr = decoder->SetStream(stream, url);
	if(FAILED(hr))
	{
		// error; stream may not be a Atrac9 stream
		delete decoder;
		stream->Release();
		return hr;
	}

	*instance = static_cast<IUnknown*>(decoder); // return as IUnknown
	stream->Release(); // release stream because the decoder already holds it

	return S_OK;
}
#endif
//---------------------------------------------------------------------------
// Atrac9WaveDecoder implementation #########################################
//---------------------------------------------------------------------------
Atrac9WaveDecoder::Atrac9WaveDecoder()
{
	// Atrac9WaveDecoder constructor
#if 0
	RefCount = 1;
#endif
	InputFileInit = false;
	InputFile = nullptr;
	InputStream = nullptr;
}
//---------------------------------------------------------------------------
Atrac9WaveDecoder::~Atrac9WaveDecoder()
{
	// Atrac9WaveDecoder destructor
	if(InputFileInit)
	{
#if TEST_OUT_WAV
		if (InputFile->outfile)
			fclose(InputFile->outfile);
#endif
		if (InputFile->decHandle)
			KrAt9ReleaseHandle(InputFile->decHandle);
		if (InputFile->data_buffer)
			free(InputFile->data_buffer);
		InputFileInit = false;
		InputFile = nullptr;
	}
	if(InputStream)
	{
		InputStream->Release();
		InputStream = nullptr;
	}
}
//---------------------------------------------------------------------------
#if 0
HRESULT Atrac9WaveDecoder::QueryInterface(REFIID iid, void ** ppvObject)
{
	// IUnknown::QueryInterface

	if(!ppvObject) return E_INVALIDARG;

	*ppvObject= nullptr;
	if(!memcmp(&iid,&IID_IUnknown,16))
		*ppvObject=static_cast<IUnknown*>(this);
	else if(!memcmp(&iid,&IID_ITSSWaveDecoder,16))
		*ppvObject=static_cast<ITSSWaveDecoder*>(this);

	if(*ppvObject)
	{
		AddRef();
		return S_OK;
	}
	return E_NOINTERFACE;
}
#endif
//---------------------------------------------------------------------------
#if 0
ULONG __stdcall Atrac9WaveDecoder::AddRef()
{
	return ++RefCount;
}
#endif
//---------------------------------------------------------------------------
#if 0
ULONG __stdcall Atrac9WaveDecoder::Release()
{
	if(RefCount == 1)
	{
		delete this;
		return 0;
	}

	return --RefCount;
}
#endif
//---------------------------------------------------------------------------
#if 0
HRESULT __stdcall Atrac9WaveDecoder::GetFormat(TSSWaveFormat *format)
{
	// return PCM format
	if(!InputFileInit)
	{
		return E_FAIL;
	}

	*format = Format;

	return S_OK;
}
#endif
void Atrac9WaveDecoder::GetFormat(tTVPWaveFormat & format)
{
	if (InputFileInit)
	{
		format = Format;
	}
}
//---------------------------------------------------------------------------
#if 0
HRESULT __stdcall Atrac9WaveDecoder::Render(void *buf, unsigned long bufsamplelen,
	unsigned long *rendered, unsigned long *status)
#endif
bool Atrac9WaveDecoder::Render(void *buf, tjs_uint bufsamplelen, tjs_uint& rendered)
{
	// render output PCM
#if 0
	if (!InputFileInit) return E_FAIL; // InputFile is yet not inited
#endif
	if (!InputFileInit) return false;

	const auto superframeSamples = InputFile->inWaveHeader.at9Header.Samples.wSamplesPerBlock;

	samples *pPcmBuffer = static_cast<samples *>(buf);
	unsigned int pos = 0; // decoded PCM (in sample)
	tjs_uint64 totalsample = bufsamplelen;
	if (InputFile->totallen - InputFile->current_sample < totalsample)
		totalsample = InputFile->totallen - InputFile->current_sample;

	int handle_status;
	
	while(totalsample)
	{
		if (InputFile->samples_filled)
		{	// sample_buf 中有数据
			unsigned int samples_to_get = InputFile->samples_filled;	// 要复制的 sample 长度

			if(InputFile->samples_to_discard)
			{	// 有要跳过的 sample
				if (InputFile->samples_to_discard > samples_to_get)
				{	// 若要跳过的 sample 大于要复制的 sample 长度则丢弃本块
					InputFile->samples_to_discard -= samples_to_get;
					InputFile->samples_used = 0;
					InputFile->samples_filled = 0;
					samples_to_get = 0;
				}
				else
				{
					samples_to_get -= InputFile->samples_to_discard;
					InputFile->samples_used = InputFile->samples_to_discard;
					InputFile->samples_to_discard = 0;
					InputFile->samples_filled -= InputFile->samples_used;
				}
			}
			
			if (totalsample < samples_to_get)
				samples_to_get = static_cast<unsigned int>(totalsample);

			if(samples_to_get)
				memcpy(pPcmBuffer,
					InputFile->sample_buffer + InputFile->samples_used * InputFile->codecInfo.channels,
					samples_to_get * InputFile->codecInfo.channels * sizeof(samples));

#if TEST_OUT_WAV
			fwrite(InputFile->sample_buffer + InputFile->samples_used * InputFile->codecInfo.channels, InputFile->codecInfo.channels * sizeof(samples), samples_to_get, InputFile->outfile);
#endif

			InputFile->samples_used += samples_to_get;
			InputFile->samples_filled -= samples_to_get;

			pPcmBuffer += samples_to_get * InputFile->codecInfo.channels;
			totalsample -= samples_to_get;
			pos += samples_to_get;
		}
		else
		{
			/* 读取一个超帧 (superframe) */
			ULONG bytes;
			InputStream->Read(InputFile->data_buffer, static_cast<ULONG>(InputFile->codecInfo.superframeSize), &bytes);
			if (bytes != static_cast<ULONG>(InputFile->codecInfo.superframeSize)) {
				goto fail;
			}
			unsigned char *pStream = InputFile->data_buffer;

			if (InputFile->samples_to_discard || totalsample < superframeSamples)
			{	// 有需要跳过的 sample 或剩余 sample 数小于超帧中的帧数

				// 解码至 sample_buffer
				samples * p_sample_buffer = InputFile->sample_buffer;

				for (int iframe = 0; iframe < InputFile->codecInfo.framesInSuperframe; iframe++) {
					int nbytes_used;

					handle_status = KrAt9Decode(InputFile->decHandle,
						pStream,
						&nbytes_used,
						p_sample_buffer,
						0,
						InputFile->codecInfo.frameSamples);

					if (handle_status < 0) {
						tjs_char string_buf[256];
						TJS_sprintf(string_buf, TJS_W("Atrac9: [ERROR] Atrac9DecDecode() = 0x%x."), handle_status);
						TVPAddLog(string_buf);
#ifndef USE_OPEN_SOURCE_LIBRARY
						if (handle_status == SCE_AT9_ERROR_INTERNAL_ERROR) {
							SceAt9InternalErrorInfo internalErrorInfo;

							sceAt9GetInternalErrorInfo(InputFile->decHandle, &internalErrorInfo);
							TJS_sprintf(string_buf, TJS_W("Atrac9: [ERROR] internalErrorInfo [%d, %d, %d]"), internalErrorInfo.errorCode, internalErrorInfo.detailError, internalErrorInfo.detailError);
							TVPAddLog(string_buf);
						}
#endif
						goto fail;
					}

					pStream += nbytes_used;
					p_sample_buffer += InputFile->codecInfo.frameSamples * InputFile->codecInfo.channels;
				}
				InputFile->samples_filled = superframeSamples;
				InputFile->samples_used = 0;
			}
			else
			{	// 直接解码至 buf
				/* 解码超帧中的所有帧 */
				for (int iframe = 0; iframe < InputFile->codecInfo.framesInSuperframe; iframe++) {
					int nbytes_used;

					const int outSample = InputFile->codecInfo.frameSamples;

					handle_status = KrAt9Decode(InputFile->decHandle,
						pStream,
						&nbytes_used,
						pPcmBuffer,
						0,
						outSample);

					if (handle_status < 0) {
						tjs_char string_buf[256];
						TJS_sprintf(string_buf, TJS_W("Atrac9: [ERROR] Atrac9DecDecode() = 0x%x."), handle_status);
						TVPAddLog(string_buf);
#ifndef USE_OPEN_SOURCE_LIBRARY
						if (handle_status == SCE_AT9_ERROR_INTERNAL_ERROR) {
							SceAt9InternalErrorInfo internalErrorInfo;

							sceAt9GetInternalErrorInfo(InputFile->decHandle, &internalErrorInfo);
							TJS_sprintf(string_buf, TJS_W("Atrac9: [ERROR] internalErrorInfo [%d, %d, %d]"), internalErrorInfo.errorCode, internalErrorInfo.detailError, internalErrorInfo.detailError);
							TVPAddLog(string_buf);
						}
#endif
						goto fail;
					}

#if TEST_OUT_WAV
					fwrite(pPcmBuffer, InputFile->codecInfo.channels * sizeof(samples), outSample, InputFile->outfile);
#endif
					pStream += nbytes_used;
					pPcmBuffer += outSample * InputFile->codecInfo.channels;
					totalsample -= outSample;

					pos += outSample;
				}
			}
		}
	}

	InputFile->current_sample += pos;

	rendered = pos;

	return !(pos < bufsamplelen);
#if 0
	if (status)
	{
		*status = pos < bufsamplelen ? 0 : 1;
		// *status will be 0 if the decoding is ended
	}

	if (rendered)
	{
		*rendered = pos; // return renderd PCM samples
	}
#endif

#if 0
	return S_OK;
#endif

fail:
#if 0
	return E_FAIL;
#endif
	return false;
}
//---------------------------------------------------------------------------
#if 0
HRESULT __stdcall Atrac9WaveDecoder::SetPosition(unsigned __int64 samplepos)
#endif
bool Atrac9WaveDecoder::SetPosition(tjs_uint64 samplepos)
{
	// set PCM position (seek)
	if(!InputFileInit) return false;

	InputFile->current_sample = samplepos;

	// 需要加上编码延迟
	samplepos += InputFile->encdelay;

	const unsigned int superframeSamples = InputFile->inWaveHeader.at9Header.Samples.wSamplesPerBlock; // 每个 superframe 中的 sample
	const auto superframeLength = static_cast<unsigned int>(samplepos / superframeSamples);

	// 设定需要跳过的 sample 数
	InputFile->samples_to_discard = samplepos % superframeSamples;

	// 清零以重新解码
	InputFile->samples_used = 0;
	InputFile->samples_filled = 0;

	// seek 至最近的 superframe
	seek_func(this, InputFile->dataChunkStart + superframeLength * InputFile->codecInfo.superframeSize, SEEK_SET);

	return true;
}
//---------------------------------------------------------------------------
#if 0
HRESULT Atrac9WaveDecoder::SetStream(IStream *stream, LPWSTR url)
#endif
bool Atrac9WaveDecoder::SetStream(IStream *stream, const ttstr & url)
{
	int status;

	// set input stream
	InputStream = stream;
	InputStream->AddRef(); // add-ref

	InputFile = static_cast<ATRAC9File *>(malloc(sizeof(ATRAC9File)));
	memset(InputFile, 0, sizeof(ATRAC9File));
#if 0
	ZeroMemory(InputFile, sizeof(ATRAC9File));
#endif

	InputFile->encdelay = -1;
	InputFile->callbacks = { read_func, seek_func, tell_func, close_func };
	// callback functions

	const int format = parseWaveHeader(this, &InputFile->inWaveHeader, &InputFile->totallen, &InputFile->encdelay, &InputFile->callbacks);
	if (format != FORMAT_AT9 || (InputFile->dataChunkStart = tell_func(this)) == EOF)
	{
		goto fail;
	}

	HANDLE_ATRAC9 decHandle;
	decHandle = KrAt9GetHandle();
	if (decHandle == nullptr)
	{
		TVPAddLog(TJS_W("Atrac9: [ERROR] Atrac9GetHandle() = NULL"));
		goto fail;
	}
	InputFile->decHandle = decHandle;

	status = KrAt9DecInit(decHandle,
		InputFile->inWaveHeader.at9Header.configData,
		KRATRAC9_PCM_WORD_LENGTH);

	if (status < 0) {
		tjs_char buf[256];
		TJS_sprintf(buf, TJS_W("Atarc9: [ERROR] Atarc9DecInit() = 0x%x"), status);
		TVPAddLog(buf);
#ifndef USE_OPEN_SOURCE_LIBRARY
		if (status == SCE_AT9_ERROR_INTERNAL_ERROR) {
			SceAt9InternalErrorInfo internalErrorInfo;

			sceAt9GetInternalErrorInfo(decHandle, &internalErrorInfo);
			TJS_sprintf(buf, TJS_W("Atarc9: [ERROR] internalErrorInfo [%d, %d, %d]"), internalErrorInfo.errorCode, internalErrorInfo.detailError, internalErrorInfo.detailError);
			TVPAddLog(buf);
		}
#endif
		goto fail;
	}

	InputFileInit = true;

	// set Format up
	status = KrAt9GetCodecInfo(decHandle,
		&InputFile->codecInfo);
	if (status < 0) {
		tjs_char buf[256];
		TJS_sprintf(buf, TJS_W("Atrac9: [ERROR] Atrac9GetCodecInfo() = 0x%x."), status);
		TVPAddLog(buf);
		goto fail;
	}

	InputFile->data_buffer = static_cast<unsigned char *>(calloc(sizeof(unsigned char), InputFile->codecInfo.superframeSize));
	InputFile->sample_buffer = static_cast<samples *>(calloc(sizeof(samples), InputFile->codecInfo.channels * InputFile->codecInfo.frameSamples * InputFile->codecInfo.framesInSuperframe));

	unsigned int *reserved;
	reserved = reinterpret_cast<unsigned int *>(InputFile->inWaveHeader.at9Header.Reserved);
	switch (InputFile->inWaveHeader.at9Header.nChannels) {
		case ATRAC9_CHANNEL_CH4_0:
		case ATRAC9_CHANNEL_CH5_1:
		case ATRAC9_CHANNEL_CH7_1:
			switch (*reserved) {
			case AT9_MULTICHVERSION_PARAM_LF:
				TVPAddLog(TJS_W("Atrac9: [ERROR] This input file cannot be decoded."));
				TVPAddLog(TJS_W("Atrac9: [ERROR] Input file is ATRAC9 file which is generated by the alpha version tool."));
				goto fail;
			case AT9_MULTICHVERSION_PARAM_FF:
				TVPAddLog(TJS_W("Atrac9: [!Warning!] Input file has a special header (Reserved of fmt chunk) temporarily"));
				break;
			case AT9_MULTICHVERSION_PARAM_CF:
			default:
				break;
		}
		break;
	default:
		break;
	}

#if TEST_OUT_WAV
	fopen_s(&InputFile->outfile, TEST_OUT_WAV_PATH, "wb");
	createPcmHeader(InputFile->outfile, &InputFile->inWaveHeader, static_cast<unsigned __int32>(InputFile->totallen), sizeof(samples));
#endif

#if 0
	ZeroMemory(&Format, sizeof(Format));
	Format.dwSamplesPerSec = InputFile->codecInfo.samplingRate;
	Format.dwChannels = InputFile->codecInfo.channels;
	Format.dwBitsPerSample = InputFile->codecInfo.wlength;
	Format.dwSeekable = 2;
	Format.ui64TotalSamples = InputFile->totallen;
#endif
	memset(&Format, 0, sizeof(Format));
	Format.SamplesPerSec = InputFile->codecInfo.samplingRate;
	Format.Channels = InputFile->codecInfo.channels;
	Format.BitsPerSample = InputFile->codecInfo.wlength;
	Format.BytesPerSample = InputFile->codecInfo.wlength / 8;
	Format.IsFloat = false;
	Format.Seekable = 2;
	Format.TotalSamples = InputFile->totallen;

	double timetotal;
	timetotal = static_cast<double>(Format.TotalSamples) / Format.SamplesPerSec;
	if (timetotal<0) Format.TotalTime = 0; else Format.TotalTime = static_cast<unsigned long>(timetotal * 1000.0);

	// Reset position
	SetPosition(0);

	return true;

fail:
	// error!
	InputStream->Release();
	InputStream = nullptr;

	if (InputFile->data_buffer)
		free(InputFile->data_buffer);
	if (InputFile->sample_buffer)
		free(InputFile->sample_buffer);
	free(InputFile);
	InputFile = nullptr;
	return false;
}
//---------------------------------------------------------------------------
int _cdecl Atrac9WaveDecoder::read_func(void *stream, void *ptr, int nbytes)
{
	// read function (wrapper for IStream)

	auto * decoder = static_cast<Atrac9WaveDecoder*>(stream);
	if(!decoder->InputStream) return 0;

	ULONG bytesread;
	if(FAILED(decoder->InputStream->Read(ptr, static_cast<ULONG>(nbytes), &bytesread)))
	{
		return -1; // failed
	}

	return bytesread;
}
//---------------------------------------------------------------------------
int _cdecl Atrac9WaveDecoder::seek_func(void *stream, long long offset, int whence)
{
	// seek function (wrapper for IStream)

	auto * decoder = static_cast<Atrac9WaveDecoder*>(stream);
	if(!decoder->InputStream) return -1;

	LARGE_INTEGER newpos;
	ULARGE_INTEGER result;
	newpos.QuadPart = offset;
	int seek_type = STREAM_SEEK_SET;
	
	switch(whence)
	{
	case SEEK_SET:
		seek_type = STREAM_SEEK_SET;
		break;
	case SEEK_CUR:
		seek_type = STREAM_SEEK_CUR;
		break;
	case SEEK_END:
		seek_type = STREAM_SEEK_END;
		break;
	}

	if(FAILED(decoder->InputStream->Seek(newpos, seek_type, &result)))
	{
		return -1;
	}

	return 0;
}
//---------------------------------------------------------------------------
int _cdecl Atrac9WaveDecoder::close_func(void *stream)
{
	// close function (wrapper for IStream)

	auto * decoder = static_cast<Atrac9WaveDecoder*>(stream);
	if(!decoder->InputStream) return -1;
	
	decoder->InputStream->Release();
	decoder->InputStream = nullptr;

	return 0;
}
//---------------------------------------------------------------------------
long long _cdecl Atrac9WaveDecoder::tell_func(void *stream)
{
	// tell function (wrapper for IStream)

	auto * decoder = static_cast<Atrac9WaveDecoder*>(stream);
	if(!decoder->InputStream) return -1;

	LARGE_INTEGER newpos;
	ULARGE_INTEGER old_result;
	ULARGE_INTEGER result;
	newpos.QuadPart = 0;

	decoder->InputStream->Seek({0}, STREAM_SEEK_CUR, &old_result);
	STATSTG statstg;
	decoder->InputStream->Stat(&statstg, 0);
	if((old_result.QuadPart == statstg.cbSize.QuadPart))
	{
		return EOF;
	}

	if(FAILED(decoder->InputStream->Seek(newpos, STREAM_SEEK_CUR, &result)))
	{
		return EOF;
	}

	return result.QuadPart;
}
//---------------------------------------------------------------------------
// ##########################################################################
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
static Atrac9WaveDecoderCreator kratrac9_creator;
static void kratrac9_init()
{
	TVPRegisterWaveDecoderCreator(&kratrac9_creator);
}

NCB_PRE_REGIST_CALLBACK(kratrac9_init);
#if 0
extern "C" __declspec(dllexport) HRESULT _stdcall GetModuleInstance(ITSSModule **out, ITSSStorageProvider *provider,
	IStream * config, HWND mainwin)
{
	// GetModuleInstance function (exported)
	StorageProvider = provider;
	*out = new Atrac9Module();
	return S_OK;
}
//---------------------------------------------------------------------------
extern "C" __declspec(dllexport) HRESULT _stdcall V2Link(iTVPFunctionExporter *exporter)
{
	TVPInitImportStub(exporter);
	
	TVPAddLog(TJS_W("Atrac9: Loaded."));
	return S_OK;
}
//---------------------------------------------------------------------------
extern "C" __declspec(dllexport) HRESULT _stdcall V2Unlink()
{
	TVPUninitImportStub();
	return S_OK;
}
#endif
//---------------------------------------------------------------------------
