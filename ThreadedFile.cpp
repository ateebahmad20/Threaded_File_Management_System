#include<iostream>
#include<string>
#include<fstream>
#include<sstream>
using namespace std;

// A function for going back
void clear();

// Amount of directories that can be created in our File Management System
#define SIZE 100 

class file
{
public:

	file* next; //Pointer to the next file
	string name; //Variable to store the name
	string text; //Variable to store file's content
	int size = 0; //Variable to store file's size
};

class FileSystem
{
public:

	file* head[SIZE]; //Pointer to head node
	int MEMORY = 1000000; //Total Memory of OS => Approximately 100MB
	int Dir = 0; //A counter to maintain directories
	fstream* output = NULL;
	int t_id;

	//Initializing all directories as NULL:

	FileSystem()
	{
		for (int i = 0; i < SIZE; i++)
		{
			head[i] = NULL;
		}
	}

	FileSystem(int n)
	{
		output = new fstream[n];
	}

	//  Get thread id
	void get_Tid(int t)
	{
		t_id = t;
		output[t_id].open("output_thread" + to_string(t_id + 1) + ".txt", ios::out);
		cout << "Creating Output File " << t_id + 1 << "..." << endl;
	}

	void set_size(string Fname, int size)
	{
		for (int i = 0;i < SIZE;i++)
		{
			//Searching for the file:

			file* temp = head[i];

			while (temp != NULL && temp->name != Fname)
			{
				temp = temp->next;
			}

			if (temp != NULL && temp->name == Fname) //When file is found
			{
				temp->size = size;
			}
		}
		MEMORY -= size;
	}

	//To check whether any more directories can be created or not:
	bool Any_isEmpty()
	{
		int flag = 0;

		for (int i = 0; i < SIZE; i++)
		{
			if (head[i] == NULL)
				flag = 1; //if any directory is empty, flag returns 1
		}

		return flag;
	}

	//Function to check if any directory exist in the system or not:

	bool All_isEmpty()
	{
		int flag = 1;

		for (int i = 0; i < SIZE; i++)
		{
			if (head[i] != NULL)
				flag = 0; //if atleast one directory exist, flag becomes 0
		}

		return flag;
	}

	//Function to Create Folders:

	void Mkdir(string dirName)
	{

		if (Any_isEmpty() && !Dir_Exist(dirName)) //if folder not already exists
		{
			//Creating new directory:
			file* dir_name = new file();
			dir_name->name = dirName;

			//Pointing head to the new directory
			head[Dir] = dir_name;
			output[t_id] << "\n**Folder " << head[Dir]->name << " Created Successfully!**" << endl;
			Dir++;
		}

		else
			output[t_id] << "Directories Are Full OR Directory Already Exist!" << endl;
	}

	//Function to Search for a particular directory

	bool Dir_Exist(string dname)
	{
		int flag = 0;
		
		for (int i = 0;i < SIZE;i++)
		{
			file* temp = head[i];

			if (temp != NULL && temp->name == dname)
				flag = 1; //if directory exists, flag becomes 1 
		}

		return flag;
	}

	//To check whether a given file exist in the folder or not:

	bool File_Exist(string fname, string dname)
	{
		int flag = 0;

		for (int i = 0;i < SIZE;i++)
		{
			if (head[i] != NULL && head[i]->name == dname) //Searching folder
			{
				file* temp = head[i]->next;

				while (temp != NULL && temp->name != fname) //Searching File
				{
					temp = temp->next;
				}

				if (temp != NULL && temp->name == fname) //If file exist
					flag = 1;
			}

		}

		return flag;
	}

	//Function to Create Files in a Specific Folder

	void Create(string Fname, string d_name)
	{

		file* new_file = new file();
		new_file->name = Fname;

		if (All_isEmpty()) //if no directory exists
		{
			output[t_id] << "\nCreate a Folder First" << endl;
		}

		else if (!File_Exist(Fname, d_name)) //If file not already exist in folder
		{
			//If such a folder already exist in the System:

			if (Dir_Exist(d_name))
			{
				for (int i = 0;i < SIZE;i++)
				{
					//If directory is found and head is not NULL

					if (head[i] != NULL && head[i]->name == d_name)
					{
						file* temp = head[i]; //Creating a temporary node to point head

						while (temp->next != NULL) //Till end of list is reached
						{
							temp = temp->next;
						}

						temp->next = new_file; //Storing new file
						output[t_id] << "\n" << temp->next->name << " was created in Folder " << d_name << "**" << endl;
					}
				}
			}

			else
			{
				output[t_id] << "\nFolder \"" << d_name << "\" Does not Exist!" << endl;
			}
		}

		else
		{
			output[t_id] << "\nFile " << Fname << " Already Exist!" << endl;
		}
	}

	//A function for deleting directory:

	void Delete_Dir(string dname)
	{
		if (Dir_Exist(dname))
		{
			for (int i = 0;i < SIZE;i++)
			{
				if (head[i] != NULL && head[i]->name == dname)
				{
					file* temp = head[i];

					while (temp->next != NULL) //traversing all files and freeing memory
					{
						temp = temp->next;
						MEMORY += temp->size;
					}

					head[i] = NULL; // making folder equal to NULL if it exists
				}
			}

			output[t_id] << "\n**Folder \"" << dname << "\" Deleted!**" << endl;

		}

		else
		{
			output[t_id] << dname << " Doesnot Exist" << endl;
		}
	}

	void Delete_file(string fname, string dname)
	{
		
		if (File_Exist(fname,dname))
		{
			for (int i = 0;i < SIZE;i++)
			{
				if (head[i] != NULL && head[i]->name == dname)
				{
					file* temp = head[i];

					while (temp->next->name != fname) //Until the file is found
					{
						temp = temp->next; //Increment Temp
					}

					//Freeing Space Occupied by the file 
					MEMORY += temp->next->size;

					//Deleting the file by pointing it to NULL
					temp->next = temp->next->next;
				}
			}
			output[t_id] << "\n\"" << fname << "\" Deleted!**" << endl;

		}

		else
		{
			output[t_id] << fname << " Doesnot Exist" << endl;
		}
	}

	//A function for moving a file from one folder to another:

	void Move(string fname, string dname)
	{
		string cur_dir, cur_text;
		int length, new_size;

		//Finding current directory of the file:

		for (int i = 0;i < SIZE;i++)
		{
			file* temp = head[i];

			while (temp != NULL && temp->name != fname)
			{
				temp = temp->next;
			}

			if (temp != NULL && temp->name == fname)
			{
				cur_dir = head[i]->name;
				cur_text = temp->text;
			}
		}

		if (File_Exist(fname, cur_dir) && Dir_Exist(dname)) //if both the file and folder exists
		{
			output[t_id] << "\n\nMoving File " << fname << " From " << cur_dir << "..." << endl;
			Delete_file(fname, cur_dir); //Deleting Previous file

			//Creating the Same file in another folder, hence MOVING:

			file* new_file = new file();
			new_file->name = fname;
		
			if (!File_Exist(fname, dname)) //If file not already exist in folder
			{
				//If such a folder already exist in the System:
				if (Dir_Exist(dname))
				{
					for (int i = 0;i < SIZE;i++)
					{
						//If directory is found and head is not NULL

						if (head[i] != NULL && head[i]->name == dname)
						{
							file* temp = head[i]; //Creating a temporary node to point head

							while (temp->next != NULL) //Till end of list is reached
							{
								temp = temp->next;
							}

							temp->next = new_file; //Storing new file
							output[t_id] << "\nFile " << fname << " Successfully Moved!" << endl;
							
							//Moving File Content:
							temp->next->text = cur_text;
							length = cur_text.length();
							new_size = length * 1; //Assuming each alphabet takes 1 byte to store

							temp->next->size = new_size; //Writing new file size
							MEMORY -= new_size; //Decreasing OS memory
						}
					}
				}

				else
					output[t_id] << "\nFolder \"" << dname << "\" Does not Exist!" << endl;
			}

			else
				output[t_id] << "\nFile " << fname << " Already Exist!" << endl;
		}

		else
			output[t_id] << "File or Folder Doesnot Exist" << endl;
	}

	//Function to Read From a File:

	void ReadFile(string Fname)
	{
		for (int i = 0;i < SIZE;i++)
		{
			//Searching for the file:

			file* temp = head[i];

			while (temp != NULL && temp->name != Fname)
			{
				temp = temp->next;
			}

			if (temp != NULL && temp->name == Fname) //When file is found
			{
				if (temp != NULL && temp->text.empty()) //if file doesnot contain text
				{
					output[t_id] << "\nFile is Empty. Enter some text..." << endl;
				}


				else
				{
					output[t_id] << endl << "\"" << temp->text << "\"" << endl;

				}
			}
		}
	}

	//Function for writing text in a file:

	void WriteFile(string Fname, string str,string Folder)
	{
		int length, new_size=0;
		file* temp = head[0];

		for (int i = 0;i < SIZE;i++)
		{
		
			for (int i = 0;i < SIZE;i++) //Finding Directory
			{
				if (head[i] != NULL && head[i]->name == Folder)
				{
					temp = head[i];
					break;
				}
			}

			while (temp != NULL && temp->name != Fname)
			{
				temp = temp->next;
			}

			if (temp != NULL && temp->name == Fname) //When file is found
			{
				temp->text = str; //Inserting text into File

				//Calculating File Size:
				length = temp->text.length();
				new_size = length; //Assuming each alphabet takes 1 byte to store
				
				temp->size = new_size; //Writing new file size
			}
		}
		MEMORY -= new_size; //Decreasing OS memory

	}

	//Opening a file to perform various operations:

	void OpenFile(string fname, string Folder, string mode)
	{
		string text;

		if (All_isEmpty()) //if no file exists
		{
			output[t_id] << "\nNo Such File Exist!" << endl;
		}

		else
		{
			if (mode == "r")
			{
				if (File_Exist(fname, Folder))
				{
					output[t_id] << endl << fname << " Opened in read mode!";

					ReadFile(fname);
				}

				else
					output[t_id] << fname << " Does not Exist!" << endl;
			}

			else if (mode == "w")
			{
				if (File_Exist(fname, Folder))
				{
					output[t_id] << endl << fname << " Opened in Write mode!";
				}

				else
					output[t_id] << fname << " Does not Exist!" << endl;
			}
			
		}
	}

	//Function to truncate file for new size:

	void Truncate_file(string fname, int new_size)
	{
		string t_text = "";
		int t_size;

		//searching for file

		for (int i = 0;i < SIZE;i++)
		{
			file* temp = head[i];

			while (temp != NULL && temp->name != fname)
			{
				temp = temp->next;
			}

			if (temp != NULL && temp->name == fname) //When file is found
			{
				if (temp->text.empty())
					cout << "No Content Exist to Truncate in the file" << endl;

				// truncating file:

				else
				{
					for (int i = 0;i < new_size / 2;i++)
					{
						t_text += temp->text[i];
					}

					t_size = temp->size - new_size; //calculating truncated size
					temp->text = t_text; //inserting truncated text

					//Inserting new size:
					MEMORY += t_size;
					temp->size = new_size;

					cout << "\n**File " << fname << " Truncated to " << new_size << " Bytes**" << endl;
				}
			}
		}
	}

	//Closing the File

	void Close(string fname)
	{
		output[t_id] << fname << " Closed Successfully!" << endl;
	}
	
	//Checking Memory of the OS:

	void OS_Memory()
	{
		cout << "\nMemory: " << MEMORY << " Bytes" << endl;
	}

	//Showing Distribution of files and folders:

	void Memory_Map()
	{
		fstream data;
		data.open("Data.txt", ios::out);

		if (!All_isEmpty()) //If atleast one directory exist to show
		{

			for (int i = 0;i < SIZE;i++)
			{
				if (head[i] != NULL)
				{
					file* temp = head[i]->next;

					output[t_id] << head[i]->name << ":";
					data << head[i]->name << ":";

					while (temp != NULL)
					{
						output[t_id] << temp->name << "[" << temp->size << "]" << "->";
						data << temp->name << "[" << temp->size << "]" << "->";
						temp = temp->next;
					}

					data << endl;
					output[t_id] << "NULL" << endl;
				}
			}
		}

		else
		{
			output[t_id] << "\nNo Folder Or Files to Show" << endl;
		}
	}
};

// A function for loading data from DAT file
void load_data(FileSystem* obj)
{
	fstream data;
	char line[SIZE], * word, * word2, *word3;
	int size;
	data.open("Data.txt", ios::in);

	while (data.getline(line, SIZE)) //while there's still some text to read
	{
		//parsing string based on spaces:
		word = strtok(line, ":");
		obj->Mkdir(word); //creating folder

		while (1)
		{
			word2 = strtok(NULL, "[");

			if (word2 == NULL) //if delimeter doesn't exist, break
				break;

			obj->Create(word2, word); //creating file

			word3 = strtok(NULL, ">");

			if (word3 == NULL) //if delimeter doesn't exist, break
				break;

			size = stoi(word3); //converting string to integer
			obj->set_size(word2, size); //setting file's size
		}
	}
}

int main()
{
	char line[SIZE], * word, * word2, * word3;
	int threads;

	cout << "\t\tx----------------------------------------------------------------x" << endl;
	cout << "\t\t\t\t\tFILE MANAGEMENT SYSTEM\t\t" << endl << endl;
	cout << "\t\t1-Create Folder       2-Create File\t    3-Show Memory Map" << endl;
	cout << "\t\t4-Delete Folder\t      5-Delete File         6-Move File" << endl;
	cout << "\t\t7-Open File\t      8-Close File          9-Show OS Memory" << endl;
	cout << "\t\t10-Truncate File      11-Exit" << endl << endl;
	cout << "\t\tx----------------------------------------------------------------x" << endl;

	// Creating threads:

	cout << "\nEnter Number of Threads You Would Like to Create (Max 6): ";
	cin >> threads;
	FileSystem obj(threads);

	fstream* file = new fstream[threads];
	
	for (int i = 0;i < threads;i++)
	{
		file[i].open("input_thread" + to_string(i+1) + ".txt", ios::in);
	}

	// Loading existing data (files & folders) from DAT file:
	load_data(&obj);
	
	//Executing Threads:
	cout << "\nThreads Execution Started..." << endl << endl;

	for (int i = 0;i < threads;i++)
	{
		// sending thread id
		obj.get_Tid(i);

		while (file[i].getline(line, SIZE)) //while there's still some text to read
		{
			//parsing string based on spaces:
			word = strtok(line, " ");

			// Create Directory
			if (strcmp(word, "mk_dir") == 0)
			{
				word = strtok(NULL, " ");
				obj.Mkdir(word);
			}

			// Create File
			else if (strcmp(word, "create") == 0)
			{
				word = strtok(NULL, ",");
				word2 = strtok(NULL, " ");
				obj.Create(word, word2);
			}

			// Delete File
			else if (strcmp(word, "delete") == 0)
			{
				word = strtok(NULL, ",");
				word2 = strtok(NULL, " ");
				obj.Delete_file(word, word2);
			}

			// Opening File
			else if (strcmp(word, "open") == 0)
			{
				word = strtok(NULL, ",");
				word2 = strtok(NULL, ",");
				word3 = strtok(NULL, " ");
				obj.OpenFile(word, word2, word3);
			}

			// Moving file
			else if (strcmp(word, "move") == 0)
			{
				word = strtok(NULL, ",");
				word3 = strtok(NULL, " ");
				obj.Move(word, word3);
			}

			// Writing to a File
			else if (strcmp(word, "write_to_file") == 0)
			{
				word = strtok(NULL, ",");
				word2 = strtok(NULL, ",");
				word3 = strtok(NULL, " ");

				obj.output[i] << " Text Written: " << word3 << endl;
				obj.WriteFile(word, word3, word2);
			}

			// Closing File
			else if (strcmp(word, "close") == 0)
			{
				word = strtok(NULL, " ");
				obj.Close(word);
			}

			// Showing Memory Map
			else if (strcmp(word, "show_memory_map") == 0)
			{
				obj.output[i] << "\nMemory Map:" << endl;
				obj.Memory_Map();
			}

		}
	}
	cout << "\nThreads Execution Finished..." << endl;


}


void clear()
{
	cout << "\n\nPress E to go back..." << endl;
	getchar();
	getchar();
	system("CLS");
}
