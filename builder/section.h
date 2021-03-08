#ifndef __SECTION_H__
#define __SECTION_H__

#include "../platform/windows/include.h"

#define ALIGN_DOWN(x, align)  (x & ~(align-1))
#define ALIGN_UP(x, align)    ((x & (align-1))?ALIGN_DOWN(x,align)+align:x)
// typedef unsigned long long QWORD;

class Section
{
public:

	Section(const char* name)
	{
		int sz = strlen(name)+1;
		m_name = (char*) malloc(sz);
		strcpy(m_name, name);

		memset((char*) &m_header, 0, sizeof(IMAGE_SECTION_HEADER));
		strcpy((char*) &m_header.Name, name);
		m_rawSize = 0;
		m_vaSize = 0;
	}

	virtual ~Section ()
	{
		free(m_name);
	}

	const char* name() { return m_name; }
	IMAGE_SECTION_HEADER* header() { return &m_header; }

	void beginUpdate() { m_update = true; }
	void endUpdate()
	{
		if (m_update)
		{
			m_update = false;
			prepare();
			recalc();
		}
	}

	int vaSize() { return m_vaSize; }
	int rawSize() { return m_rawSize; }
	void setImageBase(uint64_t base) { m_imageBase = base; }
//	void setVA(int VA) { m_va = VA; }
//	int va() { return m_va; }

	virtual void prepare() {}
	virtual void recalc() {}
	virtual void write(std::ofstream& f, int align)
	{
		if (m_rawSize == 0)
			return;

		char* tmp = (char*) malloc(align);
		memset(tmp, 0, align);
		f.write(tmp, align);
	}

//	virtual int dataDirectory() { return -1; }

	virtual void preLoad(IMAGE_NT_HEADERS64* header)
	{
		header->Signature = header->Signature;
	}

protected:
	bool m_update;
	int m_rawSize;
	int m_vaSize;
	char* m_name;
	uint64_t m_imageBase;
//	int m_va;
	IMAGE_SECTION_HEADER m_header;
};


#endif
