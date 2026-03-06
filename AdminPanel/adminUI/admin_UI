

// File: admin_ui.cpp
// Build: g++ -std=c++17 -O2 -Wall -Wextra admin_ui.cpp -o admin_ui

#include <algorithm>
#include <cctype>
#include <iostream>
#include <optional>
#include <string>
#include <unordered_map>
#include <utility>

namespace rfid {

struct Student {
    std::string id;    // VARCHAR(20)
    std::string name;  // VARCHAR(100)
};

struct Machine {
    int id = 0;         // int
    std::string name;   // VARCHAR(100)
};

static std::string trim(std::string s) {
    auto not_space = [](unsigned char ch) { return !std::isspace(ch); };
    s.erase(s.begin(), std::find_if(s.begin(), s.end(), not_space));
    s.erase(std::find_if(s.rbegin(), s.rend(), not_space).base(), s.end());
    return s;
}

static std::string upper(std::string s) {
    for (char& c : s) c = static_cast<char>(std::toupper(static_cast<unsigned char>(c)));
    return s;
}

// ---------------- Repository abstraction (DB boundary) ----------------

class IRepository {
public:
    virtual ~IRepository() = default;

    virtual bool adminExists(const std::string& username) = 0;
    virtual bool createAdmin(const std::string& username, const std::string& password) = 0;
    virtual bool verifyAdmin(const std::string& username, const std::string& password) = 0;

    virtual bool createStudent(const Student& s) = 0;
    virtual std::optional<Student> findStudentById(const std::string& id) = 0;
    virtual std::optional<Student> findStudentByName(const std::string& name) = 0;
    virtual bool updateStudentById(const std::string& oldId, const Student& updated) = 0;

    virtual bool createMachine(const Machine& m) = 0;
    virtual std::optional<Machine> findMachineById(int id) = 0;
    virtual std::optional<Machine> findMachineByName(const std::string& name) = 0;
    virtual bool updateMachineById(int oldId, const Machine& updated) = 0;

    virtual std::optional<bool> getPermission(const std::string& studentId, int machineId) = 0;
    virtual bool setPermission(const std::string& studentId, int machineId, bool allowed) = 0;
};

// ---------------- In-memory demo repo (replace with DB calls later) ----------------

class InMemoryRepository final : public IRepository {
public:
    bool adminExists(const std::string& username) override {
        return admins_.count(username) > 0;
    }

    bool createAdmin(const std::string& username, const std::string& password) override {
        // TODO(DB): INSERT INTO AdminTable ... or AuthenticationTable equivalent
        if (admins_.count(username) > 0) return false;
        admins_[username] = password; // NOTE: store hashes in real system
        return true;
    }

    bool verifyAdmin(const std::string& username, const std::string& password) override {
        // TODO(DB): SELECT ... WHERE username=? AND passwordHash=?
        auto it = admins_.find(username);
        return it != admins_.end() && it->second == password;
    }

    bool createStudent(const Student& s) override {
        // TODO(DB): INSERT INTO StudentTable (StudentID, StudentName) VALUES (?,?)
        if (studentsById_.count(s.id) > 0) return false;
        studentsById_[s.id] = s;
        studentsByName_[normalizeKey(s.name)] = s.id;
        return true;
    }

    std::optional<Student> findStudentById(const std::string& id) override {
        // TODO(DB): SELECT ... FROM StudentTable WHERE StudentID=?
        auto it = studentsById_.find(id);
        if (it == studentsById_.end()) return std::nullopt;
        return it->second;
    }

    std::optional<Student> findStudentByName(const std::string& name) override {
        // TODO(DB): SELECT ... FROM StudentTable WHERE StudentName=?
        auto it = studentsByName_.find(normalizeKey(name));
        if (it == studentsByName_.end()) return std::nullopt;
        return findStudentById(it->second);
    }

    bool updateStudentById(const std::string& oldId, const Student& updated) override {
        // TODO(DB): UPDATE StudentTable SET StudentID=?, StudentName=? WHERE StudentID=?
        auto it = studentsById_.find(oldId);
        if (it == studentsById_.end()) return false;

        if (updated.id != oldId && studentsById_.count(updated.id) > 0) return false;

        studentsByName_.erase(normalizeKey(it->second.name));
        studentsById_.erase(it);

        studentsById_[updated.id] = updated;
        studentsByName_[normalizeKey(updated.name)] = updated.id;

        // If studentId changed, permissions keys should be migrated in a real DB via FK cascade.
        // Here we'll do a best-effort migration.
        if (updated.id != oldId) {
            std::unordered_map<std::string, bool> migrated;
            for (const auto& [key, val] : permissions_) {
                const auto [sid, mid] = splitPermKey(key);
                if (sid == oldId) {
                    migrated[permKey(updated.id, mid)] = val;
                } else {
                    migrated[key] = val;
                }
            }
            permissions_.swap(migrated);
        }

        return true;
    }

    bool createMachine(const Machine& m) override {
        // TODO(DB): INSERT INTO MachineTable (MachineID, MachineName) VALUES (?,?)
        if (machinesById_.count(m.id) > 0) return false;
        machinesById_[m.id] = m;
        machinesByName_[normalizeKey(m.name)] = m.id;
        return true;
    }

    std::optional<Machine> findMachineById(int id) override {
        // TODO(DB): SELECT ... FROM MachineTable WHERE MachineID=?
        auto it = machinesById_.find(id);
        if (it == machinesById_.end()) return std::nullopt;
        return it->second;
    }

    std::optional<Machine> findMachineByName(const std::string& name) override {
        // TODO(DB): SELECT ... FROM MachineTable WHERE MachineName=?
        auto it = machinesByName_.find(normalizeKey(name));
        if (it == machinesByName_.end()) return std::nullopt;
        return findMachineById(it->second);
    }

    bool updateMachineById(int oldId, const Machine& updated) override {
        // TODO(DB): UPDATE MachineTable SET MachineID=?, MachineName=? WHERE MachineID=?
        auto it = machinesById_.find(oldId);
        if (it == machinesById_.end()) return false;

        if (updated.id != oldId && machinesById_.count(updated.id) > 0) return false;

        machinesByName_.erase(normalizeKey(it->second.name));
        machinesById_.erase(it);

        machinesById_[updated.id] = updated;
        machinesByName_[normalizeKey(updated.name)] = updated.id;

        // Migrate permissions if machineId changed.
        if (updated.id != oldId) {
            std::unordered_map<std::string, bool> migrated;
            for (const auto& [key, val] : permissions_) {
                const auto [sid, mid] = splitPermKey(key);
                if (mid == oldId) {
                    migrated[permKey(sid, updated.id)] = val;
                } else {
                    migrated[key] = val;
                }
            }
            permissions_.swap(migrated);
        }

        return true;
    }

    std::optional<bool> getPermission(const std::string& studentId, int machineId) override {
        // TODO(DB): SELECT permission FROM AuthenticationTable WHERE StudentID=? AND MachineID=?
        auto it = permissions_.find(permKey(studentId, machineId));
        if (it == permissions_.end()) return std::nullopt;
        return it->second;
    }

    bool setPermission(const std::string& studentId, int machineId, bool allowed) override {
        // TODO(DB): UPSERT into AuthenticationTable (StudentID, MachineID, Permission)
        permissions_[permKey(studentId, machineId)] = allowed;
        return true;
    }

private:
    static std::string normalizeKey(const std::string& s) {
        std::string t = trim(s);
        for (char& c : t) c = static_cast<char>(std::tolower(static_cast<unsigned char>(c)));
        return t;
    }

    static std::string permKey(const std::string& studentId, int machineId) {
        return studentId + "#" + std::to_string(machineId);
    }

    static std::pair<std::string, int> splitPermKey(const std::string& key) {
        auto pos = key.find('#');
        if (pos == std::string::npos) return {key, 0};
        std::string sid = key.substr(0, pos);
        int mid = 0;
        try { mid = std::stoi(key.substr(pos + 1)); } catch (...) { mid = 0; }
        return {sid, mid};
    }

    std::unordered_map<std::string, std::string> admins_;
    std::unordered_map<std::string, Student> studentsById_;
    std::unordered_map<std::string, std::string> studentsByName_;
    std::unordered_map<int, Machine> machinesById_;
    std::unordered_map<std::string, int> machinesByName_;
    std::unordered_map<std::string, bool> permissions_;
};

// ---------------- Console UI ----------------

class AdminUI {
public:
    explicit AdminUI(IRepository& repo) : repo_(repo) {}

    void run() {
        while (true) {
            if (!loginFlow()) continue;
            if (!adminMenuLoop()) break; // true => go back to login, false => quit program
        }
        std::cout << "Goodbye.\n";
    }

private:
    IRepository& repo_;

    static std::string readLine(const std::string& prompt) {
        std::cout << prompt;
        std::string s;
        std::getline(std::cin, s);
        return s;
    }

    static std::string readNonEmpty(const std::string& prompt) {
        while (true) {
            std::string s = trim(readLine(prompt));
            if (!s.empty()) return s;
            std::cout << "Input cannot be empty. Try again.\n";
        }
    }

    static bool readYesNo(const std::string& prompt) {
        while (true) {
            std::string s = upper(trim(readLine(prompt)));
            if (s == "Y" || s == "YES") return true;
            if (s == "N" || s == "NO") return false;
            std::cout << "Please enter Y or N.\n";
        }
    }

    static char readChoice(const std::string& prompt, const std::string& allowed) {
        while (true) {
            std::string s = upper(trim(readLine(prompt)));
            if (!s.empty()) {
                char c = s[0];
                if (allowed.find(c) != std::string::npos) return c;
            }
            std::cout << "Invalid choice. Allowed: " << allowed << "\n";
        }
    }

    static int readInt(const std::string& prompt) {
        while (true) {
            std::string s = trim(readLine(prompt));
            try {
                size_t idx = 0;
                int val = std::stoi(s, &idx);
                if (idx == s.size()) return val;
            } catch (...) {}
            std::cout << "Please enter a valid integer.\n";
        }
    }

    static void pause() {
        (void)readLine("Press ENTER to continue...");
    }

    bool loginFlow() {
        std::cout << "\n=== RFID Power Delivery Admin Login ===\n";
        bool registered = readYesNo("Are you a registered Administrator? (Y/N): ");

        if (!registered) {
            std::cout << "\n--- Create Administrator Account ---\n";
            std::string username = readNonEmpty("Create Username: ");
            std::string password = readNonEmpty("Create Password: ");

            if (!repo_.createAdmin(username, password)) {
                std::cout << "Account creation failed (username may already exist).\n";
                pause();
                return false;
            }

            std::cout << "Account created. You are now logged in.\n";
            pause();
            return true;
        }

        std::cout << "\n--- Administrator Login ---\n";
        std::string username = readNonEmpty("Enter Username: ");
        std::string password = readNonEmpty("Enter Password: ");

        if (!repo_.verifyAdmin(username, password)) {
            std::cout << "Invalid credentials.\n";
            pause();
            return false;
        }

        std::cout << "Login successful.\n";
        pause();
        return true;
    }

    bool adminMenuLoop() {
        while (true) {
            std::cout
                << "\n=== Admin Screen ===\n"
                << "A) Create New Student\n"
                << "B) Edit Student\n"
                << "C) Create New Machine\n"
                << "D) Edit Machine\n"
                << "E) Logout to Login Screen\n"
                << "Q) Quit Program\n";

            char choice = readChoice("Choose option (A/B/C/D/E/Q): ", "ABCDEQ");
            switch (choice) {
                case 'A': createStudentFlow(); break;
                case 'B': editStudentFlow(); break;
                case 'C': createMachineFlow(); break;
                case 'D': editMachineFlow(); break;
                case 'E': return true;   // back to login
                case 'Q': return false;  // quit program
                default: break;
            }
        }
    }

    void createStudentFlow() {
        std::cout << "\n--- Create New Student ---\n";
        Student s;
        s.name = readNonEmpty("Enter Student Name: ");
        s.id = readNonEmpty("Enter Student ID: ");

        if (!repo_.createStudent(s)) {
            std::cout << "Failed to create student (ID may already exist).\n";
            pause();
            return;
        }

        std::cout << "Student created: " << s.name << " (ID: " << s.id << ")\n";

        if (readYesNo("Edit Machine Permissions now? (Y/N): ")) {
            studentOnMachineFlow(s.id);
        } else {
            std::cout << "No permissions added.\n";
        }

        pause();
    }

    void editStudentFlow() {
        std::cout << "\n--- Edit Student ---\n";
        char mode = readChoice("Search by (N)ame or (I)D? ", "NI");

        std::optional<Student> found;
        if (mode == 'N') {
            std::string name = readNonEmpty("Enter Student Name: ");
            found = repo_.findStudentByName(name);
        } else {
            std::string id = readNonEmpty("Enter Student ID: ");
            found = repo_.findStudentById(id);
        }

        if (!found) {
            std::cout << "This Student does not exist.\n";
            pause();
            return;
        }

        Student current = *found;
        std::cout << "Found: " << current.name << " (ID: " << current.id << ")\n";

        if (readYesNo("Would you like to change Student information? (Y/N): ")) {
            Student updated;
            updated.name = readNonEmpty("Enter NEW Student Name: ");
            updated.id = readNonEmpty("Enter NEW Student ID: ");

            if (!repo_.updateStudentById(current.id, updated)) {
                std::cout << "Update failed (new ID may already exist).\n";
                pause();
                return;
            }

            std::cout << "Student updated to: " << updated.name << " (ID: " << updated.id << ")\n";
            current = updated;
        } else {
            if (readYesNo("Would you like to change this student's permissions? (Y/N): ")) {
                studentOnMachineFlow(current.id);
            }
        }

        pause();
    }

    void createMachineFlow() {
        std::cout << "\n--- Create New Machine ---\n";
        Machine m;
        m.name = readNonEmpty("Enter new Machine Name: ");
        m.id = readInt("Enter new Machine ID (int): ");

        if (!repo_.createMachine(m)) {
            std::cout << "Failed to create machine (ID may already exist).\n";
            pause();
            return;
        }

        std::cout << "Machine created: " << m.name << " (ID: " << m.id << ")\n";
        pause();
    }

    void editMachineFlow() {
        std::cout << "\n--- Edit Machine ---\n";
        char mode = readChoice("Search by (N)ame or (I)D? ", "NI");

        std::optional<Machine> found;
        if (mode == 'N') {
            std::string name = readNonEmpty("Enter Machine Name: ");
            found = repo_.findMachineByName(name);
        } else {
            int id = readInt("Enter Machine ID (int): ");
            found = repo_.findMachineById(id);
        }

        if (!found) {
            std::cout << "This Machine does not exist.\n";
            pause();
            return;
        }

        Machine current = *found;
        std::cout << "Found: " << current.name << " (ID: " << current.id << ")\n";

        if (!readYesNo("Would you like to change Machine information? (Y/N): ")) {
            pause();
            return;
        }

        Machine updated;
        updated.name = readNonEmpty("Enter NEW Machine Name: ");
        updated.id = readInt("Enter NEW Machine ID (int): ");

        if (!repo_.updateMachineById(current.id, updated)) {
            std::cout << "Update failed (new ID may already exist).\n";
            pause();
            return;
        }

        std::cout << "Machine updated to: " << updated.name << " (ID: " << updated.id << ")\n";
        pause();
    }

    void studentOnMachineFlow(const std::string& studentId) {
        std::cout << "\n--- Student Machine Permission ---\n";

        auto student = repo_.findStudentById(studentId);
        if (!student) {
            std::cout << "Student no longer exists.\n";
            return;
        }

        int machineId = readInt("Enter Machine ID (int): ");
        auto machine = repo_.findMachineById(machineId);
        if (!machine) {
            std::cout << "Machine does not exist.\n";
            return;
        }

        auto currentPerm = repo_.getPermission(studentId, machineId);
        std::string state = currentPerm.has_value()
            ? (*currentPerm ? "HAS" : "DOES NOT HAVE")
            : "DOES NOT HAVE";

        std::cout
            << "\nCurrent status:\n"
            << "Student Name: " << student->name << "\n"
            << "Student ID:   " << student->id << "\n"
            << "Machine Name: " << machine->name << "\n"
            << "Machine ID:   " << machine->id << "\n"
            << "Access:       " << state << " access\n\n";

        bool allowed = readYesNo("Set permission to ALLOW access? (Y/N): ");
        if (!repo_.setPermission(studentId, machineId, allowed)) {
            std::cout << "Failed to set permission.\n";
            return;
        }

        std::cout << "Updated: student " << student->id << (allowed ? " ALLOWED" : " DENIED")
                  << " on machine " << machine->id << ".\n";
    }
};

} // namespace rfid

int main() {
    rfid::InMemoryRepository repo;
    rfid::AdminUI ui(repo);
    ui.run();
    return 0;
}