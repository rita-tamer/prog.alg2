#include <iostream>
#include <fstream>
#include <string>
#include <sstream>

struct Password {
    std::string label;
    std::string encryptedPassword;
    Password* next;

    Password(const std::string& lbl, const std::string& pwd) : label(lbl), encryptedPassword(pwd), next(nullptr) {}
};

struct User {
    std::string name;
    std::string email;
    std::string password; // Encrypted
    Password* keychain; // Pointer to the first password in the keychain
    User* next;

    User(const std::string& nm, const std::string& eml, const std::string& pwd) : name(nm), email(eml), password(pwd), keychain(nullptr), next(nullptr) {}
};
class PasswordManager {
private:
    User* usersHead; // Head pointer for the linked list of users

public:
    PasswordManager() : usersHead(nullptr) {}
    ~PasswordManager(); // Destructor to clean up memory
    void createUser(const std::string& name, const std::string& email, const std::string& password);
    void addPasswordToUser(const std::string& email, const std::string& label, const std::string& password);
    void displayKeychain(const std::string& email);
    void loadDatabase(const std::string& filename);
    void saveDatabase(const std::string& filename);
    bool userExists(const std::string& email, const std::string& password);
};
void PasswordManager::createUser(const std::string& name, const std::string& email, const std::string& password) {
    User* newUser = new User(name, email, password);
    // Insert at the beginning for simplicity
    newUser->next = usersHead;
    usersHead = newUser;
}
void PasswordManager::addPasswordToUser(const std::string& email, const std::string& label, const std::string& password) {
    for (User* cur = usersHead; cur != nullptr; cur = cur->next) {
        if (cur->email == email) {
            Password* newPassword = new Password(label, password);
            newPassword->next = cur->keychain;
            cur->keychain = newPassword;
            break;
        }
    }
}
void PasswordManager::displayKeychain(const std::string& email) {
    for (User* cur = usersHead; cur != nullptr; cur = cur->next) {
        if (cur->email == email) {
            std::cout << "Keychain for " << cur->name << ":\n";
            for (Password* pw = cur->keychain; pw != nullptr; pw = pw->next) {
                std::cout << "Label: " << pw->label << ", Password: " << pw->encryptedPassword << "\n";
            }
            return;
        }
    }
}

void PasswordManager::loadDatabase(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Failed to open file: " << filename << std::endl;
        return;
    }

    std::string line;
    while (getline(file, line)) {
        std::istringstream iss(line);
        std::string name, email, password;
        if (!(getline(iss, name, ',') && getline(iss, email, ',') && getline(iss, password))) {
            break; // Error or end of file
        }
        createUser(name, email, password);
    }
    file.close();
}

void PasswordManager::saveDatabase(const std::string& filename) {
    std::ofstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Failed to open file for writing: " << filename << std::endl;
        return;
    }

    for (User* cur = usersHead; cur != nullptr; cur = cur->next) {
        file << cur->name << "," << cur->email << "," << cur->password << std::endl;
        // Note: This does not save the passwords from the keychain.
        // Additional logic is needed to save those as well.
    }

    file.close();
}

PasswordManager::~PasswordManager() {
    User* curUser = usersHead;
    while (curUser != nullptr) {
        Password* curPassword = curUser->keychain;
        while (curPassword != nullptr) {
            Password* tempPassword = curPassword;
            curPassword = curPassword->next;
            delete tempPassword;
        }
        User* tempUser = curUser;
        curUser = curUser->next;
        delete tempUser;
    }
}

