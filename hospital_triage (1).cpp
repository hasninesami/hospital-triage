/*
 * Hospital Patient Management & Emergency Triage System
 * Data Structures: Max-Heap, Queue, Linked List
 * Instructor: Dr. Nafis Sadeq | CSE Department
 */

#include <iostream>
#include <string>
#include <vector>
#include <queue>
using namespace std;

#define RESET   "\033[0m"
#define RED     "\033[91m"
#define GREEN   "\033[92m"
#define YELLOW  "\033[93m"
#define CYAN    "\033[96m"
#define BOLD    "\033[1m"

// ---------- Linked List: Treatment Record ----------
struct Record {
    string note;
    Record* next;
    Record(string n) : note(n), next(nullptr) {}
};

// ---------- Patient ----------
struct Patient {
    int id, age, severity;
    string name, condition;
    bool isEmergency;
    Record* history;

    Patient(int id, string name, int age, int sev, string cond)
        : id(id), name(name), age(age), severity(sev),
          condition(cond), isEmergency(sev >= 6), history(nullptr) {}

    void addRecord(string note) {
        Record* r = new Record(note);
        if (!history) { history = r; return; }
        Record* cur = history;
        while (cur->next) cur = cur->next;
        cur->next = r;
    }

    void showRecords() {
        if (!history) { cout << "  No records.\n"; return; }
        Record* cur = history; int i = 1;
        while (cur) { cout << "  " << i++ << ". " << cur->note << "\n"; cur = cur->next; }
    }

    void print() {
        cout << CYAN << "  ID       : " << RESET << id << "\n";
        cout << CYAN << "  Name     : " << RESET << name << " (Age " << age << ")\n";
        cout << CYAN << "  Condition: " << RESET << condition << "\n";
        cout << CYAN << "  Severity : " << RESET;
        if      (severity >= 8) cout << RED    << severity << "/10  CRITICAL\n" << RESET;
        else if (severity >= 5) cout << YELLOW << severity << "/10  MODERATE\n" << RESET;
        else                    cout << GREEN  << severity << "/10  MILD\n"     << RESET;
        cout << CYAN << "  Type     : " << RESET;
        cout << (isEmergency ? RED "EMERGENCY\n" RESET : GREEN "REGULAR\n" RESET);
    }
};

// ---------- Heap Comparator (Max-Heap by severity) ----------
struct Cmp {
    bool operator()(Patient* a, Patient* b) { return a->severity < b->severity; }
};

// ---------- Globals ----------
vector<Patient*> db;
priority_queue<Patient*, vector<Patient*>, Cmp> emergHeap;
queue<Patient*> regQueue;
int nextID = 1001, totalE = 0, totalR = 0, served = 0;

Patient* findPatient(int id) {
    for (auto p : db) if (p->id == id) return p;
    return nullptr;
}

// ---------- 1. Register ----------
void registerPatient() {
    string name, cond; int age, sev;
    cout << "\n--- Register Patient ---\n";
    cout << "  Name       : "; cin.ignore(); getline(cin, name);
    cout << "  Age        : "; cin >> age;
    cout << "  Condition  : "; cin.ignore(); getline(cin, cond);
    cout << "  Severity (1-10): "; cin >> sev;
    sev = max(1, min(10, sev));

    Patient* p = new Patient(nextID++, name, age, sev, cond);
    db.push_back(p);
    if (p->isEmergency) { emergHeap.push(p); totalE++; cout << RED   << "\n  >> EMERGENCY queue.\n" << RESET; }
    else                { regQueue.push(p);  totalR++; cout << GREEN  << "\n  >> REGULAR queue.\n"   << RESET; }
    p->print();
}

// ---------- 2. Serve Next ----------
void serveNext() {
    cout << "\n--- Serve Next Patient ---\n";
    Patient* p = nullptr;
    if      (!emergHeap.empty()) { p = emergHeap.top(); emergHeap.pop(); cout << RED   << "  Serving EMERGENCY:\n" << RESET; }
    else if (!regQueue.empty())  { p = regQueue.front(); regQueue.pop(); cout << GREEN << "  Serving REGULAR:\n"   << RESET; }
    else { cout << YELLOW << "  No patients waiting.\n" << RESET; return; }
    served++;
    p->addRecord("Called for consultation");
    p->print();
}

// ---------- 3. Update Severity ----------
void updateSeverity() {
    cout << "\n--- Update Severity ---\n";
    cout << "  Patient ID: "; int id; cin >> id;
    Patient* p = findPatient(id);
    if (!p) { cout << RED << "  Not found.\n" << RESET; return; }
    cout << "  Current: " << p->severity << "  New (1-10): "; int s; cin >> s;
    p->severity = max(1, min(10, s));
    p->isEmergency = (p->severity >= 6);
    if (p->isEmergency) { emergHeap.push(p); cout << RED << "  Updated & pushed to emergency heap.\n" << RESET; }
    else cout << GREEN << "  Severity updated.\n" << RESET;
}

// ---------- 4. Add Record ----------
void addRecord() {
    cout << "\n--- Add Treatment Record ---\n";
    cout << "  Patient ID: "; int id; cin >> id; cin.ignore();
    Patient* p = findPatient(id);
    if (!p) { cout << RED << "  Not found.\n" << RESET; return; }
    cout << "  Note: "; string note; getline(cin, note);
    p->addRecord(note);
    cout << GREEN << "  Record added.\n" << RESET;
}

// ---------- 5. View Records ----------
void viewRecords() {
    cout << "\n--- Medical Records ---\n";
    cout << "  Patient ID: "; int id; cin >> id;
    Patient* p = findPatient(id);
    if (!p) { cout << RED << "  Not found.\n" << RESET; return; }
    p->print();
    cout << "  -- Treatment History --\n";
    p->showRecords();
}

// ---------- 6. View Waiting ----------
void viewWaiting() {
    cout << "\n--- Waiting Patients ---\n";
    cout << RED << "\n  EMERGENCY QUEUE (by severity):\n" << RESET;
    auto eh = emergHeap; int i = 1;
    if (eh.empty()) cout << "  (empty)\n";
    while (!eh.empty()) {
        Patient* p = eh.top(); eh.pop();
        cout << "  " << i++ << ". " << p->name << " | ID:" << p->id << " | Sev:" << p->severity << "\n";
    }
    cout << GREEN << "\n  REGULAR QUEUE (FIFO):\n" << RESET;
    auto rq = regQueue; i = 1;
    if (rq.empty()) cout << "  (empty)\n";
    while (!rq.empty()) {
        Patient* p = rq.front(); rq.pop();
        cout << "  " << i++ << ". " << p->name << " | ID:" << p->id << " | " << p->condition << "\n";
    }
}

// ---------- 7. Statistics ----------
void showStats() {
    cout << "\n--- Statistics ---\n";
    cout << CYAN   << "  Total Registered : " << RESET << (totalE + totalR) << "\n";
    cout << RED    << "  Emergency Cases  : " << RESET << totalE << "\n";
    cout << GREEN  << "  Regular Cases    : " << RESET << totalR << "\n";
    cout << YELLOW << "  Total Served     : " << RESET << served << "\n";
    cout << CYAN   << "  Still Waiting    : " << RESET << (emergHeap.size() + regQueue.size()) << "\n";
}

// ---------- Main ----------
int main() {
    cout << BOLD << CYAN;
    cout << "\n  ==========================================\n";
    cout << "    HOSPITAL PATIENT MANAGEMENT SYSTEM\n";
    cout << "    Emergency Triage & Record Keeper\n";
    cout << "  ==========================================\n" << RESET;

    // Sample patients
    auto load = [&](string name, int age, int sev, string cond) {
        Patient* p = new Patient(nextID++, name, age, sev, cond);
        db.push_back(p);
        if (p->isEmergency) { emergHeap.push(p); totalE++; }
        else                { regQueue.push(p);  totalR++; }
    };
    load("Rahim Uddin",   45, 9, "Cardiac Arrest");
    load("Fatema Begum",  30, 3, "Mild Fever");
    load("Karim Mia",     60, 7, "Stroke Symptoms");
    db[0]->addRecord("Rushed to ICU");
    cout << GREEN << "\n  3 sample patients loaded.\n" << RESET;

    int choice;
    do {
        cout << BOLD << "\n  ==========================================\n";
        cout << "   [1] Register        [2] Serve Next\n";
        cout << "   [3] Update Severity [4] Add Record\n";
        cout << "   [5] View Records    [6] View Waiting\n";
        cout << "   [7] Statistics      [0] Exit\n";
        cout << "  ==========================================\n" << RESET;
        cout << "  Choice: "; cin >> choice;

        switch (choice) {
            case 1: registerPatient(); break;
            case 2: serveNext();       break;
            case 3: updateSeverity();  break;
            case 4: addRecord();       break;
            case 5: viewRecords();     break;
            case 6: viewWaiting();     break;
            case 7: showStats();       break;
            case 0: cout << CYAN << "\n  Goodbye!\n\n" << RESET; break;
            default: cout << RED << "  Invalid choice.\n" << RESET;
        }
    } while (choice != 0);

    for (auto p : db) delete p;
    return 0;
}
