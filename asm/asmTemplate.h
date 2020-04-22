#ifndef __ASM_TEMPLATE_H__
#define __ASM_TEMPLATE_H__

class AsmTemplate
{
public:
	AsmTemplate() {}
	virtual ~AsmTemplate() {}
	
	virtual int reserve() { return 0; }
	
protected:
	
};

#endif