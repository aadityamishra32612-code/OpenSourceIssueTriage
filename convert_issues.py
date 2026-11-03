
import json
import csv
from pathlib import Path

INPUT = Path("data/github_issues.json")
OUTPUT = Path("data/issues.csv")

def get_severity(labels):
    names = [label["name"].lower() for label in labels]

    if any("critical" in x or "security" in x for x in names):
        return 4
    if any("high" in x or "bug" in x for x in names):
        return 3
    if any("medium" in x for x in names):
        return 2
    return 1

def convert():
    with INPUT.open("r", encoding="utf-8") as file:
        issues = json.load(file)

    # Remove pull requests. GitHub's issues API returns both.
    if issues and isinstance(issues[0], list):
        issues = [item for page in issues for item in page]

    rows = []

    for issue in issues:
        if "pull_request" in issue:
            continue

        labels = issue.get("labels", [])
        severity = get_severity(labels)

        rows.append({
            "id": issue["number"],
            "title": issue.get("title", "").replace("\n", " "),
            "severity": severity,
            "urgency": 3,
            "impact": 3,
            "daysLeft": 7,
            "requiredSkill": "General",
            "estimatedHours": 2
        })

    with OUTPUT.open("w", newline="", encoding="utf-8") as file:
        writer = csv.DictWriter(
            file,
            fieldnames=[
                "id", "title", "severity", "urgency",
                "impact", "daysLeft", "requiredSkill",
                "estimatedHours"
            ]
        )
        writer.writeheader()
        writer.writerows(rows)

    print(f"Successfully converted {len(rows)} issues.")
    print(f"CSV saved to: {OUTPUT}")

if __name__ == "__main__":
    convert()
