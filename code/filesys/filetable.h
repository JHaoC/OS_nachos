#ifndef FILETABLE_H
#define FILETABLE_H
//------------------------------------------------------------------------------------------
// Reference https://github.com/RyanBeatty/CS420-FileSystems
//
// Defines data structures to manage references to open files and file descriptors
//	OpenFileIdTable:
//
//		- an system-wide array of OpenFile objects that can be accessed by processes for
//		  reading and writing
//
//	Table:
//		- A simple List of Entries containing references to OpenFileObjects.
//
//	FileEntry:
//		- an Entry that has an OpenFile pointer and reference count of the amount of
//		  references that exist to that OpenFile
//
//	FileVector:
//		- a per process vector that maps OpenFileId's for a single process
// 		  to the global system wide OpenFileTable 
//

#include "../userprog/syscall.h"
#include "openfile.h"
#include "synch.h"

#define OPEN_FILE_NUM 20
#define FREE_SPACE NULL


// stores an OpenFile object and a reference count of all
// file descriptors to that file
class FileEntry {
	public:
		FileEntry(OpenFile *f);
		~FileEntry();
		OpenFile *GetFile();
		int GetCount();
		void IncrementCount();
		void DecrementCount();
		Lock *readLock;
		Lock *writeLock;
		int readCounter;

	private:
		OpenFileId fileId;
		int count;
		
};

// List of file Entry
class FileEntryTable {
	public:
		FileEntryTable();
		~FileEntryTable();

		FileEntry *ReadEntry(OpenFileId id);
		void InsertEntry(OpenFile *f);  
		int DecreasEntry(OpenFileId id); 
		int RemoveEntry(OpenFileId id);  // reduce count and delete if count == 0
        //void ExpandFileTable();         
        void AddReference(OpenFileId id);
		//int Length();
	private:
		FileEntry **fileEntryTable; //entrypointer array, num of
		//int length;
};


// a per process vector that maps OpenFileId's for a single process
// to the global system wide OpenFileTable
class OpenFileTable {
	public:
		OpenFileTable();
		OpenFileTable(OpenFileTable *parent);		// constructor used when sharing file descriptors when Exec()-ing
		~OpenFileTable();
        void ExpandFileTable();  
		OpenFileId Insert(OpenFile *f);		// creates new file descriptor to an OpenFile
		OpenFile *Resolve(OpenFileId id);	// resolve the passed in OpenFileId to the OpenFile object it points to
		int Remove(OpenFileId id);			// decrement reference, 0 should be clear, -1 no file;
        int Length();

	private:
		OpenFile **openfiles;			// list of OpenfileId's that correspond to Openfiles in the system-wide file table
		int length;						// length of per process file vector
};

#endif // FILETABLE_H