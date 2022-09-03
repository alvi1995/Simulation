#ifndef SIMULATION_H
#define SIMULATION_H

#include <chrono>
#include <random>
#include <climits>
#include <iostream>


typedef unsigned long long ull;
typedef unsigned short us;

us cell_value[400];
bool cell_condition[400];

ull sim_time = ULLONG_MAX % 100000;
std::random_device rd;
std::mt19937::result_type seed = rd() ^ (
	(std::mt19937::result_type)
	std::chrono::duration_cast<std::chrono::seconds>(
		std::chrono::system_clock::now().time_since_epoch()
		).count() +
	(std::mt19937::result_type)
	std::chrono::duration_cast<std::chrono::microseconds>(
		std::chrono::high_resolution_clock::now().time_since_epoch()
		).count());

std::mt19937 gen(seed);
std::uniform_int_distribution<unsigned> distrib(1, 40000);
us random_number_generator(us range)
{
	return distrib(gen) % range;
}

void initialize(us* cell_value, us energy)
{
	for (short i = 0; i < 400; i++)
	{
		cell_value[i] = energy;
	}
}

void cell_status(us* list, bool* condition, us range)
{
	for (us i = 0; i < range; i++)
	{
		if (list[i] == 0)
		{
			condition[i] = false;
		}
		else
		{
			condition[i] = true;
		}
	}
}

bool check_validity(us* list)
{
	us sum = 0;
	for (us i = 0; i < 400; i++)
	{
		sum = sum + list[i];
	}

	if (sum == 400)
	{
		return true;
	}
	else 
	{
		return false;
	}
}

us total_active_cell(bool* condition)
{
	us sum = 0;
	for (us i = 0; i < 400; i++)
	{
		if (condition[i] == true)
		{
			sum++;
		}
	}
	return sum;
}

void create_active_cell_list(bool* condition, us* list)
{
	us j = 0;
	for (us i = 0; i < 400; i++)
	{
		if (condition[i] == true)
		{
			list[j] = i;
			j++;
		}
	}
}

void display(us* value)
{
	for (us i = 0; i < 400; i++)
	{
		std::cout << "[" << i << "]:" << value[i] << " ";
		if (i % 50 == 0 && i > 0)
		{
			std::cout << std::endl;
		}
	}
}

us r, a;
void simulation()
{
	us remove = 0, add = 0;
	cell_status(cell_value, cell_condition, 400);
	us no_of_active_cell = total_active_cell(cell_condition);
	us* active_cell_list = new us[no_of_active_cell];
	create_active_cell_list(cell_condition, active_cell_list);
	remove = random_number_generator(no_of_active_cell);
	r = active_cell_list[remove];
	cell_value[active_cell_list[remove]]--;
	add = random_number_generator(400);
	a = add;
	cell_value[add]++;
	delete[]active_cell_list;
	if (check_validity(cell_value))
	{
		//std::cout << "1";
	}
	else
	{
		std::cout << "0";
	}
}
#endif // !SIMULATION_H

