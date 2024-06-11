
#include"Machines.h"
#include<cmath>
#include<ctime>
int main()
{
	srand(time(0));
	cout << "\t\tWelcome\nKindly set up your file system\n\n";
	cout << "Enter Number of Bits for identifier Space: ";
	cin >> no_of_bits;
	identifier_space = pow(2, no_of_bits);
	long long unsigned int machine_no;
	cout << "Enter the number of starting machines: ";
	cin >> machine_no;
	int choice = -1;

	string name;
	RingDht ring;
	while (machine_no<1 || machine_no>identifier_space)
	{
		cout << "Invalid Number of Machines, please choose a number between 1 & " << identifier_space << endl;
		cin >> machine_no;
	}
	for (int i = 0; i < machine_no; i++)
	{
		cout << "\n1. Manually Enter Name\n2. Automatically Set Name\nChoose an option: \n";
		cin >> choice;
		while (choice != 1 && choice != 2)
		{
			cout << "Invalid input, pls enter again: ";
			cin >> choice;
		}
		if (choice == 1)
		{
			cout << "Enter a name: ";
			cin >> name;
			while (ring.find(name))
			{
				cout << "Name/ID Already Exists, enter another one: ";
				cin >> name;
			}
			ring.insert(name);
		}
		else
		{
			name = "Machine_";
			int num = 1 + rand() % 100;
			name += to_string(num);
			while (ring.find(name))
			{
				name = "Machine_";
				num = 1 + rand() % 100;
				name += to_string(num);
			}
			cout << "Name generated: " << name << endl;
			ring.insert(name);
		}
	}
	ring.update_routing_tables();


	choice = -1;
	cout << "\n-----------------------------------\nWelcome To IPFS\n-----------------------------------\n";
	string temp_path;
	Machine* temp;
	string file_name, file_data, machine_name;
	while (choice != 9)
	{
		cout << "\n1. Insert Data\n2. Print Routing Table of Machine\n3. Print B-Tree of Machine\n4. Print Ring DHT\n5. Delete Data\n6. Search Data\n7. Add Machine\n8. Delete Machine\n9. Exit\nEnter Your Choice: ";
		cin >> choice;
		cin.ignore();
		if (choice == 9)
			break;
		if (choice == 1)
		{
			cout << "1.Existing File\n2.New File\n";
			cin >> choice;
			while (choice < 1 || choice > 2)
			{
				cout << "Invalid Input; Please Enter Valid Choice: ";
				cin >> choice;
			}
			if (choice == 1)
			{
				cout << "Enter File Path: (Make sure there are no spaces in the text file's name)\n";
				cin >> temp_path;
				//cout << temp_path;
				cin.ignore();
				file_name = get_file_name(temp_path);
				file_data = get_file_data(temp_path);
			}
			else if (choice == 2)
			{
				cout << "Enter File Name (Make sure there are no spaces in name): ";
				cin >> file_name;
				//	cout << file_name;
				cin.ignore(255, '\n');
				cout << "Enter File Data: ";
				getline(cin, file_data);
				cin.ignore(255, '\n');
			}

			cout << "Enter Name of Machine: ";
			cin >> machine_name;
			//	cout << machine_name;
			if (machine_name == "")
				machine_name = " ";
			cin.ignore(255, '\n');
			while (!ring.find(machine_name))
			{
				cout << "Sorry Machine Not Found, Enter a valid name: ";
				cin >> machine_name;
				if (machine_name == "")
					machine_name = " ";
				cin.ignore(255, '\n');
			}
			//	cout << file_name << " " << file_data << " " << machine_name;
			ring.insert_data(file_name, file_data, machine_name);
		}
		else if (choice == 2)
		{
			cout << "\nEnter Machine's Name: ";
			cin >> machine_name;
			while (!ring.find(machine_name))
			{
				cout << "\nSorry Machine Not Found, Enter a valid name: ";
				cin >> machine_name;
			}
			temp = ring.get_machine(machine_name);
			cout << "\n-------------------------------------\n";
			temp->display_routing_table();
			cout << "\n\n";
		}
		else if (choice == 3)
		{
			cout << "\nEnter Machine's Name: ";
			cin >> machine_name;
			while (!ring.find(machine_name))
			{
				cout << "\nSorry Machine Not Found, Enter a valid name: ";
				cin >> machine_name;
			}
			cout << "\n\n1. Level Order\n2. Pre-order\nChoose: ";
			cin >> choice;
			temp = ring.get_machine(machine_name);
			cout << "BTree of " << temp->name << "\tID: " << temp->id << endl;
			cout << "\n-------------------------------------\n";
			if (choice == 1)
				temp->data->level_order();
			else if (choice == 2)
				temp->data->display();
			cout << "\n-------------------------------------\n";
		}
		else if (choice == 4)
		{
			cout << "\n1. With all routing tables\n2. Without routing tables\nChoose: ";
			cin >> choice;
			while (choice < 1 && choice > 2)
			{
				cout << "Invalid option please enter valid choice: ";
				cin >> choice;
			}
			ring.display(choice);
		}
		else if (choice == 5)
		{
			long long unsigned int id;
			cout << "Enter id to delete: ";
			cin >> id;
			cout << "\nEnter Machine Name: ";
			cin >> machine_name;
			cin.ignore(255, '\n');
			while (!ring.find(machine_name))
			{
				cout << "Sorry Machine Not Found, Enter a valid name: ";
				cin >> machine_name;
			}
			ring.delete_data(id, machine_name);
		}
		else if (choice == 6)
		{
			long long unsigned int id;
			cout << "Enter id to search: ";
			cin >> id;
			cout << "\nEnter Machine Name: ";
			cin >> machine_name;
			cin.ignore(255, '\n');
			while (!ring.find(machine_name))
			{
				cout << "Sorry Machine Not Found, Enter a valid name: ";
				cin >> machine_name;
				cin.ignore(255, '\n');
			}
			ring.search_data(id, machine_name);
		}
		else if (choice == 7)
		{
			if (ring.machine_no == identifier_space)
				cout << "Max Number of Machines has reached";
			else
			{
				cout << "\n1. Manually Enter Name\n2. Automatically Set Name\nChoose an option: \n";
				cin >> choice;
				while (choice != 1 && choice != 2)
				{
					cout << "Invalid input, pls enter again: ";
					cin >> choice;
				}
				if (choice == 1)
				{
					cout << "Enter a name: ";
					cin >> name;
					while (ring.find(name))
					{
						cout << "Name/ID Already Exists, enter another one: ";
						cin >> name;
					}
					ring.insert(name);
				}
				else
				{
					name = "Machine_";
					int num = 1 + rand() % 100;
					name += to_string(num);
					while (ring.find(name))
					{
						name = "Machine_";
						num = 1 + rand() % 100;
						name += to_string(num);
					}
					cout << "Name generated: " << name << endl;
					ring.insert(name);
				}
				ring.update_routing_tables();
				ring.update_data(name);
			}
		}
		else if (choice == 8)
		{
			if (ring.machine_no == 1)
				cout << "Cannot delete Machine otherwise ring will become empty\n";
			else
			{
				cout << "Enter Machine's name to delete: ";
				cin >> name;
				cin.ignore(255, '\n');
				while (!ring.find(name))
				{
					cout << "Machine not found, enter valid name\n";
					cin >> name;
					cin.ignore(255, '\n');
				}
				ring.delete_machine(name);
				ring.update_routing_tables();
			}
		}
	}



	return 0;
}
