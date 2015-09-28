#ifndef _MemHeap_h
#define _MemHeap_h 1
#include <stddef.h>

#ifdef _MSC_VER
#define noexcept(x) 
#endif /* _MSC_VER */

void initializeHeap(void);
void* allocateMemory(unsigned size);
void* reallocateMemory(void *old, unsigned size);
void deallocateMemory(void* ptr);
void displayChunks(void);
bool isEmptyHeap(void);
bool isSaneHeap(void);

//#define malloc allocateMemory
//#define free deallocateMemory
//#define realloc reallocateMemory

inline void* operator new(size_t sz) { return allocateMemory(sz); }
inline void* operator new [](size_t sz) { return allocateMemory(sz); }
inline void operator delete(void* p) noexcept(true) { deallocateMemory(p); }
inline void operator delete [](void* p) noexcept(true) { deallocateMemory(p); }

#undef noexcept

#endif /* _MemHeap_h */
