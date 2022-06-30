#ifndef NPNGLIBINTERFACE_H
#define NPNGLIBINTERFACE_H

#ifdef __cplusplus
extern "C"
{
#endif

int mv_graphics_png_write(const char* png_file, const char* png_data, int png_width, int png_height, int png_bits);
int mv_graphics_png_read(const char* png_file, char** png_data, int* png_width, int* png_height, int* png_bits, size_t file_offset);

#ifdef __cplusplus
}
#endif

#endif

