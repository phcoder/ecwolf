#ifndef __W_ZIP
#define __W_ZIP

#pragma pack(push, 1)
// FZipCentralInfo
struct FZipEndOfCentralDirectory
{
	uint32_t	Magic;
	uint16_t	DiskNumber;
	uint16_t	FirstDisk;
	uint16_t	NumEntries;
	uint16_t	NumEntriesOnAllDisks;
	uint32_t	DirectorySize;
	uint32_t	DirectoryOffset;
	uint16_t	ZipCommentLength;
};

// FZipFileInfo
struct FZipCentralDirectoryInfo
{
	uint32_t	Magic;
	BYTE	VersionMadeBy[2];
	BYTE	VersionToExtract[2];
	uint16_t	Flags;
	uint16_t	Method;
	uint16_t	ModTime;
	uint16_t	ModDate;
	uint32_t	CRC32;
	uint32_t	CompressedSize;
	uint32_t	UncompressedSize;
	uint16_t	NameLength;
	uint16_t	ExtraLength;
	uint16_t	CommentLength;
	uint16_t	StartingDiskNumber;
	uint16_t	InternalAttributes;
	uint32_t	ExternalAttributes;
	uint32_t	LocalHeaderOffset;
	// file name and other variable length info follows
};

// FZipLocalHeader
struct FZipLocalFileHeader
{
	uint32_t	Magic;
	BYTE	VersionToExtract[2];
	uint16_t	Flags;
	uint16_t	Method;
	uint16_t	ModTime;
	uint16_t	ModDate;
	uint32_t	CRC32;
	uint32_t	CompressedSize;
	uint32_t	UncompressedSize;
	uint16_t	NameLength;
	uint16_t	ExtraLength;
	// file name and other variable length info follows
};


#pragma pack(pop)

#define ZIP_LOCALFILE	MAKE_ID('P','K',3,4)
#define ZIP_CENTRALFILE	MAKE_ID('P','K',1,2)
#define ZIP_ENDOFDIR	MAKE_ID('P','K',5,6)

#define METHOD_STORED	0
#define METHOD_SHRINK	1
#define METHOD_IMPLODE	6
#define METHOD_DEFLATE	8
#define METHOD_BZIP2	12
#define METHOD_LZMA		14
#define METHOD_PPMD		98

// File header flags.
#define ZF_ENCRYPTED			0x1

#endif
