#include "Tokenizer.h"

#include <sstream>

using std::cout;
using std::endl;

namespace {
	void eatWhitespaces(std::istream& in, std::function<bool(char)> skip) {
		while(in.good() && skip(in.peek()))
			in.get();
	}
}

/////////////////////

namespace openanim {

std::vector<Token> tokenize(std::istream& in, std::function<bool(char)> skip, std::set<char> separateTokens, char commentStart, char commentEnd) {
	std::vector<Token> tokens;

	// read lines until the EOF is reached
	unsigned lineCtr = 0;
	while(in.good()) {
		++lineCtr;

		// get a single line from the file
		std::string lineString;	
		std::getline(in, lineString);
		std::stringstream line(lineString + "\n");

		while(!line.bad() && !line.eof()) {
			eatWhitespaces(line, skip);

			// if a comment is found, skip it
			if(line.peek() == commentStart) {
				line.get();
				while(!line.bad() && line.get() != commentEnd)
					;
			}

			// if a separable character is found, use it as a token
			else if(separateTokens.find(line.peek()) != separateTokens.end()) {
				std::string token;
				token += line.get();

				tokens.push_back(Token{lineCtr, token});
			}

			// read a single normal token
			else {
				std::string current;
				while(line.good() && (!skip(line.peek()) && (separateTokens.find(line.peek()) == separateTokens.end())))
					current += line.get();
				if(!current.empty())
					tokens.push_back(Token{lineCtr, current});
			}
		}
	}

	// for(auto& t : tokens)
	// 	cout << t.line << " " << t.value << endl;

	return tokens;
}

}