#include "Util.h"

#include <chrono>
#include <fstream>

namespace URL
{
	std::string GetBase(const std::string& aUrl)
	{
		size_t httpIdx = aUrl.find("http://");
		size_t httpsIdx = aUrl.find("https://");

		size_t off = 0;
		if (httpIdx != std::string::npos)
		{
			off = httpIdx + 7; // 7 is length of "http://"
		}
		if (httpsIdx != std::string::npos)
		{
			off = httpsIdx + 8; // 8 is length of "https://"
		}

		size_t idx = aUrl.find('/', off);
		if (idx == std::string::npos)
		{
			return aUrl;
		}

		return aUrl.substr(0, idx);
	}

	std::string GetEndpoint(const std::string& aUrl)
	{
		size_t httpIdx = aUrl.find("http://");
		size_t httpsIdx = aUrl.find("https://");

		size_t off = 0;
		if (httpIdx != std::string::npos)
		{
			off = httpIdx + 7; // 7 is length of "http://"
		}
		if (httpsIdx != std::string::npos)
		{
			off = httpsIdx + 8; // 8 is length of "https://"
		}

		size_t idx = aUrl.find('/', off);
		if (idx == std::string::npos)
		{
			return aUrl;
		}

		return aUrl.substr(idx);
	}
}

namespace String
{
	std::string Replace(const std::string& aString, const std::string& aOld, const std::string& aNew, size_t aPosition)
	{
		std::string retStr = aString;
		if (aOld.empty())
		{
			return retStr;
		}

		size_t pos = aPosition;
		while ((pos = retStr.find(aOld, pos)) != std::string::npos)
		{
			retStr.replace(pos, aOld.length(), aNew);
			pos += aNew.length();
		}

		return retStr;
	}

#define MAX_STRING_FORMAT_LENGTH 4096
	std::string Format(std::string aFmt, ...)
	{
		va_list args;
		va_start(args, aFmt);
		char buffer[MAX_STRING_FORMAT_LENGTH];
		vsprintf_s(buffer, MAX_STRING_FORMAT_LENGTH - 1, aFmt.c_str(), args);
		va_end(args);

		return buffer;
	}
}

namespace Time
{
	unsigned long long GetTimestampMillis()
	{
		return std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
	}
}

namespace Input
{
	unsigned short KeystrokeMessageFlags::GetScanCode()
	{
		unsigned short ret = ScanCode;

		if (ExtendedFlag)
		{
			ret |= 0xE000;
		}

		return ret;
	}

	KeystrokeMessageFlags::KeystrokeMessageFlags(LPARAM aLParam)
	{
		/* direct assignment is not possible, so we cast and then copy */
		KeystrokeMessageFlags tmp = *(KeystrokeMessageFlags*)&aLParam;

		RepeatCount = tmp.RepeatCount;
		ScanCode = tmp.ScanCode;
		ExtendedFlag = tmp.ExtendedFlag;
		Reserved = tmp.Reserved;
		ContextCode = tmp.ContextCode;
		PreviousKeyState = tmp.PreviousKeyState;
		TransitionState = tmp.TransitionState;
	}

	LPARAM& KMFToLParam(KeystrokeMessageFlags& aKmf)
	{
		return *(LPARAM*)&aKmf;
	}

	LPARAM GetKeyMessageLPARAM(unsigned aVKey, bool aIsDown, bool aIsSystem)
	{
		KeyLParam lp;

		lp.RepeatCount = 1;
		lp.ScanCode = MapVirtualKeyA(aVKey, MAPVK_VK_TO_VSC);
		lp.ExtendedFlag = lp.ScanCode & 0xE0 ? 1 : 0;
		lp.Reserved = 0;
		lp.ContextCode = aIsSystem ? 1 : 0;
		lp.PreviousKeyState = aIsDown ? 0 : 1;
		lp.TransitionState = aIsDown ? 0 : 1;

		return KMFToLParam(lp);
	}
}

namespace Resources
{
	bool Get(HMODULE aModule, LPCSTR aResourceName, LPCSTR aResourceType, LPVOID* aOutLockedResource, DWORD* aOutResourceSize)
	{
		HRSRC hRes = FindResourceA(aModule, aResourceName, aResourceType);
		if (!hRes)
		{
			return false;
		}

		HGLOBAL hLRes = LoadResource(aModule, hRes);
		if (!hLRes)
		{
			return false;
		}

		LPVOID pLRes = LockResource(hLRes);
		if (!pLRes)
		{
			return false;
		}

		DWORD dwResSz = SizeofResource(aModule, hRes);
		if (!dwResSz)
		{
			return false;
		}

		*aOutLockedResource = pLRes;
		*aOutResourceSize = dwResSz;

		return true;
	}

	bool Unpack(HMODULE aModule, std::filesystem::path aPath, unsigned int aResourceID, std::string aResourceType, bool aIsBinary)
	{
		LPVOID res{}; DWORD sz{};
		Resources::Get(aModule, MAKEINTRESOURCE(aResourceID), aResourceType.c_str(), &res, &sz);

		try
		{
			if (std::filesystem::exists(aPath))
			{
				std::filesystem::remove(aPath);
			}

			std::ofstream file(aPath, aIsBinary ? std::ios::binary : 0);
			file.write((const char*)res, sz);
			file.close();
		}
		catch (...)
		{
			return false;
		}

		return true;
	}
}
