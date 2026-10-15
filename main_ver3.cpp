
#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <queue>
using namespace std;

struct Issue {
    int id;
    string title;
    int severity;
    int urgency;
    int impact;
    int daysLeft;
};

int calculatePriority(const Issue& issue) {
    return 10 * issue.severity
         + 5 * issue.urgency
         + 3 * issue.impact;
}

string severityLabel(int severity) {
    switch (severity) {
        case 1: return "Low";
        case 2: return "Medium";
        case 3: return "High";
        case 4: return "Critical";
        default: return "Unknown";
    }
}

void displayIssues(const vector<Issue>& issues) {
    cout << "\n--- Issue Priority Report ---\n";

    for (const auto& issue : issues) {
        cout << "ID: " << issue.id
             << " | " << issue.title
             << " | Severity: " << severityLabel(issue.severity)
             << " | Priority: " << calculatePriority(issue)
             << " | Days left: " << issue.daysLeft << '\n';
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

// Kahn's algorithm for topological sorting
void dependencyOrder(const vector<Issue>& issues,
                     const vector<vector<int>>& graph) {
    int n = static_cast<int>(issues.size());
    vector<int> indegree(n, 0);

    // Calculate incoming edges for every issue
    for (int u = 0; u < n; u++) {
        for (int v : graph[u]) {
            indegree[v]++;
        }
    }

    // Start with issues having no prerequisites
    queue<int> ready;

    for (int i = 0; i < n; i++) {
        if (indegree[i] == 0) {
            ready.push(i);
        }
    }

    vector<int> order;

    while (!ready.empty()) {
        int u = ready.front();
        ready.pop();

        order.push_back(u);

        // Completing u unlocks its dependent issues
        for (int v : graph[u]) {
            indegree[v]--;

            if (indegree[v] == 0) {
                ready.push(v);
            }
        }
    }

    if (static_cast<int>(order.size()) != n) {
        cout << "\nCycle detected! No valid dependency order exists.\n";
        return;
    }

    cout << "\n--- Valid Dependency Order ---\n";

    for (int i = 0; i < n; i++) {
        const Issue& issue = issues[order[i]];

        cout << i + 1 << ". Issue #" << issue.id
             << " - " << issue.title
             << " (Priority: " << calculatePriority(issue) << ")\n";
    }

    cout << "\nAll dependencies are respected.\n";
}

int main() {
    vector<Issue> issues = {
        {101, "Login failure", 4, 4, 4, 1},
        {102, "Documentation typo", 1, 1, 1, 10},
        {103, "Database timeout", 3, 4, 4, 2},
        {104, "Broken search feature", 2, 3, 3, 3},
        {105, "Security vulnerability", 4, 5, 5, 2}
    };

    int n = static_cast<int>(issues.size());

    // graph[u] contains issues that depend on issue u.
    // Issue indices: 0=101, 1=102, 2=103, 3=104, 4=105.
    vector<vector<int>> graph(n);

    // Database timeout must be addressed before login failure.
    graph[2].push_back(0);

    // Login failure must be addressed before broken search.
    graph[0].push_back(3);

    // Database timeout must also precede broken search.
    graph[2].push_back(3);

    int choice;

    do {
        cout << "\n=== Open Source Issue Triage V3 ===\n";
        cout << "1. Display all issues\n";
        cout << "2. Sort by priority score\n";
        cout << "3. Search issue by ID\n";
        cout << "4. Generate dependency order\n";
        cout << "0. Exit\n";
        cout << "Enter choice: ";

        if (!(cin >> choice)) {
            cout << "Invalid input.\n";
            return 1;
        }

        if (choice == 1) {
            displayIssues(issues);
        }
        else if (choice == 2) {
            sortByPriority(issues);
            displayIssues(issues);
        }
        else if (choice == 3) {
            int searchId;
            cout << "Enter issue ID: ";

            if (!(cin >> searchId)) {
                cout << "Invalid issue ID.\n";
                return 1;
            }

            auto it = find_if(
                issues.begin(), issues.end(),
                [searchId](const Issue& issue) {
                    return issue.id == searchId;
                });

            if (it != issues.end()) {
                displayIssues(vector<Issue>{*it});
            } else {
                cout << "Issue not found.\n";
            }
        }
        else if (choice == 4) {
            dependencyOrder(issues, graph);
        }
        else if (choice != 0) {
            cout << "Invalid choice.\n";
        }

    } while (choice != 0);

    cout << "Triage program closed.\n";
    return 0;
}
