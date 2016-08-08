#include "SPExtractor.h"

int exportImageToFile(SPPoint* pa, int size, int image_index, SPConfig config) { 
    int rc=0;
    char filepath[1024] ={'\0'};
    SP_CONFIG_MSG conf_msg = spConfigGetImagePath(filepath, config, image_index);
    if (conf_msg != SP_CONFIG_SUCCESS)
        return 1;

    char* ptr=filepath + strlen(filepath);
    while(*ptr != '.'){
        *ptr = '\0';
        ptr--;
    }
    sprintf(ptr, ".feats");
    printf("%s\n", filepath); //TODO delete
     
    FILE *output = fopen(filepath, "w");
    if(!output) {
        //TODO logger message
        return 1;
    }

    /*writes image index and num of points at the beginning of file*/
    rc = fprintf(output, "%d\n%d\n", image_index,
                                     size);
    if (rc<1){
        //TODO logger message
        fclose(output);                                          
        return 1; 
    }

    /*writes double array inside points*/
    int i,j, dims;
    SPPoint curr;
    for(i=0;i<size;i++) {
        curr = pa[i];
        dims = spPointGetDimension(curr);
        for(j=0;j<dims;j++) {
            rc = fprintf(output,"%f%c",spPointGetAxisCoor(curr,j),
                                       j+1==dims ? '\n' : ' ');
            if (rc<1){
                //TODO logger message
                fclose(output);
                return 1;
            }
        }
    }
    fclose(output);
    return 0;
}
