// stdafx.cpp : source file that includes just the standard includes
// *.pch will be the pre-compiled header
// stdafx.obj will contain the pre-compiled type information

// DebugLog reference: http://hojjatjafary.blogspot.ca/2013/01/c-plugin-debug-log.html

#include "stdafx.h"
#include "atlstr.h"
#include <iostream>
#include "wincodec.h"
#include "winnt.h"
#include <vector>

// TODO: reference any additional headers you need in STDAFX.H
// and not in this file

#pragma comment( lib, "windowscodecs.lib" )


using namespace std;


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

std::vector<std::vector<std::vector<int>>> _gAnimation = std::vector<std::vector<std::vector<int>>>();

void CopyPixels(COLORREF* pColor, UINT width, UINT height)
{
	vector<vector<int>> colors = vector<vector<int>>();
	for (int i = 0; i < (int)height; i++)
	{
		COLORREF* nextRow = pColor + width;
		vector<int> row = vector<int>();
		for (int j = 0; j < (int)width; j++)
		{
			int red = GetBValue(*pColor);
			int green = GetGValue(*pColor) << 8;
			int blue = GetRValue(*pColor) << 16;

			int color = red | green | blue;
			row.push_back(color);			
			pColor++;
		}
		colors.push_back(row);
		pColor = nextRow;
	}
	_gAnimation.push_back(colors);
}

extern "C"
{
	EXPORT_API void PluginSetLogDelegate(DebugLogPtr fp)
	{
		_gDebugLogPtr = fp;
	}

	EXPORT_API void PluginLoadImage(char* path)
	{
		_gAnimation.clear();

		if (NULL == path)
		{
			return;
		}

		/*
		char buffer[256] = { 0 };
		sprintf_s(buffer, "PluginLoadImage: %s", path);
		Debug(buffer);
		*/

		HRESULT hr = CoInitialize(NULL);

		// The factory pointer
		IWICImagingFactory* pFactory = nullptr;

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

		// the number of frames
		UINT frameCount = 0;

		if (SUCCEEDED(hr))
		{
			//Debug("Loaded image.");
			hr = pDecoder->GetFrameCount(&frameCount);
		}
		else
		{
			//Debug("Failed to load image!");
			frameCount = 0;
		}

		for (UINT frameIndex = 0; frameIndex < frameCount; ++frameIndex)
		{
			// holds the frame data
			IWICBitmapFrameDecode* pFrame = NULL;

			// Retrieve the first frame of the image from the decoder
			if (SUCCEEDED(hr))
			{
				hr = pDecoder->GetFrame(frameIndex, &pFrame);
			}

			// Retrieve IWICBitmapSource from the frame
			IWICBitmapSource* pOriginalBitmapSource = NULL;
			if (SUCCEEDED(hr))
			{
				hr = pFrame->QueryInterface(IID_IWICBitmapSource, reinterpret_cast<void **>(&pOriginalBitmapSource));
			}

			IWICBitmapSource* pToRenderBitmapSource = NULL;

			// convert the pixel format
			if (SUCCEEDED(hr))
			{
				IWICFormatConverter* pConverter = NULL;

				hr = pFactory->CreateFormatConverter(&pConverter);

				// Format convert to 32bppBGR
				if (SUCCEEDED(hr))
				{
					hr = pConverter->Initialize(
						pFrame,                          // Input bitmap to convert
						GUID_WICPixelFormat32bppBGR,     // Destination pixel format
						WICBitmapDitherTypeNone,         // Specified dither patterm
						NULL,                            // Specify a particular palette 
						0.f,                             // Alpha threshold
						WICBitmapPaletteTypeCustom       // Palette translation type
					);

					// Store the converted bitmap if successful
					if (SUCCEEDED(hr))
					{
						hr = pConverter->QueryInterface(IID_PPV_ARGS(&pToRenderBitmapSource));
					}
				}

				if (pConverter)
				{
					pConverter->Release();
				}
			}

			// Check BitmapSource format
			WICPixelFormatGUID pixelFormat;
			if (SUCCEEDED(hr))
			{
				hr = pToRenderBitmapSource->GetPixelFormat(&pixelFormat);
			}

			if (SUCCEEDED(hr))
			{
				hr = (pixelFormat == GUID_WICPixelFormat32bppBGR) ? S_OK : E_FAIL;
			}

			// the width of the frame
			UINT nWidth = 0;

			// the height of the frame
			UINT nHeight = 0;

			// Retrieve the image dimensions
			if (SUCCEEDED(hr))
			{
				hr = pFrame->GetSize(&nWidth, &nHeight);
			}

			//_gImageData.push_back();

			// Create a DIB from the converted IWICBitmapSource
			HBITMAP hDIBBitmap = 0;
			if (SUCCEEDED(hr))
			{
				// Get image attributes and check for valid image
				UINT width = 0;
				UINT height = 0;

				void *ImageBits = NULL;

				// Check BitmapSource format
				WICPixelFormatGUID pixelFormat;
				hr = pToRenderBitmapSource->GetPixelFormat(&pixelFormat);

				if (SUCCEEDED(hr))
				{
					hr = (pixelFormat == GUID_WICPixelFormat32bppBGR) ? S_OK : E_FAIL;
				}

				if (SUCCEEDED(hr))
				{
					hr = pToRenderBitmapSource->GetSize(&width, &height);
				}

				// Holds the image pixels
				void* pImageBits = NULL;

				// Create a DIB section based on Bitmap Info
				// BITMAPINFO Struct must first be setup before a DIB can be created.
				// Note that the height is negative for top-down bitmaps
				if (SUCCEEDED(hr))
				{
					BITMAPINFO bminfo;
					ZeroMemory(&bminfo, sizeof(bminfo));
					bminfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
					bminfo.bmiHeader.biWidth = width;
					bminfo.bmiHeader.biHeight = -(LONG)height;
					bminfo.bmiHeader.biPlanes = 1;
					bminfo.bmiHeader.biBitCount = 32;
					bminfo.bmiHeader.biCompression = BI_RGB;

					// Get a DC for the full screen
					HDC hdcScreen = GetDC(NULL);

					hr = hdcScreen ? S_OK : E_FAIL;

					// Release the previously allocated bitmap 
					if (SUCCEEDED(hr))
					{
						if (hDIBBitmap)
						{
							DeleteObject(hDIBBitmap);
						}

						hDIBBitmap = CreateDIBSection(hdcScreen, &bminfo, DIB_RGB_COLORS, &pImageBits, NULL, 0);

						ReleaseDC(NULL, hdcScreen);

						hr = hDIBBitmap ? S_OK : E_FAIL;
					}
				}

				UINT cbStride = 0;
				if (SUCCEEDED(hr))
				{
					// Size of a scan line represented in bytes: 4 bytes each pixel
					hr = UIntMult(width, sizeof(DWORD), &cbStride);
				}

				UINT cbImage = 0;
				if (SUCCEEDED(hr))
				{
					// Size of the image, represented in bytes
					hr = UIntMult(cbStride, height, &cbImage);
				}

				// Extract the image into the HBITMAP    
				if (SUCCEEDED(hr) && pToRenderBitmapSource)
				{
					hr = pToRenderBitmapSource->CopyPixels(
						NULL,
						cbStride,
						cbImage,
						reinterpret_cast<BYTE *> (pImageBits));

					COLORREF* pColor = (COLORREF*)pImageBits;
					CopyPixels(pColor, width, height);
				}

				// done with bitmap
				if (hDIBBitmap)
				{
					DeleteObject(hDIBBitmap);
					hDIBBitmap = NULL;
				}
			}

			if (pOriginalBitmapSource)
			{
				pOriginalBitmapSource->Release();
			}

			if (pToRenderBitmapSource)
			{
				pToRenderBitmapSource->Release();
			}

			if (pFrame)
			{
				pFrame->Release();
			}
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
		return (int)_gAnimation.size();
	}

	EXPORT_API int PluginGetHeight()
	{
		//Debug("PluginGetHeight:");
		if (_gAnimation.size() > 0)
		{
			return (int)_gAnimation[0].size();
		}
		return 0;
	}

	EXPORT_API int PluginGetWidth()
	{
		//Debug("PluginGetWidth:");
		if (_gAnimation.size() > 0 &&
			_gAnimation[0].size() > 0)
		{
			return (int)_gAnimation[0][0].size();
		}
		return 0;
	}

	EXPORT_API int PluginGetPixel(int frame, int x, int y)
	{
		/*
		char buffer[256] = { 0 };
		sprintf_s(buffer, "PluginGetPixel: frame=%d x=%d y=%d", frame, x, y);
		Debug(buffer);
		*/
		if (frame < 0 ||
			x < 0 ||
			y < 0)
		{
			return 0;
		}
		if (frame < (int)_gAnimation.size() &&
			y < (int)_gAnimation[frame].size() &&
			x < (int)_gAnimation[frame][y].size())
		{
			return _gAnimation[frame][y][x];
		}
		return 0;
	}
}
