#include <iostream>
#include <fstream>
#include <windows.h>
#include <ctime>

//#include "import64.h"
#include "builder64\builder.h"

#include "builder64\importSection.h"
#include "builder64\bssSection.h"
#include "builder64\dataSection.h"
#include "builder64\codeSection.h"
#include "asm\asmType.h"
//#include "builder64\command.h"

int main()
{
	Builder builder (Builder::AppType::App64);
	
	CodeSection* cs = new CodeSection(".text");
	ImportSection* is = new ImportSection(".idata");	
	DataSection* ds = new DataSection(".data");
	BssSection* bs = new BssSection(".bss");
	
	cs->beginUpdate();
	is->beginUpdate();	
	ds->beginUpdate();
	bs->beginUpdate();
	
	builder.addSection(cs);
	builder.addSection(ds);
	builder.addSection(bs);
	builder.addSection(is);
	
	Variable* flags = bs->add(Variable::Type::i32, "flags");
	
	auto caption = ds->add(Data::Type::ConstString, "caption", "Win32 App");
	auto msg = ds->add(Data::Type::ConstString, "message", "Hello world!");
	
	is->add("KERNEL32.DLL", "ExitProcess");
	is->add("USER32.DLL", "MessageBoxA");	
	
	auto exit = is->getFunc("KERNEL32.DLL", "ExitProcess");
	auto msgBox = is->getFunc("USER32.DLL", "MessageBoxA");
	
	cs->add( new Command(ASM_CMD::MOV, new Operand{Type::Bss, 4, (QWORD)flags}, new Operand{Type::Constant, 4, 0x40 } ) );
	cs->add( new Command(ASM_CMD::SUB, new Operand{Type::Register, 4, (QWORD)RegEnum::RSP}, new Operand{Type::Constant, 1, 0x28 } ) );
	cs->add( new Command(ASM_CMD::MOV, new Operand{Type::Register, 4, (QWORD) RegEnum::RCX}, new Operand{Type::Constant, 4, 0 }) );
	cs->add( new Command(ASM_CMD::MOV, new Operand{Type::Register, 4, (QWORD) RegEnum::RDX}, new Operand{Type::RData, 4, (QWORD) msg}) );
	cs->add( new Command(ASM_CMD::MOV, new Operand{Type::Register, 4, (QWORD) RegEnum::R8}, new Operand{Type::RData, 4, (QWORD) caption}) );
	cs->add( new Command(ASM_CMD::MOV, new Operand{Type::Register, 4, (QWORD) RegEnum::R9}, new Operand{Type::Bss, 4, (QWORD) flags}) );
	cs->add( new Command(ASM_CMD::CALL, new Operand{Type::Library, 4, (QWORD) msgBox}) );
	cs->add( new Command(ASM_CMD::MOV, new Operand{Type::Register, 4, (QWORD) RegEnum::RCX}, new Operand{Type::Constant, 4, 0x0 }) );
	cs->add( new Command(ASM_CMD::CALL, new Operand{Type::Library, 4, (QWORD) exit}) );
	cs->add( new Command(ASM_CMD::ADD, new Operand{Type::Register, 4, (QWORD)RegEnum::RSP}, new Operand{Type::Constant, 1, 0x28 } ) );
	
	//cs->add( new Command(ASM_CMD::NOP, new Operand{Type::Constant, 4, (QWORD) 4}) );
	
	//cs->add( new Command( Command::Cmd::MOV, new Operand{Type::Pointer, 4, (char*)flags}, new Operand{Type::i32, 4, (char*)0x40} ) );
	/*Reg reg;
	std::cout << (reg.get(RegEnum::EAX))->len << std::endl;*/
	
	bs->endUpdate();
	ds->endUpdate();
	is->endUpdate();
	cs->endUpdate();
	
	builder.build("test64builder.exe");
	
	
	
	std::ofstream f("test64.exe", std::ofstream::out);
	
	// DOS header
	IMAGE_DOS_HEADER dosHeader;
	
	memset(&dosHeader, 0, sizeof(IMAGE_DOS_HEADER));
	
	dosHeader.e_magic  = 0x5a4d;	// MZ
	dosHeader.e_cblp = 0x80;
	dosHeader.e_cp = 0x01;
	dosHeader.e_cparhdr = 0x04;
	dosHeader.e_minalloc = 0x10;
	dosHeader.e_maxalloc = 0xFFFF;
	dosHeader.e_sp = 0x0140;
	dosHeader.e_lfarlc = 0x40;
	
	dosHeader.e_lfanew = 0x80;		// Start PE-Header
	
	
	f.write((const char*)&dosHeader, sizeof(IMAGE_DOS_HEADER));
	
	// DOS-Stub
	
	char* dosStub = (char*)malloc(64);
	memset(dosStub, 0, 64);
	const char* stub = "\x0E\x1F\xBA\x0E\x00\xB4\x09\xCD\x21\xB8\x01\x4C\xCD\x21"\
						"This program cannot be run in DOS mode.\x0A\x24";
	memcpy(dosStub, stub, 56);
	
	f.write(dosStub, 63);
	
	free(dosStub);
	
	// PE header
	IMAGE_NT_HEADERS64 peHeader;
	memset(&peHeader, 0, sizeof(IMAGE_NT_HEADERS64));
	
	peHeader.Signature = 0x4550;	// PE
	peHeader.FileHeader.Machine = IMAGE_FILE_MACHINE_AMD64;
	peHeader.FileHeader.TimeDateStamp = std::time(nullptr);
	peHeader.FileHeader.NumberOfSections = 4;
	peHeader.FileHeader.SizeOfOptionalHeader = sizeof(IMAGE_OPTIONAL_HEADER64);
	peHeader.FileHeader.Characteristics = ( IMAGE_FILE_RELOCS_STRIPPED |
											IMAGE_FILE_EXECUTABLE_IMAGE | // Executable file
											IMAGE_FILE_LINE_NUMS_STRIPPED |
											IMAGE_FILE_LOCAL_SYMS_STRIPPED |
											IMAGE_FILE_LARGE_ADDRESS_AWARE); // 64-bit machine

	peHeader.OptionalHeader.Magic = 0x020b; //64-bit optional header
	peHeader.OptionalHeader.AddressOfEntryPoint = 0x1000;
	peHeader.OptionalHeader.BaseOfCode = 0x1000;
//	peHeader.OptionalHeader.BaseOfData = 0x1000;
	peHeader.OptionalHeader.ImageBase = 0x400000;
	peHeader.OptionalHeader.MajorSubsystemVersion = 5;
	peHeader.OptionalHeader.MinorSubsystemVersion = 2;
	peHeader.OptionalHeader.MajorOperatingSystemVersion = 5;
	peHeader.OptionalHeader.MinorOperatingSystemVersion = 2;
	peHeader.OptionalHeader.SectionAlignment = 0x1000;
	peHeader.OptionalHeader.FileAlignment = 0x0200;
	peHeader.OptionalHeader.Subsystem = 2; // 2 - Windows GUI; 3 - Console
	peHeader.OptionalHeader.SizeOfImage = 0x5000; // размер файла (в байтах) в памяти, включая все заголовки. Должен быть кратен SectionAlignment.
	peHeader.OptionalHeader.SizeOfHeaders = 0x400; // размер всех заголовков (DOS, DOS-Stub, PE, Section) выравненный на FileAligment.
	peHeader.OptionalHeader.NumberOfRvaAndSizes = IMAGE_NUMBEROF_DIRECTORY_ENTRIES; // 16
//	peHeader.OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT].VirtualAddress = 0;
//	peHeader.OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT].Size = 0;

	peHeader.OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT].VirtualAddress = 0x4000;
	peHeader.OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT].Size = 0x200;
	
	peHeader.OptionalHeader.SizeOfStackReserve = 0x1000;
	peHeader.OptionalHeader.SizeOfStackCommit = 0x1000;
	peHeader.OptionalHeader.SizeOfHeapReserve = 0x10000;
	peHeader.OptionalHeader.SizeOfHeapCommit = 0;
	
	peHeader.OptionalHeader.SizeOfCode = 0x200;
	peHeader.OptionalHeader.SizeOfInitializedData = 0x200;
	peHeader.OptionalHeader.SizeOfUninitializedData = 0x200;
	
	f.write((const char*)&peHeader, sizeof(IMAGE_NT_HEADERS64));
	
	// Write sections
	
	IMAGE_SECTION_HEADER* secText = new IMAGE_SECTION_HEADER;
	memset(secText, 0, sizeof(IMAGE_SECTION_HEADER));
	strcpy((char*)secText->Name, ".text");	
	secText->Misc.VirtualSize = 0x2A;
	secText->VirtualAddress = 0x1000;
	secText->SizeOfRawData = 0x200;
	secText->PointerToRawData = 0x400;
	secText->Characteristics = IMAGE_SCN_CNT_CODE | IMAGE_SCN_MEM_EXECUTE | IMAGE_SCN_MEM_READ ;
	
	IMAGE_SECTION_HEADER* secData = new IMAGE_SECTION_HEADER;
	memset(secData, 0, sizeof(IMAGE_SECTION_HEADER));
	strcpy((char*)secData->Name, ".data");	
	secData->Misc.VirtualSize = 0x24;
	secData->VirtualAddress = 0x2000;
	secData->SizeOfRawData = 0x200;
	secData->PointerToRawData = 0x600;
	secData->Characteristics = IMAGE_SCN_CNT_INITIALIZED_DATA | IMAGE_SCN_MEM_READ | IMAGE_SCN_MEM_WRITE;
	
	IMAGE_SECTION_HEADER* secBss = new IMAGE_SECTION_HEADER;
	memset(secBss, 0, sizeof(IMAGE_SECTION_HEADER));
	strcpy((char*)secBss->Name, ".bss");	
	secBss->Misc.VirtualSize = 0x4;
	secBss->VirtualAddress = 0x3000;
	secBss->SizeOfRawData = 0x0;
	secBss->PointerToRawData = 0x800;
	secBss->Characteristics = IMAGE_SCN_CNT_UNINITIALIZED_DATA | IMAGE_SCN_MEM_READ | IMAGE_SCN_MEM_WRITE;
	
	IMAGE_SECTION_HEADER* secImport = new IMAGE_SECTION_HEADER;
	memset(secImport, 0, sizeof(IMAGE_SECTION_HEADER));
	strcpy((char*)secImport->Name, ".idata");	
	secImport->Misc.VirtualSize = 0x200;
	secImport->VirtualAddress = 0x4000;
	secImport->SizeOfRawData = 0x200;
	secImport->PointerToRawData = 0x800;
	secImport->Characteristics = IMAGE_SCN_CNT_INITIALIZED_DATA | IMAGE_SCN_MEM_READ;
	
	f.write((char*)secText, sizeof(IMAGE_SECTION_HEADER));
	f.write((char*)secData, sizeof(IMAGE_SECTION_HEADER));		
	f.write((char*)secBss, sizeof(IMAGE_SECTION_HEADER));
	f.write((char*)secImport, sizeof(IMAGE_SECTION_HEADER));
	{
		int sz = (0x200+120)-40*peHeader.FileHeader.NumberOfSections;
		char* t = (char*)malloc(sz);
		memset(t, 0, sz);
		f.write(t, sz);
		free(t);
	}
	
	{
		// No magic
		const char* movConstPtr = "\xC7\x05"; // Asm: MOV const to [dword]
		const char* movRcx = "\x48\xC7\xC1";
		const char* movRdx = "\x48\xC7\xC2";
		const char* movR8 = "\x49\xC7\xC0";
		const char* movR9Ptr = "\x44\x8B\x0D";
		const char* subRsp = "\x48\x83\xEC";
		const char* addRsp = "\x48\x83\xC4";
		const char* callPtr = "\xFF\x15";
		
		DWORD flags = secBss->VirtualAddress - secText->VirtualAddress - 0xA;
		DWORD t = 0x40;
		f.write(movConstPtr, 2);
		f.write((char*)&flags, sizeof(flags));
		f.write((char*)&t, sizeof(t));
		
		BYTE b = 0x28;
		f.write(subRsp, 3);
		f.write((char*)&b, sizeof(b));
		
		t = 0;
		f.write(movRcx, 3);
		f.write((char*)&t, sizeof(t));
		
		t = peHeader.OptionalHeader.ImageBase + secData->VirtualAddress + 23;
		f.write(movRdx, 3);
		f.write((char*)&t, sizeof(t));
		
		t -= 23;
		f.write(movR8, 3);
		f.write((char*)&t, sizeof(t));
		
		t =  secBss->VirtualAddress - secText->VirtualAddress - 0x29;
		f.write(movR9Ptr, 3);
		f.write((char*)&t, sizeof(t));

		
		t = secImport->VirtualAddress - secText->VirtualAddress + 0x98 - 0x30;
		f.write(callPtr, 2);
		f.write((char*)&t, sizeof(t));

		t = 0;
		f.write(movRcx, 3);
		f.write((char*)&t, sizeof(t));
		
		t = secImport->VirtualAddress - secText->VirtualAddress + 0x68 - 0x3C;
		f.write(callPtr, 2);
		f.write((char*)&t, sizeof(t));
		
		t = 0x28;
		f.write(addRsp, 3);
		f.write((char*)&t, sizeof(t));		
	}
	
	
	{		
		int sz = 0x200 - 0x44;
		char* t = (char*)malloc(sz);
		memset(t, 0, sz);
		f.write(t, sz);
		free(t);
	}
	
	
	{
		const char* caption = "Win32 assembly program";
		const char* msg = "Hello World!";
		f.write(caption, strlen(caption)+1);
		f.write(msg, strlen(msg)+1);

		int sz = 0x200 - ( strlen(caption)+1 + strlen(msg)+1 );
		char* t = (char*)malloc(sz);
		memset(t, 0, sz);
		f.write(t, sz);
		free(t);
	}

	/*
	// Offset 800
	Import import;
	import.setBaseVA(secImport->VirtualAddress);
	
	Library* lib = new Library("KERNEL32.DLL");
	lib->add("ExitProcess");
	import.addLibrary(lib);
	
	lib = new Library("USER32.DLL");
	lib->add("MessageBoxA");
	import.addLibrary(lib);
	
	import.write(f);
	*/
	{
		int sz = 0x200-0xA8;
		char* t = (char*)malloc(sz);
		memset(t, 0, sz);
		f.write(t, sz);
		free(t);
	}

	
	f.close();
	return 0;
}