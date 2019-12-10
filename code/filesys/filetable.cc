#include "filetable.h"
#include "../threads/main.h"
#include "../userprog/syscall.h"
#include "synch.h"

//-----------------------------------------------------------
// FileEntry::FileEntry()
//	create a new FileEntry object. Stores a reference to the
//	OpenFile and a reference count to that file
//-----------------------------------------------------------
FileEntry::FileEntry(OpenFile *f) {
	fileId = f->GetOpenFileID();
	count = 1;
	readLock = new Lock("readLock");
	writeLock = new Lock("writeLock");
	readCounter = 0;

}

//-----------------------------------------------------------
// FileEntry::~FileEntry()
//	destructor
//-----------------------------------------------------------
FileEntry::~FileEntry() {
	delete readLock;
	delete writeLock;
}

//-----------------------------------------------------------
// FileEntry::GetFile()
//	return OpenFile
//-----------------------------------------------------------
OpenFile *
FileEntry::GetFile() {
	OpenFile * res = new OpenFile(fileId);
	return res;
}

//-----------------------------------------------------------
// FileEntry::GetCount()
//	return count
//-----------------------------------------------------------
int
FileEntry::GetCount() {
	return count;
}

//-----------------------------------------------------------
// FileEntry::IncrementCount()
//	increment count
//-----------------------------------------------------------
void
FileEntry::IncrementCount() {
	count++;
}

//-----------------------------------------------------------
// FileEntry::DecrementCount()
//	decrement count
//-----------------------------------------------------------
void
FileEntry::DecrementCount() {
	count--;
}

//######################################################################################


//-----------------------------------------------------------
// FileEntryTable::FileEntryTable()
//	create a new FileEntryTable object. Initialize array of OpenFile objects
//-----------------------------------------------------------
FileEntryTable::FileEntryTable() {
	fileEntryTable = new FileEntry*[NumSectors];
	for(int i=0; i<NumSectors; i++)
	{
		fileEntryTable[i]=FREE_SPACE;
	}			
}

//-----------------------------------------------------------
// FileEntryTable::~FileEntryTable()
//	destructor
//-----------------------------------------------------------
FileEntryTable::~FileEntryTable() {
	delete fileEntryTable;
}

// //-----------------------------------------------------------
// // FileEntryTable::ExpandFileTable()
// //	double table size
// //-----------------------------------------------------------
// void 
// FileEntryTable::ExpandFileTable() {
    
//     FileEntry ** newtable = new FileEntry*[length*2];

//     for(int i=0; i < length; i++)
//     {
//         newtable[i] = fileEntryTable[i];
//     }
//     delete fileEntryTable;
//     fileEntryTable = newtable;
//     length *= 2;
// }

//-----------------------------------------------------------
// FileEntryTable::ReadEntry()
//	Return the Openfile object that maps to the passed in OpenFileId
//	NOTE: id must be in the allowed range
//-----------------------------------------------------------
FileEntry *
FileEntryTable::ReadEntry(OpenFileId id) {
	//ASSERT(id >= 0 && id < length);
	return fileEntryTable[id]; 
}

//-----------------------------------------------------------
// FileEntryTable::InsertEntry()
//	Insert Entry if does not existed, otherwise adding count
//-----------------------------------------------------------
void 
FileEntryTable::InsertEntry(OpenFile *f) {

	int fileId = f->GetOpenFileID();
	if(fileId == -1) return;
	if(ReadEntry(fileId)== NULL)
	{
		fileEntryTable[fileId] = new FileEntry(f);
	}
	else
	{
		fileEntryTable[fileId]->IncrementCount();
	}
	
}


//-----------------------------------------------------------
// FileEntryTable::DecreasEntry()
//	reduce the count 
//-----------------------------------------------------------
int
FileEntryTable::DecreasEntry(OpenFileId id) {
	if(id == -1) return -1;
	FileEntry *entry = fileEntryTable[id];
	if(entry == FREE_SPACE) return -1;
    entry->DecrementCount();
	return entry->GetCount();
}


//-----------------------------------------------------------
// FileEntryTable::RemoveEntry()
//	reduce the count and deletes an entry in the table if count
//  is 0
//  FileSystem remove should use it to check the file
//-----------------------------------------------------------
int
FileEntryTable::RemoveEntry(OpenFileId id)
{
	int res = DecreasEntry(id);
    if(res <= 0)
    {
	    delete fileEntryTable[id];
		fileEntryTable[id]= FREE_SPACE;
    }
	return res;
}


//-----------------------------------------------------------
// FileEntryTable::AddReference()
//	Increment reference count of the passed in OpenFileId 
//-----------------------------------------------------------
void
FileEntryTable::AddReference(OpenFileId id) 
{
	FileEntry *entry = ReadEntry(id);			// get TableEntry
	if(entry == NULL)		// entry does not exist
		return;
	entry->IncrementCount();			// increment reference count of OpenFile object
}

// //-----------------------------------------------------------
// // FileEntryTable::Length()
// //	Return length
// //-----------------------------------------------------------
// int 
// FileEntryTable::Length() {
// 	return length;
// }

//######################################################################################

//-----------------------------------------------------------
// OpenFileTable::OpenFileTable()
//	constructor for a per process FileVector with no sharing
//-----------------------------------------------------------
OpenFileTable::OpenFileTable() {
	length = OPEN_FILE_NUM;
	openfiles = new OpenFile*[length];

	openfiles[0] = (OpenFile*)ConsoleInputId;						// set descriptor 0 to be ConsoleInput
	openfiles[1] = (OpenFile*)ConsoleOutputId;					// set descriptor 1 to be ConsoleOutput
	kernel->globalFileTable->AddReference(ConsoleInputId);		// Add reference to ConsoleInput
	kernel->globalFileTable->AddReference(ConsoleOutputId);		// add reference to ConsoleOutput

	for(int i = 0; i < length; ++i)	// iterate over vector, settting each value to FREE
    {
        openfiles[i] = FREE_SPACE;
    }				
		
}

//-----------------------------------------------------------
// OpenFileTable::OpenFileTable(OpenFileTable *parent)
//	constructor for a per process OpenFileTable with sharing.
//	Takes in the OpenFileTable of the parent process and copies
// everything.
//-----------------------------------------------------------
OpenFileTable::OpenFileTable(OpenFileTable *parent) {
	length = parent->length;
	openfiles = new OpenFile*[length];
	for(int i = 0; i < length; ++i) {		// iterate over parent vector and copy all references
		openfiles[i] = parent->openfiles[i];
		kernel->globalFileTable->InsertEntry(openfiles[i]);		// increment reference count of each openfileid
	}
}


//-----------------------------------------------------------
// OpenFileTable::~OpenFileTable()
//	close all references in the vector
//-----------------------------------------------------------
OpenFileTable::~OpenFileTable() {
	for(int i = 2; i < length; ++i)
    {
		if(openfiles[i]==NULL) continue;
		OpenFileId id = (OpenFileId)openfiles[i]->GetOpenFileID();
		kernel->globalFileTable->RemoveEntry(id);
		delete openfiles[i];
    }
	delete openfiles;
}


//-----------------------------------------------------------
// OpenFileTable::ExpandFileTable()
//	double the file table size
//-----------------------------------------------------------
void
OpenFileTable::ExpandFileTable() {
	int newlength = length*2;
	OpenFile **newfiles = new OpenFile*[newlength];
	for(int i = 0; i < newlength; ++i) {
        if(i<length)
        {
            newfiles[i] = openfiles[i];
        }
        else
        {
            newfiles[i] = FREE_SPACE;
        }
	}
	delete openfiles;
	openfiles = newfiles; 
}


//-----------------------------------------------------------
// FileVector::Insert()
//	Creates new file descriptor to the passed in OpenFile
//-----------------------------------------------------------
OpenFileId
OpenFileTable::Insert(OpenFile *f) {
	kernel->globalFileTable->InsertEntry(f);		

	for(int i = 2; i < length; ++i) {			// iterate over file vector, looking for a free spot
		if(openfiles[i] == FREE_SPACE) {			// found free slot
			openfiles[i] = f;				// save global file descriptor
			return f->GetOpenFileID();
		}
	}
    int index = length;
    ExpandFileTable();
    openfiles[index] = f;				
    return f->GetOpenFileID();
}

//-----------------------------------------------------------
// OpenFileTable::Resolve()
//	Returns the Openfile object associated with the passed in
// OpenFileId
//-----------------------------------------------------------
OpenFile *
OpenFileTable::Resolve(OpenFileId id) 
{
	for(int i = 0; i <length; i++)
	{
		if(openfiles[i] == FREE_SPACE) continue;
		if(openfiles[i]->GetOpenFileID()==(int)id)
		{
			return openfiles[i];
		}
	}
	return NULL;
}

//-----------------------------------------------------------
// OpenFileTable::Remove()
//	Remove reference to the file pointed to by the passed in id
//  0 no reference on the file
//  >1 with reference
//  -1 no file at all
//-----------------------------------------------------------
int
OpenFileTable::Remove(OpenFileId id) 
{
	for(int i = 0; i <length; i++)
	{
		if(openfiles[i] == FREE_SPACE) continue;
		if(openfiles[i]->GetOpenFileID()==(int)id)
		{
			OpenFileId id = (OpenFileId)openfiles[i]->GetOpenFileID();
			delete openfiles[i];
			openfiles[i] = FREE_SPACE; // clear spot in local file vector
			return kernel->globalFileTable->DecreasEntry(id);
		}
	}	
	return -1;			
}

