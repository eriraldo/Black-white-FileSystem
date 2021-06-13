#include<errno.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <time.h>
#include <string.h>
#include <stdlib.h>

typedef enum {TYPE_ORDINARY, TYPE_DIRECTORY} file_type;
#define BLKSIZE 8192
#define NO_BLKS 150
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
void crearArbol();
int crearRaiz();
int hacerNodo(const char *path,int rootno,struct stat *s,file_type ftype);
int buscarNodo(const char *path,int rootno);
int eliminarNodo(const char *path,int rootno);
void parsearRuta(const char* path,char *directory,char *name);
void imprimirArbol(int rootno);
void llenarData();

//BLOCK FUCNTIONS
int hacerBloque(int inode_no);
int hacerInodo(struct stat *s,file_type ftype);
void iniciarINodoBmap(struct inode_bitmap *ibp);
void iniciarDataBmap(struct data_bitmap *dbp);
void iniciarNodoBmap(NBMAP *nbmap);
int buscarINodoVacio(struct inode_bitmap *ibp);
int buscarBloqueVacio(struct data_bitmap *dbp);
int buscarNodoVacio(NBMAP *nbmap);
void limpiarInfoINodo();

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


