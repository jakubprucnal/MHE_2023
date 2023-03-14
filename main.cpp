#include <iostream>
#include <array>
#include <string>
#include <vector>
#include <algorithm>
#include <memory>
#include <math.h>
#include <random>
#include <list>
#include <set>
#include <algorithm>

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
            if (t[i]) {
                sums_difference += p[i];
            } else {
                sums_difference -= p[i];
            }
        }
        return fabs(sums_difference);
    }
};

using neighbours_t = std::vector<solution_t>;

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

std::random_device rd;
std::mt19937 rgen(rd());

solution_t random_modify(solution_t current_point) {
    using namespace std;
    uniform_int_distribution<int> distr(0, current_point.size() - 1);
    int a = distr(rgen);
    current_point[a] = !current_point[a];
    return current_point;
}

solution_t brute_force(solution_t start_point) {
    auto solution = start_point;
    for (int i = 0; i < solution.size(); i++) {
        solution[i] = true;
    }
    auto best_solution = solution;
    int i = 0;
    for (int j = 0; j < solution.size() / 2; j++) {
        solution[j] = 0;
        do {
            if (solution.goal() <= best_solution.goal()) {
                best_solution = solution;
                std::cout << (i++) << " " << solution << "  " << solution.goal() << " *** " << best_solution << "  "
                          << best_solution.goal() << std::endl;
            }
        } while (std::next_permutation(solution.begin(), solution.end()));
    }
    return best_solution;
}

neighbours_t current_neighbours(solution_t current_point) {
    using namespace std;
    neighbours_t neighbours;
    for (int i = 0; i < current_point.size(); i++) {
        solution_t neighbour = current_point;
        neighbour[i] = !neighbour[i];
        neighbours.push_back(neighbour);
    }
    return neighbours;
}

solution_t best_neighbour(solution_t current_point) {
    using namespace std;
    vector<solution_t> neighbours = current_neighbours(current_point);
    solution_t best_neigh = current_point;
    for (int i = 0; i < neighbours.size(); i++) {
        if (neighbours[i].goal() <= best_neigh.goal()) {
            best_neigh = neighbours[i];
        }
    }
    return best_neigh;
}

solution_t hillclimb(solution_t solution) {
    for (int i = 0; i < 254; i++) {
        auto new_solution = best_neighbour(solution);
        if (new_solution.goal() <= solution.goal()) {
            solution = new_solution;
            std::cout << i << " " << solution << "  " << solution.goal() << std::endl;
        }
    }
    return solution;
}

solution_t random_hillclimb(solution_t solution) {
    for (int i = 0; i < 254; i++) {
        auto new_solution = random_modify(solution);
        if (new_solution.goal() <= solution.goal()) {
            solution = new_solution;
            std::cout << i << " " << solution << "  " << solution.goal() << std::endl;
        }
    }
    return solution;
}

solution_t tabu_function(solution_t solution) {
    using namespace std;
    set<solution_t> tabu_set;
    list<solution_t> tabu_list;

    auto is_in_taboo = [&](auto e) {
        return tabu_set.count(e); };

    auto add_to_taboo = [&](auto e) {
        tabu_set.insert(e);
        tabu_list.push_back(e);
    };

    solution_t new_solution = solution;
    for (int i = 0; i < 254; i++) {
        auto neighbours = current_neighbours(new_solution);
        neighbours.erase(remove_if(neighbours.begin(),
                                        neighbours.end(),
                                        [&](auto e) { return is_in_taboo(e); }),
                         neighbours.end());
        if (neighbours.size() == 0) break;
        new_solution = *max_element(neighbours.begin(), neighbours.end(), [&](auto a, auto b) {
            return a.goal() > b.goal();
        });
        add_to_taboo(new_solution);
        if (new_solution.goal() <= solution.goal()) {
            solution = new_solution;
            cout << i << " " << solution << "  Result: " << solution.goal() << std::endl;
        }
    }
    return solution;
}

int main() {
    using namespace std;
    problem_t part_problem = {
            1, 3, 5, 7, 9, 5, 1, 1, 1
    };
    cout << part_problem << std::endl;
    solution_t solution(make_shared<problem_t>(part_problem));
    for (int i = 0; i < part_problem.size(); i++) solution.push_back(true);
//    solution = brute_force(solution);
//    solution = random_hillclimb(solution);
//    solution = hillclimb(solution);
    solution = tabu_function(solution);
    cout << solution << " Result  " << solution.goal() << std::endl;

}
