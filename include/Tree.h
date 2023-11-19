/**
 * @file
 * @brief Main tree functions
*/
#ifndef TREE_H
#define TREE_H

#include "config.h"
#include "TreeErrors.h"

struct TreeSegment {
    char* data;
    size_t data_len;

    TreeSegment* parent;
    TreeSegment* left;
    TreeSegment* right;
};

struct TreeData {
    TreeSegment* root;
    size_t size;
};

treeErrorCode tree_verify(TreeData* tree);

treeErrorCode tree_ctor(TreeData* tree);

treeErrorCode tree_dtor(TreeData* tree);

TreeSegment* new_segment(TreeSegment** parent_segment = nullptr, treeErrorCode* error = nullptr);

treeErrorCode del_segment(TreeSegment* segment);

treeErrorCode tree_dump(TreeData* tree);

TreeSegment* find_segment(TreeData* tree, const char* data);

#endif