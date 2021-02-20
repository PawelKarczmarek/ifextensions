#pragma once
#pragma once
#include <string>
#include <fstream>
#include <iostream>
#include <vector>
#include "binary_trees.h"
#include "fuzzy_trees.h"
#include "n_ary_trees.h"
#include "binary_trees_with_center.h"
#include "fuzzy_trees_2_nodes.h"
#include "fuzzy_c_trees.h"
#include "main.h"
using namespace std;

class file_multi_dim
{
public:
	file_multi_dim(string nazwa);
	~file_multi_dim();
	void present();
	void data_portion(double ** t);
	int rows;
	int columns;
	double **tab;
	string file_name;
	void return_lower_and_upper_bounds(double *l, double *u);
private:
	void read();
	vector<string> split(string s, string delimiter);
};

file_multi_dim::file_multi_dim(string name)
{
	cout << "Data table creation" << endl;
	ifstream input_file(name + "_info.txt");
	string linia;
	input_file >> rows;
	input_file >> columns;
	columns--;  //THE LAST ATTRIBUT IS AN INFORMATION: ABNORMAL (1) OR NORMAL (0)
	input_file.close();
	file_name = name;
	tab = new double*[rows];
	for (int i = 0; i < rows; i++)
		tab[i] = new double[columns];
	for (int i = 0; i < rows; i++)
		for (int j = 0; j < columns; j++)
			tab[i][j] = 0;
	read();
}

file_multi_dim::~file_multi_dim()
{
	cout << "Deleting data" << endl;
	for (int i = 0; i < rows; i++)
		delete[]tab[i];
	delete[]tab;
}

void file_multi_dim::read()
{
	int i = 0;
	cout << "Reading data" << endl;
	ifstream input_file(file_name + ".txt");
	string line;
	while (!input_file.eof())
	{
		getline(input_file, line);
		if (line.length() < 5)
			return;
		vector<string> v = split(line, ",");
		for (int j = 0; j < columns; j++)
		{
			tab[i][j] = stof(v[j]);
		}
		i++;
	}
	input_file.close();
}


void file_multi_dim::present()
{
	for (int i = 0; i < rows; i++)
	{
		for (int j = 0; j < columns; j++)
			cout << tab[i][j] << "\t";
		cout << endl;
	}
}

vector<string> file_multi_dim::split(string s, string delimiter) {
	size_t pos_start = 0, pos_end, delim_len = delimiter.length();
	string token;
	vector<string> res;

	while ((pos_end = s.find(delimiter, pos_start)) != string::npos) {
		token = s.substr(pos_start, pos_end - pos_start);
		pos_start = pos_end + delim_len;
		res.push_back(token);
	}

	res.push_back(s.substr(pos_start));
	return res;
}

void file_multi_dim::data_portion(double ** t)
{
	for (int i = 0; i < 256; i++)
	{
		int index = rand() % rows;
		for (int j = 0; j < columns; j++)
		{
			t[i][j] = tab[index][j];
		}
	}
}

void file_multi_dim::return_lower_and_upper_bounds(double *d, double *g)
{
	for (int k = 0; k < columns; k++)
	{
		double min = tab[0][k];
		double max = tab[0][k];
		for (int w = 1; w < rows; w++)
		{
			if (tab[w][k] < min)
				min = tab[w][k];
			if (tab[w][k] > max)
				max = tab[w][k];
		}
		d[k] = min - 0.0000000001;
		g[k] = max + 0.0000000001;
	}
}

void isolation_forest_for_multi_dim(string _file)
{
	cout << "ISOLATION FOREST";
	file_multi_dim *data_from_multi_dim = new file_multi_dim(_file);
	double ** tab = new double *[256];
	for (int i = 0; i < 256; i++)
		tab[i] = new double[data_from_multi_dim->columns];
	double * results = new double[data_from_multi_dim->rows];
	for (int i = 0; i < data_from_multi_dim->rows; i++)
		results[i] = 0;
	double * record = new double[data_from_multi_dim->columns];
	const clock_t begin_time = clock();
	for (int i = 0; i < 100; i++)
	{
		cout << "TREE NUMBER " << i << endl;
		data_from_multi_dim->data_portion(tab);
		binary_tree **db = new binary_tree*[100];
		db[i] = new binary_tree(256, data_from_multi_dim->columns, tab);
		db[i]->build_tree();
		for (int j = 0; j < data_from_multi_dim->rows; j++)
		{
			for (int k = 0; k < data_from_multi_dim->columns; k++)
				record[k] = data_from_multi_dim->tab[j][k];
			results[j] += db[i]->traverse(record) / 100.0;
		}
		db[i]->remove_tree();
	}
	for (int i = 0; i < data_from_multi_dim->rows; i++)
	{
		results[i] = pow(2.0, -results[i] / c_function(256));
	}

	ofstream output_file(data_from_multi_dim->file_name + "_results_if.txt");
	double min = results[0];
	double max = results[0];
	for (int i = 0; i < data_from_multi_dim->rows; i++)
	{
		output_file << i << ":" << results[i] << endl;
		if (results[i] < min)
			min = results[i];
		if (results[i] > max)
			max = results[i];
	}
	output_file << endl << endl << "min:" << min << endl << "max:" << max;
	output_file.close();

	for (int i = 0; i < 256; i++)
		delete[]tab[i];
	delete[]tab;

	data_from_multi_dim->~file_multi_dim();
}

void n_ary_isolation_forest_for_multi_dim(string _file)
{
	cout << "3-ARY ISOLATION FOREST";
	double how_many_n_ary = 3;
	file_multi_dim *data_from_multi_dim = new file_multi_dim(_file);

	double ** tab = new double *[256];
	for (int i = 0; i < 256; i++)
		tab[i] = new double[data_from_multi_dim->columns];

	double * results = new double[data_from_multi_dim->rows];
	for (int i = 0; i < data_from_multi_dim->rows; i++)
		results[i] = 0;
	double * record = new double[data_from_multi_dim->columns];

	const clock_t begin_time = clock();

	for (int i = 0; i < 100; i++)
	{
		cout << "TREE NUMBER " << i << endl;
		data_from_multi_dim->data_portion(tab);
		n_ary_tree **db = new n_ary_tree*[100];
		db[i] = new n_ary_tree(256, data_from_multi_dim->columns, tab, how_many_n_ary);
		db[i]->build_tree();
		for (int j = 0; j < data_from_multi_dim->rows; j++)
		{
			for (int k = 0; k < data_from_multi_dim->columns; k++)
				record[k] = data_from_multi_dim->tab[j][k];
			results[j] += db[i]->traverse(record) / 100.0;
		}
		db[i]->remove_tree();
	}
	for (int i = 0; i < data_from_multi_dim->rows; i++)
	{
		results[i] = pow(how_many_n_ary, -results[i] / c_function(256, how_many_n_ary));
	}

	ofstream output_file(data_from_multi_dim->file_name + "_results_nif.txt");
	double min = results[0];
	double max = results[0];
	for (int i = 0; i < data_from_multi_dim->rows; i++)
	{
		output_file << i << ":" << results[i] << endl;
		if (results[i] < min)
			min = results[i];
		if (results[i] > max)
			max = results[i];
	}
	output_file << endl << endl << "min:" << min << endl << "max:" << max;
	output_file.close();

	for (int i = 0; i < 256; i++)
		delete[]tab[i];
	delete[]tab;

	data_from_multi_dim->~file_multi_dim();
}

void fuzzy_i_forest_for_multi_dim(string _file)
{
	cout << "K-MEANS-BASED ISOLATION FOREST";
	file_multi_dim *data_from_multi_dim = new file_multi_dim(_file);

	double ** tab = new double *[256];
	for (int i = 0; i < 256; i++)
		tab[i] = new double[data_from_multi_dim->columns];

	double * results = new double[data_from_multi_dim->rows];
	for (int i = 0; i < data_from_multi_dim->rows; i++)
		results[i] = 0;
	double * record = new double[data_from_multi_dim->columns];

	const clock_t begin_time = clock();

	for (int i = 0; i < 100; i++)
	{
		cout << "TREE NUMBER " << i << endl;
		data_from_multi_dim->data_portion(tab);
		fuzzy_tree **db = new fuzzy_tree*[100];
		db[i] = new fuzzy_tree(256, data_from_multi_dim->columns, tab);
		db[i]->build_tree();
		for (int j = 0; j < data_from_multi_dim->rows; j++)
		{
			for (int k = 0; k < data_from_multi_dim->columns; k++)
				record[k] = data_from_multi_dim->tab[j][k];
			results[j] = results[j] + db[i]->traverse(record) / 100.0;
		}
		db[i]->remove_tree();
	}

	ofstream output_file(data_from_multi_dim->file_name + "_results_kif.txt");
	double min = results[0];
	double max = results[0];
	for (int i = 0; i < data_from_multi_dim->rows; i++)
	{
		output_file << i << ":" << results[i] << endl;
		if (results[i] < min)
			min = results[i];
		if (results[i] > max)
			max = results[i];
	}
	output_file << endl << endl << "min:" << min << endl << "max:" << max;
	output_file.close();

	for (int i = 0; i < 256; i++)
		delete[]tab[i];
	delete[]tab;

	data_from_multi_dim->~file_multi_dim();
}

void fuzzy_c_forest_for_multi_dim(string _file)
{
	cout << "FUZZY C-MEANS-BASED ISOLATION FOREST";
	file_multi_dim *data_from_multi_dim = new file_multi_dim(_file);

	double ** tab = new double *[256];
	for (int i = 0; i < 256; i++)
		tab[i] = new double[data_from_multi_dim->columns];

	double * results = new double[data_from_multi_dim->rows];
	for (int i = 0; i < data_from_multi_dim->rows; i++)
		results[i] = 0;
	double * record = new double[data_from_multi_dim->columns];

	const clock_t begin_time = clock();

	for (int i = 0; i < 100; i++)
	{
		cout << "TREE NUMBER " << i << endl;
		data_from_multi_dim->data_portion(tab);
		fuzzy_c_tree **db = new fuzzy_c_tree*[100];
		db[i] = new fuzzy_c_tree(256, data_from_multi_dim->columns, tab);
		db[i]->build_tree();
		for (int j = 0; j < data_from_multi_dim->rows; j++)
		{
			for (int k = 0; k < data_from_multi_dim->columns; k++)
				record[k] = data_from_multi_dim->tab[j][k];
			results[j] = results[j] + db[i]->traverse(record) / 100.0;
		}
		db[i]->remove_tree();
	}

	ofstream output_file(data_from_multi_dim->file_name + "_results_cif.txt");
	double min = results[0];
	double max = results[0];
	for (int i = 0; i < data_from_multi_dim->rows; i++)
	{
		output_file << i << ":" << results[i] << endl;
		if (results[i] < min)
			min = results[i];
		if (results[i] > max)
			max = results[i];
	}
	output_file << endl << endl << "min:" << min << endl << "max:" << max;
	output_file.close();

	for (int i = 0; i < 256; i++)
		delete[]tab[i];
	delete[]tab;

	data_from_multi_dim->~file_multi_dim();
}

void memb_i_forest_for_multi_dim(string _file)
{
	cout << "FUZZY SET-BASED ISOLATION FOREST";
	file_multi_dim *data_from_multi_dim = new file_multi_dim(_file);
	double *low_bounds = new double[data_from_multi_dim->columns];
	double *upp_bounds = new double[data_from_multi_dim->columns];
	data_from_multi_dim->return_lower_and_upper_bounds(low_bounds, upp_bounds);
	double ** tab = new double *[256];
	for (int i = 0; i < 256; i++)
		tab[i] = new double[data_from_multi_dim->columns];

	double * results = new double[data_from_multi_dim->rows];
	for (int i = 0; i < data_from_multi_dim->rows; i++)
		results[i] = 0;
	double * record = new double[data_from_multi_dim->columns];

	const clock_t begin_time = clock();

	for (int i = 0; i < 100; i++)
	{
		cout << "TREE NUMBER " << i << endl;
		data_from_multi_dim->data_portion(tab);
		binary_tree_with_center **db = new binary_tree_with_center*[100];
		db[i] = new binary_tree_with_center(256, data_from_multi_dim->columns, tab, low_bounds, upp_bounds);
		db[i]->build_tree();
		for (int j = 0; j < data_from_multi_dim->rows; j++)
		{
			for (int k = 0; k < data_from_multi_dim->columns; k++)
				record[k] = data_from_multi_dim->tab[j][k];
			results[j] = results[j] + db[i]->traverse(record) / 100.0;
		}
		db[i]->remove_tree();
	}

	ofstream output_file(data_from_multi_dim->file_name + "_results_mif.txt");
	double min = results[0];
	double max = results[0];
	for (int i = 0; i < data_from_multi_dim->rows; i++)
	{
		output_file << i << ":" << results[i] << endl;
		if (results[i] < min)
			min = results[i];
		if (results[i] > max)
			max = results[i];
	}
	output_file << endl << endl << "min:" << min << endl << "max:" << max;
	output_file.close();

	for (int i = 0; i < 256; i++)
		delete[]tab[i];
	delete[]tab;

	data_from_multi_dim->~file_multi_dim();
}

void fuzzy_c2_forest_for_multi_dim(string _file)
{
	cout << "FUZZY C-MEANS-BASED ISOLATION FOREST - 2 NODES";
	file_multi_dim *data_from_multi_dim = new file_multi_dim(_file);

	double ** tab = new double *[256];
	for (int i = 0; i < 256; i++)
		tab[i] = new double[data_from_multi_dim->columns];

	double * results = new double[data_from_multi_dim->rows];
	for (int i = 0; i < data_from_multi_dim->rows; i++)
		results[i] = 0;
	double * record = new double[data_from_multi_dim->columns];

	const clock_t begin_time = clock();

	for (int i = 0; i < 100; i++)
	{
		cout << "TREE NUMBER " << i << endl;
		data_from_multi_dim->data_portion(tab);
		fuzzy_c_tree_2_nodes **db = new fuzzy_c_tree_2_nodes*[100];
		db[i] = new fuzzy_c_tree_2_nodes(256, data_from_multi_dim->columns, tab);
		db[i]->build_tree();
		for (int j = 0; j < data_from_multi_dim->rows; j++)
		{
			for (int k = 0; k < data_from_multi_dim->columns; k++)
				record[k] = data_from_multi_dim->tab[j][k];
			results[j] = results[j] + db[i]->traverse(record) / 100.0;
		}
		db[i]->remove_tree();
	}

	ofstream output_file(data_from_multi_dim->file_name + "_results_c2if.txt");
	double min = results[0];
	double max = results[0];
	for (int i = 0; i < data_from_multi_dim->rows; i++)
	{
		output_file << i << ":" << results[i] << endl;
		if (results[i] < min)
			min = results[i];
		if (results[i] > max)
			max = results[i];
	}
	output_file << endl << endl << "min:" << min << endl << "max:" << max;
	output_file.close();

	for (int i = 0; i < 256; i++)
		delete[]tab[i];
	delete[]tab;

	data_from_multi_dim->~file_multi_dim();
}

void fuzzy_c2_forest_for_multi_dim_variant_with_distance(string _file)
{
	cout << "FUZZY C-MEANS-BASED ISOLATION FOREST - 2 NODES AND DISTANCE";
	file_multi_dim *data_from_multi_dim = new file_multi_dim(_file);

	double ** tab = new double *[256];
	for (int i = 0; i < 256; i++)
		tab[i] = new double[data_from_multi_dim->columns];

	double * results = new double[data_from_multi_dim->rows];
	for (int i = 0; i < data_from_multi_dim->rows; i++)
		results[i] = 0;
	double * record = new double[data_from_multi_dim->columns];

	const clock_t begin_time = clock();

	for (int i = 0; i < 100; i++)
	{
		cout << "TREE NUMBER " << i << endl;
		data_from_multi_dim->data_portion(tab);
		fuzzy_c_tree_2_nodes **db = new fuzzy_c_tree_2_nodes*[100];
		db[i] = new fuzzy_c_tree_2_nodes(256, data_from_multi_dim->columns, tab);
		db[i]->build_tree();
		for (int j = 0; j < data_from_multi_dim->rows; j++)
		{
			for (int k = 0; k < data_from_multi_dim->columns; k++)
				record[k] = data_from_multi_dim->tab[j][k];
			results[j] = results[j] + db[i]->traverse2(record) / 100.0;
		}
		db[i]->remove_tree();
	}

	ofstream output_file(data_from_multi_dim->file_name + "_results_c2dif.txt");
	double min = results[0];
	double max = results[0];
	for (int i = 0; i < data_from_multi_dim->rows; i++)
	{
		output_file << i << ":" << results[i] << endl;
		if (results[i] < min)
			min = results[i];
		if (results[i] > max)
			max = results[i];
	}
	output_file << endl << endl << "min:" << min << endl << "max:" << max;
	output_file.close();

	for (int i = 0; i < 256; i++)
		delete[]tab[i];
	delete[]tab;

	data_from_multi_dim->~file_multi_dim();
}
