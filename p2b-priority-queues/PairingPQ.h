// Project identifier: 43DE0E0C4C76BFAA6D8C2F5AEAE0518A9C42CF4E

#ifndef PAIRINGPQ_H
#define PAIRINGPQ_H

#include "Eecs281PQ.h"
#include <deque>
#include <utility>

// A specialized version of the priority queue ADT implemented as a pairing
// heap.
template<typename TYPE, typename COMP_FUNCTOR = std::less<TYPE>>
class PairingPQ : public Eecs281PQ<TYPE, COMP_FUNCTOR> {
    // This is a way to refer to the base class object.
    using BaseClass = Eecs281PQ<TYPE, COMP_FUNCTOR>;

public:
    // Each node within the pairing heap
    class Node {
        public:
            // TODO: After you add add one extra pointer (see below), be sure
            // to initialize it here.
            explicit Node(const TYPE &val)
                : elt{ val }, child{ nullptr }, sibling{ nullptr }, parent{ nullptr }
            {}

            // Description: Allows access to the element at that Node's
            // position.  There are two versions, getElt() and a dereference
            // operator, use whichever one seems more natural to you.
            // Runtime: O(1) - this has been provided for you.
            const TYPE &getElt() const { return elt; }
            const TYPE &operator*() const { return elt; }

            // The following line allows you to access any private data
            // members of this Node class from within the PairingPQ class.
            // (ie: myNode.elt is a legal statement in PairingPQ's add_node()
            // function).
            friend PairingPQ;

        private:
            TYPE elt;
            Node *child;
            Node *sibling;
            // TODO: Add one extra pointer (parent or previous) as desired.
            Node *parent;
    }; // Node


    // Description: Construct an empty pairing heap with an optional
    //              comparison functor.
    // Runtime: O(1)
    explicit PairingPQ(COMP_FUNCTOR comp = COMP_FUNCTOR()) :
        BaseClass{ comp }, numNodes{ 0 }, root{ nullptr } {
        // TODO: Implement this function.
    } // PairingPQ()


    // Description: Construct a pairing heap out of an iterator range with an
    //              optional comparison functor.
    // Runtime: O(n) where n is number of elements in range.
    template<typename InputIterator>
    PairingPQ(InputIterator start, InputIterator end, COMP_FUNCTOR comp = COMP_FUNCTOR()) :
        BaseClass{ comp }, numNodes{ 0 }, root{ nullptr } {
        // TODO: Implement this function.
        while (start != end) {
            push(*start);
            ++start;
        }
    } // PairingPQ()


    // Description: Copy constructor.
    // Runtime: O(n)
    PairingPQ(const PairingPQ &other) :
        BaseClass{ other.compare }, numNodes{ 0 }, root{ nullptr } {
        // TODO: Implement this function.
        std::deque<Node*> dq;
        if (other.root) {
            dq.push_back(other.root);
            while (!dq.empty()) {
                Node *temp = dq.front();
                dq.pop_back();
                if (temp->child != nullptr) {
                    dq.push_back(temp->child);
                }
                if (temp->sibling != nullptr) {
                    dq.push_back(temp->sibling);
                }
                push(temp->getElt());
            }
        }
        // NOTE: The structure does not have to be identical to the original,
        //       but it must still be a valid pairing heap.
    } // PairingPQ()


    // Description: Copy assignment operator.
    // Runtime: O(n)
    PairingPQ &operator=(const PairingPQ &rhs) {
        // TODO: Implement this function.
        // HINT: Use the copy-swap method from the "Arrays and Containers"
        // lecture.
        PairingPQ temp(rhs);
        std::swap(root, temp.root);
        std::swap(numNodes, temp.numNodes);  // Delete this line when you implement this function
        return *this;
    } // operator=()


    // Description: Destructor
    // Runtime: O(n)
    ~PairingPQ() {
        // TODO: Implement this function.
        std::deque<Node*> dq;
        if (root) {
            dq.push_back(root);
            while (!dq.empty()) {
                Node *temp = dq.front();
                if (temp->child != nullptr) {
                    dq.push_back(temp->child);
                }
                if (temp->sibling != nullptr) {
                    dq.push_back(temp->sibling);
                }
                numNodes--;
                delete temp;
                dq.pop_front();
            }
        }
    } // ~PairingPQ()


    // Description: Assumes that all elements inside the pairing heap are out
    //              of order and 'rebuilds' the pairing heap by fixing the
    //              pairing heap invariant.  You CANNOT delete 'old' nodes
    //              and create new ones!
    // Runtime: O(n)
    virtual void updatePriorities() {
        // TODO: Implement this function.
        std::deque<Node*> dq;
        Node* temp = root;
        if (temp->child != nullptr)
            dq.push_back(temp->child);
        temp->child = nullptr;
        
        while (!dq.empty()) {

            temp = dq.front();
            if (temp->sibling != nullptr)
                dq.push_back(temp->sibling);
            if (temp->child != nullptr)
                dq.push_back(temp->child);

            temp->child = nullptr;
            temp->sibling = nullptr;
            temp->parent = nullptr;

            root = meld(root, temp);

            dq.pop_front();
        }
    } // updatePriorities()


    // Description: Add a new element to the pairing heap. This is already
    //              done. You should implement push functionality entirely
    //              in the addNode() function, and this function calls
    //              addNode().
    // Runtime: O(1)
    virtual void push(const TYPE &val) {
        addNode(val);
    } // push()


    // Description: Remove the most extreme (defined by 'compare') element
    //              from the pairing heap.
    // Note: We will not run tests on your code that would require it to pop
    // an element when the pairing heap is empty. Though you are welcome to
    // if you are familiar with them, you do not need to use exceptions in
    // this project.
    // Runtime: Amortized O(log(n))
    virtual void pop() {
        // TODO: Implement this function.
        std::deque<Node*> dq;
        Node* temp = root->child;
        
        if (temp != nullptr)
        {
            while (temp != nullptr) {
                dq.push_back(temp);
                temp = temp->sibling;
            }

            while (dq.size() > 1) {

                temp = dq.front();
                temp->sibling = nullptr;
                temp->parent = nullptr;
                dq.pop_front();

                dq.front()->sibling = nullptr;
                dq.front()->parent = nullptr;

                dq.push_back(meld(temp, dq.front()));
                dq.pop_front();
            }

            delete root;
            numNodes--;
            root = dq.front();
            dq.pop_front();
        }
        else {
            numNodes--;
            delete root;
            root = nullptr;
        }
    } // pop()


    // Description: Return the most extreme (defined by 'compare') element of
    //              the pairing heap. This should be a reference for speed.
    //              It MUST be const because we cannot allow it to be
    //              modified, as that might make it no longer be the most
    //              extreme element.
    // Runtime: O(1)
    virtual const TYPE &top() const {
        // TODO: Implement this function

        // These lines are present only so that this provided file compiles.
        return root->elt;      // TODO: Delete or change this line
    } // top()


    // Description: Get the number of elements in the pairing heap.
    // Runtime: O(1)
    virtual std::size_t size() const {
        // TODO: Implement this function
        return numNodes; // TODO: Delete or change this line
    } // size()

    // Description: Return true if the pairing heap is empty.
    // Runtime: O(1)
    virtual bool empty() const {
        // TODO: Implement this function
        return numNodes == 0; // TODO: Delete or change this line
    } // empty()


    // Description: Updates the priority of an element already in the pairing
    //              heap by replacing the element refered to by the Node with
    //              new_value.  Must maintain pairing heap invariants.
    //
    // PRECONDITION: The new priority, given by 'new_value' must be more
    //              extreme (as defined by comp) than the old priority.
    //
    // Runtime: As discussed in reading material.
    void updateElt(Node* node, const TYPE &new_value) {
        // TODO: Implement this function
        node->elt = new_value;

        if (node->parent != nullptr) {
            if (this->compare(node->parent->elt, node->elt)) {
                Node* left = node->parent->child;

                if (left == node) {
                    node->parent->child = node->sibling;
                }

                else {
                    while (left->sibling != node) {
                        left = left->sibling;
                    }
                    left->sibling = node->sibling;
                }

                node->parent = nullptr;
                node->sibling = nullptr;

                root = meld(root, node);
            }
        }
    } // updateElt()


    // Description: Add a new element to the pairing heap. Returns a Node*
    //              corresponding to the newly added element.
    // Runtime: O(1)
    // NOTE: Whenever you create a node, and thus return a Node *, you must
    //       be sure to never move or copy/delete that node in the future,
    //       until it is eliminated by the user calling pop(). Remember this
    //       when you implement updateElt() and updatePriorities().
    Node* addNode(const TYPE &val) {
        // TODO: Implement this function
        Node *newNode = new Node(val);
        numNodes++;
        if (root == nullptr) {
            root = newNode;
        }
        else {
            this->root = meld(newNode, root);
        }
        return newNode; // TODO: Delete or change this line
    } // addNode()


private:
    // TODO: Add any additional member variables or member functions you
    // require here.
    size_t numNodes;
    Node *root;
    // TODO: We recommend creating a 'meld' function (see the Pairing Heap
    // papers).
    Node *meld(Node *pa, Node *pb) {
        if (pa == nullptr) {
            return pb;
        }
        else if (pb == nullptr) {
            return pa;
        }

        if (this->compare(pa->getElt(), pb->getElt())) {
            pa->sibling = pb->child;
            pb->child = pa;
            pa->parent = pb;
            return pb;
        }
        else {
            pb->sibling = pa->child;
            pa->child = pb;
            pb->parent = pa;
            return pa;
        }
    }

    // NOTE: For member variables, you are only allowed to add a "root
    //       pointer" and a "count" of the number of nodes. Anything else
    //       (such as a deque) should be declared inside of member functions
    //       as needed.
};


#endif // PAIRINGPQ_H
