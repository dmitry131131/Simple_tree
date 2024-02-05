#include <stdio.h>
#include <string.h>
#include <assert.h>

#include "TreeErrors.h"
#include "DataBuffer.h"
#include "Tree.h"
#include "TreeLog.h"

static treeErrorCode write_dot_elem_recursive(outputBuffer* buffer, TreeSegment* segment, TreeSegment* call_segment);
static treeErrorCode write_command_by_opcode(char (*str)[TREE_TEXT_SEGMENT_DATA_LEN], OpCodes code);
static void dump_key_word(outputBuffer* buffer, TreeSegment* segment);
static const char* write_key_word_by_code(KeyWords K_word);

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
        print_to_buffer(buffer, "%lu [shape = Mrecord, style = filled, fillcolor = \"#d19e1b93\", color = \"#800000\", label = "
                                "\" {{DATA: %lu | TYPE: ID(5)} | {<fl> LEFT | <fr> RIGHT}} \"];\n",
                                segment, segment->data.Id);
        break;
    case KEYWORD:
        dump_key_word(buffer, segment);
        break;
    case FUNCTION_DEFINITION:
        print_to_buffer(buffer, "%lu [shape = Mrecord, style = filled, fillcolor = \"#1fcf2593\", color = \"#800000\", label = "
                                "\" {{DATA: %lu | TYPE: FUNCTION DEF(7)} | {<fl> LEFT | <fr> RIGHT}} \"];\n",
                                segment, segment->data.Id);
        break;
    case PARAMETERS:
        print_to_buffer(buffer, "%lu [shape = Mrecord, style = filled, fillcolor = \"#1743b393\", color = \"#800000\", label = "
                                "\" {{DATA: NONE | TYPE: PARAMETERS(8)} | {<fl> LEFT | <fr> RIGHT}} \"];\n",
                                segment);
        break;
    case VAR_DECLARATION:
        print_to_buffer(buffer, "%lu [shape = Mrecord, style = filled, fillcolor = \"#1fcf2593\", color = \"#800000\", label = "
                                "\" {{DATA: %lu | TYPE: VAR DECLARATION(9)} | {<fl> LEFT | <fr> RIGHT}} \"];\n",
                                segment, segment->data.Id);
        break;
    case CALL:
        print_to_buffer(buffer, "%lu [shape = Mrecord, style = filled, fillcolor = \"#1743b393\", color = \"#800000\", label = "
                                "\" {{DATA: NONE | TYPE: CALL(10)} | {<fl> LEFT | <fr> RIGHT}} \"];\n",
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

static void dump_key_word(outputBuffer* buffer, TreeSegment* segment)
{
    assert(buffer);
    assert(segment);

    switch ((size_t) segment->data.K_word)
    {
    case KEY_NEXT:
        print_to_buffer(buffer, "%lu [shape = Mrecord, style = filled, fillcolor = \"#fa7f7f93\", color = \"#800000\", label = "
                                "\" {{DATA: %s | TYPE: KEYWORD(6)} | {<fl> LEFT | <fr> RIGHT}} \"];\n",
                                segment, write_key_word_by_code(segment->data.K_word));
        break;
    
    default:
        print_to_buffer(buffer, "%lu [shape = Mrecord, style = filled, fillcolor = \"#a32e9793\", color = \"#800000\", label = "
                                "\" {{DATA: %d | TYPE: KEYWORD(6)} | {<fl> LEFT | <fr> RIGHT}} \"];\n",
                                segment, segment->data.K_word);
        break;
    }
}

static const char* write_key_word_by_code(KeyWords K_word)
{
    switch (K_word)
    {
    case KEY_IF:
        return "if";
    case KEY_WHILE:
        return "while";
    case KEY_ASSIGMENT:
        return "=";
    case KEY_SIN:
        return "sin";
    case KEY_COS:
        return "cos";
    case KEY_FLOOR:
        return "floor";
    case KEY_PLUS:
        return "+";
    case KEY_MINUS:
        return "-";
    case KEY_MUL:
        return "*";
    case KEY_DIV:
        return "/";
    case KEY_DIFF:
        return "diffetent";
    case KEY_EQUAL:
        return "==";
    case KEY_LESS:
        return "Less";
    case KEY_MORE:
        return "More";
    case KEY_LESS_EQUAL:
        return "Less || Equal";
    case KEY_MORE_EQUAL:
        return "More || Equal";
    case KEY_NOT_EQUAL:
        return "Not equal";
    case KEY_AND:
        return "&&";
    case KEY_OR: 
        return "||";
    case KEY_NOT:       
        return "!";
    case KEY_NEXT:      
        return ";";
    case KEY_ENUM:     
        return ",";
    case KEY_NUMBER:    
        return "number(int)";
    case KEY_DEF:   
        return "def";
    case KEY_IN:      
        return "in";
    case KEY_OUT:       
        return "out";
    case KEY_RETURN:    
        return "return";
    case KEY_BREAK:     
        return "break";
    case KEY_CONTINUE:  
        return "continue";
    case KEY_OBR:      
        return "(";
    case KEY_CBR:      
        return ")";
    case KEY_O_CURBR:  
        return "{";
    case KEY_C_CURBR: 
        return "}";
    
    default:
        return "????";
    }
}