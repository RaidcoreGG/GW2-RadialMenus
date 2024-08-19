#ifndef UTIL_H
#define UTIL_H

#include <string>

namespace URL
{
	std::string GetBase(const std::string& aUrl);

	std::string GetEndpoint(const std::string& aUrl);
}

namespace String
{
	std::string Replace(const std::string& aString, const std::string& aOld, const std::string& aNew, size_t aPosition = 0);
}

#endif
