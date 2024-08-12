#pragma once
#include <string>
struct State
{
	std::string text;
	long long number;
	bool flag;
	template <typename Archive>
	void serialize(Archive& ar, const unsigned int version)
	{
		std::ignore = version;
		ar& text;
		ar& number;
		ar& flag;
	}
};