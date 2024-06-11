#pragma once
#include"BTree.h"
#include<Windows.h>
#include<filesystem>
#include<cstdio>
#include<direct.h>
string directory = "Folders/";
string get_file_name(string path)
{
	int index = 0;
	for (index = 0; path[index] != '\0'; index++) {}
	index--;
	while (path[index] != '\\' && path[index] != '/')
		index--;
	index++;
	string ans = "";
	for (index; path[index] != '\0'; index++)
		ans += path[index];
	return ans;
}
string get_file_data(string path)
{
	fstream file;
	file.open(path);
	string ans = "-";
	if (!file)
		cout << "File Not opened (get_file_data function)\n";
	else
	{
		ans = "";
		string str;
		while (getline(file, str))
		{
			ans += str;
			ans.push_back('\n');
		}
		ans.push_back('\0');
	}
	file.close();
	return ans;

}


class Machine
{
public:
	long long unsigned int id;
	Machine** routing_table;
	BTree* data;
	Machine* next;
	string name;
	string hash_id;
	string path;
	string folder_path;
	Machine(string name_machine)
	{
		cout << "Generating Hash ID.....\n";
		name = name_machine;
		hashing.update(name_machine);
		hash_id = hashing.final();
		path = directory;
		folder_path = directory;
		path += hash_id;
		folder_path = path;
		cout << "Creating Machine Folder...\n";
		wstring temp(path.begin(), path.end());
		if (CreateDirectory(temp.c_str(), NULL))
			cout << "Folder Created Successfully\n";
		else
			cout << "Error in Folder Creation...\n";
		path.push_back('/');
		cout << "Path of Machines Folder: " << path << endl;
		id = convert_hash_to_int(hash_id);
		data = new BTree;
		next = nullptr;
		routing_table = new Machine * [no_of_bits] {0};
		cout << "Machine Created!\n\n";
	}
	void display_routing_table()
	{
		cout << "Routing Table of " << name;
		cout << "\n-------------------------------------\n";
		for (int i = 0; i < no_of_bits; i++)
		{
			if (routing_table[i] != 0)
				cout << i + 1 << ". Name: " << routing_table[i]->name << "\tID: " << routing_table[i]->id << endl;
		}
		cout << "-------------------------------------\n";
	}
	void insert_data(string file_name, string file_data, long long unsigned int data_key)
	{
		cout << "Creating Copy of file in machine's folder....\n";
		string temp = path;
		temp += file_name;
		ofstream file(temp);
		if (!file)
		{
			cout << "File Not Created\n";
			cout << temp;
			return;
		}
		file << file_data;
		file.close();
		Node* insertion = new Node(temp, data_key);
		data->insert(insertion);
		cout << "Data Inserted\n";
		data->level_order();
	}
	Node* delete_data(long long unsigned int data_key)
	{
		BTree* temp = new BTree;
		queue<Node*> queue = data->input_queue;
		Node* item = NULL;
		while (!queue.empty())
		{
			Node* tempo = queue.front();
			queue.pop();
			if (tempo->key == data_key)
			{
				item = tempo;
				continue;
			}
			temp->insert(tempo);
		}
		delete data;
		data = temp;
		return item;
	}
	void search_data(long long unsigned int data_key)
	{
		Node* ans = data->search(data_key);
		if (ans == nullptr)
			cout << "Data Not Found\n\n";
		else
		{
			cout << "Data Found\n\n";
			ans->display();
		}
	}
	bool is_data(long long unsigned int data_key)
	{
		Node* ans = data->search(data_key);
		if (ans == nullptr)
			return 0;
		return 1;
	}

	~Machine()
	{
		cout << "\n-------------------------------------\n";
		cout << "\nDeleting Machine files...\n";
		queue<Node*> queue = data->input_queue;
		Node* temp;
		while (!queue.empty())
		{
			temp = queue.front();
			queue.pop();
			if (!(::remove(temp->path.c_str())))
				cout << "File Removed Succesfully\n";
			else
				cout << "Error in removing file\n";
		}
		cout << "\nDeleting Machine folder...\n";
		if (!_rmdir(folder_path.c_str()))
			cout << "Folder deleted sucessfully\n";
		else
			cout << "Error in deleting file\n";
		cout << "\n-------------------------------------\n";
	}
};

class RingDht
{
	Machine* root;
public:
	int machine_no;
	RingDht()
	{
		machine_no = 0;
		root = nullptr;
	}
	void insert(string name)
	{
		machine_no++;
		Machine* temp = new Machine(name);
		cout << "Inserting Machine into the Ring DHT....\n";
		if (!root)
		{
			root = temp;
			root->next = root;
		}
		else if (root->next == root)
		{
			if (temp->id > root->id)
			{
				root->next = temp;
				temp->next = root;
			}
			else
			{
				temp->next = root;
				root->next = temp;
				root = temp;
			}
		}
		else
		{
			Machine* curr = root->next;
			Machine* prev = root;
			if (root->id > temp->id)
			{
				temp->next = root;
				while (curr->next != root)
					curr = curr->next;
				curr->next = temp;
				root = temp;
				return;
			}
			while (curr != root && curr->id < temp->id)
			{
				prev = curr;
				curr = curr->next;
			}
			prev->next = temp;
			temp->next = curr;
		}
		cout << "Machine Insetered Sucessfully\n";
	}
	void update_routing_tables()
	{
		long long unsigned int num;
		Machine* curr = root;
		cout << "\n-------------------------------------\n";
		cout << "Updating Routing Table....\n";
		do
		{
			for (int i = 0; i < no_of_bits; i++)
			{
				num = curr->id + pow(2, i);
				num = num % identifier_space;
				//cout << "Number generated: " << num << endl;
				curr->routing_table[i] = this->get_machine(num);
				//if (curr->routing_table[i] != 0)
					//cout << "Stored in machine " << curr->routing_table[i]->id << " with Name " << curr->routing_table[i]->name << endl;
			}
			curr = curr->next;
		} while (curr != root);
		cout << "\nRouting Table Updated";
		cout << "\n-------------------------------------\n";
	}
	Machine* get_machine(long long unsigned int id)
	{
		if (root == nullptr)
		{
			return NULL;
		}
		Machine* curr = root;
		while (id > curr->id)
		{
			curr = curr->next;
			if (curr == root)
				break;
		}

		return curr;
	}

	bool find(string name)
	{
		if (root == NULL)
			return 0;
		hashing.update(name);
		string h = hashing.final();
		long long unsigned int id = convert_hash_to_int(h);
		Machine* curr = root;
		bool found = 0;
		do
		{
			if (curr->id == id)
			{
				found = 1;
				break;
			}
			curr = curr->next;
		} while (curr != root);
		return found;
	}
	Machine* get_machine(string name)
	{
		if (root == NULL)
			return 0;
		hashing.update(name);
		string h = hashing.final();
		Machine* curr = root;
		bool found = 0;
		do
		{
			if (curr->hash_id == h)
			{
				found = 1;
				break;
			}
			curr = curr->next;
		} while (curr != root);
		if (found)
			return curr;
		else
			return 0;
	}
	Machine* get_previous(string name)
	{
		Machine* curr = root;
		Machine* prev = root;
		hashing.update(name);
		bool found = 0;
		string h = hashing.final();
		do
		{
			if (curr->hash_id == h)
			{
				found = 1;
				break;
			}
			prev = curr;
			curr = curr->next;
		} while (curr != root);
		if (found)
		{
			if (curr == root)
			{
				while (prev->next != root)
					prev = prev->next;
			}
			return prev;
		}
	}
	void display(int type)
	{
		Machine* curr = root;
		cout << "\nStarting Display\n-------------------------------------\n";
		do
		{
			cout << "Name: " << curr->name << "\tID: " << curr->id << '\n';
			if (type == 1)
				curr->display_routing_table();
			cout << "\n-------------------------------------\n";
			curr = curr->next;
		} while (curr != root);
	}

	void insert_data(string file_name, string file_data, string machine_name)
	{
		cout << "\n-------------------------------------\n";
		hashing.update(file_data);
		string hash = hashing.final();
		long long unsigned int id = convert_hash_to_int(hash);
		id = id % identifier_space;
		cout << "ID Generated: " << id;
		cout << "\n-------------------------------------\n";
		Machine* curr = get_machine(machine_name);
		Machine* prev = get_previous(machine_name);
		bool inserted = 0;
		while (!inserted)
		{
			//cout << "Comparing " << prev->id << " with " << id << " and " << curr->id << endl;
			if (curr == root && id <= curr->id)
			{
				cout << "ID is in range of current Machine\n";
				curr->insert_data(file_name, file_data, id);
				cout << "Inserted in current Machine\n";
				return;
			}
			else if (id > prev->id && id <= curr->id && curr != root)
			{
				cout << "ID is in range of current Machine\n";
				curr->insert_data(file_name, file_data, id);
				cout << "Inserted in current Machine\n";
				return;
			}
			else if (id > prev->id && curr == root)
			{
				cout << "ID is in range of current Machine\n";
				curr->insert_data(file_name, file_data, id);
				cout << "Inserted in current Machine\n";
				return;
			}
			else
			{
				bool transfered = 0;
				cout << "ID not in range of current Machine (ID: " << curr->id << " ) \nLooking at current Machine's Routing Table...\n";
				//	cout << "\nComparing " << id << " with " << curr->routing_table[0]->id << " \n";
				if (id <= curr->routing_table[0]->id || curr->routing_table[0] == root)
				{
					curr = curr->routing_table[0];
					transfered = 1;
				}
				else
				{
					for (int i = 0; i < no_of_bits - 1; i++)
					{
						if (id > curr->routing_table[i]->id && id <= curr->routing_table[i + 1]->id)
						{
							curr = curr->routing_table[i];
							transfered = 1;
							break;
						}
					}
					if (!transfered)
					{
						//cout << "Last Index\n";
						curr = curr->routing_table[no_of_bits - 1];
						transfered = 1;
					}
				}
				if (transfered)
				{
					prev = get_previous(curr->name);
					cout << "Transfered Query to: " << curr->name << "\tID: " << curr->id << endl;
					cout << "\n-------------------------------------\n";

				}
			}
		}

	}
	void delete_data(long long unsigned int id, string machine_name)
	{
		cout << "\n-------------------------------------\n";
		Machine* curr = get_machine(machine_name);
		Machine* prev = get_previous(machine_name);
		bool deleted = 0;
		Node* item;
		while (!deleted)
		{
			//cout << "Comparing " << prev->id << " with " << id << " and " << curr->id << endl;
			if (curr == root && id <= curr->id)
			{
				cout << "ID is in range of current Machine\n";
				Node* temp = curr->data->search(id);
				if (temp == nullptr)
					cout << "Item is not in BTree already\n";
				else
				{
					cout << "Found Item\nDeleting Item...\n\n";
					item = curr->delete_data(temp->key);
					cout << "Item has been deleted\n\n";
					if (item != NULL)
					{
						if (!(::remove(item->path.c_str())))
							cout << "File Removed Succesfully\n\n";
						else
							cout << "Error in removing file\n";
					}
					delete item;
					curr->data->level_order();
				}
				return;
			}
			else if (id > prev->id && id <= curr->id && curr != root)
			{
				cout << "ID is in range of current Machine\n";
				Node* temp = curr->data->search(id);
				if (temp == nullptr)
					cout << "Item is not in BTree already\n";
				else
				{
					cout << "Found Item\nDeleting Item...\n\n";
					item = curr->delete_data(temp->key);
					cout << "Item has been deleted\n\n";
					if (item != NULL)
					{
						if (!(::remove(item->path.c_str())))
							cout << "File Removed Succesfully\n\n";
						else
							cout << "Error in removing file\n";
						delete item;
					}
					curr->data->level_order();
				}
				return;
			}
			else if (id > prev->id && curr == root)
			{
				cout << "ID is in range of current Machine\n";
				Node* temp = curr->data->search(id);
				if (temp == nullptr)
					cout << "Item is not in BTree already\n";
				else
				{
					cout << "Found Item\nDeleting Item...\n\n";
					item = curr->delete_data(temp->key);
					cout << "Item has been deleted\n\n";
					if (item != NULL)
					{
						if (!(::remove(item->path.c_str())))
							cout << "File Removed Succesfully\n\n";
						else
							cout << "Error in removing file\n";
					}
					delete item;
					curr->data->level_order();
				}
				return;
			}
			else
			{
				bool transfered = 0;
				cout << "ID not in range of current Machine (ID: " << curr->id << " ) \nLooking at current Machine's Routing Table...\n";
				if (id <= curr->routing_table[0]->id || curr->routing_table[0] == root)
				{
					curr = curr->routing_table[0];
					transfered = 1;
				}
				else
				{
					for (int i = 0; i < no_of_bits - 1; i++)
					{
						if (id > curr->routing_table[i]->id && id <= curr->routing_table[i + 1]->id)
						{
							curr = curr->routing_table[i];
							transfered = 1;
							break;
						}
					}
					if (!transfered)
					{
						curr = curr->routing_table[no_of_bits - 1];
						transfered = 1;
					}
				}
				if (transfered)
				{
					prev = get_previous(curr->name);
					cout << "Transfered Query to: " << curr->name << "\tID: " << curr->id << endl;
					cout << "\n-------------------------------------\n";

				}
			}
		}

	}
	void search_data(long long unsigned int id, string machine_name)
	{
		cout << "\n-------------------------------------\n";
		Machine* curr = get_machine(machine_name);
		Machine* prev = get_previous(machine_name);
		bool searching = 0;
		while (!searching)
		{
			//cout << "Comparing " << prev->id << " with " << id << " and " << curr->id << endl;
			if (curr == root && id <= curr->id)
			{
				cout << "ID is in range of current Machine\n";
				curr->search_data(id);
				searching = 1;
				return;
			}
			else if (id > prev->id && id <= curr->id && curr != root)
			{
				cout << "ID is in range of current Machine\n";
				curr->search_data(id);
				searching = 1;
				return;
			}
			else if (id > prev->id && curr == root)
			{
				cout << "ID is in range of current Machine\n";
				curr->search_data(id);
				searching = 1;
				return;
			}
			else
			{
				bool transfered = 0;
				cout << "ID not in range of current Machine (ID: " << curr->id << " ) \nLooking at current Machine's Routing Table...\n";
				if (id <= curr->routing_table[0]->id || curr->routing_table[0] == root)
				{
					curr = curr->routing_table[0];
					transfered = 1;
				}
				else
				{
					for (int i = 0; i < no_of_bits - 1; i++)
					{
						if (id > curr->routing_table[i]->id && id <= curr->routing_table[i + 1]->id)
						{
							curr = curr->routing_table[i];
							transfered = 1;
							break;
						}
					}
					if (!transfered)
					{
						curr = curr->routing_table[no_of_bits - 1];
						transfered = 1;
					}
				}
				if (transfered)
				{
					prev = get_previous(curr->name);
					cout << "Transfered Query to: " << curr->name << "\tID: " << curr->id << endl;
					cout << "\n-------------------------------------\n";

				}
			}
		}

	}

	void update_data(string machine_name)
	{
		Node* dat = 0;
		string t1, t2;
		long long unsigned int key;
		Machine* curr = get_machine(machine_name);
		Machine* prev = get_previous(machine_name);
		cout << "\n-------------------------------------\n";
		cout << "Transfering data from neighbouring machines to current machines (if any)";
		cout << "\n-------------------------------------\n";
		if (curr != root)
		{
			for (long long unsigned int i = prev->id + 1; i <= curr->id; i++)
			{
				if (curr->next->is_data(i))
				{
					dat = curr->next->delete_data(i);
					if (dat != NULL)
					{
						t1 = get_file_name(dat->path);
						t2 = get_file_data(dat->path);
						key = dat->key;
						if (!(::remove(dat->path.c_str())))
							cout << "File Removed Succesfully\n\n";
						else
							cout << "Error in removing file\n";
						curr->insert_data(t1, t2, key);
						delete dat;
						dat = 0;
					}
				}
			}
		}
		else
		{
			for (long long unsigned int i = prev->id + 1; i < identifier_space; i++)
			{
				if (curr->next->is_data(i))
				{
					dat = curr->next->delete_data(i);
					if (dat != NULL)
					{
						t1 = get_file_name(dat->path);
						t2 = get_file_data(dat->path);
						key = dat->key;
						if (!(::remove(dat->path.c_str())))
							cout << "File Removed Succesfully\n\n";
						else
							cout << "Error in removing file\n";
						curr->insert_data(t1, t2, key);
						delete dat;
						dat = 0;
					}
				}
			}
			for (long long unsigned int i = 0; i <= curr->id; i++)
			{
				if (curr->next->is_data(i))
				{
					dat = curr->next->delete_data(i);
					if (dat != NULL)
					{
						t1 = get_file_name(dat->path);
						t2 = get_file_data(dat->path);
						key = dat->key;
						if (!(::remove(dat->path.c_str())))
							cout << "File Removed Succesfully\n\n";
						else
							cout << "Error in removing file\n";
						curr->insert_data(t1, t2, key);
						delete dat;
						dat = 0;
					}
				}
			}
		}
		cout << "\nData Transfer Sucessful\n";
	}

	void delete_machine(string machine_name)
	{
		machine_no--;
		Machine* curr = get_machine(machine_name);
		Machine* prev = get_previous(machine_name);
		Node* temp = 0;
		string t1, t2;
		long long unsigned int key;
		cout << "\nMoving data to sucessor....\n";
		queue<Node*> queue = curr->data->input_queue;
		while (!queue.empty())
		{
			temp = queue.front();
			queue.pop();
			curr->delete_data(temp->key);
			t1 = get_file_name(temp->path);
			t2 = get_file_data(temp->path);
			if (!(::remove(temp->path.c_str())))
				cout << "File Removed Succesfully\n\n";
			else
				cout << "Error in removing file\n";
			curr->next->insert_data(t1, t2, temp->key);
			delete temp;
			temp = 0;
		}
		if (curr == root)
			root = curr->next;

		prev->next = curr->next;
		delete curr;
		cout << "Deleted Machine\n";
	}

	~RingDht()
	{
		Machine* curr = root;
		Machine* temp;
		cout << "\n-------------------------------------\n";
		cout << "Starting Deletion Process...";
		cout << "\n-------------------------------------\n";
		while (curr->next != root)
		{
			temp = curr;
			curr = curr->next;
			delete temp;
		}
		delete curr;
	}
};