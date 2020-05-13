#ifndef __TYPEDEF_H__
#define __TYPEDEF_H__

enum IntType {
	i32,
	u32,
	i16,
	u16,
	i8,
	u8,
	
	Char,
	
	Class,
	Object,
	External, // Pointer
	
	ConstString,
	ConstInt,
	
	Unknown,
};

struct Type {
	IntType type;
	char* name;
	int size;
	Type* ptr;
};

class Types
{
public:
	Types()
	{
		Type* t;
		t = createType("i32", IntType::i32, 4);				m_types.push_back( t );
		t = createType("^i32", IntType::External, 4, t);	m_types.push_back( t );
		
		t = createType("u32", IntType::u32, 4);				m_types.push_back( t );
		t = createType("^u32", IntType::External, 4, t);	m_types.push_back( t );
		
		t = createType("char", IntType::Char, 1);			m_types.push_back( t );
		t = createType("^char", IntType::External, 4, t);	m_types.push_back( t );
		
		t = createType("~ConstInt", IntType::ConstInt, 4);			m_types.push_back( t );
		t = createType("~ConstString", IntType::ConstString, -1);	m_types.push_back( t );
	}
	
	Type* getTypeByName(const char* name)
	{
		for (auto it=m_types.begin(); it!=m_types.end(); ++it)
		{
			if ( strcmp((*it)->name, name) == 0 ) {
				return (*it);
			}
		}
		return nullptr;
	}

private:
	Type* createType(const char* name, IntType type, int sz, Type* ptr = nullptr)
	{
		auto t = new Type{type, (char*)malloc( strlen(name)+1 ), sz, ptr}; 
		strcpy(t->name, name);
		return t;
	}

	std::vector<Type*> m_types;
	
};

#endif