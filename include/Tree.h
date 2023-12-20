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
    DOUBLE_SEGMENT_DATA,
    OP_CODE_SEGMENT_DATA,
    VAR_SEGMENT_DATA,

    IDENTIFIER,
    KEYWORD,
    FUNCTION_DEFINITION,
    PARAMETERS,
    VAR_DECLARATION,
    CALL
};

enum KeyWords {
    KEY_IF          = 11,
    KEY_WHILE       = 12,
    KEY_ASSIGMENT   = 13,
    KEY_SIN         = 21,
    KEY_COS         = 22,
    KEY_FLOOR       = 23,
    KEY_PLUS        = 24,
    KEY_MINUS       = 25,
    KEY_MUL         = 26,
    KEY_DIV         = 27,
    KEY_DIFF        = 28,
    KEY_EQUAL       = 31,
    KEY_LESS        = 32,
    KEY_MORE        = 33,
    KEY_LESS_EQUAL  = 34,
    KEY_MORE_EQUAL  = 35,
    KEY_NOT_EQUAL   = 36,
    KEY_AND         = 37,
    KEY_OR          = 38,
    KEY_NOT         = 39,
    KEY_NEXT        = 41,
    KEY_ENUM        = 42,
    KEY_NUMBER      = 51,
    KEY_DEF         = 52,
    KEY_OBR         = 1,
    KEY_CBR         = 2,
    KEY_O_CURBR     = 3,
    KEY_C_CURBR     = 4
};

enum OpCodes {
    NONE,
    PLUS,
    MINUS,
    MUL,
    DIV,
    SIN,
    COS,
    TAN,
    OBR,
    CBR,
    POW,
    LN
};

union SegmentData {
    char*       stringPtr;
    double      D_number;
    OpCodes     Op_code;
    int         Var;
    KeyWords    K_word;
};

struct TreeSegment {
    SegmentData  data;
    size_t       data_len;
    SegmemtType  type;
    size_t       weight;

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

TreeSegment* new_segment(SegmemtType type, size_t dataLen, TreeSegment* parent_segment = nullptr, treeErrorCode* error = nullptr);

treeErrorCode del_segment(TreeSegment* segment);

treeErrorCode tree_dump(TreeData* tree);

TreeSegment* find_segment(TreeData* tree, const void* data);

treeErrorCode wrire_tree_to_file(const char* filename, TreeData* tree);

treeErrorCode read_tree_from_file(TreeData* tree, const char* filename);

treeErrorCode copy_segment(TreeSegment* dest, const TreeSegment* src);

treeErrorCode copy_subtree(const TreeSegment* src, TreeSegment** dest, TreeSegment* parent = nullptr);

#endif