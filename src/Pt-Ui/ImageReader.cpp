/* Copyright (C) 2013 Laurentiu-Gheorghe Crisan 
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * As a special exception, you may use this file as part of a free
 * software library without restriction. Specifically, if other files
 * instantiate templates or use macros or inline functions from this
 * file, or you compile this file and link it with other files to
 * produce an executable, this file does not by itself cause the
 * resulting executable to be covered by the GNU General Public
 * License. This exception does not however invalidate any other
 * reasons why the executable file might be covered by the GNU Library
 * General Public License.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA*/
#include <Pt/Ui/ImageReader.h>
#include <fstream>

#include "png.h"

namespace Pt{
namespace Ui{
    
void userReadData(png_structp pngPtr, png_bytep data, png_size_t length)
{
    //Here we get our IO pointer back from the read struct.
    //This is the parameter we passed to the png_set_read_fn() function.
    //Our std::istream pointer.
    png_voidp a = png_get_io_ptr(pngPtr);
    //Cast the pointer to std::istream* and read 'length' bytes into 'data'
    ((std::istream*)a)->read((char*)data, length);
}
    
Image* ImageReader::read(std::istream& source)
{
    enum{PNGSIGSIZE = 8};
    
    //Allocate a buffer of 8 bytes, where we can put the file signature.
    png_byte pngsig[PNGSIGSIZE];
    int is_png = 0;
    
    //Read the 8 bytes from the stream into the sig buffer.
    source.read((char*)pngsig, PNGSIGSIZE);
    
    //Check if the read worked...
    if (!source)
         throw std::runtime_error("ERROR: Couldn't initialize png info struct" );
    
    //Let LibPNG check the sig. If this function returns 0, everything is OK.
    is_png = png_sig_cmp(pngsig, 0, PNGSIGSIZE);
    
    if(is_png != 0)
         throw std::runtime_error("ImageReader: wrong file format");
    
    //Here we create the png read struct. The 3 NULL's at the end can be used
    //for your own custom error handling functions, but we'll just use the default.
    //if the function fails, NULL is returned. Always check the return values!
    png_structp pngPtr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    
    if (!pngPtr)
    {
        throw std::runtime_error("ERROR: Couldn't initialize png read struct" );
    }
    
    //Here we create the png info struct.
    //Note that this time, if this function fails, we have to clean up the read struct!
    png_infop infoPtr = png_create_info_struct(pngPtr);
    
    if (!infoPtr)
    {
        png_destroy_read_struct(&pngPtr, (png_infopp)0, (png_infopp)0);
        throw std::runtime_error("ERROR: Couldn't initialize png info struct" );        
    }
    
    //Here I've defined 2 pointers up front, so I can use them in error handling.
    //I will explain these 2 later. Just making sure these get deleted on error.
    png_bytep* rowPtrs = NULL;
    char* data = NULL;
    
    if (setjmp(png_jmpbuf(pngPtr)))
    {
        //An error occured, so clean up what we have allocated so far...
        png_destroy_read_struct(&pngPtr, &infoPtr,(png_infopp)0);
        
        if (rowPtrs != NULL) delete [] rowPtrs;
       
        if (data != NULL) delete [] data;
        
        throw std::runtime_error("ERROR: An error occured while reading the PNG file");
    }
    
    png_set_read_fn(pngPtr,(png_voidp)&source, userReadData);

    
    //Set the amount signature bytes we've already read:
    //We've defined PNGSIGSIZE as 8;
    png_set_sig_bytes(pngPtr, PNGSIGSIZE);
    
    //Now call png_read_info with our pngPtr as image handle, and infoPtr to receive the file info.
    png_read_info(pngPtr, infoPtr);
    
    png_uint_32 imgWidth =  png_get_image_width(pngPtr, infoPtr);
    png_uint_32 imgHeight = png_get_image_height(pngPtr, infoPtr);
    
    //bits per CHANNEL! note: not per pixel!
    png_uint_32 bitdepth   = png_get_bit_depth(pngPtr, infoPtr);
    //Number of channels
    png_uint_32 channels   = png_get_channels(pngPtr, infoPtr);
    //Color type. (RGB, RGBA, Luminance, luminance alpha... palette... etc)
    png_uint_32 color_type = png_get_color_type(pngPtr, infoPtr);
    
    switch (color_type)
    {
        case PNG_COLOR_TYPE_PALETTE:
            png_set_palette_to_rgb(pngPtr);
            //Don't forget to update the channel info (thanks Tom!)
            //It's used later to know how big a buffer we need for the image
            channels = 3;
        break;
        
        case PNG_COLOR_TYPE_GRAY:
            if (bitdepth < 8)
                png_set_expand_gray_1_2_4_to_8(pngPtr);
            //And the bitdepth info
            bitdepth = 8;
        break;
    }
    
    /*if the image has a transperancy set.. convert it to a full Alpha channel..*/
    if (png_get_valid(pngPtr, infoPtr, PNG_INFO_tRNS))
    {
        png_set_tRNS_to_alpha(pngPtr);
        channels+=1;
    }
    
    //We don't support 16 bit precision.. so if the image Has 16 bits per channel
    //precision... round it down to 8.
    if (bitdepth == 16)
        png_set_strip_16(pngPtr);
    
    //Here's one of the pointers we've defined in the error handler section:
    //Array of row pointers. One for every row.
    rowPtrs = new png_bytep[imgHeight];
    
    //Alocate a buffer with enough space.
    //(Don't use the stack, these blocks get big easilly)
    //This pointer was also defined in the error handling section, so we can clean it up on error.
    data	 = new char[imgWidth * imgHeight * bitdepth * channels / 8];
    //This is the length in bytes, of one row.
    const unsigned int stride = imgWidth * bitdepth * channels / 8;
    
    //A little for-loop here to set all the row pointers to the starting
    //Adresses for every row in the buffer
    
    for (size_t i = 0; i < imgHeight; i++)
    {
        //Set the pointer to the data pointer + i times the row stride.
        //Notice that the row order is reversed with q.
        //This is how at least OpenGL expects it,
        //and how many other image loaders present the data.
        png_uint_32 q = (imgHeight- i - 1) * stride;
        rowPtrs[i] = (png_bytep)data + q;
    }
    
    //And here it is! The actuall reading of the image!
    //Read the imagedata and write it to the adresses pointed to
    //by rowptrs (in other words: our image databuffer)
    png_read_image(pngPtr, rowPtrs);
	    


		  Image* image = new Image( Ui::Size(imgWidth, imgHeight), Ui::ImageFormat::argb8888() );				  
    
			for( size_t x = 0; x < imgWidth; ++x)
			{
				for( size_t y = 0; y < imgHeight; ++y)
				{

					if( bitdepth == 8 && channels == 3)
					{
						unsigned char *red = (unsigned char *)rowPtrs[ y ] + channels * x;
						unsigned char *green = (unsigned char *)rowPtrs[ y ] + channels * x + 1;
						unsigned char *blue = (unsigned char *)rowPtrs[ y ] + channels * x + 2;		
						image->setColor(x,y, Ui::Color(0, (*red)/255.0, (*green)/255.0, (*blue)/255.0));
					}


					if( bitdepth == 8 && channels == 4)
					{
						unsigned char *red = (unsigned char *)rowPtrs[ y ] + channels * x;
						unsigned char *green = (unsigned char *)rowPtrs[ y ] + channels * x + 1;
						unsigned char *blue = (unsigned char *)rowPtrs[ y ] + channels * x + 2;		
						unsigned char *alpha = (unsigned char *)rowPtrs[ y ] + channels * x + 3;
						image->setColor(x,y, Ui::Color( (*alpha)/255.0, (*red)/255.0, (*green)/255.0, (*blue)/255.0));
					}
				}
			}
    //Delete the row pointers array....
    delete[] (png_bytep)rowPtrs;
    
    //And don't forget to clean up the read and info structs !
    png_destroy_read_struct(&pngPtr, &infoPtr,(png_infopp)0);
    return image;
}


Image* ImageReader::read(const char* file)
{
    std::fstream stream(file, std::ios_base::in|std::ios_base::binary);
    
    if( !stream)
        throw std::runtime_error("ImageReader: Open file failed");
    
    return read(stream);
}

    
}}


