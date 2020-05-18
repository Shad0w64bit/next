#include <fstream>
#include <vector>
#include <cstring>

//#include "../core/types.h"
#include "token.h"

class Lexer
{
public:
	Lexer(std::ifstream& stream, std::vector<Token*>& tokens):
		m_stream(stream), m_tokens(tokens) {}
	
	void analyze();
private:
	std::ifstream& m_stream;
	std::vector<Token*>& m_tokens;
	
//	Namespaces m_namespaces;
	
//	Types types;
	
	bool isKeyword(const char* str);
//	bool isType(const char* str);
	
	void step1(); // File to lexem
	void step2(); // Fix type
	
	bool testDefinition(std::vector<Token*>::iterator it);
	bool testTypedefDefinition(std::vector<Token*>::iterator it);
	bool testObjectDefinition(std::vector<Token*>::iterator it);
	bool testClassDefinition(std::vector<Token*>::iterator it);
	bool testUse(std::vector<Token*>::iterator it);
	bool testFunction(std::vector<Token*>::iterator it);
	
//	void fixUse(std::vector<Token*>::iterator it);
	void fixFunction(std::vector<Token*>::iterator it);
	bool fixNamespaceDefinition(std::vector<Token*>::iterator it);
		
/*	bool testNewType(std::vector<Token*>::iterator it);
	bool testNewNamespace(std::vector<Token*>::iterator it);
	
	bool testAssign(std::vector<Token*>::iterator it);*/
};

#define INSERT_TOKEN() { if((token->kind() == Token::Kind::Identifier) && isKeyword(word.c_str())) { token->setKind(Token::Kind::Keyword); }; int len = word.length() + 1; char* w = (char*) malloc(len); memcpy(w, word.c_str(), len); token->set(w); lastToken = token; m_tokens.insert(m_tokens.end(), token); }
#define INSERT_TOKEN_AND_CLEAR() { INSERT_TOKEN(); token = nullptr; word.clear(); }

#define TEST_CHAR(char,type) else if (c == char) { token = new Token(Token::Kind::type); INSERT_TOKEN_AND_CLEAR(); }

void Lexer::step1()
{
	Token* token = nullptr;
	Token* lastToken = nullptr;

	char c;
	std::string word;
	bool lcomment, mcomment, escape, str;
	mcomment = lcomment = escape = str = false;
	
/*	Namespace* ns = new Namespace("Program");
	m_namespaces.add(ns);*/
	
	int line = 1;
	
	while ((c = m_stream.get()) != EOF)
	{
lex_start:
		if ((!lcomment && !mcomment) && (c == '/')) {
			auto pos = m_stream.tellg();
			c = m_stream.get();
			if (c == '*') {
				token = new Token(Token::Kind::Comment);
				word.clear();
				mcomment = true;
				continue;
			}
			m_stream.seekg(pos);
		} else if (mcomment && c == '*') {
			auto pos = m_stream.tellg();
			c = m_stream.get();
			if (c == '/') {
				mcomment = false;
				INSERT_TOKEN_AND_CLEAR();
				continue;
			}
			m_stream.seekg(pos);
		}
		
		if ((!lcomment && !mcomment) && (c == '/')) {
			auto pos = m_stream.tellg();
			c = m_stream.get();
			if (c == '/') {
				lcomment = true;
				word.clear();
				token = new Token(Token::Kind::Comment);
				continue;
			}
			m_stream.seekg(pos);
		} else if (lcomment && (c == (int)13)) {
			lcomment = false;
			INSERT_TOKEN_AND_CLEAR();
			continue;
		}
		
		if (lcomment || mcomment) {
			word += c;
			continue;
		}

		if (str && c == '\\')
		{
			if (escape) {
				word +=c;
				escape = false;
			}
			escape = true;
			continue;
		} else if (str && !escape and (c != '\''))
		{
			word += c;
			escape = false;
			continue;
		} else if (str and escape)
		{
			switch(c) {
				case 'n': word += "\n"; break;
				case 't': word += "\t"; break;
				default: word += c; break;
			}
			escape = false;
			continue;
		}
		
		// First init
		if (token == nullptr)
		{
			word = c;
			if (isalpha(c) || (c == '_') || (c == '^')) {
				token = new Token(Token::Kind::Identifier);
				continue;
			} else if (isdigit(c)) {
				token = new Token(Token::Kind::Number);
				continue;
			/*} else if (c == '\'') {
				token = new Token(Token::Kind::String);
				str = true;
				continue;
			}*/
			} else if (c == '\'') {
/*				token = new Token(Token::Kind::SingleQuote);
				INSERT_TOKEN_AND_CLEAR();*/
				word.clear();
				token = new Token(Token::Kind::String);
				str = true;
				continue;
			} else if (c == (int)13) {
				line++;
				if (!escape) {
					if ((lastToken != nullptr) && 
						(lastToken->kind() != Token::Kind::LineBreak))
					{
						token = new Token(Token::Kind::LineBreak);
						INSERT_TOKEN_AND_CLEAR();						
					}
					continue;
				}
			}
			else if (c == '=') {
				auto pos = m_stream.tellg();
				c = m_stream.get();
				if (c == '=') {
					word += c;
					token = new  Token(Token::Kind::Equal);
					INSERT_TOKEN_AND_CLEAR();
					continue;
				}				
				m_stream.seekg(pos);
				token = new Token(Token::Kind::Assign);
				INSERT_TOKEN_AND_CLEAR();
				continue;
			} else if (c == '!') {
				auto pos = m_stream.tellg();
				c = m_stream.get();
				if (c == '=') {
					word += c;
					token = new  Token(Token::Kind::Equal);
					INSERT_TOKEN_AND_CLEAR();
					continue;
				}
				m_stream.seekg(pos);
				token = new Token(Token::Kind::NotEqual);
				INSERT_TOKEN_AND_CLEAR();
				continue;
			} else if (c == '\\') {
				auto pos = m_stream.tellg();
				m_stream.seekg(-2, std::ios_base::cur);				
				c = m_stream.get();
				if ((c == ' ') || (c == (int)9))
				{
					c = m_stream.get();
					c = m_stream.get();
					if (c == (int)13)
					{
						line++;
						word.clear();
						token = nullptr;
						continue;
					}
				}
				m_stream.seekg(pos);
				token = new Token(Token::Kind::Backslash);
				INSERT_TOKEN_AND_CLEAR();
				continue;
			}
//			TEST_CHAR('=', Equal)
			TEST_CHAR('+', Plus)
			TEST_CHAR('-', Minus)
			TEST_CHAR('*', Asterisk)
			TEST_CHAR('/', Slash)
//			TEST_CHAR('\\', Backslash)
			TEST_CHAR('(', LeftParen)
			TEST_CHAR(')', RightParen)
			TEST_CHAR('[', LeftSquare)
			TEST_CHAR(']', RightSquare)
			TEST_CHAR('{', LeftCurly)
			TEST_CHAR('}', RightCurly)
			TEST_CHAR(',', Comma)
			TEST_CHAR(';', Semicolon)
//			TEST_CHAR((int)13, LineBreak)
			
		} else { // Secondary
			Token::Kind kind = token->kind();
			
			if (kind == Token::Kind::Identifier) {
				if (isalpha(c) || isdigit(c) || (c == '_')) {
					word += c;
					continue;
				}
				goto lex_new;
				
			} else if (kind == Token::Kind::Number) {
				if (isdigit(c)) {
					word += c;
					continue;
				}
				goto lex_new;
			} else if (kind == Token::Kind::String) {
				if (c == '\'') {
					str = false;
					INSERT_TOKEN_AND_CLEAR(); // Insert String
/*					word = c;
					token = new Token(Token::Kind::SingleQuote);
					INSERT_TOKEN_AND_CLEAR(); // Insert Quote*/
				} 
				continue;
			}
			
			
			/*
			else if (kind == Token::Kind::String) {
				word += c;
				if (c == '\'') {
					str = false;
					INSERT_TOKEN_AND_CLEAR();
				} 
				continue;
			}
			*/
		}

		continue;
lex_new:
		INSERT_TOKEN_AND_CLEAR();
		goto lex_start;		
	}
	
	if (token != nullptr) {
		INSERT_TOKEN_AND_CLEAR();		
	}
	
}

bool Lexer::testClassDefinition(std::vector<Token*>::iterator it)
{
	if (((*it)->kind() == Token::Kind::Keyword) && 
		(strcmp((*it)->get(), "class") == 0))
	{
		it++;
		if (((*it)->kind() == Token::Kind::Identifier) ||
			((*it)->kind() == Token::Kind::Keyword))
		{
			it++;
			auto e = (*it)->kind();
			if ((e == Token::Kind::Semicolon) || 
				(e == Token::Kind::LineBreak) || 
				(e == Token::Kind::LeftCurly))
			{
				return true;
			}
		}
	}
	return false;
}

bool Lexer::testObjectDefinition(std::vector<Token*>::iterator it)
{
	if (((*it)->kind() == Token::Kind::Keyword) && 
		(strcmp((*it)->get(), "object") == 0))
	{
		it++;
		if (((*it)->kind() == Token::Kind::Identifier) ||
			((*it)->kind() == Token::Kind::Keyword))
		{
			it++;
			auto e = (*it)->kind();
			if ((e == Token::Kind::Semicolon) || 
				(e == Token::Kind::LineBreak) || 
				(e == Token::Kind::LeftCurly))
			{
				return true;
			}
		}
	}
	return false;
}

void Lexer::fixFunction(std::vector<Token*>::iterator it)
{
	if (!((*it)->is(Token::Kind::Keyword) && (strcmp((*it)->get(), "func") == 0)))
	{
		(*it)->setKind(Token::Kind::Type);
	}
	
	it++;
	(*it)->setKind(Token::Kind::Identifier);
	
	it++;
	
	if ((*it)->is(Token::Kind::LeftParen)) {
		it++;
		while (!((*it)->is(Token::Kind::RightParen) || (*it)->is(Token::Kind::Semicolon)))
		{			
			std::vector<Token*>::iterator next = it;
			next++;
			if ( ((*it)->is(Token::Kind::Identifier) || (*it)->is(Token::Kind::Keyword) ) && 
				((*next)->is(Token::Kind::Identifier) || (*next)->is(Token::Kind::Keyword) ) )
			{
				(*it)->setKind(Token::Kind::Type);
				(*next)->setKind(Token::Kind::Identifier);				
			}
			
			it++;
		}
		
		it++;
		
		if ((*it)->is(Token::Kind::Keyword) && (strcmp( (*it)->get(), "as") == 0))
		{
			it++; 
			(*it)->setKind(Token::Kind::Identifier);
		}
	}
}

bool Lexer::testFunction(std::vector<Token*>::iterator it)
{
	if ( ((*it)->is(Token::Kind::Keyword) && (strcmp((*it)->get(), "func") == 0)) || 
		 (*it)->is(Token::Kind::Type) || (*it)->is(Token::Kind::Identifier) )
	{				
		it++;
		if ((*it)->is(Token::Kind::Identifier)) {
			
			it++;
			if ((*it)->is(Token::Kind::LeftParen)) { 
				it++;
				while (!(*it)->is(Token::Kind::RightParen) )
				{
					if (!((*it)->is(Token::Kind::Identifier) || (*it)->is(Token::Kind::Type) || (*it)->is(Token::Kind::Comma)) )
					{
						return false;
					}
					
					it++;
				}				
				return true;
			}
		}
		
	}
	return false;
}

bool Lexer::fixNamespaceDefinition(std::vector<Token*>::iterator it)
{
	if (((*it)->is(Token::Kind::Keyword)) && (strcmp((*it)->get(), "namespace") == 0))
	{
		it++;
		 for(;(((*it)->kind()==Token::Kind::Identifier) || ((*it)->kind()==Token::Kind::Keyword) || ((*it)->kind()==Token::Kind::Backslash)); ++it)
		 {
			if ((*it)->kind()==Token::Kind::Keyword) {
				(*it)->setKind(Token::Kind::Identifier);
			}
		 }
		 return true;
	}
	return false;
}

bool Lexer::testTypedefDefinition(std::vector<Token*>::iterator it)
{
	if (((*it)->kind() == Token::Kind::Keyword) && 
		(strcmp((*it)->get(), "typedef") == 0))
	{
		it++;
		if (((*it)->kind() == Token::Kind::Identifier) ||
			((*it)->kind() == Token::Kind::Keyword))
		{
			it++;
			if (((*it)->kind() == Token::Kind::Identifier) ||
			((*it)->kind() == Token::Kind::Keyword))
			{
				return true;
			}
		}
	}
	return false;
}

bool Lexer::testDefinition(std::vector<Token*>::iterator it)
{
	if (((*it)->is(Token::Kind::Identifier)) ||
	    (((*it)->is(Token::Kind::Keyword)) && !(strcmp((*it)->get(), "namespace") == 0)))
	{
		it++;
		
		if (((*it)->kind() == Token::Kind::Keyword) ||
			((*it)->kind() == Token::Kind::Identifier))
		{
			while ( ((*it)->kind() == Token::Kind::Keyword) ||
					((*it)->kind() == Token::Kind::Identifier) ||
					((*it)->kind() == Token::Kind::Comma) )
			{
				it++;
			}
			
			if (((*it)->kind() == Token::Kind::LineBreak) ||
				((*it)->kind() == Token::Kind::Semicolon) ||
				((*it)->kind() == Token::Kind::Assign))
			{
				return true;
			} else {
				return false;
			}

			return true;
		}
	}
	return false;
}


void fixDefinition(std::vector<Token*>::iterator it)
{
	if ((*it)->kind() == Token::Kind::Identifier)
	{
		(*it)->setKind(Token::Kind::Type);
		it++;
				
		while ( ((*it)->kind() == Token::Kind::Keyword) ||
				((*it)->kind() == Token::Kind::Identifier) ||
				((*it)->kind() == Token::Kind::Comma) )
		{
			if ((*it)->kind() != Token::Kind::Comma)
				(*it)->setKind(Token::Kind::Identifier);
			it++;
		}
	}
}



bool Lexer::testUse(std::vector<Token*>::iterator it)
{
	if (((*it)->kind() == Token::Kind::Keyword) && 
		(strcmp((*it)->get(), "use") == 0))
	{
		it++;
		for(;(((*it)->kind()==Token::Kind::Identifier) || ((*it)->kind()==Token::Kind::Keyword) || ((*it)->kind()==Token::Kind::Backslash)); ++it)
		{}
		
		if (((*it)->kind() == Token::Kind::LineBreak) || ((*it)->kind() == Token::Kind::Semicolon))
			return true;
		
		for(;(((*it)->kind()!=Token::Kind::LineBreak) && ((*it)->kind()!=Token::Kind::Semicolon)); ++it) {}
		
		auto l1 = --it;
		auto l2 = --it;
		
		if ((((*l1)->kind() == Token::Kind::Keyword) || ((*l1)->kind() == Token::Kind::Identifier) || ((*l1)->kind() == Token::Kind::Type)) &&
			(((*l2)->kind() == Token::Kind::Modificator) || 
			(((*l2)->kind() == Token::Kind::Keyword) && (strcmp((*l2)->get(), "as") == 0))))
		{
				return true;
		}
	}
	return false;
}
/*
void Lexer::fixUse(std::vector<Token*>::iterator it)
{
	it++;
	for(;(((*it)->kind()==Token::Kind::Identifier) || ((*it)->kind()==Token::Kind::Keyword) || ((*it)->kind()==Token::Kind::Backslash)); ++it)
	{
		if 
	}
	
}*/

void Lexer::step2()
{
	// Modificator
	
	for (auto it = m_tokens.begin(); it != m_tokens.end(); ++it)
	{
		if (testUse(it))
		{
			for(;(((*it)->kind()!=Token::Kind::LineBreak) && ((*it)->kind()!=Token::Kind::Semicolon)); ++it) {}
			auto l1 = --it;
			auto l2 = --it;
			
			if ((((*l1)->kind() == Token::Kind::Keyword) || ((*l1)->kind() == Token::Kind::Identifier)) &&
				((*l2)->kind() == Token::Kind::Keyword) && (strcmp((*l2)->get(), "as") == 0))
			{
				(*l2)->setKind(Token::Kind::Modificator);
				(*l1)->setKind(Token::Kind::Type);
			}
			
		}
		
	}
	
	// End Modificator
	
	for (auto it = m_tokens.begin(); it != m_tokens.end(); ++it)
	{
		if ((*it)->kind() == Token::Kind::LineBreak)
			continue;
		
		if (fixNamespaceDefinition(it)) {}
		else if (testClassDefinition(it)) {
			it++;
			(*it)->setKind(Token::Kind::Type);
		} else if (testObjectDefinition(it)) {
			it++;
			(*it)->setKind(Token::Kind::Type);
		} else if (testTypedefDefinition(it)) {
			it++; (*it)->setKind(Token::Kind::Type);
			it++; (*it)->setKind(Token::Kind::Type);
		} else if (testFunction(it)) {			
			fixFunction(it);
		} else if (testDefinition(it)) { // Must be Last Definition
			fixDefinition(it);
		} else if (testUse(it)) {
			it++;
			Token* last = nullptr;			
			for(;(((*it)->kind()==Token::Kind::Identifier) || ((*it)->kind()==Token::Kind::Backslash) || ((*it)->kind()==Token::Kind::Keyword)); ++it)
			{
				last = (*it);
				if (last->kind()==Token::Kind::Keyword)
					last->setKind(Token::Kind::Identifier);
			}
			
			if (last->kind()==Token::Kind::Identifier)
				last->setKind(Token::Kind::Type);
		}
	}
	
}

void Lexer::analyze()
{
	for (auto it = m_tokens.begin(); it != m_tokens.end(); ++it)
	{
		delete (*it);
	}
	m_tokens.clear();
	
	step1();
	step2();
}

bool Lexer::isKeyword(const char* str) 
{
	const char* const names[]{
		"func", "return", "typedef", "class", "object",
		"namespace", "use", "as", "from", "__cdecl",
	};
	
	for(auto n : names) {
		if (strcmp(str, n) == 0) 
		return true;
	}
	
	return false;
}
