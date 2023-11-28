#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <sys/stat.h>
#include <string.h>
#include <assert.h>

#include "TreeErrors.h"
#include "Tree.h"
#include "DataBuffer.h"
#include "TreeLog.h"

struct SegmentValue {
    SegmentData data;
    SegmemtType type;
    size_t size;
};

static TreeSegment* find_segment_recursive(TreeSegment* segment, const void* data);

static treeErrorCode tree_verify_recurse(TreeSegment* segment);

static treeErrorCode write_tree_to_buffer_recursive(outputBuffer* buffer, const TreeSegment* segment);

static SegmentValue get_segment_value(outputBuffer* buffer);

static TreeSegment* read_tree_from_file_recursive(outputBuffer* buffer, TreeSegment** par_segment, treeErrorCode* error);

treeErrorCode tree_verify(TreeData* tree)
{
    assert(tree);

    if (!tree->root)
    {
        return TREE_NO_ROOT;
    }

    return tree_verify_recurse(tree->root);
}

static treeErrorCode tree_verify_recurse(TreeSegment* segment)
{
    assert(segment);
    treeErrorCode error = NO_TREE_ERRORS;

    if (segment->left)
    {
        if ((segment->left)->parent != segment)
        {
            return TREE_LINK_ERROR;
        }

        if ((error = tree_verify_recurse(segment->left))) return error;
    }
    if (segment->right)
    {
        if ((segment->right)->parent != segment)
        {
            return TREE_LINK_ERROR;
        }

        if ((error = tree_verify_recurse(segment->right))) return error;
    }

    return error;
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
    treeErrorCode error = NO_TREE_ERRORS;

    if (!segment)
    {
        return error;
    }

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
    buffer.AUTO_FLUSH = true;

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

    write_buffer_to_file(&buffer);

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
        case OP_CODE_SEGMENT_DATA:
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

treeErrorCode wrire_tree_to_file(const char* filename, TreeData* tree)
{
    assert(tree);
    assert(filename);

    outputBuffer buffer = {};
    buffer.AUTO_FLUSH = true;

    create_output_file(&buffer.filePointer, filename, TEXT);

    write_tree_to_buffer_recursive(&buffer, tree->root);
    
    write_buffer_to_file(&buffer);
    
    return NO_TREE_ERRORS;
}

static treeErrorCode write_tree_to_buffer_recursive(outputBuffer* buffer, const TreeSegment* segment)
{
    assert(buffer);

    print_to_buffer(buffer, "(");

    switch(segment->type)
    {
        case TEXT_SEGMENT_DATA:
            print_to_buffer(buffer, "%s ", segment->data.stringPtr);
            break;
        case DOUBLE_SEGMENT_DATA:
            print_to_buffer(buffer, "%lf ", segment->data.D_number);
            break;
        case INTEGER_SEGMENT_DATA:
            print_to_buffer(buffer, "%d ", segment->data.I_number);
            break;
        case NO_TYPE_SEGMENT_DATA:
            print_to_buffer(buffer, "NONE ");
            break;
        case OP_CODE_SEGMENT_DATA:
            break;

        default:
            break;
    }

    if (segment->left)
    {
        write_tree_to_buffer_recursive(buffer, segment->left);
    }
    else
    {
        print_to_buffer(buffer, "nil ");
    }

    if (segment->right)
    {
        write_tree_to_buffer_recursive(buffer, segment->right);
    }
    else
    {
        print_to_buffer(buffer, "nil");
    }

    print_to_buffer(buffer, ") ");

    return NO_TREE_ERRORS;
}

treeErrorCode read_tree_from_file(TreeData* tree, const char* filename)
{
    assert(tree);
    assert(filename);

    FILE* file = fopen(filename, "r");
    treeErrorCode error = NO_TREE_ERRORS;

    if (!file)
    {
        return FILE_READ_ERROR;
    }

    struct stat buff = {};
    if (fstat(fileno(file), &buff))
    {
        return FILE_DESCRIPTOR_ERROR;
    }
    if (buff.st_size < 0) return FILE_SIZE_ERROR;

    outputBuffer treeBuffer;

    if (buffer_ctor(&treeBuffer, buff.st_size))
    {
        return BUFFER_CTOR_ERROR;
    }

    if (fread(treeBuffer.customBuffer, sizeof(char), treeBuffer.customSize, file) != treeBuffer.customSize)
    {
        return FILE_READ_ERROR;
    }

    tree->root = read_tree_from_file_recursive(&treeBuffer, &tree->root, &error);

    buffer_dtor(&treeBuffer);

    return error;
}

static TreeSegment* read_tree_from_file_recursive(outputBuffer* buffer, TreeSegment** par_segment, treeErrorCode* error)
{
    assert(buffer);
    assert(par_segment);

    TreeSegment* seg = NULL;
    SegmentValue val = {};
    char Nil_buffer[30] = {};

    if (((char) buffer->customBuffer[buffer->bufferPointer]) == '(')
    {
        (buffer->bufferPointer)++;

        val = get_segment_value(buffer);
        seg = new_segment(val.type, val.size, par_segment, error);

        if (val.type == TEXT_SEGMENT_DATA)
        {
            int count_of_symb = 0;
            sscanf(buffer->customBuffer + buffer->bufferPointer, "%s%n", seg->data.stringPtr, &count_of_symb);
            buffer->bufferPointer += (size_t) count_of_symb;

            (buffer->bufferPointer)++;
        }
        else
        {
            seg->data = val.data;
        }
    }
    else
    {
        if (error) *error = WRONG_TREE_SYNTAX;
        return NULL;
    }
    
    if (((char) buffer->customBuffer[buffer->bufferPointer]) == '(')
    {
        seg->left = read_tree_from_file_recursive(buffer, &seg->left, error);
    }
    else
    {
        int count_of_symb = 0;
        buffer->bufferPointer += (size_t) sscanf(buffer->customBuffer + buffer->bufferPointer, "%3s%n", Nil_buffer, &count_of_symb);
        buffer->bufferPointer += (size_t) count_of_symb;

        if (!strcmp("nil", Nil_buffer))
        {
            seg->left = NULL;
        }
        else
        {
            if (error) *error = WRONG_TREE_SYNTAX;
            return NULL;
        }
    }
    
    if (((char) buffer->customBuffer[buffer->bufferPointer]) == '(')
    {
        seg->right = read_tree_from_file_recursive(buffer, &seg->right, error);
    }
    else
    {
        int count_of_symb = 0;
        buffer->bufferPointer += (size_t) sscanf(buffer->customBuffer + buffer->bufferPointer, "%3s%n", Nil_buffer, &count_of_symb);
        buffer->bufferPointer += (size_t) count_of_symb;

        if (!strcmp("nil", Nil_buffer))
        {
            seg->right = NULL;
        }
        else
        {
            if (error) *error = WRONG_TREE_SYNTAX;
            return NULL;
        }
    }
    
    if (((char) buffer->customBuffer[buffer->bufferPointer - 1]) == ')')
    {
        (buffer->bufferPointer)++;
        return seg;
    }
    else
    {
        if (error) *error = WRONG_TREE_SYNTAX;
        return NULL;
    }
}

static SegmentValue get_segment_value(outputBuffer* buffer)
{
    assert(buffer);
    SegmentValue value;

    int count_of_symb = 0;
    if ((sscanf(buffer->customBuffer + buffer->bufferPointer, "%lf%n", &(value.data.D_number), &count_of_symb)))
    {
        value.type = DOUBLE_SEGMENT_DATA;
        value.size = sizeof(double);
    }
    else
    {
        value.data.stringPtr = NULL;
        value.type = TEXT_SEGMENT_DATA;
        value.size = TREE_TEXT_SEGMENT_DATA_LEN;
    }
    buffer->bufferPointer += (size_t) count_of_symb;

    return value;
}