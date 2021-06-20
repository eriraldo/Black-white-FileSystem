#include "bmpGen.h"
#include "structure.h"

extern char* folder;
extern char* almacenamiento;
extern DISCODURO HDD;
void generarBMP(){
    int asd = sizeof(DISCODURO);

    int asd2 = _pixelbytesize;

    FILE *fp = fopen(almacenamiento,"wb");
    bitmap *pbitmap  = (bitmap*)calloc(1,sizeof(bitmap));
    uint8_t *pixelbuffer = (uint8_t*)malloc(sizeof(DISCODURO));
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
    memset(pixelbuffer,pixel,sizeof(DISCODURO));
    //fwrite(pixelbuffer,1,sizeof(DISCODURO),fp);

    fseek (fp, headerSize, SEEK_SET);
    fseek (fp, 0, SEEK_END);
    int size2 = ftell(fp);

    fwrite(&HDD,1,sizeof(DISCODURO),fp);
    fclose(fp);
    free(pbitmap);
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