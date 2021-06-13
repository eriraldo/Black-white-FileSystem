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
char* folder;
char* almacenamiento;
static int do_getattr(const char *path,struct stat *s)
{
    FILE *fptr;
	fptr=fopen(folder,"wb");
	char * test = folder;
	if(fptr != NULL)
	    fseek (fptr, 0, SEEK_SET);

	printf( "[getattr] Called\n" );
	printf("\n\tTREE:\n");
	print_tree(HDD.t.rootno);
	printf( "\n\tAttributes of %s requested\n", path );


	if(strcmp(path,"/")==0)
	{
		if(HDD.node[HDD.t.rootno].ino == -1)
		{
			int a=make_inode(s,TYPE_DIRECTORY);
			HDD.node[HDD.t.rootno].ino=a;
			s->st_ino=a;
		}
		s->st_gid=getgid();
		s->st_uid=getuid();
		s->st_atime=s->st_mtime=time(NULL);
		s->st_mode=S_IFDIR | 0755;
		s->st_nlink=1;
		s->st_blocks=s->st_blksize=s->st_size=0;

		fwrite(&HDD, sizeof(HARDDISK), 1, fptr);
		fclose(fptr);
		return 0;
	}
	else if(strcmp(path,"/.Trash")!=0 && strcmp(path,"/.Trash-1000")!=0 && strcmp(path,"/.xdg-volume-info")!=0 && strcmp(path,"/autorun.inf")!=0)
	{
		int temp=search_node(path,HDD.t.rootno);
		if(temp==-1)
		{
			printf("PATH DOES NOT EXIST\n");
			return -ENOENT;
		}
		s->st_ino=HDD.node[temp].ino;
		s->st_gid=HDD.inode[HDD.node[temp].ino].gid;
		s->st_uid=HDD.inode[HDD.node[temp].ino].uid;
		s->st_atime=s->st_mtime=s->st_ctime=time(NULL);
		s->st_mode=HDD.inode[HDD.node[temp].ino].mode;
		s->st_nlink=HDD.inode[HDD.node[temp].ino].nlink+HDD.node[temp].no_children;
		s->st_blocks=HDD.inode[HDD.node[temp].ino].no_blocks;
		s->st_blksize=BLKSIZE;
		s->st_size=HDD.inode[HDD.node[temp].ino].filesize;//s->st_size=BLKSIZE*HDD.inode[HDD.node[temp].ino].no_blocks;

		fwrite(&HDD, sizeof(HARDDISK), 1, fptr);
		fclose(fptr);
		return 0;
	}
}

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

static int do_readdir(const char *path,void *buffer, fuse_fill_dir_t filler, off_t offset, struct fuse_file_info *f)
{
	/*FILE *fptr;
	fptr=fopen(folder,"wb");
	fseek (fptr, 0, SEEK_SET);*/
    HARDDISK debug32 = HDD;
	printf( "--> Getting The List of Files of %s\n", path );

	filler( buffer, ".", NULL, 0 );
	filler( buffer, "..", NULL, 0 );
	printf("%s",HDD.node[HDD.t.rootno+1].path_name);
	int temp=search_node(path,HDD.t.rootno);

	if(temp==-1)
	{
		printf("INVALID PATH\n");
		return -ENOENT;
	}

	else
	{
		int i=0;
		int ptr=HDD.node[temp].headchildno;
		while(ptr!=-1)
		{
			char *d,*name;
			d=(char*)calloc(strlen(path),sizeof(char));
			name=(char*)calloc(strlen(path),sizeof(char));
			parse_path(HDD.node[ptr].path_name,d,name);
			filler( buffer, name, NULL, 0 );
			ptr=HDD.node[ptr].nextno;
		}
	}

	/*fwrite(&HDD, sizeof(HARDDISK), 1, fptr);
	fclose(fptr);*/
	return 0;
}

static int do_mkdir(const char *path, mode_t mode)
{
	printf( "--> Trying to mkdir %s\n", path);


	struct stat *s=(struct stat*)malloc(sizeof(struct stat));
	s->st_gid=getgid();
	s->st_uid=getuid();
	s->st_atime=s->st_mtime=time(NULL);
	s->st_mode=S_IFDIR | 0775;
	s->st_nlink=1;
	int temp=make_node(path,HDD.t.rootno,s,TYPE_DIRECTORY);

	return 0;
}

static int do_rmdir(const char * path)
{
	printf( "--> Trying to rmdir %s\n", path);

	int ans=rem_node(path,HDD.t.rootno);
	if(ans==0)
	{
		return -ENOTEMPTY;
	}
	return 0;
}

static int do_create (const char *path, mode_t mode, struct fuse_file_info *f)
{
	printf( "--> Trying to create %s\n", path);

	struct stat *s=(struct stat*)malloc(sizeof(struct stat));
	s->st_gid=getgid();
	s->st_uid=getuid();
	s->st_atime=s->st_mtime=time(NULL);
	s->st_mode=S_IFREG | 0644;
	s->st_nlink=1;
	int temp=make_node(path,HDD.t.rootno,s,TYPE_ORDINARY);
	return 0;
}

static int do_mknod (const char *path, mode_t mode, dev_t dev)
{
	printf( "--> Trying to create %s\n", path);

	struct stat *s=(struct stat*)malloc(sizeof(struct stat));
	s->st_gid=getgid();
	s->st_uid=getuid();
	s->st_atime=s->st_mtime=time(NULL);
	s->st_mode=S_IFREG | 0644;
	s->st_nlink=1;
	int temp=make_node(path,HDD.t.rootno,s,TYPE_ORDINARY);
	return 0;
}

static int do_unlink(const char *path)
{
	printf( "--> Trying to unlink %s\n", path);

	int ans=rem_node(path,HDD.t.rootno);
	if(ans==0)
	{
		return -ENOTEMPTY;
	}
	return 0;
}

static int do_read(const char *path, char *buffer, size_t size,off_t offset, struct fuse_file_info *fi)
{
	printf( "--> Trying to read %s, %lu, %lu\n", path, offset, size );

	int temp = search_node(path,HDD.t.rootno);
	size=HDD.inode[HDD.node[temp].ino].filesize;
	if(temp == -1)
	{
		printf("INVALID PATH\n");
		return -ENOENT;
	}

	int number_blocks=HDD.inode[HDD.node[temp].ino].no_blocks-1, j;	
	char *text=(char*)calloc(size,sizeof(char));
    HARDDISK debug = HDD;
	for(j=0;j<=number_blocks;j++)
	{
		strncat(text, (HDD.inode[HDD.node[temp].ino].bp[j])->blk ,(HDD.inode[HDD.node[temp].ino].bp[j])->end);
	}

    memcpy(buffer, text+offset, size);
    free(text);

    return size;
}

static int do_write(const char *path, const char *buffer, size_t size, off_t offset,struct fuse_file_info *f)
{
	printf( "--> Trying to write %s, %lu, %lu\n", path, offset, size );

	int temp = search_node(path,HDD.t.rootno);offset=0;

	if(temp == -1)
	{
		printf("INVALID PATH\n");
		return -ENOENT;
	}

	int last = (HDD.inode[HDD.node[temp].ino].bp[HDD.inode[HDD.node[temp].ino].no_blocks-1])->end;
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
			(HDD.inode[HDD.node[temp].ino].bp[HDD.inode[HDD.node[temp].ino].no_blocks-1])->blk[j]=text1[j-last];

		(HDD.inode[HDD.node[temp].ino].bp[HDD.inode[HDD.node[temp].ino].no_blocks-1])->end += size1;

		int inode_no=HDD.node[temp].ino;
		int block_no=make_blk(inode_no);
		HDD.inode[HDD.node[temp].ino].bp[HDD.inode[HDD.node[temp].ino].no_blocks]=&HDD.block[block_no];
		HDD.inode[inode_no].no_blocks++;

		last = (HDD.inode[HDD.node[temp].ino].bp[HDD.inode[HDD.node[temp].ino].no_blocks-1])->end;

		rem_space=BLKSIZE;
	}

	int size1=rem_space;
	char text2[size1];

	memcpy(text2 + (offset) ,buffer+written, size1);

	for(int j=last;j<(last+size1);j++)
		(HDD.inode[HDD.node[temp].ino].bp[HDD.inode[HDD.node[temp].ino].no_blocks-1])->blk[j]=text2[j-last];

	(HDD.inode[HDD.node[temp].ino].bp[HDD.inode[HDD.node[temp].ino].no_blocks-1])->end += size1;


	//printf("%s\n",buffer );
	HDD.inode[HDD.node[temp].ino].filesize+=size;
    HARDDISK debug = HDD;
	return size;
}

static int do_chmod(const char *path, mode_t mode)
{
	printf( "--> Trying to change permissions %s\n", path);

	int temp = search_node(path,HDD.t.rootno);

	if(temp == -1)
	{
		printf("INVALID PATH\n");
		return -ENOENT;
	}
	HDD.inode[HDD.node[temp].ino].mode=mode;
	return 0;
}

static int do_open(const char *path, struct fuse_file_info *fi)
{
	printf( "--> Trying to open %s\n", path);

	int temp = search_node(path,HDD.t.rootno);

	if(temp==-1)
		return -ENOENT;
	else if(strcmp(path,"/")!=0)
		return 0;
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
    char* archivo = "Harddisk.dat";
    strcat(folder,archivo);
    char* archivoBMP = "almacenamiento.bmp";
    strcat(almacenamiento,archivoBMP);


    FILE *fptr;


    int size = 0;


    if(strcmp(modo, "mount") == 0){
        struct stat st2 = {0};
        fptr=fopen(folder,"ab+");
        fseek (fptr, 0, SEEK_END);
        size = ftell(fptr);
        if(stat(argv[3], &st2) == -1){
            mkdir(argv[3], 0700);
        }
        if(size==0){
            intialize_databmap(HDD.dbmap);
            intialize_inodebmap(HDD.ibmap);
            intialize_nodebmap(HDD.nbmap);
            create_tree();
            printf("\n\n - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -\n" );
            printf("\tFILE SYSTEM MOUNTED\n");
        }
        else{
            fseek (fptr, 0, SEEK_SET);
            while(fread(&HDD, sizeof(HARDDISK), 1, fptr));
            HARDDISK debug32 = HDD;
            printf("\n\n - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -\n" );
            printf("\tFILE SYSTEM RE-MOUNTED\n");

            fillData();

        }
        argv[1] = "-f";
        argv[2] = "-s";


    }else if(strcmp(modo, "create") == 0){
        fptr=fopen(folder,"wb");
        fseek (fptr, 0, SEEK_END);
        size = ftell(fptr);
        intialize_databmap(HDD.dbmap);
        intialize_inodebmap(HDD.ibmap);
        intialize_nodebmap(HDD.nbmap);
        create_tree();
        fwrite(&HDD, sizeof(HARDDISK), 1, fptr);
        exit(0);
    }else{
        printf("Checkeo de consistencia\n");
        exit(0);
    }

    fclose(fptr);
    //test();
/*
    generateBMP();
    readBMP();
    exit(0);*/
    for(int i = 0; i<argc; i++){
        printf("%s\n", argv[i]);
    }
	return fuse_main( argc, argv, &operations, NULL );
}
