#pragma once

#include <iostream>
#include <cctype>
#include <functional>
#include <vector>
#include <set>

namespace openanim {

struct Token {
	unsigned line;
	std::string value;
};

inline bool isspaceAdapter(char c) { return std::isspace(c); }

/// This is a very simple tokenizer. A more generic solution would use a proper parser,
/// but for simple text-based file formats that is not really necessary.
/// I might rewrite this once it is not enough :)
std::vector<Token> tokenize(std::istream& in, 
	std::function<bool(char)> skip = isspaceAdapter, 
	std::set<char> separateTokens = {'(', ')'}, 
	char commentStart = '#', char commentEnd = '\n');

}