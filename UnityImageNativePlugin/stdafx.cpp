// stdafx.cpp : source file that includes just the standard includes
// *.pch will be the pre-compiled header
// stdafx.obj will contain the pre-compiled type information

// DebugLog reference: http://hojjatjafary.blogspot.ca/2013/01/c-plugin-debug-log.html

#include "stdafx.h"
#include "atlstr.h"
#include "wincodec.h"
#include "winnt.h"
#include <iostream>

// TODO: reference any additional headers you need in STDAFX.H
// and not in this file

#pragma comment( lib, "windowscodecs.lib" )


/* Setup log mechanism */
typedef void(*DebugLogPtr)(const char *);
static DebugLogPtr _gDebugLogPtr;
void Debug(const char* text)
{
	if (NULL == _gDebugLogPtr)
	{
		return;
	}
	else if (NULL == text)
	{
		_gDebugLogPtr("");
	}
	else
	{
		_gDebugLogPtr(text);
	}
}
/* End of setup log mechanism */

static UINT _gFrameCount = 0;

extern "C"
{
	EXPORT_API void PluginSetLogDelegate(DebugLogPtr fp)
	{
		_gDebugLogPtr = fp;
	}

	EXPORT_API void PluginLoadImage(char* path)
	{
		if (NULL == path)
		{
			return;
		}

		char buffer[256] = { 0 };
		sprintf_s(buffer, "PluginLoadImage: %s", path);
		Debug(buffer);

		HRESULT hr = CoInitialize(NULL);

		// The factory pointer
		IWICImagingFactory* pFactory = nullptr;

		// hr = CoCreateInstance(CLSID_WICImagingFactory, NULL, CLSCTX_INPROC_SERVER, IID_IWICImagingFactory, (LPVOID*)&pFactory);

		// Create the COM imaging factory
		hr = CoCreateInstance(
			CLSID_WICImagingFactory,
			NULL,
			CLSCTX_INPROC_SERVER,
			IID_PPV_ARGS(&pFactory)
		);

		// Decode the source image to IWICBitmapSource
		IWICBitmapDecoder *pDecoder = nullptr;

		CString strPath = path;

		// Create a decoder for the given image file
		hr = pFactory->CreateDecoderFromFilename(
			strPath, // Image to be decoded
			NULL,                            // Do not prefer a particular vendor
			GENERIC_READ,                    // Desired read access to the file
			WICDecodeMetadataCacheOnDemand,  // Cache metadata when needed
			&pDecoder                        // Pointer to the decoder
		);

		IWICBitmapFrameDecode* frame = NULL;

		IWICBitmapScaler *pScaler = nullptr;
		IWICBitmapFrameDecode *pWicFrame = nullptr;

		UINT nWidth = 0;
		UINT nHeight = 0;
		UINT uiFrameCount = 0;

		if (SUCCEEDED(hr))
		{
			Debug("Loaded image.");

			UINT frameCount = 1;
			hr = pDecoder->GetFrameCount(&uiFrameCount);
			_gFrameCount = uiFrameCount;
		}
		else
		{
			Debug("Failed to load image!");

			_gFrameCount = 0;
		}

		for (UINT frameIndex = 0; frameIndex < _gFrameCount; ++frameIndex)
		{

		}

		if (pDecoder)
		{
			pDecoder->Release();
		}

		if (pFactory)
		{
			pFactory->Release();
		}
	}

	EXPORT_API int PluginGetFrameCount()
	{
		//Debug("PluginGetFrameCount:");
		return (int)_gFrameCount;
	}

	EXPORT_API int PluginGetHeight()
	{
		Debug("PluginGetHeight:");
		return 0;
	}

	EXPORT_API int PluginGetWidth()
	{
		Debug("PluginGetWidth:");
		return 0;
	}

	EXPORT_API int PluginGetPixel(int frame, int x, int y)
	{
		char buffer[256] = { 0 };
		sprintf_s(buffer, "PluginGetPixel: frame=%d x=%d y=%d", frame, x, y);
		Debug(buffer);
		return 0;
	}
}
