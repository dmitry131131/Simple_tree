#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <sys/stat.h>
#include <string.h>
#include <assert.h>
#include <math.h>

#include "TreeErrors.h"
#include "Tree.h"
#include "DataBuffer.h"
#include "TreeLog.h"

struct SegmentValue {
    SegmentData data;
    SegmemtType type;
    size_t size;
};

static bool is_equal(const double first, const double second);

static TreeSegment* find_segment_recursive(TreeSegment* segment, const void* data);

static treeErrorCode tree_verify_recurse(TreeSegment* segment);

static treeErrorCode write_tree_to_buffer_recursive(outputBuffer* buffer, const TreeSegment* segment);

static SegmentValue get_segment_value(outputBuffer* buffer);

static TreeSegment* read_tree_from_file_recursive(outputBuffer* buffer, TreeSegment* par_segment, treeErrorCode* error);

static void read_string_from_buffer(outputBuffer* buffer, TreeSegment* segment, treeErrorCode* error);

static void read_op_code_from_buffer(outputBuffer* buffer, TreeSegment* segment, treeErrorCode* error);

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

TreeSegment* new_segment(SegmemtType type, size_t dataLen, TreeSegment* parent_segment, treeErrorCode* error)
{
    TreeSegment* segment = (TreeSegment*) calloc(1, sizeof(TreeSegment));

    if (!segment)
    {
        if (error) *error = ALLOC_MEMORY_ERROR; 
    }

    segment->data_len   = dataLen;
    segment->type       = type;
    segment->left       = NULL;
    segment->right      = NULL;

    if (parent_segment)
    {
        segment->parent = parent_segment;
    }
    else 
    {
        segment->parent = nullptr;
    }

    return segment;
}

treeErrorCode del_segment(TreeSegment* segment)
{
    treeErrorCode error = NO_TREE_ERRORS;

    if (!segment)
    {
        return NULL_SEGMENT_POINTER;
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
    if (segment->type == TEXT_SEGMENT_DATA || segment->type == IDENTIFIER || segment->type == FUNCTION_DEFINITION 
     || segment->type == VAR_DECLARATION)
    {
        free(segment->data.stringPtr);
    }
    
    free(segment);

    return error;
}

treeErrorCode tree_dump(TreeData* tree)
{
    assert(tree);
    #define RETURN(code) do{                    \
        print_tree_error(code);                 \
        fclose(buffer.filePointer);             \
        return code;                            \
    }while(0)

    #define CHECK_FUNCTION_WORK(funk) do{       \
        if ((error = funk))                     \
        {                                       \
            RETURN(error);                      \
        }                                       \
    }while(0)

    outputBuffer buffer = {};
    buffer.AUTO_FLUSH = true;

    treeErrorCode error = NO_TREE_ERRORS;

    if (create_output_file(&(buffer.filePointer), "dump.dot", TEXT))
    {
        RETURN(CREATE_OUTPUT_FILE_ERROR);
    }

    CHECK_FUNCTION_WORK(write_dot_header(&buffer));
    
    CHECK_FUNCTION_WORK(write_dot_body(&buffer, tree));

    CHECK_FUNCTION_WORK(write_dot_footer(&buffer, tree));

    if (write_buffer_to_file(&buffer))
    {
        RETURN(WRITE_TO_OUTPUT_FILE_ERROR);
    }

    fclose(buffer.filePointer);

    if (system("dot dump.dot -Tpng -o dump.png"))
    {
        print_tree_error(NO_GRAPHVIZ_LIB_ERROR);                 
        return NO_GRAPHVIZ_LIB_ERROR;  
    }

    return NO_TREE_ERRORS;

    #undef CHECK_FUNCTION_WORK
    #undef RETURN
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

    switch ((int) segment->type)
    {
        case TEXT_SEGMENT_DATA:
            if (!strncmp(segment->data.stringPtr, (const char*) data, segment->data_len))
            {
                return segment;
            }
            break;
        case DOUBLE_SEGMENT_DATA:
            if (is_equal(segment->data.D_number, *((const double*) data)))
            {
                return segment;
            }
            break;
        case OP_CODE_SEGMENT_DATA:
            if (segment->data.Op_code == *((const int*) data))
            {
                return segment;
            }
            break;
        case VAR_SEGMENT_DATA:
            if (segment->data.Var == *((const int*) data))
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
    if (segment->right && (!ptr))
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

    treeErrorCode error = NO_TREE_ERRORS;

    create_output_file(&buffer.filePointer, filename, TEXT);

    if ((error = write_tree_to_buffer_recursive(&buffer, tree->root)))
    {
        return error;
    }
    
    write_buffer_to_file(&buffer);

    fclose(buffer.filePointer);
    
    return NO_TREE_ERRORS;
}

static treeErrorCode write_tree_to_buffer_recursive(outputBuffer* buffer, const TreeSegment* segment)
{
    assert(buffer);

    if (!segment)
    {
        return NULL_SEGMENT_POINTER;
    }

    print_to_buffer(buffer, "(");

    switch((int) segment->type)
    {
        case TEXT_SEGMENT_DATA:
            print_to_buffer(buffer, "\"%s\" ", segment->data.stringPtr);
            break;
        case DOUBLE_SEGMENT_DATA:
            print_to_buffer(buffer, "%lf ", segment->data.D_number);
            break;
        case OP_CODE_SEGMENT_DATA:
            print_to_buffer(buffer, "\'%c\'", segment->data.Op_code);
            break;
        case VAR_SEGMENT_DATA:
            print_to_buffer(buffer, "~%c~", segment->data.Var);
            break;
        case NO_TYPE_SEGMENT_DATA:
            print_to_buffer(buffer, "NONE ");
            break;

        default:
            break;
    }

    if (segment->left)
    {
        write_tree_to_buffer_recursive(buffer, segment->left);
        print_to_buffer(buffer, " ");
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

    print_to_buffer(buffer, ")");

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

    if (buffer_ctor(&treeBuffer, (size_t) buff.st_size))
    {
        return BUFFER_CTOR_ERROR;
    }

    if (fread(treeBuffer.customBuffer, sizeof(char), treeBuffer.customSize, file) != treeBuffer.customSize)
    {
        return FILE_READ_ERROR;
    }

    if (tree->root)
    {
        if ((error = tree_dtor(tree)))
        {
            return error;
        }
    }

    tree->root = read_tree_from_file_recursive(&treeBuffer, nullptr, &error);

    buffer_dtor(&treeBuffer);
    fclose(file);

    return error;
}

static TreeSegment* read_tree_from_file_recursive(outputBuffer* buffer, TreeSegment* par_segment, treeErrorCode* error)
{
    assert(buffer);

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
            read_string_from_buffer(buffer, seg, error);
            (buffer->bufferPointer)++;
        }
        else if (val.type == OP_CODE_SEGMENT_DATA)
        {
            read_op_code_from_buffer(buffer, seg, error);
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
        del_segment(seg);
        return NULL;
    }
    
    if (((char) buffer->customBuffer[buffer->bufferPointer]) == '(')
    {
        seg->left = read_tree_from_file_recursive(buffer, seg, error);
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
            del_segment(seg);
            return NULL;
        }
    }
    
    if (((char) buffer->customBuffer[buffer->bufferPointer]) == '(')
    {
        seg->right = read_tree_from_file_recursive(buffer, seg, error);
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
            del_segment(seg);
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
        del_segment(seg);
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
    else if (buffer->bufferPointer[buffer->customBuffer] == '\'')
    {
        value.type = OP_CODE_SEGMENT_DATA;
        value.size = sizeof(OpCodes);
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

treeErrorCode copy_segment(TreeSegment* dest, const TreeSegment* src)
{
    assert(dest);
    assert(src);

    if (dest->type != src->type)
    {
        return DIFFERENT_SEGMENT_TYPES;
    }

    switch ((int) dest->type)
    {
    case TEXT_SEGMENT_DATA:
        if (dest->data.stringPtr)
        {
            free(dest->data.stringPtr);
        }  

        dest->data.stringPtr = (char*) calloc(src->data_len + 1, sizeof(char));
        strncpy(dest->data.stringPtr, src->data.stringPtr, src->data_len);
        dest->data_len = src->data_len;

        break;
    case DOUBLE_SEGMENT_DATA:
        dest->data.D_number = src->data.D_number;
        break;
    case OP_CODE_SEGMENT_DATA:
        dest->data.Op_code = src->data.Op_code;
        break;
    case VAR_SEGMENT_DATA:
        dest->data.Var = src->data.Var;
        break;

    case NO_TYPE_SEGMENT_DATA:
        break;
    default:
        break;
    }

    return NO_TREE_ERRORS;
}

treeErrorCode copy_subtree(const TreeSegment* src, TreeSegment** dest, TreeSegment* parent)
{
    assert(src);
    assert(dest);
    treeErrorCode error = NO_TREE_ERRORS;

    *dest = new_segment(src->type, src->data_len, parent, &error);
    if (error) return error;
    (*dest)->data   = src->data;
    (*dest)->type   = src->type;
    (*dest)->weight = src->weight;

    if (src->left)
    {
        if ((error = copy_subtree(src->left, &((*dest)->left), (*dest))))
        {
            return error;
        }
    }
    if (src->right)
    {
        if ((error = copy_subtree(src->right, &((*dest)->right), (*dest))))
        {
            return error;
        }
    }

    return error;
}

static void read_string_from_buffer(outputBuffer* buffer, TreeSegment* segment, treeErrorCode* error)
{
    assert(buffer);
    if (!segment)
    {
        if (error) *error = NULL_SEGMENT_POINTER;
        return ;
    }

    if (buffer->customBuffer[buffer->bufferPointer] != '\"')
    {
        if (error) *error = WRONG_TREE_SYNTAX;
        return ;
    }
    (buffer->bufferPointer)++;

    size_t count = 0;
    while ((buffer->customBuffer[buffer->bufferPointer + count] != '\"') && buffer->customBuffer[buffer->bufferPointer + count] != '\0')
    {
        count++;
    }

    if (buffer->customBuffer[buffer->bufferPointer + count] != '\"')
    {
        if (error) *error = WRONG_TREE_SYNTAX;
        return ;
    }

    (segment->data).stringPtr = (char*) calloc(count + 1, sizeof(char));
    segment->data_len = count;

    for (size_t i = 0; i < count; i++)
    {
        (segment->data).stringPtr[i] = (buffer->customBuffer)[buffer->bufferPointer];
        (buffer->bufferPointer)++;
    }
    (buffer->bufferPointer)++;
}

static void read_op_code_from_buffer(outputBuffer* buffer, TreeSegment* segment, treeErrorCode* error)
{
    assert(buffer);
    if (!segment)
    {
        if (error) *error = NULL_SEGMENT_POINTER;
        return ;
    }

    if (buffer->customBuffer[buffer->bufferPointer] != '\'')
    {
        if (error) *error = WRONG_TREE_SYNTAX;
        return ;
    }
    (buffer->bufferPointer)++;

    size_t count = 0;
    while ((buffer->customBuffer[buffer->bufferPointer + count] != '\'') && buffer->customBuffer[buffer->bufferPointer + count] != '\0')
    {
        count++;
    }

    if (buffer->customBuffer[buffer->bufferPointer + count] != '\'')
    {
        if (error) *error = WRONG_TREE_SYNTAX;
        return ;
    }

    char* opCode = (char*) calloc(count + 1, sizeof(char));

    for (size_t i = 0; i < count; i++)
    {
        opCode[i] = (buffer->customBuffer)[buffer->bufferPointer];
        (buffer->bufferPointer)++;
    }
    (buffer->bufferPointer)++;

    if      (!strcmp("+", opCode))   segment->data.Op_code = PLUS;
    else if (!strcmp("-", opCode))   segment->data.Op_code = MINUS;
    else if (!strcmp("*", opCode))   segment->data.Op_code = MUL;
    else if (!strcmp("/", opCode))   segment->data.Op_code = DIV;
    else if (!strcmp("sin", opCode)) segment->data.Op_code = SIN;
    else if (!strcmp("cos", opCode)) segment->data.Op_code = COS;
    else if (!strcmp("tan", opCode)) segment->data.Op_code = TAN;
    else segment->data.Op_code = NONE;

    free(opCode);
}

static bool is_equal(const double first, const double second)
{
    return fabs(first - second) < D_EPSILON;
}