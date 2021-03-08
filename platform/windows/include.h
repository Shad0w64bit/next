#ifndef __PLATFORM_WINDOWS_INCLUDE__
#define __PLATFORM_WINDOWS_INCLUDE__

#define IMAGE_FILE_MACHINE_I386  0x014c
#define IMAGE_FILE_MACHINE_AMD64 0x8664

#define IMAGE_FILE_RELOCS_STRIPPED			0x0001
#define IMAGE_FILE_EXECUTABLE_IMAGE			0x0002
#define IMAGE_FILE_LINE_NUMS_STRIPPED		0x0004
#define IMAGE_FILE_LOCAL_SYMS_STRIPPED	0x0008
#define IMAGE_FILE_LARGE_ADDRESS_AWARE	0x0020
#define IMAGE_FILE_32BIT_MACHINE				0x0100

#define IMAGE_SCN_CNT_CODE								0x00000020
#define IMAGE_SCN_CNT_INITIALIZED_DATA		0x00000040
#define IMAGE_SCN_CNT_UNINITIALIZED_DATA	0x00000080
#define IMAGE_SCN_MEM_EXECUTE							0x20000000
#define IMAGE_SCN_MEM_READ								0x40000000
#define IMAGE_SCN_MEM_WRITE								0x80000000
//#define
//#define

#define IMAGE_DIRECTORY_ENTRY_IMPORT 1
#define IMAGE_SIZEOF_SHORT_NAME 8
#define IMAGE_NUMBEROF_DIRECTORY_ENTRIES 16

typedef struct _IMAGE_IMPORT_DESCRIPTOR {
        union {
                 uint32_t Characteristics; // 0 for terminating null import descriptor
                 uint32_t OriginalFirstThunk; // RVA to original unbound IAT
         };
         uint32_t TimeDateStamp; // 0 if not bound,
         // -1 if bound, and real date\time stamp
         // in IMAGE_DIRECTORY_ENTRY_BOUND_IMPORT (new)
         // O.W. date/time stamp of DLL bound to (old)
         uint32_t ForwarderChain; // -1 if no forwarders
         uint32_t Name;
         uint32_t FirstThunk; // RVA to IAT
} IMAGE_IMPORT_DESCRIPTOR;

typedef struct _IMAGE_SECTION_HEADER {
  uint8_t  Name[IMAGE_SIZEOF_SHORT_NAME];
  union {
    uint32_t PhysicalAddress;
    uint32_t VirtualSize;
  } Misc;
  uint32_t VirtualAddress;
  uint32_t SizeOfRawData;
  uint32_t PointerToRawData;
  uint32_t PointerToRelocations;
  uint32_t PointerToLinenumbers;
  uint16_t  NumberOfRelocations;
  uint16_t  NumberOfLinenumbers;
  uint32_t Characteristics;
} IMAGE_SECTION_HEADER, *PIMAGE_SECTION_HEADER;

typedef struct _IMAGE_FILE_HEADER {
  uint16_t	Machine;
  uint16_t	NumberOfSections;
  uint32_t	TimeDateStamp;
  uint32_t	PointerToSymbolTable;
  uint32_t	NumberOfSymbols;
  uint16_t	SizeOfOptionalHeader;
  uint16_t	Characteristics;
} IMAGE_FILE_HEADER, *PIMAGE_FILE_HEADER;

typedef struct _IMAGE_DATA_DIRECTORY {
  uint32_t VirtualAddress;
  uint32_t Size;
} IMAGE_DATA_DIRECTORY, *PIMAGE_DATA_DIRECTORY;

typedef struct _IMAGE_OPTIONAL_HEADER {
  uint16_t                Magic;
  uint8_t                 MajorLinkerVersion;
  uint8_t                 MinorLinkerVersion;
  uint32_t                SizeOfCode;
  uint32_t                SizeOfInitializedData;
  uint32_t                SizeOfUninitializedData;
  uint32_t                AddressOfEntryPoint;
  uint32_t                BaseOfCode;
  uint32_t                BaseOfData;
  uint32_t                ImageBase;
  uint32_t                SectionAlignment;
  uint32_t                FileAlignment;
  uint16_t                MajorOperatingSystemVersion;
  uint16_t                MinorOperatingSystemVersion;
  uint16_t                MajorImageVersion;
  uint16_t                MinorImageVersion;
  uint16_t                MajorSubsystemVersion;
  uint16_t                MinorSubsystemVersion;
  uint32_t                Win32VersionValue;
  uint32_t                SizeOfImage;
  uint32_t                SizeOfHeaders;
  uint32_t                CheckSum;
  uint16_t                Subsystem;
  uint16_t                DllCharacteristics;
  uint32_t                SizeOfStackReserve;
  uint32_t                SizeOfStackCommit;
  uint32_t                SizeOfHeapReserve;
  uint32_t                SizeOfHeapCommit;
  uint32_t                LoaderFlags;
  uint32_t                NumberOfRvaAndSizes;
  IMAGE_DATA_DIRECTORY DataDirectory[IMAGE_NUMBEROF_DIRECTORY_ENTRIES];
} IMAGE_OPTIONAL_HEADER32, *PIMAGE_OPTIONAL_HEADER32;

typedef struct _IMAGE_NT_HEADERS {
  uint32_t                   Signature;
  IMAGE_FILE_HEADER       FileHeader;
  IMAGE_OPTIONAL_HEADER32 OptionalHeader;
} IMAGE_NT_HEADERS32, *PIMAGE_NT_HEADERS32;


typedef struct _IMAGE_OPTIONAL_HEADER64 {
  uint16_t                Magic;
  uint8_t                 MajorLinkerVersion;
  uint8_t                 MinorLinkerVersion;
  uint32_t                SizeOfCode;
  uint32_t                SizeOfInitializedData;
  uint32_t                SizeOfUninitializedData;
  uint32_t                AddressOfEntryPoint;
  uint32_t                BaseOfCode;
  uint64_t                ImageBase;
  uint32_t                SectionAlignment;
  uint32_t                FileAlignment;
  uint16_t                MajorOperatingSystemVersion;
  uint16_t                MinorOperatingSystemVersion;
  uint16_t                MajorImageVersion;
  uint16_t                MinorImageVersion;
  uint16_t                MajorSubsystemVersion;
  uint16_t                MinorSubsystemVersion;
  uint32_t                Win32VersionValue;
  uint32_t                SizeOfImage;
  uint32_t                SizeOfHeaders;
  uint32_t                CheckSum;
  uint16_t                Subsystem;
  uint16_t                DllCharacteristics;
  uint64_t                SizeOfStackReserve;
  uint64_t                SizeOfStackCommit;
  uint64_t                SizeOfHeapReserve;
  uint64_t                SizeOfHeapCommit;
  uint32_t                LoaderFlags;
  uint32_t                NumberOfRvaAndSizes;
  IMAGE_DATA_DIRECTORY DataDirectory[IMAGE_NUMBEROF_DIRECTORY_ENTRIES];
} IMAGE_OPTIONAL_HEADER64, *PIMAGE_OPTIONAL_HEADER64;


typedef struct _IMAGE_NT_HEADERS64 {
  uint32_t                Signature;
  IMAGE_FILE_HEADER       FileHeader;
  IMAGE_OPTIONAL_HEADER64 OptionalHeader;
} IMAGE_NT_HEADERS64, *PIMAGE_NT_HEADERS64;



typedef struct _IMAGE_DOS_HEADER
{
     uint16_t e_magic;
     uint16_t e_cblp;
     uint16_t e_cp;
     uint16_t e_crlc;
     uint16_t e_cparhdr;
     uint16_t e_minalloc;
     uint16_t e_maxalloc;
     uint16_t e_ss;
     uint16_t e_sp;
     uint16_t e_csum;
     uint16_t e_ip;
     uint16_t e_cs;
     uint16_t e_lfarlc;
     uint16_t e_ovno;
     uint16_t e_res[4];
     uint16_t e_oemid;
     uint16_t e_oeminfo;
     uint16_t e_res2[10];
     uint32_t e_lfanew;
} IMAGE_DOS_HEADER, *PIMAGE_DOS_HEADER;

#endif
