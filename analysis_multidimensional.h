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
#include "main.h"
#include <filesystem>
using namespace std;

class analysis_multidimensional
{
public:
	analysis_multidimensional(string nazwa, string nazwa2, int opt);
	~analysis_multidimensional();
	int rows;
	int columns;
	double *tab_info;
	double *tab_wy;
	string filename_in;
	string filename_out;
	double max = 0;
	double min = 0;
	int opt = 1;
private:
	void read_data();
	void read_results();
	void analyze();
	vector<string> split(string s, string delimiter);
};

analysis_multidimensional::analysis_multidimensional(string name, string name2, int option)
{
	cout << "Building data table" << endl;
	ifstream file(name + "_info.txt");
	string line;
	file >> rows;
	file >> columns;
	
	columns--;
	file.close();
	filename_in = name;
	filename_out = name2;
	tab_info = new double[rows];
	for (int i = 0; i < rows; i++)
		tab_info[i] = 0;
	tab_wy = new double[rows];
	for (int i = 0; i < rows; i++)
		tab_wy[i] = 0;
	read_data();
	read_results();
	analyze();
	opt = option;
}

analysis_multidimensional::~analysis_multidimensional()
{
	cout << "Deleting data" << endl;
	delete[]tab_info;
	delete[]tab_wy;
}

void analysis_multidimensional::read_data()
{
	int i = 0;
	cout << "Reading data" << endl;
	ifstream file(filename_in + ".txt");
	string line;
	while (!file.eof())
	{
		getline(file, line);
		if (line.length() < 4)
			return;
		vector<string> v = split(line, ",");
		tab_info[i] = stof(v[columns]);
		i++;
	}
	file.close();
}

vector<string> analysis_multidimensional::split(string s, string delimiter) {
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

void analysis_multidimensional::read_results()
{
	int i = 0;
	cout << "Reading results" << endl;
	ifstream file(filename_out + ".txt");
	cout << filename_out << endl;
	string line;
	while (!file.eof())
	{
		getline(file, line);
		if (line.length() <=1 )
			continue;
		vector<string> v = split(line, ":");
		if (line[0] != 'm')
		{
			tab_wy[i] = stof(v[1]);
		}
		else if (line[1] == 'a' || line[1] == 'A')
			max = stof(v[1]);
		else if (line[1] == 'i' || line[1] == 'I')
			min = stof(v[1]);
		i++;
	}
	file.close();
}

void analysis_multidimensional::analyze()
{
	cout << "Result analysis" << endl;

	double sensitivity = 0;
	double specificity = 0;
	double precision = 0;
	double recall = 0;
	double accuracy = 0;

	double v_sensitivity = 0;
	double v_specificity = 0;
	double v_precision = 0;
	double v_recall = 0;
	double v_accuracy = 0;

	double value = 0;
	
	while (value < 0.99)
	{
		double tp = 0;
		double tn = 0;
		double fp = 0;
		double fn = 0;

		for (int i = 0; i < rows; i++)
		{
			if (opt > 0)  // All except isolation forest & n-ary if
			{
				double limit = min + (max - min)*value;
				
				if (tab_info[i] == 0.0 && tab_wy[i] > limit) //ok - nie an anomaly
					tn++;
				if (tab_info[i] == 0.0 && tab_wy[i] <= limit) //oops - not an anomaly
					fp++;
				if (tab_info[i] == 1.0 && tab_wy[i] <= limit) //ok - anomaly
					tp++;
				if (tab_info[i] == 1.0 && tab_wy[i] > limit) //oops - anomaly
					fn++;
			}
			if (opt == 0)  // 
			{
				double limit = max - (max - min)*value;
				if (tab_info[i] == 0.0 && tab_wy[i] < limit) //ok - not an anomaly
					tn++;
				if (tab_info[i] == 0.0 && tab_wy[i] >= limit) //oops - not an anomaly
					fp++;
				if (tab_info[i] == 1.0 && tab_wy[i] >= limit) //ok - anomaly
					tp++;
				if (tab_info[i] == 1.0 && tab_wy[i] < limit) //oops - anomaly
					fn++;
			}
		}
		
		double sen = tp / (tp + fn + 0.0000000000000001);
		double spe = tn / (tn + fp + 0.0000000000000001);
		double pre = tp / (tp + fp + 0.0000000000000001);
		double rec = tp / (tp + fn + 0.0000000000000001);
		double acc = (tp + tn) / (tp + tn + fp + fn + 0.0000000000000001);

		if (sen>sensitivity)
		{
			sensitivity = sen;
			v_sensitivity = value;
		}
		if (spe > specificity)
		{
			specificity = spe;
			v_sensitivity = value;
		}
		if (pre > precision)
		{
			precision = pre;
			v_precision = value;
		}
		if (rec > recall)
		{
			recall = rec;
			v_recall = value;
		}
		if (acc > accuracy)
		{
			accuracy = acc;
			v_accuracy = value;
		}
		value+=0.001;
	}

	ofstream file("final.txt", std::ofstream::out | std::ofstream::app);
	file << filename_out << "\t" << sensitivity << "\t" << v_sensitivity
		<< "\t" << specificity << "\t" << v_specificity 
		<< "\t" << precision << "\t" << v_precision 
		<< "\t" << recall << "\t" << v_recall
		<< "\t" << accuracy << "\t" << v_accuracy << endl;
	file.close();
}


void statistics_for_the_dataset(string _file)
{
	file_multi_dim *data_from_multi_dim = new file_multi_dim(_file);
	int rows = data_from_multi_dim->rows;
	int columns = data_from_multi_dim->columns;
	int anomaly_counter = 0;
	int non_anomaly_counter = 0;
	
	for (int w = 0; w < data_from_multi_dim->rows; w++)
	{
		double val = data_from_multi_dim->tab[w][columns - 1];
		if (val == 1.0)
			anomaly_counter++;
		else
			non_anomaly_counter++;
	}
	ofstream output_file("statistics.txt", std::ofstream::out | std::ofstream::app);
	output_file << _file << "\t" << rows << "\t" << columns << "\t" << anomaly_counter << "\t" << non_anomaly_counter << endl;
	output_file.close();
	data_from_multi_dim->~file_multi_dim();
}