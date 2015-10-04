#pragma once

#include <type_traits>
#include <string>

#include <boost/lexical_cast.hpp>

namespace openanim {

template<typename TARGET, typename SOURCE>
struct LexicalCastHelper {
	static TARGET cast(const SOURCE& arg) {
		return boost::lexical_cast<TARGET>(arg);
	}
};

template<typename TARGET, typename SOURCE>
TARGET lexical_cast(const SOURCE& arg) {
	return LexicalCastHelper<TARGET, SOURCE>::cast(arg);
}

///
	
template<typename T>
struct LexicalCastHelper<std::string, std::vector<T>> {
	static std::string cast(const std::vector<T>& arg) {
		std::string result = "(";

		bool first = true;
		for(auto& i : arg) {
			if(!first)
				result += ", ";
			first = false;

			result += lexical_cast<std::string>(i);
		}

		// return boost::lexical_cast<TARGET>(arg);
		return result + ")";
	}
};

}