#include "Util.h"

#include <chrono>

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
}

namespace Time
{
	unsigned long long GetTimestampMillis()
	{
		return std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
	}
}
