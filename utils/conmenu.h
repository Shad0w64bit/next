#include <iostream>
#include <fstream>
#include <windows.h>
#include <conio.h>

#define KB_UP 72
#define KB_DOWN 80
#define KB_ENTER 13

class ConMenu
{
public:
	ConMenu()
	{
		m_hOut = GetStdHandle(STD_OUTPUT_HANDLE);
	}
	
	~ConMenu()
	{
		for (auto it=m_items.begin(); it!=m_items.end(); ++it)
		{
			free(*it);
		}
	}
	
	int show(unsigned int index = 0)
	{
		if (m_items.size() <= 0)
			return -1;
		
		unsigned int sel = (m_items.size() > index) ? index : 0;
		bool changed = true;
		while (true)
		{
			if (kbhit())
			{
				int KB_code = getch();
				int newSel;
			
				switch(KB_code)
				{
				case KB_UP:
					newSel = sel-1;
					if ((newSel >= 0) && (newSel < (int)m_items.size()))
						sel = newSel;
					break;
				case KB_DOWN:
					newSel = sel+1;
					if ((newSel >= 0) && (newSel < (int)m_items.size()))
						sel = newSel;
					break;
				case KB_ENTER:
					return sel;
				}
				changed = true;
			}
			
			if (changed)
			{
				system("cls");
				unsigned int i=0;
				for (auto it=m_items.begin(); it!=m_items.end(); ++it)
				{
					if (i == sel) 
						SetConsoleTextAttribute(m_hOut, BACKGROUND_BLUE | BACKGROUND_GREEN | BACKGROUND_RED);

					std::cout << (*it) << std::endl;

					if (i == sel) 
						SetConsoleTextAttribute(m_hOut, FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_RED);
					i++;
				}
				changed = false;
			}
			
		}
		
	}
	
	void add(char* str)
	{
		int sz = strlen(str) + 1;
		if (sz <= 0) return;
		char* s = (char*)malloc(sz);
		strncpy(s,str,sz);
		m_items.push_back(s);
	}
	
private:
	HANDLE	m_hOut;
	std::vector<char*> m_items;
};