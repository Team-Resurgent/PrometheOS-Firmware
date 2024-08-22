#include "PESymbol.h"
#include "PEProcess.h"

#include "..\utils.h"
#include "..\xboxInternals.h"

#include <string.h>
#include <malloc.h>

static LibraryEntry_t* ImportableSymbols;

LibraryEntry_t* PESymbol::AddLibrary(const char* filename)
{
	LibraryEntry_t* NL = (LibraryEntry_t*)malloc(sizeof(LibraryEntry_t));
	if (!NL)
	{
		return NULL;
	}

	NL->Name = strdup(filename);
	NL->Next = NULL;
	NL->SymTab = NULL;
	NL->Ready = 0;

	// Check if libraries have been added yet.
	if (ImportableSymbols == NULL) 
	{
		ImportableSymbols = NL;
	}
	else 
	{
		for (LibraryEntry_t* CL = ImportableSymbols; CL != NULL; CL = CL->Next)
		{
			if (CL->Next == NULL)
			{
				CL->Next = NL;
				break;
			}
		}
	}

	// If we get here then shit has happened.
	return NL;
}

LibraryEntry_t* PESymbol::FindLibrary(const char* filename)
{
	LibraryEntry_t* CL;

	// Check if libraries have been added yet.
	if (ImportableSymbols == NULL) {
		return NULL;
	}

	// Atleast one library added, check to see if this one has been.
	for (CL = ImportableSymbols; CL != NULL; CL = CL->Next) 
	{
		if ((CL->Name != NULL) && (!_stricmp(CL->Name, filename)))
		{
			return CL;
		}
	}

	// Requested library not found.
	return NULL;
}

LibraryEntry_t* PESymbol::FindOrAddLibrary(const char* filename)
{
	LibraryEntry_t* RV = FindLibrary(filename);
	if (RV == NULL) 
	{
		RV = AddLibrary(filename);
		utils::debugPrint("Library [%s]: Added.\n", filename);
	}

	return RV;
}

SymbolEntry_t* PESymbol::AddSymbol(const char* filename, const char* Name, uint32_t Ordinal, void* Symbol)
{
	LibraryEntry_t* LT = FindOrAddLibrary(filename);

	if (LT->SymTab == NULL)
	{
		LT->SymTab = (SymbolEntry_t*)malloc(sizeof(SymbolEntry_t));
		LT->SymTab->Name = strdup(Name);
		LT->SymTab->Ordinal = Ordinal;
		LT->SymTab->Symbol = Symbol;
		LT->SymTab->Next = NULL;
		return LT->SymTab;
	}
	else 
	{
		SymbolEntry_t* CF;
		for (CF = LT->SymTab; CF != NULL; CF = CF->Next)
		{
			if (CF->Next == NULL)
			{
				CF->Next = (SymbolEntry_t*)malloc(sizeof(SymbolEntry_t));
				CF = CF->Next;
				if (CF) 
				{
					CF->Name = strdup(Name);
					CF->Ordinal = Ordinal;
					CF->Symbol = Symbol;
					CF->Next = NULL;
				}
				return CF;
			}
		}
	}

	// If we get here then shit has happened.
	return NULL;
}

void PESymbol::UnresolvedSymbolCalled()
{
	utils::debugPrint("An unresolved symbol was called.\n");
}

uint32_t PESymbol::ResolveImportByOrdinal(const char* Filename, const char* Library, uint32_t Ordinal)
{
	SymbolEntry_t* CF;
	LibraryEntry_t* IL = FindLibrary(Library);
	if (IL == NULL) 
	{
		goto Unresolved;
	}

	for (CF = IL->SymTab; CF != NULL; CF = CF->Next) 
	{
		if (Ordinal == CF->Ordinal) 
		{
			return (uint32_t)CF->Symbol;
		}
	}

Unresolved:
	utils::debugPrint("Unable to resolve ordinal '%lu' from library '%s' for '%s'.\n", Ordinal, Library, Filename);
	return (uint32_t)UnresolvedSymbolCalled;
}

void PESymbol::RemoveLibrary(const char* library)
{
	LibraryEntry_t * PL = NULL;
	for (LibraryEntry_t* CL = ImportableSymbols; CL != NULL; CL = CL->Next)
	{
		if ((CL->Name != NULL) && (!_stricmp(CL->Name, library)))
		{
			if (PL)
			{
				PL->Next = CL->Next;
			}
			else ImportableSymbols = CL->Next;
			free(CL->Name);
			if (CL->SymTab) 
			{
				SymbolEntry_t* NE = NULL;
				for (SymbolEntry_t* CE = CL->SymTab; CE != NULL; CE = NE)
				{
					SymbolEntry_t* NE = CE->Next;
					if (CE) 
					{
						if (CE->Name)
						{
							free(CE->Name);
						}
						free(CE);
					}
				}
			}
		}
		PL = CL;
	}
}

LibraryEntry_t* PESymbol::FindOrLoadLibrary(const char* library)
{
	LibraryEntry_t* RV = FindLibrary(library);
	if (RV == NULL) 
	{
		char* fp = (char*)malloc(strlen("HDD0-E:\\Root\\Libs\\") + strlen(library) + 2);
		strcpy(fp, "HDD0-E:\\Root\\Libs\\");
		strcat(fp, library);
		
		utils::debugPrint("Library [%s]: %s Loading\n", library, fp);

		PEObject_t* lib = PEProcess::PE_Load(fp);
		if (lib) 
		{
			RV = FindLibrary(library);
			RV->Object = lib;
			RV->Ready = 1;
		}
		else
		{
			RemoveLibrary(library);
		}
		free(fp);
	}

	return RV;
}

uint32_t PESymbol::ResolveImportByName(const char* filename, const char* library, const char* symbol)
{
	SymbolEntry_t* CF;
	LibraryEntry_t* IL = FindOrLoadLibrary(library);
	if (IL == NULL)
	{
		goto Unresolved;
	}

	while (!IL->Ready)
	{
		//Sleep(10);
	}

	for (CF = IL->SymTab; CF != NULL; CF = CF->Next) 
	{
		if (!strcmp(symbol, CF->Name))
		{
			return (uint32_t)CF->Symbol;
		}
	}

Unresolved:
	utils::debugPrint("Unable to resolve symbol '%s' from library '%s' for '%s'.\n", symbol, library, filename);
	return (uint32_t)UnresolvedSymbolCalled;
}