#pragma once

#include "..\xboxInternals.h"

#define IMAGE_NUMBEROF_DIRECTORY_ENTRIES 16
#define IMAGE_SIZEOF_SHORT_NAME 8

#pragma pack(push, 1)

typedef struct XIMAGE_DOS_HEADER {      // DOS .EXE header
    uint16_t   e_magic;                     // Magic number
    uint16_t   e_cblp;                      // uchars on last page of file
    uint16_t   e_cp;                        // Pages in file
    uint16_t   e_crlc;                      // Relocations
    uint16_t   e_cparhdr;                   // Size of header in paragraphs
    uint16_t   e_minalloc;                  // Minimum extra paragraphs needed
    uint16_t   e_maxalloc;                  // Maximum extra paragraphs needed
    uint16_t   e_ss;                        // Initial (relative) SS value
    uint16_t   e_sp;                        // Initial SP value
    uint16_t   e_csum;                      // Checksum
    uint16_t   e_ip;                        // Initial IP value
    uint16_t   e_cs;                        // Initial (relative) CS value
    uint16_t   e_lfarlc;                    // File address of relocation table
    uint16_t   e_ovno;                      // Overlay number
    uint16_t   e_res[4];                    // Reserved ushorts
    uint16_t   e_oemid;                     // OEM identifier (for e_oeminfo)
    uint16_t   e_oeminfo;                   // OEM information; e_oemid specific
    uint16_t   e_res2[10];                  // Reserved ushorts
    int32_t   e_lfanew;                     // File address of new exe header
} XIMAGE_DOS_HEADER;

typedef struct XIMAGE_FILE_HEADER {
    uint16_t    Machine;
    uint16_t    NumberOfSections;
    uint32_t   TimeDateStamp;
    uint32_t   PointerToSymbolTable;
    uint32_t   NumberOfSymbols;
    uint16_t    SizeOfOptionalHeader;
    uint16_t    Characteristics;
} XIMAGE_FILE_HEADER;

#define IMAGE_DOS_SIGNATURE 0x5A4D
#define IMAGE_NT_SIGNATURE 0x00004550
#define IMAGE_FILE_MACHINE_I386 0x014c
#define IMAGE_FILE_DLL 0x2000
#define IMAGE_DIRECTORY_ENTRY_EXPORT 0
#define IMAGE_DIRECTORY_ENTRY_IMPORT 1 
#define IMAGE_DIRECTORY_ENTRY_BASERELOC 5 
#define IMAGE_REL_BASED_HIGH 1
#define IMAGE_REL_BASED_LOW 2
#define IMAGE_REL_BASED_HIGHLOW 3
#define IMAGE_REL_BASED_HIGHADJ 4
#define IMAGE_REL_BASED_DIR64 10
#define IMAGE_ORDINAL_FLAG32 0x80000000

#define IMAGE_NUMBEROF_DIRECTORY_ENTRIES 16
#define IMAGE_SIZEOF_SHORT_NAME 8

typedef struct XIMAGE_DATA_DIRECTORY {
    uint32_t   VirtualAddress;
    uint32_t   Size;
} XIMAGE_DATA_DIRECTORY;

typedef struct XIMAGE_OPTIONAL_HEADER {
    //
    // Standard fields.
    //

    uint16_t    Magic;
    uint8_t    MajorLinkerVersion;
    uint8_t    MinorLinkerVersion;
    uint32_t   SizeOfCode;
    uint32_t   SizeOfInitializedData;
    uint32_t   SizeOfUninitializedData;
    uint32_t   AddressOfEntryPoint;
    uint32_t   BaseOfCode;
    uint32_t   BaseOfData;

    //
    // NT additional fields.
    //

    uint32_t   ImageBase;
    uint32_t   SectionAlignment;
    uint32_t   FileAlignment;
    uint16_t    MajorOperatingSystemVersion;
    uint16_t    MinorOperatingSystemVersion;
    uint16_t    MajorImageVersion;
    uint16_t    MinorImageVersion;
    uint16_t    MajorSubsystemVersion;
    uint16_t    MinorSubsystemVersion;
    uint32_t   Win32VersionValue;
    uint32_t   SizeOfImage;
    uint32_t   SizeOfHeaders;
    uint32_t   CheckSum;
    uint16_t    Subsystem;
    uint16_t    DllCharacteristics;
    uint32_t   SizeOfStackReserve;
    uint32_t   SizeOfStackCommit;
    uint32_t   SizeOfHeapReserve;
    uint32_t   SizeOfHeapCommit;
    uint32_t   LoaderFlags;
    uint32_t   NumberOfRvaAndSizes;
    XIMAGE_DATA_DIRECTORY DataDirectory[IMAGE_NUMBEROF_DIRECTORY_ENTRIES];
} XIMAGE_OPTIONAL_HEADER32;

typedef struct XIMAGE_NT_HEADERS32 {
    uint32_t Signature;
    XIMAGE_FILE_HEADER FileHeader;
    XIMAGE_OPTIONAL_HEADER32 OptionalHeader;
} XIMAGE_NT_HEADERS32;

typedef struct XIMAGE_EXPORT_DIRECTORY {
    uint32_t   Characteristics;
    uint32_t   TimeDateStamp;
    uint16_t   MajorVersion;
    uint16_t   MinorVersion;
    uint32_t   Name;
    uint32_t   Base;
    uint32_t   NumberOfFunctions;
    uint32_t   NumberOfNames;
    uint32_t   AddressOfFunctions;     // RVA from base of image
    uint32_t   AddressOfNames;         // RVA from base of image
    uint32_t   AddressOfNameOrdinals;  // RVA from base of image
} XIMAGE_EXPORT_DIRECTORY;

typedef struct XIMAGE_SECTION_HEADER {
    uint8_t    Name[IMAGE_SIZEOF_SHORT_NAME];
    union {
        uint32_t   PhysicalAddress;
        uint32_t   VirtualSize;
    } Misc;
    uint32_t   VirtualAddress;
    uint32_t   SizeOfRawData;
    uint32_t   PointerToRawData;
    uint32_t   PointerToRelocations;
    uint32_t   PointerToLinenumbers;
    uint16_t    NumberOfRelocations;
    uint16_t    NumberOfLinenumbers;
    uint32_t   Characteristics;
} XIMAGE_SECTION_HEADER;

typedef struct XIMAGE_IMPORT_DIRECTORY {
    uint32_t	ImportLookupTable;
    uint32_t	TimeDateStamp;
    uint32_t	ForwarderChain;
    uint32_t	Name;
    uint32_t	ImportAddressTable;
} XIMAGE_IMPORT_DIRECTORY;

typedef struct XIMAGE_BASE_RELOCATION {
    uint32_t   VirtualAddress;
    uint32_t   SizeOfBlock;
} XIMAGE_BASE_RELOCATION;

typedef struct XIMAGE_BASE_RELOC_FIXUP {
    uint16_t Offset : 12;
    uint16_t Type : 4;
} XIMAGE_BASE_RELOC_FIXUP;

typedef struct PEObject_t
{
	void* HModule;
	XIMAGE_DOS_HEADER* DOSHeader;
	XIMAGE_NT_HEADERS32* NTHeader;
	XIMAGE_SECTION_HEADER* SectionHeader;
	XIMAGE_EXPORT_DIRECTORY* ExportTable;
	XIMAGE_IMPORT_DIRECTORY* ImportTable;
	uint32_t ImageSize;
} PEObject_t;

typedef struct PEParam_t {
    int argc;
    char** argv;
} PEEParam_t;

#pragma pack(pop)

class PEProcess
{
public:
    static PEObject_t* PE_Alloc();
    static void PE_Free(PEObject_t* PEObject);
    static PEObject_t* PE_Load(const char* filepath);
    static int32_t PE_Export(PEObject_t* PEObject, const char* filename);
    static int32_t PE_Import(PEObject_t* PEObject, const char* filename);
    static uint32_t PE_BaseReloc(PEObject_t* PEObject, const char* filename);
    static void* PE_GetSym(PEObject_t* PEObject, const char* Function);

	static char* getPluginName(const char* filePath);
    static int32_t PE_Run(const char* filePath, PEParam_t* params);
};

