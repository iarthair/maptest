#include <string>
#include <fstream>
#include <iostream>
//#include <functional>

#include <unordered_map.hpp>
#include <time.h>

int main(int argc, const char *argv[])
{
  std::string line;
  ska::unordered_map <std::string, int> table;
  long lineno = 0, total_lines = 0;
  timespec tvin, tvout;
  long ns, tally;
  std::ifstream fin(argv[1]);

  table.reserve (250000);
  table.max_load_factor (0.5f);

  // load table for lookup test and stats
  while (std::getline (fin, line))
    table[line] = ++lineno;
  total_lines = lineno;
  fin.clear ();
  fin.seekg (0);

  tally = 0;
  lineno = 0;
  for (int i = 0; i < 100; i++)
    {
      /* create a new table each time round else equivalent to
         ideal size reserved on 2nd and subsequent runs */
      ska::unordered_map <std::string, int> t2;
      t2.reserve (250000);
      t2.max_load_factor (0.5f);

      while (std::getline (fin, line))
	{
	  ++lineno;
	  clock_gettime (CLOCK_MONOTONIC, &tvin);
	  t2[line] = lineno;
	  clock_gettime (CLOCK_MONOTONIC, &tvout);
	  ns = tvout.tv_nsec - tvin.tv_nsec;
	  tally += (ns >= 0 ? ns : ns + 1000000000l);
	}
      fin.clear ();
      fin.seekg (0);
    }

  std::cout << "INSERT" << std::endl
	    << "items: " << total_lines << std::endl
            << "count: " << lineno << std::endl
            << "time: " << tally << "ns" << std::endl
  	    << "avg time: " << (double) tally / (double) lineno << std::endl
            << "buckets: " << table.bucket_count () << std::endl
  	    << "load factor: " << table.load_factor () << std::endl
  	    << "max load factor: " << table.max_load_factor () << std::endl;

  int junk = 0;
  tally = 0;
  lineno = 0;
  for (int i = 0; i < 250; i++)
    {
      while (std::getline (fin, line))
	{
	  ++lineno;
	  clock_gettime (CLOCK_MONOTONIC, &tvin);
	  junk += table[line];
	  clock_gettime (CLOCK_MONOTONIC, &tvout);
	  ns = tvout.tv_nsec - tvin.tv_nsec;
	  tally += (ns >= 0 ? ns : ns + 1000000000l);
	}
      fin.clear ();
      fin.seekg (0);
    }

  std::cout << "LOOKUP" << std::endl
	    << "count: " << lineno << std::endl
            << "time: " << tally << "ns" << std::endl
  	    << "avg time: " << (double) tally / (double) lineno << std::endl;

  return 0;
}
