#include "SPImageProc.h"
#include <cstring>
#include <cstdlib>

extern "C" {
#include "KDTree.h"
#include "KDArray.h"
#include "SPExtractor.h"
#include "SPKNNSearch.h"
#include <unistd.h>
#include "SPMainAux.h"
}

using namespace sp;

int main(int argc, char* argv[]) {

	/** Initialization **/
	SPConfig config = NULL;
	SP_CONFIG_MSG conf_msg = SP_CONFIG_SUCCESS;
	SP_LOGGER_MSG log_msg = SP_LOGGER_SUCCESS;
	ImageProc* pc = NULL;
	SPKDTreeNode kdtree = NULL;
	SPPoint* pointArray = NULL, *q_features = NULL;
	char q_path[1024], path[1024];
	int q_numOfFeats = 0, rc, numOfSimilarImages = 0, numOfFeats = -1;
	int *similar_images = NULL;
	int index, numOfImages = 0;
	fflush(stdout);

	/***create config file***/
	if(argParse(argc, argv, &config, &conf_msg,&numOfImages))
		return ERROR;

	/*** Logger and ImageProc ***/
	if (initializeSPLogger(config, &log_msg)) {
		clearAll()
		return ERROR;
	}

	pc = new ImageProc(config);

	/*********   Extraction Mode    ************/
	if (spConfigIsExtractionMode(config, &conf_msg)) {
		for (index = 0; index < numOfImages; index++) {
			path[1024] = {'\0'};
			spConfigGetImagePath(path, config, index); //No need to read conf_msg, controlling all inputs myself
			pointArray = pc->getImageFeatures(path, 0, &numOfFeats);

			/** Features Extraction **/
			if (exportImageToFile(pointArray, numOfFeats, index, config)) {

				/* Writing to File Error */
				spPointArrayDestroy(pointArray, numOfFeats);
				clearAll()
				return ERROR;
			}

		}
	}
	/*******************************************/

	rc = Setup(config, &kdtree, &log_msg, &conf_msg);
	if (rc) {
		//TODO logger msg
		clearAll()
		if (pc)
			delete pc;
		return ERROR;
	}

	/**** execute queries ****/
	q_path[1024] = {'\0'};
	numOfSimilarImages = spConfigGetNumOfSimilarImages(config, &conf_msg);

	while (1) {

		//get image path from user
		printf(PLS_ENTER);
		fflush(stdout);
		fgets(q_path, 1024, stdin);
		q_path[strlen(q_path) - 1] = '\0'; //q_path will always include at lease '/n'

		//check validity of output
		if (!strlen(q_path))
			break;
		if (access(q_path, F_OK) == -1) {
			printf(INV_PATH);
			continue;
		}

		//getting query image features
		q_features = pc->getImageFeatures(q_path, 0, &q_numOfFeats);
		if (!q_features || !(*q_features)) {
			//logger prints inside
			return 1;
		}

		//find closest images to query image
		similar_images = getClosestImages(kdtree, config, q_features,
				q_numOfFeats, &log_msg, &conf_msg);
		if (!similar_images) {
			//logger prints inside
			return 1;
		}

		//show closest images on screen
		bool gui = spConfigMinimalGui(config, &conf_msg);
		if (!gui)
			printf(BST_CND, q_path);
		for (int img = 0; img < numOfSimilarImages; img++) {
			char tmp_path[1024] = { '\0' };
			spConfigGetImagePath(tmp_path, config, similar_images[img]);
			if (gui)
				pc->showImage(tmp_path);
			else
				printf("%s\n", tmp_path);
		}

		//re-initializing query-related resources
		cleanTempResources(&q_features, q_numOfFeats, q_path);
	}

	printf(EXIT);
	cleanTempResources(&q_features, q_numOfFeats, q_path);
	clearAll()
	if (pc)
		delete pc;
	return OK;
}
