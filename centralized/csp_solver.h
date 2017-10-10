#include <vector>

class Range;

class CSP_Solver
{
public:
	CSP_Solver();
	~CSP_Solver();
	
	void add_range_bin_sequence(double min_start, double max_start, std::vector<int> bin_sequence, std::vector<Range> range_sequence);
	std::vector<double> solve();

private:
	std::vector<std::vector<Range>> range_sequences;
	std::vector<std::vector<int>> bin_sequences;
	std::vector<double> min_starts;
	std::vector<double> max_starts;
};


class Range
{
public:
	Range(double start, double end);
	~Range();

private:
	double start, double end;
};