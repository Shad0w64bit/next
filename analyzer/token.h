#ifndef TOKEN_ANALYZER
#define TOKEN_ANALYZER
class Token
{
public:
	enum class Kind {
		Identifier,
		Keyword,
		Number,
		Assign,
		
		LeftParen,
		RightParen,
		LeftSquare,
		RightSquare,
		LeftCurly,
		RightCurly,
		
		Plus,
		Minus,
		Asterisk,
		Slash,
		Backslash,
		Equal,
		NotEqual,
		
		Comma,
		Colon,
		Semicolon,
		SingleQuote,
		DoubleQuote,
		
		String,
		Comment,
		
		Unexpected,
		End,
		LineBreak,
		Start,
		Type,
		
		Constant,
		NewType,
		NewClass,
		NewObject,
		Modificator,
	};
	
	Token(Kind kind): m_kind(kind) {}
	
	bool is(Kind kind) { return m_kind == kind; } 
	void setKind(Kind kind) { m_kind = kind; }
	Kind kind() { return m_kind; }
	
	const char* get() { return m_lexem; }
	void set(char* lexem) { m_lexem = lexem; }
	
	
private:
	Kind		 m_kind;
	const char*	 m_lexem = nullptr;
	
};

#endif