#ifndef KDTREE_H_
#define KDTREE_H_

#include "KDArray.h"

typedef struct kd_tree_node_t* SPKDTreeNode;

SPKDTreeNode spKDTreeCreate(SPKDArray kdarray,SPConfig config,SP_CONFIG_MSG* conf_msg, SP_LOGGER_MSG* log_msg);

void spKDTreeDestroy(SPKDTreeNode kdtree);

#endif /* KDTREE_H_ */
