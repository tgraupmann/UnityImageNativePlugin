// stdafx.cpp : source file that includes just the standard includes
// *.pch will be the pre-compiled header
// stdafx.obj will contain the pre-compiled type information

#include "stdafx.h"
#include <iostream>

// TODO: reference any additional headers you need in STDAFX.H
// and not in this file

extern "C"
{
	EXPORT_API void PluginLoadImage(char* path)
	{
		if (NULL == path)
		{
			return;
		}

		printf("PluginLoadImage: %s", path);

		delete path;
	}

	EXPORT_API int PluginGetFrameCount()
	{
		printf("PluginGetFrameCount:");
		return 0;
	}

	EXPORT_API int PluginGetPixel(int frame, int x, int y)
	{
		printf("PluginGetPixel: frame=%d x=%d y=%d", frame, x, y);
		return 0;
	}
}