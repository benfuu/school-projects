#include <stdio.h> // declarations of printf
#include <assert.h> // declaration of assert (well, assert is a macro, so really this is the definition of assert)
#include <stdlib.h> // declaration of abort() and exit()
#include "MemHeap.h"

#define pool_size 1000
const unsigned bogus_chunk = -1;
#define true 1 // I don't like using "1" for true, I like "true"
#define false 0 // ditto for "false"
int always_false = false; // This is a debugging trick.  If you want debugging information, set this variable to true inside the debugger.

/*
 * the minimum chunk size is the smallest chunk worth having
 * NOTE: the min_chunk_size INCLUDES the space for the signatures
 * three is used currently since that would leave 1 element of 
 * available memory
 */
const unsigned min_chunk_size = 3;

int memory_pool[pool_size];

/*
 * given the index of the beginning of a chunk (bottom signature)
 * return the index of the top signature for the same chunk 
 */
unsigned topSignature(unsigned chunk) {
	unsigned first_available;
	unsigned end_signature;
	int size = memory_pool[chunk];
	
	if (size < 0) { size = -size; }
	first_available = chunk + 1;
	end_signature = first_available + size;
	return end_signature;
}

/*
 * given the index of the end of a chunk (top signature)
 * return the index of the bottom signature for the same chunk
 */
unsigned bottomSignature(unsigned chunk) {
	int size = memory_pool[chunk];
	unsigned last_available = chunk - 1;
	unsigned start_signature;

	if (size < 0) { size = -size; }
	start_signature = last_available - size;
	return start_signature;
}


void initializeHeap(void) {
	/* each chunk requires two signatures, one at the top and one
	 * at the bottom, so the available space inside a chunk is 
	 * the number of elements in the chunk minus 2
	 */
	unsigned available_size = pool_size - 2;
	
	/* write signatures at top and bottom of chunk */
	memory_pool[0] = available_size;
	memory_pool[pool_size - 1] = available_size;
}

/* 
 * given the index of the beginning of a chunk (bottom signature)
 * return the index of the beginning of the next chunk (bottom signature)
 */
unsigned successorChunk(unsigned chunk) {
	unsigned first_available;
	unsigned end_signature;
	unsigned next_chunk;
	int size = memory_pool[chunk];

	if (size < 0) { size = -size; }
	
	first_available = chunk + 1; 
	end_signature = first_available + size;
	next_chunk = end_signature + 1;
	return next_chunk;
}


/*
 * given the index of the beginning of a chunk (bottom signature)
 * return the index of the bottom signature of the preceeding chunk
 */
unsigned predecessorChunk(unsigned chunk) {
	unsigned end_signature = chunk - 1; // end signature of preceeding chunk
	unsigned start_signature = bottomSignature(end_signature);
	return start_signature;
}

/* 
 * set both the signatures for a chunk to a particular value
 * 'chunk' must be the index of the bottom signature for a chunk
 * NOTE: if you change the size of this chunk with this function
 * then we'll set the end signature for the NEW SIZE!
 */
void setBothSignatures(unsigned chunk, int value) {
	unsigned top;
	
	memory_pool[chunk] = value;
	top = topSignature(chunk);
	memory_pool[top] = value;
}

/* return the index of the start of a free chunk 
 * 1. actually, we return the index of the signature at the start
 * of the chunk, and 
 * 2. the chunk have at least 'size' available ints
 * 3. if no such chunk exists, return bogus_chunk
 */
unsigned findFreeChunk(int size) {
	unsigned next_chunk = 0;
	while (next_chunk < pool_size && memory_pool[next_chunk] < size) {
		/* this chunk is too small (or in use), try next chunk */
		next_chunk = successorChunk(next_chunk);
	}
	
	/* we can exit the loop for one of two reasons
	 * either we found a chunk that was big enough, or
	 * we checked all the chunks and didn't find anything
	 */
	if (next_chunk >= pool_size) { // uh oh! gone too far, give up
		return bogus_chunk;
	} else {
		return next_chunk;
	}
}

/*
 * Check the chunk to see if it's bigger than it needs to be
 * split the chunk in two if it's larger than size
 * be sure that the "excess" (i.e., the new chunk) is at the
 * top of the current chunk.  i.e., the new signature
 * written at memory_pool['chunk'] must be at least 'size'
 */
void splitChunkIfNecessary(unsigned chunk, int size) {
	if (memory_pool[chunk] > size + min_chunk_size) {
		/* need to split... */
		int left_over_sig = memory_pool[chunk] - size - 2;
			// left_over_sig is the signature for the excess chunk
		unsigned left_over_chunk = chunk + size + 2;
			// left_over_chunk is the index of the new chunk we'll create
		
		setBothSignatures(chunk, size);
		setBothSignatures(left_over_chunk, left_over_sig);
	}
	
	/* that's it.  If the chunk's too small, there's nothing to do
	 * so we don't need an else or nothin */
}
		
bool initialized = false;
void* allocateMemory(unsigned size_in_bytes) {
	int size;
	unsigned chunk;
	int chunk_size;
	
	if (!initialized) { 
		initializeHeap();
		initialized = true;
	}
	
	size = (size_in_bytes + sizeof(int) - 1) / sizeof(int);
	if (size == 0) { size = 1; }
	
	chunk = findFreeChunk(size);
	if (always_false) {
		displayChunks();
	}
	if (chunk == bogus_chunk) {
		printf("Ah NUTS, out of memory\n");
		exit(-1); // forces main to return -1 !immediately!
	}
	
	splitChunkIfNecessary(chunk, size);
	chunk_size = memory_pool[chunk];
	assert(chunk_size >= size); // just a sanity check, pretty insane really
	setBothSignatures(chunk, -chunk_size);
	
  /* OK, all done... all we need to do now is return... oh,
   * we're supposed to return a pointer, not an index.....
   */
	return &memory_pool[chunk + 1];
}

/*
 * check to see if the following chunk is available, if it is
 * then merge this chunk with the following chunk
 */
void mergeChunkIfOK(unsigned chunk) {
	unsigned next_chunk;
	int next_chunk_size;
	int size = memory_pool[chunk];

	if (size < 0) { return; } // not OK, this one is busy!
	next_chunk = successorChunk(chunk);
	if (next_chunk >= pool_size) { // No good, there is no successor!
		return; // give up now
	}

	next_chunk_size = memory_pool[next_chunk];

	if (next_chunk_size > 0) { // it's available!
		int new_size = size + next_chunk_size + 2;
		setBothSignatures(chunk, new_size);
	}
	/* that's it. we can leave the old signatures in the middle
	 * after all, we'll never see them */
}

void deallocateMemory(void* p) {
	/* since the available memory is one position above the signature
	 * the signature can be found at one position below the first
	 * byte of available memory 
	 */
	if (p == 0) { return; } // special case, deallocating a null pointer is a no-op

	int* chunk_address = ((int*)p) - 1;
	unsigned chunk;
	int top_signature;
	int bottom_signature;
	int size;
	
	if ((chunk_address < memory_pool) 
		|| (chunk_address >= &memory_pool[pool_size])) {
		/* due to silliness in the MetroWerks compiler, it is possible
		 * for new to be called before the memory heap is on-line.  In that
		 * case the system default heap is used, and not this one.
		 * this generally only happens for global constructors, such as
		 * the stuff associated with cout and cin.
		 * When these objects are destroyed (after main ends) the
		 * runtime system calls delete on all these things but
		 * since our heap is now online, we get all the other heap's memory
		 * back.  We're just going to ignore it.  The OS will clean up
		 * that memory anyway when the application quits, so its
		 * no big deal.
		 */
		 
		 /* if you want to see this in action, de-comment the following line */
		// cout << "hey! that ain't mine, don't throw that junk at me\n";
		return;
	}
	chunk = chunk_address - memory_pool;
	
	bottom_signature = memory_pool[chunk];
	top_signature = memory_pool[topSignature(chunk)];
	
	/* another sanity check, not really insane at all. */
	assert(bottom_signature == top_signature 
		&& bottom_signature < 0);
		
	size = -bottom_signature;
	setBothSignatures(chunk, size);
			
	mergeChunkIfOK(chunk);
	
	if (chunk > 0) { // this is not quite silly, chunk could be equal to zero
		unsigned previous_chunk = predecessorChunk(chunk);
		mergeChunkIfOK(previous_chunk);
	}
}

	

void displayChunks(void) {
	int size;
	int num_chunks = 0;
	unsigned chunk = 0;
	int top_signature;

	while (chunk < pool_size) {
		num_chunks += 1;
		size = memory_pool[chunk];
		printf("chunk %d ", num_chunks);
		top_signature = memory_pool[topSignature(chunk)];
		if (size != top_signature) {
			printf(" has a signature mismatch problem! oh fudge\n");
			exit(-1);
		}
		if (size > 0) {
			printf(" is available");
		} else {
			printf(" is in use");
		}
		if (size < 0) { size = -size; }
		printf(" and has %d words of available storage\n", size);
		
		
		chunk = successorChunk(chunk);
	}
}

/* return true if and only if the heap is empty (no in-use chunks) */
bool isEmptyHeap(void) {
	unsigned chunk = 0;
	unsigned next = successorChunk(chunk);
	
	if ((memory_pool[chunk] > 0) && (next == pool_size)) {
		return true;
	} else {
		return false;
	}
}

/*
 * sanity check the heap and return true if it looks OK,
 * return false if something is messed up
 */
bool isSaneHeap(void) {
	unsigned chunk = 0;
	
	/* check each chunk to see if top and bottom sigs match */
	while (chunk < pool_size) {
		if (memory_pool[chunk] != memory_pool[topSignature(chunk)]) {
			return false;
		}
		chunk = successorChunk(chunk);
	}
	
	return true;
}
