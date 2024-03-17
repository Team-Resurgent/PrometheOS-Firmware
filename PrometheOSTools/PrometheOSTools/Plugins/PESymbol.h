#pragma once

#include "..\xboxInternals.h"

typedef struct SymbolSetup_t {
	char* Library;
	char* Name;
	uint32_t Ordinal;
	void* Symbol;
} SymbolSetup_t;

typedef struct SymbolEntry_t {
	char* Name;
	uint32_t Ordinal;
	void* Symbol;
	struct SymbolEntry_t* Next;
} SymbolEntry_t;

typedef struct LibraryEntry_t {
	char* Name;
	SymbolEntry_t* SymTab;
	struct LibraryEntry_t* Next;
	void* Object;
	int Ready;
} LibraryEntry_t;

class PESymbol
{
public:
	static LibraryEntry_t* AddLibrary(const char* filename);
	static LibraryEntry_t* FindLibrary(const char* filename);
	static LibraryEntry_t* FindOrAddLibrary(const char* filename);
	static SymbolEntry_t* AddSymbol(const char* filename, const char* name, uint32_t ordinal, void* symbol);
	static void UnresolvedSymbolCalled();
	static uint32_t ResolveImportByOrdinal(const char* filename, const char* library, uint32_t ordinal);
	static LibraryEntry_t* FindOrLoadLibrary(const char* library);
	static void RemoveLibrary(const char* Library);
	static uint32_t ResolveImportByName(const char* filename, const char* library, const char* symbol);
};

