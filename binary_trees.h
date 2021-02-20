#pragma once
#include <string>
#include "main.h"
using namespace std;

static const int NMAX = 400;  

static const int MAX_DEPTH = 9; 

struct binary_node
{
	int attribute_id; 	
	double attribute_value; 
	double lower_bounds[NMAX]; 
	double upper_bounds[NMAX]; 
	binary_node* left; 
	binary_node* right; 
};

class binary_tree
{
public:
	binary_tree(double r, double c, double ** tab);
	~binary_tree(); 
	void remove_tree();
	void build_tree();
	void remove_data();
	double traverse(double rekord[NMAX]);
	void present();
	double binary_tree::e_function(int n, double *tab);
private:
	void remove_tree(binary_node *leaf);
	void insert(double lower_bounds[NMAX], double upper_bounds[NMAX], binary_node *leaf, int depth);
	int random_attribute();
	double random_attribute_value(int attribute, double lower_bound, double upper_bound);
	double find_min_of_attribute(int attribute);
	double find_max_of_attribute(int attribute);
	int number_of_elements(double lower_bounds[NMAX], double upper_bounds[NMAX]);
	binary_node *root;
	int rows;
	int columns;
	double **table;
	double traverse(double record[NMAX], int depth, binary_node *root);
	void present(binary_node *root);
	double c_function(double n);
};


binary_tree::binary_tree(double r, double c, double ** tab)
{
	root = new binary_node;
	rows = r;
	columns = c;
	table = new double*[rows];
	for (int i = 0; i < rows; i++)
		table[i] = new double [columns];
	for (int i = 0; i < rows; i++)
		for (int j = 0; j < columns; j++)
		{
			table[i][j] = tab[i][j];
		}
}

binary_tree::~binary_tree()
{
	remove_tree();
}

void binary_tree::remove_tree()
{
	remove_tree(root);
}

void binary_tree::remove_tree(binary_node *leaf)
{
	if (leaf->attribute_id>-1)
	{
		remove_tree(leaf->left);
		remove_tree(leaf->right);
	}
	delete leaf;
}

void binary_tree::remove_data()
{
	for (int i = 0; i < rows; i++)
		delete[] table[i];
	delete[]table;
}

void binary_tree::build_tree()
{
	double lower_bounds[NMAX];
	double upper_bounds[NMAX];
	for (int i = 0; i < columns; i++)
	{
		lower_bounds[i] = find_min_of_attribute(i);
	}
	for (int i = 0; i < columns; i++)
	{
		upper_bounds[i] = find_max_of_attribute(i);
	}
	root = new binary_node;
	insert(lower_bounds,upper_bounds,root, 0);
}
	
void binary_tree::insert(double lower_bounds[NMAX], double upper_bounds[NMAX], binary_node *root, int depth)
{
	if (depth > MAX_DEPTH)
	{
		root->attribute_id = -1;
		return;
	}
	depth++;
	int ile = number_of_elements(lower_bounds, upper_bounds);
	if (ile <= 1)
	{
		root->attribute_id = -1;
		return;
	}
	root->attribute_id = random_attribute();
	root->attribute_value = random_attribute_value(root->attribute_id, lower_bounds[root->attribute_id], upper_bounds[root->attribute_id]);
	for (int i = 0; i < columns; i++)
	{
		root->lower_bounds[i] = lower_bounds[i];
		root->upper_bounds[i] = upper_bounds[i];
	}
	double lower_bounds_of_left_leaf[NMAX];
	double upper_bounds_of_left_leaf[NMAX];
	double lower_bounds_of_right_leaf[NMAX];
	double upper_bound_or_right_leaf[NMAX];
	for (int i = 0; i < columns; i++)
	{
		lower_bounds_of_left_leaf[i] = lower_bounds[i];
		upper_bounds_of_left_leaf[i] = upper_bounds[i];
		lower_bounds_of_right_leaf[i] = lower_bounds[i];
		upper_bound_or_right_leaf[i] = upper_bounds[i];
	}
	upper_bounds_of_left_leaf[root->attribute_id] = root->attribute_value;
	lower_bounds_of_right_leaf[root->attribute_id] = root->attribute_value;
	root->left = new binary_node;
	root->right = new binary_node;
	insert(lower_bounds_of_left_leaf, upper_bounds_of_left_leaf, root->left, depth);
	insert(lower_bounds_of_right_leaf, upper_bound_or_right_leaf, root->right, depth);
}	
	
int binary_tree::random_attribute()
{
	return rand() % columns;
}
	
double binary_tree::random_attribute_value(int attribute, double lower_bound, double upper_bound)
{
	double d = (double)rand() / RAND_MAX;
	return lower_bound + d * (upper_bound - lower_bound);
}
	
double binary_tree::find_min_of_attribute(int id_atrybutu)
{
	double min = table[0][id_atrybutu];
	for (int i = 1; i < rows; i++)
		if (table[i][id_atrybutu] < min)
			min = table[i][id_atrybutu];
	return min-0.00000000001;
}
	
double binary_tree::find_max_of_attribute(int id_atrybutu)
{
	double max = table[0][id_atrybutu];
	for (int i = 1; i < rows; i++)
		if (table[i][id_atrybutu] > max)
			max = table[i][id_atrybutu];
	return max+0.000000000001;
}
	
int binary_tree::number_of_elements(double lower_bounds[NMAX], double upper_bounds[NMAX])
{
	int counter = 0;
	for (int i = 0; i < rows; i++)
	{
		bool found = true;
		for (int j = 0; j < columns; j++)
			if (lower_bounds[j] > table[i][j] || upper_bounds[j] < table[i][j])
				found = false;
		if (found == true)
			counter++;
	}
	return counter;
}

double binary_tree::traverse(double record[NMAX])
{
	return traverse(record, 0, root);
}

double binary_tree::traverse(double record[NMAX], int depth, binary_node *root)
{
	if (root->attribute_id>-1)
	{
		int attr_id = root->attribute_id;
		double value = root->attribute_value;
		bool found = true;
		for (int i = 0; i < columns; i++)
			if (record[i] < root->lower_bounds[i])
				found = false;
		if (found && record[attr_id] < value)
			return 1 + traverse(record, depth, root->left);
		found = true;
		for (int i = 0; i < columns; i++)
			if (record[i] > root->upper_bounds[i])
				found = false;
		if (found && record[attr_id] >= value)
			return 1 + traverse(record, depth, root->right);
	}
	double count = number_of_elements(root->lower_bounds, root->upper_bounds);
	if (count == 0)
		return 0;
	else if (count == 1)
		return 1;
	else
		return 1+c_function(count);
}

void binary_tree::present()
{
	present(root);
}

void binary_tree::present(binary_node *root)
{
	if (root->attribute_id <0)
		cout << "Empty";
	else
	{
		cout << endl;
		cout << "Node"<<endl;
		cout << "Data:" << endl;
		cout << root->attribute_id << ": " << root->attribute_value;
		cout << "Lower bounds:" << endl;
		for (int i = 0; i < columns; i++)
			cout << i << ": " << root->lower_bounds[i]<<endl;
		cout << endl;
		cout << "Upper bounds:" << endl;
		for (int i = 0; i < columns; i++)
			cout << i << ": " << root->upper_bounds[i]<<endl;
		system("pause");
		present(root->left);
		present(root->right);
	}
}

double binary_tree::e_function(int n, double *tab)
{
	return 0;
}

double binary_tree::c_function(double n)
{
	return 2 * (log(n - 1) + 0.5572156649) - 2 * ((n - 1) / n);
}