#include<errno.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <time.h>
#include <string.h>
#include <stdlib.h>

typedef enum {TYPE_ORDINARY, TYPE_DIRECTORY} file_type;
#define BLKSIZE 4096
#define NO_BLKS 230
#define BLK_LIMIT 230

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
	int inodo;
	int nodoPadre;
	int signodo;
	int cabezaNodoHijo;
	int nodoHijo;
}NODE;

typedef struct inode
{
	int inodo;
	file_type tipo;
	mode_t mode;
	int nlink;
	uid_t uid;
	gid_t gid;
	int no_blocks;
	int tamanioArchivo;
	time_t ultimoArchivoModificado;
	time_t ultimoArchivoAccesado;
	time_t ultimoInodoModificado;
	BLOCK *bp[BLK_LIMIT];
}INODE;

typedef struct block
{
  int nodoBlk;
  char blk[BLKSIZE];
  int fin;
  INODE *i;
}BLOCK;

typedef struct inode_bitmap
{
  int inode_no;
  int revisar;
}IBMAP;

typedef struct data_bitmap
{
  int nodoBlk;
  int revisar;
}DBMAP;

typedef struct node_bitmap
{
	int node_no;
	int revisar;
}NBMAP;

//TREE FUNCTIONS
void crearArbol();
int crearRaiz();
int hacerNodo(const char *ruta,int rootno,struct stat *s,file_type ftype);
int buscarNodo(const char *ruta,int rootno);
int eliminarNodo(const char *ruta,int rootno);
void parsearRuta(const char* ruta,char *directory,char *name);
void imprimirArbol(int rootno);
void llenarData();

//BLOCK FUNCTIONS
int hacerBloque(int inode_no);
int hacerInodo(struct stat *s,file_type ftype);
void iniciarINodoBmap(struct inode_bitmap *ibp);
void iniciarDataBmap(struct data_bitmap *dbp);
void iniciarNodoBmap(NBMAP *nbmap);
int buscarINodoVacio(struct inode_bitmap *ibp);
int buscarBloqueVacio(struct data_bitmap *dbp);
int buscarNodoVacio(NBMAP *nbmap);
void limpiarInfoINodo();
#define log_struct(st, field, format, typecast) \
  log_msg("    " #field " = " #format "\n", typecast st->field)
//GLOBAL VARIABLES STRUCTURE
typedef struct discoDuro
{
	INODE inode[NO_BLKS];
	BLOCK block[NO_BLKS];
	IBMAP ibmap[NO_BLKS];
	DBMAP dbmap[NO_BLKS];
	NBMAP nbmap[NO_BLKS];
	NODE node[NO_BLKS];
	TREE t;
}DISCODURO;


