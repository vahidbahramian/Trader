#include <vector>
#include <stdio.h>
#include <stdlib.h>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <cstring>
#include <time.h>
#include <chrono>
#include <thread>
#include <algorithm>
#include <numeric>
#include <iomanip>
#include <windows.h>

bool read_from_excel_file(std::string file,int numberOfColumn, std::vector<std::vector<double>>& cell, std::vector<std::string>& columnTitle)
{
	std::string line;
	std::ifstream infile(file);
	if (!infile.is_open())
		return false;
	std::vector<double> temp_row;
	std::getline(infile, line, '\n');
	std::istringstream title(line);
	std::string buff;
	for (int i = 0; i < numberOfColumn-1; i++)
	{
		title >> buff;
		columnTitle.push_back(buff);
	}

	while (std::getline(infile, line, '\n'))
	{
		std::istringstream buffer(line);
		for (int i = 0; i < numberOfColumn; i++)
		{
			buffer >> buff;
			if (i > 2)
				temp_row.push_back(std::stod(buff));
		}
		cell.push_back(temp_row);
		temp_row.clear();
	}
	return true;
}
std::vector<std::vector<int>> count_one_in_each_column(const std::vector<std::vector<double>>& data, int begin, int end)
{
	std::vector<int> temp;
	std::vector<std::vector<int>> data_out;
	for (int i = begin; i <= end; i++)
	{
		for (int j = 0; j < data.size(); j++)
			if (data[j][i] == 1)
			{
				temp.push_back(j);
			}
		data_out.push_back(temp);
		temp.clear();
	}
	return data_out;
}
std::vector<std::vector<int>> count_if_greater_than_i_for_each_column(const std::vector<std::vector<double>>& data,
	const std::vector<std::vector<int>>& index_of_data, float factor, int j)
{
	std::vector<int> temp;
	temp.clear();
	std::vector<std::vector<int>> data_out;
	for (int i = 0; i < index_of_data.size(); i++)
	{
		for(int k : index_of_data[i])
			if (data[k][j] >= factor)
			{
				temp.push_back(k);
			}
		data_out.push_back(temp);
		temp.clear();
	}
	return data_out;
}
std::vector<int> common_elements(const std::vector<std::vector<int>>& index_of_data)
{
	if (index_of_data.size() < 2)
		return {};
	std::vector<int> out;
	int baseIndex = 0;
	std::vector<int> indices(index_of_data.size() - 1);
	int totalMatchFound;
	bool smallestArrayTraversalComplete = false;
	while ((baseIndex < index_of_data[0].size()) && (!smallestArrayTraversalComplete))
	{
		totalMatchFound = 0;
		for (int i = 1; i < index_of_data.size(); i++)
		{
			int currIndex = indices[i - 1];
			while ((currIndex < index_of_data[i].size()) && (index_of_data[i][currIndex] < index_of_data[0][baseIndex]))
			{
				currIndex += 1;
			}
			if (currIndex < index_of_data[i].size())
			{
				if ((index_of_data[i][currIndex] == index_of_data[0][baseIndex]))
				{
					totalMatchFound += 1;
				}
			}
			else
			{
				smallestArrayTraversalComplete = true;
			}
			indices[i - 1] = currIndex;
		}
		if (totalMatchFound == index_of_data.size() - 1)
		{
			out.push_back(index_of_data[0][baseIndex]);
		}
		baseIndex += 1;
	}
	return out;
}
std::vector<std::vector<int>> index_of_two_one_in_equal_index(const std::vector<std::vector<int>>& index_of_data, int k)
{
	std::vector<int> temp;
	std::vector<std::vector<int>> data_out;
	for (int i = k; i < index_of_data.size(); i++)
	{
	//	for (int j = i; j <= index_of_data.size() - 1; j++)
	//	{
		if (index_of_data[k].size() <= index_of_data[i].size())
		{
			for (int j : index_of_data[k])
			{
				auto it = std::find(index_of_data[i].begin(), index_of_data[i].end(), j);
				if (it != index_of_data[i].end())
					temp.push_back(j);
			}
			data_out.push_back(temp);
			temp.clear();
		}
		else
		{
			for (int j : index_of_data[i])
			{
				auto it = std::find(index_of_data[k].begin(), index_of_data[k].end(), j);
				if (it != index_of_data[k].end())
					temp.push_back(j);
			}
			data_out.push_back(temp);
			temp.clear();
		}
	//	}
	}
	return data_out;
}
unsigned long long int Factorial(int n)
{
	unsigned long long int result = 1;
	while (n > 1) {
		result *= n--;
	}
	return result;
}
void PermGenerator(int n, int k)
{

	std::vector<int> d(n);
	std::iota(d.begin(), d.end(), 1);
	std::cout << "These are the Possible Permutations: " << std::endl;
	unsigned long long int repeat = Factorial(n - k);
	do
	{
		bool isPrint = false;
		for (int i = 0; i < k; i++)
		{
			if (d[i] > d[i+1] && (i+1 < k))
				isPrint = true;
		}
		if (!isPrint)
		{
			for (int i = 0; i < k; i++)
			{
				std::cout << d[i] << " ";
			}
			std::cout << std::endl;
		}
		for (unsigned long long int i = 1; i != repeat; ++i)
		{
			next_permutation(d.begin(), d.end());
		}
	} while (next_permutation(d.begin(), d.end()));
}
void makeCombiUtil(std::vector<std::vector<int> >& ans,
	std::vector<int>& tmp, int n, int left, int k)
{
	// Pushing this vector to a vector of vector 
	if (k == 0) {
		ans.push_back(tmp);
		return;
	}

	// i iterates from left to n. First time 
	// left will be 1 
	for (int i = left; i <= n; ++i)
	{
		tmp.push_back(i);
		makeCombiUtil(ans, tmp, n, i + 1, k - 1);

		// Popping out last inserted element 
		// from the vector 
		tmp.pop_back();
	}
}

// Prints all combinations of size k of numbers 
// from 1 to n. 
std::vector<std::vector<int> > makeCombi(int n, int k)
{
	std::vector<std::vector<int> > ans;
	std::vector<int> tmp;
	makeCombiUtil(ans, tmp, n, 1, k);
	return ans;
}
void SetWindow(int Width, int Height)
{
	_COORD coord;
	coord.X = Width;
	coord.Y = Height;

	_SMALL_RECT Rect;
	Rect.Top = 0;
	Rect.Left = 0;
	Rect.Bottom = Height - 1;
	Rect.Right = Width - 1;

	HANDLE Handle = GetStdHandle(STD_OUTPUT_HANDLE);      // Get Handle 
	SetConsoleScreenBufferSize(Handle, coord);            // Set Buffer Size 
	SetConsoleWindowInfo(Handle, TRUE, &Rect);            // Set Window Size 
}
int main() {
	auto started = std::chrono::high_resolution_clock::now();
	SetWindow(200, 40);
	std::vector<std::vector<double>> data;
	std::vector<std::string> title;
	double max[20] = { -10,-10,-10,-10,-10,-10,-10,-10,-10,-10,-10,-10,-10,-10,-10,-10,-10,-10,-10,-10 };

	read_from_excel_file("saeed.txt", 32, data, title);

	int jj;
	double temp_max;

	std::vector<std::vector<double>> show(20);
	for (int i = 0; i < 20; i++)
		show[i] = std::vector<double>(6);

	std::vector<std::string> show_title(20);

	std::vector<std::vector<int> > ans = makeCombi(17, 1);
	std::vector<std::vector<int>> c = count_one_in_each_column(data, 12, 28);
	std::vector<std::vector<int>> cc;
	std::vector<std::vector<int>> b;
	std::vector<std::vector<int>> bb;

	//auto a = common_elements(c);
	for (float i = 1.1; i <= 2.1; i += 0.5)
	{
		for (int j = 0; j <= 6; j++)
		{
			b = count_if_greater_than_i_for_each_column(data, c, i, j + 5);
			for (int x = 0; x < b.size(); x++)
			{
				if (c.size() == 0 || c[x].size() < 20)
					continue;
				temp_max = (i * 0.02 * ((float)b[x].size() / (float)c[x].size())) - (0.02 * (1 - ((float)b[x].size() / (float)c[x].size())));
				for (int l = 0; l < 20; l++) {
					if (temp_max > max[l]) {
						max[l] = temp_max;
						show_title[l] = title[x+14];
						show[l][0] = c[x].size();
						show[l][1] = b[x].size();
						show[l][2] = ((float)b[x].size() / (float)c[x].size()) * 100;
						show[l][3] = j + 5;
						show[l][4] = i;
						show[l][5] = max[l] * 100;
						break;
					}
				}
			}

			for (int k = 2; k <= 3; k++)
			{
				std::vector<std::vector<int> > ans = makeCombi(17, k);
				for (int x = 0; x < ans.size(); x++) {
					std::vector<std::vector<int>> temp;
					std::string temp_title="";
					cc.clear();
					for (auto j : ans[x]) {
						temp.push_back(c[j-1]);
						temp_title += title[j + 13] + "--";
					}
					cc.push_back(common_elements(temp));
					temp_title.erase(temp_title.end() - 2, temp_title.end());
					temp.clear();
					bb = count_if_greater_than_i_for_each_column(data, cc, i, j + 5);
					if (cc.size() == 0 || cc[0].size() < 20)
						continue;
					temp_max = (i * 0.02 * ((float)bb[0].size() / (float)cc[0].size())) - (0.02 * (1 - ((float)bb[0].size() / (float)cc[0].size())));
					for (int l = 0; l < 20; l++) {
						if (temp_max > max[l]) {
							max[l] = temp_max;
							show_title[l] = temp_title;
							show[l][0] = cc[0].size();
							show[l][1] = bb[0].size();
							show[l][2] = ((float)bb[0].size() / (float)cc[0].size()) * 100;
							show[l][3] = j + 5;
							show[l][4] = i;
							show[l][5] = max[l] * 100;
							break;
						}
					}
				}
			}
			b.clear();
			b.shrink_to_fit();
		}
	}
	//time(&end);
	auto done = std::chrono::high_resolution_clock::now();
	//double execute_time_min = double(end - start);
	auto execute_time_second = std::chrono::duration_cast<std::chrono::milliseconds>(done - started).count();
	std::cout <<"\t\t\t\t" <<"Execution Time: "<< execute_time_second << " millisecond"
		<< std::endl << std::endl;

	for (int i = 0; i < show.size(); i++)
	{
		std::cout.width(100);
		std::cout.setf(std::ios::left, std::ios::adjustfield);
		std::cout << show_title[i] << "\t";
		for (int j = 0; j < show[0].size(); j++)
		{
			 std::cout << std::fixed << std::setprecision(1) << show[i][j];
			if (j == 2 || j == 5)
				std::cout << "%";
			std::cout << "\t";
		}
		std::cout << std::endl;
	}
	std::getchar();
	return 0;
}