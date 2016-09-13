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
	char q_path[1024], path[1024];
        char msg[1024] = {'\0'};
	int q_numOfFeats = 0, rc, numOfSimilarImages = 0, numOfFeats = -1;
	int *similar_images = NULL;
	int index;
        declareLogMsg();
        declareConfMsg();
	fflush(stdout);

	/***create config file***/
	if(argParse(argc, argv, &config, conf_msg))
		return ERROR;

	/*** Logger and ImageProc ***/
	if (initializeSPLogger(config, log_msg) || !(pc = new ImageProc(config))) {
		clearAll()
		return ERROR;
	}

	/*********   Extraction Mode    ************/
	if (spConfigIsExtractionMode(config, conf_msg)) {
                printInfo("Starting Extraction Mode");
		for (index = 0; index < spConfigGetNumOfImages(config, conf_msg); index++) {
                        sprintf(msg, "Extracting image in index %d", index);
                        printInfo(msg);
			path[1024] = {'\0'};
			spConfigGetImagePath(path, config, index); //No need to read conf_msg, controlling all inputs myself
			checkifImgExists()

                        printInfo("Extracting features from image");
			pointArray = pc->getImageFeatures(path, 0, &numOfFeats);
			if(!pointArray){
				clearAll()
                                if(pc)
                                    delete pc;
				return ERROR;
			}
                        printInfo("Exporting image features into a file");
			rc = exportImageToFile(pointArray, numOfFeats, index, config);
			spPointArrayDestroy(pointArray, numOfFeats);
                        if (rc) {
				/* Writing to File Error */
				clearAll()
                                if(pc)
                                    delete pc;
				return ERROR;
			}
		}
	}
        else {
                printInfo("Skipping Extraction Mode");
        }
	/*******************************************/

        printInfo("Creating Global Resources");
	rc = Setup(config, &kdtree);
	if (rc) {
                printFinishProgram(rc);
		clearAll()
		if (pc)
			delete pc;
		return rc;
	}

	/**** execute queries ****/
	q_path[1024] = {'\0'};
	numOfSimilarImages = spConfigGetNumOfSimilarImages(config, conf_msg);
        if (*conf_msg != SP_CONFIG_SUCCESS) {
            loggerWithArgs(CONFIG_MSG_ERR,conf_msg);
            printFinishProgram(1);
            clearAll()
            if (pc)
                delete pc;
            return ERROR;
        }
        printInfo("Start getting queries from user");
	while (1) {

		//get image path from user
		printf(PLS_ENTER);
		fflush(stdout);
		fgets(q_path, 1024, stdin);
		q_path[strlen(q_path) - 1] = '\0'; //q_path will always include at least '/n'

		//check validity of output
		if (!strcmp(q_path,CLS_QRY)){
                        printInfo("Got Exiting Signal ('<>')");
                        rc = 0;
			break;
                }
		if (!isValidFile(q_path)) {
			printf(INV_PATH);
			continue;
		}
                sprintf(msg, "Got query image path: %s", q_path);
                printInfo(msg);

		//getting query image features
                printInfo("Getting features for query image");
		q_features = pc->getImageFeatures(q_path, 0, &q_numOfFeats);
		if (!q_features || !(*q_features)) {
                        printError("Failed in getting features for query image");
                        rc = 1;
                        break;
		}
                sprintf(msg, "extracted %d features from image %s", q_numOfFeats, q_path);
                printInfo(msg);

		//find closest images to query image
                printInfo("Searching for the closest images in database for the query image");
		similar_images = getClosestImages(kdtree, config, q_features, q_numOfFeats);
		if (!similar_images) {
                        printError("Failed in searching for closest images");
                        rc = 1;
			break;
		}

		//show closest images on screen
                printInfo("Showing results:");
		bool gui = spConfigMinimalGui(config, conf_msg);
                char* tmp_path;
		if (!gui)
			printf(BST_CND, q_path);

		for (int img = 0; img < numOfSimilarImages; img++) {
			tmp_path = (char*) calloc(1024, sizeof(char));
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
