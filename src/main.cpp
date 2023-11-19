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

    new_segment(&(tree.root->left), &error);
    new_segment(&(tree.root->right), &error);

    new_segment(&(tree.root->right->right), &error);
    new_segment(&(tree.root->right->left), &error);

    if (error)
    {
        print_tree_error(error);
        RETURN(error);
    }

    tree_dump(&tree);

    RETURN(0);

    #undef RETURN
}