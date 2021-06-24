#include "bmpGen.h"
#include "structure.h"

extern char* folder;
extern char* almacenamiento;
extern DISCODURO HDD;
void generarBMP(){
    //int asd = sizeof(DISCODURO);

    //int asd2 = _pixelbytesize;

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
    fwrite (mibitmap, 1, sizeof(bitmap),fp);
    memset(pixelbuffer,pixel,sizeof(DISCODURO));
    //fwrite(pixelbuffer,1,sizeof(DISCODURO),fp);

    fseek (fp, headerSize, SEEK_SET);
    fseek (fp, 0, SEEK_END);
    int size2 = ftell(fp);

    fwrite(&HDD,1,sizeof(DISCODURO),fp);
    fclose(fp);
    free(mibitmap);
    free(pixelbuffer);
}

void readBMP(){
    int asd = sizeof(DISCODURO);
    FILE *fp = fopen(almacenamiento,"rb");
    fseek (fp, 0, SEEK_END);
    fseek (fp, headerSize, SEEK_SET);
    FILE *fpTest = fopen("test.dat", "wb");
    unsigned char buffer[sizeof(DISCODURO)];
    fread(buffer,sizeof(buffer),1,fp);
    fseek (fpTest, 0, SEEK_END);
    fwrite(buffer,sizeof(buffer),1,fpTest);
    /*
    fseek (fpTest, 0, SEEK_SET);
    fread(fpTest,size,1,fp);*/
    fclose(fp);
    fclose(fpTest);
}