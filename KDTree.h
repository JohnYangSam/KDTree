/********************************************************************
 * File: KDTree.h
 * Author: TODO: Your name here!
 *
 * An interface representing a kd-tree in some number of dimensions.
 * The tree can be constructed from a set of data and then queried
 * for membership and nearest neighbors.
 */

#ifndef KDTREE_INCLUDED
#define KDTREE_INCLUDED

#include "Point.h"
#include "BoundedPQueue.h"
#include <stdexcept>
#include <cmath>
#include <assert.h>
#include <set>

// Again, "using namespace" in a header file is not conventionally a good idea,
// but we use it here so that you may use things like size_t without having to
// type std::size_t every time.
using namespace std;

template <size_t N, typename ElemType>
class KDTree {
public:
    /**
     * Constructor: KDTree();
     * Usage: KDTree<3, int> myTree;
     * ----------------------------------------------------
     * Constructs an empty KDTree.
     */
    KDTree();

    /**
     * Destructor: ~KDTree()
     * Usage: (implicit)
     * ----------------------------------------------------
     * Cleans up all resources used by the KDTree.
     */
    ~KDTree();

    /**
     * KDTree(const KDTree& rhs);
     * KDTree& operator=(const KDTree& rhs);
     * Usage: KDTree<3, int> one = two;
     * Usage: one = two;
     * -----------------------------------------------------
     * Deep-copies the contents of another KDTree into this
     * one.
     */
    KDTree(const KDTree& rhs);
    KDTree& operator=(const KDTree& rhs);

    /**
     * size_t dimension() const;
     * Usage: size_t dim = kd.dimension();
     * ----------------------------------------------------
     * Returns the dimension of the points stored in this
     * KDTree.
     */
    size_t dimension() const;

    /**
     * size_t size()  const;
     * bool empty() const;
     * Usage: if (kd.empty())
     * ----------------------------------------------------
     * Returns the number of elements in the kd-tree and
     * whether the tree is empty.
     */
    size_t size() const;
    bool empty() const;

    /**
     * bool contains(const Point<N>& pt) const;
     * Usage: if (kd.contains(pt)) { ... }
     * ----------------------------------------------------
     * Returns whether the specified point is contained in 
     * the KDTree.
     */
    bool contains(const Point<N>& pt) const;

    /**
     * void insert(const Point<N>& pt, const ElemType& value);
     * Usage: kd.insert(v, "This value is associated with v.");
     * ----------------------------------------------------
     * Inserts the point pt into the KDTree, associating it
     * with the specified value. If the element already existed
     * in the tree, the new value will overwrite the existing
     * one.
     */
    void insert(const Point<N>& pt, const ElemType& value);

    /**
     * ElemType& operator[](const Point<N>& pt);
     * Usage: kd[v] = "Some Value";
     * ----------------------------------------------------
     * Returns a reference to the value associated with point
     * pt in the KDTree. If the point does not exist, then
     * it is added to the KDTree using the default value of
     * ElemType as its key.
     */
    ElemType& operator[](const Point<N>& pt);

    /**
     * ElemType& at(const Point<N>& pt);
     * const ElemType& at(const Point<N>& pt) const;
     * Usage: cout << kd.at(v) << endl;
     * ----------------------------------------------------
     * Returns a reference to the key associated with the point
     * pt. If the point is not in the tree, this function throws
     * an out_of_range exception.
     */
    ElemType& at(const Point<N>& pt);
    const ElemType& at(const Point<N>& pt) const;

    /**
     * ElemType kNNValue(const Point<N>& key, size_t k) const
     * Usage: cout << kd.kNNValue(v, 3) << endl;
     * ----------------------------------------------------
     * Given a point v and an integer k, finds the k points
     * in the KDTree nearest to v and returns the most common
     * value associated with those points. In the event of
     * a tie, one of the most frequent value will be chosen.
     */
    ElemType kNNValue(const Point<N>& key, size_t k) const;

private:
  /******************************************
   *        Implementation details.         *
   ******************************************/
    struct Node {
        
        Point<N> key;
        ElemType value;
        size_t level;
        
        Node* rNodePtr;
        Node* lNodePtr;
    };
    
    Node* root;
    
    /* The number of elements currently stored */
    size_t numElements;
    
    /* Takes in a node and recursively delete's the subtree it represents
     going from its children up */
    void deleteNode(Node* currentNode);
    
    /* Helper function for copyinga KDTree */
    Node* copyTree(Node* rootNode);
    
    /* Recursive helper function for the KNNValue function */
    void KNNValueRecurse(const Point<N>&key, BoundedPQueue<Node*>& nearestPQ, Node* currentNode) const;
    
    /* A helper function that returns the most commonly occuring value
     stored in the Nodes of a Node* PQ */
    ElemType FindMostCommonValueInPQ(BoundedPQueue<Node*> nearestPQ) const;
    
};


/////////////////////////////////////////
// KDTree class implementation details //
/////////////////////////////////////////

/* 
 * Constructor 
 */
template <size_t N, typename ElemType>
KDTree<N, ElemType>::KDTree() {
    numElements = 0;
    root = NULL;
}

/* 
 * Destructor 
 */
template <size_t N, typename ElemType>
KDTree<N, ElemType>::~KDTree() {
    deleteNode(root);
    numElements = 0;
}
/* 
 * DeleteNode(currentNode)
 
 * Private helper function for deletion
 */
template <size_t N, typename ElemType>
void KDTree<N, ElemType>::deleteNode(Node* currentNode) {
    //Base Case
    if (currentNode == NULL) return;
    //Recursion
    deleteNode(currentNode->rNodePtr);
    deleteNode(currentNode->lNodePtr);
    delete currentNode;
}

/* 
 * Copy constructor 
 */
template <size_t N, typename ElemType>
KDTree<N, ElemType>::KDTree(const KDTree& rhs) {
    root = copyTree(rhs.root);
    numElements = rhs.numElements;
}

/*
 * KDTree myKDTree = other
 * Assignment operator. Clears old tree if not the same tree and then
 * copies the "other" tree into the new tree
 */
template <size_t N, typename ElemType>
KDTree<N, ElemType>& KDTree<N, ElemType>::operator=(const KDTree& rhs) {
    if (this != &rhs) {
        deleteNode(this->root);
        
        root = copyTree(rhs.root);
        numElements = rhs.numElements;
    }
    return *this;
}

/*
 * copyTree(rootNode)
 * Private function to copy one tree from another. It uses a recursive method
 * subsequently copying sub trees fro each node in the overall tree. It then
 * returns the root node of the copy.
 */
template <size_t N, typename ElemType>
typename KDTree<N, ElemType>::Node* KDTree<N, ElemType>::copyTree(Node* rootNode) {
    if(rootNode == NULL) {
        return NULL;
    }
    Node* rootNodeCopy = new Node;
    rootNodeCopy->key = rootNode->key;
    rootNodeCopy->value = rootNode->value;
    rootNodeCopy->level = rootNode->level;
    
    rootNodeCopy->rNodePtr = copyTree(rootNode->rNodePtr);
    rootNodeCopy->lNodePtr = copyTree(rootNode->lNodePtr);
    
    return rootNodeCopy;
}

/*
 * dimension()
 * returns the dimension of the KDTree
 */
template <size_t N, typename ElemType>
size_t KDTree<N, ElemType>::dimension() const {
  return N;
}

/* 
 * size() 
 * returns the number of elements in the KDTree 
 */
template <size_t N, typename ElemType>
size_t KDTree<N, ElemType>::size() const {
    return numElements;
}

/*  
 * empty() returns whether the tree has any elements 
 */
template <size_t N, typename ElemType>
bool KDTree<N, ElemType>::empty() const {
    return size() == 0;
}

/*
 * contains(pt)
 * Searches the KDTree for a specified point and returns true if
 * that point exists in the KDTree
 */
template<size_t N, typename ElemType>
bool KDTree<N, ElemType>::contains(const Point<N>& pt) const {
    Node* currentNode = root;
    while (currentNode != NULL) {
        if(currentNode->key == pt) return true;
        
        /*Compares the correct parts of the pts to determine which of a node's
        subtrees to look in next. */
        pt[currentNode->level % N] >= currentNode->key[currentNode->level % N] ? currentNode = currentNode->rNodePtr : currentNode = currentNode->lNodePtr;
    }
    return false;
}

/* 
 * The insert(pt, value) 
 * Looks up the correct place to enter the node and places it in the tree 
 */
template <size_t N, typename ElemType>
void KDTree<N, ElemType>::insert(const Point<N>& pt, const ElemType& value) {
    //Find the palace to insert the node
    Node* currentNode = root;
    Node* prevNode = NULL;
    size_t level = 0;
    while (currentNode != NULL) {
        ++level;
        //Edge Case: Duplicate Points
        if (pt == currentNode->key) {
            currentNode->value = value;
            return;
        }
        
        //Determine the level
        size_t keyIndex = currentNode->level % N;
        //Compare the appropriate key indicies
        if(pt[keyIndex] < currentNode->key[keyIndex]) {
            prevNode = currentNode;
            currentNode = currentNode->lNodePtr;
        } else if (pt[keyIndex] >= currentNode->key[keyIndex]) {
            prevNode = currentNode;
            currentNode = currentNode->rNodePtr;
        } else {
            //Should never get to this point in code
            cout << "Error inserting elements into binary tree" << endl;
            return;
        }
    }
    ++numElements;
    //Make the new node to insert into the KDTree
    Node* newNode = new Node;
    newNode->key = pt;
    newNode->value = value;
    newNode->level = level;
    newNode->rNodePtr = NULL;
    newNode->lNodePtr = NULL;
    
    if (currentNode == root) {
        root = newNode;
    } else {
        pt[prevNode->level % N] >= prevNode->key[prevNode->level % N] ? prevNode->rNodePtr = newNode : prevNode->lNodePtr = newNode;
    }
    
}

/*
 * operator[]
 * Returns a reference to the value associated with the Point key in the KDTree
 * If the key does not exist it is added to the KDTree using the ElemType default value.
 */
template<size_t N, typename Elemtype>
Elemtype& KDTree<N, Elemtype>::operator[](const Point<N>& pt) {
    Node* currentNode = root;
    Node* prevNode = NULL;
    size_t level = 0;
    while (currentNode != NULL) {
        ++level;
        //Edge Case: Duplicate Points
        if (pt == currentNode->key) {
            return currentNode->value;
        }
        
        //Determine the level
        size_t keyIndex = currentNode->level % N;
        //Compare the appropriate key indicies
        if(pt[keyIndex] < currentNode->key[keyIndex]) {
            prevNode = currentNode;
            currentNode = currentNode->lNodePtr;
        } else if (pt[keyIndex] >= currentNode->key[keyIndex]) {
            prevNode = currentNode;
            currentNode = currentNode->rNodePtr;
        } else {
            //Should never get to this point in code
            cout << "Error inserting elements into binary tree" << endl;
            return;
        }
    }
    ++numElements;
    //Make the new node to insert into the KDTree
    Node* newNode = new Node;
    newNode->key = pt;
    newNode->value = Elemtype();
    newNode->level = level;
    newNode->rNodePtr = NULL;
    newNode->lNodePtr = NULL;
    
    if (currentNode == root) {
        root = newNode;
        return newNode->value;
    } else {
        pt[prevNode->level % N] >= prevNode->key[prevNode->level % N] ? prevNode->rNodePtr = newNode : prevNode->lNodePtr = newNode;
        return newNode->value;
    }
}

/*
 * at(pt)
 */

template<size_t N, typename Elemtype>
Elemtype& KDTree<N, Elemtype>::at(const Point<N>& pt) {
    Node* currentNode = root;
    while (currentNode != NULL) {
        if(currentNode->key == pt) {
            return currentNode->value;
        }
        //Find the keyIndex
        size_t keyIndex = currentNode->level % N;
        //Compare the appropriate key indicies
        if(pt[keyIndex] < currentNode->key[keyIndex]) {
            currentNode = currentNode->lNodePtr;
        } else  {
            currentNode = currentNode->rNodePtr;
        }
    }
    throw out_of_range("That point does not exist");
}

/*
 * at(pt)
 * Returns a reference to the value associated with the point
 * pt. If the point isn't in the tree it throws an exception.
 */

template<size_t N, typename Elemtype>
const Elemtype& KDTree<N, Elemtype>::at(const Point<N>& pt) const {
    Node* currentNode = root;
    while (currentNode != NULL) {
        if(currentNode->key == pt) {
            return currentNode->value;
        }
        //Find the keyIndex
        size_t keyIndex = currentNode->level % N;
        //Compare the appropriate key indicies
        if(pt[keyIndex] < currentNode->key[keyIndex]) {
            currentNode = currentNode->lNodePtr;
        } else  {
            currentNode = currentNode->rNodePtr;
        }
    }
    throw out_of_range("That point does not exist");
}

/*
 * kNNValue(pt, integer)
 * Given a point and integer, KNNValue finds the k points
 * in the KDTree nearest to v and returns the most common
 * value associated with those points. In a tie, one of the
 * most frequent will be chosen.
 */
template<size_t N, typename ElemType>
ElemType KDTree<N, ElemType>::kNNValue(const Point<N>& key, size_t k) const {
    BoundedPQueue<Node*> nearestPQ(k);
    KNNValueRecurse(key, nearestPQ, root);
    
    return FindMostCommonValueInPQ(nearestPQ);
    
}
/*
 * kNNValueRecurse(pt, bpq, currentNode)
 * A recursive helper function which builds a bounded
 * priority queue of the points nearest to the entered point in 
 * the KDTree
 */
template<size_t N, typename ElemType>
void KDTree<N, ElemType>::KNNValueRecurse(const Point<N>&key, BoundedPQueue<Node*>& nearestPQ, Node* currentNode) const{
    //Base case
    if (currentNode == NULL) return;
    //Execution
    nearestPQ.enqueue(currentNode, Distance(currentNode->key, key));
    //Recursion
    size_t keyIndex = currentNode->level % N;
    if(key[keyIndex] < currentNode->key[keyIndex]) {
        KNNValueRecurse(key, nearestPQ, currentNode->lNodePtr);
        //If the hypersphere crosses the splitting plane check the other subtree
        if ( (nearestPQ.size() != nearestPQ.maxSize()) || fabs(currentNode->key[keyIndex] - key[keyIndex]) < nearestPQ.worst() ) {
            KNNValueRecurse(key, nearestPQ, currentNode->rNodePtr);
        }
    } else {
        KNNValueRecurse(key, nearestPQ, currentNode->rNodePtr);
        //If the hypersphere crosses the splitting plane check the other subtree
        if ( (nearestPQ.size() != nearestPQ.maxSize()) || fabs(currentNode->key[keyIndex] - key[keyIndex]) < nearestPQ.worst() ) {
            KNNValueRecurse(key, nearestPQ, currentNode->lNodePtr);
        }
    }
}

/*
 * FindMostCommonValueInPQ(bpq)
 * Takes in a bounded priority queue of Node*'s in the KDTree and
 * returns the most common value stored in the nodes.
 */
template<size_t N, typename ElemType>
ElemType KDTree<N, ElemType>::FindMostCommonValueInPQ(BoundedPQueue<Node*> nearestPQ) const{
    multiset<ElemType> values;
    while(!nearestPQ.empty()) {
        values.insert((nearestPQ.dequeueMin())->value);
    }
    
    ElemType best;
    size_t bestFrequency = 0;
    for(typename multiset<ElemType>::iterator it = values.begin(); it !=values.end(); ++it) {
        if (values.count(*it) > bestFrequency) {
            best = *it;
            bestFrequency = values.count(*it);
        }
    }
    return best;
}


#endif // KDTREE_INCLUDED
