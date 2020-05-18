#ifndef __LINKER_H__
#define __LINKER_H__

#include <ctime>

#include "../builder/builder.h"
#include "../builder/codeSection.h"
#include "../builder/importSection.h"
#include "../builder/dataSection.h"
#include "../builder/bssSection.h"

class Linker
{
public:
	Linker(Parser* pars, const char* ns, const char* mainFunc)
	{
		int sz = strlen(ns)+1;
		m_namespace = (char*)malloc(sz);
		memcpy(m_namespace, ns, sz);
		
		sz = strlen(mainFunc)+1;
		m_main = (char*)malloc(sz);
		memcpy(m_main, mainFunc, sz);
		
		m_parser = pars;
	}
	
	~Linker()
	{
		free(m_main);
		free(m_namespace);
	}
	
	void build(const char* fn)
	{
		Builder builder (Builder::AppType::App64);
		
		CodeSection* cs = new CodeSection(".text");
		ImportSection* is = new ImportSection(".idata");	
		DataSection* ds = new DataSection(".data");
		BssSection* bs = new BssSection(".bss");

		m_cs = cs;
		m_is = is;
		m_ds = ds;
		m_bs = bs;
		
		cs->beginUpdate();
		is->beginUpdate();	
		ds->beginUpdate();
		bs->beginUpdate();		
		
		// Build Code 
		
		Namespace* mns = m_parser->getNamespace(m_namespace);
		Function* mf = mns->getFunction(m_main);
		
		FuncToAsm(mf);
		
		// End Build Code
		
		bs->endUpdate();
		ds->endUpdate();
		is->endUpdate();
		cs->endUpdate();
		
		builder.addSection(cs);
		builder.addSection(ds);
		if (bs->vaSize() > 0)
			builder.addSection(bs);
		builder.addSection(is);
		
		builder.build(fn);		
	}
	
	void FuncToAsm(Function* func)
	{
		CodeblockToAsm(func->getCodeblock());	
	}
	
	void CodeblockToAsm(Codeblock* cb)
	{
		auto ops = cb->getOperations();

//		std::cout << "funcArgSize: " << cb->funcArgSize() << std::endl;
		if (cb->funcArgSize() > 0) {
			m_cs->add( new AsmCommand(ASM_CMD::PUSH, new Operand{AsmType::Register, 8, (QWORD) RegEnum::RBP} ) );
			m_cs->add( new AsmCommand(ASM_CMD::MOV, new Operand{AsmType::Register, 8, (QWORD) RegEnum::RBP}, new Operand{AsmType::Register, 8, (QWORD) RegEnum::RSP} ) );
			m_cs->add( new AsmCommand(ASM_CMD::SUB, new Operand{AsmType::Register, 8, (QWORD) RegEnum::RSP}, new Operand{AsmType::Constant, 1, (QWORD) (((cb->funcArgSize() > 0) && (cb->funcArgSize()<4)) ? 4*8 : cb->funcArgSize()*8) } ) );
		}
		
		for (auto it=ops->begin(); it!=ops->end(); ++it)
		{
			if ((*it)->type() == Operation::Type::CallFunction) {
				auto op = (OpCallFunction*)(*it);
				Function* func = op->getFunction();
				auto args = op->getArguments();
				
//				int paramSize = 0;
				if (args->size() > 0) {
//					paramSize = (args->size() > 4) ? args->size() : 4;
//					m_cs->add( new AsmCommand(ASM_CMD::SUB, new Operand{AsmType::Register, 4, (QWORD) RegEnum::RSP}, new Operand{AsmType::Constant, 1, (QWORD)paramSize*8 } ) );
					
					int c = 0;
					auto arg=args->begin();
					for (; (arg!=args->end() && c < 4); ++arg)
					{
						
						Operand* op1 = nullptr;
						Operand* op2 = nullptr;
						switch(c) {
							case 0: op1 = new Operand{AsmType::Register, 4, (QWORD) RegEnum::RCX}; break;
							case 1: op1 = new Operand{AsmType::Register, 4, (QWORD) RegEnum::RDX}; break;
							case 2: op1 = new Operand{AsmType::Register, 4, (QWORD) RegEnum::R8}; break;
							case 3: op1 = new Operand{AsmType::Register, 4, (QWORD) RegEnum::R9}; break;
						};
						
						if ((*arg)->name() == nullptr) {
							if ((*arg)->type()->type == IntType::ConstInt) {
								QWORD dat = (*((*arg)->data()));
								op2 = new Operand{AsmType::Constant, 4, dat};
								// std::cout << "Input: " << dat << std::endl;
							} else if ((*arg)->type()->type == IntType::ConstString) {								
								auto addr = m_ds->add(Data::Type::ConstString, (*arg)->data());
								op2 = new Operand{AsmType::RData, 4, (QWORD) addr};
								//std::cout << (*arg)->data() << std::endl;
							}
							
						}
						
						m_cs->add( new AsmCommand(ASM_CMD::MOV, op1, op2) );
						
						c++;
					}
					
					int offset = 4*8;
					while (arg != args->end())
					{
						Operand* op3 = nullptr;
						if ((*arg)->name() == nullptr) {
							if ((*arg)->type()->type == IntType::ConstInt) {
								QWORD dat = (*((*arg)->data()));
								op3 = new Operand{AsmType::Constant, 4, dat};
					    		//std::cout << "Dop Input: " << dat << std::endl;
							} else if ((*arg)->type()->type == IntType::ConstString) {								
								auto addr = m_ds->add(Data::Type::ConstString, (*arg)->data());
								op3 = new Operand{AsmType::RData, 4, (QWORD) addr};
								//std::cout << (*arg)->data() << std::endl;
							}
						}
						
						m_cs->add( new AsmCommand(ASM_CMD::PMOV, 
							new Operand{AsmType::Register, 8, (QWORD) RegEnum::RSP}, 
							new Operand{AsmType::Pointer, 4, (QWORD) offset},
							op3)
						);
						
						offset += 8;
						arg++;
					}
				}
				
				if (func->isImport()) {
					m_is->add(func->getLibrary(), func->name(true));
					auto lfunc = m_is->getFunc(func->getLibrary(), func->name(true));
					
					m_cs->add( new AsmCommand(ASM_CMD::CALL, new Operand{AsmType::Library, 4, (QWORD) lfunc}) );					
				}
				
//				if (args->size() > 0) {
//					m_cs->add( new AsmCommand(ASM_CMD::ADD, new Operand{AsmType::Register, 4, (QWORD) RegEnum::RSP}, new Operand{AsmType::Constant, 1, (QWORD)paramSize*8 } ) );
//				}
				//std::cout << "CallFunction" << std::endl;
			}
		}
		
		if (cb->funcArgSize() > 0) {
			m_cs->add( new AsmCommand(ASM_CMD::ADD, new Operand{AsmType::Register, 8, (QWORD) RegEnum::RSP}, new Operand{AsmType::Constant, 1, (QWORD) (((cb->funcArgSize() > 0) && (cb->funcArgSize()<4)) ? 4*8 : cb->funcArgSize()*8) } ) );
			m_cs->add( new AsmCommand(ASM_CMD::POP, new Operand{AsmType::Register, 8, (QWORD) RegEnum::RBP} ) );
		}
		
	}

private:
	Parser* m_parser;
	char* m_main;
	char* m_namespace;
	
	CodeSection* m_cs;
	ImportSection* m_is;
	DataSection* m_ds;
	BssSection* m_bs;
		
};

#endif