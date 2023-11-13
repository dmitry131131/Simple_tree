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

        CHECK_CODE(ALLOC_MEMORY_ERROR,          "Error in calloc, alloced memory for buffer!\n");


    default:
        fprintf(stream, "Unknown error!\n");
        break;
    }
    #undef CHECK_CODE
}