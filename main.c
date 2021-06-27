/**
 * Less Simple, Yet Stupid Filesystem.
 *
 * Mohammed Q. Hussain - http://www.maastaar.net
 *
 * This is an example of using FUSE to build a simple filesystem. It is a part of a tutorial in MQH Blog with the title "Writing Less Simple, Yet Stupid Filesystem Using FUSE in C": http://maastaar.net/fuse/linux/filesystem/c/2019/09/28/writing-less-simple-yet-stupid-filesystem-using-FUSE-in-C/
 *
 * License: GNU GPL
 */

#define FUSE_USE_VERSION 30

#include <fuse.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <time.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <limits.h>

// ... //

char dir_list[ 256 ][ 256 ];
int curr_dir_idx = -1;

char files_list[ 256 ][ 256 ];
int curr_file_idx = -1;

char files_content[ 256 ][ 256 ];
int curr_file_content_idx = -1;

void add_dir( const char *dir_name )
{
    curr_dir_idx++;
    strcpy( dir_list[ curr_dir_idx ], dir_name );
}

int is_dir( const char *path )
{
    path++; // Eliminating "/" in the path

    for ( int curr_idx = 0; curr_idx <= curr_dir_idx; curr_idx++ )
        if ( strcmp( path, dir_list[ curr_idx ] ) == 0 )
            return 1;

    return 0;
}

void add_file( const char *filename )
{
    curr_file_idx++;
    strcpy( files_list[ curr_file_idx ], filename );

    curr_file_content_idx++;
    strcpy( files_content[ curr_file_content_idx ], "" );
}

int is_file( const char *path )
{
    path++; // Eliminating "/" in the path

    for ( int curr_idx = 0; curr_idx <= curr_file_idx; curr_idx++ )
        if ( strcmp( path, files_list[ curr_idx ] ) == 0 )
            return 1;

    return 0;
}

int get_file_index( const char *path )
{
    path++; // Eliminating "/" in the path

    for ( int curr_idx = 0; curr_idx <= curr_file_idx; curr_idx++ )
        if ( strcmp( path, files_list[ curr_idx ] ) == 0 )
            return curr_idx;

    return -1;
}

void write_to_file( const char *path, const char *new_content )
{
    int file_idx = get_file_index( path );

    if ( file_idx == -1 ) // No such file
        return;

    strcpy( files_content[ file_idx ], new_content );
}

// ... //

static int do_getattr( const char *path, struct stat *st )
{
    st->st_uid = getuid(); // The owner of the file/directory is the user who mounted the filesystem
    st->st_gid = getgid(); // The group of the file/directory is the same as the group of the user who mounted the filesystem
    st->st_atime = time( NULL ); // The last "a"ccess of the file/directory is right now
    st->st_mtime = time( NULL ); // The last "m"odification of the file/directory is right now

    if ( strcmp( path, "/" ) == 0 || is_dir( path ) == 1 )
    {
        st->st_mode = S_IFDIR | 0755;
        st->st_nlink = 2; // Why "two" hardlinks instead of "one"? The answer is here: http://unix.stackexchange.com/a/101536
    }
    else if ( is_file( path ) == 1 )
    {
        st->st_mode = S_IFREG | 0644;
        st->st_nlink = 1;
        st->st_size = 1024;
    }
    else
    {
        return -ENOENT;
    }

    return 0;
}

static int do_readdir( const char *path, void *buffer, fuse_fill_dir_t filler, off_t offset, struct fuse_file_info *fi )
{
    filler( buffer, ".", NULL, 0 ); // Current Directory
    filler( buffer, "..", NULL, 0 ); // Parent Directory

    if ( strcmp( path, "/" ) == 0 ) // If the user is trying to show the files/directories of the root directory show the following
    {
        for ( int curr_idx = 0; curr_idx <= curr_dir_idx; curr_idx++ )
            filler( buffer, dir_list[ curr_idx ], NULL, 0 );

        for ( int curr_idx = 0; curr_idx <= curr_file_idx; curr_idx++ )
            filler( buffer, files_list[ curr_idx ], NULL, 0 );
    }

    return 0;
}

static int do_read( const char *path, char *buffer, size_t size, off_t offset, struct fuse_file_info *fi )
{
    int file_idx = get_file_index( path );

    if ( file_idx == -1 )
        return -1;
    char *content = files_content[ file_idx ];

    memcpy( buffer, content + offset, size );

    return strlen( content ) - offset;
}

static int do_mkdir( const char *path, mode_t mode )
{
    path++;
    add_dir( path );

    return 0;
}

static int do_mknod( const char *path, mode_t mode, dev_t rdev )
{
    path++;
    add_file( path );

    return 0;
}

static int do_write( const char *path, const char *buffer, size_t size, off_t offset, struct fuse_file_info *info )
{
    write_to_file( path, buffer );

    return size;
}

static struct fuse_operations operations = {
        .getattr	= do_getattr,
        .readdir	= do_readdir,
        .read		= do_read,
        .mkdir		= do_mkdir,
        .mknod		= do_mknod,
        .write		= do_write,
};

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <malloc.h>
#define _height 600
#define _width 800
#define _bitsperpixel 24
#define _planes 1
#define _compression 0
#define _pixelbytesize _height*_width*_bitsperpixel/8
#define _filesize _pixelbytesize+sizeof(bitmap)
#define _xpixelpermeter 0x130B //2835 , 72 DPI
#define _ypixelpermeter 0x130B //2835 , 72 DPI
#define pixel 0xFF
#pragma pack(push,1)
typedef struct{
    uint8_t signature[2];
    uint32_t filesize;
    uint32_t reserved;
    uint32_t fileoffset_to_pixelarray;
} fileheader;
typedef struct{
    uint32_t dibheadersize;
    uint32_t width;
    uint32_t height;
    uint16_t planes;
    uint16_t bitsperpixel;
    uint32_t compression;
    uint32_t imagesize;
    uint32_t ypixelpermeter;
    uint32_t xpixelpermeter;
    uint32_t numcolorspallette;
    uint32_t mostimpcolor;
} bitmapinfoheader;
typedef struct {
    fileheader fileheader;
    bitmapinfoheader bitmapinfoheader;
} bitmap;
#pragma pack(pop)
/*
int main( int argc, char *argv[] ){
    FILE *fp = fopen("test4.bmp","wb");
    bitmap *pbitmap  = (bitmap*)calloc(1,sizeof(bitmap));
    uint8_t *pixelbuffer = (uint8_t*)malloc(_pixelbytesize);
    strcpy(pbitmap->fileheader.signature,"BM");
    pbitmap->fileheader.filesize = _filesize;
    pbitmap->fileheader.fileoffset_to_pixelarray = sizeof(bitmap);
    pbitmap->bitmapinfoheader.dibheadersize =sizeof(bitmapinfoheader);
    pbitmap->bitmapinfoheader.width = _width;
    pbitmap->bitmapinfoheader.height = _height;
    pbitmap->bitmapinfoheader.planes = _planes;
    pbitmap->bitmapinfoheader.bitsperpixel = _bitsperpixel;
    pbitmap->bitmapinfoheader.compression = _compression;
    pbitmap->bitmapinfoheader.imagesize = _pixelbytesize;
    pbitmap->bitmapinfoheader.ypixelpermeter = _ypixelpermeter ;
    pbitmap->bitmapinfoheader.xpixelpermeter = _xpixelpermeter ;
    pbitmap->bitmapinfoheader.numcolorspallette = 0;
    fwrite (pbitmap, 1, sizeof(bitmap),fp);
    memset(pixelbuffer,pixel,_pixelbytesize);
    fwrite(pixelbuffer,1,_pixelbytesize,fp);
    fclose(fp);
    free(pbitmap);
    free(pixelbuffer);
}

    /*

    FILE* fd;
    // Construct arguments to pass to FUSE
    int fuse_argc;
    char ** fuse_argv = NULL;
    fuse_argv = realloc (fuse_argv, 2*sizeof(char*));
    fuse_argv[0] = argv[0];
    fuse_argv[1] = argv[optind+2];
    fuse_argc = 2;


    int i;
    for (i = 0; i < fuse_argc; ++i)
      printf ("fuse_argv[%d] = %s\n", i, fuse_argv[i]);

    char dir[PATH_MAX];
    // Path of mosaicfs archive
    if (argv[optind+1][0] == '/') {
        // Mount point is absoloute path
        strncpy(dir, argv[optind+1], sizeof(dir));
    } else {
        // Mount point is relative path: convert to absolute
        char cwd[PATH_MAX];
        getcwd(cwd, sizeof(cwd));
        snprintf(dir, sizeof(dir), "%s/%s", cwd, argv[optind+1]);
    }

    // Test mount point
    int rv;
    struct stat s;
    rv = stat(fuse_argv[1], &s);
    if (rv == 0) {
        // Mount point exists: make sure it is a regular and empty file
        if ( !S_ISREG(s.st_mode) | (s.st_size !=0) ) {
            fprintf(stderr,"Mount point (%s) is not a regular empty file\n",fuse_argv[1]);
            exit(-1);
        }
    } else {
        // Mount point does not exist: create it
        fd = fopen(fuse_argv[1], "w");
        fclose(fd);
    }
    //return fuse_main( argc, argv, &operations, NULL );
}*/