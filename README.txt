This solution is published only for scientific purposes. Author of the solution is not responsible for the errors caused by using it.

Data input: Two files: datasetname.txt and datasetname_info.txt.
The former contains data records put in rows. The last column is to mark the record as abnomal (1)/normal (0).
The last file is to write number of rows (in its firs row) and number of lines (in its second row). For example:

abc.txt:
1,2,3,4,0
0.1,2,3,4.1,1
0,1,2,3,0

abc_info.txt:
3
5

abc_results_method_abbreviation.txt:
0:0.408623
1:0.393042
2:0.37438
(these are anomaly scores for the corresponding lines of abc.txt).

The names of files should be placed in main.cpp file and it should be rebuilt according to the dataset number and recompiled.