
#include "SPImageProc.h"

// this is comment

//almog's comment


int main(int argc, char* argv[]) {

	SPConfig project; SP_CONFIG_MSG msg;


	switch (argc) {

	case 1:
		project = spConfigCreate("spcbir.config", &msg);
		if (!project)
			return ERROR;
		break;

	case 3:

		if (strcmp(argv[1], "-c")) {
			printf(INVALID_COMLINE);
			return ERROR;
		}
		project = spConfigCreate(argv[2], &msg);
		if (!project)
			return ERROR;

		break;

	default:
		printf(INVALID_COMLINE);
		break;
	}

	return OK;
}

