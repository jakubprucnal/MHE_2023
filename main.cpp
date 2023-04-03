#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <memory>
#include <math.h>
#include <random>
#include <list>
#include <set>
#include <map>
#include <complex>
#include <fstream>
#include <sstream>
#include <any>

using problem_t = std::vector<int>;

auto argu = [](int argc, char** argv, std::string name,
              auto default_value) -> decltype(default_value) {
    using namespace std;
    string paramname = "";
    any ret = default_value;
    for (auto argument : vector<string>(argv, argv + argc)) {
        if ((argument.size() > 0) && (argument[0] == '-')) {
            if (paramname != "") {
                if (name == argument.substr(1)) ret = true;
            }
            paramname = argument.substr(1);
        } else if (name == paramname) {
            if (std::is_same_v<decltype(default_value), int>)
                ret = stoi(argument);
            else if (std::is_same_v<decltype(default_value), double>)
                ret = stod(argument);
            else if (std::is_same_v<decltype(default_value), char>)
                ret = argument.at(0);
            else if (std::is_same_v<decltype(default_value), bool>)
                ret = (argument == "true") || (argument == "1") || (argument == "yes");
            else
                ret = argument;
            paramname = "";
        }
    }
    return std::any_cast<decltype(default_value)>(ret);
};

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

solution_t random_modify_with_random_points_count(solution_t current_point) {
    using namespace std;
    vector<int> v;
    for (int i=0; i < current_point.size(); i++) v.push_back(i);
    uniform_int_distribution<int> distr(0, current_point.size() - 1);
    int quantity = distr(rgen);
    std::shuffle(v.begin(), v.end(),rgen);
    for (int i=0; i < quantity; i++){
        current_point[v[i]] = !current_point[v[i]];
    }
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

solution_t tabu_function(solution_t solution, int tabu_size) {
    using namespace std;
    set<solution_t> tabu_set;
    list<solution_t> tabu_list;

    auto is_in_tabu = [&](auto e) {
        return tabu_set.count(e);
    };

    auto add_to_tabu = [&](auto e) {
        tabu_set.insert(e);
        tabu_list.push_back(e);
    };

    auto shrink_tabu = [&]() {
        if (tabu_set.size() > tabu_size && tabu_size != 0) {
            tabu_set.erase(tabu_list.front());
            tabu_list.pop_front();
        }
    };

    solution_t new_solution = solution;
    for (int i = 0; i < 254; i++) {
        auto neighbours = current_neighbours(new_solution);
        neighbours.erase(remove_if(neighbours.begin(),
                                   neighbours.end(),
                                   [&](auto e) { return is_in_tabu(e); }),
                         neighbours.end());
        if (neighbours.size() == 0) break;
        new_solution = *max_element(neighbours.begin(), neighbours.end(), [&](auto a, auto b) {
            return a.goal() > b.goal();
        });
        add_to_tabu(new_solution);
        if (new_solution.goal() <= solution.goal()) {
            solution = new_solution;
            cout << i << " " << solution << "  Result: " << solution.goal() << std::endl;
        }
        shrink_tabu();
    }
    return solution;
}

solution_t simulated_anneling(solution_t start_point, std::function<double(int)> temperature) {
    auto solution = start_point;
    for (int i = 0; i < solution.size(); i++) {
        solution[i] = true;
    }
    int iterator = 1;
    auto best_solution = solution;
    int i = 0;
    do {
        auto neighbour = random_modify_with_random_points_count(solution);
        if (neighbour.goal() < solution.goal()) {
            solution = neighbour;
            std::cout << (i++) << " " << solution << "  " << solution.goal() << " *** " << best_solution << "  "
                      << best_solution.goal() << std::endl;
        } else {
            std::uniform_real_distribution<double> u(0,1);
            if (u(rgen) < exp(-abs(solution.goal() - neighbour.goal()) / temperature(iterator))) {
                solution = neighbour;
            }
        }
        if (solution.goal() <= best_solution.goal()) {
            best_solution = solution;
        }
        iterator++;
    } while (iterator < 254);

    return best_solution;
}

problem_t load_problem(const std::string fname)
{
using namespace std;
problem_t problem;
istream *fst = &cin;
ifstream f;
if (fname != "") {
f = ifstream(fname);
fst = &f;
}
string line;
getline(*fst, line);
stringstream sline(line);
int x;
while (sline >> x){
    problem.push_back(x);
}
return problem;
}

int main(int argc, char** argv) {
    using namespace std;
    auto fname = argu(argc, argv, "fname", string(""));
    auto show_progress = argu(argc, argv, "show_progress", false);
    auto iterations = argu(argc, argv, "iterations", 1000);
    auto tabu_size = argu(argc, argv, "tabu_size", 100); // If tabu_size == 0 then tabu is infinity
    auto method = argu(argc, argv, "method", string("tabu"));

    auto pop_size = argu(argc, argv, "pop_size", 50);
    auto crossover_p = argu(argc, argv, "crossover_p", 0.9);
    auto mutation_p = argu(argc, argv, "mutation_p", 0.1);

    cout << "# fname = " << fname << ";" << endl;
    map<string, string> params;
    vector<string> arguments(argv, argv + argc);
    string paramname = "";

    auto part_problem = load_problem(fname);
    solution_t solution(make_shared<problem_t>(part_problem));
    for (int i = 0; i < part_problem.size(); i++) solution.push_back(true);
    cout << part_problem << std::endl;

//    std::chrono::duration<double> calculation_duration;
    if (method == "full_revision") {
        solution = brute_force(solution);
//        best_solution = a;
//        calculation_duration = b;
    } else if (method == "hillclimb") {
        solution = hillclimb(solution);
//        best_solution = a;
//        calculation_duration = b;
    } else if (method == "random_hillclimb") {
        solution = random_hillclimb(solution);
//        best_solution = a;
//        calculation_duration = b;
    } else if (method == "tabu") {
        solution = tabu_function(solution, 100);
//        best_solution = a;
//        calculation_duration = b;
    } else if (method == "simulated_annealing") {
        function<double(int)> temperature = [](int t){
            return 100/t;
        };
        solution = simulated_anneling(solution, temperature);
    }

    cout << solution << " Result  " << solution.goal() << std::endl;

}
