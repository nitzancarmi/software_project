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
 * Imports all features of the images that are defined in the configuration object,
 * from feature file of the type ".feats".
 * Expected name of the .feats files are as the image filename, only with the extension ".feats"
 * instead of the image extension (as the pattern defined in the configuration object).
 * If an image exists but the file cannot be opened (read error or file does not exist),
 * the .feats file is IGNORED and the function skips to the next index.
 *
 * Format of the file is expected to be exactly as created by the function exportImageToFile,
 * which is the following format (for simplicity assume that
 * pointArray[i][j] = i'th double of the j'th point):
 *
 * image_index
 * pointArray_size
 * pointArray[0][0] pointArray[0][1] pointArray[0][2] ... pointArray[0][m]
 * ...
 * pointArray[n][0] pointArray[n][1] pointArray[n][2] ... pointArray[n][m]
 *
 * (END OF FILE)
 *
 * Expecting points to be separated with exactly one space (no space after the last point in each line).
 * No empty lines are permitted in the file.
 * The file must contain pointArray_size features exactly, and each feature has to be
 * exactly as defined in spCADimension in the configuration object.
 *
 * If one of the mentioned constraints were not met than the .feats file is
 * IGNORED and the function skips to the next index.
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
SPPoint* extractImagesFeatures(int* totalNumOfFeaturesPtr, SPConfig config,
		SP_LOGGER_MSG* log_msg, SP_CONFIG_MSG* conf_msg);

#endif /* SPEXTRACTOR_H_ */
