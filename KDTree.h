#ifndef KDTREE_H_
#define KDTREE_H_

#include "KDArray.h"

typedef struct kd_tree_node_t* KDTreeNode;

KDTreeNode spKDTreeCreate(SPKDArray kdarray,SPConfig config,SP_CONFIG_MSG* conf_msg, SP_LOGGER_MSG* log_msg);


#endif /* KDTREE_H_ */
