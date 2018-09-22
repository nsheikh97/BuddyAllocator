#include "Ackerman.h"
#include "BuddyAllocator.h"
#include "getopt.h"

int main(int argc, char ** argv) {

    int basic_block_size = 128, memory_length = 1024*512;

    //parsing command line argument for arguments using getopt()
    int c ;
    while( ( c = getopt (argc, argv, "b:s:") ) != -1 )
    {
        switch(c)
        {
            case 'b':
            if(optarg) basic_block_size = atoi(optarg);
            break;
            case 's':
            if(optarg) memory_length = atoi(optarg);
            break;
        }
    }

    //debug
    cout << basic_block_size << endl;
    cout << memory_length << endl;

    // create memory manager
    BuddyAllocator * allocator = new BuddyAllocator(basic_block_size, memory_length);

    //run ackerman
    Ackerman* am = new Ackerman ();
    am->test(allocator); // this is the full-fledged test.
    allocator->debug();
    // destroy memory manager
    delete allocator;
    delete am;
}
