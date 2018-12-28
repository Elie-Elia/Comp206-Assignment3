/* FILE: A3_solutions.c is where you will code your answers for Assignment 3.
 * 
 * Each of the functions below can be considered a start for you. They have 
 * the correct specification and are set up correctly with the header file to
 * be run by the tester programs.  
 *
 * You should leave all of the code as is, especially making sure not to change
 * any return types, function name, or argument lists, as this will break
 * the automated testing. 
 *
 * Your code should only go within the sections surrounded by
 * comments like "REPLACE EVERTHING FROM HERE... TO HERE.
 *
 * The assignment document and the header A3_solutions.h should help
 * to find out how to complete and test the functions. Good luck!
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "A3_provided_functions.h"

unsigned char*
bmp_open( char* bmp_filename,        unsigned int *width,
          unsigned int *height,      unsigned int *bits_per_pixel,
          unsigned int *padding,     unsigned int *data_size,
          unsigned int *data_offset                                  )
{
    unsigned char *img_data=NULL;
    if(bmp_filename == NULL || height == NULL || width == NULL || padding == NULL || data_offset == NULL || data_size == NULL || bits_per_pixel == NULL)
        return NULL;
    FILE *img = fopen(bmp_filename,"rb");
    if (img==NULL){
        return NULL;
    }
    unsigned char header[54];
    fread(header,sizeof(char),54,img);
    *width=*(unsigned int*)&header[18];
    *height=*(unsigned int*)&header[22];
    *bits_per_pixel=*(unsigned short int*)&header[28];
    *data_size=*(unsigned int*)&header[34];
    *data_offset=*(unsigned int*)&header[10];

    int lastarrayslot = (*width * *bits_per_pixel) %32;
    if(lastarrayslot ==0){
        *padding=0;
    }else{
        *padding = (32-lastarrayslot)/8;
    }

    size_t sz;
    fseek(img, 0L, SEEK_END);
    sz=ftell(img);
    rewind(img);

    unsigned char *img_data_array=(unsigned char*) malloc((sz)*sizeof(char));
    fread(img_data_array,1,sz,img);
    img_data=img_data_array;
    rewind(img);
    return img_data;
}

void
bmp_close( unsigned char **img_data ) {

    if(img_data == NULL)
        return;
    if(*img_data == NULL) {
        return;
    }
    if(img_data[0][0] != 'B' || img_data[0][1] != 'M')
        return;


    free(*img_data);
    img_data = NULL;


}


unsigned char***
bmp_scale( unsigned char*** pixel_array, unsigned char* header_data, unsigned int header_size,
           unsigned int* width, unsigned int* height, unsigned int num_colors,
           float scale )
{
    unsigned char*** new_pixel_array = NULL;
    if(scale<=0 || height==NULL || *height==0 || width == NULL || *width==0 || header_size == 0 || num_colors==0 || header_data==NULL
       || pixel_array == NULL || *pixel_array ==NULL || **pixel_array == NULL)
        return NULL;
    int calcwidth=(int)(*width * scale);
    int calcheight=(int)(*height * scale);
    new_pixel_array = (unsigned char***)malloc( sizeof(unsigned char*) * (calcheight));
    for( int row=0; row<calcheight; row++ ){
        new_pixel_array[row] = (unsigned char**)malloc( sizeof(unsigned char*) * (calcwidth) );
        for( int col=0; col<calcwidth; col++ ){
            new_pixel_array[row][col] = (unsigned char*)malloc( sizeof(unsigned char) * (num_colors) );
        }
    }

    for( int row=0; row<calcheight; row++ )
        for( int col=0; col<calcwidth; col++ )
            for( int color=0; color<num_colors; color++ )
                new_pixel_array[row][col][color] = pixel_array[(int)(row/scale)][(int)(col/scale)][color];

    *(int*)&header_data[34]=(int)(*((int*)&header_data[34]) * scale);
    *(int*)&header_data[18]=calcwidth;
    *(int*)&header_data[22]=calcheight;
    *width=calcwidth;
    *height=calcheight;
    return new_pixel_array;
}

int
bmp_collage( char* background_image_filename,     char* foreground_image_filename,
             char* output_collage_image_filename, int row_offset,
             int col_offset,                      float scale )
{
    char* bg_filename=background_image_filename;
    unsigned char* header_data;
    unsigned int header_size, width, height, num_colors;
    unsigned char*** background=bmp_to_3D_array(bg_filename,&header_data,&header_size,&width,&height,&num_colors);
    if(foreground_image_filename == NULL || background_image_filename == NULL || output_collage_image_filename == NULL){
        return -1;
    }
    if(num_colors != 4){
        return -1;
    }
    if(background == NULL){
        return -1;
    }

    char* fg_filename=foreground_image_filename;
    unsigned char* header_datafg;
    unsigned int header_sizefg, widthfg, heightfg, num_colorsfg;
    unsigned char*** foreground=bmp_to_3D_array(fg_filename,&header_datafg,&header_sizefg,&widthfg,&heightfg,&num_colorsfg);
    if(num_colorsfg != 4){
        return -1;
    }
    if(foreground == NULL){
        return -1;
    }


    foreground=bmp_scale(foreground,header_datafg,header_sizefg,&widthfg,&heightfg,num_colorsfg,scale);
    if( (widthfg+col_offset)>width || (heightfg+row_offset) >height) {
        return -1;
    }
    if(foreground == NULL){
        return -1;
    }

    for( int row=0; row<heightfg; row++ )
        for( int col=0; col<widthfg; col++ )
                if (foreground[row][col][0]==0)
                    continue;
                else
                    for(int color=0 ; color<4; color++)
                        background[row+row_offset][col+col_offset][color] = foreground[row][col][color];
                    char* output_filename=output_collage_image_filename;
if((bmp_from_3D_array(output_filename,header_data,header_size,background,width,height,num_colors) ==-1))
return -1;



    return 0;
}              

