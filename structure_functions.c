#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include"structure.h"

void print_tree(int rootno)
{
	if(rootno==-1)
		printf("#\n");
	printf("\t%s\n", HDD.node[rootno].path_name);
	int ptr=HDD.node[rootno].headchildno;
	while(ptr!=-1)
	{
		print_tree(ptr);
		ptr=HDD.node[ptr].nextno;
	}
}

void create_tree()
{
	HDD.t.rootno=create_root();
}

int create_root()
{
	int index=find_empty_node(HDD.nbmap);
	if(index==-1)
	{
		return -1;
	}
	strcpy(HDD.node[index].path_name,"/");
	HDD.node[index].ino=-1;
	HDD.node[index].parentno=-1;
	HDD.node[index].headchildno=-1;
	HDD.node[index].nextno=-1;
	HDD.node[index].no_children=0;
	return index;
}

int search_node(const char *path,int rootno)
{
	if(rootno==-1)
		return -1;
	else if(strcmp(HDD.node[rootno].path_name,path)==0)
		return rootno;
	else
	{
		int temp=HDD.node[rootno].headchildno;
		int ans;
 		while(temp!=-1)
		{
        	ans=search_node(path,temp);
			if(ans!=-1)
				return ans;
			temp=HDD.node[temp].nextno;
 		}

	}
}

int make_node(const char *path,int rootno,struct stat *s,file_type ftype)
{
	int inode_no=make_inode(s,ftype);
	if(inode_no == -1)
	{
		return -1;
	}
	else if(search_node(path,rootno)!=-1)
	{
		printf("File already exists\n");
		return -1;
	}
	else
	{
		int index=find_empty_node(HDD.nbmap);
		int j;
		char *d,*name;
		d=(char*)calloc(strlen(path),sizeof(char));
		name=(char*)calloc(strlen(path),sizeof(char));
		parse_path(path,d,name);

		int par=search_node(d,rootno);

		if(par==-1)
		{
			printf("INVALID PATH\n");
			return -1;
		}

		int length=HDD.node[par].no_children;
		HDD.node[index].ino=inode_no;
		strcpy(HDD.node[index].path_name,path);
		HDD.node[index].parentno=par;
		HDD.node[index].no_children=0;

		if(HDD.inode[HDD.node[index].ino].type==TYPE_DIRECTORY)
		{
			for(int i=0;i<BLK_LIMIT;i++)
				HDD.inode[HDD.node[index].ino].bp[i]=NULL;
		}
		else
		{
			int block_no=make_blk(inode_no);
			HDD.inode[HDD.node[index].ino].bp[HDD.inode[HDD.node[index].ino].no_blocks]=&HDD.block[block_no];
			HDD.inode[inode_no].no_blocks++;
		}

		int ptr=HDD.node[par].headchildno;
		if(ptr==-1)
		{
			ptr=index;
			HDD.node[ptr].headchildno=-1;
			HDD.node[ptr].nextno=-1;
			HDD.node[par].headchildno=ptr;
		}
	    else
		{
			while(HDD.node[ptr].nextno!=-1)
			{
				ptr=HDD.node[ptr].nextno;
			}

			HDD.node[ptr].nextno=index;
			ptr=HDD.node[ptr].nextno;
			HDD.node[ptr].headchildno=-1;
			HDD.node[ptr].nextno=-1;
		}
		HDD.node[par].no_children++;

		return index;
	}
}

int rem_node(const char *path,int rootno)
{
	int rn=search_node(path,rootno);
	if(rn==-1)
	{
		printf("FILE NOT FOUND\n");
		return 0;
	}
	else if(HDD.node[rn].no_children>0)
	{
		printf("Cannot delete non-empty directory\n");
		return 0;
	}
	else
	{
		int temp=HDD.node[rn].ino;
		HDD.ibmap[temp].check=0;
		int par=HDD.node[rn].parentno;
		int ptr=HDD.node[par].headchildno;
		int length=HDD.node[par].no_children;
		if(HDD.node[ptr].ino==temp)
		{
			HDD.node[par].headchildno=HDD.node[ptr].nextno;
			HDD.node[par].no_children--;
		}
		else
		{
			while(HDD.node[HDD.node[ptr].nextno].ino!=temp)
			{
				ptr=HDD.node[ptr].nextno;
			}
			int temp1=HDD.node[ptr].nextno;
			HDD.node[ptr].nextno=HDD.node[temp1].nextno;
			HDD.node[ptr].no_children--;
		}
		if(HDD.inode[HDD.node[rn].ino].type==TYPE_ORDINARY)
		{
			for(int i=0;i<HDD.inode[HDD.node[rn].ino].no_blocks;i++)
			{
				int bn=HDD.inode[HDD.node[rn].ino].bp[i]->blk_no;
				HDD.dbmap[bn].check=0;
			}
		}
		HDD.nbmap[rn].check=0;
		return 1;
	}
}
void parse_path(const char* path,char *directory,char *name)
{
   int length=strlen(path);
   int i=length-1,j=0;

   while(path[i]!='/')
   	i--;

   if(i==0)
   	directory[0]=path[0];

   else
	{
		for (j=0;j<i;j++)
			directory[j]=path[j];
	}
   i=0;j++;
   while(j<length)
   	name[i++]=path[j++];
}

void intialize_nodebmap(NBMAP *n)
{
	for(int i=0;i<NO_BLKS;i++)
	{
		n[i].node_no=i;
		n[i].check=0;
	}
}

void intialize_inodebmap(IBMAP *ibp)
{
	for(int i=0;i<NO_BLKS;i++)
	{
		ibp[i].inode_no=i;
		ibp[i].check=0;
	}
}

void intialize_databmap(DBMAP *dbp)
{
	for(int i=0;i<NO_BLKS;i++)
	{
		dbp[i].blk_no=i;
		dbp[i].check=0;
	}
}

int find_empty_node(NBMAP *n)
{
	for(int i=0;i<NO_BLKS;i++)
		if(n[i].check==0)
		{
			n[i].check=1;
			return n[i].node_no;
		}
	return -1;
}

int find_empty_inode(IBMAP *ibp)
{
	for(int i=0;i<NO_BLKS;i++)
		if(ibp[i].check==0)
		{
			ibp[i].check=1;
			return ibp[i].inode_no;
		}
	return -1;
}

int find_empty_block(DBMAP *dbp)
{
	for(int i=0;i<NO_BLKS;i++)
		if(dbp[i].check==0)
		{
			dbp[i].check=1;
			return dbp[i].blk_no;
		}
	return -1;
}

int make_blk(int inode_no)
{
	int B_index=find_empty_block(HDD.dbmap);
	if(B_index==-1)
	{
		printf("HARD DISK FULL\n");
		return -1;
	}
	HDD.block[B_index].blk_no=B_index;
	HDD.block[B_index].end=0;
	HDD.block[B_index].i=&HDD.inode[inode_no];
	for(int i=0;i<BLKSIZE;i++)
		HDD.block[B_index].blk[i]='\0';
	return B_index;
}

int make_inode(struct stat *s,file_type ftype)
{
	int I_index=find_empty_inode(HDD.ibmap);
	if(I_index==-1)
	{
		printf("No empty INODES\n");
		return -1;
	}
	HDD.inode[I_index].ino=I_index;
	HDD.inode[I_index].mode=s->st_mode;
	HDD.inode[I_index].uid=s->st_uid;
	HDD.inode[I_index].gid=s->st_gid;
	HDD.inode[I_index].nlink=s->st_nlink;
	HDD.inode[I_index].type=ftype;
	HDD.inode[I_index].no_blocks=0;
	HDD.inode[I_index].filesize=0;
	return I_index;
}

void cleanINodeData(){
    for(int i = 0; i<NO_BLKS; i++){
        for(int j=0; j<BLK_LIMIT; j++){
            HDD.inode[i].bp[j] = NULL;
        }
    }
}

void fillData(){
    HARDDISK debug = HDD;
    cleanINodeData();
    //HDD.inode[1].bp[0] = &HDD.block[0];
    HARDDISK debug2 = HDD;
    int initialBlock = 0;
    for(int i = 0; i<NO_BLKS; i++) {
        if(HDD.inode[i].no_blocks == 0)
            continue;

        int currentBlock = initialBlock + HDD.inode[i].no_blocks;
        int indexBlock = 0;
        for (initialBlock; initialBlock < currentBlock; initialBlock++) {
            HDD.inode[i].bp[indexBlock] = &HDD.block[initialBlock];
            indexBlock++;
            /*
            if(!HDD.inode[i].bp[j]){
                HDD.inode[i].bp[j] = &HDD.block[0];
            }*/
        }
    }
    HARDDISK debug3 = HDD;
    /*
    for(int i = 0; i < 20; i++){
        if((HDD.block[i].end)) {
            char test2[512];
            strcpy(HDD.block[i].i->bp[i]->blk, HDD.block[i].blk);
        }
        else
            break;
    }*/
}
