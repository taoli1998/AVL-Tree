#include<iostream>
#include<stdexcept>
#include<vector>
#include<algorithm>
using namespace std;

// AVL Node Clas
template <class K, class V>
class AVLTreeNode {
public:
	// Should have attributes named:
	AVLTreeNode* parent;    // parent - pointer to parent
	AVLTreeNode* left;      // left - pointer to left child
	AVLTreeNode* right;     // right - pointer to right child
	K key;                  // key - node's key
	V value;                // value - node's value
	unsigned int height;    // height - node's height

	// Constructors 
	AVLTreeNode<K,V>(K AVLkey, V AVLval);
};

//AVLTreeNode construction - public
template <class K, class V>
AVLTreeNode<K,V>::AVLTreeNode(K AVLkey, V AVLval){
	key = AVLkey;
	value = AVLval;
	parent = nullptr;
	left = nullptr;
	right = nullptr;
	height = 0;

}

// AVL Tree class
template <class K, class V>
class AVLTree
{
private:
	AVLTreeNode<K,V>* root;
	unsigned int NodeNum;
	void DeleteTree(AVLTreeNode<K,V>* node, unsigned int &NodeNum);
	int getBalance(AVLTreeNode<K,V>* node);
	AVLTreeNode<K,V>* RotateR(AVLTreeNode<K,V>* node);
	AVLTreeNode<K,V>* RotateL(AVLTreeNode<K,V>* node);
	AVLTreeNode<K,V>* CopyTree(AVLTreeNode<K,V>* rhs, AVLTreeNode<K,V>* parentNode);
	AVLTreeNode<K,V>* CopyKeys(AVLTreeNode<K,V>* node, vector<K> &k);
	AVLTreeNode<K,V>* SearchNode(const K k);
	void Rotation(AVLTreeNode<K,V>* node, K AVLkey);
	AVLTreeNode<K,V>* minkey(AVLTreeNode<K,V>* node);
	AVLTreeNode<K,V>* deleteNode(AVLTreeNode<K,V>* root, K key);
	void _inorder(AVLTreeNode<K,V>* node, vector<K> &vecK);
	void inorder(AVLTreeNode<K,V>* node, vector<K> &vecK);
	int height(AVLTreeNode<K,V>* node);
	void updateH(AVLTreeNode<K,V>* node);

public:
	AVLTree();
	AVLTree(const AVLTree<K, V>& tree);
	AVLTree& operator=(const AVLTree<K, V>& tree);
	~AVLTree();
	bool insert(K AVLkey, V AVLval);
	bool remove(K AVLkey);
	V search(K k);
	vector<K> keys();
	vector<V> values();
	unsigned int size();
	void* getRoot() const { return root; };

	//test to print the AVL Tree
	void printTree(AVLTreeNode<K, V>* root, int level);
	void printTree2();

};

//-----------------------------------------------------------------------helper function------------------------------------------------
//helper function, recursive delete tree node
template <class K, class V>
void AVLTree<K,V>::DeleteTree(AVLTreeNode<K,V>* node, unsigned int &NodeNum){
	if(node != nullptr){
		DeleteTree(node->left,NodeNum);
		DeleteTree(node->right,NodeNum);
		delete node;
		NodeNum--;
		node = nullptr;
	}
	
	
}

//find the minimum key
template <class K, class V>
AVLTreeNode<K,V>* AVLTree<K,V>::minkey(AVLTreeNode<K,V>* node) {
	AVLTreeNode<K,V>* current = node;
	while (current->left != NULL)
		current = current->left;
	return current;
}

//find and delete a node
template <class K, class V>
AVLTreeNode<K,V>* AVLTree<K,V>::deleteNode(AVLTreeNode<K,V>* root, K key) {
	if (root == NULL){
		return root;
	}
	if (key < root->key){
		root->left = deleteNode(root->left, key);
	}
	else if (key > root->key){
		root->right = deleteNode(root->right, key);
	}
	else {
	if ((root->left == NULL) || (root->right == NULL)) {
		AVLTreeNode<K,V>* temp = root->left ? root->left : root->right;
		if (temp == NULL) {
			temp = root;
			root = NULL;
		} else{
			*root = *temp;
			free(temp);
		}	
	} else {
		AVLTreeNode<K,V>* temp = minkey(root->right);
		root->key = temp->key;
		root->right = deleteNode(root->right, temp->key);
	}
	}

	if (root == NULL){
		return root;
	}
	
	root->height = height(root);
	int balanceFactor = getBalance(root);
	if (balanceFactor > 1) {
		if (getBalance(root->left) >= 0) {
			return RotateR(root);
		} 
		else {
			root->left = RotateL(root->left);
			return RotateR(root);
		}
	}
	if (balanceFactor < -1) {
		if (getBalance(root->right) <= 0) {
			return RotateL(root);
		} 
		else {
			root->right = RotateR(root->right);
			return RotateL(root);
		}
	}
	
	return root;
}

//helper function, get balance of the tree node
template <class K, class V>
int AVLTree<K,V>::getBalance(AVLTreeNode<K,V>* node){
	if(node == nullptr){
		return 0;
	}
	else{
		return (height(node->left) - height(node->right));
	}
}

//Deep code the entire tree, used to help copy constructor
template <class K, class V>
AVLTreeNode<K,V>* AVLTree<K,V>::CopyTree(AVLTreeNode<K,V>* rhs, AVLTreeNode<K,V>* parentNode){
	if(rhs){
		AVLTreeNode<K,V>* lhs =new AVLTreeNode<K,V>(rhs->key,rhs->value);
		lhs->parent = parentNode;
		lhs->height = rhs->height;
		lhs->left = CopyTree(rhs->left,lhs);
		lhs->right = CopyTree(rhs->right,lhs);
		return lhs;
	}
	else{
		return nullptr;
	}
}

//search node by key
template <class K, class V>
AVLTreeNode<K,V>* AVLTree<K,V>::SearchNode(const K k){
	AVLTreeNode<K,V>* cur = root;
	while(cur){
		if(cur->key>k){
			cur = cur->left;
		}
		else if(cur->key<k){
			cur = cur->right;
		}
		else if(cur->key == k){
			return cur;
		}
	}
	return cur;
}

//used to push keys in the vector
template <class K, class V>
AVLTreeNode<K,V>* AVLTree<K,V>::CopyKeys(AVLTreeNode<K,V>* node,  vector<K> &k){
	if(node){
		k.push_back(node->key);
		node->left = CopyKeys(node->left, k);
		node->right = CopyKeys(node->right, k);
	}
	return node;
}

//rotate right
template <class K, class V>
AVLTreeNode<K,V>* AVLTree<K,V>::RotateR(AVLTreeNode<K,V>* node){
	AVLTreeNode<K,V>* tempL = node->left;
	AVLTreeNode<K,V>* tempLR = tempL->right;
	node->left = tempLR;
	if (tempLR){
		tempLR->parent = node;
	}
	if(tempL){
		tempL->parent = node;
	}

	tempL->right = node;
	AVLTreeNode<K,V>* Gparent = node->parent;
	node->parent = tempL;
	tempL->parent = Gparent;

	if(Gparent == nullptr){
		root = tempL;
	}
	else{
		if(Gparent->left == node){
			Gparent->left = tempL;
		}
		else{
			Gparent->right = tempL;
		}
	}

	node->height = height(node);
	tempL->height = height(tempL);

	return tempL;
}

//rotate left
template <class K, class V>
AVLTreeNode<K,V>* AVLTree<K,V>::RotateL(AVLTreeNode<K,V>* node){
	AVLTreeNode<K,V>* tempR = node->right;
	AVLTreeNode<K,V>* tempRL = tempR->left;
	node->right = tempRL;
	if (tempRL){
		tempRL->parent = node;
	}

	if(tempR){
		tempR->parent = node;
	}
	tempR->left = node;
	AVLTreeNode<K,V>* Gparent = node->parent;
	node->parent = tempR;
	tempR->parent = Gparent;

	if(Gparent == nullptr){
		root = tempR;
	}
	else{
		if(Gparent->left == node){
			Gparent->left = tempR;
		}
		else{
			Gparent->right = tempR;
		}
	}

	node->height = height(node);
	tempR->height = height(tempR);

	return tempR;
}

//rotaion
template <class K, class V>
void AVLTree<K,V>::Rotation(AVLTreeNode<K,V>* node, K AVLkey){
	node->height = height(node);
	AVLTreeNode<K,V>* parent = node;
	parent->parent = node->parent;
	parent->key = node->key;
	while(parent != nullptr){
			int bf = height(parent->left)-height(parent->right);
			if(bf>1){
				if(AVLkey<parent->left->key){
					RotateR(parent);
					return ;
				}
				else if(AVLkey>parent->left->key){
					parent->left = RotateL(parent->left);
					RotateR(parent);
					return;
				}
			}
			if(bf<-1){
				if(AVLkey>parent->right->key){
					RotateL(parent);
					return ;
				}		
				else if(AVLkey<parent->right->key){
					parent->right = RotateR(parent->right);
					RotateL(parent);
					return;
				}
			}
			if(parent!=nullptr){
				parent = parent->parent;		
			}

		}

}

//find all keys in ascending order
template <class K, class V>
void AVLTree<K,V>::inorder(AVLTreeNode<K,V>* node, vector<K>& vecK){
	_inorder(node, vecK);
}

template <class K, class V>
void AVLTree<K,V>::_inorder(AVLTreeNode<K,V>* node, vector<K>& vecK){
	if(node){
		_inorder(node->left, vecK);
		vecK.push_back(node->key);
		_inorder(node->right, vecK);
	}
}

template<class K, class V>
int AVLTree<K,V>::height(AVLTreeNode<K,V>* node) //height helper function.
{
    if (node == nullptr)
    {
      return 0;
    } 
    else
    {
      int leftHeight = height(node->left); //node->left->height
      int rightHeight = height(node->right);
      if (leftHeight > rightHeight)
      {
        return(leftHeight + 1);
      }
      else {
        return(rightHeight + 1);
      }
    }
}

template<class K, class V>
void AVLTree<K,V>::updateH(AVLTreeNode<K,V>* node){
		if(node){
		updateH(node->left);
			node->height = height(node)-1;
		updateH(node->right);
	}
}

//---------------------------------------------------------------Public function------------------------------------------------------------------------------------
//default constructor
template <class K, class V>
AVLTree<K,V>::AVLTree(){
	root = nullptr;
	NodeNum = 0;
}

//copy constructor
template <class K, class V>
AVLTree<K,V>::AVLTree(const AVLTree<K,V>& tree){
	if(tree.root == nullptr){
		cout<<"The tree is empty!"<<endl;
	}
	else{
		root = CopyTree(tree.root, nullptr);
	}
}

//Overload assignment operator
template <class K, class V>
AVLTree<K,V>& AVLTree<K,V>:: operator=(const AVLTree<K, V>& tree){
	//if(this!= tree){
		DeleteTree(root,NodeNum);
		root = CopyTree(tree.root,nullptr);
	//}
	return*this;
}

//Desctructor
template <class K, class V>
AVLTree<K,V>::~AVLTree(){
	DeleteTree(root, NodeNum);
}

//Insertion
template <class K, class V>
bool AVLTree<K,V>::insert(K AVLkey, V AVLval){
	if (root==nullptr){
		root = new AVLTreeNode<K,V>(AVLkey, AVLval);
		root->key = AVLkey;
		root->value = AVLval;
		NodeNum++;
		return true;
	}
	else{
		AVLTreeNode<K,V>* cur = root;
		AVLTreeNode<K,V>* parent = nullptr;
		while(cur){
			parent = cur;
			if (AVLkey<cur->key){
				cur = cur->left;
			}
			else if(AVLkey>cur->key){
				cur = cur->right;
			}
			else{
				cout<<"Cannot have duplicate keys"<<endl;
				return false;
			}
		}
		cur = new AVLTreeNode<K,V>(AVLkey, AVLval);
		if (AVLkey < parent->key){
			parent->left = cur;
		}
		else{
			parent->right = cur;
		}
		cur->parent = parent;
		Rotation(cur, AVLkey);
		NodeNum++;	
		return true;		
	}
}

//remove function
template <class K, class V>
bool AVLTree<K,V>::remove(K AVLkey){
	
	AVLTreeNode<K,V>* temp = root;
	temp = SearchNode(AVLkey);
	if(temp == NULL){
		cout<<"key not found!"<<endl;
		return false;
	}
	
	AVLTreeNode<K,V>* cur = root;
	cur = deleteNode(cur,AVLkey);
	if (cur == NULL){
		return false;
	}
	else{
		NodeNum--;
		return true;
	}
	
}

//search key and return its value
template <class K, class V>
V AVLTree<K,V>::search(const K k){
	AVLTreeNode<K,V>* cur = root;

	while(cur){
		if(cur->key>k){
			cur = cur->left;
		}
		else if(cur->key<k){
			cur = cur->right;
		}
		else if(cur->key == k){
			return cur->value;
		}
	}
	throw runtime_error("The key does not match any key in the tree!");

}

//return all keys in ascending order
template <class K, class V>
vector<K> AVLTree<K,V>::keys(){
	vector<K> vecK;
	if(root == nullptr){
		return vecK;
	}
	else{
		AVLTreeNode<K,V>* cur = root;
		inorder(cur, vecK);
		sort(vecK.begin(), vecK.end());
		return vecK;
	}

}

//return the vector of the value in key order
template <class K, class V>
vector<V> AVLTree<K,V>::values(){
	vector<V> vecV;
	vector<K> vecK;
	if(root == nullptr){
		return vecV;
	}
	else{
		AVLTreeNode<K,V>* cur = root;
		inorder(cur, vecK);
		sort(vecK.begin(), vecK.end());
		for (long unsigned int i=0; i<vecK.size(); i++){
			K k = vecK[i];
			AVLTreeNode<K,V>* temp = root;
			temp = SearchNode(k);
			vecV.push_back(temp->value); 
		}
		return vecV;
	}

}

//return the size of the tree
template <class K, class V>
unsigned int AVLTree<K,V>::size(){
	return NodeNum;
}

//pirnt tree
template <class K, class V>
void AVLTree<K,V>::printTree(AVLTreeNode<K, V>* nodes, int level) //using recursion to print tree.
{
    if (nodes == NULL)
    {
      return;
    }
    if (level == 1)
    {
      std::cout << nodes->key << " ";
      if(nodes != root)
      {
          if(nodes->parent->key < nodes->key)
        {
          std::cout << "(" << nodes->parent->key << "->right)"<< "  ";
        }
        if(nodes->parent->key > nodes->key)
        {
          std::cout << "(" << nodes->parent->key << "->left)"<< "  ";
        }
      }
    }
    else if (level > 1)
    {
        printTree(nodes->left, level-1);
        printTree(nodes->right, level-1);
    }
}

template <class K, class V>
void AVLTree<K,V>::printTree2(){ //using recursion to print tree.
int height1 = height(root); //calling height to get total level tp put in for loop
//cout<<"root height "<<height1<<endl;
//cout<<root->height<<endl;
    for (int i = 1; i <= height1; i++)
    {
    printTree(root, i); //calling recursion to print out the whole tree.
    std::cout << std::endl; 
      }
  }


