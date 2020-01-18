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
#include <mutex>

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

std::vector<std::vector<float>> data;
std::vector<std::vector<std::vector<float>>> dataMonth;
std::vector<std::string> title;
std::vector<tm> date;

std::vector<std::vector<std::vector<float>>> show_final;

std::map<std::string, std::pair<std::vector<unsigned short>, std::vector<byte>>> ccc;

std::vector<std::vector<unsigned short>> c;
bool end_of_thread = false;

template <class Key, class Value>
unsigned long mapCapacity(const std::map<Key, Value>& map) {
	unsigned long cap = sizeof(map);
	for (typename std::map<Key, Value>::const_iterator it = map.begin(); it != map.end(); ++it) {
		cap += it->first.capacity();
		cap += it->second.capacity();
	}
	return cap;
}

template <class Value>
unsigned long vectorCapacity(const std::vector<Value>& vector) {
	unsigned long cap = sizeof(vector);
	for (int i = 0; i < vector.size(); i++) {
		cap += vector.capacity();
	}
	return cap;
}
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
void makeCombiUtil(std::vector<std::vector<byte> >& ans,
	std::vector<byte>& tmp, int n, int left, int k, std::mutex& mtx)
{
	// Pushing this vector to a vector of vector 
	if (k == 0) {
		//const std::lock_guard<std::mutex> lock(mtx);
		//while (!mtx.try_lock());
		mtx.lock();
		ans.push_back(tmp);
		mtx.unlock();
		return;
	}

	// i iterates from left to n. First time 
	// left will be 1 
	for (int i = left; i <= n; ++i)
	{
		tmp.push_back(i);
		makeCombiUtil(ans, tmp, n, i + 1, k - 1, mtx);
		// Popping out last inserted element 
		// from the vector 
		//tmp.pop_back();
		tmp.erase(tmp.end() - 1, tmp.end());
		tmp.shrink_to_fit();
	}
}

// Prints all combinations of size k of numbers 
// from 1 to n. 
void makeCombi(std::vector<std::vector<byte>>& ans, int n, int k, std::mutex& mtx)
{
	std::vector<byte> tmp;
	makeCombiUtil(ans, tmp, n, 1, k, mtx);
	end_of_thread = true;

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

void first_iteration_calculate(std::vector<std::vector<float>>& show, std::vector<std::string>& show_title, std::vector<int>& num_days,
	float i, int j)
{
	auto First = std::chrono::high_resolution_clock::now();
	auto Last_Fisrt = First;
	std::vector<std::vector<unsigned short>> cc;
	std::vector<std::vector<int>> bb;
	std::thread th1;
	for (int k = 1; k <= m_ConfigParam.combination; k++)
	{
		std::mutex mtx;
		std::vector<std::vector<byte>> ans;
		th1 = std::thread(makeCombi, std::ref(ans), m_ConfigParam.combination, k, std::ref(mtx));
		end_of_thread = false;
		std::cout << "k = " << k << "\n";
		long long int count = 0;
		long long int x = 0;
		//Sleep(1000);
		while (!end_of_thread || ans.size() != 0)
		{

			std::vector<std::vector<unsigned short>> temp;
			std::string temp_title = "";
			cc.clear();
			std::vector<byte> t;
			if (ans.size() != 0)
			{
				x++;
				//const std::lock_guard<std::mutex> lock(mtx);
				//while (!mtx.try_lock());
				mtx.lock();
				for (int p = 0; p < ans[0].size(); p++) {
					temp_title += title[ans[0][p] + m_ConfigParam.beginKColumn - 2] + "--";
					if (ans[0].size() - p == 2)
					{
						if (ccc.find(temp_title) != ccc.end())
							temp.push_back(ccc[temp_title].first);
						else
							break;
					}
					if (ans[0].size() - p == 1)
						temp.push_back(c[ans[0][p] - 1]);
					t.push_back(ans[0][p]);
				}
				//mtx.unlock();
								//mtx.lock();
				ans.erase(ans.begin(), ans.begin() + 1);
				ans.shrink_to_fit();
				mtx.unlock();
				if (temp.size() > 1)
					cc.push_back(common_elements(temp));
				else if (temp.size() == 1)
					cc.push_back(temp[0]);

				if (cc.size() != 0)
					if (cc[0].size() >= m_ConfigParam.cSmallerThan)
					{
						ccc[temp_title] = std::make_pair(cc[0], t);
					}
					else
						count++;
				else
					count++;


				auto Second = std::chrono::high_resolution_clock::now();
				if (auto execute_time_second = std::chrono::duration_cast<std::chrono::seconds>(Second - Last_Fisrt).count() > 10)
				{
					std::cout << "Code not stop!" << "\t" << ccc.size() << "\t" << ans.size() << "\t" << count << "\n";
					Last_Fisrt = Second;
				}

				t.clear();
				int days = 0;
				bb.clear();
				bb = count_if_greater_than_i_for_each_column(data, date, cc, i, j + 5, days);
				if (cc.size() == 0 || cc[0].size() < m_ConfigParam.cSmallerThan)
					continue;
				auto max = (float)cc[0].size() * (i * 0.02 * ((float)bb[0].size() / (float)cc[0].size())) - (float)cc[0].size() * (0.02 * (1 - ((float)bb[0].size() / (float)cc[0].size())));
				for (int l = 0; l < show.size(); l++) {
					if (max * 100 > show[l][5]) {
						std::vector<float> showParam;
						show.erase(show.end() - 1, show.end());
						showParam.push_back(cc[0].size());
						showParam.push_back(bb[0].size());
						showParam.push_back(((float)bb[0].size() / (float)cc[0].size()) * 100);
						showParam.push_back(j + 5);
						showParam.push_back(i);
						showParam.push_back(max * 100);
						show.insert(show.begin() + l, showParam);
						showParam.clear();

						show_title.erase(show_title.end() - 1, show_title.end());
						show_title.insert(show_title.begin() + l, temp_title);

						num_days.erase(num_days.end() - 1, num_days.end());
						num_days.insert(num_days.begin() + l, days);
						break;
					}
				}
			}
		}
		th1.join();
		if (count == x)
			break;
	}
}
void other_iteration_calculate(std::vector<std::vector<float>>& show, std::vector<std::string>& show_title, std::vector<int>& num_days,
	float i, int j)
{
	//std::vector<std::vector<float>> show_temp;
	//show_temp = show;
	//std::vector<std::string> show_title_temp;
	//show_title_temp = show_title;
	//std::vector<int> num_days_temp;
	//num_days_temp = num_days;
	std::string temp_title = "";
	std::vector<std::vector<unsigned short>> cc;
	std::vector<std::vector<int>> bb;
	for (auto &x : ccc)
	{
		temp_title = x.first;
		cc.clear();
		cc.push_back(x.second.first);
		int days = 0;
		bb = count_if_greater_than_i_for_each_column(data, date, cc, i, j + 5, days);
		if (cc.size() == 0 || cc[0].size() < m_ConfigParam.cSmallerThan)
			continue;
		auto max = (float)cc[0].size() * (i * 0.02 * ((float)bb[0].size() / (float)cc[0].size())) - (float)cc[0].size() * (0.02 * (1 - ((float)bb[0].size() / (float)cc[0].size())));
		for (int l = 0; l < show.size(); l++) {
			if (max * 100 > show[l][5]) {
				std::vector<float> showParam;
				show.erase(show.end() - 1, show.end());
				showParam.push_back(cc[0].size());
				showParam.push_back(bb[0].size());
				showParam.push_back(((float)bb[0].size() / (float)cc[0].size()) * 100);
				showParam.push_back(j + 5);
				showParam.push_back(i);
				showParam.push_back(max * 100);
				show.insert(show.begin() + l, showParam);
				//mtx.lock();
				show_title.erase(show_title.end() - 1, show_title.end());
				show_title.insert(show_title.begin() + l, temp_title);

				num_days.erase(num_days.end() - 1, num_days.end());
				num_days.insert(num_days.begin() + l, days);
				//mtx.unlock();
				break;
			}
		}

	}
	//show = show_temp;
	//show_title = show_title_temp;
	//num_days = num_days_temp;
}
int main() {
	SetPriorityClass(GetCurrentProcess(), REALTIME_PRIORITY_CLASS);
	SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_TIME_CRITICAL);
	auto Start = std::chrono::high_resolution_clock::now();
	auto started = std::chrono::high_resolution_clock::now();
	read_config("Config.txt");

	auto done = std::chrono::high_resolution_clock::now();
	auto execute_time_second = std::chrono::duration_cast<std::chrono::milliseconds>(done - started).count();
	std::cout <<"Read Config Time: " << execute_time_second << " milliseconds"<< std::endl;

	SetWindow(200, 3000);

	started = std::chrono::high_resolution_clock::now();

	read_from_excel_file(m_ConfigParam.fileName, m_ConfigParam.columnNumber + 1, data, title, date);

	done = std::chrono::high_resolution_clock::now();
	execute_time_second = std::chrono::duration_cast<std::chrono::milliseconds>(done - started).count();
	std::cout << "Read Input Time: " << execute_time_second << " milliseconds" << std::endl;


	started = std::chrono::high_resolution_clock::now();

	std::vector<int> inMonth(m_ConfigParam.maxOutput, 0);
	std::vector<int> num_days(m_ConfigParam.maxOutput);
	std::vector<std::string> show_title(m_ConfigParam.maxOutput);
	std::vector<std::vector<float>> show(m_ConfigParam.maxOutput);
	for (int j = 0; j < m_ConfigParam.maxOutput; j++)
		show[j] = std::vector<float>(6,-1000);

	int numberOfK = (m_ConfigParam.endKColumn - m_ConfigParam.beginKColumn) + 1;
	int numberOfJ = (m_ConfigParam.endJColumn - m_ConfigParam.beginJColumn) + 1;

	c = count_one_in_each_column(data, m_ConfigParam.beginKColumn - 3, m_ConfigParam.endKColumn - 3);
	for (int i = 0; i< m_ConfigParam.IValues.size();i++)
	{
		for (int j = 0; j < numberOfJ; j++)
		{
			if (j == 0 && i == 0)
			{
				auto s = std::chrono::high_resolution_clock::now();
				first_iteration_calculate(show, show_title, num_days, m_ConfigParam.IValues[i], j);
				auto st = std::chrono::high_resolution_clock::now();
				auto e = std::chrono::duration_cast<std::chrono::milliseconds>(st - s).count();
				std::cout << "First Iteration Execution Time: " << e << " milliseconds"
					<< std::endl;
				//ans.clear();
			}
			else
			{
				//th1[(j + numberOfJ * i)-1] = std::thread(other_iteration_calculate, std::ref(show), std::ref(show_title), std::ref(num_days), m_ConfigParam.IValues[i], j);
				other_iteration_calculate(show, show_title, num_days, m_ConfigParam.IValues[i], j);
			}
		}
	}
	//for (int i = 0; i < m_ConfigParam.IValues.size(); i++)
	//{
	//	for (int j = 1; j < numberOfJ; j++)
	//	{
	//		th1[(j + numberOfJ * i) - 1].join();
	//	}
	//}
	done = std::chrono::high_resolution_clock::now();
	execute_time_second = std::chrono::duration_cast<std::chrono::milliseconds>(done - started).count();
	std::cout << "Algorithm Execution Time: " << execute_time_second << " milliseconds"
		<< std::endl;

	started = std::chrono::high_resolution_clock::now();

	show_final.push_back(show);
	c.clear();
	split_data_to_month(data, date, dataMonth);
	std::vector<std::vector<unsigned short>> cc;
	std::vector<std::vector<int>> bb;
	for (int m = 0; m < dataMonth.size(); m++)
	{
		c = count_one_in_each_column(dataMonth[m], m_ConfigParam.beginKColumn - 3, m_ConfigParam.endKColumn - 3);
		for (int s = 0; s < show.size(); s++)
		{
			show[s].clear();
			std::vector<std::vector<unsigned short>> temp;
			cc.clear();

			auto a = ccc[show_title[s]];
			for (auto x : a.second)
			{
				temp.push_back(c[x-1]);
			}
			if (temp.size() > 1)
				cc.push_back(common_elements(temp));
			else if (temp.size() == 1)
				cc.push_back(temp[0]);
			float i = show_final[0][s][4];
			int j = show_final[0][s][3];
			bb = count_if_greater_than_i_for_each_column(dataMonth[m], cc, i, j);
			if (cc.size() == 0 || cc[0].size() < 1)
				show[s].clear();
			else
			{
				auto max = (float)cc[0].size() * (i * 0.02 * ((float)bb[0].size() / (float)cc[0].size())) - (float)cc[0].size() * (0.02 * (1 - ((float)bb[0].size() / (float)cc[0].size())));
				show[s].push_back(cc[0].size());
				show[s].push_back(bb[0].size());
				show[s].push_back(((float)bb[0].size() / (float)cc[0].size()) * 100);
				show[s].push_back(j);
				show[s].push_back(i);
				show[s].push_back(max * 100);
				inMonth[s]++;
			}
		}
		show_final.push_back(show);
	}
	done = std::chrono::high_resolution_clock::now();
	execute_time_second = std::chrono::duration_cast<std::chrono::milliseconds>(done - started).count();
	std::cout <<"Split to month Time: "<< execute_time_second << " milliseconds"
		<< std::endl;


	auto End = std::chrono::high_resolution_clock::now();
	execute_time_second = std::chrono::duration_cast<std::chrono::milliseconds>(End - Start).count();
	std::cout << "Overall Execution Time: " << execute_time_second << " milliseconds"
		<< std::endl << std::endl;

	std::ofstream outfile;
	outfile.open("Output.txt", std::ios::out | std::ios::trunc);
	if (outfile.is_open())
	{
		int temp = -1;
		for (int i = 0; i < show_final.size(); i++)
		{
			if (i == 0)
				outfile << "\t\t\t\t" << "Overall" << "\n";
			else
			{
				outfile << "\t\t\t\t" << date[dataMonth[i - 1].size() + temp].tm_mon << "/" << date[dataMonth[i - 1].size() + temp].tm_year << "\n";
				temp += dataMonth[i - 1].size() - 1;
			}

			for (int j = 0; j < show_final[i].size(); j++)
			{
				//std::cout.width(100);
				//std::cout.setf(std::ios::left, std::ios::adjustfield);
				if (show_final[i][j].size() != 0)
				{
					outfile << show_title[j] << "\t";
					for (int k = 0; k < show_final[i][j].size(); k++)
					{
						if (i == 0 && k == 5)
							outfile << num_days[j] << "\t";
						if (i == 0 && k == 5)
							outfile << inMonth[j] << "\t";
						if (k == 2 || k == 5)
							outfile << std::fixed << std::setprecision(1) << show_final[i][j][k] << "%";
						else if (k == 4)
							outfile << std::fixed << std::setprecision(1) << show_final[i][j][k];
						else
							outfile << std::noshowpoint << std::setprecision(0) << show_final[i][j][k];
						outfile << "\t";
					}
					outfile << std::endl;
				}

			}
			outfile << "\n\n";
		}
	}
	outfile.close();

	int temp = -1;
	for (int i = 0; i < show_final.size(); i++)
	{
		if (i == 0)
			std::cout << "\t\t\t\t" << "Overall" << "\n";
		else
		{
			std::cout << "\t\t\t\t" << date[dataMonth[i - 1].size() + temp].tm_mon << "/" << date[dataMonth[i - 1].size() + temp].tm_year << "\n";
			temp += dataMonth[i - 1].size() - 1;
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