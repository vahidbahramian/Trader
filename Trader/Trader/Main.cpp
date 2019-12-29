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
#include <iterator>
#include <windows.h>
#include <map>

struct ConfigParam
{
	std::string fileName;
	int columnNumber;
	int maxOutput;
	int beginKColumn;
	int endKColumn;
	int beginJColumn;
	int endJColumn;
	std::vector<float> IValues;
	int combination;
	int cSmallerThan;

}m_ConfigParam;
bool read_config(std::string config)
{
	std::string line;
	std::ifstream infile(config);
	if (!infile.is_open())
		return false;
	while (std::getline(infile, line, '\n'))
	{
		if (line == "")
			continue;
		std::istringstream buffer(line);
		std::vector<std::string> results(std::istream_iterator<std::string>{buffer},
			std::istream_iterator<std::string>());
		if (results[0] == "File_Name")
			m_ConfigParam.fileName = results[2];
		else if (results[0] == "Number_Of_Column")
			m_ConfigParam.columnNumber = std::stoi(results[2]);
		else if (results[0] == "Number_Of_Maximum_Output")
			m_ConfigParam.maxOutput = std::stoi(results[2]);
		else if (results[0] == "Begin_Column_Of_K")
			m_ConfigParam.beginKColumn = std::stoi(results[2]);
		else if (results[0] == "End_Column_Of_K")
			m_ConfigParam.endKColumn = std::stoi(results[2]);
		else if (results[0] == "Begin_Column_Of_J")
			m_ConfigParam.beginJColumn = std::stoi(results[2]);
		else if (results[0] == "End_Column_Of_J")
			m_ConfigParam.endJColumn = std::stoi(results[2]);
		else if (results[0] == "I_Values")
			for(int i = 2; i < results.size(); i++)
				m_ConfigParam.IValues.push_back(std::stof(results[i]));
		else if (results[0] == "Maximum_Combination")
			m_ConfigParam.combination = std::stoi(results[2]);
		else if (results[0] == "C_Smaller_Than")
			m_ConfigParam.cSmallerThan = std::stoi(results[2]);
	}
	return true;
}
bool read_from_excel_file(std::string file,int numberOfColumn, std::vector<std::vector<float>>& cell,
	std::vector<std::string>& columnTitle, std::vector<tm>& date)
{
	std::string line;
	std::ifstream infile(file);
	if (!infile.is_open())
		return false;
	std::vector<float> temp_row;
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
		tm temp_date;
		for (int i = 0; i < numberOfColumn; i++)
		{
			buffer >> buff;
			if (i > 2)
				temp_row.push_back(std::stod(buff));
			else
			{
				char p[30];
				strcpy(p, buff.c_str());
				char* pch;
				if (i == 0)
				{
					pch = strtok(p, " ,.-:/");
					temp_date.tm_mon = atoi(pch);
					temp_date.tm_mday = atoi(strtok(NULL, " ,.-:/"));
					temp_date.tm_year = atoi(strtok(NULL, " ,.-:/"));
				}
				else if (i == 1)
				{
					pch = strtok(p, " ,.-:/");
					temp_date.tm_hour = atoi(pch); //get the hour value
					temp_date.tm_min = atoi(strtok(NULL, " ,.-:/")); //get the min value
					temp_date.tm_sec = atoi(strtok(NULL, " ,.-:/"));
					buffer >> buff;
					if (buff == "PM")
						temp_date.tm_hour += 12;
					i++;

					date.push_back(temp_date);
				}
			}
		}
		cell.push_back(temp_row);
		temp_row.clear();
	}
	return true;
}
void split_data_to_month(std::vector<std::vector<float>>& data, std::vector<tm> date, std::vector<std::vector<std::vector<float>>>& dataMonth)
{
	int last_month = date[0].tm_mon;
	std::vector<std::vector<float>> temp;
	for (int i = 0; i < data.size(); i++)
	{
		if (date[i].tm_mon == last_month)
			temp.push_back(data[i]);
		else
		{
			dataMonth.push_back(temp);
			temp.clear();
			temp.push_back(data[i]);
		}
		last_month = date[i].tm_mon;
	}
	dataMonth.push_back(temp);
}
std::vector<std::vector<unsigned short>> count_one_in_each_column(const std::vector<std::vector<float>>& data, int begin, int end)
{
	std::vector<unsigned short> temp;
	std::vector<std::vector<unsigned short>> data_out;
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
std::vector<std::vector<int>> count_if_greater_than_i_for_each_column(const std::vector<std::vector<float>>& data,
	std::vector<tm>& date ,const std::vector<std::vector<unsigned short>>& index_of_data, float factor, int j, int& days)
{
	std::vector<int> temp;
	temp.clear();
	std::vector<std::vector<int>> data_out;
	for (int i = 0; i < index_of_data.size(); i++)
	{
		int last_day = 0;
		int last_month = 0;
		for (int k : index_of_data[i])
		{
			if (data[k][j] >= factor)
			{
				temp.push_back(k);
			}
			if (date[k].tm_mday != last_day || date[k].tm_mon != last_month)
				days++;
			last_day = date[k].tm_mday;
			last_month = date[k].tm_mon;
		}

		data_out.push_back(temp);
		temp.clear();
	}
	return data_out;
}
std::vector<std::vector<int>> count_if_greater_than_i_for_each_column(const std::vector<std::vector<float>>& data,
	const std::vector<std::vector<unsigned short>>& index_of_data, float factor, int j)
{
	std::vector<int> temp;
	temp.clear();
	std::vector<std::vector<int>> data_out;
	for (int i = 0; i < index_of_data.size(); i++)
	{
		for (int k : index_of_data[i])
		{
			if (data[k][j] >= factor)
			{
				temp.push_back(k);
			}
		}
		data_out.push_back(temp);
		temp.clear();
	}
	return data_out;
}
std::vector<unsigned short> common_elements(const std::vector<std::vector<unsigned short>>& index_of_data)
{
	if (index_of_data.size() < 2)
	{
		std::vector<unsigned short> temp;
		for (int j = 0; j < index_of_data[0].size(); j++)
			if (index_of_data[0][j] == 1)
			{
				temp.push_back(j);
			}
		return temp;
	}

	std::vector<unsigned short> out;
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
	read_config("Config.txt");
	SetWindow(200, 3000);

	std::vector<std::vector<float>> data;
	std::vector<std::vector<std::vector<float>>> dataMonth;
	std::vector<std::string> title;
	std::vector<tm> date;
	read_from_excel_file(m_ConfigParam.fileName, m_ConfigParam.columnNumber + 1, data, title, date);

	std::vector<int> inMonth(m_ConfigParam.maxOutput,0);
	std::vector<int> num_days(m_ConfigParam.maxOutput);
	std::vector<std::vector<int>> k_store(m_ConfigParam.maxOutput);
	std::vector<std::string> show_title(m_ConfigParam.maxOutput);
	std::vector<std::vector<float>> show(m_ConfigParam.maxOutput);
	for (int j = 0; j < m_ConfigParam.maxOutput; j++)
		show[j] = std::vector<float>(6);

	std::vector<std::vector<std::vector<float>>> show_final;


	int numberOfK = (m_ConfigParam.endKColumn - m_ConfigParam.beginKColumn) + 1;
	int numberOfJ = (m_ConfigParam.endJColumn - m_ConfigParam.beginJColumn) + 1;
	float temp_max;
	std::map<std::string, std::vector<unsigned short>> ccc;
	std::vector<std::vector<int>> ans;
	std::vector<std::vector<unsigned short>> cc;
	std::vector<std::vector<int>> bb;

	std::vector<float> maxx(m_ConfigParam.maxOutput,-10);
	std::vector<std::vector<unsigned short>> c;

	std::vector<std::vector<std::string>> erase_map;
	std::vector<std::string> temp_erase_map(1,"");

	c = count_one_in_each_column(data, m_ConfigParam.beginKColumn - 3, m_ConfigParam.endKColumn - 3);
	for (float i : m_ConfigParam.IValues)
	{
		for (int j = 0; j < numberOfJ; j++)
		{
			for (int k = 1; k <= m_ConfigParam.combination; k++)
			{
				erase_map.push_back(temp_erase_map);
				temp_erase_map.clear();
				ans.clear();
				ans = makeCombi(numberOfK, k);
				for (int x = 0; x < ans.size(); x++) {
					std::vector<std::vector<unsigned short>> temp;
					std::string temp_title = "";
					std::string key_map = "";
					cc.clear();
					for (int p = 0; p < ans[x].size(); p++) {
						if (ans[x][p] < 10)
							key_map += std::to_string(ans[x][p]) + "-";
						else
							key_map += std::to_string(ans[x][p]) + "-";
						
						if (ans[x].size() - p == 2)
							temp.push_back(ccc[key_map]);
						if (ans[x].size() - p == 1)
							temp.push_back(c[ans[x][p] - 1]);
						temp_title += title[ans[x][p] + m_ConfigParam.beginKColumn - 2] + "--";
					}
					if (temp.size() > 1)
						cc.push_back(common_elements(temp));
					else
						cc.push_back(temp[0]);
					ccc[key_map] = cc[0];
					temp_erase_map.push_back(key_map);
					if (x < erase_map[0].size() && k > 1)
						ccc.erase(erase_map[0][x]);
					temp_title.erase(temp_title.end() - 2, temp_title.end());
					int days = 0;
					bb.clear();
					bb = count_if_greater_than_i_for_each_column(data, date, cc, i, j + 5, days);
					if (cc.size() == 0 || cc[0].size() < m_ConfigParam.cSmallerThan)
						continue;
					temp_max = (float)cc[0].size() * (i * 0.02 * ((float)bb[0].size() / (float)cc[0].size())) - (float)cc[0].size() * (0.02 * (1 - ((float)bb[0].size() / (float)cc[0].size())));
					for (int l = 0; l < show.size(); l++) {
						if (temp_max > maxx[l]) {
							maxx[l] = temp_max;
							show_title[l] = temp_title;
							show[l][0] = cc[0].size();
							show[l][1] = bb[0].size();
							show[l][2] = ((float)bb[0].size() / (float)cc[0].size()) * 100;
							show[l][3] = j + 5;
							show[l][4] = i;
							show[l][5] = maxx[l] * 100;
							k_store[l].clear();
							for (int a = 0; a < ans[x].size(); a++)
							{
								k_store[l].push_back(ans[x][a] - 1);
							}
							num_days[l] = days;
							break;
						}
					}
				}
				if (k > 1)
					erase_map.erase(erase_map.begin(), erase_map.begin() + 1);
			}
		}
	}
	show_final.push_back(show);
	c.clear();
	split_data_to_month(data, date, dataMonth);
	for (int m = 0; m < dataMonth.size(); m++)
	{
		c = count_one_in_each_column(dataMonth[m], m_ConfigParam.beginKColumn - 3, m_ConfigParam.endKColumn - 3);
		for (int s = 0; s < show.size(); s++)
		{
			show[s].clear();
			std::vector<std::vector<unsigned short>> temp;
			cc.clear();
			for (int x = 0; x < k_store[s].size(); x++)
			{
				temp.push_back(c[k_store[s][x]]);
			}
			if (temp.size() > 1)
				cc.push_back(common_elements(temp));
			else
				cc.push_back(temp[0]);
			float i = show_final[0][s][4];
			int j = show_final[0][s][3];
			bb = count_if_greater_than_i_for_each_column(dataMonth[m], cc, i, j);
			if (cc.size() == 0 || cc[0].size() < 1)
				show[s].clear();
			else
			{
				temp_max = (float)cc[0].size() * (i * 0.02 * ((float)bb[0].size() / (float)cc[0].size())) - (float)cc[0].size() * (0.02 * (1 - ((float)bb[0].size() / (float)cc[0].size())));
				show[s].push_back(cc[0].size());
				show[s].push_back(bb[0].size());
				show[s].push_back(((float)bb[0].size() / (float)cc[0].size()) * 100);
				show[s].push_back(j);
				show[s].push_back(i);
				show[s].push_back(temp_max * 100);
				inMonth[s]++;
			}

		}
		show_final.push_back(show);
	}
	//time(&end);
	auto done = std::chrono::high_resolution_clock::now();
	//float execute_time_min = float(end - start);
	auto execute_time_second = std::chrono::duration_cast<std::chrono::seconds>(done - started).count();
	std::cout <<"\t\t\t\t" <<"Execution Time: "<< execute_time_second << " Second"
		<< std::endl << std::endl;
	int temp = -1;
	for (int i = 0; i < show_final.size(); i++)
	{
		if (i == 0)
			std::cout << "\t\t\t\t" << "Overall" << "\n";
		else
		{
			std::cout << "\t\t\t\t" << date[dataMonth[i-1].size() + temp].tm_mon << "/" << date[dataMonth[i-1].size() + temp].tm_year << "\n";
			temp += dataMonth[i-1].size() - 1;
		}

		for (int j = 0; j < show_final[i].size(); j++)
		{
			std::cout.width(100);
			std::cout.setf(std::ios::left, std::ios::adjustfield);
			if (show_final[i][j].size() != 0)
			{
				std::cout << show_title[j] << "\t";
				for (int k = 0; k < show_final[i][j].size(); k++)
				{
					if (i == 0 && k == 5)
						std::cout << num_days[j] << "\t";
					if (i == 0 && k == 5)
						std::cout << inMonth[j] << "\t";
					if (k == 2 || k == 5)
						std::cout << std::fixed << std::setprecision(1) << show_final[i][j][k] << "%";
					else if (k == 4)
						std::cout << std::fixed << std::setprecision(1) << show_final[i][j][k];
					else 
						std::cout << std::noshowpoint << std::setprecision(0) << show_final[i][j][k];
					std::cout << "\t";
				}
				std::cout << std::endl;
			}

		}
		std::cout << "\n\n";
	}
	std::getchar();
	return 0;
}