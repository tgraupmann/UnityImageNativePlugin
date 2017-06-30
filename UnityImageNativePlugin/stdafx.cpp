// stdafx.cpp : source file that includes just the standard includes
// *.pch will be the pre-compiled header
// stdafx.obj will contain the pre-compiled type information

#include "stdafx.h"

// TODO: reference any additional headers you need in STDAFX.H
// and not in this file

extern "C"
{
	EXPORT_API void LoadImage()
	{
	}

	EXPORT_API int GetFrameCount()
	{
		return 0;
	}

	EXPORT_API int GetPixel(int frame, int x, int y)
	{
		return 0;
	}
}