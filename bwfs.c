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

static int do_getattr(const char *path,struct stat *s)
{

	FILE *fptr;
	fptr=fopen("Harddisk.dat","ab+");

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

static int do_readdir(const char *path,void *buffer, fuse_fill_dir_t filler, off_t offset, struct fuse_file_info *f)
{
	FILE *fptr;
	fptr=fopen("Harddisk.dat","ab+");
	fseek (fptr, 0, SEEK_SET);

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

	fwrite(&HDD, sizeof(HARDDISK), 1, fptr);
	fclose(fptr);
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

int main( int argc, char *argv[] )
{
	/*FILE *fptr;
	fptr=fopen("Harddisk.dat","ab+");
	//fseek (fptr, 0, SEEK_SET);

	if(fptr==NULL)
    {
        fprintf(stderr, "\nError opend file\n");
        exit (1);
    }
    else
    {
    	fseek (fptr, 0, SEEK_END);
        int size = ftell(fptr);
    	if(size==0)
    	{
			intialize_databmap(HDD.dbmap);
			intialize_inodebmap(HDD.ibmap);
			intialize_nodebmap(HDD.nbmap);
			create_tree();
			printf("\n\n - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -\n" );
			printf("\tFILE SYSTEM MOUNTED\n");
		}
		else
		{ fseek (fptr, 0, SEEK_SET);
			while(fread(&HDD, sizeof(HARDDISK), 1, fptr));
			printf("\n\n - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -\n" );
			printf("\tFILE SYSTEM RE-MOUNTED\n");
		}
	}
	fclose(fptr);

	printf("I-node bitmap:    ");
	for(int i=0;i<20;i++){
		printf("%d ",HDD.ibmap[i].check);
	}
	printf("\nData-node bitmap: ");
	for(int i=0;i<20;i++){
		printf("%d ",HDD.dbmap[i].check);
	}
	printf("\nNode bitmap:      ");
	for(int i=0;i<20;i++){
		printf("%d ",HDD.nbmap[i].check);
	}
	printf("\n");
	int a;
	scanf("%d",&a);*/
    FILE *fptr;
    fptr=fopen("Harddisk.dat","ab+");

    fseek (fptr, 0, SEEK_END);
    int size = ftell(fptr);
    if(size==0)
    {
        intialize_databmap(HDD.dbmap);
        intialize_inodebmap(HDD.ibmap);
        intialize_nodebmap(HDD.nbmap);
        create_tree();
        printf("\n\n - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -\n" );
        printf("\tFILE SYSTEM MOUNTED\n");
    }
    else
    { fseek (fptr, 0, SEEK_SET);
        while(fread(&HDD, sizeof(HARDDISK), 1, fptr));
        HARDDISK debug = HDD;

        printf("\n\n - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -\n" );
        printf("\tFILE SYSTEM RE-MOUNTED\n");
    }
    fclose(fptr);
    fillData();

    /*intialize_databmap(HDD.dbmap);
    intialize_inodebmap(HDD.ibmap);
    intialize_nodebmap(HDD.nbmap);
    create_tree();*/

	return fuse_main( argc, argv, &operations, NULL );
}
