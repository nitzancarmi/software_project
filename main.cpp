#include "SPImageProc.h"
#include <cstring>
#include <cstdlib>

extern "C" {
#include "KDTree.h"
#include "SPExtractor.h"
#include <unistd.h>

}

#define FIRST_MSG "SPConfig file imported.\nLogger opened."
#define LOGGERnMSG "SPLogger Error. Message Type: %d"
#define FINISH_PRG "Program Finished Successfully"
using namespace sp;

void finishProgram(SPConfig config) {
	spLoggerPrintInfo("#Removing SPLogger and SPConfig");
	spLoggerDestroy();
	spConfigDestroy(config);

}

int main(int argc, char* argv[]) {

	SPConfig config;
	SP_CONFIG_MSG conf_msg = SP_CONFIG_SUCCESS;
	SP_LOGGER_MSG log_msg = SP_LOGGER_SUCCESS;
	fflush(stdout);
	switch (argc) {

	case 1:
		config = spConfigCreate("spcbir.config", &conf_msg);
		if (!config)
			return ERROR;
		break;

	case 3:

		if (strcmp(argv[1], "-c")) {
			printf(INVALID_COMLINE);
			return ERROR;
		}
		config = spConfigCreate(argv[2], &conf_msg);
		if (!config)
			return ERROR;

		break;

	default:
		config = NULL;
		printf(INVALID_COMLINE);
		break;
	}
	if (!config
			|| (log_msg = spLoggerCreate(NULL, SP_LOGGER_WARNING_ERROR_LEVEL))
					!= SP_LOGGER_SUCCESS) {
		finishProgram(config);
		return ERROR;
	}
	log_msg = spLoggerPrintDebug(FIRST_MSG, __FILE__, __func__, __LINE__);
	if (log_msg != SP_LOGGER_SUCCESS) {
		printf(LOGGERnMSG, log_msg);
		finishProgram(config);
	}
	printAttributes(config);

	ImageProc* pc = new ImageProc(config);
	int numOfImages = spConfigGetNumOfImages(config, &conf_msg); //No need to read conf_msg, controlling all inputs myself

	/*******************************************/
	/*********   Extraction Mode    ************/
	/*******************************************/

	if (spConfigIsExtractionMode(config, &conf_msg)) {
		for (int index = 0; index < numOfImages; index++) {
			char path[1024] = { '\0' };
			int numOfFeats = -1;
			spConfigGetImagePath(path, config, index); //No need to read conf_msg, controlling all inputs myself
			SPPoint* pointArray = pc->getImageFeatures(path, 0, &numOfFeats);

			/** Features Extraction **/
			if (exportImageToFile(pointArray, numOfFeats, index, config)) {

				/* Writing to File Error */
				spPointArrayDestroy(pointArray, numOfFeats);
				delete pc;
				finishProgram(config);
				return ERROR;
			}
			/************************/
		}
	}
	int totalNumOfFeatures = 0;
	SPPoint* arr = extractImagesFeatures(&totalNumOfFeatures, config, &log_msg,
			&conf_msg);

	/**** execute queries ****/
	char q_path[1024];
	int q_numOfFeats;
	while (1) {

		//get get image path from user
		printf("Please enter an image path:\n");
		fgets(q_path, 1024, stdin);
		q_path[strlen(q_path) - 1] = '\0';

		//check validity of output
		if (!strlen(q_path))
			break;
		if (access(q_path, F_OK) == -1) {
			printf("invalid path to image. Please try again.\n");
			continue;
		}

		//getting image features
		SPPoint *q_pp = pc->getImageFeatures(q_path, 0, &q_numOfFeats);
		if (!q_pp || !*q_pp) {
			//TODO logger message
			printf("NULL POINTER EXCEPTION");
			exit(1);
		}

		//freeing temporary resources
//            spPointArrayDestroy(q_pp); //TODO uncomment after pull
		q_pp = NULL;
	}

	printf("Exiting...\n");
	return 0;

//
//int numOfFeats = -1;
//char path[1024] = { '\0' };
//conf_msg = spConfigGetImagePath(path, config, 0);
//printf("******path = %s\n", path);
//SPPoint* pp = pc->getImageFeatures(path, 0, &numOfFeats);
//if (!pp || !*pp) {
//	printf("NULL POINTER EXCEPTION");
//	exit(1);
//}
//int i, rc = 0;
//int numOfImages = spConfigGetNumOfImages(config, &conf_msg);
//if(numOfImages < 0){
//    printf("ERROR IN NUM OF IMAGES\n");
//    exit(1);
//}
//for(i=0;i<numOfImages;i++) {
//    rc = exportImageToFile(pp, numOfFeats, i, config);
//    if(rc) {
//        printf("ERROR - in exportImagesToFile");
//        exit(1);
//    }
//}
//
//
//

//	char q_path[1024];
//	int q_numOfFeats;
//	while (1) {
//		fflush(stdout);
//		//get get image path from user
//		printf("Please enter an image path:\n");
//		fflush(stdout);
//		fgets(q_path, 1024, stdin);
//		fflush(stdout);
//		q_path[strlen(q_path) - 1] = '\0';
//
//		//check validity of output
//		if (!strlen(q_path))
//			break;
//		if (access(q_path, F_OK) == -1) {
//			printf("invalid path to image. Please try again.\n");
//			continue;
//		}
//
//		//getting image features
//		SPPoint *q_pp = pc->getImageFeatures(q_path, 0, &q_numOfFeats);
//		if (!q_pp || !*q_pp) {
//			//TODO logger message
//			printf("NULL POINTER EXCEPTION");
//			exit(1);
//		}
//
//		//freeing temporary resources
//		spPointArrayDestroy(q_pp,q_numOfFeats); //TODO uncomment after pull
//		q_pp = NULL;
//	}

	printf("Exiting...\n");

	/***************************/
	log_msg = spLoggerPrintInfo(FINISH_PRG);
	if (log_msg != SP_LOGGER_SUCCESS) {
		printf(LOGGERnMSG, log_msg);
	}
	printf(FINISH_PRG);
	finishProgram(config);
	return OK;
}

/*** first imageProc run: **/

//int numOfFeats = -1;
//char path[1024] = { '\0' };
//conf_msg = spConfigGetImagePath(path, config, 0);
//printf("******path = %s\n", path);
//ImageProc* pc = new ImageProc(config);
//SPPoint* pp = pc->getImageFeatures(path, 0, &numOfFeats);
//if (!pp || !*pp) {
//	printf("NULL POINTER EXCEPTION");
//	exit(1);
//}
////int dims = spConfigGetPCADim(config, &conf_msg);
////printf("rows = %d, cols = %d\n", dims, numOfFeats);
////	for (int j = 0; j < numOfFeats; j++) {
////		printf("%d. (", j);
////		for (int i = 0; i < dims; i++) {
////			printf((i == dims - 1 ? "%f" : "%f, "),
////					spPointGetAxisCoor(pp[j], i));		}
////		printf(")\n");
////	}
//
//SPKDArray arr = spKDArrayCreate(config, pp, numOfFeats, &log_msg,
//		&conf_msg);
//SPKDTreeNode tree = spKDTreeCreate(arr, config, &conf_msg, &log_msg);
//
//spKDTreeDestroy(tree);
//SPKDArrayDestroy(arr);
//for (int k = 0; k < numOfFeats; k++) {
//	spPointDestroy(pp[k]);
//}
//free(pp);
//delete pc;
/***************************/

/** First check **/
//	printAttributes(config);
//	double data1[2] = { 1, 2 }, data2[2] = { 123, 70 }, data3[2] = { 2, 7 },
//			data4[2] = { 9, 11 }, data5[2] = { 3, 4 };
//	SPPoint a = spPointCreate(data1, 2, 1);
//	SPPoint b = spPointCreate(data2, 2, 2);
//	SPPoint c = spPointCreate(data3, 2, 3);
//	SPPoint d = spPointCreate(data4, 2, 4);
//	SPPoint e = spPointCreate(data5, 2, 5);
//
//	SPPoint* arr = (SPPoint*) malloc(5 * sizeof(SPPoint));
//	arr[0] = a;
//	arr[1] = b;
//	arr[2] = c;
//	arr[3] = d;
//	arr[4] = e;
//	SPKDArray kd = spKDArrayCreate(config, arr, 5, &log_msg, &conf_msg);
//	printKDArrayMatrix(kd);
//	printf("=========\n");
//	SPKDTreeNode kdtree = spKDTreeCreate(kd, config, &conf_msg, &log_msg);
//	SPKDArray kd = spKDArrayCreate(config, arr, 2, &log_msg, &conf_msg), kd1 = NULL, kd2 =NULL;
//	printKDArrayMatrix(kd);
//	spKDArraySplit(kd, 1, &kd1, &kd2, &log_msg, &conf_msg);
//	spKDTreeDestroy(kdtree);
//	SPKDArrayDestroy(kd);
//	for (int i = 0; i < 5; i++) {
//		spPointDestroy(arr[i]);
//	}
//	free(arr);
