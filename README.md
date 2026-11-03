# 🚀 Open Source Issue Triage System

A C++ based project that helps organize, prioritize, search, and assign open-source software issues using **Design and Analysis of Algorithms (DAA)** concepts.

The system fetches real GitHub issues, converts the data into CSV format, and processes them using sorting, searching, graph algorithms, and greedy techniques.

---

## 📌 Project Overview

Open-source repositories often contain hundreds or thousands of unresolved issues. Managing these issues manually can be time-consuming.

The Open Source Issue Triage System aims to simplify this process by:

- Fetching real issues from GitHub repositories.
- Calculating priority scores for issues.
- Sorting issues based on priority.
- Searching issues using their IDs.
- Analyzing issue dependencies using graph algorithms.
- Assigning issues to developers using a greedy approach.

## ✨ Features

| Feature | Description |
|---|---|
| GitHub Integration | Fetches real issues using GitHub CLI |
| CSV Data Processing | Converts JSON issue data into CSV |
| Issue Display | Displays imported GitHub issues |
| Priority Calculation | Calculates issue priority scores |
| Sorting | Sorts issues by priority and deadline |
| Searching | Finds issues using issue IDs |
| Dependency Analysis | Uses topological sorting and cycle detection |
| Developer Assignment | Assigns issues using a greedy algorithm |

## 🧠 Algorithms Used

### 1. Priority Calculation

A weighted scoring formula is used:

`Priority = 10 × Severity + 5 × Urgency + 3 × Impact`

Higher scores indicate higher priority.

### 2. Sorting Algorithm

**Algorithm:** STL Sort (Introsort implementation)

Issues are sorted in descending order of priority. If priorities are equal, issues with fewer days remaining are placed first.

**Time Complexity:** O(n log n)

### 3. Searching Algorithm

**Algorithm:** Linear Search

The system searches for an issue using its unique GitHub issue ID.

**Time Complexity:** O(n)

### 4. Graph Algorithm

**Algorithm:** Kahn's Topological Sorting

Used to determine a valid execution order for dependent issues.

It also detects dependency cycles using the number of processed vertices.

**Time Complexity:** O(V + E)

### 5. Greedy Algorithm

**Algorithm:** Greedy Developer Assignment

Issues are processed in priority order. Each issue is assigned to an eligible developer with the lowest current workload, provided the developer has the required skill and sufficient capacity.

This is a heuristic approach and does not guarantee a globally optimal assignment.

**Time Complexity:** O(n log n + nd)

Where:
- n = Number of issues
- d = Number of developers

---

## 🛠️ Technologies Used

- **C++17:** Core application and algorithm implementation
- **Python 3:** GitHub JSON to CSV conversion
- **GitHub CLI:** Fetching GitHub issue data
- **CSV:** Structured issue dataset
- **Git:** Version control
- **VS Code:** Development environment

## 📂 Project Structure

```text
opensource-issue-triage/
│
├── data/
│   ├── github_issues.json
│   └── issues.csv
│
├── main_ver1.cpp
├── main_ver2.cpp
├── main_ver3.cpp
├── main_ver4.cpp
│
├── convert_issues.py
├── README.md
└── .gitignore
```

## ⚙️ Installation and Setup

### Prerequisites

Make sure you have installed:

- Git
- GitHub CLI
- Python 3
- G++ compiler with C++17 support

### Step 1: Clone the repository

```bash
git clone https://github.com/aadityamishra32612-code/OpenSourceIssueTriage
