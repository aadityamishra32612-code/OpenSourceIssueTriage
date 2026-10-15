
#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
using namespace std;

struct Issue {
    int id;
    string title;
    int severity; // 1 = Low, 2 = Medium, 3 = High, 4 = Critical
    int daysLeft;
};

void displayIssues(const vector<Issue>& issues) {
    cout << "\n--- Open Source Issues ---\n";

    for (const auto& issue : issues) {
        string level;

        switch (issue.severity) {
            case 1: level = "Low"; break;
            case 2: level = "Medium"; break;
            case 3: level = "High"; break;
            case 4: level = "Critical"; break;
            default: level = "Unknown";
        }

        cout << "ID: " << issue.id
             << " | Title: " << issue.title
             << " | Severity: " << level
             << " | Days left: " << issue.daysLeft << '\n';
    }
}

int main() {
    vector<Issue> issues = {
        {101, "Login failure", 4, 1},
        {102, "Typo in documentation", 1, 10},
        {103, "Database timeout", 3, 2},
        {104, "Broken search feature", 2, 3},
        {105, "Security vulnerability", 4, 2}
    };

    int choice;

    do {
        cout << "\n=== Open Source Issue Triage ===\n";
        cout << "1. Display all issues\n";
        cout << "2. Sort issues by severity\n";
        cout << "3. Search issue by ID\n";
        cout << "0. Exit\n";
        cout << "Enter choice: ";

        if (!(cin >> choice)) {
            cout << "Invalid input. Please enter a number.\n";
            return 1;
        }

        if (choice == 1) {
            displayIssues(issues);
        }
        else if (choice == 2) {
            sort(issues.begin(), issues.end(),
                 [](const Issue& a, const Issue& b) {
                     if (a.severity != b.severity)
                         return a.severity > b.severity;

                     return a.daysLeft < b.daysLeft;
                 });

            cout << "\nIssues sorted by severity and deadline.\n";
            displayIssues(issues);
        }
        else if (choice == 3) {
            int searchId;
            cout << "Enter issue ID: ";
            cin >> searchId;

            auto it = find_if(issues.begin(), issues.end(),
                [searchId](const Issue& issue) {
                    return issue.id == searchId;
                });

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
