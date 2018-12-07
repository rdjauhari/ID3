/* ID3 ALGORITHM (a subset)
   -input is in CSV format
		-one data item per row
		-attributes are separated by commas
		-last attribute is the classification (a string)
		-everything else are floating point numbers (in class, said doubles)
		-data items will always belong to one of the two classifications
   -start with a single node (contains every item in initial dataset)
   -split node by deciding split attribute and cutoff then add 2 children to parent node
		-data items under cutoff point will go left
		-data items over or equal will go right
   -for every attribute, consider every possible split within that attribute
		-compute the entropy gain for every split in every attribute
		-largest gain is the desired split attribute and value
		-closer to C4.5 algorithm
		-2 way splits only
   -if have leaf with only single type of item, stop the recursion
   -output ENTIRE tree
		-show each split point and every leaf
		-output should be easily understood by sight
		-show item counts and classifications for every leaf
*/

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <sstream>
#include <cmath>
#include <algorithm>

using namespace std;

//Globals
string file_name;
ifstream dataset_file;
bool T = true;
bool F = false;


void output_under_cutoff(double cutoff, double attribute, vector<string> current_node, string decision1, string decision2, int tab) {
	double item_count1 = 0.0;
	double item_count2 = 0.0;

	for (unsigned int i = 0; i < current_node.size(); i++) {
		if (current_node[i] == decision1) {
			item_count1++;
		}
		else {
			item_count2++;
		}
	}

	for (int t = 0; t < tab; t++)
	{
		cout << '\t';
	}

	cout << "Split based on attribute " << attribute + 1 << endl;

	for (int t = 0; t < tab; t++)
	{
		cout << '\t';
	}
	cout << "Left: Attributes < " << cutoff << endl;
	for (int t = 0; t < tab; t++)
	{
		cout << '\t';
	}
	cout << decision1 << "[" << item_count1 << "]" << endl;
	for (int t = 0; t < tab; t++)
	{
		cout << '\t';
	}
	cout << decision2 << "[" << item_count2 << "]" << endl << endl;
}

void output_over_or_equal_cutoff(double cutoff, double attribute, vector<string> current_node, string decision1, string decision2, int tab) {
	double item_count1 = 0.0;
	double item_count2 = 0.0;

	for (unsigned int i = 0; i < current_node.size(); i++) {
		if (current_node[i] == decision1) {
			item_count1++;
		}
		else {
			item_count2++;
		}
	}
	for (int t = 0; t < tab; t++)
	{
		cout << '\t';
	}
	cout << "Split based on attribute " << attribute + 1 << endl;
	for (int t = 0; t < tab; t++)
	{
		cout << '\t';
	}
	cout << "Right: Attributes >= " << cutoff << endl;
	for (int t = 0; t < tab; t++)
	{
		cout << '\t';
	}
	cout << decision1 << "[" << item_count1 << "]" << endl;
	for (int t = 0; t < tab; t++)
	{
		cout << '\t';
	}
	cout << decision2 << "[" << item_count2 << "]" << endl << endl;
}

bool check(vector<string> decision, string decision1, string decision2) {
	double counter1 = 0.0;
	double counter2 = 0.0;

	for (unsigned int i = 0; i < decision.size(); i++) {
		if (decision[i] == decision1) {
			counter1++;
		}
		else {
			counter2++;
		}
	}
	
	if (counter1 == decision.size() || counter2 == decision.size()) {
		return T;
	}
	else {
		return F;
	}
	
}

double entropy_calculation(vector<string> current_node) {
	string string1 = current_node[0];
	string string2;
	double decision1 = 0.0;
	double decision2 = 0.0;
	double num_decisions = 0.0;
	double entropy = 0;

	//will only be 2 classifications -- count how many of each
	for (unsigned int i = 0; i < current_node.size(); i++) {
		if (current_node[i] == string1) {
			decision1++;
		}
		else {
			string2 = current_node[i];
			decision2++;
		}
	}
	num_decisions = decision1 + decision2; //total number of decisions

	//log2(0) is 0
	if (decision1 == 0.0 && decision2 != 0.0) {
		entropy = -(decision2 / num_decisions)*log2(decision2 / num_decisions);
	}
	else if (decision2 == 0.0 && decision1 != 0.0) {
		entropy = -(decision1 / num_decisions)*log2(decision1 / num_decisions);
	}
	else if (decision2 == 0.0 && decision1 == 0.0) {
		entropy = 0;
	}
	else {
		entropy = -(decision1 / num_decisions)*log2(decision1 / num_decisions) - (decision2 / num_decisions)*log2(decision2 / num_decisions);
	}

	return entropy;
}

double gain_calculation(double proportion1, double proportion2, double entropy1, double entropy2, double entropy_decision) {
	double gain;

	gain = entropy_decision - (proportion1*entropy1) - (proportion2*entropy2);

	return gain;
}

void split(vector<vector<double>> attributes, double num_att, vector<string> decision, double entropy_data, string decision1, string decision2,int tab) {
	//create a vector for different gain values and decision
	vector<double> gains;
	vector<double> max_gains;
	vector<double>::iterator gain_iterator;
	double gain_index = 0.0;
	vector<double> max_gain_index;
	vector<string> split_under_vec;
	vector<string> split_over_equal_vec;
	
	//find cutoff
	double split_total = 0.0;
	double high_gain = 0.0;

	for (unsigned int att = 0; att < attributes.size() /*number of different types of attributes*/; att++) {

		gains.clear();

		for (unsigned int cut = 0; cut < num_att /*number of attributes in a column*/; cut++) {

			int split_under = 0;
			int split_over_equal = 0;
			split_under_vec.clear();
			split_over_equal_vec.clear();

			double temp_cutoff = attributes[att][cut];

			for (unsigned int i = 0; i < num_att; i++) {
				if (attributes[att][i] < temp_cutoff) {
					split_under_vec.push_back(decision[i]);
					split_under++;
				}
				else if (attributes[att][i] >= temp_cutoff) {
					split_over_equal_vec.push_back(decision[i]);
					split_over_equal++;
				}
			}
			split_total = split_under + split_over_equal;

			//calculate entropies and gain
			double proportion1 = split_under / split_total;
			double proportion2 = split_over_equal / split_total;
			double entropy_under = 0.0;
			double entropy_over_equal = 0.0;

			if (split_under != 0.0 && split_over_equal == 0.0) {
				entropy_under = entropy_calculation(split_under_vec);
			}
			else if (split_over_equal != 0.0 && split_under == 0.0) {
				entropy_over_equal = entropy_calculation(split_over_equal_vec);
			}
			else if (split_under != 0.0 && split_over_equal != 0.0) {
				entropy_under = entropy_calculation(split_under_vec);
				entropy_over_equal = entropy_calculation(split_over_equal_vec);
			}

			gains.push_back(gain_calculation(proportion1, proportion2, entropy_under, entropy_over_equal, entropy_data));

			//maximum gain
			high_gain = gains[0];
			for (unsigned int i = 1; i < gains.size(); i++) {
				if (gains[i] > high_gain) {
					high_gain = gains[i];
				}
				else {
					high_gain = high_gain;
				}
			}
		}
		max_gains.push_back(high_gain);
		gain_iterator = max_element(gains.begin(), gains.end());
		gain_index = distance(gains.begin(), gain_iterator);
		max_gain_index.push_back(gain_index); //indices to later obtain the value of the highest gain
	}

	//split the node according to maximum gain
	vector<double>::iterator split_att_iterator = max_element(max_gains.begin(), max_gains.end());

	//attribute the highest gain is in
	double split_att = distance(max_gains.begin(), split_att_iterator); 

	double split_val = attributes[split_att][max_gain_index[split_att]];

	//2 sets of vectors to be able to replace and recurse
	vector<vector<double>> under_vec;
	vector<vector<double>> over_equal_vec;
	vector<double> temp_u;
	vector<double> temp_oe;
	vector<string> under;
	vector<string> over_equal;

	vector<vector<double>> under_vec_store;
	vector<vector<double>> over_equal_vec_store;
	vector<string> under_store;
	vector<string> over_equal_store;

	for (unsigned int j = 0; j < attributes.size() /*number of different types of attributes*/; j++) {
		temp_u.clear();
		temp_oe.clear();
		under.clear();
		over_equal.clear();
		for (unsigned int i = 0; i < num_att /*number of attributes in a column*/; i++) {
			if (attributes[split_att][i] < split_val) {
				under.push_back(decision[i]);
				temp_u.push_back(attributes[j][i]);
			}
			else if (attributes[split_att][i] >= split_val) {
				over_equal.push_back(decision[i]);
				temp_oe.push_back(attributes[j][i]);
			}
		}
		under_vec.push_back(vector<double>(temp_u));
		over_equal_vec.push_back(vector<double>(temp_oe));
	}
	
	//storing
	under_vec_store.clear();
	over_equal_vec_store.clear();
	under_store.clear();
	over_equal_store.clear();

	for (unsigned int i = 0; i < under_vec.size(); i++) {
		under_vec_store.push_back(under_vec[i]);
	}

	for (unsigned int i = 0; i < over_equal_vec.size(); i++) {
		over_equal_vec_store.push_back(over_equal_vec[i]);
	}

	for (unsigned int i = 0; i < under.size(); i++) {
		under_store.push_back(under[i]);
	}
	
	for (unsigned int i = 0; i < over_equal.size(); i++) {
		over_equal_store.push_back(over_equal[i]);
	}

	//check if the node is only filled with one decision
	//left side
	if (check(under, decision1, decision2) == T) {
		output_under_cutoff(split_val, split_att, under, decision1, decision2 , tab);
	}
	else if (check(under, decision1, decision2) == F) {
		output_under_cutoff(split_val, split_att, under, decision1, decision2, tab);
		
		//clearing before re-splitting
		under_vec.clear();
		under.clear();

		split(under_vec_store, under_store.size(), under_store, entropy_data, decision1, decision2, tab+1);
	}
	//right side
	if (check(over_equal, decision1, decision2) == T) {
		output_over_or_equal_cutoff(split_val, split_att, over_equal, decision1, decision2, tab);
	}
	else if (check(over_equal, decision1, decision2) == F) {
		output_over_or_equal_cutoff(split_val, split_att, over_equal, decision1, decision2, tab);

		//clearing before re-splitting
		over_equal_vec.clear();
		over_equal.clear();

		split(over_equal_vec_store, over_equal_store.size(), over_equal_store, entropy_data, decision1, decision2, tab+1);
	}
}

int main(int argc, char** argv) {
	file_name = argv[1];

	//opening file
	dataset_file.open(file_name);

	//if file fails to open
	if (dataset_file.fail()) {
		cerr << "File failed to open.";
		return 1;
	}
	
	//if input file is empty
	if (dataset_file.eof()) {
		cerr << "File contains no data.";
		return 2;
	}

	//creating 2D vector for attributes
	vector<vector<double>> attributes;

	string temp_line = { "\0" };
	string temp_comma = { "\0" };
	double hold_var;
	vector<double> temp_vec;
	vector<string> decision;

	//reading data into temporary string variable and ignoring commas
	while (!dataset_file.eof() && getline(dataset_file, temp_line)) {
		
		//clearing temporary vector for new values
		temp_vec.clear();

		stringstream iss(temp_line);

		while (getline(iss, temp_comma, ',')) {
			stringstream iss1(temp_comma);
			
			if (!(iss1 >> hold_var)) {

				//pushback the decision/classification into a separate 1d string vector
				decision.push_back(temp_comma); //only pushes back what could not be read in the stringstream
			}
			else {

				//using stringstream to read the data from string to a double variable
				iss1 >> hold_var;

				//pushback into a 1d double vector
				temp_vec.push_back(hold_var);
			}
		}

		//push 1d vector into 2d vector
		attributes.push_back(vector<double>(temp_vec));
	}

	//calculating entropy to determine cut-off attribute to split parent node into 2 children nodes by comparing gains
	//entropy is the measure of the amount of uncertainty in the data set
	//gain is the difference in entropies before and after split (need largest information gain)

	//entropy of dataset
	double entropy_decision = 0.0;

	entropy_decision = entropy_calculation(decision);

	//rearranging vectors by attribute
	vector<double> temp;
	vector<vector<double>> attributes1;

	for (unsigned int j = 0; j < temp_vec.size(); j++) {

		temp.clear();

		for (unsigned int i = 0; i < attributes.size(); i++)
		{
			temp.push_back(attributes[i][j]);
		}
		attributes1.push_back(vector<double>(temp));
	}

	//name of decisions
	string string1 = decision[0];
	string string2;

	for (unsigned int i = 0; i < decision.size(); i++) {
		if (decision[i] == string1) {
		}
		else {
			string2 = decision[i];
		}
	}
	
	//splitting nodes
	split(attributes1, attributes.size(), decision, entropy_decision, string1, string2,0);

	return 0;
}