#ifndef __LINKER_H__
#define __LINKER_H__

#ifdef __unix__
#include <stdint.h>
#endif

#include <ctime>
#include <vector>

#include "../builder/builder.h"
#include "../builder/codeSection.h"
#include "../builder/importSection.h"
#include "../builder/dataSection.h"
#include "../builder/bssSection.h"
#include "../asm/asmType.h"

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
		m_env = Builder::AppEnvironment::Console;
	}
	
	~Linker()
	{
		free(m_main);
		free(m_namespace);
	}
	
	void build(const char* fn)
	{
		Builder builder (Builder::AppType::App64);
		
		builder.setEnvironment(m_env);
		
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
		
		m_releaseFunctions.push_back(mf); // Main Function
		
		FuncToAsm();
		
		// End Build Code
		
		bs->endUpdate();
		ds->endUpdate();
		is->endUpdate();
		cs->endUpdate();
		
		builder.addSection(cs);
		builder.addSection(ds);
		builder.addSection(bs);
		builder.addSection(is);
		
		builder.build(fn);		
	}
	
	void FuncToAsm()
	{
//		CodeblockToAsm(func->getCodeblock());	
		
		while (m_releaseFunctions.size() > 0)
		{
			Function* func = m_releaseFunctions.back();			
			m_releaseFunctions.pop_back();
//				std::cout << func->name() << std::endl;
						
			if (func->address() == 0)
			{
				auto off = m_cs->curVaOffset();
				CodeblockToAsm(func->getCodeblock());
				func->setOffset(off);
			}
		}
		
/*		auto it = m_releaseFunctions.begin();
		while (it != m_releaseFunctions.end())
		{
			if ((*it)->address() == 0)
			{
				auto off = m_cs->curVaOffset();
				CodeblockToAsm((*it)->getCodeblock());
				//std::cout << "Address: " << off << std::endl;
				(*it)->setOffset(off);
				m_releaseFunctions.erase(it);
				continue;
			}
			
			it++;
		}*/
	}
	
	void CodeblockToAsm(Codeblock* cb)
	{
		auto ops = cb->getOperations();

		int localVar = 0;
		auto vars = cb->getVariables();
		for (auto it=vars->begin(); it != vars->end(); ++it)
		{
			localVar += (*it)->size();				
		}
		
		localVar = ALIGN_UP(localVar, 8);
		
		auto funcStack = localVar + 
			(((cb->funcArgSize() > 0) && (cb->funcArgSize()<4)) ? 4*8 : cb->funcArgSize()*8); // Arguments Function Stack 
				
//		std::cout << "funcArgSize: " << cb->funcArgSize() << std::endl;
		if (funcStack > 0) {
			m_cs->add( new AsmCommand(ASM_CMD::PUSH, new Operand{AsmType::Register, 8, (uint64_t) RegEnum::RBP} ) );
			m_cs->add( new AsmCommand(ASM_CMD::MOV, new Operand{AsmType::Register, 8, (uint64_t) RegEnum::RBP}, new Operand{AsmType::Register, 8, (uint64_t) RegEnum::RSP} ) );			
			m_cs->add( new AsmCommand(ASM_CMD::SUB, new Operand{AsmType::Register, 8, (uint64_t) RegEnum::RSP}, new Operand{AsmType::Constant, 1, (uint64_t) funcStack } ) ); 
		}
		
		{ // Define Variables
			int offset = 0;
			for (auto it=vars->begin(); it != vars->end(); ++it)
			{
				Variable* var = (*it);
//				printf("Var %s size %d offset %d\n", var->name(), var->size(), offset);
				var->setOffset(offset*-1);				
				offset += ALIGN_UP(var->size(), 8);
			}
			
			for (auto it=vars->begin(); it != vars->end(); ++it)
			{
				Variable* var = (*it);
				if (var->isInit()) {
					Operand* op3;
					
					if ((var->type()->type == IntType::u32) || (var->type()->type == IntType::i32))
						//int* data = (int*)var->data();
						op3 = new Operand{ AsmType::Constant, 4, (uint64_t) (*( (int*)var->data() )) };
					
					m_cs->add( 
						new AsmCommand(
							ASM_CMD::PMOV, 
							new Operand{AsmType::Register, 8, (uint64_t) RegEnum::RBP}, 
							new Operand{AsmType::Offset, 4, (uint64_t) var->offset()},
							op3
						)
					);
				}				
			}
		}
		
		for (auto it=ops->begin(); it!=ops->end(); ++it)
		{
			if ((*it)->type() == Operation::Type::CallFunction) {
				auto op = (OpCallFunction*)(*it);
				Function* func = op->getFunction();
				
/*				printf("Linker func %s %s\n", 
					func->name(), 
					(func->isImport()) ? "Import" : "Local"
				);*/
//				std::cout << "Linker func: " << func->name() << ((func->isImport()) ? " Import" : " Local") << std::endl;
				
				auto args = op->getArguments();
				
//				int paramSize = 0;
				if (args->size() > 0) {
//					paramSize = (args->size() > 4) ? args->size() : 4;
//					m_cs->add( new AsmCommand(ASM_CMD::SUB, new Operand{AsmType::Register, 4, (uint64_t) RegEnum::RSP}, new Operand{AsmType::Constant, 1, (uint64_t)paramSize*8 } ) );
					
					int c = 0;
					auto arg=args->begin();
					for (; (arg!=args->end() && c < 4); ++arg)
					{
//						printf("Var!!! %s", (*arg)->name());
						
						Operand* op1 = nullptr;
						Operand* op2 = nullptr;
						Operand* op3 = nullptr;
						switch(c) {
							case 0: op1 = new Operand{AsmType::Register, (*arg)->size(), (uint64_t) ((*arg)->size() == 1) ? RegEnum::CL : (((*arg)->size() == 2) ? RegEnum::CX : (((*arg)->size() == 4) ? RegEnum::ECX : RegEnum::RCX) )}; break;
							case 1: op1 = new Operand{AsmType::Register, (*arg)->size(), (uint64_t) ((*arg)->size() == 1) ? RegEnum::DL : (((*arg)->size() == 2) ? RegEnum::DX : (((*arg)->size() == 4) ? RegEnum::EDX : RegEnum::RDX) )}; break;
							case 2: op1 = new Operand{AsmType::Register, (*arg)->size(), (uint64_t) ((*arg)->size() == 1) ? RegEnum::R8L : (((*arg)->size() == 2) ? RegEnum::R8W : (((*arg)->size() == 4) ? RegEnum::R8D : RegEnum::R8) )}; break;
							case 3: op1 = new Operand{AsmType::Register, (*arg)->size(), (uint64_t) ((*arg)->size() == 1) ? RegEnum::R9L : (((*arg)->size() == 2) ? RegEnum::R9W : (((*arg)->size() == 4) ? RegEnum::R9D : RegEnum::R9) )}; break;
						};
						
						
						if ((*arg)->name() == nullptr) {
							if ((*arg)->type()->type == IntType::ConstInt) {
								uint64_t dat = (*((*arg)->data()));
								op2 = new Operand{AsmType::Constant, 4, dat};
								// std::cout << "Input: " << dat << std::endl;
								m_cs->add( new AsmCommand(ASM_CMD::MOV, op1, op2) );
							} else if ((*arg)->type()->type == IntType::ConstString) {								
								auto addr = m_ds->add(Data::Type::ConstString, (*arg)->data());
								op2 = new Operand{AsmType::RData, 4, (uint64_t) addr};
								//std::cout << (*arg)->data() << std::endl;
								m_cs->add( new AsmCommand(ASM_CMD::MOV, op1, op2) );
							}
							
							
						} else if (((*arg)->type()->type == IntType::i32) || ((*arg)->type()->type == IntType::u32)) {
							
							//printf("var %s is %s\n", (*arg)->name(), (((*arg)->type()->type == IntType::i32) || ((*arg)->type()->type == IntType::u32)) ? "true" : "false" );
							
							
							op2 = new Operand{AsmType::Register, 8, (uint64_t) RegEnum::RBP};							
							op3 = new Operand{AsmType::Offset, 4, (uint64_t) (*arg)->offset()};
							m_cs->add( new AsmCommand(ASM_CMD::PMOV, op1, op2, op3) );
						}						
						
													
						/*if (((*arg)->type()->type == IntType::i32) || ((*arg)->type()->type == IntType::u32))
						{
							op2 = new Operand{AsmType::Register, 4, (uint64_t) RegEnum::RSP};							
							Variable* var = (Variable*)((*arg)->data);
							op3 = new Operand{AsmType::Constant, 4, (uint64_t) var->offset()};
							
							m_cs->add( new AsmCommand(ASM_CMD::PMOV, op1, op2, op3) );
						}*/
						
						
						c++;
					}
					
					int offset = 4*8;
					while (arg != args->end())
					{
						Operand* op3 = nullptr;
						if ((*arg)->name() == nullptr) {
							if ((*arg)->type()->type == IntType::ConstInt) {
								uint64_t dat = (*((*arg)->data()));
								op3 = new Operand{AsmType::Constant, 4, dat};
					    		//std::cout << "Dop Input: " << dat << std::endl;
							} else if ((*arg)->type()->type == IntType::ConstString) {								
								auto addr = m_ds->add(Data::Type::ConstString, (*arg)->data());
								op3 = new Operand{AsmType::RData, 4, (uint64_t) addr};
								//std::cout << (*arg)->data() << std::endl;
							}
							
//							op3 = new Operand{AsmType::Constant, 4, (uint64_t) (*arg)->offset()};
							
							m_cs->add( new AsmCommand(ASM_CMD::PMOV, 
								new Operand{AsmType::Register, 8, (uint64_t) RegEnum::RSP}, 
								new Operand{AsmType::Offset, 4, (uint64_t) offset},
								op3));
							
							offset += 8;
							arg++;
							continue;
							
						} else if (((*arg)->type()->type == IntType::i32) || ((*arg)->type()->type == IntType::u32)) {
							
							// mov eax, [rbp+offset]
							m_cs->add( new AsmCommand(ASM_CMD::PMOV, 
								new Operand{AsmType::Register, 4, (uint64_t) RegEnum::EAX}, 
								new Operand{AsmType::Register, 4, (uint64_t) RegEnum::RBP},
								new Operand{AsmType::Offset, 4, (uint64_t) (*arg)->offset()}
								) );
							
							m_cs->add( new AsmCommand(ASM_CMD::PMOV, 
								new Operand{AsmType::Register, 4, (uint64_t) RegEnum::RSP},
								new Operand{AsmType::Offset, 4, (uint64_t) offset},
								new Operand{AsmType::Register, 4, (uint64_t) RegEnum::EAX}
								) );
							
							offset += 8;
							arg++;
							continue;
						}
						
						
						offset += 8;
						arg++;
					}
				}
				
				if (func->isImport()) {
					m_is->add(func->getLibrary(), func->name(true));
					auto lfunc = m_is->getFunc(func->getLibrary(), func->name(true));
					
					m_cs->add( new AsmCommand(ASM_CMD::CALLPTR, new Operand{AsmType::Library, 4, (uint64_t) lfunc}) );
				} else {
/*					if (func->address() != 0) {
						m_cs->add( new AsmCommand(ASM_CMD::CALL, new Operand{AsmType::Pointer, 8, (uint64_t) func->address()}) );
					} else {
					}*/
					m_releaseFunctions.push_back( func );
					m_cs->add( new AsmCommand(ASM_CMD::CALL, new Operand{AsmType::Function, 4, (uint64_t) func}) );
				}
				
//				if (args->size() > 0) {
//					m_cs->add( new AsmCommand(ASM_CMD::ADD, new Operand{AsmType::Register, 4, (uint64_t) RegEnum::RSP}, new Operand{AsmType::Constant, 1, (uint64_t)paramSize*8 } ) );
//				}
				//std::cout << "CallFunction" << std::endl;
			}
		}
		
		if (funcStack > 0) {
			m_cs->add( new AsmCommand(ASM_CMD::ADD, new Operand{AsmType::Register, 8, (uint64_t) RegEnum::RSP}, new Operand{AsmType::Constant, 1, (uint64_t) funcStack } ) );
			m_cs->add( new AsmCommand(ASM_CMD::POP, new Operand{AsmType::Register, 8, (uint64_t) RegEnum::RBP} ) );
		}
		m_cs->add( new AsmCommand(ASM_CMD::RET) );
	}
	
	void setEnvironment(Builder::AppEnvironment env)
	{
		m_env = env;		
	}

private:
	Parser* m_parser;
	char* m_main;
	char* m_namespace;
	
	Builder::AppEnvironment m_env;
	
	CodeSection* m_cs;
	ImportSection* m_is;
	DataSection* m_ds;
	BssSection* m_bs;
	
	std::vector<Function*> m_releaseFunctions;
};

#endif
