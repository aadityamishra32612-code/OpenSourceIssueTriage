
#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <queue>
#include <fstream>
#include <sstream>
#include <limits>

using namespace std;

// ===============================
// STRUCTURES
// ===============================

struct Issue {
    int id;
    string title;
    int severity;
    int urgency;
    int impact;
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

// ===============================
// CSV PARSER
// ===============================

vector<string> parseCSVLine(const string& line) {
    vector<string> fields;
    string field;
    bool insideQuotes = false;

    for (size_t i = 0; i < line.size(); i++) {
        char c = line[i];

        if (c == '"') {
            if (insideQuotes && i + 1 < line.size()
                && line[i + 1] == '"') {
                field += '"';
                i++;
            } else {
                insideQuotes = !insideQuotes;
            }
        }
        else if (c == ',' && !insideQuotes) {
            fields.push_back(field);
            field.clear();
        }
        else {
            field += c;
        }
    }

    fields.push_back(field);
    return fields;
}

// ===============================
// LOAD ISSUES FROM CSV
// ===============================

vector<Issue> loadIssuesFromCSV(const string& filename) {
    vector<Issue> issues;
    ifstream file(filename);

    if (!file.is_open()) {
        cerr << "Error: Cannot open " << filename << '\n';
        return issues;
    }

    string line;

    // Skip CSV header
    getline(file, line);

    int skipped = 0;

    while (getline(file, line)) {
        vector<string> row = parseCSVLine(line);

        if (row.size() != 8) {
            skipped++;
            continue;
        }

        try {
            Issue issue;

            issue.id = stoi(row[0]);
            issue.title = row[1];
            issue.severity = stoi(row[2]);
            issue.urgency = stoi(row[3]);
            issue.impact = stoi(row[4]);
            issue.daysLeft = stoi(row[5]);
            issue.requiredSkill = row[6];
            issue.estimatedHours = stoi(row[7]);

            if (issue.severity < 1 || issue.severity > 4 ||
                issue.urgency < 1 || issue.urgency > 5 ||
                issue.impact < 1 || issue.impact > 5 ||
                issue.estimatedHours < 0 ||
                issue.daysLeft < 0) {
                skipped++;
                continue;
            }

            issues.push_back(issue);
        }
        catch (const exception&) {
            skipped++;
        }
    }

    file.close();

    cout << "Skipped invalid rows: " << skipped << '\n';

    return issues;
}

// ===============================
// PRIORITY CALCULATION
// ===============================

int calculatePriority(const Issue& issue) {
    return 10 * issue.severity
         + 5 * issue.urgency
         + 3 * issue.impact;
}

// ===============================
// DISPLAY ISSUES
// ===============================

void displayIssues(const vector<Issue>& issues) {
    cout << "\n========== GITHUB ISSUE REPORT ==========\n";

    if (issues.empty()) {
        cout << "No issues available.\n";
        return;
    }

    for (const auto& issue : issues) {
        cout << "\nIssue ID: #" << issue.id
             << "\nTitle: " << issue.title
             << "\nSeverity: " << issue.severity
             << "\nUrgency: " << issue.urgency
             << "\nImpact: " << issue.impact
             << "\nPriority Score: " << calculatePriority(issue)
             << "\nRequired Skill: " << issue.requiredSkill
             << "\nEstimated Hours: " << issue.estimatedHours
             << "\n-----------------------------------------\n";
    }

    cout << "\nTotal Issues: " << issues.size() << '\n';
}

// ===============================
// SORT BY PRIORITY
// ===============================

void sortByPriority(vector<Issue>& issues) {
    sort(issues.begin(), issues.end(),
        [](const Issue& a, const Issue& b) {
            if (calculatePriority(a) != calculatePriority(b))
                return calculatePriority(a) > calculatePriority(b);

            return a.daysLeft < b.daysLeft;
        });

    cout << "\nIssues sorted by priority and deadline.\n";
}

// ===============================
// SEARCH ISSUE BY ID
// ===============================

void searchIssue(const vector<Issue>& issues) {
    int id;

    cout << "\nEnter GitHub Issue ID: ";

    if (!(cin >> id)) {
        cout << "Invalid ID.\n";
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        return;
    }

    auto it = find_if(issues.begin(), issues.end(),
        [id](const Issue& issue) {
            return issue.id == id;
        });

    if (it != issues.end()) {
        cout << "\nIssue Found!\n";

        vector<Issue> result = {*it};
        displayIssues(result);
    }
    else {
        cout << "Issue not found.\n";
    }
}

// ===============================
// DEPENDENCY GRAPH
// KAHN'S TOPOLOGICAL SORT
// ===============================

void dependencyOrder(
    const vector<Issue>& issues,
    const vector<vector<int>>& graph
) {
    int n = static_cast<int>(issues.size());

    vector<int> indegree(n, 0);

    for (int u = 0; u < n; u++) {
        for (int v : graph[u]) {
            indegree[v]++;
        }
    }

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

        for (int v : graph[u]) {
            indegree[v]--;

            if (indegree[v] == 0) {
                ready.push(v);
            }
        }
    }

    if (static_cast<int>(order.size()) != n) {
        cout << "\nDependency cycle detected!\n";
        return;
    }

    cout << "\n========== DEPENDENCY ORDER ==========\n";

    for (int index : order) {
        cout << "#" << issues[index].id
             << " - " << issues[index].title << '\n';
    }

    if (graph.empty() || order.size() == issues.size()) {
        cout << "\nNote: Only explicitly configured dependencies "
             << "are considered.\n";
    }
}

// ===============================
// GREEDY DEVELOPER ASSIGNMENT
// ===============================

void assignDevelopers(
    const vector<Issue>& originalIssues,
    const vector<Developer>& originalDevelopers
) {
    vector<Issue> issues = originalIssues;
    vector<Developer> developers = originalDevelopers;

    sortByPriority(issues);

    cout << "\n========== DEVELOPER ASSIGNMENTS ==========\n";

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
            cout << "#" << issue.id << " "
                 << issue.title
                 << " -> No eligible developer available\n";
        }
        else {
            developers[best].workload += issue.estimatedHours;

            cout << "#" << issue.id << " "
                 << issue.title
                 << " -> " << developers[best].name
                 << " | Priority: " << calculatePriority(issue)
                 << " | Workload: "
                 << developers[best].workload
                 << "/" << developers[best].maxHours
                 << " hours\n";
        }
    }

    cout << "\n========== FINAL DEVELOPER WORKLOAD ==========\n";

    for (const auto& dev : developers) {
        cout << dev.name << " (" << dev.skill << "): "
             << dev.workload << "/" << dev.maxHours
             << " hours\n";
    }
}

// ===============================
// MAIN FUNCTION
// ===============================

int main() {

    cout << "\n========================================\n";
    cout << "       OPEN SOURCE ISSUE TRIAGE\n";
    cout << "========================================\n";

    // Load GitHub issues from CSV
    vector<Issue> issues =
        loadIssuesFromCSV("data/issues.csv");

    if (issues.empty()) {
        cout << "\nNo issues loaded.\n";
        cout << "Check data/issues.csv and run the "
             << "Python converter first.\n";
        return 1;
    }

    cout << "\nSuccessfully loaded "
         << issues.size()
         << " GitHub issues!\n";

    // Developer data
    // These are demonstration developers and capacities.
    vector<Developer> developers = {
        {"Alice", "General", 0, 40},
        {"Bob", "General", 0, 40},
        {"Charlie", "General", 0, 40},
        {"Diana", "General", 0, 40}
    };

    // Dependency graph
    // Each edge A -> B means B depends on A.
    // No real dependency relationships are imported yet.
    int n = static_cast<int>(issues.size());

    vector<vector<int>> graph(n);

    int choice;

    do {
        cout << "\n========== MAIN MENU ==========\n";
        cout << "1. Display all issues\n";
        cout << "2. Sort issues by priority\n";
        cout << "3. Search issue by ID\n";
        cout << "4. Dependency order\n";
        cout << "5. Assign developers\n";
        cout << "0. Exit\n";

        cout << "\nEnter your choice: ";

        if (!(cin >> choice)) {
            cout << "Invalid input. Enter a number.\n";

            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');

            continue;
        }

        switch (choice) {

            case 1:
                displayIssues(issues);
                break;

            case 2:
                sortByPriority(issues);
                displayIssues(issues);
                break;

            case 3:
                searchIssue(issues);
                break;

            case 4:
                dependencyOrder(issues, graph);
                break;

            case 5:
                assignDevelopers(issues, developers);
                break;

            case 0:
                cout << "\nTriage program closed.\n";
                break;

            default:
                cout << "Invalid choice. Try again.\n";
        }

    } while (choice != 0);

    return 0;
}
