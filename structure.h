#include<errno.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <time.h>
#include <string.h>
#include <stdlib.h>

typedef enum {TYPE_ORDINARY, TYPE_DIRECTORY} file_type;
#define BLKSIZE 512
#define NO_BLKS 20
#define BLK_LIMIT 10

//STRUCUTRES
typedef struct tree
{
	int rootno;
}TREE;
struct block;
typedef struct block BLOCK;
typedef struct node
{
	char path_name[100];
	int ino;
	int parentno;
	int nextno;
	int headchildno;
	int no_children;
}NODE;

typedef struct inode
{
	int ino;
	file_type type;
	mode_t mode;
	int nlink;
	uid_t uid;
	gid_t gid;
	int no_blocks;
	int filesize;
	time_t last_modified_file;
	time_t last_accessed_file;
	time_t last_modified_inode;
	BLOCK *bp[BLK_LIMIT];
}INODE;

typedef struct block
{
  int blk_no;
  char blk[BLKSIZE];
  int end;
  INODE *i;
}BLOCK;

typedef struct inode_bitmap
{
  int inode_no;
  int check;
}IBMAP;

typedef struct data_bitmap
{
  int blk_no;
  int check;
}DBMAP;

typedef struct node_bitmap
{
	int node_no;
	int check;
}NBMAP;

//TREE FUCNTIONS
void create_tree();
int create_root();
int make_node(const char *path,int rootno,struct stat *s,file_type ftype);
int search_node(const char *path,int rootno);
int rem_node(const char *path,int rootno);
void parse_path(const char* path,char *directory,char *name);
void print_tree(int rootno);
void fillData();

//BLOCK FUCNTIONS
int make_blk(int inode_no);
int make_inode(struct stat *s,file_type ftype);
void intialize_inodebmap(struct inode_bitmap *ibp);
void intialize_databmap(struct data_bitmap *dbp);
void intialize_nodebmap(NBMAP *nbmap);
int find_empty_inode(struct inode_bitmap *ibp);
int find_empty_block(struct data_bitmap *dbp);
int find_empty_node(NBMAP *nbmap);
void cleanINodeData();

//GLOBAL VARIABLES STRUCTURE
typedef struct hardisk
{
	INODE inode[NO_BLKS];
	BLOCK block[NO_BLKS];
	IBMAP ibmap[NO_BLKS];
	DBMAP dbmap[NO_BLKS];
	NBMAP nbmap[NO_BLKS];
	NODE node[NO_BLKS];
	TREE t;
}HARDDISK;

HARDDISK HDD;
