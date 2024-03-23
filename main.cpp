#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <limits>
#include <cstdlib> // For system commands
#include <ctime>   // For seeding the random number generator
#include <filesystem> // For performing operations on file systems & their components

#define MAX_USERS 100 // Define a maximum number of users

// Linked list containing a label and a password
struct PasswordNode {
    std::string label;
    std::string password;
    PasswordNode* next;

    PasswordNode(std::string lbl, std::string pwd) : label(lbl), password(pwd), next(nullptr) {}
};

// Representing the user, email and linked lists of passwords 
struct User {
    std::string name;
    std::string email;
    std::string password; // User's own password to access the manager
    PasswordNode* passwordsHead; // Head of the linked list of passwords

    User(std::string nm, std::string eml, std::string pwd)
        : name(nm), email(eml), password(pwd), passwordsHead(nullptr) {}
};

// Tracking the current number of users to ensure it does not pass the limit
User* users[MAX_USERS]; // Array to hold user pointers
int userCount = 0; 

// Finds and removes passwords from a user's linked list of passwords
void removePassword(User& user, const std::string& label) {
    PasswordNode* curr = user.passwordsHead, * prev = nullptr;
    while (curr != nullptr && curr->label != label) {
        prev = curr;
        curr = curr->next;
    }
    if (curr == nullptr) return; // Not found
    if (prev == nullptr) {
        // Removing the first node
        user.passwordsHead = curr->next;
    }
    else {
        prev->next = curr->next;
    }
    delete curr; // Free the removed node
    std::cout << "You have successfully removed your password! \n";
}

// Deletes passwords from the user's file 
void deletePassword(User& user, const std::string& label) {
    PasswordNode* curr = user.passwordsHead, * prev = nullptr;
    while (curr != nullptr && curr->label != label) {
        prev = curr;
        curr = curr->next;
    }
    if (curr == nullptr) {
        std::cout << "Password label not found.\n";
        return;
    }
    if (prev == nullptr) user.passwordsHead = curr->next;
    else prev->next = curr->next;
    delete curr;
    // Rewrite the user's file
    std::ofstream userFile(user.email + ".txt", std::ios::trunc);
    if (userFile.is_open()) {
        userFile << user.email << '\n' << user.password << '\n' << user.name << '\n';
        for (PasswordNode* node = user.passwordsHead; node != nullptr; node = node->next) {
            userFile << node->label << '\n' << node->password << '\n';
        }
        userFile.close();
    }
    else {
        std::cout << "Failed to open the file for updating passwords.\n";
    }
    std::cout << "You have successfully removed your password! \n";
}

// Random Password Generator
std::string generateRandomPassword() {
    std::string chars =
        "0123456789"
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
        "abcdefghijklmnopqrstuvwxyz"
        "!@#$%^&*()";
    std::string password;
    srand(static_cast<unsigned>(time(nullptr)));
    int length = rand() % 5 + 8; // Length between 8 and 12
    for (int i = 0; i < length; ++i) {
        password += chars[rand() % chars.length()];
    }
    return password;
}

// Password Encryption and Decryption through Caesar Cipher
std::string caesarEncrypt(const std::string& text, int shift = 3) {
    std::string encrypted = "";
    for (char c : text) {
        if (isalpha(c)) {
            char base = islower(c) ? 'a' : 'A';
            encrypted += static_cast<char>(((c - base + shift) % 26) + base);
        } else {
            encrypted += c;
        }
    }
    return encrypted;
}
std::string caesarDecrypt(const std::string& text, int shift = 3) {
    return caesarEncrypt(text, 26 - shift); // Utilize encryption in reverse for decryption
}
const int SHIFT = 3;

// Allows users to update their passwords
void updatePassword(User& user, const std::string& label) {
    PasswordNode* curr = user.passwordsHead;
    while (curr != nullptr) {
        if (curr->label == label) {
            std::string oldPassword;
            std::cout << "Enter the old password for label " << label << ": ";
            std::cin >> oldPassword;
            if (caesarDecrypt(curr->password, SHIFT) != oldPassword) { // Decrypt and compare
                std::cout << "Incorrect old password.\n";
                return;
            }
            std::string newPassword;
            std::cout << "Enter the new password: ";
            std::cin >> newPassword;
            curr->password = caesarEncrypt(newPassword, SHIFT); // Encrypt new password
            std::cout << "Password updated.\n";
            return;
        }
        curr = curr->next;
    }

    std::cout << "Password label not found.\n";
}

// Displays the user's passwords - Decrypting them to plain text
void displayUserPasswords(const User& user) {
    std::cout << user.name << "'s passwords:\n";
    for (PasswordNode* curr = user.passwordsHead; curr != nullptr; curr = curr->next) {
        // Decrypt here inside the loop
        std::string decryptedPassword = caesarDecrypt(curr->password, SHIFT);
        std::cout << "Label: " << curr->label << ", Password: " << decryptedPassword << "\n";
    }
}

// Forward Declaration of ShowMenu
void showMenu(User& user); 

// Adding a password to the user's file & encrypting it
void addPassword(User& user, const std::string& label, const std::string& password) {
    // Encrypt the password before adding
    std::string encryptedPassword = caesarEncrypt(password, SHIFT);
    PasswordNode* newNode = new PasswordNode(label, encryptedPassword);
    if (user.passwordsHead == nullptr) {
        user.passwordsHead = newNode;
    } else {
        PasswordNode* current = user.passwordsHead;
        while (current->next != nullptr) {
            current = current->next;
        }
        current->next = newNode;
    }
    std::ofstream userFile(user.email + ".txt", std::ios::app); 
    if (userFile.is_open()) {
        userFile << label << '\n' << encryptedPassword << '\n';
        userFile.close();
    } else {
        std::cout << "Failed to open the file for appending a new password.\n";
    }
    std::cout << "Password for '" << label << "' added.\n";
}

// Loads the user's data 
void loadUserData() {
    userCount = 0; // Reset user count in case this function is called multiple times
    for (const auto& entry : std::filesystem::directory_iterator(".")) {
        if (entry.is_regular_file() && entry.path().extension() == ".txt") {
            std::ifstream userFile(entry.path());
            if (!userFile.is_open()) continue; // Skip if the file cannot be opened
            std::string email, password, name;
            std::getline(userFile, email);
            std::getline(userFile, password);
            std::getline(userFile, name);
            // Create a new user and add them to the users array
            User* newUser = new User(name, email, password);
            users[userCount++] = newUser; // Add user to the array and increment count
            // Load passwords into the user's password list
            std::string label, Spassword;
            while (std::getline(userFile, label) && std::getline(userFile, Spassword)) {
                PasswordNode* newNode = new PasswordNode(label, Spassword);
                if (newUser->passwordsHead == nullptr) {
                    newUser->passwordsHead = newNode;
                }
                else {
                    PasswordNode* current = newUser->passwordsHead;
                    while (current->next != nullptr) {
                        current = current->next;
                    }
                    current->next = newNode;
                }
            }
            userFile.close();
        }
        if (userCount >= MAX_USERS) break; // Stop if the maximum number of users is reached
    }
}

// Signs Up a new user
void newUser() {
    if (userCount >= MAX_USERS) {
        std::cerr << "Maximum number of users reached. Cannot register more users.\n";
        return;
    }
    std::string name, email, password, confirmPassword;
    std::cout << "Enter your name: ";
    getline(std::cin, name);
    std::cout << "Enter your email: ";
    getline(std::cin, email);
    // Check if the email already exists
    for (int i = 0; i < userCount; i++) {
        if (users[i]->email == email) {
            std::cout << "A user with this email already exists. Please try a different email.\n";
            return;
        }
    }
    std::cout << "Set your password: ";
    getline(std::cin, password);
    std::cout << "Confirm your password: ";
    getline(std::cin, confirmPassword);
    if (password != confirmPassword) {
        std::cout << "Passwords do not match. Please try again.\n";
        return;
    }
    std::string encryptedPassword = caesarEncrypt(password, SHIFT); 
    User* newUser = new User(name, email, encryptedPassword);
    users[userCount++] = newUser;
    std::ofstream userFile(email + ".txt");
    if (userFile.is_open()) {
        userFile << email << '\n' << encryptedPassword << '\n' << name << '\n'; // Stores encrypted info
        userFile.close();
    } else {
        std::cerr << "Error creating file for new user.\n";
    }
    std::cout << "Thank you for signing up with us, " << name << "!\n";
    showMenu(*newUser);
}

// Logs in users
void returningUser() {
    std::string email, inputPassword;
    std::cout << "Enter your email: ";
    getline(std::cin, email);
    std::cout << "Enter your password: ";
    getline(std::cin, inputPassword);
    for (int i = 0; i < userCount; ++i) {
        if (users[i]->email == email) {
            // Correctly use the encrypted password from the user struct
            std::string decryptedUserPassword = caesarDecrypt(users[i]->password, SHIFT);
            if (decryptedUserPassword == inputPassword) {
                std::cout << "Welcome back, " << users[i]->name << "!\n";
                showMenu(*users[i]);
                return;
            } else {
                std::cout << "Invalid password.\n";
                return;
            }
        }
    }
    std::cout << "User not found.\n";
}

// Lists the program's options to the user
void showMenu(User& user) {
    int choice;
    do {
        std::cout << "\nHello, " << user.name << "! What would you like to do today?\n";
        std::cout << "1. View your registered passwords\n";
        std::cout << "2. Add a new password to your keychain\n";
        std::cout << "3. Update one of your passwords\n";
        std::cout << "4. Delete one of the registered passwords\n";
        std::cout << "5. Exit the application\nChoose an option: ";
        std::cin >> choice;
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        switch (choice) {
        case 1:
            displayUserPasswords(user);
            break;
        case 2: {
            std::string label, password;
            std::cout << "Please enter a label for your new password: ";
            std::getline(std::cin, label);
            std::cout << "Would you like to generate a new password? (yes/no): ";
            std::string decision;
            std::getline(std::cin, decision);
            if (decision == "yes" || decision == "Yes") {
                password = generateRandomPassword();
                std::cout << "Generated Password: " << password << '\n';
            }
            else {
                std::cout << "Enter your password: ";
                std::getline(std::cin, password);
            }
            addPassword(user, label, password);
            break;
        }
        case 3: {
            displayUserPasswords(user); // Display passwords before updating
            std::string label;
            std::cout << "Enter the label of the password you want to update: ";
            getline(std::cin, label);
            updatePassword(user, label);
            break; // Make sure to break after each case
        }
        case 4: {
            displayUserPasswords(user); // Display passwords before deleting
            std::string label;
            std::cout << "Enter the label of the password you want to delete: ";
            getline(std::cin, label);
            deletePassword(user, label);
            break; // Ensure there is a break here
        }
        case 5:
            std::cout << "Goodbye, See you soon!\n";
            return;
        }
    } while (true);
}

// Initializes the program
int main() {
    loadUserData();
    std::cout << "Welcome to Rita's Password Manager!\n";
    std::cout << "1. New user\n2. Returning user\nChoose an option: ";
    int option;
    std::cin >> option;
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // Clear buffer

    if (option == 1) {
        newUser(); // Assume newUser() is modified to call showMenu() with appropriate parameters.
    }
    else if (option == 2) {
        returningUser();
    }
    else {
        std::cout << "Invalid option.\n";
    }
    return 0;
}