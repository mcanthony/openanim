#pragma once

#include <iostream>
#include <functional>

#include <boost/noncopyable.hpp>

namespace openanim {

/// a passive tokenizer class built on std::functions to do the main work - makes the code C++-like
/// but still really simple and short. Its not too powerful, though, about as much as regular languages
/// (which is kinda normal for tokenizers, discounting C++).
class Tokenizer : public boost::noncopyable {
	public:
		struct Token {
			unsigned line;
			std::string value;
		};

		Tokenizer(std::istream& in);
		
		// true if nothing more is to be read
		bool eof() const;
		
		// reads a next token and returns it
		const Token& next();
		// returns the current token
		const Token& current() const;
	
	protected:
		class State : public boost::noncopyable {
			public:
				State(Tokenizer* parent);
				
				State& operator = (const std::function<void(char)>& parser);

				void setActive();
				
			private:
				Tokenizer* m_parent;
				std::function<void(char)> m_parse;

			friend class Tokenizer;
		};
		
		// emit current token (on exit, of course)
		void emit();
		
		// accepts a character (can be case-converted if needed)
		void accept(char c);
		// skips current character
		void reject();
	
	private:
		std::istream& m_input;

		State* m_active;
		Token m_current, m_future;
		unsigned m_line;

	friend class State;
};

}