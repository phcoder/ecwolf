

#ifndef __RESFILE_H
#define __RESFILE_H

#include "files.h"
#include "zdoomsupport.h"
#include "zstring.h"

#if defined(_MSC_VER) || defined(__WATCOMC__)
#define STACK_ARGS __cdecl
#else
#define STACK_ARGS
#endif

class FResourceFile;
class FTexture;

struct FResourceLump
{
	friend class FResourceFile;

	int				LumpSize;
	FString			FullName;		// only valid for files loaded from a non-wad archive
	union
	{
		char		Name[9];

		uint32_t		dwName;	// These are for accessing the first 4 or 8 chars of
		uint64_t		qwName;	// Name as a unit without breaking strict aliasing rules
	};
	BYTE			Flags;
	SBYTE			RefCount;
	char *			Cache;
	FResourceFile *	Owner;
	FTexture *		LinkedTexture;
	int				Namespace;

	FResourceLump()
	{
		Cache = NULL;
		Owner = NULL;
		Flags = 0;
		RefCount = 0;
		Namespace = 0;	// ns_global
		*Name = 0;
		LinkedTexture = NULL;
	}

	virtual ~FResourceLump();
	virtual FileReader *GetReader();
	virtual FileReader *NewReader();
	virtual int GetFileOffset() { return -1; }
	virtual int GetIndexNum() const { return 0; }
	virtual void DoFinishRemap() {} // For handling any changes that may happen after the WL6 remapper takes action
	void LumpNameSetup(FString iname);
	void CheckEmbedded();

	void *CacheLump();
	int ReleaseCache();

protected:
	virtual int FillCache() = 0;

};

class FResourceFile
{
public:
	FileReader *Reader;
	const char *Filename;
protected:
	uint32_t NumLumps;

	FResourceFile(const char *filename, FileReader *r);

	// for archives that can contain directories
	void PostProcessArchive(void *lumps, size_t lumpsize);

private:
	uint32_t FirstLump;

	int FilterLumps(FString filtername, void *lumps, size_t lumpsize, uint32_t max);
	//int FilterLumpsByGameType(int gametype, void *lumps, size_t lumpsize, uint32_t max);
	bool FindPrefixRange(FString filter, void *lumps, size_t lumpsize, uint32_t max, uint32_t &start, uint32_t &end);
	void JunkLeftoverFilters(void *lumps, size_t lumpsize, uint32_t max);

public:
	static FResourceFile *OpenResourceFile(const char *filename, FileReader *file, bool quiet = false);
	static FResourceFile *OpenDirectory(const char *filename, bool quiet = false);
	virtual ~FResourceFile();
	FileReader *GetReader() const { return Reader; }
	uint32_t LumpCount() const { return NumLumps; }
	uint32_t GetFirstLump() const { return FirstLump; }
	void SetFirstLump(uint32_t f) { FirstLump = f; }

	virtual void FindStrifeTeaserVoices ();
	virtual bool Open(bool quiet) = 0;
	virtual FResourceLump *GetLump(int no) = 0;
};

struct FUncompressedLump : public FResourceLump
{
	int				Position;

	virtual FileReader *GetReader();
	virtual int FillCache();
	virtual int GetFileOffset() { return Position; }

};


// Base class for uncompressed resource files (WAD, GRP, PAK and single lumps)
class FUncompressedFile : public FResourceFile
{
protected:
	FUncompressedLump * Lumps;


	FUncompressedFile(const char *filename, FileReader *r);
	virtual ~FUncompressedFile();
	virtual FResourceLump *GetLump(int no) { return ((unsigned)no < NumLumps)? &Lumps[no] : NULL; }

public:
};


struct FExternalLump : public FResourceLump
{
	const char *filename;	// the actual file name. This is not necessarily the same as the lump name!

	FExternalLump(const char *_filename, int filesize = -1);
	~FExternalLump();
	virtual int FillCache();

};

//==========================================================================
//
// FileReader that reads from a lump's cache
//
//==========================================================================

class FLumpReader : public MemoryReader
{
	FResourceLump *source;

public:
	FLumpReader(FResourceLump *src)
		: MemoryReader(NULL, src->LumpSize), source(src)
	{
		src->CacheLump();
		bufptr = src->Cache;
	}

	~FLumpReader()
	{
		source->ReleaseCache();
	}

	FResourceFile *LumpOwner() const
	{
		return source->Owner;
	}
};

struct FDirectoryLump : public FResourceLump
{
	virtual FileReader *NewReader();
	virtual int FillCache();

private:
};

class FDirectory : public FResourceFile
{
	TArray<FDirectoryLump> Lumps;

	int AddDirectory(const char *dirpath);
	void AddEntry(const char *fullpath, int size);

public:
	FDirectory(const char * dirname);
	bool Open(bool quiet);
	virtual FResourceLump *GetLump(int no) { return ((unsigned)no < NumLumps)? &Lumps[no] : NULL; }
};

#endif
