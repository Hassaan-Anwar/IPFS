#pragma once
#include <sstream>
#include <iomanip>
#include <fstream>
#include<iostream>
#include<fstream>	
#include<queue>
#include"Sha1.h"
long long unsigned int identifier_space = 1;
int no_of_bits = 0;
SHA1 hashing;
using namespace std;
long long unsigned int convert_hash_to_int(string hash)
{
	bool below_4 = 0;
	int temp_no_bits;
	if (no_of_bits <= 3)
	{
		temp_no_bits = no_of_bits;
		no_of_bits = 4;
		below_4 = 1;
	}
	int i;
	string id = "";
	for (i = 0; hash[i] != '\0'; i++)
	{
	}

	int temp = 0;

	i = i - (no_of_bits / 4);
	for (i; hash[i] != '\0'; i++)
		id += hash[i];

	//cout << "Id extracted using number of bits: " << id << endl;
	i = 0;
	while (id[i] != '\0')
		i++;
	i--;

	long long unsigned int id_num = 0, tempo;
	int multi = 1;

	while (i >= 0)
	{
		if (id[i] > 58)
		{
			switch (id[i])
			{
			case 'a':
				tempo = 10;
				break;
			case 'b':
				tempo = 11;
				break;
			case 'c':
				tempo = 12;
				break;
			case 'd':
				tempo = 13;
				break;
			case 'e':
				tempo = 14;
				break;
			case 'f':
				tempo = 15;
				break;
			default:
				tempo = -1;
			}
			if (tempo == -1)
				cout << "Error";
		}
		else
		{
			tempo = id[i] - '0';
		}
		id_num += tempo * multi;
		multi *= 16;
		i--;
	}
	if (below_4)
	{
		no_of_bits = temp_no_bits;
		id_num = id_num % identifier_space;
		below_4 = 0;
	}
	return id_num;
}
class Node
{
public:
	long long unsigned int key;
	string path;
	string hash;
	Node* next;
	Node()
	{
		path = "";
		key = 0;
		next = nullptr;
	}
	Node(string path, long long unsigned int hash_id)
	{
		this->path = path;
		key = hash_id;
	}

	bool check_content(string s)
	{
		fstream file;
		file.open(path);
		if (!file)
			cout << "File not opened\n";
		else
		{
			string ans, str;
			while (getline(file, str))
			{
				ans += str;
				ans.push_back('\n');
			}
			if (ans == s)
				return 1;
		}
		return 0;
	}
	void display()
	{
		ifstream filea;
		filea.open(this->path);
		if (!filea)
			cout << "File Not opened\n" << key << endl;
		else
		{
			string ans, str;
			while (getline(filea, str))
			{
				ans += str;
				ans.push_back('\n');
			}
			cout << "\n-------------------------------------\n";
			cout << "ID: " << key << "\nValue: " << ans;
			cout << "\n-------------------------------------\n";
		}
		filea.close();
	}
};

class BTree_Node
{
public:
	Node** datas;
	int min_degree;
	BTree_Node** children;
	int no_of_keys;
	bool leaf;
	BTree_Node(bool is_leaf = 0)
	{
		min_degree = 2;
		datas = new Node * [3];
		children = new BTree_Node * [4] {0};
		no_of_keys = 0;
		leaf = is_leaf;
	}
	void pre_order_traverse(BTree_Node* node)
	{
		if (node == 0)
			return;
		for (int i = 0; i < node->no_of_keys; i++)
		{
			pre_order_traverse(node->children[i]);
			node->datas[i]->display();
			pre_order_traverse(node->children[i + 1]);
		}
	}
	void insert_key(Node* dat)
	{
		int index = no_of_keys - 1;
		if (leaf)
		{
			while (index >= 0 && datas[index]->key > dat->key) {
				datas[index + 1] = datas[index];
				index--;
			}
			if (datas[index]->key == dat->key)
			{

			}
			datas[index + 1] = dat;
			no_of_keys++;
		}
		else
		{
			while (index >= 0 && datas[index]->key > dat->key)
				index--;
			if (children[index + 1]->no_of_keys == 3)
			{
				split_children(index + 1, children[index + 1]);
				if (datas[index + 1]->key < dat->key)
					index++;
			}
			children[index + 1]->insert_key(dat);

		}
	}
	void split_children(int index, BTree_Node* node)
	{
		BTree_Node* new_child = new BTree_Node(node->leaf);
		new_child->no_of_keys = 1;
		new_child->datas[0] = node->datas[2];
		if (!node->leaf)
		{
			new_child->children[0] = node->children[2];
			new_child->children[1] = node->children[3];
		}
		node->no_of_keys = 1;
		for (int i = no_of_keys; i >= index + 1; i--)
		{
			children[i + 1] = children[i];
		}
		children[index + 1] = new_child;
		for (int i = no_of_keys - 1; i >= index; i--)
			datas[i + 1] = datas[i];
		datas[index] = node->datas[1];
		no_of_keys++;

	}
};


class BTree
{
public:
	BTree_Node* root;
	int min_degree;
	queue<Node*> input_queue;
	BTree()
	{
		root = nullptr;
		min_degree = 3;
	}
	void display()
	{
		if (root != nullptr)
			root->pre_order_traverse(root);
		else
			cout << "Tree is Empty\n";
	}
	void level_order()
	{
		if (root == nullptr)
		{
			cout << "Tree is Empty\n";
			return;
		}
		queue<BTree_Node*> q;
		q.push(root);
		BTree_Node* curr;
		while (!q.empty())
		{
			int level = q.size();
			//cout << level << endl;
			for (int j = 0; j < level; j++)
			{
				curr = q.front();
				q.pop();
				for (int i = 0; i < curr->no_of_keys; i++)
					cout << "|" << curr->datas[i]->key << "|";
				cout << '\t';
				for (int i = 0; i < 4; i++)
				{
					if (curr->children[i] != 0)
						q.push(curr->children[i]);
				}
			}
			cout << endl;
		}
	}
	Node* search(int key)
	{
		BTree_Node* curr = root;
		bool found = 0;
		int index = 0;
		while (curr != NULL && !found)
		{
			int i = 0;
			while (i < curr->no_of_keys && key>curr->datas[i]->key)
				i++;
			if (i < curr->no_of_keys && key == curr->datas[i]->key)
			{

				found = 1;
				index = i;
				break;
			}
			if (curr->leaf)
				break;
			curr = curr->children[i];
		}
		if (found)
			return curr->datas[index];
		else
			return nullptr;
	}
	void insert(Node* n)
	{
		input_queue.push(n);
		if (!root)
		{
			root = new BTree_Node(true);
			root->datas[0] = n;
			root->no_of_keys = 1;
		}
		else
		{
			if (root->no_of_keys == 3)
			{
				BTree_Node* new_node = new BTree_Node();
				new_node->children[0] = root;
				new_node->split_children(0, root);

				int i = 0;
				if (new_node->datas[0]->key < n->key)
					i++;
				new_node->children[i]->insert_key(n);

				root = new_node;
			}
			else
				root->insert_key(n);
		}
	}
};