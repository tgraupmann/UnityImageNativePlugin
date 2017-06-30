// stdafx.cpp : source file that includes just the standard includes
// *.pch will be the pre-compiled header
// stdafx.obj will contain the pre-compiled type information

// DebugLog reference: http://hojjatjafary.blogspot.ca/2013/01/c-plugin-debug-log.html

#include "stdafx.h"
#include <iostream>

// TODO: reference any additional headers you need in STDAFX.H
// and not in this file


/* Setup log mechanism */
typedef void(*DebugLogPtr)(const char *);
DebugLogPtr _gDebugLogPtr;
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
		sprintf(buffer, "PluginLoadImage: %s", path);
		Debug(buffer);
	}

	EXPORT_API int PluginGetFrameCount()
	{
		Debug("PluginGetFrameCount:");
		return 0;
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
		sprintf(buffer, "PluginGetPixel: frame=%d x=%d y=%d", frame, x, y);
		Debug(buffer);
		return 0;
	}
}