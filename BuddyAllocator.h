#ifndef _BuddyAllocator_h_                   // include file only once
#define _BuddyAllocator_h_
#include <iostream>
#include <math.h>
#include <vector>
using namespace std;
typedef unsigned int uint;

/* declare types as you need */


struct BlockHeader{
	int blockSize;
	bool free;
	BlockHeader* next;

};

struct LinkedList{
	// this is a special linked list that is made out of BlockHeader s.
	BlockHeader* head;
	BlockHeader* tail;
	int size = 0;
	void insert (BlockHeader* b){	// adds a block to the list
		if (size < 1){
			head = b;
			tail = b;
			b->next = NULL;
			size++;
		}else{
			b->next = head;
			head = b;
			size++;
		}
	}

	void remove (BlockHeader* b){
		BlockHeader* curr = head;

		//if b is head
		if (head == b){
			if (head->next == NULL){
				head = NULL;
				tail = NULL;
				size --;
				return;
			}

			head = b->next;
			size --;
			return;
		}
		else{
			//find node before deletion
			while(curr->next != b){
				curr = curr->next;
			}

			//if next node is tail, delete tail assuming a node must be deleted
			if(curr->next == tail){
				curr->next = NULL;
				tail = curr;
				size --;
				return;
			}

			curr->next  = curr->next->next;
			size --;
		}

	}

	void print(){
		BlockHeader* curr = head;
		cout << "->";
		if(size > 0){
			while(curr != NULL){
				if(curr->free == true) cout<<"| " << curr->blockSize << " "<< curr <<" | ->";
				else cout<<"|* " << curr->blockSize << " *| ->";
				curr = curr->next;
			}
			cout << endl;
		}else
			cout << endl;
	}
};


class BuddyAllocator{
private:
	/* declare member variables as necessary */
	char* startingPointer;
	int basicBlockSize;
	int totalMemoryLength;
	int freeSize;
	vector<LinkedList> freelist;


private:

	BlockHeader* getbuddy (BlockHeader* addr);

	bool arebuddies (BlockHeader* block1, BlockHeader* block2);

	BlockHeader* merge (BlockHeader* block1, BlockHeader* block2);

	BlockHeader* split (BlockHeader* block);

	int listPosition(BlockHeader* b);

public:
	 BuddyAllocator (uint _basic_block_size, uint _total_memory_length);

	~BuddyAllocator();
	
	char* alloc(uint _length);

	int free(char* _a);

	void debug ();

};

#endif
