#include "Util.h"

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
