#ifndef TREE_ERRORS_H
#define TREE_ERRORS_H

enum treeErrorCode {
    NO_TREE_ERRORS,
    ALLOC_MEMORY_ERROR,
    TREE_LINK_ERROR,
    TREE_NO_ROOT,
    WRONG_TREE_SYNTAX,
    FILE_DESCRIPTOR_ERROR,
    FILE_SIZE_ERROR,
    BUFFER_CTOR_ERROR,
    FILE_READ_ERROR
};

void print_tree_error(treeErrorCode error);

void print_tree_error_message(treeErrorCode error, FILE* stream);

#endif