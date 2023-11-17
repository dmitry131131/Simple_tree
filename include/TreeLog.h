/**
 * @file 
 * @brief Log functions
*/
#ifndef TREE_LOG_H
#define TREE_LOG_H

treeErrorCode write_dot_header(outputBuffer* buffer);

treeErrorCode write_dot_body(outputBuffer* buffer, TreeData* tree);

treeErrorCode write_dot_footer(outputBuffer* buffer, TreeData* tree);

#endif