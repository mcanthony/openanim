#include "Tokenizer.h"

#include <cassert>

namespace openanim {

Tokenizer::Tokenizer(std::istream& in) : m_input(in), m_active(NULL), m_line(1) {
}

bool Tokenizer::eof() const {
	return m_input.eof() || (!m_input.good());
}

const Tokenizer::Token& Tokenizer::current() const {
	return m_current;
}

const Tokenizer::Token& Tokenizer::next() {
	// current and future values should be empty
	assert(m_future.value.empty());
	m_current.value = "";

	// loop until emit is called, or until the end of file 
	// (emit sets m_current value)
	assert(m_active != NULL);
	while(m_current.value.empty() && (!eof()))
		// process next-to-be-read character
		// (accept() / reject() call get() to move on the next character;
		// allows to switch states without reading anything)
		m_active->m_parse(m_input.peek());

	// if no explicit emit() was called, the eof() is true, and the
	// future token contains data, emit
	if(eof() && (!m_future.value.empty()))
		emit();

	// after emit, future should be empty and current should not (or eof)
	assert(m_future.value.empty());
	assert(!m_current.value.empty() || eof());

	return m_current;
}

void Tokenizer::emit() {
	// emit only non-empty tokens
	if(!m_future.value.empty()) {
		assert(m_current.value.empty());
		
		m_current = m_future;
		m_future.value = "";
	}
}

void Tokenizer::accept(char c) {
	// remember the line the token started on
	if(m_future.value.empty())
		m_future.line = m_line;
	
	// add the character to the future token
	m_future.value += c;

	// and skip to the next character
	reject();
}

void Tokenizer::reject() {
	// and read next character, counting the lines in the process
	if(m_input.get() == '\n')
		++m_line;
}

/////

Tokenizer::State::State(Tokenizer* parent) : m_parent(parent) {
}

Tokenizer::State& Tokenizer::State::operator = (const std::function<void(char)>& parser) {
	m_parse = parser;

	return *this;
}

void Tokenizer::State::setActive() {
	assert(m_parent != NULL);
	m_parent->m_active = this;
}

}