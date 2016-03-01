//
//  mergMarkdown.c
//  mergMarkdown
//
//  Created by Monte Goulding on 24/03/13.
//  Copyright (c) 2013 mergExt. All rights reserved.
//

#include <stdio.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "external.h"

#define LIVECODE_FUNCTION(x) void x(char *p_arguments[], int p_argument_count, char **r_result, Bool *r_pass, Bool *r_err)
#define LIVECODE_ERROR(x) { *r_err = True; 		*r_pass = False; 		*r_result = strdup(x); 		return; }

#define LIVECODE_WRITEVARIABLE(var,number) { \
SetVariable(p_arguments[ number ],var, &success); \
if (success == EXTERNAL_FAILURE) { \
LIVECODE_ERROR("Could not write variable"); \
} \
}

#define LIVECODE_ARG(argn) { if(p_argument_count < argn) { 	LIVECODE_ERROR("Incorrect number of arguments"); }}

#define LIVECODE_RETURN_THIS_STRING(x) { \
*r_err = False; *r_pass = False; \
if (x == NULL) { *r_result = strdup(""); } \
else { *r_result = x; }}

#define LIVECODE_NOERROR { *r_err = False; *r_pass = False; *r_result = strdup(""); }

#ifndef max
    #define max( a, b ) ( ((a) > (b)) ? (a) : (b) )
#endif

#ifndef min
    #define min( a, b ) ( ((a) < (b)) ? (a) : (b) )
#endif


// Command:
//   blur
// Parameters:
//   p1: blur radius
//   p2: name of variable containing image data
//   p3: image width
//   p4: image height
//   p5: name of variable to output new image data to
// -- the following are ootional
//   p6: name of variable to output alpha data to
//   p7: number of pixels to alpha fade out. Default is 5.
//   p8: feather the left side. If not set it will be feathered
//   p9: feather the top side. If not set it will be feathered
//   p10: feather the right side. If not set it will be feathered
//   p11: feather the bottom side. If not set it will be feathered
// Result:
//   Error message
// Description:
//   Blurs the passed in image data.
//
LIVECODE_FUNCTION(blur)
{
    LIVECODE_ARG(5);
    
    int retvalue;
	*r_pass = False;
	*r_err = False;
	
	const char *t_error;
	
	t_error = NULL;
	
	// Check variables passed in
	int radius = atoi(p_arguments[0]);
	int imgWidth = atoi(p_arguments[2]);
	int imgHeight = atoi(p_arguments[3]);
	int theY = 0, theX = 0;
	int g = 0, sum = 0;
	int rSum = 0, gSum = 0, bSum = 0, aSum = 0;
	int theR = 0, theG = 0, theB = 0;
	int byteOffset = 0, byteToInsertInto = 0, pixelToRead = 0, radiusX = 0, radiusY = 0, baseRadiusY = 0;
	int i = 0;
	
    ExternalString imgData;
	char *interimImgData = NULL;
	char *newImgData = NULL;
    
    ExternalString alphaData;
    bool feather_left;
    bool feather_top;
    bool feather_right;
    bool feather_bottom;
    
    char *newAlphaData = NULL;
    
    int maxBlend;
    
    if (p_argument_count > 5)
    {
        feather_left = true;
        feather_top = true;
        feather_right = true;
        feather_bottom = true;
        
        newAlphaData = (char*)malloc((int)(imgWidth * imgHeight));
        
        maxBlend = atoi(p_arguments[6]); //MAX(5, atoi(p_arguments[6]));
        
        maxBlend = min( (int)((double)imgHeight / 2), maxBlend);
        maxBlend = min( (int)((double)imgWidth / 2), maxBlend);
        
        if (p_argument_count > 7) feather_left = strcmp(p_arguments[7], "true") == 0;
        if (p_argument_count > 8) feather_top = strcmp(p_arguments[8], "true") == 0;
        if (p_argument_count > 9) feather_right = strcmp(p_arguments[9], "true") == 0;
        if (p_argument_count > 10) feather_bottom = strcmp(p_arguments[10], "true") == 0;
    }

	// Generate Gaussian Kernel
	radius = min(max(1,radius), 248);
	int kernelSize = 1+radius*2;
	int *kernel = new int[kernelSize];
	//int kernel[kernelSize];
	
	//Gaussian filter
	for (i = 0; i<radius;i++) {
		g = i*i+1;
		kernel[i] = kernel[kernelSize-i-1] = g;
		sum+=g*2;
	}
	g = radius*radius;
	kernel[radius] = g;
	sum+=g;
	
	GetVariableEx(p_arguments[1], "", &imgData, &retvalue); //get image
	if (imgData.buffer == NULL) {
		t_error = "unable to get image data";
	}
    
	if (t_error == NULL) {
		if (imgData.length != (imgWidth * imgHeight * 4))
			t_error = "invalid image heigth/width";
	}
	
	if (t_error == NULL) {
		interimImgData = (char*)malloc(imgData.length);
		newImgData = (char*)malloc(imgData.length);
		
		// Horizontal Blur
		for (theY = 1; theY <= imgHeight; theY++) {
            
			byteOffset = ((theY-1) * (imgWidth * 4));
			
			for (theX = 1; theX <= imgWidth; theX++) {
				rSum = 0, gSum = 0, bSum = 0, aSum = 0;
				
				// blur it
				radiusX = theX - radius;
				
				for (i = 0; i < kernelSize; i++) {
					pixelToRead = radiusX + i - 1;
					
					if (pixelToRead < 0) {
						// use pixel on extreme left
						pixelToRead = 0;
					} else if (pixelToRead >= imgWidth) {
						// use pixel on extreme right
						pixelToRead = imgWidth - 1;
					}
					
					pixelToRead *= 4;
					pixelToRead += byteOffset;
					
					// Get pixel components
					theR = (imgData.buffer[pixelToRead+1]) & 0xff;
					theG = (imgData.buffer[pixelToRead+2]) & 0xff;
					theB = (imgData.buffer[pixelToRead+3]) & 0xff;
					
					// update sums
					rSum += theR * kernel[i];
					gSum += theG * kernel[i];
					bSum += theB * kernel[i];
				}
				
				// Stick pixel into new data
				interimImgData[byteToInsertInto] = 0x00;
				interimImgData[byteToInsertInto+1] = (char)((double)rSum/sum);
				interimImgData[byteToInsertInto+2] = (char)((double)gSum/sum);
				interimImgData[byteToInsertInto+3] = (char)((double)bSum/sum);
                
				byteToInsertInto += 4;
			}
		}
		
		// Vertical Blur
		byteToInsertInto = 0;
		
		for (theY = 1; theY <= imgHeight; theY++) {
			baseRadiusY = theY - radius;
			
			for (theX = 1; theX <= imgWidth; theX++) {
				rSum = 0, gSum = 0, bSum = 0, aSum = 0;
                
				for (i = 0; i < kernelSize; i++) {
					radiusY = baseRadiusY + i - 1;
					
					if (radiusY < 0) {
						// use pixel on extreme left
						radiusY = 0;
					} else if (radiusY >= imgHeight) {
						// use pixel on extreme right
						radiusY = imgHeight - 1;
					}
					
					byteOffset = radiusY * (imgWidth * 4);
                    
					pixelToRead = byteOffset + ((theX-1) * 4);
					
					// Get pixel components
					theR = (interimImgData[pixelToRead+1]) & 0xff;
					theG = (interimImgData[pixelToRead+2]) & 0xff;
					theB = (interimImgData[pixelToRead+3]) & 0xff;
					
					// update sums
					rSum += theR * kernel[i];
					gSum += theG * kernel[i];
					bSum += theB * kernel[i];
				}
				
				// Stick pixel into new data
				newImgData[byteToInsertInto] = 0x00;
				newImgData[byteToInsertInto+1] = (char)((double)rSum/sum);
				newImgData[byteToInsertInto+2] = (char)((double)gSum/sum);
				newImgData[byteToInsertInto+3] = (char)((double)bSum/sum);
                
				byteToInsertInto += 4;
			}
		}
	}
	
    if (p_argument_count > 5)
    {
        if (t_error == NULL) {
            // Finish off alpha blur
            for (theY = 1; theY <= imgHeight; theY++) {
                byteOffset = ((theY-1) * imgWidth) - 1;
                
                for (theX = 1; theX <= imgWidth; theX++) {
                    if (feather_top && theY <= maxBlend) {
                        // Do we blend with X?
                        if (feather_left && theX <= maxBlend) {
                            /*
                             // http://en.wikipedia.org/wiki/Hypotenuse
                             double hypotenuse = Math.sqrt (Math.pow (radius - col - 1, 2.0) + Math.pow (radius - 1 - row, 2.0));
                             // calculate alpha based on percent distance from image
                             alphas[col] = alphas[alphas.length - col - 1] = (byte)(opacity * Math.max (((radius - hypotenuse) / radius), 0));
                             */
                            double hypotenuse = sqrt( pow((double)maxBlend - theX - 1, 2) + pow((double)maxBlend - theY - 1, 2) );
                            newAlphaData[byteOffset + theX] = (char)(255 * max(0, ((maxBlend - hypotenuse) / maxBlend)));
                            
                        } else if (feather_right && theX > (imgWidth - maxBlend)) {
                            double hypotenuse = sqrt( pow((double)maxBlend - (imgWidth - theX), 2) + pow((double)maxBlend - theY, 2) );
                            newAlphaData[byteOffset + theX] = (char)(255 * max(0, ((maxBlend - hypotenuse) / maxBlend)));
                            
                        } else { // just fill in Y
                            newAlphaData[byteOffset + theX] = (char)(255 * max(0, (double)theY / maxBlend));
                        }
                    } else if (feather_bottom && theY > (imgHeight - maxBlend)) {
                        if (feather_left && theX <= maxBlend) {
                            double hypotenuse = sqrt( pow((double)maxBlend - theX, 2) + pow((double)maxBlend - (imgHeight - theY), 2) );
                            newAlphaData[byteOffset + theX] = (char)(255 * max(0, ((maxBlend - hypotenuse) / maxBlend)));
                            
                        } else if (feather_right && theX > (imgWidth - maxBlend)) {
                            double hypotenuse = sqrt( pow((double)maxBlend - (imgWidth - theX), 2) + pow((double)maxBlend - (imgHeight - theY), 2) );
                            newAlphaData[byteOffset + theX] = (char)(255 * max(0, ((maxBlend - hypotenuse) / maxBlend)));
                            
                        } else // just fill in Y
                            newAlphaData[byteOffset + theX] = (char)(255 * max(0, (double)(imgHeight - theY) / maxBlend));
                        
                    } else {
                        if (feather_left && theX <= maxBlend)
                            newAlphaData[byteOffset + theX] = (char)(255 * max(0, ((double)theX / maxBlend)));
                        else if (feather_right && theX > (imgWidth - maxBlend))
                            newAlphaData[byteOffset + theX] = (char)(255 * max(0, (double)(imgWidth - theX) / maxBlend));
                        else
                            newAlphaData[byteOffset + theX] = (char)255;
                    }
                }
            }
        }
    }
	
	if (t_error == NULL) {
		imgData.buffer = newImgData;
		SetVariableEx(p_arguments[4],"",&imgData, &retvalue);
		
        if (p_argument_count > 5)
        {
            alphaData.length = imgWidth * imgHeight;
            alphaData.buffer = newAlphaData;
            SetVariableEx(p_arguments[5],"",&alphaData, &retvalue);
        }
		
		LIVECODE_NOERROR;
	} else {
        LIVECODE_RETURN_THIS_STRING(strdup(t_error));
	}
	
	if (interimImgData) free(interimImgData);
	if (newImgData) free(newImgData);
	if (newAlphaData) free(newAlphaData);
    
	delete [] kernel;
	kernel = NULL;
       
}

EXTERNAL_BEGIN_DECLARATIONS("blur")
EXTERNAL_DECLARE_COMMAND("blur", blur)
EXTERNAL_END_DECLARATIONS

#if __IPHONE_OS_VERSION_MIN_REQUIRED
  EXTERNAL_LIBINFO(blur)
#endif
