// redblacktree.cc
// declare function for redblacktree

#include "redblacktree.h"
#include <stdlib.h>


//----------------------------------------------------------------------
// Node::Node(int val)
// 	Initialize a Node
//----------------------------------------------------------------------
template <class T>
Node<T>::Node(T tItem)
{
    color = Red;
    item_value = 0;
    parent_node = left_node = right_node = NULL;
    item = tItem;
}

//----------------------------------------------------------------------
// Node::~Node()
// 	Destructor
//----------------------------------------------------------------------
template <class T>
Node<T>::~Node()
{
    //if(parent_node != NULL) delete parent_node;
    if (left_node != NULL)
        delete left_node;
    if (right_node != NULL)
        delete right_node;
    //if(item != NULL) item = NULL;
    if(parent_node != NULL) parent_node = NULL;
}

// //----------------------------------------------------------------------
// // Node::toString()
// // 	Destructor
// //----------------------------------------------------------------------
// template <class T>
// std::string Node<T>::toString()
// {
//     if(this == NULL) return NULL;
//     std::string color_str;
//     if (color == Red)
//     {
//         color_str = " RED ";
//     }
//     else
//     {
//         color_str = " BLACK ";
//     }
//     return color_str + to_string(item_value);
// }

// //----------------------------------------------------------------------
// // Node::Node(const Node & nd )
// // 	Copy a Node
// //----------------------------------------------------------------------
// Node::Node(const Node &nd)
// {
//     color = nd.color;
//     item_value = nd.item_value;
//     if(nd.parent_node!=NULL)
//     {
//         parent_node = new Node(nd.parent_node->item_value);
//     }
//     if(nd.left_node!=NULL)
//     {
//         left_node = new Node(nd.left_node->item_value);
//     }
//     if(nd.right_node!=NULL)
//     {
//         right_node = new Node(nd.right_node->item_value);
//     }
// }

// //----------------------------------------------------------------------
// // Node::Node(Node && nd )
// // 	move constructor
// //----------------------------------------------------------------------
// Node::Node(Node &&nd)
// {
//     color = nd.color;
//     item_value = nd.item_value;
//     parent_node = nd.parent_node;
//     nd.parent_node = NULL;
//     left_node = nd.left_node;
//     nd.left_node =NULL;
//     right_node = nd.right_node;
//     nd.right_node = NULL;
// }

//----------------------------------------------------------------------
// RedBlackTree::ReBlackTree()
// 	Initialize a tree
//----------------------------------------------------------------------
template <class T>
RedBlackTree<T>::RedBlackTree()
{
    root = NULL;
    numsofNode = 0;
}

//----------------------------------------------------------------------
// RedBlackTree::~ReBlackTree()
// 	Destructor
//----------------------------------------------------------------------
template <class T>
RedBlackTree<T>::~RedBlackTree()
{
    if (root != NULL)
    {
        delete root;
    }
}

//----------------------------------------------------------------------
// RedBlackTree::FindUncle(Node* node)
// 	Return the node's uncle node;
//----------------------------------------------------------------------
template <class T>
Node<T>* RedBlackTree<T>::FindSibling(Node<T> *node)
{
    if (node == NULL)
        return NULL;
    if (node->parent_node == NULL)
        return NULL;
    Node<T> *parent = node->parent_node;
    if (parent->right_node == node)
    {
        return parent->left_node;
    }
    return parent->right_node;
}

//----------------------------------------------------------------------
// RedBlackTree::Recolor(Node* node, Color cr)
//  Change node color to cr
//----------------------------------------------------------------------
template <class T>
void RedBlackTree<T>::Recolor(Node<T> *node, Color cr)
{
    node->color = cr;
}

//----------------------------------------------------------------------
// InsertHelper(Node* target, Node* root, Node* parent, bool isLeft)
// insert a node into root
//----------------------------------------------------------------------
template <class T>
void RedBlackTree<T>::InsertHelper(Node<T> *target, Node<T> *rt, Node<T> *parent, bool isLeft)
{
    if (rt == NULL)
    {
        rt = target;
        target->parent_node = parent;
        if (parent != NULL)
        {
            if (isLeft)
            {
                parent->left_node = rt;
            }
            else
            {
                parent->right_node = rt;
            }
        }
        return;
    }
    if (rt->item_value < target->item_value)
    {
        InsertHelper(target, rt->right_node, rt, false);
    }
    else
    {
        InsertHelper(target, rt->left_node, rt, true);
    }
}

//----------------------------------------------------------------------
// RedBlackTree::Insert(Node* node)
// insert a node into tree based on the value of node
//----------------------------------------------------------------------
template <class T>
void RedBlackTree<T>::Insert(Node<T>* node)
{
    InsertHelper(node, this->root, NULL, true);
    numsofNode++;
    //std::cout << "After insert: " << std::endl;
    //PrintTree();
    CheckNode(node);
}

//----------------------------------------------------------------------
// RedBlackTree::LeftRotate(Node* node)
// lefe Rotate b
// a <- b ->c  => a<- b<- c->NULL
//----------------------------------------------------------------------
template <class T>
void RedBlackTree<T>::LeftRotate(Node<T> *node)
{
    Node<T> *oldparent = node->parent_node;
    Node<T> *right = node->right_node;
    Node<T> *rightleft = right->left_node;
    node->parent_node = right;
    node->right_node = rightleft;
    if(rightleft!=NULL)
    {
        rightleft->parent_node = node;
    }
    right->left_node = node;
    right->parent_node = oldparent;
    if (oldparent != NULL)
    {
        bool isleft = oldparent->left_node == node;
        if (isleft)
        {
            oldparent->left_node = right;
        }
        else
        {
            oldparent->right_node = right;
        }
    }
    else
    {
        root = right;
    }
    
}

//----------------------------------------------------------------------
// RedBlackTree::RightRotate(Node* node)
// Right Rotate b
// a <- b ->c  => a-> b -> c
//----------------------------------------------------------------------
template <class T>
void RedBlackTree<T>::RightRotate(Node<T> *node)
{
    Node<T> *oldparent = node->parent_node;
    Node<T> *left = node->left_node;
    Node<T> *leftright = left->right_node;
    node->parent_node = left;
    node->left_node = leftright;
    if(leftright != NULL)
    {
        leftright->parent_node = node;
    }
    left->right_node = node;
    left->parent_node = oldparent;
    if (oldparent != NULL)
    {
        bool isleft = oldparent->left_node == node;

        if (isleft)
        {
            oldparent->left_node = left;
        }
        else
        {
            oldparent->right_node = left;
        }
    }
    else
    {
        root = left;
    }
    
}

//----------------------------------------------------------------------
// RedBlackTree::CheckNode(Node* node)
// Check rules from the node
//----------------------------------------------------------------------
template <class T>
void RedBlackTree<T>::CheckNode(Node<T> *node)
{
    if(node == NULL) return;
    if (node->parent_node == NULL)
    {
        if (node->color != Black)
        {
            node->color = Black;
        }
        root = node;
        return;
    }
    if(node->color == Black)
    {
        CheckNode(node->parent_node);
        return;
    }
    Node<T>* parent = node->parent_node;
    if (parent->color == Red)
    {
        Node<T> *uncle = FindSibling(parent);
        if (uncle != NULL && uncle->color == Red) // uncle is red
        {
            Recolor(parent, Black);
            Recolor(uncle, Black);
            Recolor(parent->parent_node, Red);
            CheckNode(parent->parent_node);
            return;
        }
        else // uncle is black or uncle is NUll
        {
            //four cases configure
            Node<T> *grandParent = parent->parent_node;
            if (grandParent == NULL && uncle == NULL) //???
            {
                CheckNode(parent);
                return;
            }
            if (grandParent->left_node == parent)
            {
                if (parent->right_node == NULL || parent->right_node->color == Black)
                {
                    RightRotate(grandParent);
                    Recolor(parent, Black);
                    Recolor(grandParent, Red);
                    CheckNode(parent);
                }
                if (parent->left_node == NULL || parent->left_node->color == Black)
                {
                    LeftRotate(parent);
                    CheckNode(parent);
                }
            }
            else
            {
                if (parent->left_node == NULL || parent->left_node->color == Black)
                {
                    LeftRotate(grandParent);
                    Recolor(parent, Black);
                    Recolor(grandParent, Red);
                    CheckNode(parent);
                }
                if (parent->right_node == NULL || parent->right_node->color == Black)
                {
                    RightRotate(parent);
                    CheckNode(parent);
                }
            }
        }
    }
}

//----------------------------------------------------------------------
// RedBlackTree::GetLowestNode()
// Return the lowestLest;
//----------------------------------------------------------------------
template <class T>
Node<T>* RedBlackTree<T>::GetLowestNode()
{
    Node<T> *lowestleft = root;
    while (lowestleft!= NULL && lowestleft->left_node != NULL)
    {
        lowestleft = lowestleft->left_node;
    }
    return lowestleft;
}

//----------------------------------------------------------------------
// RedBlackTree::GetNumofNodes()
// Return # num of Nodes;
//----------------------------------------------------------------------
template <class T>
int RedBlackTree<T>::GetNumofNodes()
{
    return numsofNode;
}

//----------------------------------------------------------------------
// RedBlackTree::Remove(Node* node)
// Remove a node from the tree
//----------------------------------------------------------------------
template <class T>
void RedBlackTree<T>::Remove(Node<T> *node)
{
    Node<T> *parent = node->parent_node;
    if (parent == NULL) // node is root
    {
        // root has not child
        if (node->right_node == NULL)
        {
            root = NULL;
        }
        else // root has a right child
        {
            root = node->right_node;
            root->parent_node = NULL;
            Recolor(root, Black);
            node->right_node = NULL;
        }
        return;
    }
    bool isleft = parent->left_node == node;
    if (isleft)
    {
        parent->left_node = NULL;
    }
    else
    {
        parent->right_node = NULL;
    }
    node->parent_node = NULL;
}

//----------------------------------------------------------------------
// RedBlackTree::DoubleBlack(Node* node)
// the node is double black position
// release double black
//----------------------------------------------------------------------
template <class T>
void RedBlackTree<T>::DoubleBlack(Node<T> *node)
{
    // if node is root, terminate it.
    if (node->parent_node == NULL)
        return;
    // Double Black with red and single black
    // if left low node color is red, sibiling is NULL 
    if (node->color == Red) 
    {
        Recolor(node, Black);
        return;
    }

    Node<T>* sibling = FindSibling(node);
    // sibling would not be null otherwise not balance
    if (sibling->color == Black) //sibling is black
    {
        // sibling has no red child 
        if ((sibling->left_node == NULL || sibling->left_node->color==Black) && (sibling->right_node == NULL || sibling->right_node->color==Black))
        {
            Recolor(sibling, Red);
            DoubleBlack(node->parent_node);
            return;
        }
        // sibling has red righe child 
        if ( sibling->right_node != NULL && sibling->right_node->color == Red)
        {
            Node<T> *parent = node->parent_node;
            Recolor(sibling, parent->color);
            Recolor(parent,Black);
            Recolor(sibling->right_node,Black);
            LeftRotate(parent);
            return;
        }
        // sibling has red left child
        if (sibling->left_node != NULL && sibling->left_node->color == Red)
        {
            Recolor(sibling,Red);
            Recolor(sibling->left_node, Black);
            RightRotate(sibling);
            DoubleBlack(node);
            return;
        }
    }
    else // sibiling is red which means parent is black
    {
        Node<T> *parent = node->parent_node;
        Recolor(sibling, Black);
        Recolor(parent, Red);
        LeftRotate(parent);
        DoubleBlack(node);
        //resNode->parent_node = NULL;
    }
}

//----------------------------------------------------------------------
// RedBlackTree::Delete()
// Delete the LowestNode from the three
// return the node
//----------------------------------------------------------------------
template <class T>
Node<T>* RedBlackTree<T>::Delele()
{
    numsofNode--;
    Node<T> *resNode = GetLowestNode();
    // resNode is root
    // if the lowest node is red, it would not has child otherwise is not balance
    if (resNode != root && resNode->right_node == NULL)
    {
        DoubleBlack(resNode);
    }
    // if the lowest node is black and has right child node
    // the right node must be Red otherwise is not balance
    if (resNode->right_node != NULL)
    {
        LeftRotate(resNode);
        Recolor(resNode->parent_node, Black);
    }
    // ResNode is black and not child, if delete it,
    // it will be double black case and checking sibling
    Remove(resNode);
    return resNode;
}

//----------------------------------------------------------------------
// RedBlackTree::PrintTree()
// print left - root - right : tree (T R/B left root right)
//
//----------------------------------------------------------------------
// template <class T>
// void RedBlackTree<T>::PrintTree()
// {
//     helperForTree(root);
// }

// template <class T>
// void RedBlackTree<T>::helperForTree(Node<T> *rt)
// {
//     if (rt == NULL)
//     {
//         printf(" NULL ");
//     }

//     if (rt->color == Red)
//     {
//         color = " RED ";
//     }
//     else
//     {
//         color = " BLACK ";
//     }
//     if (rt->left_node == NULL && rt->right_node == NULL)
//     {
//         return "( T" + color + to_string(rt->item_value) + " ) ";
//     }
//     return "( T" + color + helperForTree(rt->left_node) + to_string(rt->item_value) + " " + helperForTree(rt->right_node) + " ) ";
// }

//----------------------------------------------------------------------
// RedBlackTree::IsEmpty()
//  return # of nodes == 0
//----------------------------------------------------------------------
template <class T>
bool RedBlackTree<T>::IsEmpty()
{
    return numsofNode == 0;
}

// //----------------------------------------------------------------------
// // RedBlackTree::Apply
// //      Apply function to every item on a list.
// //
// //	"func" -- the function to apply
// //----------------------------------------------------------------------

// template <class T>
// void
// RedBlackTree<T>::Apply(void (*func)(T)) const
// { 
//     helperForApply(root, void(*func)(T));
// }

// template <class T>
// void
// RedBlackTree<T>::helperForApply(Node<T> *rt, void (*func)(T)) const
// {
//     if (rt == NULL)
//     {
//         return;
//     }
//     if (rt->left_node == NULL && rt->right_node == NULL)
//     {
//         (*func)(rt->item);
//         return;
//     }

//     helperForApply(rt->left_node,void (*func)(T));
//     (*func)(rt->item);
//     helperForApply(rt->right_node,void (*func)(T));
// }
