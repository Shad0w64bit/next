#include <windows.h>
#include <stdio.h>
#include <shellapi.h>

#include <map>
#include <vector>
#include <string>
#include <shellapi.h>

class Argument
{
public:
	Argument(char* key) {
		int sz = strlen(key) + 1;
		m_key = (char*)malloc(sz);
		memcpy(m_key, key, sz);
	}
	
	~Argument() {
		if (m_key != nullptr)
			free(m_key);
		for(auto it = m_values.begin(); it != m_values.end(); ++it)
		{
			free(*it);
		}
		m_values.clear();
	}
	
	void addValue(char* val)
	{	
		int sz = strlen(val) + 1;
		char* l_val = (char*)malloc(sz);
		memcpy(l_val, val, sz);
		
		m_values.insert(m_values.end(), l_val);
	}
	
	const char* getValue()
	{
		auto it = m_values.begin();
		return (*it);
	}
	
	bool isKey(const char* key) {
		return (strcmp(key, m_key) == 0);
	}
	
private:
	char* m_key = nullptr;
	std::vector<char*> m_values;
};

class Arguments
{
public:
	Arguments(bool init = true);
	
	bool is(const char* key);
	const char* get(const char* key);

private:
	std::vector<Argument*> m_args;
};

bool Arguments::is(const char* key)
{
	for (auto it=m_args.begin(); it != m_args.end(); ++it)
	{
		if ((*it)->isKey(key))
			return true;
	}
	return false;
}

const char* Arguments::get(const char* key)
{
	for (auto it=m_args.begin(); it != m_args.end(); ++it)
	{
		if ((*it)->isKey(key))
			return (*it)->getValue();
	}
	return "";
}

Arguments::Arguments(bool init)
{
	if (!init) return;
//	std::map<std::string; std::string> args;
	
	int argc;
	LPWSTR* lpArgv = CommandLineToArgvW( GetCommandLineW(), &argc );

	auto argv = (char**)malloc( argc*sizeof(char*) );
	int size, i = 0;
	for( ; i < argc; ++i )
	{
		size = wcslen( lpArgv[i] ) + 1;
		argv[i] = (char*)malloc( size );
		wcstombs( argv[i], lpArgv[i], size );
	}
	
	Argument* last_arg = nullptr;
	for (i=0; i<argc; ++i)
	{	
		if (argv[i][0] == '-')
		{
			last_arg = new Argument(argv[i]);
			m_args.insert(m_args.end(), last_arg);
		} else {
			if (last_arg != nullptr) {
				last_arg->addValue(argv[i]);
			}
		}
	}
	
	i = 0;
	for( ; i < argc; ++i )
	free( argv[i] );
	free( argv );	
	
	LocalFree( lpArgv );	
}