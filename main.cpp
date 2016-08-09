#include "SPImageProc.h"
#include <cstring>
#include <cstdlib>

extern "C" {
#include "KDTree.h"
#include "KDArray.h"
#include "SPExtractor.h"
#include "SPBPriorityQueue.h"
#include <unistd.h>

}

#define FIRST_MSG "SPConfig file imported.\nLogger opened."
#define LOGGERnMSG "SPLogger Error. Message Type: %d"
#define FINISH_PRG "Program Finished Successfully"
using namespace sp;

SPPoint* findKNearestNeighbors(SPKDTreeNode kdtree, SPBPQueue bpq,
		SPPoint point) {
	return NULL;
}

void finishProgram(SPConfig config, SPBPQueue bpq, ImageProc* pc,
		SPKDTreeNode kdtree, SPPoint* all_points, int* img_near_cnt,
		int* similar_images, int all_points_size) {
	spLoggerPrintInfo("#Removing SPLogger and SPConfig");
	spLoggerDestroy();
	if (config)
		spConfigDestroy(config);
	if (bpq)
		spBPQueueDestroy(bpq);
	if (pc)
		delete pc;
	if (kdtree)
		spKDTreeDestroy(kdtree);
	if (all_points)
		spPointArrayDestroy(all_points, all_points_size);
	free(img_near_cnt);
	free(similar_images);
}

int main(int argc, char* argv[]) {

	/*variables declaration*/
	SPConfig config = NULL;
	SPBPQueue bpq = NULL;

	SPPoint* all_points = NULL, *q_features = NULL;
	int all_points_size = 0, knn_size = 0;
	SPKDArray kdarray = NULL;
	SPKDTreeNode kdtree = NULL;
	int numOfImages = 0, numOfSimilarImages = 0;
	int *img_near_cnt = NULL, *similar_images = NULL;
	SP_CONFIG_MSG conf_msg = SP_CONFIG_SUCCESS;
	SP_LOGGER_MSG log_msg = SP_LOGGER_SUCCESS;
	fflush(stdout);
	switch (argc) {

	/***create config file***/
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
	printAttributes(config);
	ImageProc* pc = new ImageProc(config);
	/***initiallize logger***/
	if ((log_msg = spLoggerCreate(NULL, SP_LOGGER_WARNING_ERROR_LEVEL))
			!= SP_LOGGER_SUCCESS) {
		finishProgram(config, bpq, pc, kdtree, all_points, img_near_cnt,
				similar_images, 0);
		return ERROR;
	}
	log_msg = spLoggerPrintDebug(FIRST_MSG, __FILE__, __func__, __LINE__);
	if (log_msg != SP_LOGGER_SUCCESS) {
		printf(LOGGERnMSG, log_msg);
		finishProgram(config, bpq, pc, kdtree, all_points, img_near_cnt,
				similar_images, 0);
		return ERROR;
	}

	/***initialize additional resources using config parameters***/


	bpq = spBPQueueCreate(spConfigGetKNN(config, &conf_msg));

	numOfImages = spConfigGetNumOfImages(config, &conf_msg);
	numOfSimilarImages = spConfigGetNumOfSimilarImages(config, &conf_msg);
	img_near_cnt = (int*) malloc(numOfImages * sizeof(int));
	similar_images = (int*) malloc(numOfSimilarImages * sizeof(int));
	memset(&img_near_cnt, 0, numOfImages);
	memset(&similar_images, 0, numOfSimilarImages);



	all_points = extractImagesFeatures(&all_points_size, config, &log_msg,
			&conf_msg);

	kdarray = spKDArrayCreate(config, all_points, all_points_size, &log_msg,
			&conf_msg);

	kdtree = spKDTreeCreate(kdarray, config, &conf_msg, &log_msg);

	if (!kdtree || !kdarray || !bpq || !pc) {
		//TODO logger msg
		finishProgram(config, bpq, pc, kdtree, all_points, img_near_cnt,
				similar_images, all_points_size);
		return ERROR;
	}

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
				finishProgram(config, bpq, pc, kdtree, all_points, img_near_cnt,
						similar_images, all_points_size);
				return ERROR;
			}
			/************************/
		}
	}

	/**** execute queries ****/
	char q_path[1024] = { '\0' };
	;
	int q_numOfFeats, i, j;
	SPPoint* knn = NULL;
	SPPoint curr_pnt = NULL;
	knn_size = spConfigGetKNN(config, &conf_msg);



	while (1) {

		//get image path from user
		fflush(stdout);
		printf("Please enter an image path:\n");
		fflush(stdout);
		fgets(q_path, 1024, stdin);
		fflush(stdout);
		q_path[strlen(q_path) - 1] = '\0';

		//check validity of output
		if (!strlen(q_path))
			break;
		if (access(q_path, F_OK) == -1) {
			printf("invalid path to image. Please try again\n");
			continue;
		}

		//getting query image features
		q_features = pc->getImageFeatures(q_path, 0, &q_numOfFeats);
		if (!q_features || !(*q_features)) {
			//TODO logger message
			printf("NULL POINTER EXCEPTION");
			exit(1);								//TODO CHANGE
		}

		//for each point in the query image, find k-nearest neighbors
		for (i = 0; i < q_numOfFeats; i++) {
			spBPQueueClear(bpq);
			knn = findKNearestNeighbors(kdtree, bpq, q_features[i]);
			if (!knn) {
				printf("NULL POINTER EXCEPTION");				//TODO CHANGE
				exit(1);
			}
			//count image indices related to neighbors just found
			for (j = 0; j < knn_size; j++) {
				curr_pnt = knn[j];
				img_near_cnt[spPointGetIndex(curr_pnt)]++;
			}
			spPointArrayDestroy(knn, knn_size);
			knn = NULL;
		}

		//return the k nearest images based on img_near_cnt array
		// assumes numOfSimilarImages << n
		for (i = 0; i < numOfSimilarImages; i++) {
			similar_images[i] = 0;
			for (j = 0; j < numOfImages; j++) {
				if (img_near_cnt[j] > img_near_cnt[similar_images[i]])
					similar_images[i] = j;
			}
			img_near_cnt[similar_images[i]] = 0;
		}

		//print result
		printf("the %d most similar images are:", numOfSimilarImages);
		for (i = 0; i < numOfSimilarImages; i++) {
			printf("%d%c", similar_images[i],
					i + 1 == numOfSimilarImages ? '\n' : ' ');
		}

		//re-initializing query-related resources
		spPointArrayDestroy(q_features, q_numOfFeats);
		memset(&q_path[0], 0, strlen(q_path));
		memset(&img_near_cnt[0], 0, numOfImages);
		memset(&similar_images[0], 0, numOfSimilarImages);
		q_features = NULL;
	}

	printf("Exiting...\n");
	//need to add free for query parameters        
	finishProgram(config, bpq, pc, kdtree, all_points, img_near_cnt,
			similar_images, all_points_size);
	return OK;

	//freeing temporary resources
//            spPointArrayDestroy(q_pp); //TODO uncomment after pull
}

//printf("Exiting...\n");
////TODO add finishing query resources
//finishProgram(config, bpq, pc, kdtree, all_points, img_near_cnt, similar_images,all_points_size);
//return OK;
//
////GARBAGE FROM HERE AND ON//
//int numOfFeats = -1;
//char path[1024] = { '\0' };
//conf_msg = spConfigGetImagePath(path, config, 0);
//printf("******path = %s\n", path);
//SPPoint* pp = pc->getImageFeatures(path, 0, &numOfFeats);
//if (!pp || !*pp) {
//	printf("NULL POINTER EXCEPTION");
//	exit(1);
//}
//int rc = 0;
//numOfImages = spConfigGetNumOfImages(config, &conf_msg);
//if(numOfImages < 0) {
//	printf("ERROR IN NUM OF IMAGES\n");
//	exit(1);
//}
//for(i=0;i<numOfImages;i++) {
//	rc = exportImageToFile(pp, numOfFeats, i, config);
//	if(rc) {
//		printf("ERROR - in exportImagesToFile");
//		exit(1);
//	}
//}
//
///***************************/
//log_msg = spLoggerPrintInfo(FINISH_PRG);
//if (log_msg != SP_LOGGER_SUCCESS) {
//	printf(LOGGERnMSG, log_msg);
//}
//printf(FINISH_PRG);
//finishProgram(config, bpq, pc, kdtree, all_points, img_near_cnt, similar_image);
//return OK;
//}

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
