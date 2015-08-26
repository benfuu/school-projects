#ifndef _MemHeap_h
#define _MemHeap_h 1

void initializeHeap(void);
void* allocateMemory(unsigned size);
void deallocateMemory(void* ptr);
void displayChunks(void);
bool isEmptyHeap(void);
bool isSaneHeap(void);

#define malloc allocateMemory
#define free deallocateMemory

#endif /* _MemHeap_h */
