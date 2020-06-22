#include <iostream>
#include <fstream>
#include <windows.h>
	
class Logger
{
public:
	enum Type
	{
		Info,
		Warning,
		Error,
	};
	
	static Logger* getInstance()
	{
		if (m_instance == nullptr)
		{
			m_instance = new Logger();
		}
		return m_instance;
	}
	
	static void start(const char* file, bool clear)
	{
		m_hOut = GetStdHandle(STD_OUTPUT_HANDLE);
		m_file.open(file, std::ios::out | std::ios::binary | ((clear) ? std::ios::trunc : std::ios::app));
	}
	
	static void write(Logger::Type t, const char* str, DWORD color = 0);
	
	static void info(const char* str)
	{
		write(Logger::Info, str, FOREGROUND_BLUE);
	}
	
	static void warning(const char* str)
	{
		write(Logger::Warning, str, FOREGROUND_GREEN | FOREGROUND_RED);
	}
	
	static void error(const char* str)
	{
		write(Logger::Error, str, FOREGROUND_RED);
	}
	
private:
	static Logger* m_instance;
	static std::ofstream m_file;
	static HANDLE m_hOut;
	Logger() {}
	~Logger() {
		m_file.close();
	}
};

std::ofstream Logger::m_file;
HANDLE Logger::m_hOut;

std::ostream& operator<<(std::ostream& os, const Logger::Type& type) {
	static const char* const names[]{
		"I",
		"W",
		"E",
	};
	return os << names[static_cast<int>(type)];
}

void Logger::write(Logger::Type t, const char* str, DWORD color)
{
	SetConsoleTextAttribute(m_hOut, color | FOREGROUND_INTENSITY);
	if (m_file.is_open())
	{
		m_file << t << "\t" << str << std::endl;
	}
	std::cerr << t << "\t" << str << std::endl;
	SetConsoleTextAttribute(m_hOut, FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_RED);
}
