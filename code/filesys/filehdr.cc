// filehdr.cc
//	Routines for managing the disk file header (in UNIX, this
//	would be called the i-node).
//
//	The file header is used to locate where on disk the
//	file's data is stored.  We implement this as a fixed size
//	table of pointers -- each entry in the table points to the
//	disk sector containing that portion of the file data
//	(in other words, there are no indirect or doubly indirect
//	blocks). The table size is chosen so that the file header
//	will be just big enough to fit in one disk sector,
//
//      Unlike in a real system, we do not keep track of file permissions,
//	ownership, last modification date, etc., in the file header.
//
//	A file header can be initialized in two ways:
//	   for a new file, by modifying the in-memory data structure
//	     to point to the newly allocated data blocks
//	   for a file already on disk, by reading the file header from disk
//
// Copyright (c) 1992-1993 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation
// of liability and disclaimer of warranty provisions.

#include "copyright.h"

#include "filehdr.h"
#include "debug.h"
#include "synchdisk.h"
#include "main.h"
#include "pbitmap.h"

//----------------------------------------------------------------------
// IndirectBlock::IndirectBlock()
// 	Constructor
//----------------------------------------------------------------------
IndirectBlock::IndirectBlock()
{
    int index = 0;
    while (index != NumBlock)
    {
        dataSectors[index] = -1;
        index++;
    }
}

//----------------------------------------------------------------------
// IndirectBlock::Allocate
// 	Initialize a fresh IndirectBlock for a newly created file.
//	Allocate data blocks for the file out of the map of free disk blocks.
//	Return FALSE if there are not enough free blocks to accomodate
//	the new file.
//----------------------------------------------------------------------
int
IndirectBlock::Allocate(PersistentBitmap *freeMap, int num_Sectors)
{
    if(num_Sectors < 0 ) return -1;
    if (freeMap->NumClear() < num_Sectors) return -1; // not enough space
    int count = 0;
    for (int i = 0; i < NumBlock; i++)
    {
        if(count >= num_Sectors) break; // finish
        if(dataSectors[i] == -1 )
        {
            dataSectors[i] = freeMap->FindAndSet();
            count++;
        }
        // since we checked that there was enough free space,
        // we expect this to succeed
        ASSERT(dataSectors[i] >= 0);
    }
    return count;
}

//-----------------------------------------------------------------------
// IndirectBlock::Deallocate
// 	De-allocate all the space allocated for data blocks for this file.
//
//	"freeMap" is the bit map of free disk sectors
//----------------------------------------------------------------------

void IndirectBlock::Deallocate(PersistentBitmap *freeMap)
{
    for (int i = 0; i < NumBlock; i++)
    {
        if(dataSectors[i] != -1)
        {
            ASSERT(freeMap->Test((int)dataSectors[i])); // ought to be marked!
            freeMap->Clear((int)dataSectors[i]);
            dataSectors[i] = -1;
        }
    }
}

//----------------------------------------------------------------------
// IndirectBlock::FetchFrom
// 	Fetch contents of file header from disk.
//
//	"sector" is the disk sector containing the file header
//----------------------------------------------------------------------

void IndirectBlock::FetchFrom(int sector)
{
    kernel->synchDisk->ReadSector(sector, (char *)this);
}

//----------------------------------------------------------------------
// IndirectBlock::WriteBack
// 	Write the modified contents of the file header back to disk.
//
//	"sector" is the disk sector to contain the file header
//----------------------------------------------------------------------

void IndirectBlock::WriteBack(int sector)
{
    kernel->synchDisk->WriteSector(sector, (char *)this);
}

//----------------------------------------------------------------------
// IndirectBlock::ByteToSector
// 	Return which disk sector is storing a particular byte within the file.
//      This is essentially a translation from a virtual address (the
//	offset in the file) to a physical address (the sector where the
//	data at the offset is stored).
//
//	"offset" is the location within the file of the byte in question
//----------------------------------------------------------------------

int IndirectBlock::ByteToSector(int offset)
{
    return (dataSectors[offset / SectorSize]);
}







//************************************************************************************
//***********************************************************************************

//----------------------------------------------------------------------
// DoubleIndirectBlock::DoubleIndirectBlock()
// 	Constructor
//----------------------------------------------------------------------
DoubleIndirectBlock::DoubleIndirectBlock()
{
    int index = 0;
    while (index != NumBlock)
    {
        dataSectors[index] = -1;
        index++;
    }
}

//----------------------------------------------------------------------
// DoubleIndirectBlock::Allocate
// 	Initialize a fresh DoubleIndirectBlock for a newly created file.
//	Allocate data blocks for the file out of the map of free disk blocks.
//	Return FALSE if there are not enough free blocks to accomodate
//	the new file.
//----------------------------------------------------------------------
int
DoubleIndirectBlock::Allocate(PersistentBitmap *freeMap, int num_Sectors)
{
    if(num_Sectors < 0 ) return -1;
    if (freeMap->NumClear() < num_Sectors) return -1; // not enough space
    int count = 0;
    for (int i = 0; i < NumBlock; i++)
    {
        if(count >= num_Sectors) break; // finish
        IndirectBlock *tmp = new IndirectBlock(); // Create an IndirectBlock
        if(dataSectors[i] == -1 )
        {
            dataSectors[i] = freeMap->FindAndSet();
        }
        else
        {   
            tmp->FetchFrom(dataSectors[i]);
        }
        // since we checked that there was enough free space,
        // we expect this to succeed
        ASSERT(dataSectors[i] >= 0);

        int allocate_num = num_Sectors-count;
        count += tmp->Allocate(freeMap,allocate_num);
        tmp->WriteBack(dataSectors[i]);
        delete tmp;
    }
    return count;
}

//-----------------------------------------------------------------------
// DoubleIndirectBlock::Deallocate
// 	De-allocate all the space allocated for data blocks for this file.
//
//	"freeMap" is the bit map of free disk sectors
//----------------------------------------------------------------------

void DoubleIndirectBlock::Deallocate(PersistentBitmap *freeMap)
{
    for (int i = 0; i < NumBlock; i++)
    {
        if(dataSectors[i] != -1)
        {
            ASSERT(freeMap->Test((int)dataSectors[i])); // ought to be marked!
            // clear IndirectBlock
            IndirectBlock *tmp = new IndirectBlock();
            tmp->FetchFrom(dataSectors[i]);
            tmp->Deallocate(freeMap);
            freeMap->Clear((int)dataSectors[i]);
            delete tmp;
            dataSectors[i] =-1;
        }
    }
}

//----------------------------------------------------------------------
// DoubleIndirectBlock::FetchFrom
// 	Fetch contents of file header from disk.
//
//	"sector" is the disk sector containing the file header
//----------------------------------------------------------------------

void DoubleIndirectBlock::FetchFrom(int sector)
{
    kernel->synchDisk->ReadSector(sector, (char *)this);
}

//----------------------------------------------------------------------
// DoubleIndirectBlock::WriteBack
// 	Write the modified contents of the file header back to disk.
//
//	"sector" is the disk sector to contain the file header
//----------------------------------------------------------------------

void DoubleIndirectBlock::WriteBack(int sector)
{
    kernel->synchDisk->WriteSector(sector, (char *)this);
}

//----------------------------------------------------------------------
// DoubleIndirectBlock::ByteToSector
// 	Return which disk sector is storing a particular byte within the file.
//      This is essentially a translation from a virtual address (the
//	offset in the file) to a physical address (the sector where the
//	data at the offset is stored).
//
//	"offset" is the location within the file of the byte in question
//----------------------------------------------------------------------

int DoubleIndirectBlock::ByteToSector(int offset)
{
    int index = offset / SectorSize;
    IndirectBlock *tmp = new IndirectBlock();
    tmp->FetchFrom(dataSectors[index/NumBlock]);
    int result = tmp->ByteToSector((index % NumBlock)*SectorSize);
    delete tmp;
    return result;
}


//************************************************************************************
//***********************************************************************************

//----------------------------------------------------------------------
// FileHeader::FileHeader()
// 	Constructor
//----------------------------------------------------------------------
FileHeader::FileHeader()
{ 
    int index = 0;
    while(index != NumDirect)
    {
        dataSectors[index]= -1;
        index++;
    }
    numBytes = 0;
    numSectors = 0;
    isReadable = TRUE;
    isWriteable = TRUE;
    isExcutable = FALSE;
    isRemoved = FALSE;
}

//----------------------------------------------------------------------
// FileHeader::FileHeader(int protection)
// 	Constructor
//----------------------------------------------------------------------
FileHeader::FileHeader(int protection)
{ 
    int index = 0;
    while(index != NumDirect)
    {
        dataSectors[index]= -1;
        index++;
    }
    numBytes = 0;
    numSectors = 0;

    int a[3] = {0,0,0};
    int i = 2;
    while(protection > 0)
    {
        a[i] = protection%2;
        protection /= 2;
        i--;
    }
    isReadable = (bool)a[0];
    isWriteable = (bool)a[1];
    isExcutable = (bool)a[3];
    isRemoved = FALSE;

}

//----------------------------------------------------------------------
// FileHeader::Allocate
// 	Initialize a fresh file header for a newly created file.
//	Allocate data blocks for the file out of the map of free disk blocks.
//	Return FALSE if there are not enough free blocks to accomodate
//	the new file.
//
//	"freeMap" is the bit map of free disk sectors
//	"fileSize" is the bit map of free disk sectors
//----------------------------------------------------------------------

bool
FileHeader::Allocate(PersistentBitmap *freeMap, int fileSize)
{ 
    //numBytes = fileSize;
    int ReqSectors  = divRoundUp(fileSize, SectorSize);
    if (freeMap->NumClear() < ReqSectors)
	return FALSE;		// not enough space

    int count = 0;
    for (int i = 0; i < NumDirect; i++) 
    {
        if(count >= ReqSectors) break; // finish
        
        if(i== NumDirect-2)
        {
            IndirectBlock *tmp = new IndirectBlock(); // Create an IndirectBlock
            if(dataSectors[i] == -1 )
            {
                dataSectors[i] = freeMap->FindAndSet();
            }
            else
            {   
                tmp->FetchFrom(dataSectors[i]);
            }
            count += tmp->Allocate(freeMap,ReqSectors-count);
            tmp->WriteBack(dataSectors[i]);
            delete tmp;
            continue;
        }
        if(i == NumDirect-1)
        {
            DoubleIndirectBlock *tmp = new DoubleIndirectBlock(); // Create an IndirectBlock
            if(dataSectors[i] == -1 )
            {
                dataSectors[i] = freeMap->FindAndSet();
            }
            else
            {   
                tmp->FetchFrom(dataSectors[i]);
            }
            count += tmp->Allocate(freeMap,ReqSectors-count);
            tmp->WriteBack(dataSectors[i]);
            delete tmp;
            continue;
        }

        if(dataSectors[i] == -1 )
        {
            dataSectors[i] = freeMap->FindAndSet();
            count++;
        }
    }

    numBytes += fileSize;
    numSectors += count;

    return TRUE;
}



//----------------------------------------------------------------------
// FileHeader::Deallocate
// 	De-allocate all the space allocated for data blocks for this file.
//
//	"freeMap" is the bit map of free disk sectors
//----------------------------------------------------------------------

void 
FileHeader::Deallocate(PersistentBitmap *freeMap)
{
    for (int i = 0; i < NumDirect; i++) {

        if(dataSectors[i]!= -1)
        {
            ASSERT(freeMap->Test((int) dataSectors[i]));  // ought to be marked!
            if(i == NumDirect - 2)
            {
                IndirectBlock * tmp = new IndirectBlock();
                tmp->FetchFrom(dataSectors[i]);
                tmp->Deallocate(freeMap);
                delete tmp;
            }
            else if(i == NumDirect - 1)
            {
                DoubleIndirectBlock * tmp = new DoubleIndirectBlock();
                tmp->FetchFrom(dataSectors[i]);
                tmp->Deallocate(freeMap);
                delete tmp;
            }
            freeMap->Clear((int) dataSectors[i]);
            dataSectors[i] = -1;
        }
    }
}

//----------------------------------------------------------------------
// FileHeader::FetchFrom
// 	Fetch contents of file header from disk. 
//
//	"sector" is the disk sector containing the file header
//----------------------------------------------------------------------

void
FileHeader::FetchFrom(int sector)
{
    kernel->synchDisk->ReadSector(sector, (char *)this);
}

//----------------------------------------------------------------------
// FileHeader::WriteBack
// 	Write the modified contents of the file header back to disk. 
//
//	"sector" is the disk sector to contain the file header
//----------------------------------------------------------------------

void
FileHeader::WriteBack(int sector)
{
    kernel->synchDisk->WriteSector(sector, (char *)this); 
}

//----------------------------------------------------------------------
// FileHeader::ByteToSector
// 	Return which disk sector is storing a particular byte within the file.
//      This is essentially a translation from a virtual address (the
//	offset in the file) to a physical address (the sector where the
//	data at the offset is stored).
//
//	"offset" is the location within the file of the byte in question
//----------------------------------------------------------------------

int
FileHeader::ByteToSector(int offset)
{
    int index = offset / SectorSize;
    if (index < NumDirect-2)
    {
        return(dataSectors[offset / SectorSize]);
    }
    if(index < NumDirect-2 + NumBlock)
    {
        IndirectBlock *tmp = new IndirectBlock();
        tmp->FetchFrom(dataSectors[NumDirect-2]);
        int result = tmp->ByteToSector(offset-(NumDirect-2)*SectorSize);
        delete tmp;
        return result;
    }
    
    DoubleIndirectBlock *tmp = new DoubleIndirectBlock();
    tmp->FetchFrom(dataSectors[NumDirect-1]);
    int result = tmp->ByteToSector(offset-(NumDirect-2+NumBlock)*SectorSize);
    delete tmp;
    return result;
   
}

//----------------------------------------------------------------------
// FileHeader::FileLength
// 	Return the number of bytes in the file.
//----------------------------------------------------------------------

int
FileHeader::FileLength()
{
    return numBytes;
}

//----------------------------------------------------------------------
// FileHeader::Print
// 	Print the contents of the file header, and the contents of all
//	the data blocks pointed to by the file header.
//----------------------------------------------------------------------

void
FileHeader::Print()
{
    int i, j, k;
    char *data = new char[SectorSize];

    printf("FileHeader contents.  File size: %d.  File blocks:\n", numBytes);
    for (i = 0; i < numSectors; i++)
	printf("%d ", dataSectors[i]);
    printf("\nFile contents:\n");
    for (i = k = 0; i < numSectors; i++) {
	kernel->synchDisk->ReadSector(dataSectors[i], data);
        for (j = 0; (j < SectorSize) && (k < numBytes); j++, k++) {
	    if ('\040' <= data[j] && data[j] <= '\176')   // isprint(data[j])
		printf("%c", data[j]);
            else
		printf("\\%x", (unsigned char)data[j]);
	}
        printf("\n"); 
    }
    delete [] data;
}
