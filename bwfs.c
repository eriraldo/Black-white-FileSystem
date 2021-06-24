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
#include <stdbool.h>
#include "structure.h"
#include "bmpGen.h"
#include <stdarg.h>
#include <linux/limits.h>
#include <dirent.h>

char* folder;
char* almacenamiento;
extern DISCODURO disco;
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
			s->st_ino=a;
		}
		s->st_gid=getgid();
		s->st_uid=getuid();
		s->st_atime=s->st_mtime=time(NULL);
		s->st_mode=S_IFDIR | 0755;
		s->st_nlink=1;
		s->st_blocks=s->st_blksize=s->st_size=0;
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

static int do_write(const char *ruta, const char *buffer, size_t size, off_t offset,struct fuse_file_info *f)
{
    printf( "--> Intentando de escribir %s, %lu, %lu\n", ruta, offset, size );

    int temp = buscarNodo(ruta,disco.t.rootno);offset=0;

    if(temp == -1)
    {
        printf("RUTA INVALIDA\n");
        return -ENOENT;
    }

    int last = (disco.inode[disco.node[temp].inodo].bp[disco.inode[disco.node[temp].inodo].no_blocks-1])->fin;
    int rem_space=BLKSIZE-last;
    int tempsize=size;
    int written=0;

    char text1[size];
    while(rem_space<tempsize)
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


    //printf("%s\n",buffer );
    disco.inode[disco.node[temp].inodo].tamanioArchivo+=size;
    DISCODURO debug = disco;
    return size;
}

static int do_read(const char *ruta, char *buffer, size_t size,off_t offset, struct fuse_file_info *fi)
{
    size_t size2 = size;
    printf( "--> Intentado de leer %s, %lu, %lu\n", ruta, offset, size );
    DISCODURO debug = disco;
    int temp = buscarNodo(ruta,disco.t.rootno);

    if(temp == -1)
    {
        printf("RUTA INVALIDA\n");
        return -ENOENT;
    }
    size_t size1=disco.inode[disco.node[temp].inodo].tamanioArchivo;

    int number_blocks=disco.inode[disco.node[temp].inodo].no_blocks-1, j;
    char text [sizeof(DISCODURO)];
    for(j=0;j<=number_blocks;j++)
    {
        strncat(text, (disco.inode[disco.node[temp].inodo].bp[j])->blk ,(disco.inode[disco.node[temp].inodo].bp[j])->fin);
    }
    //strcpy(text,cat);
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
/*
void printFile(){

    FILE *ptr;

    ptr = fopen(folder,"rb");  // r for read, b for binary
    fseek (ptr, 0, SEEK_END);
    int size = ftell(ptr);
    unsigned char buffer[size];
    fread(buffer,sizeof(buffer),1,ptr); // read 10 bytes to our buffer

    printf("IMPRIMIENDO: %d  BYTES\n", size);
    bool test = false;
    for(int i = 0; i<size; i++)
        if(i!=0 && i%30 == 0) {
            printf("\n");
        }else{
            printf("%u ", buffer[i]); // prints a series of bytes
            test = !test;

        }



}

void test(){
    FILE *ptr;
    ptr = fopen(folder,"rb");  // r for read, b for binary
    fseek (ptr, 0, SEEK_END);
    int size = ftell(ptr);
    ptr = fopen(folder,"rb");  // r for read, b for binary
    FILE *fp;
    fp = fopen("bwfs/test.bmp","ab+");
    fwrite(ptr, size, 1, fp);
    fclose(fp);
    fclose(ptr);
}
*/
static int do_readdir(const char *ruta,void *buffer, fuse_fill_dir_t filler, off_t offset, struct fuse_file_info *f)
{
    DISCODURO debug32 = disco;
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
		while(ptr!=-1)
		{
			char *d,*name;
			d=(char*)calloc(strlen(ruta)+1,sizeof(char));
			name=(char*)calloc(strlen(ruta)+1,sizeof(char));
			parsearRuta(disco.node[ptr].path_name,d,name);
			filler( buffer, name, NULL, 0 );
			ptr=disco.node[ptr].signodo;
		}
	}
	return 0;
}



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

static int do_unlink(const char *ruta)
{
	printf( "--> Intentando de desligar  %s\n", ruta);

	int ans=eliminarNodo(ruta,disco.t.rootno);
	if(ans==0)
	{
		return -ENOTEMPTY;
	}
	return 0;
}

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



static int do_chmod(const char *ruta, mode_t mode)
{
	printf( "--> Intentando de cambiar permisos %s\n", ruta); //

	int temp = buscarNodo(ruta,disco.t.rootno);

	if(temp == -1)
	{
		printf("RUTA INVALIDA\n");
		return -ENOENT;
	}
	disco.inode[disco.node[temp].inodo].mode=mode;
	return 0;
}

static int do_open(const char *ruta, struct fuse_file_info *fi)
{
	printf( "--> Intentando de abrir %s\n", ruta);

	int temp = buscarNodo(ruta,disco.t.rootno);

	if(temp==-1)
		return -ENOENT;
	else if(strcmp(ruta,"/")!=0)
		return 0;
};

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

static int do_flush(const char *ruta, struct fuse_file_info *fi){
    generarBMP();
    return 0;
};

void log_msg(const char *format, ...)
{
    va_list ap;
    va_start(ap, format);
    FILE *fp;
    fp = fopen("bwfs/reportMessage.txt","w");
    vfprintf(fp, format, ap);
    fclose(fp);

}
int log_error(char *func)
{
    int ret = -errno;

    log_msg("    ERROR %s: %s\n", func, strerror(errno));

    return ret;
}
void log_retstat(char *func, int retstat)
{
    int errsave = errno;
    log_msg("    %s returned %d\n", func, retstat);
    errno = errsave;
}

// make a system call, checking (and reporting) return status and
// possibly logging error
int log_syscall(char *func, int retstat, int min_ret)
{
    log_retstat(func, retstat);

    if (retstat < min_ret) {
        log_error(func);
        retstat = -errno;
    }

    return retstat;
}
/*
static void bb_fullpath(char fpath[PATH_MAX], const char *ruta)
{
    strcpy(fpath, BB_DATA->rootdir);
    strncat(fpath, ruta, PATH_MAX); // ridiculously long paths will
    // break here

    log_msg("    bb_fullpath:  rootdir = \"%s\", ruta = \"%s\", fpath = \"%s\"\n",
            BB_DATA->rootdir, ruta, fpath);
}
int do_statfs(const char *ruta, struct statvfs *statv)
{
    int retstat = 0;
    char fpath[PATH_MAX];

    log_msg("\nbb_statfs(ruta=\"%s\", statv=0x%08x)\n",
            ruta, statv);
    bb_fullpath(fpath, ruta);
    // get stats for underlying filesystem
    retstat = log_syscall("statvfs", statvfs(fpath, statv), 0);
    log_statvfs(statv);
    return retstat;
}

*/
void log_fi (struct fuse_file_info *fi)
{
    log_msg("    fi:\n");

    /** Open flags.  Available in open() and release() */
    //	int flags;
    log_struct(fi, flags, 0x%08x, );

    /** Old file handle, don't use */
    //	unsigned long fh_old;
    log_struct(fi, fh_old, 0x%08lx,  );

    /** In case of a write operation indicates if this was caused by a
        writepage */
    //	int writepage;
    log_struct(fi, writepage, %d, );

    /** Can be filled in by open, to use direct I/O on this file.
        Introduced in version 2.4 */
    //	unsigned int keep_cache : 1;
    log_struct(fi, direct_io, %d, );

    /** Can be filled in by open, to indicate, that cached file data
        need not be invalidated.  Introduced in version 2.4 */
    //	unsigned int flush : 1;
    log_struct(fi, keep_cache, %d, );

    /** Padding.  Do not use*/
    //	unsigned int padding : 29;

    /** File handle.  May be filled in by filesystem in open().
        Available in all other file operations */
    //	uint64_t fh;
    log_struct(fi, fh, 0x%016llx,  );

    /** Lock owner id.  Available in locking operations and flush */
    //  uint64_t lock_owner;
    log_struct(fi, lock_owner, 0x%016llx, );
}

int do_opendir(const char *ruta, struct fuse_file_info *fi) //CREO QUE YA FUNCIONA, FALTA REVISAR
{
    DIR *dp;
    int retstat = 0;
    log_msg("\ndo_opendir(ruta=\"%s\", fi=0x%08x)\n",
            ruta, fi);
    int temp = buscarNodo(ruta,disco.t.rootno);

    if(temp==-1)
        return -ENOENT;
    else {
        dp = opendir(ruta);
        //return 0;
    }
    // since opendir returns a pointer, takes some custom handling of
    // return status.
    log_msg("    opendir retornó 0x%p\n", dp);
    if (dp == NULL)
        retstat = log_error("do_opendir opendir");

    fi->fh = (intptr_t) dp;

    log_fi(fi);

    return retstat;
}

static struct fuse_operations operations = {
    .getattr = do_getattr,
    .readdir = do_readdir,	//ls function
    .mkdir = do_mkdir,		//MAKING DIRECTORY
    .create = do_create,	//USING TOUCH
    .mknod = do_mknod,		//USING TEXT EDITOR
    .rmdir = do_rmdir,		//REMOVING DIRECTORY
    .unlink = do_unlink,	//REMOVING FILE
    .read = do_read,		//READING FILE
    .write = do_write,		//WRITING FILE
    .chmod = do_chmod,		//CHANGING PERMISSION
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
    char* newArgv[3] = {argv[0], "-f", ""};
    char mountpoint[100] ={""};
    if(argc > 3) {
        strcpy(mountpoint,argv[3]);
        newArgv[2] = mountpoint;
    }
    char* modo = argv[1];
    char parseFolder[100] = {""};
    strcpy(parseFolder,argv[2]);
    char parseBMP[100] = {""};
    strcpy(parseBMP,argv[2]);
    folder = parseFolder;
    almacenamiento = parseBMP;
    struct stat st = {0};
    if(stat(folder, &st ) == -1){
        mkdir(folder, 0700);
    }
    char* archivoBMP = "almacenamiento.bmp";
    strcat(almacenamiento,archivoBMP);
    FILE *fp;
    int size = 0;
    if(strcmp(modo, "mount") == 0){
        struct stat st2 = {0};
        fp=fopen(almacenamiento,"rb");
        if(fp == NULL){
            fp = fopen(almacenamiento,"wb");
        }
        fseek (fp, 0, SEEK_END);
        size = ftell(fp);
        if(stat(argv[3], &st2) == -1){
            mkdir(argv[3], 0700);
        }
        if(size==0){
            iniciarDataBmap(disco.dbmap);
            iniciarINodoBmap(disco.ibmap);
            iniciarNodoBmap(disco.nbmap);
            crearArbol();
            printf("\n\n - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -\n" );
            printf("\tSISTEMA DE ARCHIVOS MONTADO\n");
        }
        else{
            fseek (fp, headerSize, SEEK_SET);
            while(fread(&disco, sizeof(DISCODURO), 1, fp));
            DISCODURO debug32 = disco;
            printf("\n\n - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -\n" );
            printf("\tSISTEMA DE ARCHIVOS MONTADO NUEVAMENTE\n");

            llenarData();

        }
        argv[1] = "-f";
        argv[2] = "-s";
        fclose(fp);

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
