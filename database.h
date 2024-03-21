#include <iostream>
#include <fstream>
#include <string>
#include <limits>
#include <cstdlib> // For system commands
#include <ctime>   // For seeding the random number generator
#include <unordered_map>

struct User {
    std::string name;
    std::string email;
    std::string password;
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
    std::ifstream file(userEmail + ".txt");
    if (!file) {
        std::cout << "No passwords stored.\n";
        return;
    }

    std::string label, password;
    std::cout << userEmail << "'s passwords:\n";
    while (file >> label >> password) {
        std::cout << "Label: " << label << ", Password: " << password << std::endl;
    }
}

void addUserPassword(const std::string& userEmail) {
    std::ofstream file(userEmail + ".txt", std::ios::app);
    if (!file) {
        std::cout << "Error opening file for user.\n";
        return;
    }

    std::string label, passwordChoice, password;
    std::cout << "Please enter a label for your new password: ";
    getline(std::cin, label);

    std::cout << "Would you like to generate a new password? (yes/no): ";
    getline(std::cin, passwordChoice);

    if (passwordChoice == "yes" || passwordChoice == "Yes") {
        password = generateRandomPassword();
        std::cout << "Generated Password: " << password << std::endl;
    } else {
        std::cout << "Enter your password: ";
        getline(std::cin, password);
    }

    file << label << " " << password << std::endl;
    std::cout << "Password added to your keychain.\n";
}

void newUser() {
    std::string name, email, password, confirmPassword;
    std::cout << "Enter your name: ";
    getline(std::cin, name);

    std::cout << "Enter your email: ";
    getline(std::cin, email);

    std::cout << "Set your password: ";
    getline(std::cin, password);

    std::cout << "Confirm your password: ";
    getline(std::cin, confirmPassword);

    if (password == confirmPassword) {
        users[email] = new User{name, email, password};
        std::cout << "Thank you for signing up with us, " << name << "!\n";
    } else {
        std::cout << "Passwords do not match. Please try again.\n";
    }
}

void returningUser() {
    std::string email, password;
    std::cout << "Enter your email: ";
    getline(std::cin, email);

    std::cout << "Enter your password: ";
    getline(std::cin, password);

    if (users.find(email) != users.end() && users[email]->password == password) {
        std::cout << "Welcome back, " << users[email]->name << "!\n";
        showUserPasswords(email);
    } else {
        std::cout << "Invalid email or password.\n";
    }
}

void showMenu(const std::string& userEmail) {
    int choice;
    do {
        std::cout << "What would you like to do today?\n";
        std::cout << "1. View your registered passwords\n";
        std::cout << "2. Add a new password to your keychain\n";
        std::cout << "3. Update one of your passwords (Placeholder)\n";
        std::cout << "4. Delete your account (Placeholder)\n";
        std::cout << "5. Exit the application\n";
        std::cin >> choice;
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // Clear buffer

        switch (choice) {
            case 1:
                showUserPasswords(userEmail);
                std::cout << "Your stored passwords have been displayed.";
                break;
            case 2:
                addUserPassword(userEmail);
                std::cout << "Congratulations! You have just added a new password to your keychain.";
                break;
            case 5:
                std::cout << "Goodbye, See you soon!\n";
                waitForExit();
                return;
            default:
                std::cout << "This option is not yet implemented.\n";
                break;
        }
    } while (true);
}

int main() {
    std::cout << "Welcome to Rita's Password Manager!\n";
    std::cout << "1. New user\n2. Returning user\nChoose an option: ";
    int option;
    std::cin >> option;
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // Clear buffer

    if (option == 1) {
        newUser();
    } else if (option == 2) {
        returningUser();
    } else {
        std::cout << "Invalid option.\n";
    }

    // Memory cleanup for users
    for (auto& pair : users) {
        delete pair.second;
    }
}
