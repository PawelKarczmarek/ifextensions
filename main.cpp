#include "main.h"
#include <filesystem>
#include <iostream>
#include <string>
#include <vector>
#include <fstream>
using namespace std;

vector<string> split(string s, string delimiter) {
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

int main(int argc, char *argv)
{
	srand(time(0));
	
	string * tab = new string[100];
	tab[0] = "annthyroid";
	tab[1] = "arrhythmia";
	tab[2] = "breastw";
	tab[3] = "cardio";
	tab[4] = "cover";
	tab[6] = "glass";
	tab[7] = "ionosphere";
	tab[8] = "letter";
	tab[9] = "lympho";
	tab[10] = "mammography";
	tab[11] = "mnist";
	tab[12] = "musk";
	tab[13] = "optdigits";
	tab[14] = "pendigits";
	tab[15] = "pima";
	tab[16] = "satellite";
	tab[18] = "satimage-2";
	tab[19] = "shuttle";
	tab[20] = "speech";
	tab[21] = "thyroid";
	tab[22] = "vertebral";
	tab[23] = "vowels";
	tab[24] = "wine";
	tab[25] = "creditcard";
	tab[26] = "ds2ostraffictraces";
	tab[27] = "nad";
	tab[28] = "unsw0";
	tab[29] = "unsw1";
	tab[30] = "bitcoin";
	tab[31] = "zbior2";
	tab[32] = "position1";
	for (int iteracja = 0; iteracja<100; iteracja++)
		for (int i = 0; i < 32; i++)
		if (i!=5 && i!=17)
		{
			isolation_forest_for_multi_dim("C:\\data\\" + tab[i]);
			n_ary_isolation_forest_for_multi_dim("C:\\data\\" + tab[i]);
			fuzzy_i_forest_for_multi_dim("C:\\data\\" + tab[i]);
			fuzzy_c_forest_for_multi_dim("C:\\data\\" + tab[i]);
			memb_i_forest_for_multi_dim("C:\\data\\" + tab[i]);
			fuzzy_c2_forest_for_multi_dim("C:\\data\\" + tab[i]); 
			fuzzy_c2_forest_for_multi_dim_variant_with_distance("C:\\data\\" + tab[i]);
			
			analysis_multidimensional("C:\\data\\" + tab[i], "C:\\data\\" + tab[i] + "_results_if", false);
			analysis_multidimensional("C:\\data\\" + tab[i], "C:\\data\\" + tab[i] + "_results_kif", true);
			analysis_multidimensional("C:\\data\\" + tab[i], "C:\\data\\" + tab[i] + "_results_mif", true);
			analysis_multidimensional("C:\\data\\" + tab[i], "C:\\data\\" + tab[i] + "_results_nif", false);
			analysis_multidimensional("C:\\data\\" + tab[i], "C:\\data\\" + tab[i] + "_results_cif", true);
			analysis_multidimensional("C:\\data\\" + tab[i], "C:\\data\\" + tab[i] + "_results_c2if", true);
			analysis_multidimensional("C:\\data\\" + tab[i], "C:\\data\\" + tab[i] + "_results_c2dif", true);
		}
}

