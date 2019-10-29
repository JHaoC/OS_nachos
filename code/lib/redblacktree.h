// redblacktree.h 
//	Data structures defining a redblacktree -- an balance search tree each node of which
//	can be fetched, inserted and deleted by O(log(n)). 
//
//	The rea black tree can be a structure for CFS schedule of ready queue.

#ifndef REDBLACKTREE_H
#define REDBLACKTREE_H

// The tree design according to the https://www.cs.auckland.ac.nz/software/AlgAnim/red_black.html
// more tree info, please read the link directly.

// define Color red or black
enum Color {Red, Black};

// Node is an element in the RedBlackTree, Each node has a value and color.
// And three pointers named parents, left and right.
template <class T>
struct Node
{
    Color color;
    int item_value;
    T item;
    Node<T>* parent_node;
    Node<T>* left_node;
    Node<T>* right_node;
    Node(T tItem);
    ~Node();
    //std::string toString(); 
};

template <class T>
class RedBlackTree
{
    public:
        RedBlackTree();
        ~RedBlackTree();
        void Insert(Node<T>* node);
        Node<T>* Delele(); // delete the LowestNode always and Return the node
        Node<T>* GetLowestNode();
        int GetNumofNodes();
        //void PrintTree();
        bool IsEmpty();
        // void Apply(void (*func)(T)) const;
        
        
    private:
        Node<T>* root;
        int numsofNode;
        Node<T>* FindSibling(Node<T>* node);
        void Remove(Node<T>* node);
        void Recolor(Node<T>* node, Color cr);
        void DoubleBlack(Node<T> * node);
        void CheckNode(Node<T>* node);
        void LeftRotate(Node<T>* node);
        void RightRotate(Node<T>* Node);
        void InsertHelper(Node<T>* target, Node<T>* rt, Node<T>* parent, bool isLeft);
        //std::string helperForTree(Node<T>* rt);
        // void helperForApply(Node<T> *rt, void (*f)(T)) const;

};

#include "redblacktree.cc"

#endif