#ifndef UTIL_H
#define UTIL_H

#include <Windows.h>
#include <string>

/* FIXME: this entire file is copied stuff from Nexus/Util. */

namespace URL
{
	std::string GetBase(const std::string& aUrl);

	std::string GetEndpoint(const std::string& aUrl);
}

namespace String
{
	std::string Replace(const std::string& aString, const std::string& aOld, const std::string& aNew, size_t aPosition = 0);

	std::string Format(std::string aFmt, ...);
}

namespace Time
{
	unsigned long long GetTimestampMillis();
}

namespace Input
{
	typedef struct KeystrokeMessageFlags
	{
		unsigned RepeatCount : 16;
		unsigned ScanCode : 8;
		unsigned ExtendedFlag : 1;
		unsigned Reserved : 4;
		unsigned ContextCode : 1;
		unsigned PreviousKeyState : 1;
		unsigned TransitionState : 1;

		///----------------------------------------------------------------------------------------------------
		/// GetScanCode:
		/// 	Returns the scancode (including extended key flag) of the LParam.
		///----------------------------------------------------------------------------------------------------
		unsigned short GetScanCode();

		KeystrokeMessageFlags() = default;
		KeystrokeMessageFlags(LPARAM aLParam);
	} KeyLParam;

	LPARAM& KMFToLParam(KeystrokeMessageFlags& aKmf);

	LPARAM GetKeyMessageLPARAM(unsigned aVKey, bool aIsDown, bool aIsSystem);
}

#endif
