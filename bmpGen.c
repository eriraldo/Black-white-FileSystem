#include "bmpGen.h"
#include "structure.h"

extern char* folder;
extern char* almacenamiento;
extern DISCODURO disco;

void generarBMP(){
    //informacion del header del BMP
    FILE *fp = fopen(almacenamiento,"wb");
    bitmap *mibitmap  = (bitmap*)calloc(1,sizeof(bitmap));
    uint8_t *pixelbuffer = (uint8_t*)malloc(sizeof(DISCODURO));
    strcpy(mibitmap->fileheader.signature,"BM");
    mibitmap->fileheader.filesize = _filesize;
    mibitmap->fileheader.fileoffset_to_pixelarray = sizeof(bitmap);
    mibitmap->bitmapinfoheader.dibheadersize =sizeof(bitmapinfoheader);
    mibitmap->bitmapinfoheader.width = _width;
    mibitmap->bitmapinfoheader.height = _height;
    mibitmap->bitmapinfoheader.planes = _planes;
    mibitmap->bitmapinfoheader.bitsperpixel = _bitsperpixel;
    mibitmap->bitmapinfoheader.compression = _compression;
    mibitmap->bitmapinfoheader.imagesize = _pixelbytesize;
    mibitmap->bitmapinfoheader.ypixelpermeter = _ypixelpermeter ;
    mibitmap->bitmapinfoheader.xpixelpermeter = _xpixelpermeter ;
    mibitmap->bitmapinfoheader.numcolorspallette = 0;
    fwrite (mibitmap, 1, sizeof(bitmap),fp);//se escribe el header del archivo
    memset(pixelbuffer,pixel,sizeof(DISCODURO));
    fseek (fp, headerSize, SEEK_SET);// Se salta el header para definir el inicio del archivo
    fseek (fp, 0, SEEK_END);
    fwrite(&disco,1,sizeof(DISCODURO),fp);//se escriben los datos de la estructura al archivo
    fclose(fp);
    free(mibitmap);
    free(pixelbuffer);
}
