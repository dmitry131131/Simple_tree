#include <stdio.h>
#include <string.h>

#include "Tree.h"

int main()
{
    #define RETURN(err) do{                 \
        tree_dtor(&tree);                   \
        return 0;                           \
    }while(0)

    TreeData tree = {};
    treeErrorCode error = NO_TREE_ERRORS;

    /*
    if ((error = tree_ctor(&tree)))
    {
        print_tree_error(error);
        RETURN(error);
    }
    
    TreeSegment* seg = new_segment(DOUBLE_SEGMENT_DATA, sizeof(double), &(tree.root->left), &error);

    seg->data.D_number = 8.34;

    if (error)
    {
        print_tree_error(error);
        RETURN(error);
    }

    wrire_tree_to_file("hh.txt", &tree);
    */

    if ((error = read_tree_from_file(&tree, "hh.txt")))
    {
        print_tree_error(error);
        RETURN(error);
    }

    tree_dump(&tree);

    RETURN(0);

    #undef RETURN
}