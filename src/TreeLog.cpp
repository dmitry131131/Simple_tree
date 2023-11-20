#include <stdio.h>
#include <assert.h>

#include "TreeErrors.h"
#include "DataBuffer.h"
#include "Tree.h"
#include "TreeLog.h"

static treeErrorCode write_dot_elem_recursive(outputBuffer* buffer, TreeSegment* segment, TreeSegment* call_segment);

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

    write_dot_elem_recursive(buffer, tree->root, tree->root);

    return NO_TREE_ERRORS;
}

static treeErrorCode write_dot_elem_recursive(outputBuffer* buffer, TreeSegment* segment, TreeSegment* call_segment)
{
    assert(buffer);
    if (segment->type == TEXT_SEGMENT_DATA)
    {
        print_to_buffer(buffer, "%lu [shape = Mrecord, style = filled, fillcolor = \"#FFF5EE\", color = \"#800000\", label = "
                                "\" {{DATA: %s | TYPE: %d} | {<fl> LEFT | <fr> RIGHT}} \"];\n",
                                segment, segment->data.stringPtr, segment->type);
    }
    else if (segment->type == DOUBLE_SEGMENT_DATA)
    {
        print_to_buffer(buffer, "%lu [shape = Mrecord, style = filled, fillcolor = \"#FFF5EE\", color = \"#800000\", label = "
                                "\" {{DATA: %lf | TYPE: %d} | {<fl> LEFT | <fr> RIGHT}} \"];\n",
                                segment, segment->data.D_number, segment->type);
    }
    else if (segment->type == INTEGER_SEGMENT_DATA)
    {
        print_to_buffer(buffer, "%lu [shape = Mrecord, style = filled, fillcolor = \"#FFF5EE\", color = \"#800000\", label = "
                                "\" {{DATA: %d | TYPE: %d} | {<fl> LEFT | <fr> RIGHT}} \"];\n",
                                segment, segment->data.I_number, segment->type);
    }
    else 
    {
        print_to_buffer(buffer, "%lu [shape = Mrecord, style = filled, fillcolor = \"#FFF5EE\", color = \"#800000\", label = "
                                "\" {{DATA: %s | TYPE: %d} | {<fl> LEFT | <fr> RIGHT}} \"];\n",
                                segment, segment->data, segment->type);
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
        write_dot_elem_recursive(buffer, segment->left, segment);
    }
    if (segment->right)
    {
        write_dot_elem_recursive(buffer, segment->right, segment);
    }

    return NO_TREE_ERRORS;
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