#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "TreeErrors.h"
#include "Tree.h"
#include "DataBuffer.h"
#include "TreeLog.h"

static TreeSegment* find_segment_recursive(TreeSegment* segment, const char* data);

static treeErrorCode tree_verify_recurse(TreeSegment* segment);

treeErrorCode tree_verify(TreeData* tree)
{
    assert(tree);



    return NO_TREE_ERRORS;
}
//TODO write verify
static treeErrorCode tree_verify_recurse(TreeSegment* segment)
{
    
}

treeErrorCode tree_ctor(TreeData* tree)
{
    assert(tree);

    treeErrorCode error = NO_TREE_ERRORS;
    tree->root = new_segment(nullptr, &error);

    return error;
}

treeErrorCode tree_dtor(TreeData* tree)
{
    assert(tree);
    treeErrorCode error;

    error = del_segment(tree->root);

    return error;
}

TreeSegment* new_segment(TreeSegment** parent_segment, treeErrorCode* error)
{
    TreeSegment* segment = (TreeSegment*) calloc(1, sizeof(TreeSegment));
    segment->data = (char*) calloc(TREE_SEGMENT_DATA_LEN, sizeof(char));
    if (!segment || !segment->data)
    {
        if (error) *error = ALLOC_MEMORY_ERROR; 
    }

    if (parent_segment) *parent_segment = segment;

    segment->data_len   = TREE_SEGMENT_DATA_LEN;
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
    free(segment->data);
    free(segment);

    return error;
}

treeErrorCode tree_dump(TreeData* tree)
{
    assert(tree);

    outputBuffer buffer = {};

    buffer_ctor(&buffer, 10000);

    FILE* file = NULL;
    if (create_output_file(&file, "tree_test.dot", TEXT))
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

    write_buffer_to_file(file, &buffer);

    buffer_dtor(&buffer);

    return NO_TREE_ERRORS;
}

TreeSegment* find_segment(TreeData* tree, const char* data)
{
    assert(tree);
    return find_segment_recursive(tree->root, data);
}

static TreeSegment* find_segment_recursive(TreeSegment* segment, const char* data)
{
    assert(segment);
    TreeSegment* ptr = NULL;
    if (!strcmp(segment->data, data))
    {
        ptr = segment;
    }
    else if (segment->left)
    {
        ptr = find_segment_recursive(segment->left, data);
    }
    else if (segment->right)
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