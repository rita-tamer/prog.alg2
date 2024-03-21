#include <iostream>
#include <fstream>
#include <string>
#include <string>
#include <unordered_map>
#include <limits>
#include <cstdlib> // For system commands
#include <ctime>   // For seeding the random number generator


struct Password {
    std::string label;
    std::string password;
    Password* next;

    Password(const std::string& lbl, const std::string& pwd) : label(lbl), password(pwd), next(nullptr) {}
};

struct User {
    std::string name;
    std::string email;
    std::string password;
    std::unordered_map<std::string, std::string> keychain; // Using a hash table for passwords

    User(const std::string& nm, const std::string& eml, const std::string& pwd) : name(nm), email(eml), password(pwd) {}
};

std::unordered_map<std::string, User*> users;

void waitForExit() {
    std::cout << "Press Enter to exit...";
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    std::cin.get();
}

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

bool promptForMore() {
    std::string choice;
    std::cout << "Would you like to do anything else today? (yes/no): ";
    std::cin >> choice;
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // Clear the buffer
    return (choice == "yes" || choice == "Yes");
}

void showMenu(const std::string& userEmail);

void showUserPasswords(const std::string& userEmail) {
    std::string filePath = userEmail + ".txt";
    std::ifstream file(filePath);
    if (!file) {
        std::cerr << "Error opening file for user." << std::endl;
        return;
    }

    std::string label, password;
    bool empty = true;
    std::cout << userEmail << "'s passwords:" << std::endl;
    while (file >> label >> password) {
        std::cout << "Label: " << label << ", Password: " << password << std::endl;
        empty = false;
    }

    if (empty) {
        std::cout << "No passwords stored." << std::endl;
    }

    file.close();
}


void addUserPassword(const std::string& userEmail) {
    std::string filePath = userEmail + ".txt";
    std::ofstream file(filePath, std::ios::app); // Append mode
    if (!file) {
        std::cerr << "Error opening file for user." << std::endl;
        return;
    }

    std::string label, passwordChoice, password;

    std::cout << "Please enter a label for your new password: ";
    getline(std::cin, label);

    std::cout << "Would you like to generate a new password? (yes/no): ";
    getline(std::cin, passwordChoice);

    if (passwordChoice == "yes" || passwordChoice == "Yes") {
        password = generateRandomPassword();
        std::cout << "Generated Password: " << password << "\n";
    } else {
        std::cout << "Enter your password: ";
        getline(std::cin, password);
    }

    file << label << " " << password << std::endl;
    std::cout << "Password added to your keychain.\n";

    file.close();
}



void newUser() {
    std::string name, email, password, confirmPassword;

    std::cout << "Enter your name: ";
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    while (getline(std::cin, name) && name.empty()) {
        std::cout << "Name cannot be left empty. Please enter your name: ";
    }

    std::cout << "Enter your email: ";
    while (getline(std::cin, email) && email.empty()) {
        std::cout << "Email cannot be left empty. Please enter your email: ";
    }

    std::cout << "Set your password: ";
    while (getline(std::cin, password) && password.empty()) {
        std::cout << "Password cannot be left empty. Please set your password: ";
    }

    std::cout << "Confirm your password: ";
    getline(std::cin, confirmPassword);

    if (password == confirmPassword) {
        User* newUser = new User(name, email, password);
        users[email] = newUser;
        std::cout << "Thank you for signing up with us, " << name << "!\n";
        showMenu(email);
    } else {
        std::cout << "Passwords do not match. Please try again.\n";
    }
}

void returningUser() {
    std::string email, password;

    std::cout << "Enter your email: ";
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    getline(std::cin, email);

    std::cout << "Enter your password: ";
    getline(std::cin, password);

    if (users.find(email) != users.end() && users[email]->password == password) {
        std::cout << "Welcome back, " << users[email]->name << "!\n";
        showUserPasswords(email); // Show user's passwords
        addUserPassword(email); // Option to add a new password
        showMenu(email);
    } else {
        std::cout << "Invalid email or password.\n";
    }
}

void showMenu(const std::string& userEmail) {
    int choice;
    bool continueMenu = true;
    
    while (continueMenu) {
        std::cout << "What would you like to do today?\n";
        std::cout << "1. View your registered passwords\n";
        std::cout << "2. Add a new password to your keychain\n";
        std::cout << "3. Update one of your passwords (Placeholder)\n";
        std::cout << "5. Delete your account (Placeholder)\n";
        std::cout << "6. Exit the application\n";
        std::cin >> choice;
        
        switch (choice) {
            case 1:
                showUserPasswords(userEmail);
                std::cout << "Your stored passwords have been displayed.";
                break;
            case 2:
                addUserPassword(userEmail);
                std::cout << "Congratulations! You have just added a new password to your keychain.";
                break;
            case 6:
                std::cout << "Goodbye, See you soon!\n";
                continueMenu = false;
                break;
            default:
                std::cout << "This option is not yet implemented.\n";
                break;
        }
        
        if (continueMenu) {
            continueMenu = promptForMore();
        }
    }
    
    waitForExit();
}

int main() {
    std::cout << "Welcome to Rita's Password Manager!\n";
    std::cout << "1. New user\n2. Returning user\nChoose an option: ";
    int option;
    std::cin >> option;

    switch (option) {
        case 1:
            newUser();
            break;
        case 2:
            returningUser();
            break;
        default:
            std::cout << "Invalid option.\n";
    }

    // Cleanup
    for (auto& pair : users) {
        delete pair.second;
    }
    return 0;
}
