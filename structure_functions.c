#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include"structure.h"
DISCODURO disco;
void imprimirArbol(int rootno)
{
	if(rootno==-1)
		printf("#\n");
	printf("\t%s\n", disco.node[rootno].path_name);
	int ptr=disco.node[rootno].cabezaNodoHijo;
	while(ptr!=-1)
	{
		imprimirArbol(ptr);
		ptr=disco.node[ptr].signodo;
	}
}

void crearArbol()
{
	disco.t.rootno=crearRaiz();
}

int crearRaiz()
{
	int index=buscarNodoVacio(disco.nbmap);
	if(index==-1)
	{
		return -1;
	}
	strcpy(disco.node[index].path_name,"/");
	disco.node[index].inodo=-1;
	disco.node[index].nodoPadre=-1;
	disco.node[index].cabezaNodoHijo=-1;
	disco.node[index].signodo=-1;
	disco.node[index].nodoHijo=0;
	return index;
}

int buscarNodo(const char *ruta,int rootno)
{
	if(rootno==-1)
		return -1;
	else if(strcmp(disco.node[rootno].path_name,ruta)==0)
		return rootno;
	else
	{
		int temp=disco.node[rootno].cabezaNodoHijo;
		int ans;
 		while(temp!=-1)
		{
        	ans=buscarNodo(ruta,temp);
			if(ans!=-1)
				return ans;
			temp=disco.node[temp].signodo;
 		}

	}
}

int hacerNodo(const char *ruta,int rootno,struct stat *s,file_type ftype)
{
	int inode_no=hacerInodo(s,ftype);
	if(inode_no == -1)
	{
		return -1;
	}
	else if(buscarNodo(ruta,rootno)!=-1)
	{
		printf("El archivo ya existe\n");
		return -1;
	}
	else
	{
		int index=buscarNodoVacio(disco.nbmap);
		int j;
		char *d,*name;
		d=(char*)calloc(strlen(ruta),sizeof(char));
		name=(char*)calloc(strlen(ruta),sizeof(char));
		parsearRuta(ruta,d,name);

		int par=buscarNodo(d,rootno);

		if(par==-1)
		{
			printf("RUTA INVALIDA\n");
			return -1;
		}

		int length=disco.node[par].nodoHijo;
		disco.node[index].inodo=inode_no;
		strcpy(disco.node[index].path_name,ruta);
		disco.node[index].nodoPadre=par;
		disco.node[index].nodoHijo=0;

		if(disco.inode[disco.node[index].inodo].tipo==TYPE_DIRECTORY)
		{
			for(int i=0;i<BLK_LIMIT;i++)
				disco.inode[disco.node[index].inodo].bp[i]=NULL;
		}
		else
		{
			int block_no=hacerBloque(inode_no);
			disco.inode[disco.node[index].inodo].bp[disco.inode[disco.node[index].inodo].no_blocks]=&disco.block[block_no];
			disco.inode[inode_no].no_blocks++;
		}

		int ptr=disco.node[par].cabezaNodoHijo;
		if(ptr==-1)
		{
			ptr=index;
			disco.node[ptr].cabezaNodoHijo=-1;
			disco.node[ptr].signodo=-1;
			disco.node[par].cabezaNodoHijo=ptr;
		}
	    else
		{
			while(disco.node[ptr].signodo!=-1)
			{
				ptr=disco.node[ptr].signodo;
			}

			disco.node[ptr].signodo=index;
			ptr=disco.node[ptr].signodo;
			disco.node[ptr].cabezaNodoHijo=-1;
			disco.node[ptr].signodo=-1;
		}
		disco.node[par].nodoHijo++;

		return index;
	}
}

int eliminarNodo(const char *ruta,int rootno)
{
	int rn=buscarNodo(ruta,rootno);
	if(rn==-1)
	{
		printf("ARCHIVO NO ENCONTRADO\n");
		return 0;
	}
	else if(disco.node[rn].nodoHijo>0)
	{
		printf("No se puede eliminar un directorio no vacio\n");
		return 0;
	}
	else
	{
		int temp=disco.node[rn].inodo;
		disco.ibmap[temp].revisar=0;
		int par=disco.node[rn].nodoPadre;
		int ptr=disco.node[par].cabezaNodoHijo;
		int length=disco.node[par].nodoHijo;
		if(disco.node[ptr].inodo==temp)
		{
			disco.node[par].cabezaNodoHijo=disco.node[ptr].signodo;
			disco.node[par].nodoHijo--;
		}
		else
		{
			while(disco.node[disco.node[ptr].signodo].inodo!=temp)
			{
				ptr=disco.node[ptr].signodo;
			}
			int temp1=disco.node[ptr].signodo;
			disco.node[ptr].signodo=disco.node[temp1].signodo;
			disco.node[ptr].nodoHijo--;
		}
		if(disco.inode[disco.node[rn].inodo].tipo==TYPE_ORDINARY)
		{
			for(int i=0;i<disco.inode[disco.node[rn].inodo].no_blocks;i++)
			{
				int bn=disco.inode[disco.node[rn].inodo].bp[i]->nodoBlk;
				disco.dbmap[bn].revisar=0;
			}
		}
		disco.nbmap[rn].revisar=0;
		return 1;
	}
}
void parsearRuta(const char* ruta,char *directory,char *name)
{
   int length=strlen(ruta);
   int i=length-1,j=0;

   while(ruta[i]!='/')
   	i--;

   if(i==0)
   	directory[0]=ruta[0];

   else
	{
		for (j=0;j<i;j++)
			directory[j]=ruta[j];
	}
   i=0;j++;
   while(j<length)
   	name[i++]=ruta[j++];
}

void iniciarNodoBmap(NBMAP *n)
{
	for(int i=0;i<NO_BLKS;i++)
	{
		n[i].node_no=i;
		n[i].revisar=0;
	}
}

void iniciarINodoBmap(IBMAP *ibp)
{
	for(int i=0;i<NO_BLKS;i++)
	{
		ibp[i].inode_no=i;
		ibp[i].revisar=0;
	}
}

void iniciarDataBmap(DBMAP *dbp)
{
	for(int i=0;i<NO_BLKS;i++)
	{
		dbp[i].nodoBlk=i;
		dbp[i].revisar=0;
	}
}

int buscarNodoVacio(NBMAP *n)
{
	for(int i=0;i<NO_BLKS;i++)
		if(n[i].revisar==0)
		{
			n[i].revisar=1;
			return n[i].node_no;
		}
	return -1;
}

int buscarINodoVacio(IBMAP *ibp)
{
	for(int i=0;i<NO_BLKS;i++)
		if(ibp[i].revisar==0)
		{
			ibp[i].revisar=1;
			return ibp[i].inode_no;
		}
	return -1;
}

int buscarBloqueVacio(DBMAP *dbp)
{
	for(int i=0;i<NO_BLKS;i++)
		if(dbp[i].revisar==0)
		{
			dbp[i].revisar=1;
			return dbp[i].nodoBlk;
		}
	return -1;
}

int hacerBloque(int inode_no)
{
	int B_index=buscarBloqueVacio(disco.dbmap);
	if(B_index==-1)
	{
		printf("DISCO DURO LLENO\n");
		return -1;
	}
	disco.block[B_index].nodoBlk=B_index;
	disco.block[B_index].fin=0;
	disco.block[B_index].i=&disco.inode[inode_no];
	for(int i=0;i<BLKSIZE;i++)
		disco.block[B_index].blk[i]='\0';
	return B_index;
}

int hacerInodo(struct stat *s,file_type ftype)
{
	int I_index=buscarINodoVacio(disco.ibmap);
	if(I_index==-1)
	{
		printf("No hay INodos vacios\n");
		return -1;
	}
	disco.inode[I_index].inodo=I_index;
	disco.inode[I_index].mode=s->st_mode;
	disco.inode[I_index].uid=s->st_uid;
	disco.inode[I_index].gid=s->st_gid;
	disco.inode[I_index].nlink=s->st_nlink;
	disco.inode[I_index].tipo=ftype;
	disco.inode[I_index].no_blocks=0;
	disco.inode[I_index].tamanioArchivo=0;
	return I_index;
}

void limpiarInfoINodo(){
    for(int i = 0; i<NO_BLKS; i++){
        for(int j=0; j<BLK_LIMIT; j++){
            disco.inode[i].bp[j] = NULL;
        }
    }
}

void llenarData(){
    DISCODURO debug = disco;
    limpiarInfoINodo();
    //disco.inode[1].bp[0] = &disco.block[0];
    DISCODURO debug2 = disco;
    int initialBlock = 0;
    for(int i = 0; i<NO_BLKS; i++) {
        if(disco.inode[i].no_blocks == 0)
            continue;

        int currentBlock = initialBlock + disco.inode[i].no_blocks;
        int indexBlock = 0;
        for (initialBlock; initialBlock < currentBlock; initialBlock++) {
            disco.inode[i].bp[indexBlock] = &disco.block[initialBlock];
            indexBlock++;
            /*
            if(!disco.inode[i].bp[j]){
                disco.inode[i].bp[j] = &disco.block[0];
            }*/
        }
    }
    DISCODURO debug3 = disco;
    /*
    for(int i = 0; i < 20; i++){
        if((disco.block[i].fin)) {
            char test2[512];
            strcpy(disco.block[i].i->bp[i]->blk, disco.block[i].blk);
        }
        else
            break;
    }*/
}
