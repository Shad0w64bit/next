#ifdef __unix__

#elif _WIN32
#include <windows.h>
#include <shellapi.h>
#endif
#include <stdio.h>
#include <cstring>
#include <map>
#include <vector>
#include <string>


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
    if (m_values.empty())
      return nullptr;
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
	Arguments(int argc, char* argv[]);

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

Arguments::Arguments(int argc, char* argv[])
{
	Argument* last_arg = nullptr;
	for (int i=0; i<argc; ++i)
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
}

