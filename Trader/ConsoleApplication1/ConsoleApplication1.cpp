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

bool read_from_excel_file(std::string file,int numberOfColumn, std::vector<std::vector<double>>& cell)
{
	std::string line;
	std::ifstream infile(file);
	if (!infile.is_open())
		return false;
	std::vector<double> temp_row;
	std::getline(infile, line, '\n');
	while (std::getline(infile, line, '\n'))
	{
		std::istringstream buffer(line);
		std::vector<std::string> buff(numberOfColumn);
		for (int i = 0; i < buff.size(); i++)
		{
			buffer >> buff[i];
			if (i > 2)
				temp_row.push_back(std::stod(buff[i]));
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
int main() {
	//time_t start, end;
	//time(&start);
	//std::thread t1(task1, "Hello");
	auto started = std::chrono::high_resolution_clock::now();
	std::vector<std::vector<double>> data;
	std::vector<std::vector<double>> show(20);
	double max[20] = { -10,-10,-10,-10,-10,-10,-10,-10,-10,-10,-10,-10,-10,-10,-10,-10,-10,-10,-10,-10 };

	read_from_excel_file("saeed.txt", 32, data);

	int jj;
	double temp;
	for (int i = 0; i < 20; i++)
		show[i] = std::vector<double>(6);

	std::vector<std::vector<int>> c = count_one_in_each_column(data, 12, 28);
	std::vector<std::vector<int>> cc;
	std::vector<std::vector<int>> b;
	std::vector<std::vector<int>> bb;

	for (float i = 1.1; i <= 2.1; i += 0.5)
	{
		for (int j = 0; j <= 6; j++)
		{
			b = count_if_greater_than_i_for_each_column(data, c, i,j+5);
			for (int k = 0; k <= 16; k++)
			{
				if (c[k].size() != 0)
					temp = (i * 0.02 * ((float)b[k].size() / (float)c[k].size())) - (0.02 * (1 - ((float)b[k].size() / (float)c[k].size())));
				for (int l = 0; l < 20; l++) {
					if (temp > max[l]) {
						max[l] = temp;
						show[l][0] = c[k].size();
						show[l][1] = b[k].size();
						show[l][2] = ((float)b[k].size() / (float)c[k].size()) * 100;
						show[l][3] = j + 5;
						show[l][4] = i;
						show[l][5] = max[l] * 100;
						break;
					}
				}
				cc = index_of_two_one_in_equal_index(c,k);
				bb = count_if_greater_than_i_for_each_column(data, cc, i, j+5);
				for (jj = k; jj <= 16; jj++)
				{
					int idx = jj - k;
					if (cc[idx].size() != 0)
						temp = (i * 0.02 * ((float)bb[idx].size() / (float)cc[idx].size())) - (0.02 * (1 - ((float)bb[idx].size() / (float)cc[idx].size())));
					for (int l = 0; l < 20; l++) {
						if (temp > max[l]) {
							max[l] = temp;
							show[l][0] = cc[idx].size();
							show[l][1] = bb[idx].size();
							show[l][2] = ((float)bb[idx].size() / (float)cc[idx].size()) * 100;
							show[l][3] = j+5;
							show[l][4] = i;
							show[l][5] = max[l] * 100;
							break;
						}
					}
				}
				bb.clear();
				bb.shrink_to_fit();
				cc.clear();
				cc.shrink_to_fit();
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
		for (int j = 0; j < show[0].size(); j++)
		{
			std::cout << "\t" << show[i][j];
			if (j == 2 || j == 5)
				std::cout << "%";
			std::cout << "\t";
		}
		std::cout << std::endl;
	}
	std::getchar();
	return 0;
}
