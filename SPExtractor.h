/*
 * SPExtractor.h
 *
 *  Created on: Aug 8, 2016
 *      Author: almog6564
 */

#ifndef SPEXTRACTOR_H_
#define SPEXTRACTOR_H_

#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "SPPoint.h"
#include "SPConfig.h"
#include "KDArray.h"
#include "macros.h"


/*
 * Exports image properties to a .feats file. The .feats file will be created in the same folder
 * as the image and will have the same filename, only with the extension ".feats" instead of the
 * image extension (as the pattern defined in the configuration object).
 *
 * Format of the file will be the following (for simplicity assume that
 * pointArray[i][j] = i'th double of the j'th point):
 *
 * image_index
 * pointArray_size
 * pointArray[0][0] pointArray[0][1] pointArray[0][2] ... pointArray[0][m]
 * ...
 * pointArray[n][0] pointArray[n][1] pointArray[n][2] ... pointArray[n][m]
 *
 * (END OF FILE)
 * The points are separated with exactly one space (no space after the last point in each line).
 * No new line is inserted at the end of the file.
 *
 * @param pointArray 	 	 SPPoint array contains the image's features
 * @param pointArray_size	 size of pointArray (number of the image's features)
 * @param image_index		 The index of the image in its directory
 * @param config			 Configuration attributes
 *
 * @return
 * 1 if failure for one of the following reasons:
 * 		- SPConfig access error
 * 		- .feats file cannot be created (or Memory Allocation Failure)
 * 		- Writing error to .feats file
 * 0 for successful operation
 */
int exportImageToFile(SPPoint* pointArray, int pointArray_size, int image_index, SPConfig config);

/*
 * This function imports all features of the images that are defined in the configuration object,
 * from feature file of the type ".feats", and allocates a SPPoint array to conatin all of them.
 * The size of the array will be stored in *totalNumOfFeaturesPtr.
 *
 * The following constraints must be met:
 * 1.	Expected name of the .feats files are as the image filename, only with the extension ".feats"
 * 		instead of the image extension (as the pattern defined in the configuration object).
 * 2.	Feats file for every image must exist and be readable.
 * 3.	Format of the file is expected to be as created by the function exportImageToFile.
 * 		@see exportImageToFile
 * 4.	Integers are expected to be positive (index can be 0, number of integers must be > 0).
 * 5.	Image index inside file (first line) is expected to be the same as in the filename.
 * 6.	Points are separated with space characters (Space characters are permitted before .
 * 		and after each number in the file (integers and doubles).
 * 7.	File contains no empty lines.
 * 8.	The file contains the same amount of features as written on the second line of the file exactly,
 * 		and each feature's size has to be exactly as defined in spCADimension in the configuration object.
 *
 * If one of the mentioned constraints were not met than the .feats file is
 * IGNORED and the function SKIPS to the next index.
 * The returned array will contain only the features of the .feats file the met with the constraints.
 *
 * @param totalNumOfFeaturesPtr	 	 Address to the size of the
 * @param config					 Configuration attributes
 * @param log_msg					 SPLogger message for debugging purposes
 * @param conf_msg					 SPConfig message for debugging purposes
 *
 * @return
 * NULL if failure for one of the following reasons:
 * 		- ALL of the .feats file didn't meet the constrains mentioned above
 *		- config == NULL || log_msg == NULL || conf_msg == NULL
 *		- Memory allocation error
 *
 * The allocated SPPoint array on success
 */
SPPoint* extractImagesFeatures(int* totalNumOfFeaturesPtr, SPConfig config);

#endif /* SPEXTRACTOR_H_ */
