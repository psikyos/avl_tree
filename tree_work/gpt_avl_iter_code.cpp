#include <iostream>
#include <stack>

// Structure for a tree node
struct Node {
    int key;
    Node* left;
    Node* right;
    int balanceFactor; // Incremental balance factor
};

// Function to create a new node with the given key
Node* createNode(int key) {
    Node* newNode = new Node();
    newNode->key = key;
    newNode->left = nullptr;
    newNode->right = nullptr;
    newNode->balanceFactor = 0; // New node is initially added at leaf level
    return newNode;
}

// Function to perform a right rotation
Node* rightRotate(Node* y) {
    Node* x = y->left;
    Node* T2 = x->right;

    // Perform rotation
    x->right = y;
    y->left = T2;

    // Update balance factors
    y->balanceFactor = y->balanceFactor - 1 - std::max(0, x->balanceFactor);
    x->balanceFactor = x->balanceFactor - 1 + std::min(0, y->balanceFactor);

    // Return the new root
    return x;
}

// Function to perform a left rotation
Node* leftRotate(Node* x) {
    Node* y = x->right;
    Node* T2 = y->left;

    // Perform rotation
    y->left = x;
    x->right = T2;

    // Update balance factors
    x->balanceFactor = x->balanceFactor + 1 - std::min(0, y->balanceFactor);
    y->balanceFactor = y->balanceFactor + 1 + std::max(0, x->balanceFactor);

    // Return the new root
    return y;
}

// Function to insert a node into the AVL tree
Node* insertNode(Node* root, int key) {
    Node* newNode = createNode(key);

    if (root == nullptr) {
        return newNode;
    }

    std::stack<Node*> nodeStack;
    Node* currNode = root;

    while (true) {
        if (key < currNode->key) {
            if (currNode->left == nullptr) {
                currNode->left = newNode;
                break;
            } else {
                nodeStack.push(currNode);
                currNode = currNode->left;
            }
        } else if (key > currNode->key) {
            if (currNode->right == nullptr) {
                currNode->right = newNode;
                break;
            } else {
                nodeStack.push(currNode);
                currNode = currNode->right;
            }
        } else {
            // Duplicate keys are not allowed in AVL tree
            delete newNode;
            return root;
        }
    }
	// nodeStack里为插入结点时经过的路径
    // Update balance factors and perform rotations if necessary
    while (!nodeStack.empty()) {
        Node* parent = nodeStack.top();
        nodeStack.pop();
	//此循环不断更新父结点上的bf
	//?应该是最小不平衡子树调整完毕,就可以跳出循环吧?
        if (key < parent->key) {
            parent->balanceFactor -= 1;
        } else {
            parent->balanceFactor += 1;
        }

        if (parent->balanceFactor > 1) {
            if (key < parent->left->key) {
                // Left Left Case
                parent = rightRotate(parent);
            } else {
                // Left Right Case
                parent->left = leftRotate(parent->left);
                parent = rightRotate(parent);
            }
        } else if (parent->balanceFactor < -1) {
            if (key > parent->right->key) {
                // Right Right Case
                parent = leftRotate(parent);
            } else {
                // Right Left Case
                parent->right = rightRotate(parent->right);
                parent = leftRotate(parent);
            }
        }

		//调整一次之后,最小不平衡子树已经处理完成,这里就应该终止

        // Update the parent reference after rotations if it has changed
        if (!nodeStack.empty()) {
            Node* grandParent = nodeStack.top();
            if (key < grandParent->key) {
                grandParent->left = parent;
            } else {
                grandParent->right = parent;
            }
        } else {
            // If the parent is the root node, update the root reference
            root = parent;
        }
    }

    return root;
}

// Function to print the AVL tree in in-order traversal
void inOrderTraversal(Node* root) {
    if (root == nullptr) {
        return;
    }
    inOrderTraversal(root->left);
    std::cout << root->key << " ";
    inOrderTraversal(root->right);
}

int main() {
    Node* root = nullptr;

    // Inserting nodes into the AVL tree
    root = insertNode(root, 10);
    root = insertNode(root, 20);
    root = insertNode(root, 30);
    root = insertNode(root, 40);
    root = insertNode(root, 50);
    root = insertNode(root, 25);

    // Print the AVL tree
    std::cout << "In-order traversal of the AVL tree: ";
    inOrderTraversal(root);
    std::cout << std::endl;

    return 0;
}
