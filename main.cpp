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
	ImageProc* pc = NULL;
	SPKDTreeNode kdtree = NULL;
	SPPoint* pointArray = NULL, *q_features = NULL;
	char q_path[MAX_LENGTH], path[MAX_LENGTH];
	char msg[MAX_LENGTH] = { '\0' };
	int q_numOfFeats = 0, rc, numOfSimilarImages = 0, numOfFeats = -1;
	int *similar_images = NULL;
	int index;
	declareLogMsg();
	declareConfMsg();
	fflush(stdout);

	/***create config file***/
	if (argParse(argc, argv, &config, conf_msg))
		return ERROR;

	/*** Logger and ImageProc ***/
	if (initializeSPLogger(config, log_msg) || !(pc = new ImageProc(config))) {
		clearAll()
		return ERROR;
	}

	/*********   Extraction Mode    ************/
	if (spConfigIsExtractionMode(config, conf_msg)) {
		printInfo(EXTRCT_MODE);
		for (index = 0; index < spConfigGetNumOfImages(config, conf_msg);
				index++) {

			infoWithArgs(EXTRCT_IMG_NUM, index);

			path[MAX_LENGTH] = {'\0'};
			spConfigGetImagePath(path, config, index); //No need to read conf_msg, controlling all inputs myself
			checkifImgExists()

			/* Getting features of each photo */
			printInfo(EXTRCT_FROM_IMG);
			pointArray = pc->getImageFeatures(path, 0, &numOfFeats);
			if (!pointArray) {
				clearAll()
				if (pc)
					delete pc;
				return ERROR;
			}

			printInfo(EXPORT_TO_FILE);

			/* exporting to .feats file */
			rc = exportImageToFile(pointArray, numOfFeats, index, config);
			spPointArrayDestroy(pointArray, numOfFeats);
			if (rc) {
				/* Writing to File Error */
				clearAll()
				if (pc)
					delete pc;
				return ERROR;
			}
		}
	} else {
		printInfo(EXTRCT_SKIP);
	}
	/*******************************************/

	printInfo(GLOBAL_RES);
	rc = Setup(config, &kdtree);
	if (rc) {
		printFinishProgram(rc);
		clearAll()
		if (pc)
			delete pc;
		return rc;
	}

	/**** execute queries ****/
	q_path[MAX_LENGTH] = {'\0'};
	numOfSimilarImages = spConfigGetNumOfSimilarImages(config, conf_msg);
	if (*conf_msg != SP_CONFIG_SUCCESS) {
		loggerWithArgs(CONFIG_MSG_ERR, *conf_msg);
		printFinishProgram(ERR);
		clearAll()
		if (pc)
			delete pc;
		return ERROR;
	}
	printInfo(USR_QRY);
	while (1) {

		//get image path from user
		printf(PLS_ENTER);
		fflush(stdout);
		fgets(q_path, MAX_LENGTH, stdin);
		q_path[strlen(q_path) - 1] = '\0'; //q_path will always include at least '/n'

		//check validity of input
		if (!strcmp(q_path, CLS_QRY)) {
			printInfo(EXIT_SNGL);
			rc = OK;
			break;
		}
		//check if the file exists
		if (!isValidFile(q_path)) {
			printf(INV_PATH);
			continue;
		}
		sprintf(msg, QRY_PATH_GOT, q_path);
		printInfo(msg);

		//getting query image features
		printInfo(QRY_GET);
		q_features = pc->getImageFeatures(q_path, 0, &q_numOfFeats);
		if (!q_features || !(*q_features)) {
			printError(QRY_FEAT_ERR);
			rc = ERR;
			break;
		}
		infoWithArgs(IMG_EXTRCT, q_numOfFeats,q_path);

		//find closest images to query image
		printInfo(CLS_SRCH);
		similar_images = getClosestImages(kdtree, config, q_features,
				q_numOfFeats);
		if (!similar_images) {
			printError(SRCH_FAIL);
			rc = ERR;
			break;
		}

		//show closest images on screen
		printInfo(RSLTS);
		bool gui = spConfigMinimalGui(config, conf_msg);
		char* tmp_path;
		if (!gui)
			printf(BST_CND, q_path);

		for (int img = 0; img < numOfSimilarImages; img++) {
			tmp_path = (char*) calloc(MAX_LENGTH, sizeof(char));
			spConfigGetImagePath(tmp_path, config, similar_images[img]);

			if (gui)
				pc->showImage(tmp_path);
			else
				printf(STR_LINE, tmp_path);

			free(tmp_path);
			tmp_path = NULL;
		}
		//re-initializing query-related resources
		cleanTempResources(&q_features, q_numOfFeats, q_path, &similar_images);
	}

	printf(EXIT);

	/* Cleaning resources */
	cleanTempResources(&q_features, q_numOfFeats, q_path, &similar_images);
	printFinishProgram(rc);
	clearAll()
	if (pc)
		delete pc;

	return OK;
}
