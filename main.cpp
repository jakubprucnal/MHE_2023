#include <iostream>
#include <array>
#include <string>
#include <vector>
#include <algorithm>
#include <memory>
#include <math.h>

using problem_t = std::vector<int>;

class solution_t : public std::vector<bool> {
public:
    std::shared_ptr<problem_t> problem;

    solution_t(std::shared_ptr<problem_t> problem_) : problem(problem_) {}

    int goal() {
        int sums_difference = 0;
        for (int i = 0; i < size(); i++) {
            auto &p = *problem;
            auto &t = *this;
            if (t[i]){
                sums_difference += p[i];
            }
            else {
                sums_difference -= p[i];
            }
        }
        return fabs(sums_difference);
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
    o << "{ ";
    for (auto e: v) o << e << " ";
    o << "}";
    return o;
}


solution_t brute_force(solution_t start_point) {
    auto solution = start_point;
    for (int i = 0; i < solution.size(); i++) {
        solution[i] = false;
    }
    auto best_solution = solution;
    int i = 0;
    std::cout << i << " Last: " << solution << std::endl;
    for (int j = 0; j < solution.size()/2; j++){
        solution[0] = !solution[0];
        do {
            if (solution.goal() <= best_solution.goal()) {
                best_solution = solution;
                std::cout << (i++) << " " << solution << "  " << solution.goal() << " *** " << best_solution << "  "
                          << best_solution.goal() << std::endl;
            }
        } while (std::next_permutation(solution.begin(), solution.end()));
        std::cout << i << " Last j: " << j << " "<< solution << std::endl;
    }

    return best_solution;
}



int main() {
    using namespace std;
    problem_t part_problem = {
            1, 3, 5, 7, 9, 5, 1, 1, 1
    };
    cout << part_problem << std::endl;
    solution_t solution(make_shared<problem_t>(part_problem));
    for (int i = 0; i < part_problem.size(); i++) solution.push_back(true);
    solution = brute_force(solution);
    cout << solution << "Result  " << solution.goal() << std::endl;

}
