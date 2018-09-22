
#include "BuddyAllocator.h"
#include <iostream>
using namespace std;

BuddyAllocator::BuddyAllocator(uint _basic_block_size, uint _total_memory_length){
	/* This initializes the memory allocator and makes a portion of
	   ’_total_memory_length’ bytes available. The allocator uses a ’_basic_block_size’ as
	   its minimal unit of allocation. The function returns the amount of
	   memory made available to the allocator. If an error occurred,
	   it returns 0.
	*/

	//save user input
	totalMemoryLength = _total_memory_length;
	basicBlockSize = _basic_block_size;

	//find starting pointer and initialize freelist
	startingPointer = new char[_total_memory_length];
	freeSize = log2(_total_memory_length) - log2(_basic_block_size) + 1;
	freelist.resize(freeSize);

	freelist[0].insert((BlockHeader*)startingPointer);
	freelist[0].head->blockSize = _total_memory_length;
	freelist[0].head->free = true;

}

BuddyAllocator::~BuddyAllocator (){
	delete [] startingPointer;
}

char* BuddyAllocator::alloc(uint _length) {
  /* This preliminary implementation simply hands the call over the
     the C standard library!
     Of course this needs to be replaced by your implementation.
  */
	LinkedList minList;

	//if memory asked for is simply more than max, return null
 	if(_length > totalMemoryLength - sizeof(BlockHeader)) return NULL;

	//finds min available block
	for(int i = 0; i < freelist.size(); i++){
		if(freelist[i].head != NULL && freelist[i].head->blockSize - sizeof(BlockHeader) >= _length) minList = freelist[i];
	}

	//no free space available for allocation
	if (minList.size == 0) return NULL;

	BlockHeader* returnPointer = minList.head;

	//split free block until smallest unit possible is achieved
	while(returnPointer->blockSize/2 - sizeof(BlockHeader) > _length && returnPointer->blockSize/2 >= basicBlockSize){
		returnPointer = split(returnPointer);
		// debug();
	}

	//set block to be allocated
	returnPointer->free = false;
	freelist[listPosition(returnPointer)].remove(returnPointer);


	//return pointer offset by header size
	returnPointer = (BlockHeader*)((char*)returnPointer + (sizeof(BlockHeader)));

	return (char*)returnPointer;

}

int BuddyAllocator::free(char* _a) {
  /* Same here! */

	BlockHeader* toFree = (BlockHeader*)(_a - sizeof(BlockHeader));

	//set block to be free
	toFree->free = true;
	freelist[listPosition(toFree)].insert(toFree);


	while (arebuddies(toFree, getbuddy(toFree))){
		toFree = merge(toFree, getbuddy(toFree));
	}

  return 0;
}

BlockHeader* BuddyAllocator::getbuddy(BlockHeader *addr){
	// given a block address, this function returns the address of its buddy
		long int address = (long int)addr;
		// cout << "here!" << endl;
		 long int start = (long int)startingPointer;
		// long int size = addr->blockSize;
		//
		long int left = address - start;
		// long int right = size + start;
		long int buddy = (left ^ addr->blockSize ) + start;
		//cout << "Budds: " << (BlockHeader*) buddy << endl << (BlockHeader*) addr << endl;
		return (BlockHeader*)((char*)buddy);
}

bool BuddyAllocator::arebuddies(BlockHeader *block1, BlockHeader *block2){
	// checks whether the two blocks are buddies are not
	return getbuddy(block1) == block1->next || getbuddy(block2) == block2->next;
}

int BuddyAllocator::listPosition(BlockHeader *b){
	//helper function
	return log2(totalMemoryLength) - log2(b->blockSize);
}

BlockHeader* BuddyAllocator::merge (BlockHeader* block1, BlockHeader* block2){
	// this function merges the two blocks returns the beginning address of the merged block
	// note that either block1 can be to the left of block2, or the other way around
	BlockHeader* beginPointer;
	int previousSize = block1->blockSize;

	//check which block is left or right
	if(block1 > block2) beginPointer = block2;
	else beginPointer = block1;
	int position = listPosition(block1);


	//remove block 1 and 2 from free list.
	freelist[position].remove(block1);
	freelist[position].remove(block2);

	//clear memory chunk
	memset(beginPointer, 0, previousSize*2);

	//add merged block to freelist
	freelist[position-1].insert(beginPointer);

	//update header
	beginPointer->blockSize = previousSize*2;
	beginPointer->free = true;

	return beginPointer;

}

BlockHeader* BuddyAllocator::split (BlockHeader* block){
	// splits the given block by putting a new header halfway through the block
	// also, the original header needs to be corrected

	int previousSize = block->blockSize;
	BlockHeader* splitPointer = (BlockHeader*)((char*)block+(block->blockSize/2));

	//find position in Free list
	int position = listPosition(block);
	freelist[position].remove(block);

	//clear memory
	memset(block, 0, previousSize);

	//second new header
	freelist[position+1].insert(splitPointer);
	block->blockSize = previousSize/2;
	block->free = true;

	//second new header
	freelist[position+1].insert(block);
	splitPointer->blockSize = previousSize/2;
	splitPointer->free = true;
	block->next = splitPointer;


	return block;

}

void BuddyAllocator::debug(){
	for(int i=0; i < freelist.size(); i++){
		cout << pow(2,log2(basicBlockSize)+i) << ": "<< freelist[freeSize-(i+1)].size << endl;
	}
}
