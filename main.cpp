#include "SPImageProc.h"
#include <cstring>
#include <cstdlib>

extern "C" {
#include "KDTree.h"
#include "KDArray.h"
#include "SPExtractor.h"
#include "SPKNNSearch.h"
#include <unistd.h>

}

#define FIRST_MSG "SPConfig file imported.\nLogger opened."
#define LOGGERnMSG "SPLogger Error. Message Type: %d"
#define FINISH_PRG "Program Finished Successfully"
using namespace sp;

void cleanGlobalResources(SPConfig config,
                          ImageProc* pc,
                          SPKDTreeNode kdtree,
                          int* img_near_cnt,
                          int* similar_images)
{
	spLoggerPrintInfo("Removing SPLogger and SPConfig");
	spLoggerDestroy();
	if (config)
		spConfigDestroy(config);
	if (pc)
		delete pc;
	if (kdtree)
		spKDTreeDestroy(kdtree);
	free(img_near_cnt);
	free(similar_images);
}

void cleanTempResources(SPPoint** q_features,
                        int q_numOfFeats,
                        char* q_path)
{
        spPointArrayDestroy(*q_features, q_numOfFeats);
	memset(&q_path[0], '\0', strlen(q_path));
	*q_features = NULL;
}

int exportAllImagesToFiles(ImageProc* pc, SPConfig config, SP_LOGGER_MSG* log_msg){
    if(*log_msg != SP_LOGGER_SUCCESS)
        return 1;
    int index, rc=0;
    SP_CONFIG_MSG conf_msg = SP_CONFIG_SUCCESS;
    int numOfImages = spConfigGetNumOfImages(config, &conf_msg);
    if(conf_msg != SP_CONFIG_SUCCESS) {
        //TODO update logger message
        return 1;
    }

    for (index = 0; index < numOfImages; index++) {
        char path[1024] = { '\0' };
	int numOfFeats = -1;
	spConfigGetImagePath(path, config, index); 
	SPPoint* pointArray = pc->getImageFeatures(path, 0, &numOfFeats);
	rc = exportImageToFile(pointArray, numOfFeats, index, config);
        if (rc){
            //TODO update logger message
            return 1;
        }
	spPointArrayDestroy(pointArray, numOfFeats);
	pointArray = NULL;
    }
    return 0;
}

int Setup(SPConfig config,
      ImageProc* pc,
      SPKDTreeNode* kdtree,
      int* img_near_cnt,
      int* similar_images,
      SP_LOGGER_MSG* log_msg,
      SP_CONFIG_MSG* conf_msg)
{
        int all_points_size=-1;
        int rc = 0;
	int numOfImages = spConfigGetNumOfImages(config, conf_msg);
	int numOfSimilarImages = spConfigGetNumOfSimilarImages(config, conf_msg);
	img_near_cnt = (int*) calloc(numOfImages, sizeof(int));
	similar_images = (int*) calloc(numOfSimilarImages, sizeof(int));
        if(!img_near_cnt || !similar_images){
            //TODO update logger message
            return 1;
        }

	if (spConfigIsExtractionMode(config, conf_msg)) {
            rc = exportAllImagesToFiles(pc, config, log_msg);
            if(rc){
                //TODO update logger message
                return 1;
            }
        }

	SPPoint* all_points = extractImagesFeatures(&all_points_size, config, log_msg,
			conf_msg);
	SPKDArray kdarray = spKDArrayCreate(config, all_points, all_points_size, log_msg,
			conf_msg);
	*kdtree = spKDTreeCreate(kdarray, config, conf_msg, log_msg);
        if(all_points)
            spPointArrayDestroy(all_points, all_points_size);
        if(kdarray)
            SPKDArrayDestroy(kdarray);
	if (!(*kdtree)) {
		//TODO update logger msg
                return 1;
	}
        return 0;
}



int main(int argc, char* argv[]) {

	/*variables declaration*/
	SPConfig config = NULL;
	ImageProc* pc = NULL;
	SPPoint* q_features = NULL;
	int knn_size = 0;
	SPKDTreeNode kdtree = NULL;
	int numOfImages = 0, numOfSimilarImages = 0;
	int rc;
	int *img_near_cnt = NULL, *similar_images = NULL;
	SP_CONFIG_MSG conf_msg = SP_CONFIG_SUCCESS;
	SP_LOGGER_MSG log_msg = SP_LOGGER_SUCCESS;

	int q_numOfFeats, i, j;
	int* knn = NULL;

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
	printAttributes(config); //TODO delete
	pc = new ImageProc(config);

	/***initiallize logger***/
	if ((log_msg = spLoggerCreate(NULL, SP_LOGGER_WARNING_ERROR_LEVEL))
			!= SP_LOGGER_SUCCESS) {
		clearAll()
		return ERROR;
	}
	log_msg = spLoggerPrintDebug(FIRST_MSG, __FILE__, __func__, __LINE__);
	if (log_msg != SP_LOGGER_SUCCESS) {
		printf(LOGGERnMSG, log_msg);
		clearAll()
		return ERROR;
	}
	printAttributes(config); //TODO Delete
	pc = new ImageProc(config);
	/***initialize additional resources using config parameters***/
	rc = Setup(config, pc, &kdtree, img_near_cnt, similar_images, &log_msg,
			&conf_msg);
	if (rc) {
		//TODO logger msg
		cleanGlobalResources(config, pc, kdtree, img_near_cnt, similar_images);
		return ERROR;
	}

	/**** execute queries ****/
	char q_path[1024] = { '\0' };
//<<<<<<< Upstream, based on origin/master
//	numOfImages = spConfigGetNumOfImages(config, &conf_msg);
//	numOfSimilarImages = spConfigGetNumOfSimilarImages(config, &conf_msg);
//	int q_numOfFeats, i, j;
//	SPPoint* knn = NULL;
//	SPPoint curr_pnt = NULL;
//=======

	knn_size = spConfigGetKNN(config, &conf_msg);

	while (1) {

		//get image path from user
		printf("Please enter an image path:\n");
		fflush(stdout);
		fgets(q_path, 1024, stdin);
		q_path[strlen(q_path) - 1] = '\0'; //q_path will always include at lease '/n'

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
			printf("NULL POINTER EXCEPTION1");
			exit(1);								//TODO CHANGE
		}

		//for each point in the query image, find k-nearest neighbors
		for (i = 0; i < q_numOfFeats; i++) {
			knn = findKNearestNeighbors(kdtree, q_features[i], config);
			if (!knn) {
				printf("NULL POINTER EXCEPTION2");				//TODO CHANGE
				exit(1);
			}
			//count image indices related to neighbors just found
			for (j = 0; j < knn_size; j++) {
				img_near_cnt[knn[j]]++;
			}
			free(knn);
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

		bool gui = spConfigMinimalGui(config, &conf_msg);
		if (!gui)
			printf("Best candidates for - %s - are:\n", q_path);

		for (int img = 0; img < numOfSimilarImages; img++) {
			char tmp_path[1024] = { '\0' };
			spConfigGetImagePath(tmp_path, config, similar_images[img]);
			if (gui)
				pc->showImage(tmp_path);
			else
				printf("%s\n", tmp_path);
		}

		//re-initializing query-related resources
        cleanTempResources(&q_features,q_numOfFeats,q_path);




	}

	printf("Exiting...\n");
    cleanTempResources(&q_features,q_numOfFeats,q_path);
	clearAll()
	return OK;

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
