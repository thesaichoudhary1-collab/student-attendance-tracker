/* ============================================================
   Student Attendance & Grading Tracker
   ------------------------------------------------------------
   A terminal-based C program that:
     - Stores student records using a struct
     - Lets you add / view / save / load student data
     - Calculates attendance % and grade automatically
     - Uses file handling to write a styled report.html file
       (HTML + CSS) so the results look good in a browser
   ============================================================ */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_STUDENTS 100
#define NUM_ASSIGNMENTS 3
#define DATA_FILE "students.txt"
#define REPORT_FILE "report.html"

/* ---------- Struct: the heart of the data model ---------- */
typedef struct {
    char name[50];
    int  roll;
    int  totalClasses;
    int  classesAttended;
    float marks[NUM_ASSIGNMENTS];   /* marks out of 100 each */
} Student;

/* ---------- Global storage ---------- */
Student students[MAX_STUDENTS];
int studentCount = 0;

/* ---------- Function prototypes ---------- */
void addStudent(void);
void displayStudents(void);
void saveToFile(void);
void loadFromFile(void);
void generateHTMLReport(void);
float calculateAttendancePercent(Student s);
float calculateAverageMarks(Student s);
const char* calculateGrade(float avg);
void pause_screen(void);
void clearInputBuffer(void);
int findStudentIndexByRoll(int roll);
void deleteStudent(void);

/* ============================================================
   main() - menu driven loop
   ============================================================ */
int main(void) {
    int choice;

    loadFromFile();   /* auto-load any previously saved data */

    do {
        printf("\n===================================\n");
        printf("  STUDENT ATTENDANCE & GRADING TRACKER\n");
        printf("===================================\n");
        printf("1. Add Student\n");
        printf("2. View All Students (terminal)\n");
        printf("3. Delete a Student\n");
        printf("4. Save Records to File\n");
        printf("5. Generate HTML Report (report.html)\n");
        printf("6. Exit\n");
        printf("-----------------------------------\n");
        printf("Enter your choice: ");

        if (scanf("%d", &choice) != 1) {
            clearInputBuffer();
            choice = -1;
        }

        switch (choice) {
            case 1: addStudent(); break;
            case 2: displayStudents(); break;
            case 3: deleteStudent(); break;
            case 4: saveToFile(); break;
            case 5: generateHTMLReport(); break;
            case 6: 
                saveToFile();
                printf("\nData saved. Goodbye!\n");
                break;
            default:
                printf("\nInvalid choice, please try again.\n");
        }

    } while (choice != 6);

    return 0;
}

/* ============================================================
   addStudent() - takes input from the user and stores it
   ============================================================ */
void addStudent(void) {
    if (studentCount >= MAX_STUDENTS) {
        printf("\nStudent list is full! Cannot add more.\n");
        return;
    }

    Student s;
    clearInputBuffer();

    printf("\n--- Add New Student ---\n");
    printf("Enter Name: ");
    fgets(s.name, sizeof(s.name), stdin);
    s.name[strcspn(s.name, "\n")] = '\0';   /* strip newline */

    printf("Enter Roll Number: ");
    scanf("%d", &s.roll);

    /* prevent duplicate roll numbers */
    if (findStudentIndexByRoll(s.roll) != -1) {
        printf("A student with roll number %d already exists!\n", s.roll);
        return;
    }

    printf("Enter Total Classes Held: ");
    scanf("%d", &s.totalClasses);

    printf("Enter Classes Attended: ");
    scanf("%d", &s.classesAttended);

    if (s.classesAttended > s.totalClasses) {
        printf("Warning: attended classes exceeded total. Clamping value.\n");
        s.classesAttended = s.totalClasses;
    }

    for (int i = 0; i < NUM_ASSIGNMENTS; i++) {
        printf("Enter marks for Assignment %d (out of 100): ", i + 1);
        scanf("%f", &s.marks[i]);
    }

    students[studentCount] = s;
    studentCount++;

    printf("\nStudent '%s' added successfully!\n", s.name);
}

/* ============================================================
   deleteStudent() - removes a student by roll number
   ============================================================ */
void deleteStudent(void) {
    int roll;
    printf("\nEnter Roll Number to delete: ");
    scanf("%d", &roll);

    int idx = findStudentIndexByRoll(roll);
    if (idx == -1) {
        printf("No student found with roll number %d.\n", roll);
        return;
    }

    for (int i = idx; i < studentCount - 1; i++) {
        students[i] = students[i + 1];
    }
    studentCount--;
    printf("Student with roll number %d deleted.\n", roll);
}

/* ============================================================
   findStudentIndexByRoll() - helper, returns index or -1
   ============================================================ */
int findStudentIndexByRoll(int roll) {
    for (int i = 0; i < studentCount; i++) {
        if (students[i].roll == roll) return i;
    }
    return -1;
}

/* ============================================================
   calculateAttendancePercent()
   ============================================================ */
float calculateAttendancePercent(Student s) {
    if (s.totalClasses == 0) return 0.0f;
    return ((float)s.classesAttended / s.totalClasses) * 100.0f;
}

/* ============================================================
   calculateAverageMarks()
   ============================================================ */
float calculateAverageMarks(Student s) {
    float total = 0;
    for (int i = 0; i < NUM_ASSIGNMENTS; i++) total += s.marks[i];
    return total / NUM_ASSIGNMENTS;
}

/* ============================================================
   calculateGrade() - simple grading scale based on average
   ============================================================ */
const char* calculateGrade(float avg) {
    if (avg >= 90) return "A+";
    if (avg >= 80) return "A";
    if (avg >= 70) return "B";
    if (avg >= 60) return "C";
    if (avg >= 50) return "D";
    return "F";
}

/* ============================================================
   displayStudents() - prints a simple table in the terminal
   ============================================================ */
void displayStudents(void) {
    if (studentCount == 0) {
        printf("\nNo student records found.\n");
        return;
    }

    printf("\n%-20s %-6s %-12s %-8s %-6s\n",
           "Name", "Roll", "Attendance%", "Average", "Grade");
    printf("--------------------------------------------------------\n");

    for (int i = 0; i < studentCount; i++) {
        float att = calculateAttendancePercent(students[i]);
        float avg = calculateAverageMarks(students[i]);
        printf("%-20s %-6d %-12.2f %-8.2f %-6s\n",
               students[i].name, students[i].roll,
               att, avg, calculateGrade(avg));
    }
}

/* ============================================================
   saveToFile() - writes raw student data to students.txt
   so it can be reloaded next time the program runs
   ============================================================ */
void saveToFile(void) {
    FILE *fp = fopen(DATA_FILE, "w");
    if (fp == NULL) {
        printf("\nError: could not open %s for writing.\n", DATA_FILE);
        return;
    }

    fprintf(fp, "%d\n", studentCount);
    for (int i = 0; i < studentCount; i++) {
        Student s = students[i];
        fprintf(fp, "%s\n%d\n%d\n%d\n", s.name, s.roll,
                s.totalClasses, s.classesAttended);
        for (int j = 0; j < NUM_ASSIGNMENTS; j++) {
            fprintf(fp, "%.2f\n", s.marks[j]);
        }
    }

    fclose(fp);
    printf("\nRecords saved to %s\n", DATA_FILE);
}

/* ============================================================
   loadFromFile() - reads back students.txt if it exists
   ============================================================ */
void loadFromFile(void) {
    FILE *fp = fopen(DATA_FILE, "r");
    if (fp == NULL) return;   /* no saved data yet, that's fine */

    if (fscanf(fp, "%d\n", &studentCount) != 1) {
        studentCount = 0;
        fclose(fp);
        return;
    }

    for (int i = 0; i < studentCount; i++) {
        fgets(students[i].name, sizeof(students[i].name), fp);
        students[i].name[strcspn(students[i].name, "\n")] = '\0';

        fscanf(fp, "%d", &students[i].roll);
        fscanf(fp, "%d", &students[i].totalClasses);
        fscanf(fp, "%d", &students[i].classesAttended);
        for (int j = 0; j < NUM_ASSIGNMENTS; j++) {
            fscanf(fp, "%f", &students[i].marks[j]);
        }
        fgetc(fp); /* consume trailing newline before next fgets */
    }

    fclose(fp);
    printf("\nLoaded %d saved student record(s) from %s\n",
           studentCount, DATA_FILE);
}

/* ============================================================
   generateHTMLReport() - THE HTML CONNECTION
   Writes a fully styled HTML file with an embedded <style>
   block, using fprintf() to build the table row by row.
   ============================================================ */
void generateHTMLReport(void) {
    if (studentCount == 0) {
        printf("\nNo student data to report. Add students first.\n");
        return;
    }

    FILE *fp = fopen(REPORT_FILE, "w");
    if (fp == NULL) {
        printf("\nError: could not create %s\n", REPORT_FILE);
        return;
    }

    /* ---- HTML head + CSS ---- */
    fprintf(fp, "<!DOCTYPE html>\n<html lang=\"en\">\n<head>\n");
    fprintf(fp, "<meta charset=\"UTF-8\">\n");
    fprintf(fp, "<title>Student Attendance & Grading Report</title>\n");
    fprintf(fp, "<style>\n");
    fprintf(fp,
        "  body { font-family: 'Segoe UI', Arial, sans-serif; "
        "background: #f4f6fa; margin: 0; padding: 40px; color: #222; }\n"
        "  h1 { text-align: center; color: #2c3e50; margin-bottom: 5px; }\n"
        "  p.subtitle { text-align: center; color: #7f8c8d; margin-top: 0; }\n"
        "  table { border-collapse: collapse; width: 100%%; max-width: 1000px; "
        "margin: 30px auto; box-shadow: 0 4px 12px rgba(0,0,0,0.08); "
        "background: #fff; border-radius: 8px; overflow: hidden; }\n"
        "  th, td { padding: 12px 16px; text-align: center; }\n"
        "  th { background: #2c3e50; color: #ffffff; text-transform: uppercase; "
        "font-size: 13px; letter-spacing: 0.5px; }\n"
        "  tr:nth-child(even) { background: #f9fbfd; }\n"
        "  tr:hover { background: #eef3fb; }\n"
        "  td.name { text-align: left; font-weight: 600; }\n"
        "  .grade { font-weight: bold; padding: 4px 10px; border-radius: 12px; "
        "color: #fff; display: inline-block; min-width: 28px; }\n"
        "  .grade-A, .grade-Aplus { background: #27ae60; }\n"
        "  .grade-B { background: #2980b9; }\n"
        "  .grade-C { background: #f39c12; }\n"
        "  .grade-D { background: #e67e22; }\n"
        "  .grade-F { background: #c0392b; }\n"
        "  .att-good { color: #27ae60; font-weight: 600; }\n"
        "  .att-low  { color: #c0392b; font-weight: 600; }\n"
        "  footer { text-align: center; margin-top: 20px; color: #95a5a6; "
        "font-size: 12px; }\n"
    );
    fprintf(fp, "</style>\n</head>\n<body>\n");

    fprintf(fp, "<h1>Student Attendance & Grading Report</h1>\n");
    fprintf(fp, "<p class=\"subtitle\">Generated automatically by tracker.c "
                "&mdash; %d student(s)</p>\n", studentCount);

    /* ---- Table ---- */
    fprintf(fp, "<table>\n<tr>");
    fprintf(fp, "<th>Roll No.</th><th>Name</th><th>Classes Attended</th>"
                "<th>Attendance %%</th>");
    for (int j = 0; j < NUM_ASSIGNMENTS; j++) {
        fprintf(fp, "<th>Assignment %d</th>", j + 1);
    }
    fprintf(fp, "<th>Average</th><th>Grade</th></tr>\n");

    for (int i = 0; i < studentCount; i++) {
        Student s = students[i];
        float att = calculateAttendancePercent(s);
        float avg = calculateAverageMarks(s);
        const char *grade = calculateGrade(avg);

        /* pick a css-safe class name for the grade badge */
        char gradeClass[10];
        if (strcmp(grade, "A+") == 0) strcpy(gradeClass, "Aplus");
        else strcpy(gradeClass, grade);

        fprintf(fp, "<tr>");
        fprintf(fp, "<td>%d</td>", s.roll);
        fprintf(fp, "<td class=\"name\">%s</td>", s.name);
        fprintf(fp, "<td>%d / %d</td>", s.classesAttended, s.totalClasses);
        fprintf(fp, "<td class=\"%s\">%.1f%%</td>",
                (att >= 75.0f ? "att-good" : "att-low"), att);
        for (int j = 0; j < NUM_ASSIGNMENTS; j++) {
            fprintf(fp, "<td>%.1f</td>", s.marks[j]);
        }
        fprintf(fp, "<td>%.2f</td>", avg);
        fprintf(fp, "<td><span class=\"grade grade-%s\">%s</span></td>",
                gradeClass, grade);
        fprintf(fp, "</tr>\n");
    }

    fprintf(fp, "</table>\n");
    fprintf(fp, "<footer>Attendance below 75%% is highlighted in red. "
                "Report generated by the C Student Tracker project.</footer>\n");
    fprintf(fp, "</body>\n</html>\n");

    fclose(fp);
    printf("\nReport generated successfully: %s\n", REPORT_FILE);
    printf("Open it in any web browser to view the styled table.\n");
}

/* ============================================================
   Small utilities
   ============================================================ */
void clearInputBuffer(void) {
    int c;
    while ((c = getchar()) != '\n' && c != EOF) { }
}