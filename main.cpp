#include <iostream>
#include <array>
#include <string>
#include <vector>
#include <algorithm>
#include <memory>
#include <math.h>

using problem_t = std::vector<int>;

int subset_sum (std::vector<int> subset){
    int sum = 0;
    for (int i = 0; i < subset.size(); i++) {
        sum += subset[i];
    }
    return sum;
}

class solution_t : public std::array<std::vector<int>, 2> {
public:
    std::shared_ptr<problem_t> problem;

    solution_t(std::shared_ptr<problem_t> problem_) : problem(problem_) {}

    int goal() {
        auto &t = *this;
        int subset1_sum = subset_sum(t[0]), subset2_sum = subset_sum(t[1]);
        return fabs(subset1_sum - subset2_sum);
    }
};


std::ostream &operator<<(std::ostream &o, const problem_t v) {
    o << "{ ";
    for (auto e: v)
        o << e << " ";
    o << "}";
    return o;
}
std::ostream &operator<<(std::ostream &o, const solution_t v) {
    o << "[ ";
    for (auto e: v) o << e << " ";
    o << "]";
    return o;
}


//solution_t brute_force(solution_t start_point) {
//    auto solution = start_point;
//    solution = start_point
//    return best_solution;
//}



int main() {
    using namespace std;
    problem_t part_problem = {
            1, 3, 5, 7, 9, 5, 1, 1
    };
    cout << part_problem << std::endl;
    solution_t solution(make_shared<problem_t>(part_problem));
    solution[0] = {1};
    solution[1] = {2};
    cout << solution << "Start:  " << solution.goal() << std::endl;

}
