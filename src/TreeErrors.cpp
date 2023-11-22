#include <stdio.h>

#include "Color_output.h"
#include "TreeErrors.h"

void print_tree_error(treeErrorCode error)
{
    print_tree_error_message(error, stderr);
}

void print_tree_error_message(treeErrorCode error, FILE* stream)
{
    color_fprintf(stream, COLOR_RED, STYLE_BOLD, "Data Buffer error: ");

    #define CHECK_CODE(code, message)               \
        case code:                                  \
            fprintf(stream, message);               \
            break;                                  \

    switch (error)
    {
        case NO_TREE_ERRORS:
            break;

        CHECK_CODE(ALLOC_MEMORY_ERROR,       "Error in calloc, alloced memory for buffer!\n");
        CHECK_CODE(TREE_LINK_ERROR,          "Verify error, tree nodes have wronk links!\n");
        CHECK_CODE(TREE_NO_ROOT,             "Tree hasn't got root!\n");
        CHECK_CODE(WRONG_TREE_SYNTAX,        "Wrong syntax in file!\n");
        CHECK_CODE(FILE_DESCRIPTOR_ERROR,    "Wrong file descriptor given!\n");
        CHECK_CODE(FILE_SIZE_ERROR,          "Wrong file size!\n");
        CHECK_CODE(BUFFER_CTOR_ERROR,        "Error in buffer constructor!\n");
        CHECK_CODE(FILE_READ_ERROR,          "File read error!\n");

    default:
        fprintf(stream, "Unknown error!\n");
        break;
    }
    #undef CHECK_CODE
}