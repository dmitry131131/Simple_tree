#include <stdio.h>
#include <string.h>
#include <assert.h>

#include "TreeErrors.h"
#include "DataBuffer.h"
#include "Tree.h"
#include "TreeLog.h"

static treeErrorCode write_dot_elem_recursive(outputBuffer* buffer, TreeSegment* segment, TreeSegment* call_segment);

static treeErrorCode write_command_by_opcode(char (*str)[TREE_TEXT_SEGMENT_DATA_LEN], OpCodes code);

treeErrorCode write_dot_header(outputBuffer* buffer)
{
    assert(buffer);

    print_to_buffer(buffer, "digraph G{\n"
                            "rankdir = TB;\n"
                            "bgcolor = \"#FFEFD5\";\n"
                            "node[color = \"#800000\", fontsize = 10];\n"
                            "edge[color = \"#800000\", fontsize = 15];\n\n");

    return NO_TREE_ERRORS;
}

treeErrorCode write_dot_body(outputBuffer* buffer, TreeData* tree)
{
    assert(buffer);
    assert(tree);
    treeErrorCode error = NO_TREE_ERRORS;

    error = write_dot_elem_recursive(buffer, tree->root, tree->root);

    return error;
}

static treeErrorCode write_dot_elem_recursive(outputBuffer* buffer, TreeSegment* segment, TreeSegment* call_segment)
{
    assert(buffer);

    if (!segment)
    {
        return NULL_SEGMENT_POINTER;
    }

    treeErrorCode error = NO_TREE_ERRORS;

    switch (segment->type)
    {
    case TEXT_SEGMENT_DATA:
        print_to_buffer(buffer, "%lu [shape = Mrecord, style = filled, fillcolor = \"#FFF5EE\", color = \"#800000\", label = "
                                "\" {{DATA: %s | TYPE: %d} | {<fl> LEFT | <fr> RIGHT}} \"];\n",
                                segment, segment->data.stringPtr, segment->type);
        break;
    case DOUBLE_SEGMENT_DATA:
        print_to_buffer(buffer, "%lu [shape = Mrecord, style = filled, fillcolor = \"#FFF5EE\", color = \"#800000\", label = "
                                "\" {{DATA: %lf | TYPE: Number(2)} | {<fl> LEFT | <fr> RIGHT}} \"];\n",
                                segment, segment->data.D_number);
        break;    
    case OP_CODE_SEGMENT_DATA:
        {
            char opCode[TREE_TEXT_SEGMENT_DATA_LEN] = {};
            write_command_by_opcode(&opCode, segment->data.Op_code);

            print_to_buffer(buffer, "%lu [shape = Mrecord, style = filled, fillcolor = \"#FFF5EE\", color = \"#800000\", label = "
                                    "\" {{DATA: %s | TYPE: %d} | {<fl> LEFT | <fr> RIGHT}} \"];\n",
                                    segment, opCode, segment->type);
        }
        break;
    case VAR_SEGMENT_DATA:
        print_to_buffer(buffer, "%lu [shape = Mrecord, style = filled, fillcolor = \"#FFF5EE\", color = \"#800000\", label = "
                                "\" {{DATA: x | TYPE: %d} | {<fl> LEFT | <fr> RIGHT}} \"];\n",
                                segment, segment->type);
        break;
    
    case IDENTIFIER:
        print_to_buffer(buffer, "%lu [shape = Mrecord, style = filled, fillcolor = \"#1fcf2593\", color = \"#800000\", label = "
                                "\" {{DATA: %lu | TYPE: Identifier(5)} | {<fl> LEFT | <fr> RIGHT}} \"];\n",
                                segment, segment->data.Id);
        break;
    case KEYWORD:
        print_to_buffer(buffer, "%lu [shape = Mrecord, style = filled, fillcolor = \"#a32e9793\", color = \"#800000\", label = "
                                "\" {{DATA: %d | TYPE: KEYWORD(6)} | {<fl> LEFT | <fr> RIGHT}} \"];\n",
                                segment, segment->data.K_word);
        break;
    case FUNCTION_DEFINITION:
        print_to_buffer(buffer, "%lu [shape = Mrecord, style = filled, fillcolor = \"#1fcf2593\", color = \"#800000\", label = "
                                "\" {{DATA: %lu | TYPE: Function definition(7)} | {<fl> LEFT | <fr> RIGHT}} \"];\n",
                                segment, segment->data.Id);
        break;
    case PARAMETERS:
        print_to_buffer(buffer, "%lu [shape = Mrecord, style = filled, fillcolor = \"#1743b393\", color = \"#800000\", label = "
                                "\" {{DATA: NONE | TYPE: Parameters(8)} | {<fl> LEFT | <fr> RIGHT}} \"];\n",
                                segment);
        break;
    case VAR_DECLARATION:
        print_to_buffer(buffer, "%lu [shape = Mrecord, style = filled, fillcolor = \"#1fcf2593\", color = \"#800000\", label = "
                                "\" {{DATA: %lu | TYPE: Var declaration(9)} | {<fl> LEFT | <fr> RIGHT}} \"];\n",
                                segment, segment->data.Id);
        break;
    case CALL:
        print_to_buffer(buffer, "%lu [shape = Mrecord, style = filled, fillcolor = \"#1743b393\", color = \"#800000\", label = "
                                "\" {{DATA: NONE | TYPE: Call(10)} | {<fl> LEFT | <fr> RIGHT}} \"];\n",
                                segment);
        break;

    case NO_TYPE_SEGMENT_DATA:
    default:
        print_to_buffer(buffer, "%lu [shape = Mrecord, style = filled, fillcolor = \"#FFF5EE\", color = \"#800000\", label = "
                                "\" {{DATA: %s | TYPE: %d} | {<fl> LEFT | <fr> RIGHT}} \"];\n",
                                segment, segment->data, segment->type);
        break;
    }

    if (segment != call_segment)
    {
        if (segment == call_segment->left)
        {
            print_to_buffer(buffer, "%lu:<fl> -> %lu [weight = 1, color = \"#0000ff\"];\n", call_segment, segment);
        }
        else if (segment == call_segment->right)
        {
            print_to_buffer(buffer, "%lu:<fr> -> %lu [weight = 1, color = \"#0000ff\"];\n", call_segment, segment);
        }
    }

    if (segment->left)
    {
        error = write_dot_elem_recursive(buffer, segment->left, segment);
    }
    if (segment->right)
    {
        error = write_dot_elem_recursive(buffer, segment->right, segment);
    }

    return error;
}

treeErrorCode write_dot_footer(outputBuffer* buffer, TreeData* tree)
{
    assert(buffer);
    assert(tree);

    print_to_buffer(buffer, "All[shape = Mrecord, label = "
                            "\" HEADER | <f1> ROOT\""
                            ", style = \"filled\", fillcolor = \"#FFF5EE\"];\n",
                            tree->root);
    print_to_buffer(buffer, "All:<f1> -> %lu [color = \"#000000\"];\n", tree->root);

    print_to_buffer(buffer, "}");

    return NO_TREE_ERRORS;
}

static treeErrorCode write_command_by_opcode(char (*str)[TREE_TEXT_SEGMENT_DATA_LEN], OpCodes code)
{
    assert(str);

    switch (code)
    {
    case NONE:
        strncpy(*str, "NONE", TREE_TEXT_SEGMENT_DATA_LEN);
        break;
    case PLUS:
        strncpy(*str, "+", TREE_TEXT_SEGMENT_DATA_LEN);
        break;
    case MINUS:
        strncpy(*str, "-", TREE_TEXT_SEGMENT_DATA_LEN);
        break;
    case MUL:
        strncpy(*str, "*", TREE_TEXT_SEGMENT_DATA_LEN);
        break;
    case DIV:
        strncpy(*str, "/", TREE_TEXT_SEGMENT_DATA_LEN);
        break;
    case SIN:
        strncpy(*str, "sin", TREE_TEXT_SEGMENT_DATA_LEN);
        break;
    case COS:
        strncpy(*str, "cos", TREE_TEXT_SEGMENT_DATA_LEN);
        break;
    case TAN:
        strncpy(*str, "tan", TREE_TEXT_SEGMENT_DATA_LEN);
        break;
    case OBR:
        strncpy(*str, "obr", TREE_TEXT_SEGMENT_DATA_LEN);
        break;
    case CBR:
        strncpy(*str, "cbr", TREE_TEXT_SEGMENT_DATA_LEN);
        break;
    case POW:
        strncpy(*str, "^", TREE_TEXT_SEGMENT_DATA_LEN);
        break;
    case LN:
        strncpy(*str, "ln", TREE_TEXT_SEGMENT_DATA_LEN);
        break;
    
    default:
        break;
    }

    return NO_TREE_ERRORS;
}