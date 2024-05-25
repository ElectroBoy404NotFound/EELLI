#include <stdarg.h>
#include "utils/utils.h"
#include "fat/fat.h"

struct PartitionEntry {
	uint8_t active;
	uint8_t chsStart[3];
	uint8_t type;
	uint8_t chsEnd[3];
	uint32_t lbaStart;
	uint32_t lbaLen;
} __attribute__((packed));

struct Mbr {
	uint8_t crap[0x1be];
	struct PartitionEntry part[4];
	uint16_t sig;	//aa55
} __attribute__((packed));

struct ElfHeader {
	uint8_t		e_ident[16];
	uint16_t	e_type;
	uint16_t	e_machine;
	uint32_t	e_version;
	uint32_t	e_entry;
	uint32_t	e_phoff;		// -> program header table offset
	uint32_t	e_shoff;		// -> ElfSection[]
	uint32_t	e_flags;
	uint16_t	e_ehsize;		//this header's size in bytes
	uint16_t	e_phentsize;	//size of program header table entries
	uint16_t	e_phnum;		//number of entries in program header table
	uint16_t	e_shentsize;	//sizeof(ElfSection) (size of entry of section table
	uint16_t	e_shnum;		//num sections in section table
	uint16_t	e_shstrndx;		//section header table index of the entry associated with the section name string table. If the file has no section name string table, this member holds the value SHN_UNDEF. See Sections and String Table below for more information.
} __attribute__((packed));

#define EI_MAG0         0               /* e_ident[] indexes */
#define EI_MAG1         1
#define EI_MAG2         2
#define EI_MAG3         3
#define EI_CLASS        4
#define EI_DATA         5
#define EI_VERSION      6
#define EI_PAD          7

#define ELFMAG0         0x7F
#define ELFMAG1         'E'
#define ELFMAG2         'L'
#define ELFMAG3         'F'

#define ELFCLASS32      1
#define ELFCLASS64      2

#define ELFDATA2LSB     1
#define ELFDATA2MSB     2

#define ET_EXEC   		2

#define EM_MIPS         8 
#define EM_MIPS_RS4_BE  10

#define EV_CURRENT      1


struct ElfProgHdr{
	uint32_t	p_type;
	uint32_t	p_offset;
	uint32_t	p_vaddr;
	uint32_t	p_paddr;
	uint32_t	p_filesz;
	uint32_t	p_memsz;
	uint32_t	p_flags;
	uint32_t	p_align;
} __attribute__((packed));


#define PHF_EXEC		0x1
#define PHF_WRITE		0x2
#define PHF_READ		0x4

#define PT_NULL			0x0
#define PT_LOAD			0x1
#define PT_DYNAMIC		0x2
#define PT_INTERP		0x3
#define PT_NOTE			0x4
#define PT_SHLIB		0x5
#define PT_PHDR			0x6

typedef void (*KernelEntry)(int argc, const char **argv, uint32_t magic);

static bool loaderPrvFatReadSecProc(void *userData, uint32_t sec, void *dst)
{
	uint32_t startSec = *(const uint32_t*)userData;
	
	readblock(sec + startSec, dst);
}

void entry() {
    print_string("\n");

    static uint8_t __attribute__((aligned(512))) mFatBuf[FAT_SECTOR_SIZE];	//align guarantees no kilobyte crossings for sector read
	uint32_t startSec = 0, fSize, entryPt, i;
	struct Mbr *mbr = (struct Mbr*)mFatBuf;
	struct FatFileOpenInfoOpaque foi, foik;
	int8_t foundIdx = -1;
	struct FatFile *dir, *fil;
	struct ElfProgHdr phdr;
	struct ElfHeader ehdr;
	struct FatVolume *vol;
	uint8_t fAttr;
	char fName[13];

    readblock(0, mbr);
	if (mbr->sig != 0xaa55)
		fatal("MBR sig missing\n");
	
	for (i = 0; i < 4; i++) {
		
		//we are looking for a partition.
		
		//must be active
		if (mbr->part[i].active != 0x80)
			continue;
		
		//must have type 0x0e (fat16)
		if (mbr->part[i].type != 0x0e)
			continue;
		
		//must have a sane size (1MB+)
		if (mbr->part[i].lbaLen < 1024 * 1024 / FAT_SECTOR_SIZE)
			continue;
		
		//must be the only one
		if (foundIdx < 0)
			foundIdx = i;
		else
			fatal("More than one candidate boot partition found\n");
		
		startSec = mbr->part[i].lbaStart;
	}
	
	if (foundIdx < 0)
		fatal("No candidate boot partition found\n");

    fatInit(mFatBuf);
	
	vol = fatMount(loaderPrvFatReadSecProc, &startSec);
	if (!vol)
		fatal("mount failed\n");
	
	dir = fatOpenRoot(vol);
	if (!dir)
		fatal("failed to open root dir\n");

	foundIdx = 0;
	while (fatDirEnum(dir, fName, &fAttr, &fSize, &foi)) {
		printf(" > '%12s' 0x%02x, %u bytes\n", fName, fAttr, fSize);
		
		if (!strcmp(fName, "VMLINUX")) {
			
			if (foundIdx)
				fatal("Too many candidate kernels found\n");
			foundIdx = 1;
			foik = foi;
		}
	}
	fatFileClose(dir);

    SYSCON = 0x5555;
}