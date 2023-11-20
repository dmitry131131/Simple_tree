#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "TreeErrors.h"
#include "Tree.h"
#include "DataBuffer.h"
#include "TreeLog.h"

static TreeSegment* find_segment_recursive(TreeSegment* segment, const void* data);

static treeErrorCode tree_verify_recurse(TreeSegment* segment);

treeErrorCode tree_verify(TreeData* tree)
{
    assert(tree);



    return NO_TREE_ERRORS;
}
//TODO write verify
static treeErrorCode tree_verify_recurse(TreeSegment* segment)
{
    

    return NO_TREE_ERRORS;
}

treeErrorCode tree_ctor(TreeData* tree)
{
    assert(tree);

    treeErrorCode error = NO_TREE_ERRORS;
    tree->root = new_segment(NO_TYPE_SEGMENT_DATA, 0, nullptr, &error);

    return error;
}

treeErrorCode tree_dtor(TreeData* tree)
{
    assert(tree);
    treeErrorCode error;

    error = del_segment(tree->root);

    return error;
}

TreeSegment* new_segment(SegmemtType type, size_t dataLen, TreeSegment** parent_segment, treeErrorCode* error)
{
    TreeSegment* segment = (TreeSegment*) calloc(1, sizeof(TreeSegment));

    if (!segment)
    {
        if (error) *error = ALLOC_MEMORY_ERROR; 
    }

    if (type == TEXT_SEGMENT_DATA)
    {
        segment->data.stringPtr = (char*) calloc(dataLen, sizeof(char));

        if (!segment->data.stringPtr)
        {
            if (error) *error = ALLOC_MEMORY_ERROR; 
        }
    }

    segment->data_len   = dataLen;
    segment->type       = type;
    segment->left       = NULL;
    segment->right      = NULL;

    if (parent_segment)
    {
        segment->parent = *parent_segment;
    }
    else 
    {
        segment->parent = nullptr;
    }

    if (parent_segment) *parent_segment = segment;

    return segment;
}

treeErrorCode del_segment(TreeSegment* segment)
{
    assert(segment);
    treeErrorCode error = NO_TREE_ERRORS;
    if (segment->left) 
    {
        if ((error = del_segment(segment->left)))
        {
            return error;
        }
    }
    if (segment->right)
    {
        if ((error = del_segment(segment->right)))
        {
            return error;
        }
    }
    if (segment->type == TEXT_SEGMENT_DATA)
    {
        free(segment->data.stringPtr);
    }
    
    free(segment);

    return error;
}

treeErrorCode tree_dump(TreeData* tree)
{
    assert(tree);

    outputBuffer buffer = {};
    buffer.AUTO_FLUSH = 1;

    if (create_output_file(&(buffer.filePointer), "tree_test.dot", TEXT))
    {
        printf("Create error!\n");
    }

    if (write_dot_header(&buffer))
    {
        printf("header error!\n");
    }
    
    if (write_dot_body(&buffer, tree))
    {
        printf("body error!\n");
    }

    write_dot_footer(&buffer, tree);

    printf("%lu\n", buffer.bufferPointer);

    write_buffer_to_file(&buffer, buffer.filePointer);

    return NO_TREE_ERRORS;
}

TreeSegment* find_segment(TreeData* tree, const void* data)
{
    assert(tree);
    return find_segment_recursive(tree->root, data);
}

static TreeSegment* find_segment_recursive(TreeSegment* segment, const void* data)
{
    assert(segment);
    TreeSegment* ptr = NULL;

    switch (segment->type)
    {
        case TEXT_SEGMENT_DATA:
            if (!strncmp(segment->data.stringPtr, (const char*) data, segment->data_len))
            {
                return segment;
            }
            break;
        case DOUBLE_SEGMENT_DATA:
            if (segment->data.D_number == *((const double*) data))
            {
                return segment;
            }
            break;
        case INTEGER_SEGMENT_DATA:
            if (segment->data.I_number == *((const int*) data))
            {
                return segment;
            }
            break;

        case NO_TYPE_SEGMENT_DATA:
        default:
            break;
    }

    if (segment->left)
    {
        ptr = find_segment_recursive(segment->left, data);
    }
    if (segment->right)
    {
        ptr = find_segment_recursive(segment->right, data);
    }

    return ptr;
}

//TODO write function
treeErrorCode wrire_tree_to_file(const char* filename, TreeData* tree)
{
    assert(tree);
    assert(filename);
    
    return NO_TREE_ERRORS;
}

//TODO write read function