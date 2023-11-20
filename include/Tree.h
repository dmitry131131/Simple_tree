/**
 * @file
 * @brief Main tree functions
*/
#ifndef TREE_H
#define TREE_H

#include "config.h"
#include "TreeErrors.h"

enum SegmemtType {
    NO_TYPE_SEGMENT_DATA,
    TEXT_SEGMENT_DATA,
    INTEGER_SEGMENT_DATA,
    DOUBLE_SEGMENT_DATA,
    OP_CODE_SEGMENT_DATA
};

union SegmentData {
    char*   stringPtr;
    double  D_number;
    int     I_number;
};

struct TreeSegment {
    SegmentData data;
    size_t data_len;
    SegmemtType type;

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

TreeSegment* new_segment(SegmemtType type, size_t dataLen, TreeSegment** parent_segment = nullptr, treeErrorCode* error = nullptr);

treeErrorCode del_segment(TreeSegment* segment);

treeErrorCode tree_dump(TreeData* tree);

TreeSegment* find_segment(TreeData* tree, const void* data);

#endif