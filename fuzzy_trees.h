#pragma once
#include <string>
#include "main.h"
using namespace std;

struct fuzzy_node
{
	int attribute_id;
	int cluster_count;
	double *cluster_centers;
	double *cluster_limits; 
	double lower_bounds[NMAX]; 
	double upper_bounds[NMAX];
	fuzzy_node ** clusters;
};

class fuzzy_tree
{
public:
	fuzzy_tree(double r, double c, double ** tab);
	~fuzzy_tree();
	void remove_tree();
	void build_tree();
	void remove_data();
	double traverse(double record[NMAX]);

private:
	void remove_tree(fuzzy_node *lisc);
	void insert(double lower_bounds[NMAX], double upper_bounds[NMAX], fuzzy_node *leaf, int depth = 0);
	int random_attribute();
	double find_attribute_min(int attribute);
	double find_attribute_max(int attribute);
	int how_many_elements(double lower_bounds[NMAX], double upper_bounds[NMAX]);
	fuzzy_node *root;
	int rows;
	int columns;
	double **table;
	double traverse(double record[NMAX], int depth, fuzzy_node *root);
	double kMeans(int i, int attribute_id, double lower_bounds[NMAX], double upper_bounds[NMAX], double **centers, double **limits);
};


fuzzy_tree::fuzzy_tree(double r, double c, double ** tab)
{
	root = new fuzzy_node;
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
}

fuzzy_tree::~fuzzy_tree()
{
	remove_tree();
}

void fuzzy_tree::remove_tree()
{
	remove_tree(root);
}

void fuzzy_tree::remove_tree(fuzzy_node *lisc)
{
	if (lisc != NULL)
	{
		for (int i = 0; i<lisc->cluster_count; i++)
			remove_tree(lisc->clusters[i]);
		delete lisc;
	}
}

void fuzzy_tree::remove_data()
{
	for (int i = 0; i < rows; i++)
		delete[] table[i];
	delete[]table;
}

void fuzzy_tree::build_tree()
{
	double lower_bounds[NMAX];
	double upper_bounds[NMAX];
	for (int i = 0; i < columns; i++)
		lower_bounds[i] = find_attribute_min(i);
	for (int i = 0; i < columns; i++)
		upper_bounds[i] = find_attribute_max(i);
	insert(lower_bounds, upper_bounds, root);
}

void fuzzy_tree::insert(double lower_bounds[NMAX], double upper_bounds[NMAX], fuzzy_node *root, int depth)
{
	if (depth > MAX_DEPTH)
	{
		root->cluster_count = -1;
		return;
	}
	depth++;
	int count = how_many_elements(lower_bounds, upper_bounds);
	if (count <= 1)
		root ->cluster_count = -1;
	else
	{
		root->attribute_id = random_attribute();
		double ** centers;
		centers = new double*[12];
		for (int i = 0; i < 12; i++)
			centers[i] = new double[12];
		double ** limits;
		limits = new double*[12];
		for (int i = 0; i < 12; i++)
			limits[i] = new double[12];
		double error[12];
		double delta[12];
		double delta2[12];
		error[1] = 1;
		for (int i = 1; i <= 11; i++)
		{
			error[i] = kMeans(i, root->attribute_id, lower_bounds, upper_bounds, centers, limits);
		}
		for (int i = 11; i >=1; i--)
			error[i] = error[i] / error[1];
		for (int i = 2; i <= 11; i++)
			delta[i] = error[i] - error[i - 1];
		for (int i = 3; i <= 11; i++)
			delta2[i] = delta[i] - delta[i - 1];
		double elbow_strength[11];
		for (int i = 2; i <= 10; i++)
			elbow_strength[i] = delta2[i + 1] - delta[i + 1];
		double max_strength = elbow_strength[2];
		int kmax = 2;
		for (int i = 3; i<=10; i++)
			if (max_strength < elbow_strength[i])
			{
				max_strength = elbow_strength[i];
				kmax = i;
			}
		
		root->cluster_count = kmax;
		root->cluster_centers = new double[kmax + 1];
		root->cluster_limits = new double[kmax + 1];
		for (int i = 0; i < root->cluster_count; i++)
		{
			root->cluster_centers[i] = centers[kmax][i];
			root->cluster_limits[i] = limits[kmax][i];
		}
		root->cluster_limits[root->cluster_count] = limits[kmax][kmax];
		for (int i = 0; i < columns; i++)
		{
			root->lower_bounds[i] = lower_bounds[i];
			root->upper_bounds[i] = upper_bounds[i];
		}
		
		double tmp_lower_bounds[NMAX];
		double tmp_upper_bounds[NMAX];
		root->clusters = new fuzzy_node*[kmax];
		for (int i = 0; i<root->cluster_count; i++)
		{
			for (int j = 0; j < columns; j++)
			{
				tmp_lower_bounds[j] = lower_bounds[j];
				tmp_upper_bounds[j] = upper_bounds[j];
			}
			tmp_lower_bounds[root->attribute_id] = limits[kmax][i];
			tmp_upper_bounds[root->attribute_id] = limits[kmax][i+1];
			root->clusters[i] = new fuzzy_node;
			insert(tmp_lower_bounds, tmp_upper_bounds, root->clusters[i], depth);
		}
	}
}

int fuzzy_tree::random_attribute()
{
	return rand() % columns;
}

double fuzzy_tree::find_attribute_min(int attribute_id)
{
	double min = table[0][attribute_id];
	for (int i = 1; i < rows; i++)
		if (table[i][attribute_id] < min)
			min = table[i][attribute_id];
	return min - 0.00000000001;
}

double fuzzy_tree::find_attribute_max(int attribute_id)
{
	double max = table[0][attribute_id];
	for (int i = 1; i < rows; i++)
		if (table[i][attribute_id] > max)
			max = table[i][attribute_id];
	return max + 0.00000000001;
}

int fuzzy_tree::how_many_elements(double lower_bounds[NMAX], double upper_bounds[NMAX])
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

double fuzzy_tree::traverse(double record[NMAX])
{
	return traverse(record, 0, root);
}

double fuzzy_tree::traverse(double record[NMAX], int depth, fuzzy_node *root)
{
	if (root ->cluster_count>0)
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
					double membership = 0;
					if (record[root->attribute_id] < root->cluster_centers[i])
						membership = 1 - abs(record[root->attribute_id] - root->cluster_centers[i]) / (abs(root->cluster_centers[i] - root->cluster_limits[i])+0.0000000001);
					else
						membership = 1 - abs(record[root->attribute_id] - root->cluster_centers[i]) / (abs(root->cluster_limits[i + 1] - root->cluster_centers[i])+0.0000000001);
					if (membership < 0)
						membership = 0;
					if (membership > 1)
						membership = 1;
					return membership + traverse(record, depth, root->clusters[i]);
				}
			}
		}
	}
	return 0;
}

double fuzzy_tree::kMeans(int K, int attribute_id, double lower_bounds[NMAX], double upper_bounds[NMAX], double **centers, double **limits)
{
	double *t = new double[rows];
	int n = 0;
	for (int i = 0; i < rows; i++)
	{
		bool found = true;
		for (int j = 0; j < columns; j++)
			if (table[i][j]<lower_bounds[j] || table[i][j]>upper_bounds[j])
				found = false;
		if (found)
		{
			t[n] = table[i][attribute_id];
			n++;
		}
	}
	//sorting
	for (int i = 0; i < n - 1; i++)
	{
		double min = t[i];
		int kmin = i;
		for (int k = i + 1; k < n; k++)
			if (t[k] < min)
			{
				min = t[k];
				kmin = k;
			}
		double tmp = t[i];
		t[i] = t[kmin];
		t[kmin] = tmp;
	}
	double *m = new double[K];
	double sum = 0;
	for (int i = 0; i < K; i++)
	{
		m[i] = (double)rand() / RAND_MAX;
		sum += m[i];
	}
	sum+= (double)rand() / RAND_MAX; 
	if (sum > 0)
	{
		double partial_sum = 0;
		for (int i = 0; i < K; i++)
		{
			partial_sum += m[i];
			m[i] = lower_bounds[attribute_id] + (partial_sum / sum)*(upper_bounds[attribute_id] - lower_bounds[attribute_id]);
		}
	}
	
	int *t_membership = new int[n];
	for (int iter = 0; iter < 100; iter++)
	{
		for (int i = 0; i < n; i++)
		{
			double min = abs(t[i] - m[0]);
			int kmin = 0;
			for (int j = 1; j < K; j++)
				if (abs(t[i] - m[j]) < min)
				{
					min = abs(t[i] - m[j]);
					kmin = j;
				}
			t_membership[i] = kmin;
		}
		for (int i = 0; i < K; i++)
		{
			double suma = 0;
			int counter = 0;
			for (int j = 0; j < n; j++)
				if (t_membership[j] == i)
				{
					suma += t[j];
					counter++;
				}
			if (counter != 0)
				m[i] = suma / counter;
		}
	}

	for (int i = 0; i < K; i++)
		centers[K][i] = m[i];
	limits[K][0] = lower_bounds[attribute_id];
	limits[K][K] = upper_bounds[attribute_id];
	
	for (int j = 0; j<n-1; j++)
		if (t_membership[j] - t_membership[j + 1] == -1)
				limits[K][t_membership[j+1]] = (t[j] + t[j + 1]) / 2.0;
	for (int j = 0; j < K; j++)
	{
		if (limits[K][j] > m[j])
			limits[K][j] = m[j];
		if (limits[K][j + 1] < m[j])
			limits[K][j + 1] = m[j];
	}
			
	double error = 0;
	for (int i = 0; i < n; i++)
		error += abs(t[i] - m[t_membership[i]]);
	delete[]t_membership;
	return error;
}