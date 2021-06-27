#define FUSE_USE_VERSION 30
#include<errno.h>
#include <fuse.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <time.h>
#include <string.h>
#include <stdlib.h>
#include "structure.h"
#include "bmpGen.h"
#define MODO 1
#define FILESIZE 100
#define FOLDER 2
#define MOUNTPOINT 3

char* folder;
char* almacenamiento;
extern DISCODURO disco;

//La función do_geattr lo que hace es traer la informacion de la ruta almacena en el arbol con sus  sus atributos
static int do_getattr(const char *ruta,struct stat *s)
{
	printf( "[getattr] Llamado\n" );
	printf("\n\tARBOL:\n");
	printf( "\n\tAtributos de %s solicitados\n", ruta );


	if(strcmp(ruta,"/")==0)
	{
		if(disco.node[disco.t.rootno].inodo == -1)
		{
			int a=hacerInodo(s,TYPE_DIRECTORY);
			disco.node[disco.t.rootno].inodo=a;
			s->st_ino=a; // numero de inodo
		}
		s->st_gid=getgid();// ID del grupo del dueño
		s->st_uid=getuid(); //ID del usuario del dueño
		s->st_atime=s->st_mtime=time(NULL); // hora o tiempo del ultimo acceso y modificación
		s->st_mode=S_IFDIR | 0755; // Tipo de archivo y modo
		s->st_nlink=1; // numero de link
		s->st_blocks=s->st_blksize=s->st_size=0;// tamaño  del bloque
		return 0;
	}
	else if(strcmp(ruta,"/.Trash")!=0 && strcmp(ruta,"/.Trash-1000")!=0 && strcmp(ruta,"/.xdg-volume-info")!=0 && strcmp(ruta,"/autorun.inf")!=0)
	{
		int temp=buscarNodo(ruta,disco.t.rootno);
		if(temp==-1)
		{
			printf("LA RUTA NO EXISTE\n");
			return -ENOENT;
		}
		s->st_ino=disco.node[temp].inodo;
		s->st_gid=disco.inode[disco.node[temp].inodo].gid;
		s->st_uid=disco.inode[disco.node[temp].inodo].uid;
		s->st_atime=s->st_mtime=s->st_ctime=time(NULL);
		s->st_mode=disco.inode[disco.node[temp].inodo].mode;
		s->st_nlink=disco.inode[disco.node[temp].inodo].nlink+disco.node[temp].nodoHijo;
		s->st_blocks=disco.inode[disco.node[temp].inodo].no_blocks;
		s->st_blksize=BLKSIZE;
		s->st_size=disco.inode[disco.node[temp].inodo].tamanioArchivo;//s->st_size=BLKSIZE*disco.inode[disco.node[temp].inodo].no_blocks;
		return 0;
	}
}
//Lo que hace el do_write es escribir en un archivo un string especificado
static int do_write(const char *ruta, const char *buffer, size_t size, off_t offset,struct fuse_file_info *f)
{
    printf( "--> Intentando de escribir %s, %lu, %lu\n", ruta, offset, size );

    int temp = buscarNodo(ruta,disco.t.rootno);offset=0;

    if(temp == -1)
    {
        printf("RUTA INVALIDA\n");
        return -ENOENT;
    }

    int last = (disco.inode[disco.node[temp].inodo].bp[disco.inode[disco.node[temp].inodo].no_blocks-1])->fin;// se utiliza para obtener el fin de un bloque
    int rem_space=BLKSIZE-last; // se obtiene el tamaño restante
    int tempsize=size;
    int written=0;

    char text1[size];
    while(rem_space<tempsize) //si no hay mas espacio en el bloque actual, se crea un bloque mas para poder escribir
    {
        int size1=rem_space;
        tempsize=tempsize-size1;


        memcpy(text1 + (offset) ,buffer+written, size1);
        written=written+size1;

        for(int j=last;j<(last+size1);j++)
            (disco.inode[disco.node[temp].inodo].bp[disco.inode[disco.node[temp].inodo].no_blocks-1])->blk[j]=text1[j-last];

        (disco.inode[disco.node[temp].inodo].bp[disco.inode[disco.node[temp].inodo].no_blocks-1])->fin += size1;

        int inode_no=disco.node[temp].inodo;
        int block_no=hacerBloque(inode_no);
        disco.inode[disco.node[temp].inodo].bp[disco.inode[disco.node[temp].inodo].no_blocks]=&disco.block[block_no];
        disco.inode[inode_no].no_blocks++;

        last = (disco.inode[disco.node[temp].inodo].bp[disco.inode[disco.node[temp].inodo].no_blocks-1])->fin;

        rem_space=BLKSIZE;
    }

    int size1=rem_space;
    char text2[size1];

    memcpy(text2 + (offset) ,buffer+written, size1);

    for(int j=last;j<(last+size1);j++)
        (disco.inode[disco.node[temp].inodo].bp[disco.inode[disco.node[temp].inodo].no_blocks-1])->blk[j]=text2[j-last];

    (disco.inode[disco.node[temp].inodo].bp[disco.inode[disco.node[temp].inodo].no_blocks-1])->fin += size1;

    disco.inode[disco.node[temp].inodo].tamanioArchivo+=size;

    return size;//se retorna el tamaño de lo que se va a escribir
}


// Se intenta de leer la información que contiene un archivo especificado por su ruta
static int do_read(const char *ruta, char *buffer, size_t size,off_t offset, struct fuse_file_info *fi)
{
    size_t size2 = size;
    printf( "--> Intentado de leer %s, %lu, %lu\n", ruta, offset, size );

    int temp = buscarNodo(ruta,disco.t.rootno);// se busca el archivo en el arbol

    if(temp == -1)
    {
        printf("RUTA INVALIDA\n");
        return -ENOENT;
    }
    size_t size1=disco.inode[disco.node[temp].inodo].tamanioArchivo; // se saca el tamaño del archivo

    int number_blocks=disco.inode[disco.node[temp].inodo].no_blocks-1, j;// se obtiene la cantidad de bloques que posee el archivo
    char text [sizeof(DISCODURO)];
    for(j=0;j<=number_blocks;j++)
    {
        strncat(text, (disco.inode[disco.node[temp].inodo].bp[j])->blk ,(disco.inode[disco.node[temp].inodo].bp[j])->fin);
    }

    char* text2 = text;
    if(offset < size1){
        if(offset + size > size1){
            size = size1 - offset;
        }
        memcpy(buffer, text2+offset, size);
    }else{
        size = 0;
        strcpy(buffer,"");
    }



    return size;
}

//Se toma una lista de archivos de la ruta que se especifique, en este caso, un directorio.
static int do_readdir(const char *ruta,void *buffer, fuse_fill_dir_t filler, off_t offset, struct fuse_file_info *f)
{

	printf( "--> Tomando la lista de archivos de la ruta %s\n", ruta ); //

	filler( buffer, ".", NULL, 0 );
	filler( buffer, "..", NULL, 0 );
	printf("%s",disco.node[disco.t.rootno+1].path_name);
	int temp=buscarNodo(ruta,disco.t.rootno);

	if(temp==-1)
	{
		printf("RUTA INVALIDA\n");
		return -ENOENT;
	}

	else
	{
		int i=0;
		int ptr=disco.node[temp].cabezaNodoHijo;
		while(ptr!=-1)//se van sacando cada una de las rutas y se dejan limpias para mostrar nada mas la lista de archivos
		{
			char *d,*name;
			d=(char*)calloc(strlen(ruta)+1,sizeof(char));
			name=(char*)calloc(strlen(ruta)+1,sizeof(char));
			parsearRuta(disco.node[ptr].path_name,d,name);// se saca el nombre de la ruta
			filler( buffer, name, NULL, 0 );
			ptr=disco.node[ptr].signodo;//se busca el siguiente nodo para encontrar la siguiente ruta
		}
	}
	return 0;
}


// El create intenta crear una ruta nueva
static int do_create (const char *ruta, mode_t mode, struct fuse_file_info *f)
{
	printf( "--> Intentando de crear %s\n", ruta);

	struct stat *s=(struct stat*)malloc(sizeof(struct stat));
	s->st_gid=getgid();
	s->st_uid=getuid();
	s->st_atime=s->st_mtime=time(NULL);
	s->st_mode=S_IFREG | 0644;
	s->st_nlink=1;
	int temp=hacerNodo(ruta,disco.t.rootno,s,TYPE_ORDINARY);
	return 0;
}

// El mknod intenta de usar el editor de texto
static int do_mknod (const char *ruta, mode_t mode, dev_t dev)
{
	printf( "--> Intentando de crear  %s\n", ruta);

	struct stat *s=(struct stat*)malloc(sizeof(struct stat));
	s->st_gid=getgid();
	s->st_uid=getuid();
	s->st_atime=s->st_mtime=time(NULL);
	s->st_mode=S_IFREG | 0644;
	s->st_nlink=1;
	int temp=hacerNodo(ruta,disco.t.rootno,s,TYPE_ORDINARY);
	return 0;
}

// Lo que hace esta función es intentar de desligar un archivo para eliminarlo
static int do_unlink(const char *ruta)
{
	printf( "--> Intentando de desligar  %s\n", ruta);

	int ans=eliminarNodo(ruta,disco.t.rootno);//se busca el nodo del arbol y se elimina
	if(ans==0)
	{
		return -ENOTEMPTY;
	}
	return 0;
}
// El mkdir se encarga de crear un directorio nuevo si el mismo todavía no existe
static int do_mkdir(const char *ruta, mode_t mode)
{
    printf( "--> Intentado de crear el directorio %s\n", ruta);


    struct stat *s=(struct stat*)malloc(sizeof(struct stat));
    s->st_gid=getgid();
    s->st_uid=getuid();
    s->st_atime=s->st_mtime=time(NULL);
    s->st_mode=S_IFDIR | 0775;
    s->st_nlink=1;
    int temp=hacerNodo(ruta,disco.t.rootno,s,TYPE_DIRECTORY);

    return 0;
}

//El rmdir se encarga de remover un directorio del arbol
static int do_rmdir(const char * ruta)
{
    printf( "--> Intentando de remover el directorio %s\n", ruta);

    int ans=eliminarNodo(ruta,disco.t.rootno);
    if(ans==0)
    {
        return -ENOTEMPTY;
    }
    return 0;
}

//El do_open se encarga de abrir archivos
static int do_open(const char *ruta, struct fuse_file_info *fi)
{
	printf( "--> Intentando de abrir %s\n", ruta);

	int temp = buscarNodo(ruta,disco.t.rootno);

	if(temp==-1)
		return -ENOENT;
	else if(strcmp(ruta,"/")!=0)
		return 0;
};



//El do_rename se encarga de cambiar el nombre de un archivo
static int do_rename(const char *from, const char *to){
    int temp = buscarNodo(from,disco.t.rootno);

    if(temp == -1)
    {
        printf("RUTA INVALIDA\n");
        return -ENOENT;
    }
    strcpy(disco.node[temp].path_name,to);
    DISCODURO debug2 = disco;
    return 0;
};

//Lo que hace es generar el BMP
static int do_flush(const char *ruta, struct fuse_file_info *fi){
    generarBMP();
    return 0;
};

//El do_opendir se encarga de abrir directorios
static int do_opendir(const char *ruta, struct fuse_file_info *fi)
{
    printf( "--> Intentando de abrir %s\n", ruta);

    int temp = buscarNodo(ruta,disco.t.rootno);

    if(temp==-1)
        return -ENOENT;
    else if(strcmp(ruta,"/")!=0)
        return 0;
};

static struct fuse_operations operations = {
    .getattr = do_getattr,
    .readdir = do_readdir,
    .mkdir = do_mkdir,
    .create = do_create,
    .mknod = do_mknod,
    .rmdir = do_rmdir,
    .unlink = do_unlink,
    .read = do_read,
    .write = do_write,
    .open = do_open,
    .rename = do_rename,
    .flush = do_flush,
    .opendir = do_opendir,

};



//argument's structure
// ./Black_white_FileSystem (mount|create|check) folder/ [mountpoint]

//Example of how to run this program
// ./Black_white_FileSystem create bwfs/
// ./Black_white_FileSystem mount bwfs/ /home/erickelizondo/mountpoint

int main( int argc, char *argv[] )
{

    char* modo = argv[MODO];// se utiliza para saber el modo que se quiere utilizar


    char parseBMP[FILESIZE] = {""};
    strcpy(parseBMP,argv[FOLDER]);
    almacenamiento = parseBMP;
    struct stat st = {0};
    if(stat(argv[FOLDER], &st ) == -1){// se usa para verificar si existe el folder donde se va a almacenar el filesystem
        mkdir(argv[FOLDER], 0700);//se crea la carpeta
    }
    char* archivoBMP = "almacenamiento.bmp";
    strcat(almacenamiento,archivoBMP);
    FILE *fp;
    int size = 0;
    if(strcmp(modo, "mount") == 0){//se verifica si se quiere hacer un "mount"
        struct stat st2 = {0};
        fp=fopen(almacenamiento,"rb");

        if(stat(argv[MOUNTPOINT], &st2) == -1){
            mkdir(argv[MOUNTPOINT], 0700);
        }
        if(fp== NULL){
            iniciarDataBmap(disco.dbmap);
            iniciarINodoBmap(disco.ibmap);
            iniciarNodoBmap(disco.nbmap);
            crearArbol();
            generarBMP();
            printf("\n\n - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -\n" );
            printf("\tSISTEMA DE ARCHIVOS MONTADO\n");
        }
        else{
            fseek (fp, headerSize, SEEK_SET);//se define que el inicio del archivo es despues del encabezado (header)
            while(fread(&disco, sizeof(DISCODURO), 1, fp));
            printf("\n\n - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -\n" );
            printf("\tSISTEMA DE ARCHIVOS MONTADO NUEVAMENTE\n");

            llenarData();
            fclose(fp);
        }
        argv[MODO] = "-f";//se usa para mostrar los prints
        argv[FOLDER] = "-s";//se utiliza para tener un solo hilo


    }else if(strcmp(modo, "create") == 0){
        iniciarDataBmap(disco.dbmap);
        iniciarINodoBmap(disco.ibmap);
        iniciarNodoBmap(disco.nbmap);
        crearArbol();
        generarBMP();
        exit(0);
    }else{
        printf("Checkeo de consistencia\n");
        exit(0);
    }

	return fuse_main( argc, argv, &operations, NULL );
}
