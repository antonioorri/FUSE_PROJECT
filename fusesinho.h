

#include <fuse.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <time.h>
#include <string.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <errno.h>

#define block_size 1024


typedef struct superbloque {
	char datablocks[block_size*100];		//total number of data blocks
	char data_bitmap[105];      			//array of data block numbers that are available
	char inode_bitmap[105];   				//array of inode numbers that are available
} superbloque;

typedef struct inode {
	int datablocks[16];            //data block number that the inode points to
	int number;
	int blocks;                    //==number of blocks the particular inode points to
	//int link;                    //==number of links
	int size;                      //==size of file/directory
} inode;

typedef struct filetype {
	int valid;
	char test[10];
	char path[100];
	char name[100];           //name
	inode *inum;              //inode number
	struct filetype ** children;
	int num_children;
	int num_links;
	struct filetype * parent;
	char type[20];                  //==file extension
	mode_t permissions;		        // Permissions
	uid_t user_id;		            // userid
	gid_t group_id;		            // groupid
	time_t a_time;                  // Access time
	time_t m_time;                  // Modified time
	time_t c_time;                  // Status change time
	time_t b_time;                  // Creation time
	off_t size;                     // Size of the node

	int datablocks[16];
	int number;
	int blocks;

} filetype;
