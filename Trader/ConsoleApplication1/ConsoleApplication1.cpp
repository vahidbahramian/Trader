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





/*double open[10000]; double high[10000]; double low[10000]; double close[10000];
int volume[10000]; double s10[10000]; double s20[10000]; double s30[10000];
double s40[10000];  double s50[10000]; double s60[10000]; double s70[10000];
bool osma[10000]; bool macdm[10000]; bool ma20[10000]; bool rsi[10000];
bool cci[10000]; bool pin_bar[10000]; bool osma_h4[10000]; bool macdm_h4[10000];
bool rsi_h4[10000]; bool cci_h4[10000]; bool pin_bar_h4[10000]; bool osma_d1[10000]; bool macdm_d1[10000];
bool ma20_d1[10000]; bool rsi_d1[10000]; bool cci_d1[10000]; bool pin_bar_d1[10000];*/
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

int main() {
	//time_t start, end;
	//time(&start);
	auto started = std::chrono::high_resolution_clock::now();
	std::vector<std::vector<double>> data;
	std::vector<std::vector<double>> show(20);
	double max[20] = { -10,-10,-10,-10,-10,-10,-10,-10,-10,-10,-10,-10,-10,-10,-10,-10,-10,-10,-10,-10 };

	read_from_excel_file("saeed.txt", 32, data);
	float c; float cc;
	float b; float bb;
	int jj;

	char* pch;

	for (int i = 0; i < 20; i++)
		show[i] = std::vector<double>(6);

	for (float i = 1.1; i <= 2.1; i += 0.5)
		for (int j = 5; j <= 11; j++)
			for (int k = 12; k <= 28; k++)
			{
				c = 0; b = 0;
				for (int l = 0; l <= 9831; l++) {
					if (data[l][k] == 1)
					{
						c++;
						if (data[l][j] >= i)
							b++;
					}

				}
				for (int l = 0; l < 20; l++) {
					double temp = (i * 0.02 * (b / c)) - (0.02 * (1 - (b / c)));
					if (temp > max[l]) {
						max[l] = temp;
						show[l][0] = c;
						show[l][1] = b;
						show[l][2] = (b / c)*100;
						show[l][3] = j;
						show[l][4] = i;
						show[l][5] = max[l]*100;
						break;
					}
				}
				;
				for (jj = k; jj <= 28; jj++)
				{
					cc = 0;
					bb = 0;
					for (int l = 0; l <= 9831; l++) {
						if (data[l][k] == 1 && data[l][jj] == 1)
						{
							cc++;
							if (data[l][j] >= i)
								bb++;
						}

					}
					for (int l = 0; l < 20; l++) {
						if (cc == 0)
							break;
						double temp = (i * 0.02 * (bb / cc)) - (0.02 * (1 - (bb / cc)));
						if (temp > max[l]) {
							max[l] = temp;
							show[l][0] = cc;
							show[l][1] = bb;
							show[l][2] = (bb / cc)*100;
							show[l][3] = j;
							show[l][4] = i;
							show[l][5] = max[l]*100;
							break;
						}
					}
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
