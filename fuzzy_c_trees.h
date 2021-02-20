#pragma once
#include <string>
#include "main.h"
using namespace std;

struct fuzzy_fcm_node
{
	int attribute_id;
	int number_of_clusters;
	double *cluster_centers;
	double *average_memberships_to_clusters;
	double *cluster_limits;
	double lower_bounds[NMAX];
	double upper_bounds[NMAX];
	fuzzy_fcm_node ** clusters;
};

class fuzzy_c_tree
{
public:
	fuzzy_c_tree(double r, double c, double ** tab);
	~fuzzy_c_tree();
	void remove_tree();
	void build_tree();
	void remove_data();
	double traverse(double record[NMAX]);

private:
	void remove_tree(fuzzy_fcm_node *leaf);
	void insert(double lower_bounds[NMAX], double upper_bounds[NMAX], fuzzy_fcm_node *leaf, int depth = 0);
	int random_attribute();
	double find_attribute_min(int attribute);
	double find_attribute_max(int attribute);
	int how_many_elements(double lower_bounds[NMAX], double upper_bounds[NMAX]);
	fuzzy_fcm_node *root;
	int rows;
	int columns;
	double **table;
	double traverse(double record[NMAX], int depth, fuzzy_fcm_node *root);
	double fcm(int i, int attribute_id, double lower_bounds[NMAX], double upper_bounds[NMAX], double **centers, double **limits, double ** averages);
};


fuzzy_c_tree::fuzzy_c_tree(double r, double c, double ** tab)
{
	root = new fuzzy_fcm_node;
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

fuzzy_c_tree::~fuzzy_c_tree()
{
	remove_tree();
}

void fuzzy_c_tree::remove_tree()
{
	remove_tree(root);
}

void fuzzy_c_tree::remove_tree(fuzzy_fcm_node *leaf)
{
	if (leaf != NULL)
	{
		for (int i = 0; i<leaf->number_of_clusters; i++)
			remove_tree(leaf->clusters[i]);
		delete leaf;
	}
}

void fuzzy_c_tree::remove_data()
{
	for (int i = 0; i < rows; i++)
		delete[] table[i];
	delete[]table;
}

void fuzzy_c_tree::build_tree()
{
	double lower_bounds[NMAX];
	double upper_bounds[NMAX];
	for (int i = 0; i < columns; i++)
		lower_bounds[i] = find_attribute_min(i);
	for (int i = 0; i < columns; i++)
		upper_bounds[i] = find_attribute_max(i);
	insert(lower_bounds, upper_bounds, root);
}

void fuzzy_c_tree::insert(double lower_bounds[NMAX], double upper_bounds[NMAX], fuzzy_fcm_node *root, int depth)
{
	if (depth > MAX_DEPTH)
	{
		root->number_of_clusters = -1;
		return;
	}
	depth++;
	int count = how_many_elements(lower_bounds, upper_bounds);
	if (count <= 1)
		root->number_of_clusters = -1;
	else
	{
		root->attribute_id = random_attribute();
		double ** centers;
		centers = new double*[12];
		for (int i = 0; i <= 12; i++)
			centers[i] = new double[12];
		double ** averages;
		averages = new double*[12];
		for (int i = 0; i <= 12; i++)
			averages[i] = new double[12];
		double ** limits;
		limits = new double*[12];
		for (int i = 0; i <= 12; i++)
			limits[i] = new double[12];
		double error[12];
		double delta[12];
		double delta2[12];
		error[1] = 1;
		for (int i = 2; i <= 11; i++)
		{
			error[i] = fcm(i, root->attribute_id, lower_bounds, upper_bounds, centers, limits, averages);
		}
		for (int i = 11; i >= 1; i--)
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
		for (int i = 3; i <= 10; i++)
			if (max_strength < elbow_strength[i])
			{
				max_strength = elbow_strength[i];
				kmax = i;
			}
		root->number_of_clusters = kmax;
		root->cluster_centers = new double[kmax + 1];
		root->average_memberships_to_clusters = new double[kmax + 1];
		root->cluster_limits = new double[kmax + 1];
		for (int i = 0; i < root->number_of_clusters; i++)
		{
			root->cluster_centers[i] = centers[kmax][i];
			root->average_memberships_to_clusters[i] = averages[kmax][i];
			root->cluster_limits[i] = limits[kmax][i];
		}
		root->cluster_limits[root->number_of_clusters] = limits[kmax][kmax];
		for (int i = 0; i < columns; i++)
		{
			root->lower_bounds[i] = lower_bounds[i];
			root->upper_bounds[i] = upper_bounds[i];
		}

		double tmp_lower_bounds[NMAX];
		double tmp_upper_bounds[NMAX];
		root->clusters = new fuzzy_fcm_node*[kmax];
		for (int i = 0; i<root->number_of_clusters; i++)
		{
			for (int j = 0; j < columns; j++)
			{
				tmp_lower_bounds[j] = lower_bounds[j];
				tmp_upper_bounds[j] = upper_bounds[j];
			}
			tmp_lower_bounds[root->attribute_id] = limits[kmax][i];
			tmp_upper_bounds[root->attribute_id] = limits[kmax][i + 1];
			root->clusters[i] = new fuzzy_fcm_node;
			insert(tmp_lower_bounds, tmp_upper_bounds, root->clusters[i], depth);
		}
		for (int i = 0; i < 12; i++)
			delete[] centers[i];
		delete[] centers;
		for (int i = 0; i < 12; i++)
			delete[] averages[i];
		delete[] averages;
		for (int i = 0; i < 12; i++)
			delete[] limits[i];
		delete[] limits;
	}
}

int fuzzy_c_tree::random_attribute()
{
	return rand() % columns;
}

double fuzzy_c_tree::find_attribute_min(int attribute_id)
{
	double min = table[0][attribute_id];
	for (int i = 1; i < rows; i++)
		if (table[i][attribute_id] < min)
			min = table[i][attribute_id];
	return min - 0.00000000001;
}

double fuzzy_c_tree::find_attribute_max(int attribute_id)
{
	double max = table[0][attribute_id];
	for (int i = 1; i < rows; i++)
		if (table[i][attribute_id] > max)
			max = table[i][attribute_id];
	return max + 0.00000000001;
}

int fuzzy_c_tree::how_many_elements(double lower_bounds[NMAX], double upper_bounds[NMAX])
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

double fuzzy_c_tree::traverse(double record[NMAX])
{
	return traverse(record, 0, root);
}

double fuzzy_c_tree::traverse(double record[NMAX], int depth, fuzzy_fcm_node *root)
{
	if (root->number_of_clusters>0)
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
			for (int i = 0; i < root->number_of_clusters; i++)
			{
				if (record[root->attribute_id] >= root->cluster_limits[i] && record[root->attribute_id] <= root->cluster_limits[i + 1])
				{
					return root->average_memberships_to_clusters[i] + traverse(record, depth, root->clusters[i]);
				}
			}
		}
	}
	return 0;
}

double fuzzy_c_tree::fcm(int K, int attribute_id, double lower_bounds[NMAX], double upper_bounds[NMAX], double **centers, double **limits, double **averages)
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

	double *c = new double[K];
	double sum = 0;

	double **Uk = new double*[n];
	for (int i = 0; i < n; i++)
		Uk[i] = new double[K];
	double **Uk1 = new double*[n];
	for (int i = 0; i < n; i++)
		Uk1[i] = new double[K];

	for (int i = 0; i<n; i++)
		for (int j = 0; j < K; j++)
		{
			Uk[i][j] = (double)rand() / RAND_MAX;
		}
	for (int i = 0; i < n; i++)
	{
		double s = 0;
		for (int j = 0; j < K; j++)
			s += Uk[i][j];
		for (int j = 0; j < K; j++)
		{
			Uk[i][j] /= s;
		}
	}

	for (int i = 0; i < n; i++)
		for (int j = 0; j < K; j++)
			Uk1[i][j] = Uk[i][j];
	
	double error = 0;
	for (int iter = 0; iter < 100; iter++)
	{
		for (int i = 0; i < n; i++)
			for (int j = 0; j < K; j++)
			{
				Uk[i][j] = Uk1[i][j];
			}
		for (int j = 0; j < K; j++)
		{
			double nominator = 0;
			for (int i = 0; i < n; i++)
				nominator += Uk[i][j] * Uk[i][j] * t[i];
			double denominator = 0;
			for (int i = 0; i < n; i++)
				denominator += Uk[i][j] * Uk[i][j];
			c[j] = nominator / denominator;
		}
		for (int i = 0; i<n; i++)
			for (int j = 0; j < K; j++)
			{
				sum = 0;
				for (int k = 0; k < K - 1; k++)
					if (abs(t[i] - c[k])>0.00000000001)
						sum += (abs(t[i] - c[j]) / abs(t[i] - c[k]))*(abs(t[i] - c[j]) / abs(t[i] - c[k]));
					else
						sum += 10000000000;
				if (sum > 0.00000000001)
					Uk1[i][j] = 1.0 / sum;
				else
					Uk1[i][j] = 10000000000;
			}
		for (int i = 0; i < n; i++)
		{
			double s = 0;
			for (int j = 0; j < K; j++)
				s += Uk1[i][j];
			for (int j = 0; j < K; j++)
			{
				Uk1[i][j] /= s;
			}
		}
		error = 0;
		for (int i = 0; i<n; i++)
			for (int j = 0; j < K; j++)
			{
				double tmp = abs(Uk[i][j] - Uk1[i][j]);
				if (tmp > error)
					error = tmp;
			}
	}

	for (int i = 0; i < K-1; i++)
	{
		double min = c[i];
		int kmin = i;
		for (int k = i + 1; k < K; k++)
			if (c[k] < min)
			{
				min = c[k];
				kmin = k;
			}
		double tmp = c[i];
		c[i] = c[kmin];
		c[kmin] = tmp;
		for (int j = 0; j < n; j++)
		{
			tmp = Uk1[j][i];
			Uk1[j][i] = Uk1[j][kmin];
			Uk1[j][kmin] = tmp;
		}
	}

	int *t_membership = new int[n];
	int *t_membership_second_place = new int[n];
	
	for (int i = 0; i < n; i++)
	{
		double max = Uk1[i][0];
		double max_2 = Uk[i][0];
		int jMax = 0;
		int jMax_2 = 0;
		for (int j = 1; j < K; j++)
			if (Uk1[i][j] > max)
			{
				max_2 = max;
				jMax_2 = jMax;
				max = Uk1[i][j];
				jMax = j;
			}
			else if ((Uk[i][j] <= max) && (Uk[i][j] > max_2))
			{
				max_2 = Uk[i][j];
				jMax_2 = j;
			}
		t_membership[i] = jMax;
		t_membership_second_place[i] = jMax_2;
	}

	for (int i = 0; i < K; i++)
		centers[K][i] = c[i];
	limits[K][0] = lower_bounds[attribute_id];
	limits[K][K] = upper_bounds[attribute_id];

	for (int j = 0; j<n - 1; j++)
		if (t_membership[j] - t_membership[j + 1] < 0) 
			limits[K][t_membership[j + 1]] = (t[j] + t[j + 1]) / 2.0;
	for (int j = 0; j < K; j++)
	{
		if (limits[K][j] > c[j])
			limits[K][j] = c[j];
		if (limits[K][j + 1] < c[j])
			limits[K][j + 1] = c[j];
	}

	for (int j = 0; j < K; j++)
	{
		double sum = 0;
		int counter = 0;
		for (int i = 0; i < n; i++)
		{
			double value_under_sum = 0;
			if (t_membership[i] == j)
			{
				value_under_sum += Uk1[i][j]*Uk1[i][j];
				int k = t_membership_second_place[i];
				if (Uk1[i][k]>0)
					value_under_sum = value_under_sum / Uk1[i][k];
				counter++;
				sum += value_under_sum;
			}
		}
		if (counter>1)
			averages[K][j] = sum*counter/n/n;
		else 
			averages[K][j] = 0;
	}

	delete[]t_membership;
	delete[]t_membership_second_place;
	for (int i = 0; i < n; i++)
		delete[]Uk[i];
	delete[]Uk;
	for (int i = 0; i < n; i++)
		delete[]Uk1[i];
	delete[]Uk1;
	delete[] t;
	delete[] c;
	return error;
}