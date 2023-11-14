#include <stdio.h>



listErrorCode write_dot_header(outputBuffer* buffer)
{
    assert(buffer);

    print_to_buffer(buffer, "digraph G{\n"
                            "rankdir = LR;\n"
                            "bgcolor = \"#FFEFD5\";\n"
                            "node[color = \"#800000\", fontsize = 10];\n"
                            "edge[color = \"#800000\", fontsize = 15];\n\n");

    return NO_LIST_ERRORS;
}

listErrorCode write_dot_body(outputBuffer* buffer, ListData* list)
{
    assert(buffer);
    assert(list);

    /*
    for (size_t i = 0; i < list->capacity - 1; i++)
    {
        print_to_buffer(buffer, "%lu -> ", i);
    }
    print_to_buffer(buffer, "%lu [weight = 10000, color = \"#FFEFD5\"];\n\n", list->capacity - 1);
    */

    for (size_t i = 0; i < list->capacity; i++)
    {
        print_to_buffer(buffer, "%lu [shape = Mrecord, style = filled, fillcolor = \"#FFF5EE\", color = \"#800000\", label = "
                                "\" IP: %lu | DATA: %d | NEXT: %ld | PERV: %ld \"];\n", i, i, list->data[i],
                                 list->next[i], list->prev[i]);
    }

    print_to_buffer(buffer, "\n");

    ssize_t adress = 0;
    for (size_t i = 0; i < list->len; i++)
    {
        print_to_buffer(buffer, "%ld -> %ld [weight = 1, color = \"#0000ff\"];\n", adress, list->next[adress]);
        adress = list->next[adress];
    }

    adress = list->tail;
    for (size_t i = 0; i < list->len; i++)
    {
        print_to_buffer(buffer, "%ld -> %ld [weight = 1, color = \"#00ff00\"];\n", adress, list->prev[adress]);
        adress = list->prev[adress];
    }

    adress = list->free;
    for (size_t i = 0; i < (list->capacity - list->len); i++)
    {
        print_to_buffer(buffer, "%ld -> %ld [weight = 1,color = \"#0000ff\"];\n", adress, list->next[adress]);
        adress = list->next[adress];
    }

    return NO_LIST_ERRORS;
}

listErrorCode write_dot_footer(outputBuffer* buffer, ListData* list)
{
    assert(buffer);
    assert(list);

    print_to_buffer(buffer, "All[shape = Mrecord, label = "
                            "\" HEADER | <f0> Capacity: %lu | <f1> Head: %ld | <f2> Tail: %ld | <f3> Free: %ld \""
                            ", style = \"filled\", fillcolor = \"#FFF5EE\"];\n",
                            list->capacity, list->head, list->tail, list->free);
    print_to_buffer(buffer, "All:<f1> -> %ld [color = \"#000000\"];\n", list->head);
    print_to_buffer(buffer, "All:<f2> -> %ld [color = \"#000000\"];\n", list->tail);
    print_to_buffer(buffer, "All:<f3> -> %ld [color = \"#000000\"];\n", list->free);

    print_to_buffer(buffer, "}");

    return NO_LIST_ERRORS;
}