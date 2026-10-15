
#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
using namespace std;

struct Issue {
    int id;
    string title;
    int severity;  // 1 = Low, 4 = Critical
    int urgency;   // 1 = Low, 5 = Immediate
    int impact;    // 1 = Low, 5 = Very High
    int daysLeft;
};

// Calculate the priority score
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

// Display issues and their scores
void displayIssues(const vector<Issue>& issues) {
    cout << "\n--- Issue Priority Report ---\n";

    for (const auto& issue : issues) {
        cout << "ID: " << issue.id
             << " | " << issue.title
             << " | Severity: "
             << severityLabel(issue.severity)
             << " | Urgency: " << issue.urgency
             << " | Impact: " << issue.impact
             << " | Priority: "
             << calculatePriority(issue)
             << " | Days left: " << issue.daysLeft
             << '\n';
    }
}

// Sort by priority score, then by deadline
void sortByPriority(vector<Issue>& issues) {
    sort(issues.begin(), issues.end(),
         [](const Issue& a, const Issue& b) {
             int scoreA = calculatePriority(a);
             int scoreB = calculatePriority(b);

             if (scoreA != scoreB)
                 return scoreA > scoreB;

             return a.daysLeft < b.daysLeft;
         });
}

int main() {
    vector<Issue> issues = {
        {101, "Login failure", 4, 4, 4, 1},
        {102, "Documentation typo", 1, 1, 1, 10},
        {103, "Database timeout", 3, 4, 4, 2},
        {104, "Broken search feature", 2, 3, 3, 3},
        {105, "Security vulnerability", 4, 5, 5, 2}
    };

    int choice;

    do {
        cout << "\n=== Open Source Issue Triage V2 ===\n";
        cout << "1. Display all issues and scores\n";
        cout << "2. Sort by priority score\n";
        cout << "3. Search issue by ID\n";
        cout << "0. Exit\n";
        cout << "Enter choice: ";

        if (!(cin >> choice)) {
            cout << "Invalid input. Enter a number.\n";
            return 1;
        }

        if (choice == 1) {
            displayIssues(issues);
        }
        else if (choice == 2) {
            sortByPriority(issues);
            cout << "\nIssues ranked by priority!\n";
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
                }
            );

            if (it != issues.end()) {
                displayIssues(vector<Issue>{*it});
            } else {
                cout << "Issue not found.\n";
            }
        }
        else if (choice != 0) {
            cout << "Invalid choice. Try again.\n";
        }

    } while (choice != 0);

    cout << "Triage program closed.\n";
    return 0;
}
