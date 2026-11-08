
#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <queue>
#include <fstream>
#include <limits>
#include <unordered_map>
#include <set>

using namespace std;

// ==========================================
// 1. DATA STRUCTURES
// ==========================================

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

// ==========================================
// 2. CSV PARSER
// ==========================================

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

    // Remove Windows carriage return if present
    if (!fields.empty() && !fields.back().empty()
        && fields.back().back() == '\r') {
        fields.back().pop_back();
    }

    return fields;
}

// ==========================================
// 3. LOAD ISSUES
// ==========================================

vector<Issue> loadIssuesFromCSV(const string& filename) {
    vector<Issue> issues;
    ifstream file(filename);

    if (!file.is_open()) {
        cerr << "Cannot open " << filename << '\n';
        return issues;
    }

    string line;
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

            if (issue.id <= 0 ||
                issue.severity < 1 || issue.severity > 4 ||
                issue.urgency < 1 || issue.urgency > 5 ||
                issue.impact < 1 || issue.impact > 5 ||
                issue.daysLeft < 0 ||
                issue.estimatedHours < 0) {
                skipped++;
                continue;
            }

            issues.push_back(issue);
        }
        catch (const exception&) {
            skipped++;
        }
    }

    cout << "Invalid issue rows skipped: " << skipped << '\n';
    return issues;
}

// ==========================================
// 4. LOAD DEVELOPERS
// ==========================================

vector<Developer> loadDevelopersFromCSV(
    const string& filename
) {
    vector<Developer> developers;
    ifstream file(filename);

    if (!file.is_open()) {
        cerr << "Cannot open " << filename << '\n';
        return developers;
    }

    string line;
    getline(file, line);

    int skipped = 0;

    while (getline(file, line)) {
        vector<string> row = parseCSVLine(line);

        if (row.size() != 4) {
            skipped++;
            continue;
        }

        try {
            Developer dev;

            dev.name = row[0];
            dev.skill = row[1];
            dev.workload = stoi(row[2]);
            dev.maxHours = stoi(row[3]);

            if (dev.name.empty() || dev.skill.empty() ||
                dev.workload < 0 ||
                dev.maxHours < 0 ||
                dev.workload > dev.maxHours) {
                skipped++;
                continue;
            }

            developers.push_back(dev);
        }
        catch (const exception&) {
            skipped++;
        }
    }

    cout << "Invalid developer rows skipped: "
         << skipped << '\n';

    return developers;
}

// ==========================================
// 5. BUILD DEPENDENCY GRAPH
// ==========================================

// CSV format:
// issue_id,depends_on
//
// If B depends on A:
// Edge A -> B

vector<vector<int>> loadDependencyGraph(
    const string& filename,
    const vector<Issue>& issues
) {
    int n = static_cast<int>(issues.size());

    vector<vector<int>> graph(n);

    unordered_map<int, int> issueIndex;

    for (int i = 0; i < n; i++) {
        issueIndex[issues[i].id] = i;
    }

    ifstream file(filename);

    if (!file.is_open()) {
        cerr << "Cannot open " << filename << '\n';
        return graph;
    }

    string line;
    getline(file, line);

    set<pair<int, int>> existingEdges;

    int loaded = 0;
    int skipped = 0;

    while (getline(file, line)) {
        vector<string> row = parseCSVLine(line);

        if (row.size() != 2) {
            skipped++;
            continue;
        }

        try {
            int issueID = stoi(row[0]);
            int dependsOnID = stoi(row[1]);

            if (issueIndex.find(issueID) == issueIndex.end() ||
                issueIndex.find(dependsOnID) == issueIndex.end()) {
                cout << "Unknown issue ID: "
                     << issueID << " or " << dependsOnID << '\n';
                skipped++;
                continue;
            }

            if (issueID == dependsOnID) {
                cout << "Self-dependency rejected for #"
                     << issueID << '\n';
                skipped++;
                continue;
            }

            // Edge: dependency -> dependent issue
            int u = issueIndex[dependsOnID];
            int v = issueIndex[issueID];

            if (existingEdges.count({u, v})) {
                skipped++;
                continue;
            }

            graph[u].push_back(v);
            existingEdges.insert({u, v});
            loaded++;
        }
        catch (const exception&) {
            skipped++;
        }
    }

    cout << "Dependencies loaded: " << loaded << '\n';
    cout << "Invalid dependency rows skipped: "
         << skipped << '\n';

    return graph;
}

// ==========================================
// 6. PRIORITY CALCULATION
// ==========================================

int calculatePriority(const Issue& issue) {
    return 10 * issue.severity
         + 5 * issue.urgency
         + 3 * issue.impact;
}

// ==========================================
// 7. DISPLAY ISSUES
// ==========================================

void displayIssues(const vector<Issue>& issues) {
    cout << "\n========== ISSUE REPORT ==========\n";

    for (const auto& i : issues) {
        cout << "\n#" << i.id
             << " | " << i.title
             << "\nPriority: " << calculatePriority(i)
             << "\nSkill: " << i.requiredSkill
             << "\nEstimated Hours: " << i.estimatedHours
             << "\n----------------------------------\n";
    }

    cout << "Total Issues: " << issues.size() << '\n';
}

// ==========================================
// 8. SORT BY PRIORITY
// ==========================================

void sortByPriority(vector<Issue>& issues) {
    sort(issues.begin(), issues.end(),
        [](const Issue& a, const Issue& b) {
            if (calculatePriority(a) != calculatePriority(b))
                return calculatePriority(a) >
                       calculatePriority(b);

            return a.daysLeft < b.daysLeft;
        });

    cout << "\nIssues sorted successfully.\n";
}

// ==========================================
// 9. SEARCH ISSUE
// ==========================================

void searchIssue(const vector<Issue>& issues) {
    int id;

    cout << "Enter issue ID: ";

    if (!(cin >> id)) {
        cout << "Invalid ID.\n";
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        return;
    }

    auto it = find_if(
        issues.begin(), issues.end(),
        [id](const Issue& issue) {
            return issue.id == id;
        }
    );

    if (it != issues.end())
        displayIssues(vector<Issue>{*it});
    else
        cout << "Issue not found.\n";
}

// ==========================================
// 10. KAHN'S TOPOLOGICAL SORT
// ==========================================

void dependencyOrder(
    const vector<Issue>& issues,
    const vector<vector<int>>& graph
) {
    int n = static_cast<int>(issues.size());

    vector<int> indegree(n, 0);

    // Calculate indegree
    for (int u = 0; u < n; u++) {
        for (int v : graph[u]) {
            indegree[v]++;
        }
    }

    // Queue contains vertices with indegree zero
    queue<int> ready;

    for (int i = 0; i < n; i++) {
        if (indegree[i] == 0)
            ready.push(i);
    }

    vector<int> order;

    // Process graph
    while (!ready.empty()) {
        int u = ready.front();
        ready.pop();

        order.push_back(u);

        for (int v : graph[u]) {
            indegree[v]--;

            if (indegree[v] == 0)
                ready.push(v);
        }
    }

    cout << "\n========== DEPENDENCY ORDER ==========\n";

    // Cycle detection
    if (static_cast<int>(order.size()) != n) {
        cout << "Dependency cycle detected!\n";
        cout << "A valid execution order is not possible.\n";
        return;
    }

    bool hasEdges = false;

    for (const auto& edges : graph) {
        if (!edges.empty()) {
            hasEdges = true;
            break;
        }
    }

    if (!hasEdges) {
        cout << "No dependency relationships configured.\n";
        cout << "All issues are independent.\n";
        return;
    }

    cout << "Valid execution order:\n";

    for (int index : order) {
        cout << "#" << issues[index].id
             << " - " << issues[index].title << '\n';
    }
}

// ==========================================
// 11. GREEDY DEVELOPER ASSIGNMENT
// ==========================================

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

        for (int d = 0;
             d < static_cast<int>(developers.size()); d++) {

            const auto& dev = developers[d];

            bool correctSkill =
                dev.skill == issue.requiredSkill;

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
            cout << "#" << issue.id
                 << " -> No eligible developer\n";
        }
        else {
            developers[best].workload += issue.estimatedHours;

            cout << "#" << issue.id
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

// ==========================================
// 12. DISPLAY DEVELOPERS
// ==========================================

void displayDevelopers(const vector<Developer>& developers) {
    cout << "\n========== DEVELOPER LIST ==========\n";

    for (const auto& dev : developers) {
        cout << dev.name
             << " | Skill: " << dev.skill
             << " | Workload: " << dev.workload
             << "/" << dev.maxHours << " hours\n";
    }

    cout << "Total Developers: " << developers.size() << '\n';
}

// ==========================================
// 13. MAIN FUNCTION
// ==========================================

int main() {
    cout << "\n========================================\n";
    cout << "       OPEN SOURCE ISSUE TRIAGE V6\n";
    cout << "========================================\n";

    vector<Issue> issues =
        loadIssuesFromCSV("data/issues.csv");

    if (issues.empty()) {
        cout << "No issues loaded. Check issues.csv\n";
        return 1;
    }

    vector<Developer> developers =
        loadDevelopersFromCSV("data/developers.csv");

    if (developers.empty()) {
        cout << "No developers loaded. Check developers.csv\n";
        return 1;
    }

    vector<vector<int>> graph =
        loadDependencyGraph("data/dependencies.csv", issues);

    cout << "\nSuccessfully loaded "
         << issues.size() << " issues.\n";

    cout << "Successfully loaded "
         << developers.size() << " developers.\n";

    int choice;

    do {
        cout << "\n========== MAIN MENU ==========\n";
        cout << "1. Display all issues\n";
        cout << "2. Sort issues by priority\n";
        cout << "3. Search issue by ID\n";
        cout << "4. Dependency order\n";
        cout << "5. Assign developers\n";
        cout << "6. Display developers\n";
        cout << "7. Display dependency count\n";
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

            case 6:
                displayDevelopers(developers);
                break;

            case 7: {
                int count = 0;

                for (const auto& edges : graph)
                    count += static_cast<int>(edges.size());

                cout << "\nTotal dependency edges: "
                     << count << '\n';
                break;
            }

            case 0:
                cout << "\nTriage program closed.\n";
                break;

            default:
                cout << "Invalid choice.\n";
        }

    } while (choice != 0);

    return 0;
}
