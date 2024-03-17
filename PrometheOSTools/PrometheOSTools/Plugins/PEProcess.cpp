#include "PEProcess.h"
#include "PESymbol.h"

#include "..\utils.h"
#include "..\fileSystem.h"
#include "..\xboxInternals.h"

#include <malloc.h>
#include <fstream>

PEObject_t* PEProcess::PE_Alloc() 
{
	PEObject_t* PEObject = (PEObject_t*)malloc(sizeof(PEObject_t));
	if (!PEObject)
	{
		return NULL;
	}
	PEObject->HModule = NULL;
	PEObject->DOSHeader = NULL;
	PEObject->NTHeader = NULL;
	PEObject->SectionHeader = NULL;
	PEObject->ExportTable = NULL;
	PEObject->ImportTable = NULL;
	PEObject->ImageSize = 0;
	return PEObject;
}

void PEProcess::PE_Free(PEObject_t* PEObject) 
{
	if (PEObject->HModule != NULL)
	{
		_aligned_free(PEObject->HModule);
	}
	PEObject->HModule = NULL;
	PEObject->DOSHeader = NULL;
	PEObject->NTHeader = NULL;
	PEObject->SectionHeader = NULL;
	PEObject->ExportTable = NULL;
	PEObject->ImportTable = NULL;
	PEObject->ImageSize = 0;
	free(PEObject);
}

PEObject_t* PEProcess::PE_Load(const char* filepath)
{
	std::ifstream file(filepath, std::ios::binary);
	if (!file.is_open()) 
	{
		utils::debugPrint("PE Loader: Unable to open image file.\n");
		return NULL;
	}

	char* filename = fileSystem::getFileName(filepath);

	PEObject_t* PEObject = NULL;

	PEObject = PE_Alloc();
	if (PEObject == NULL) 
	{
		utils::debugPrint("PE Loader [%s]: Unable to allocate context.\n", filename);
		file.close();
		return NULL;
	}

	file.seekg(0, std::ios::end);
	std::streampos fileSize = file.tellg();
	file.seekg(0, std::ios::beg);

	PEObject->ImageSize = fileSize;

	PEObject->DOSHeader = (XIMAGE_DOS_HEADER*)malloc(sizeof(XIMAGE_DOS_HEADER));
	if (PEObject->DOSHeader == NULL) {
		utils::debugPrint("PE Loader [%s]: \n\tUnable to allocate memory for DOSHeader [0x%08x bytes].\n", filename, sizeof(XIMAGE_DOS_HEADER));
		free(PEObject);
		file.close();
		return NULL;
	}
	
	file.read((char*)PEObject->DOSHeader, sizeof(XIMAGE_DOS_HEADER));
	if (PEObject->DOSHeader->e_magic != IMAGE_DOS_SIGNATURE) {
		utils::debugPrint("PE Loader [%s]: \n\tInvalid DOS signature 0x%04x.\n", filename, PEObject->DOSHeader->e_magic);
		free(PEObject->DOSHeader);
		free(PEObject);
		file.close();
		return NULL;
	}

	PEObject->NTHeader = (XIMAGE_NT_HEADERS32*)malloc(sizeof(XIMAGE_NT_HEADERS32));
	if (PEObject->NTHeader == NULL) {
		utils::debugPrint("PE Loader [%s]: \n\tUnable to allocate memory for NTHeader [0x%08x bytes].\n", filename, sizeof(XIMAGE_NT_HEADERS32));
		free(PEObject->DOSHeader);
		free(PEObject);
		file.close();
		return NULL;
	}

	file.seekg(PEObject->DOSHeader->e_lfanew, std::ios::beg);
	file.read((char*)PEObject->NTHeader, sizeof(XIMAGE_NT_HEADERS32));
	if (PEObject->NTHeader->Signature != IMAGE_NT_SIGNATURE) {
		utils::debugPrint("PE Loader [%s]: \n\tInvalid NT signature 0x%08x.\n", filename, PEObject->NTHeader->Signature);
		free(PEObject->NTHeader);
		free(PEObject->DOSHeader);
		free(PEObject);
		file.close();
		return NULL;
	}

	PEObject->HModule = _aligned_malloc(PEObject->NTHeader->OptionalHeader.SizeOfImage, 0x10000);
	if (PEObject->HModule == NULL) {
		utils::debugPrint("PE Loader [%s]: \n\tUnable to allocate aligned memory for HModule [0x%08x bytes].\n", filename, PEObject->NTHeader->OptionalHeader.SizeOfImage);
		free(PEObject->NTHeader);
		free(PEObject->DOSHeader);
		free(PEObject);
		file.close();
		return NULL;
	}

	memset(PEObject->HModule, 0, PEObject->NTHeader->OptionalHeader.SizeOfImage);

	file.seekg(0, std::ios::beg);
	file.read((char*)PEObject->HModule, PEObject->NTHeader->OptionalHeader.SizeOfHeaders);

	utils::debugPrint("PE Loader [%s]: Image 0x%08x.\n", filename, (uint32_t)PEObject->HModule);

	free(PEObject->DOSHeader);
	PEObject->DOSHeader = (XIMAGE_DOS_HEADER*)PEObject->HModule;

	free(PEObject->NTHeader); 
	PEObject->NTHeader = (XIMAGE_NT_HEADERS32*)((char*)PEObject->HModule + PEObject->DOSHeader->e_lfanew);

	if (PEObject->NTHeader->FileHeader.Machine != IMAGE_FILE_MACHINE_I386)
	{
		utils::debugPrint("PE Loader [%s]: \n\tInvalid archetecture 0x%04x.\n", filename, PEObject->NTHeader->FileHeader.Machine);
	}

	if ((PEObject->NTHeader->FileHeader.Characteristics & IMAGE_FILE_DLL) == 0)
	{
		utils::debugPrint("PE Loader [%s]: \n\tFile is not a DLL.\n", filename);
	}

	//This was changed to int64
	PEObject->SectionHeader = (XIMAGE_SECTION_HEADER*)(((uint32_t)&PEObject->NTHeader->OptionalHeader) + PEObject->NTHeader->FileHeader.SizeOfOptionalHeader);
	for (int i = 0; i < PEObject->NTHeader->FileHeader.NumberOfSections; i++) 
	{
		utils::debugPrint("PE Loader [%s]: Section header found '%s'.\n", filename, PEObject->SectionHeader[i].Name);
		if (PEObject->SectionHeader[i].SizeOfRawData) 
		{
			file.seekg(PEObject->SectionHeader[i].PointerToRawData, std::ios::beg);
			file.read(((char*)PEObject->HModule) + PEObject->SectionHeader[i].VirtualAddress, PEObject->SectionHeader[i].SizeOfRawData);
		}
	}

	PE_Export(PEObject, filename);
	PE_Import(PEObject, filename);
	PE_BaseReloc(PEObject, filename);

	file.close();
	return PEObject;
}

int32_t PEProcess::PE_Export(PEObject_t* PEObject, const char* library)
{
	uint32_t VirtualAddress = PEObject->NTHeader->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT].VirtualAddress;
	if (VirtualAddress == 0)
	{
		return 0;
	}

	PEObject->ExportTable = (XIMAGE_EXPORT_DIRECTORY*)((char*)PEObject->HModule + VirtualAddress);
	uint32_t* ExportNameTable = (uint32_t*)((char*)PEObject->HModule + PEObject->ExportTable->AddressOfNames);
	uint16_t* ExportOrdinalTable = (uint16_t*)((char*)PEObject->HModule + PEObject->ExportTable->AddressOfNameOrdinals);
	uint32_t* ExportAddressTable = (uint32_t*)((char*)PEObject->HModule + PEObject->ExportTable->AddressOfFunctions);
	for (uint32_t x = 0; x < PEObject->ExportTable->NumberOfNames; x++) {
		char* SymStr = (char*)PEObject->HModule + ExportNameTable[x];
		uint32_t* SymVal = (uint32_t*)((char*)PEObject->HModule + ExportAddressTable[ExportOrdinalTable[x]]);
		PESymbol::AddSymbol(library, SymStr, ExportOrdinalTable[x], SymVal);
	}

	return 1;
}

int32_t PEProcess::PE_Import(PEObject_t* PEObject, const char* filename)
{
	uint32_t VirtualAddress = PEObject->NTHeader->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT].VirtualAddress;

	if (VirtualAddress == 0) return 0;
	PEObject->ImportTable = (XIMAGE_IMPORT_DIRECTORY*)((char*)PEObject->HModule + VirtualAddress);
	for (uint32_t x = 0; PEObject->ImportTable[x].ImportLookupTable != 0; x++) {
		uint32_t* ILT = (uint32_t*)((char*)PEObject->HModule + PEObject->ImportTable[x].ImportLookupTable);
		uint32_t* IAT = (uint32_t*)((char*)PEObject->HModule + PEObject->ImportTable[x].ImportAddressTable);
		utils::debugPrint("PE Loader [%s]: \n\tDLL Reference: %s\n", filename, (char*)PEObject->HModule+PEObject->ImportTable[x].Name);
		for (uint32_t y = 0; ILT[y] != 0; y++) {
			if (ILT[y] & IMAGE_ORDINAL_FLAG32) {
				IAT[y] = PESymbol::ResolveImportByOrdinal(filename, (char*)PEObject->HModule + PEObject->ImportTable[x].Name, ILT[y] & (~IMAGE_ORDINAL_FLAG32));
				utils::debugPrint("\t\tImport by Ordinal 0x%08x : 0x%08x\n", ILT[y]&(~IMAGE_ORDINAL_FLAG32), IAT[y]);
			}
			else {
				char* ImportStr = (char*)PEObject->HModule + (ILT[y] & (~IMAGE_ORDINAL_FLAG32)) + 2;
				IAT[y] = PESymbol::ResolveImportByName(filename, (char*)PEObject->HModule + PEObject->ImportTable[x].Name, ImportStr);
				utils::debugPrint("\t\tImport by String %s : 0x%08x\n", ImportStr, IAT[y]);
			}
		}
	}

	return 1;
}

uint32_t PEProcess::PE_BaseReloc(PEObject_t* PEObject, const char* filename)
{
	uint32_t VirtualAddress = PEObject->NTHeader->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_BASERELOC].VirtualAddress;
	uint32_t DirectorySize = PEObject->NTHeader->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_BASERELOC].Size;
	uint32_t Delta = (uint32_t)PEObject->HModule - PEObject->NTHeader->OptionalHeader.ImageBase;
	
	if (VirtualAddress == 0) 
	{
		return 0;
	}

	if (Delta == 0) 
	{
		return 1;
	}

	for (uint32_t x = 0; x < DirectorySize;) 
	{
		XIMAGE_BASE_RELOCATION* BaseReloc = (XIMAGE_BASE_RELOCATION*)((char*)PEObject->HModule + VirtualAddress + x);
		uint32_t Offset = (uint32_t)PEObject->HModule + BaseReloc->VirtualAddress;
		if (BaseReloc->SizeOfBlock) 
		{
			for (uint32_t y = 0; y < BaseReloc->SizeOfBlock - 8; y += 2) 
			{
				XIMAGE_BASE_RELOC_FIXUP* Fixup = (XIMAGE_BASE_RELOC_FIXUP*)((char*)BaseReloc + sizeof(XIMAGE_BASE_RELOCATION) + y);
				switch (Fixup[0].Type) 
				{
					case IMAGE_REL_BASED_HIGH: 
					{
						uint16_t* Victim = (uint16_t*)(Offset + Fixup[0].Offset);
						uint32_t TEMP;
						TEMP = Victim[0] << 16;
						TEMP += Delta;
						Victim[0] = (uint16_t)(TEMP >> 16);
						break; 
					}
					case IMAGE_REL_BASED_LOW: 
					{
						uint16_t* Victim = (uint16_t*)(Offset + Fixup[0].Offset);
						uint32_t TEMP;
						TEMP = Victim[0];
						TEMP += Delta;
						Victim[0] = (uint16_t)(TEMP & 0xffff);
						break; 
					}
					case IMAGE_REL_BASED_HIGHLOW: 
					{
						uint32_t* Victim = (uint32_t*)(Offset + Fixup[0].Offset);
						Victim[0] += Delta;
						break; }
					case IMAGE_REL_BASED_HIGHADJ: 
					{
						uint32_t* Victim = (uint32_t*)(Offset + Fixup[0].Offset);
						Victim[0] += Delta;
						break; 
					}
					case IMAGE_REL_BASED_DIR64:
					{
						__int64* Victim = (__int64*)(Offset + Fixup[0].Offset);
						Victim[0] += Delta;
						break;
					}
				}
			}
		}
		x += BaseReloc->SizeOfBlock;
		if (BaseReloc->SizeOfBlock % 4)
		{
			x += 4 - (BaseReloc->SizeOfBlock % 4);
		}
	}

	return 1;
}

void* PEProcess::PE_GetSym(PEObject_t* PEObject, const char* Function) 
{
	if ((!PEObject) || (!PEObject->HModule) || (!PEObject->ExportTable))
	{
		return NULL;
	}

	uint32_t* ExportNameTable = (uint32_t*)((char*)PEObject->HModule + PEObject->ExportTable->AddressOfNames);
	uint16_t* ExportOrdinalTable = (uint16_t*)((char*)PEObject->HModule + PEObject->ExportTable->AddressOfNameOrdinals);
	uint32_t* ExportAddressTable = (uint32_t*)((char*)PEObject->HModule + PEObject->ExportTable->AddressOfFunctions);
	for (uint32_t i = 0; i < PEObject->ExportTable->NumberOfNames; i++) 
	{
		char* Str = (char*)PEObject->HModule + ExportNameTable[i];
		if (!strcmp(Function, Str)) 
		{
			return ((char*)PEObject->HModule + ExportAddressTable[ExportOrdinalTable[i]]);
		}
	}
	return NULL;
}



char* PEProcess::getPluginName(const char* filePath)
{
	char* buffer = NULL;

	void (*NXEMain)(char*, uint32_t) = NULL;

	PEObject_t* peObject = PEProcess::PE_Load(filePath);
	if (!peObject)
	{
		goto done;
	}
	NXEMain = (void(__cdecl*)(char*, uint32_t))PEProcess::PE_GetSym(peObject, "getPluginName");
	if (!NXEMain)
	{
		goto done;
	}

	buffer = (char*)malloc(40);
	NXEMain(buffer, 40);

done:

	if (peObject)
	{
		PEProcess::PE_Free(peObject);
	}

	return buffer;
}

int32_t PEProcess::PE_Run(const char* filePath, PEParam_t* params)
{
	int (*NXEMain)(int, char**) = NULL;

	PEObject_t* peObject = PEProcess::PE_Load(filePath);
	if (!peObject)
	{
		goto done;
	}
	NXEMain = (int(__cdecl*)(int, char**))PEProcess::PE_GetSym(peObject, "main");
	if (!NXEMain)
	{
		goto done;
	}

	return (unsigned long)NXEMain(params->argc, params->argv);

done:

	if (peObject)
	{
		PEProcess::PE_Free(peObject);
	}

	if (params)
	{
		for (int i = 0; i < params->argc; i++)
		{
			if (params->argv[i])
			{
				free(params->argv[i]);
			}
		}
		free(params);
	}

	return -1;
}
