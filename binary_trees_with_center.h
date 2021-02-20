#pragma once
#include <string>
#include "main.h"
#include <math.h>
using namespace std;

struct binary_node_with_center
{
	int attribute_id; 
	double center;
	double attribute_value; 
	double lower_bounds[NMAX]; 
	double upper_bounds[NMAX];
	binary_node_with_center* left;
	binary_node_with_center* right; 
};

class binary_tree_with_center
{
public:
	binary_tree_with_center(double r, double c, double ** tab, double *low_bounds, double *upp_bounds);
	~binary_tree_with_center();
	void remove_tree();
	void build_tree();
	void remove_data();
	double traverse(double record[NMAX]);
	void present();
	double binary_tree_with_center::E_function(int n, double *tab);
private:
	void remove_tree(binary_node_with_center *leaf);
	void insert(double lower_bounds[NMAX], double upper_bounds[NMAX], binary_node_with_center *leaf, int depth);
	int random_attribute();
	double random_attribute_value(int attribute, double lower_bound, double upper_bound);
	double find_attribute_min(int attribute);
	double find_attribute_max(int attribute);
	int how_many_elements(double lower_bounds[NMAX], double upper_bounds[NMAX], double &center, int attribute_id);
	binary_node_with_center *root;
	int rows;
	int columns;
	double **table;
	double traverse(double record[NMAX], int depth, binary_node_with_center *root);
	void present(binary_node_with_center *root);
};


binary_tree_with_center::binary_tree_with_center(double r, double c, double ** tab, double *low_bounds, double *upp_bounds)
{
	root = new binary_node_with_center;
	rows = r;
	columns = c;
	table = new double*[rows];
	for (int i = 0; i < rows; i++)
		table[i] = new double[columns];
	for (int i = 0; i < rows; i++)
		for (int j = 0; j < columns; j++)
		{
			table[i][j] = tab[i][j];
		}
	for (int i = 0; i < c; i++)
	{
		root->lower_bounds[i] = low_bounds[i];
		root->upper_bounds[i] = upp_bounds[i];
	}
}

binary_tree_with_center::~binary_tree_with_center()
{
	remove_tree();
}

void binary_tree_with_center::remove_tree()
{
	remove_tree(root);
}

void binary_tree_with_center::remove_tree(binary_node_with_center *leaf)
{
	if (leaf->attribute_id>-1)
	{
		remove_tree(leaf->left);
		remove_tree(leaf->right);
	}
	delete leaf;
}

void binary_tree_with_center::remove_data()
{
	for (int i = 0; i < rows; i++)
		delete[] table[i];
	delete[]table;
}

void binary_tree_with_center::build_tree()
{
	double lower_bounds[NMAX];
	double upper_bounds[NMAX];
	for (int i = 0; i < columns; i++)
	{
		lower_bounds[i] = root->lower_bounds[i];
		upper_bounds[i] = root->upper_bounds[i];
	}
	for (int i = 0; i < columns; i++)
	{
		lower_bounds[i] = find_attribute_min(i);
	}
	for (int i = 0; i < columns; i++)
	{
		upper_bounds[i] = find_attribute_max(i);
	}
	insert(lower_bounds, upper_bounds, root, 0);
}

void binary_tree_with_center::insert(double lower_bounds[NMAX], double upper_bounds[NMAX], binary_node_with_center *root, int depth)
{
	if (depth > MAX_DEPTH)
	{
		root->attribute_id = -1;
		return;
	}
	depth++;
	double center;
	root->attribute_id = random_attribute(); 
	int count = how_many_elements(lower_bounds, upper_bounds, center, root->attribute_id);
	if (count <= 1)
	{
		root->attribute_id = -1;
		return;
	}
	root->center = center;
	root->attribute_value = random_attribute_value(root->attribute_id, lower_bounds[root->attribute_id], upper_bounds[root->attribute_id]);
	for (int i = 0; i < columns; i++)
	{
		root->lower_bounds[i] = lower_bounds[i];
		root->upper_bounds[i] = upper_bounds[i];
	}
	double lower_bounds_of_left_leaf[NMAX];
	double upper_bounds_of_left_leaf[NMAX];
	double lower_bounds_of_right_leaf[NMAX];
	double upper_bounds_of_right_leaf[NMAX];
	for (int i = 0; i < columns; i++)
	{
		lower_bounds_of_left_leaf[i] = lower_bounds[i];
		upper_bounds_of_left_leaf[i] = upper_bounds[i];
		lower_bounds_of_right_leaf[i] = lower_bounds[i];
		upper_bounds_of_right_leaf[i] = upper_bounds[i];
	}
	upper_bounds_of_left_leaf[root->attribute_id] = root->attribute_value;
	lower_bounds_of_right_leaf[root->attribute_id] = root->attribute_value;
	root->left = new binary_node_with_center;
	root->right = new binary_node_with_center;
	insert(lower_bounds_of_left_leaf, upper_bounds_of_left_leaf, root->left, depth);
	insert(lower_bounds_of_right_leaf, upper_bounds_of_right_leaf, root->right, depth);
}

int binary_tree_with_center::random_attribute()
{
	return rand() % columns;
}

double binary_tree_with_center::random_attribute_value(int attribute, double lower_bound, double upper_bound)
{
	double d = (double)rand() / RAND_MAX;
	return lower_bound + d * (upper_bound - lower_bound);
}

double binary_tree_with_center::find_attribute_min(int attribute_id)
{
	double min = table[0][attribute_id];
	for (int i = 1; i < rows; i++)
		if (table[i][attribute_id] < min)
			min = table[i][attribute_id];
	return min - 0.00000000001;
}

double binary_tree_with_center::find_attribute_max(int id_atrybutu)
{
	double max = table[0][id_atrybutu];
	for (int i = 1; i < rows; i++)
		if (table[i][id_atrybutu] > max)
			max = table[i][id_atrybutu];
	return max + 0.000000000001;
}

int binary_tree_with_center::how_many_elements(double lower_bounds[NMAX], double upper_bounds[NMAX], double &center, int attribute_id = -1)
{
	double average = -1;
	int count = 0;
	for (int i = 0; i < rows; i++)
	{
		bool found = true;
		for (int j = 0; j < columns; j++)
			if (lower_bounds[j] > table[i][j] || upper_bounds[j] < table[i][j])
				found = false;
		if (found == true)
		{
			count++;
			if (attribute_id >= 0)
			{
				if (count == 1)
					average = table[i][attribute_id];
				else
					average += table[i][attribute_id];
			}
		}
	}
	if (count>0)
		center = average / count;
	return count;
}

double binary_tree_with_center::traverse(double record[NMAX])
{
	return traverse(record, 0, root);
}

double binary_tree_with_center::traverse(double record[NMAX], int depth, binary_node_with_center *root)
{
	if (root->attribute_id>-1)
	{
		int attr_id = root->attribute_id;
		double value = root->attribute_value;
		double center = root->center;
		double low_bound = root->upper_bounds[root->attribute_id];
		double upp_bound = root->lower_bounds[root->attribute_id];
		
		bool found = true;
		for (int i = 0; i < columns; i++)
			if (record[i] < root->lower_bounds[i])
				found = false;
		if (found && record[attr_id] < value)
		{
			double tmp;
			if (record[attr_id] != center)
				tmp = abs(record[attr_id] - center) / abs(upp_bound - center);
			else
				tmp = 0;
			if (tmp > 1)
				tmp = 1;
			if (tmp < 0)
				tmp = 0;
			return 1 - tmp + traverse(record, depth, root->left);
		}
		found = true;
		for (int i = 0; i < columns; i++)
			if (record[i] > root->upper_bounds[i])
				found = false;
		if (found && record[attr_id] >= value)
		{
			double tmp;
			if (record[attr_id] != center)
				tmp = abs(record[attr_id] - center) / abs(low_bound - center);
			else
				tmp = 0;
			if (tmp > 1)
				tmp = 1;
			if (tmp < 0)
				tmp = 0;
			return 1 - tmp + traverse(record, depth, root->right);
		}
	}
	return 0;
}

void binary_tree_with_center::present()
{
	present(root);
}

void binary_tree_with_center::present(binary_node_with_center *root)
{
	if (root->attribute_id <0)
		cout << "Empty";
	else
	{
		cout << endl;
		cout << "Node" << endl;
		cout << "Data:" << endl;
		cout << root->attribute_id << ": " << root->attribute_value;
		cout << "Lower bounds:" << endl;
		for (int i = 0; i < columns; i++)
			cout << i << ": " << root->lower_bounds[i] << endl;
		cout << endl;
		cout << "Upper bounds:" << endl;
		for (int i = 0; i < columns; i++)
			cout << i << ": " << root->upper_bounds[i] << endl;
		system("pause");
		present(root->left);
		present(root->right);
	}
}