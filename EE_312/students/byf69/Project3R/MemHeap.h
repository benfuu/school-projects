#ifndef _MemHeap_h
#define _MemHeap_h 1

void* allocateMemory(unsigned size);
int deallocateMemory(void* ptr);
void displayChunks(void);
bool isEmptyHeap(void);
bool isSaneHeap(void);
bool isHeapVar(void* ptr);

#define malloc allocateMemory
#define free deallocateMemory

#define STRICT_ENFORCEMENT_OF_CONTRACT  0

#endif /* _MemHeap_h */
