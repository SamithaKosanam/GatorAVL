#include <iostream>
#include <string>
#include <vector>

using namespace std;

/* Note: 
	1. You will have to comment main() when unit testing your code because catch uses its own main().
	2. You will submit this main.cpp file on canvas. If you are using multiple header files, 
	   you will have to include them directly in main.cpp so we can test your code as a single file. 
*/


struct Node
{
public:
    string name;
    int ID;
    int balance;
    int lVal;
    int rVal;
    int val;
    Node* left;
    Node* right;

    Node(string nm, int id) : name(nm), ID(id), balance(0), lVal(0), val(0), rVal(0), left(nullptr), right(nullptr) {}
    ~Node() {
        delete left;
        delete right;
    }
};


class AVLTree
{
private:
    int size=0;
    int count=0;

    // ---- HELPER FUNCTIONS ----

    // insert node with given name and ID in correct spot
    Node* helpInsertNameID(Node* root, string name, int ID) {
        if (root == nullptr) {
            return new Node(name, ID);
        }
        if (root->ID < ID) {
            root->right = helpInsertNameID(root->right, name, ID);
        }
        else {
            root->left = helpInsertNameID(root->left, name, ID);
        }
        return root;
    }


    // print postorder traversal, keeping count to make sure there is no comma at the end
    void helpPrintInorder(const Node* root) {
        if (root != nullptr) {
            helpPrintInorder(root->left);
            count++;
            cout << root->name;
            if (count != size) {
                cout << ", ";
            }
            else {
                cout << endl;
            }
            helpPrintInorder(root->right);
        }
        // greatest number gets printed last, no comma!
    }

    // print preorder traversal, keeping count to make sure there is no comma at the end
    void helpPrintPreorder(const Node* root) {
        if (root != nullptr) {
            count++;
            cout << root->name;
            if (count != size) {
                cout << ", ";
            }
            else {
                cout << endl;
            }
            helpPrintPreorder(root->left);
            helpPrintPreorder(root->right);
        }
        // no comma after last Node is printed!
    }

    // print postorder traversal, keeping count to make sure there is no comma at the end
    void helpPrintPostOrder(const Node* root) {
        if (root != nullptr) {
            helpPrintPostOrder(root->left);
            helpPrintPostOrder(root->right);
            count++;
            cout << root->name;
            if (count != size) {
                cout << ", ";
            }
            else {
                cout << endl;
            }
        }
    }

    // handle removing a node and changing left/right values as necessary
    void remove(Node* root, Node* parent, bool isLeft) {
        size--;
        if (root->left==nullptr && root->right==nullptr) {
            // set parent of local root to null
            if (parent == nullptr) {
                root = nullptr;
                delete root;
            }
            else if (isLeft) {
                parent->left = nullptr;
                delete root;
            }
            else {
                parent->right = nullptr;
                delete root;
            }
        }
        else if (root->left==nullptr && root->right!=nullptr) {
            // set parent of local root to reference the child
            if (parent == nullptr) {
                root = nullptr;
                delete root;
            }
            else if (isLeft) {
                parent->left = root->right;
            }
            else {
                parent->right = root->right;
                delete root;
            }
        }
        else if (root->right==nullptr && root->left!=nullptr) {
            if (parent == nullptr) {
                root = nullptr;
                delete root;
            }
            else if (isLeft) {
                parent->left = root->left;
                delete root;
            }
            else {
                parent->right = root->left;
                delete root;
            }
        }
        else {
            if (root->right->left==nullptr) {
                // set parent of local root to reference left child
                Node* temp = root->right;
                root->ID = temp->ID;
                root->name = temp->name;
                root->right = root->right->right;
                delete temp;
            }
            else {
                // copy temp's data into local root's data, and remove temp by setting its parent to reference its right child (so make parent's left nullptr?)
                Node* temp = root->right->left;
                Node* par = root->right;
                while (temp->left != nullptr) {
                    par = par->left;
                    temp = temp->left;
                }
                root->ID = temp->ID;
                root->name = temp->name;
                par->left = nullptr;
                delete temp;
            }
        }
    }

    // remove Node with specified ID
    void removeID(Node* root, int ID, Node* parent=nullptr, bool isLeft=false) {
        if (ID < root->ID) {
            removeID(root->left, ID, root, true);
        }
        else if (ID > root->ID) {
            removeID(root->right, ID, root, false);
        }
        else {
            remove(root, parent, isLeft);
        }
    }

    // find node and count and call remove on it
    void helpRemoveInorderN(const int target, Node* root, Node* parent=nullptr, bool isLeft=false) {
        if (root != nullptr) {
            helpRemoveInorderN(target, root->left, root, true);
            if (count == target) {
                remove(root, parent, isLeft);
            }
            count++;
            helpRemoveInorderN(target, root->right, root, false);
        }
    }

    // check and update the balance factor for each node within the tree
    void checkBalance(Node* root) {
        if (root != nullptr) {
            if (root->left!= nullptr)
                checkBalance(root->left);
           if (root->right!= nullptr)
                checkBalance(root->right);
            root->lVal=0;
            root->rVal=0;
            root->val=0;
            root->balance=0;
            if (root->left != nullptr) {
                root->lVal += root->left->val+1;
            }
            if (root->right != nullptr) {
                root->rVal += root->right->val+1;
            }
            root->balance = root->lVal - root->rVal;
            if (root->balance < 0) {
                root->val = root->rVal;
            }
            else {
                root->val = root->lVal;
            }
        }
    }

    // check all nodes for rotation, AND if root will be changed in balance to update root
    Node* fCheck(Node* root) {
        Node* ret = root;
        check(root);
        if (root->balance==2 && root->left->balance==1) {
            ret = root->left;
            checkRotation(root);
        }
        else if (root->balance==-2 && root->right->balance==-1) {
            ret = root->right;
            checkRotation(root);
        }
        else if (root->balance==2&&root->left->balance==-1) {
            ret = root->left->right;
            checkRotation(root);
        }
        else if (root->balance==-2&&root->right->balance==1) {
            ret = root->right->left;
            checkRotation(root);
        }
        return ret;
    }

    // check if rotation is needed all non-root nodes
    void check(Node* root) {
        if (root!=nullptr) {
            check(root->left);
            check(root->right);
            checkRotation(root->right, root);
            checkRotation(root->left, root);
            checkBalance(root);
        }
    }

    // check if rotation is needed, and call appropriate functions depending on parent and child balance factors
    void checkRotation(Node* root, Node* parent=nullptr) {
        if (root!= nullptr) {
            if (root->left != nullptr) {
                if (root->balance == 2) {
                    if (root->left->balance == 1) {
                        rotateRight(root, parent);
                    }
                    else if (root->left->balance == -1) {
                        rotateLeft(root, parent, true);
                        rotateRight(root, parent);
                    }
                }
            }
            if (root->right != nullptr) {
                if (root->balance == -2) {
                    if (root->right->balance == -1) {
                        rotateLeft(root, parent);
                    }
                    else if (root->right->balance == 1) {
                        rotateRight(root, parent, true);
                        rotateLeft(root, parent);
                    }
                }
            }
        }
    }

    // function to handle left rotation
    void rotateLeft(Node* root, Node* parent=nullptr, bool isMore=false, bool isLeft=false) {
        if (parent!= nullptr) {
            if (parent->right != nullptr && parent->right->ID == root->ID) {
                isLeft = false;
            } else if (parent->left != nullptr && parent->left->ID == root->ID) {
                isLeft = true;
            }
        }

        // if it will be followed by a right rotation
        if (isMore) {
            Node *temp = root->left;
            Node *fin = root->left->right;
            fin->left = temp;
            root->left = fin;
            temp->right = nullptr;
        }

        // otherwise
        else {
            Node* temp = root->right;
            root->right = temp->left;
            temp->left = root;
            if (isLeft && parent!= nullptr) {
                parent->left = temp;
            }
            else if (!isLeft && parent!= nullptr) {
                parent->right = temp;
            }
        }
    }

    // function to handle right rotation
    void rotateRight(Node* root, Node* parent=nullptr, bool isMore=false, bool isLeft=false) {
        if (parent!= nullptr) {
            if (parent->right->ID == root->ID) {
                isLeft = false;
            } else if (parent->left->ID == root->ID) {
                isLeft = true;
            }
        }

        if (isMore) {
            Node *temp = root->right;
            Node *fin = root->right->left;
            fin->right = temp;
            root->right = fin;
            temp->left = nullptr;
        }

        else {
            Node *temp = root->left;
            root->left = temp->right;
            temp->right = root;
            if (isLeft && parent != nullptr) {
                parent->left = temp;
            } else if (!isLeft && parent != nullptr) {
                parent->right = temp;
            }
        }
    }


public:
    // add Node with specified name and ID w/ helper function, check/update balance on tree nodes, and update root
    Node* insertNameID(Node* root, string name, int ID) {
        size++;
        Node* ret = helpInsertNameID(root, name, ID);
        checkBalance(ret);
        return fCheck(ret);
    }


    // Traversal - print element names separated by commas
    void printInorder(const Node* root) {
        helpPrintInorder(root);
        count = 0;
    }

    void printPreOrder(const Node* root) {
        helpPrintPreorder(root);
        count = 0;
    }

    void printPostorder(const Node* root) {
        helpPrintPostOrder(root);
        count=0;
    }

    // print total height of tree (first level = 1)
    void printLevelCount(Node* root) {
        if (root == nullptr) {
            cout << "0";
        }
        else {
            cout << root->val+1 << endl;
        }
    }


    // print ID(s) associated with passed in name
    void searchName(Node* root, string name, vector<int>& ids) {
        if (root!= nullptr) {
            searchName(root->left, name, ids);
            if (root->name == name) {
                ids.push_back(root->ID);
            }
            searchName(root->right, name, ids);
        }
    }


    // print name associated with passed in ID
    bool searchID(Node* root, int ID, int show=true) {
        if (root!= nullptr) {
            if (ID==root->ID) {
                if (show) {
                    cout << root->name << endl;
                }
                return true;
            }
            else if (ID<root->ID) {
                return searchID(root->left,ID,show);
            }
            else if (ID>root->ID) {
                return searchID(root->right,ID,show);
            }
        }
        else {return false;}
    }

    // check if target node is root first - handle this and update the root if necessary
    Node* removeRt(Node* root, int ID) {
        Node* ret = root;
        if (root->ID==ID) {
            if (root->right== nullptr) {
                ret = root->left;
            }
            else if (root->right->left==nullptr) {
                ret = root->right;
                root->right->left = root->left;
            }
            else {
                ret = root->right->left;
                Node* par = root->right;
                while (ret->left != nullptr) {
                    par = par->left;
                    ret = ret->left;
                }
                ret->left = root->left;
                ret->right = root->right;
                par->left = nullptr;
            }
            size--;
            return ret;
        }
        removeID(root, ID);
        return ret;
    }


    // remove Nth element by inorder traversal
    void removeInorderN(int target, Node* root) {
        // when we get to the nth element, call remove(root, parent, isLeft);
        helpRemoveInorderN(target, root);
        count = 0;
    }

};


int main() {
    Node *root = nullptr;
    AVLTree tree;
    int numCommands = 0;
    string num;
    string fullCommand;
    string command;
    string name;
    string n;
    string ID;
    bool fail = false;


    // get number of commands
    getline(cin, num);
    numCommands = stoi(num);
    if (numCommands < 1 || numCommands > 10000) {
        cout << "unsuccessful";
        return 0;
    }

    // for previously entered number of commands, getline with full command - if line is valid execute and print successful, else do not execute and print unsuccessful
    for (int i=0; i<numCommands; i++) {
        getline(cin, fullCommand);

        // parse command from line
        command = fullCommand.substr(0, fullCommand.find(' ', 0));

        // search for entered name or ID if valid
        if (command == "search") {
            if (fullCommand.at(fullCommand.find(' ') + 1) == '"') {
                if (fullCommand.at(fullCommand.size() - 1) == '"') {
                    fullCommand = fullCommand.substr(fullCommand.find('"', 0) + 1,
                                                     fullCommand.find('"', fullCommand.find('"') + 1) -
                                                     fullCommand.find('"', 0) - 1);
                    for (int j = 0; j < fullCommand.size(); j++) {
                        if (isalpha(fullCommand.at(j))) {
                            name += fullCommand.at(j);
                        } else {
                            fail = true;
                            break;
                        }
                    }
                    if (!fail) {
                        vector<int> found;
                        tree.searchName(root, name, found);
                        if (found.size()==0) {
                            fail=true;
                        }
                        else {
                            for (int j=0; j<found.size(); j++) {
                                cout << found.at(j) << endl;
                            }
                        }
                    }
                }
                else {
                    fail = true;
                }
            }
            else if (isdigit(fullCommand.at(fullCommand.find(' ') + 1))) {
                ID = fullCommand.substr(fullCommand.find(' ') + 1);
                if (ID.size() == 8) {
                    for (int j = 0; j < 8; j++) {
                        if (!isdigit(ID.at(j))) {
                            fail = true;
                            break;
                        }
                    }
                    if (!fail) {
                        if (!tree.searchID(root, stoi(ID))) {
                            fail = true;
                        }
                    }
                }
                else {
                    fail = true;
                }
            }
            else {
                fail = true;
            }
        }

        // insert new node with given name and ID if valid
        else if (command == "insert") {
            name = fullCommand.substr(fullCommand.find('"')+1, fullCommand.find('"', 8)-8);
            for (int j = 0; j < name.size(); j++) {
                if (!isalpha(name.at(j)) && !isspace(name.at(j))) {
                    fail = true;
                    break;
                }
            }
            ID = fullCommand.substr(fullCommand.find('"', 8) + 2);
            if (ID.size() != 8) {
                fail = true;
            }
            else {
                for (int j = 0; j < 8; j++) {
                    if (!isdigit(ID.at(j))) {
                        fail = true;
                        break;
                    }
                }
            }
            if (!fail) {
                if (tree.searchID(root, stoi(ID),false)) {
                    fail = true;
                }
                else {
                    root = tree.insertNameID(root, name, stoi(ID));
                }
            }
        }

        // remove node with given ID if valid
        else if (command == "remove") {
            ID = fullCommand.substr(7);
            if (ID.size()!=8) {
                fail = true;
            }
            else {
                for (int j=0; j<8; j++) {
                    if (!isdigit(ID.at(j))) {
                        fail = true;
                        break;
                    }
                }
            }
            if (!fail) {
                root = tree.removeRt(root, stoi(ID));
            }
        }

        // removeInorder nth term if valid
        else if (command == "removeInorder") {
            n = fullCommand.substr(14);
            for (int j=0; j<n.size(); j++) {
                if (!isdigit(n.at(j))) {
                    fail = true;
                    break;
                }
            }
            if (!fail) {
                // root = removeNRt
                tree.removeInorderN(stoi(n),root);
            }
        }

        // perform appropriate traversal
        else if (command == "printInorder") {
            tree.printInorder(root);
        }
        else if (command == "printPreorder") {
            tree.printPreOrder(root);
        }
        else if (command == "printPostorder") {
            tree.printPostorder(root);
        }

        // print height of tree (level count)
        else if (command == "printLevelCount") {
            tree.printLevelCount(root);
        }

        // print unsucccessful if failed, or successful if not (and if necessary for command)
        if (fail) {
            cout << "unsuccessful" << endl;
            fail = false;
        }
        else if ((command == "insert" || command == "remove" || command == "removeInorder") && !fail){
            cout << "successful" << endl;
        }

        // clear all variables before getting next line/command
        name = "";
        ID = "";
        num = "";
        fail = false;
        n = "";
        fullCommand = "";
    }



	return 0;
}

