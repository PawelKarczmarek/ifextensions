#pragma once
#include <string>
#include "main.h"
# define M_PI           3.14159265358979323846  /* pi */
using namespace std;

double c_function(double n)
{
	return 2 * (log(n - 1) + 0.5572156649) - 2 * ((n - 1) / n);
}

double c_function(double n, double par)
{
	return sqrt(2 * M_PI*n*par / (par - 1));
}

struct n_ary_node
{
	int attribute_id;
	int cluster_count;
	double *cluster_limits;
	double lower_bounds[NMAX];
	double upper_bounds[NMAX];
	n_ary_node ** clusters;
};

class n_ary_tree
{
public:
	n_ary_tree(double r, double c, double ** tab, int cluster_count);
	~n_ary_tree();
	void remove_tree();
	void build_tree();
	void remove_data();
	double traverse(double record[NMAX]);
	int cluster_count;

private:
	void remove_tree(n_ary_node *leaf);
	void insert(double lower_bounds[NMAX], double upper_bounds[NMAX], n_ary_node *leaf, int depth = 0);
	int random_attribute();
	double find_attribute_min(int attribute);
	double find_attribute_max(int attribute);
	int how_many_elements(double lower_bounds[NMAX], double upper_bounds[NMAX]);
	n_ary_node *root;
	int rows;
	int columns;
	double **table;
	double traverse(double record[NMAX], int depth, n_ary_node *root);
};


n_ary_tree::n_ary_tree(double r, double c, double ** tab, int cluster_cnt)
{
	root = new n_ary_node;
	rows = r;
	columns = c;
	cluster_count = cluster_cnt;
	table = new double*[rows];
	for (int i = 0; i < rows; i++)
		table[i] = new double[columns];
	for (int i = 0; i < rows; i++)
		for (int j = 0; j < columns; j++)
			table[i][j] = tab[i][j];
}

n_ary_tree::~n_ary_tree()
{
	remove_tree();
}

void n_ary_tree::remove_tree()
{
	remove_tree(root);
}

void n_ary_tree::remove_tree(n_ary_node *leaf)
{
	if (leaf != NULL)
	{
		for (int i = 0; i<leaf->cluster_count; i++)
			remove_tree(leaf->clusters[i]);
		delete leaf;
	}
}

void n_ary_tree::remove_data()
{
	for (int i = 0; i < rows; i++)
		delete[] table[i];
	delete[]table;
}

void n_ary_tree::build_tree()
{
	double lower_bounds[NMAX];
	double upper_bounds[NMAX];
	for (int i = 0; i < columns; i++)
		lower_bounds[i] = find_attribute_min(i);
	for (int i = 0; i < columns; i++)
		upper_bounds[i] = find_attribute_max(i);
	insert(lower_bounds, upper_bounds, root);
}

void n_ary_tree::insert(double lower_bounds[NMAX], double upper_bounds[NMAX], n_ary_node *root, int depth)
{
	if (depth > MAX_DEPTH)
	{
		root->cluster_count = -1;
		return;
	}
	depth++;
	int count = how_many_elements(lower_bounds, upper_bounds);
	if (count <= 1)
		root->cluster_count = -1;
	else
	{
		root->cluster_count = cluster_count;
		root->attribute_id = random_attribute();
		root->cluster_limits = new double[cluster_count+1];
		double *m = new double[cluster_count];
		double sum = 0;
		for (int i = 0; i<cluster_count-1 ; i++)
		{
			m[i] = (double)rand() / RAND_MAX;
			sum += m[i];
		}
		sum += (double)rand() / RAND_MAX; 
		if (sum > 0)
		{
			double partial_sum = 0;
			for (int i = 0; i < cluster_count-1; i++)
			{
				partial_sum += m[i];
				m[i] = lower_bounds[root->attribute_id] + (partial_sum / sum)*(upper_bounds[root->attribute_id] - lower_bounds[root->attribute_id]);
			}
		}
		root->cluster_limits[0] = lower_bounds[root->attribute_id];
		root->cluster_limits[cluster_count] = upper_bounds[root->attribute_id];
		
		for (int i = 1; i < cluster_count; i++)
			root->cluster_limits[i] = m[i - 1];
		for (int i = 0; i < columns; i++)
		{
			root->lower_bounds[i] = lower_bounds[i];
			root->upper_bounds[i] = upper_bounds[i];
		}
		double tmp_lower_bounds[NMAX];
		double tmp_upper_bounds[NMAX];
		root->clusters = new n_ary_node*[cluster_count];
		
		for (int i = 0; i<root->cluster_count; i++)
		{
			for (int j = 0; j < columns; j++)
			{
				tmp_lower_bounds[j] = lower_bounds[j];
				tmp_upper_bounds[j] = upper_bounds[j];
			}
			tmp_lower_bounds[root->attribute_id] = root->cluster_limits[i];
			tmp_upper_bounds[root->attribute_id] = root->cluster_limits[i + 1];
			root->clusters[i] = new n_ary_node;
			insert(tmp_lower_bounds, tmp_upper_bounds, root->clusters[i], depth);
		}
	}
}

int n_ary_tree::random_attribute()
{
	return rand() % columns;
}

double n_ary_tree::find_attribute_min(int attribute_id)
{
	double min = table[0][attribute_id];
	for (int i = 1; i < rows; i++)
		if (table[i][attribute_id] < min)
			min = table[i][attribute_id];
	return min - 0.00000000001;
}

double n_ary_tree::find_attribute_max(int attribute_id)
{
	double max = table[0][attribute_id];
	for (int i = 1; i < rows; i++)
		if (table[i][attribute_id] > max)
			max = table[i][attribute_id];
	return max + 0.00000000001;
}

int n_ary_tree::how_many_elements(double lower_bounds[NMAX], double upper_bounds[NMAX])
{
	int count = 0;
	for (int i = 0; i < rows; i++)
	{
		bool found = true;
		for (int j = 0; j < columns; j++)
			if (lower_bounds[j] > table[i][j] || upper_bounds[j] < table[i][j])
				found = false;
		if (found == true)
			count++;
	}
	return count;
}

double n_ary_tree::traverse(double record[NMAX])
{
	return traverse(record, 0, root);
}

double n_ary_tree::traverse(double record[NMAX], int depth, n_ary_node *root)
{
	if (root->cluster_count>0)
	{
		int attr_id = root->attribute_id;
		bool found = true;
		for (int i = 0; i < columns; i++)
			if (record[i] < root->lower_bounds[i] || record[i] > root->upper_bounds[i])
			{
				found = false;
				return 0;
			}
		if (found)
		{
			for (int i = 0; i < root->cluster_count; i++)
			{
				if (record[root->attribute_id] >= root->cluster_limits[i] && record[root->attribute_id] <= root->cluster_limits[i + 1])
				{
					double membership = 1;
					return membership + traverse(record, depth, root->clusters[i]);
				}
			}
		}
	}
	double cnt = how_many_elements(root->lower_bounds, root->upper_bounds);
	if (cnt == 0)
		return 0;
	else if (cnt == 1)
		return 1;
	else
		return 1 + c_function(cnt,root->cluster_count);
}

