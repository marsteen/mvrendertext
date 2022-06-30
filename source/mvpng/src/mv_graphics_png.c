//***************************************************************************
//
// @PROJECT  : mv libary
// @VERSION  : 1.0
// @FILENAME : mv_graphics_png.c
// @DATE     :
//
// @AUTHOR   :  Martin Steen
// @EMAIL    :  martin@martin-steen.de
//
//***************************************************************************

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <png.h>
#include <mvpng/inc/mv_graphics_png.h>

static int write_pngGrayscale(const char* file_name, int Width, int Height, const void* PixelData, int ByteProPixel);
static int write_png(const char* file_name, int Width, int Height, const void* PixelData, int BytePerPixel);

//---------------------------------------------------------------------------
//
// function:   mv_graphics_png_write
//
//---------------------------------------------------------------------------

int mv_graphics_png_write(const char* png_file, const char* png_data, int png_width, int png_height, int png_bits)
{
    int r;
    if ((png_bits == 16) || (png_bits == 8))
    {
        r =  write_pngGrayscale(png_file, png_width, png_height, png_data, png_bits / 8);
    }
    else
    {
        r =  write_png(png_file, png_width, png_height, png_data, png_bits / 8);
    }
    return r;
}

//---------------------------------------------------------------------------
//
// function:   mv_graphics_png_read
//
//---------------------------------------------------------------------------

int mv_graphics_png_read(const char* png_file, char** png_data, int* png_width, int* png_height, int* png_bits, size_t file_offset)
{
    png_structp png_ptr;
    png_infop info_ptr;
    FILE * fp;
    png_uint_32 width;
    png_uint_32 height;
    int bit_depth;
    int color_type;
    int interlace_method;
    int compression_method;
    int filter_method;
    int j;
    png_bytepp rows;

    fp = fopen (png_file, "rb");
    if (!fp)
    {
        return 0;
        //fatal_error ("Cannot open '%s': %s\n", png_file, strerror (errno));
    }
    png_ptr = png_create_read_struct (PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    if (!png_ptr)
    {
        return 0;
        //fatal_error ("Cannot create PNG read structure");
    }
    info_ptr = png_create_info_struct (png_ptr);
    if (!png_ptr)
    {
        return 0;
        //fatal_error ("Cannot create PNG info structure");
    }
    fseek(fp, file_offset, SEEK_SET);

    png_init_io (png_ptr, fp);
    png_read_png (png_ptr, info_ptr, 0, 0);
    png_get_IHDR (png_ptr, info_ptr, &width, &height, &bit_depth,
          &color_type, &interlace_method, &compression_method,
          &filter_method);
    rows = png_get_rows (png_ptr, info_ptr);
    int rowbytes;
    rowbytes = png_get_rowbytes(png_ptr, info_ptr);

#ifdef DEBUG_OUTPUT
    printf ("Width is %d, height is %d\n", width, height);
    printf("Row bytes = %d\n", rowbytes);
    printf("Colortype = %d\n", color_type);

    switch (color_type)
    {
        case PNG_COLOR_TYPE_RGB:       printf("PNG_COLOR_TYPE_RGB\n"); break;
        case PNG_COLOR_TYPE_PALETTE:   printf("PNG_COLOR_TYPE_PALETTE \n"); break;
        case PNG_COLOR_TYPE_RGB_ALPHA: printf("PNG_COLOR_TYPE_RGB_ALPHA\n"); break;
    }
#endif

    *png_width  = width;
    *png_height = height;
    *png_bits   = (rowbytes / width) * 8;
    *png_data   = malloc(*png_width * *png_height * *png_bits / 8);

    char* PicDataPtr = *png_data;
    int BytesPerLine = (*png_width * *png_bits) / 8;
    for (int i = 0; i < *png_height; i++)
    {
        memcpy(PicDataPtr, rows[i], BytesPerLine);
        PicDataPtr += BytesPerLine;
    }
    png_destroy_read_struct(&png_ptr, &info_ptr, (png_infopp) NULL);
    fclose(fp);
    return 1;
}

//---------------------------------------------------------------------------
//
// Klasse:    global
// Methode:   write_png
//
//
//---------------------------------------------------------------------------

static int write_png(const char* file_name, int Width, int Height, const void* PixelData, int BytePerPixel)
{
    FILE* fp;
    int   i;
    png_bytepp RowPointers;
    png_structp png_ptr;
    png_infop info_ptr;
    png_colorp palette;
    int png_transforms = PNG_TRANSFORM_BGR;
    /* open the file */
    fp = fopen(file_name, "wb");
    if (fp == NULL)
    {
        return 0;
    }

    RowPointers = (png_bytepp) malloc(Height * sizeof(void*));

    for (i = 0; i < Height; i++)
    {
        RowPointers[i] = ((unsigned char*) PixelData) + (i * Width * BytePerPixel);
    }

    /* Create and initialize the png_struct with the desired error handler
    * functions.  If you want to use the default stderr and longjump method,
    * you can supply NULL for the last three parameters.  We also check that
    * the library version is compatible with the one used at compile time,
    * in case we are using dynamically linked libraries.  REQUIRED.
    */

   png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);

    if (png_ptr == NULL)
    {
        fclose(fp);
        return 0;
    }

    /* Allocate/initialize the image information data.  REQUIRED */
    info_ptr = png_create_info_struct(png_ptr);
    if (info_ptr == NULL)
    {
        fclose(fp);
        png_destroy_write_struct(&png_ptr,  (png_infopp) NULL);
        return 0;
    }

    png_set_rows(png_ptr, info_ptr, RowPointers);

    switch (BytePerPixel)
    {
        case 3:
            png_set_IHDR(png_ptr, info_ptr, Width, Height, 8, PNG_COLOR_TYPE_RGB,
            PNG_INTERLACE_NONE, PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE);
            break;

        case 4:

            png_set_IHDR(png_ptr, info_ptr, Width, Height, 8, PNG_COLOR_TYPE_RGBA,
            PNG_INTERLACE_NONE, PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE);
            break;

        default:

            /* unsupported Pixel format */
            break;

    }

    /* Set error handling.  REQUIRED if you aren't supplying your own
    * error handling functions in the png_create_write_struct() call.
    */
    if (setjmp(png_jmpbuf(png_ptr)))
    {
        /* If we get here, we had a problem reading the file */
        fclose(fp);
        png_destroy_write_struct(&png_ptr, &info_ptr);
        return 0;
    }

    png_init_io(png_ptr, fp);

    /* This is the easy way.  Use it if you already have all the
    * image info living info in the structure.  You could "|" many
    * PNG_TRANSFORM flags into the png_transforms integer here.
    */
    png_write_png(png_ptr, info_ptr, png_transforms, (png_infopp) NULL);


    /* clean up after the write, and free any memory allocated */
    png_destroy_write_struct(&png_ptr, &info_ptr);

    free(RowPointers);

    /* close the file */
    fclose(fp);

    /* that's it */
    return 1;
}

//---------------------------------------------------------------------------
//
// Klasse:    static
// Methode:   write_pngGrayscale
//
//---------------------------------------------------------------------------

static int write_pngGrayscale(const char* file_name, int Width, int Height, const void* PixelData, int ByteProPixel)
{
    FILE* fp;
    int   i;
    png_bytepp   RowPointers;
    png_structp  png_ptr;
    png_infop    info_ptr;
    png_colorp   palette;
     int png_transforms = PNG_TRANSFORM_BGR;
    /* open the file */
    fp = fopen(file_name, "wb");
    if (fp == NULL)
    {
        return 0;
    }

    RowPointers = (png_bytepp) malloc(Height * sizeof(void*));

    for (i = 0; i < Height; i++)
    {
        RowPointers[i] = ((unsigned char*) PixelData) + (i * Width * ByteProPixel);
    }

    /* Create and initialize the png_struct with the desired error handler
    * functions.  If you want to use the default stderr and longjump method,
    * you can supply NULL for the last three parameters.  We also check that
    * the library version is compatible with the one used at compile time,
    * in case we are using dynamically linked libraries.  REQUIRED.
    */

    png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);

    if (png_ptr == NULL)
    {
        fclose(fp);
        return 0;
    }

    /* Allocate/initialize the image information data.  REQUIRED */
    info_ptr = png_create_info_struct(png_ptr);
    if (info_ptr == NULL)
    {
        fclose(fp);
        png_destroy_write_struct(&png_ptr,  (png_infopp) NULL);
        return 0;
    }

    png_set_rows(png_ptr, info_ptr, RowPointers);

    png_set_IHDR(png_ptr, info_ptr, Width, Height, ByteProPixel * 8, PNG_COLOR_TYPE_GRAY,
    PNG_INTERLACE_NONE, PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE);


    /* Set error handling.  REQUIRED if you aren't supplying your own
    * error handling functions in the png_create_write_struct() call.
    */
    if (setjmp(png_jmpbuf(png_ptr)))
    {
        /* If we get here, we had a problem reading the file */
        fclose(fp);
        png_destroy_write_struct(&png_ptr, &info_ptr);
        return 0;
    }

    png_init_io(png_ptr, fp);

    /* This is the easy way.  Use it if you already have all the
    * image info living info in the structure.  You could "|" many
    * PNG_TRANSFORM flags into the png_transforms integer here.
    */
    png_write_png(png_ptr, info_ptr, png_transforms, NULL);


    /* clean up after the write, and free any memory allocated */
    png_destroy_write_struct(&png_ptr, &info_ptr);

    free(RowPointers);

    /* close the file */
    fclose(fp);

    /* that's it */
    return 1;
}




