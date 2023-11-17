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

    if ((error = tree_ctor(&tree)))
    {
        print_tree_error(error);
        RETURN(error);
    }

    tree.root->left = new_segment(&tree, &error);

    tree.root->right = new_segment(&tree, &error);

    tree.root->right->right = new_segment(&tree, &error);

    if (error)
    {
        print_tree_error(error);
        RETURN(error);
    }

    tree_dump(&tree);

    RETURN(0);

    #undef RETURN
}