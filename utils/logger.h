#include <iostream>
#include <fstream>

#ifdef __unix__
#include <stdint.h>
#elif _WIN32
#include <windows.h>
#endif

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
		#ifdef __WIN32
    m_hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    #endif
    m_file.open(file, std::ios::out | std::ios::binary | ((clear) ? std::ios::trunc : std::ios::app));
  }

  #ifdef __unix__
  static void write_unix(Logger::Type t, const char* str);
  #elif __WIN32
  static void write_windows(Logger::Type t, const char* str, uint32_t color);
  #endif

  static void info(const char* str)
  {
    #ifdef __unix__
    write_unix(Logger::Info, str);
    #elif __WIN32
    write_windows(Logger::Info, str, FOREGROUND_BLUE);
    #endif
  }

  static void warning(const char* str)
  {
    #ifdef __unix__
    write_unix(Logger::Warning, str);
    #elif __WIN32
    write_windows(Logger::Warning, str, FOREGROUND_GREEN | FOREGROUND_RED);
    #endif
  }

  static void error(const char* str)
  {
    #ifdef __unix__
    write_unix(Logger::Error, str);
    #elif __WIN32
    write_windows(Logger::Error, str, FOREGROUND_RED);
    #endif
  }


private:
	static Logger* m_instance;
	static std::ofstream m_file;
  #ifdef __WIN32
	static HANDLE m_hOut;
  #endif

	Logger() {}
	~Logger() {
		m_file.close();
	}
};

std::ofstream Logger::m_file;
#ifdef __WIN32
HANDLE Logger::m_hOut;
#endif

std::ostream& operator<<(std::ostream& os, const Logger::Type& type) {
	static const char* const names[]{
		"I",
		"W",
		"E",
	};
	return os << names[static_cast<int>(type)];
}


#ifdef __unix__

void Logger::write_unix(Logger::Type t, const char* str)
{
	if (m_file.is_open())
	{
		m_file << t << "\t" << str << std::endl;
	}
  const char* color;
  switch (t)
  {
  	case Info:		color = "\033[34m"; break;
  	case Warning:	color = "\033[33m"; break;
  	case Error:		color = "\033[31m"; break;
  	default:			color = "\033[0m";
  }
	std::cerr << color << t << "\t" << str << "\033[0m" << std::endl;
}

#elif __WIN32

void Logger::write_windows(Logger::Type t, const char* str, uint32_t color)
{
	if (m_file.is_open())
	{
		m_file << t << "\t" << str << std::endl;
	}
	SetConsoleTextAttribute(m_hOut, color | FOREGROUND_INTENSITY);
	std::cerr << t << "\t" << str << std::endl;
	SetConsoleTextAttribute(m_hOut, FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_RED);
}

#endif
