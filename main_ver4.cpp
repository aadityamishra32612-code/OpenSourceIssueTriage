
#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <queue>
using namespace std;

struct Issue {
    int id;
    string title;
    int severity, urgency, impact;
    int daysLeft;
    string requiredSkill;
    int estimatedHours;
};

struct Developer {
    string name;
    string skill;
    int workload;
    int maxHours;
};

int calculatePriority(const Issue& issue) {
    return 10 * issue.severity
         + 5 * issue.urgency
         + 3 * issue.impact;
}

void displayIssues(const vector<Issue>& issues) {
    cout << "\n--- Issue Report ---\n";

    for (const auto& i : issues) {
        cout << "#" << i.id << " | " << i.title
             << " | Skill: " << i.requiredSkill
             << " | Priority: " << calculatePriority(i)
             << " | Hours: " << i.estimatedHours << '\n';
    }
}

void sortByPriority(vector<Issue>& issues) {
    sort(issues.begin(), issues.end(),
         [](const Issue& a, const Issue& b) {
             if (calculatePriority(a) != calculatePriority(b))
                 return calculatePriority(a) > calculatePriority(b);
             return a.daysLeft < b.daysLeft;
         });
}

void dependencyOrder(const vector<Issue>& issues,
                     const vector<vector<int>>& graph) {
    int n = static_cast<int>(issues.size());
    vector<int> indegree(n, 0);

    for (int u = 0; u < n; u++)
        for (int v : graph[u])
            indegree[v]++;

    queue<int> ready;

    for (int i = 0; i < n; i++)
        if (indegree[i] == 0)
            ready.push(i);

    vector<int> order;

    while (!ready.empty()) {
        int u = ready.front();
        ready.pop();
        order.push_back(u);

        for (int v : graph[u]) {
            if (--indegree[v] == 0)
                ready.push(v);
        }
    }

    if (static_cast<int>(order.size()) != n) {
        cout << "Dependency cycle detected!\n";
        return;
    }

    cout << "\n--- Dependency Order ---\n";

    for (int index : order)
        cout << "#" << issues[index].id
             << " - " << issues[index].title << '\n';
}

// Greedy developer assignment
void assignDevelopers(const vector<Issue>& originalIssues,
                      vector<Developer>& developers) {
    vector<Issue> issues = originalIssues;
    sortByPriority(issues);

    cout << "\n--- Developer Assignments ---\n";

    for (const auto& issue : issues) {
        int best = -1;

        for (int d = 0; d < static_cast<int>(developers.size()); d++) {
            const auto& dev = developers[d];

            bool correctSkill = dev.skill == issue.requiredSkill;
            bool enoughCapacity =
                dev.workload + issue.estimatedHours <= dev.maxHours;

            if (correctSkill && enoughCapacity) {
                if (best == -1 ||
                    dev.workload < developers[best].workload) {
                    best = d;
                }
            }
        }

        if (best == -1) {
            cout << "#" << issue.id << " " << issue.title
                 << " -> No eligible developer available\n";
        } else {
            developers[best].workload += issue.estimatedHours;

            cout << "#" << issue.id << " " << issue.title
                 << " -> " << developers[best].name
                 << " | Priority: " << calculatePriority(issue)
                 << " | Workload: " << developers[best].workload
                 << "/" << developers[best].maxHours << " hours\n";
        }
    }

    cout << "\n--- Final Developer Workloads ---\n";

    for (const auto& dev : developers) {
        cout << dev.name << " (" << dev.skill << "): "
             << dev.workload << "/" << dev.maxHours << " hours\n";
    }
}

int main() {
    vector<Issue> issues = {
        {101, "Login failure", 4, 4, 4, 1, "UI", 3},
        {102, "Documentation typo", 1, 1, 1, 10, "Docs", 1},
        {103, "Database timeout", 3, 4, 4, 2, "Database", 4},
        {104, "Broken search feature", 2, 3, 3, 3, "UI", 2},
        {105, "Security vulnerability", 4, 5, 5, 2, "Security", 5}
    };

    vector<Developer> developers = {
        {"Alice", "Security", 0, 8},
        {"Bob", "Database", 0, 8},
        {"Charlie", "UI", 0, 8},
        {"Diana", "Docs", 0, 4}
    };

    int n = static_cast<int>(issues.size());
    vector<vector<int>> graph(n);

    // Edge A -> B means B depends on A.
    graph[2].push_back(0); // Database before login fix
    graph[0].push_back(3); // Login fix before search fix
    graph[2].push_back(3); // Database before search fix

    int choice;

    do {
        cout << "\n=== Open Source Issue Triage V4 ===\n";
        cout << "1. Display issues\n";
        cout << "2. Sort by priority\n";
        cout << "3. Search by ID\n";
        cout << "4. Dependency order\n";
        cout << "5. Assign developers\n";
        cout << "0. Exit\n";
        cout << "Choice: ";

        if (!(cin >> choice)) {
            cout << "Invalid input.\n";
            return 1;
        }

        if (choice == 1) {
            displayIssues(issues);
        } else if (choice == 2) {
            sortByPriority(issues);
            displayIssues(issues);
        } else if (choice == 3) {
            int id;
            cout << "Issue ID: ";

            if (!(cin >> id)) {
                cout << "Invalid ID.\n";
                return 1;
            }

            auto it = find_if(issues.begin(), issues.end(),
                [id](const Issue& i) { return i.id == id; });

            if (it != issues.end())
                displayIssues(vector<Issue>{*it});
            else
                cout << "Issue not found.\n";

        } else if (choice == 4) {
            dependencyOrder(issues, graph);
        } else if (choice == 5) {
            assignDevelopers(issues, developers);
        } else if (choice != 0) {
            cout << "Invalid choice.\n";
        }

    } while (choice != 0);

    return 0;
}
