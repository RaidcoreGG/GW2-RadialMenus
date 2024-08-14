#ifndef UTIL_H
#define UTIL_H

#include <string>

namespace URL
{
	std::string GetBase(const std::string& aUrl);

	std::string GetEndpoint(const std::string& aUrl);
}

#endif
