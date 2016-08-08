#include "SPExtractor.h"
int exportImageToFile(SPPoint* pa, int size, int image_index, SPConfig config) { 
    int rc=0;
    char* filepath = sprintf("%s%s%d.feats",config->spImagesDirectory,
                                            config->spImagesPrefix,
                                            image_index);
    if(!filepath) {
        //TODO logger message
        return 1;
    }
    FILE *output = fopen(filepath, "w");
    if(!output) {
        //TODO logger message
        return 1;
    }

    /*writes image index and num of points at the beginning of file*/
    rc = fprintf(output, "%d\n%d\n", image_index,
                                     size);
    if (rc<1):
        //TODO logger message
        fclose(output);                                          
        return 1; 
    }

    /*writes double array inside points*/
    int i,j;
    SPPoint curr;
    for(i=0;i<size;i++) {
        curr = pa[i];
        rc = fprintf(output,"%d %d ",curr->index,
                                  curr->dim); 
        if (rc<1){
            //TODO logger message
            fclose(output);                                          
            return 1;
        } 
        for(j=0;j<curr->dim;j++) {
            rc = fprintf(output,"%d%s",(curr->data)[j],
                                        j+1==dim ? "\n" : " ");
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
