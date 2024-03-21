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
    std::cout << "\nPress Enter to exit...";
    // std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    std::cin.get(); // Wait for user to press Enter
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
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    return (choice == "yes" || choice == "Yes");
}

void showMenu(const std::string& userEmail, const std::string& userName);

void showUserPasswords(const std::string& userEmail) {
    std::ifstream file(userEmail + ".txt");
    if (!file.is_open() || file.peek() == std::ifstream::traits_type::eof()) {
        std::cout << "No passwords stored.\n";
        return;
    }

    std::string storedEmail, storedPassword, name;
    std::getline(file, storedEmail); // Skip stored email
    std::getline(file, storedPassword); // Skip stored password
    std::getline(file, name); // Get the name for personalized message

    std::string label, password;
    bool hasPasswords = false;
    std::cout << name << "'s passwords:\n"; // Display using name for a personal touch.
    while (file >> label >> password) {
        hasPasswords = true;
        std::cout << "Label: " << label << ", Password: " << password << '\n';
    }

    if (!hasPasswords) {
        std::cout << "No passwords stored.\n";
    }
    file.close();
}

void addUserPassword(const std::string& userEmail) {
    std::ofstream file(userEmail + ".txt", std::ios::app);
    if (!file.is_open()) {
        std::cerr << "Unable to open file for user.\n";
        return;
    }

    std::string label, passwordChoice, password;
    std::cout << "Please enter a label for your new password: ";
    getline(std::cin, label);
    std::cout << "Would you like to generate a new password? (yes/no): ";
    getline(std::cin, passwordChoice);

    if (passwordChoice == "yes" || passwordChoice == "Yes") {
        password = generateRandomPassword();
        std::cout << "Generated Password: " << password << '\n';
    } else {
        std::cout << "Enter your password: ";
        getline(std::cin, password);
    }

    // Write directly to avoid switching label and password.
    file << label << ' ' << password << '\n';
    std::cout << "Password added to your keychain.\n";
    file.close();
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

    if (password != confirmPassword) {
        std::cout << "Passwords do not match. Please try again.\n";
        return;
    }

    std::ofstream userFile(email + ".txt");
    if (!userFile.is_open()) {
        std::cerr << "Error creating file for new user.\n";
        return;
    }
    userFile << email << '\n' << password << '\n' << name << '\n'; // Store email, password, and name
    userFile.close();

    std::cout << "Thank you for signing up with us, " << name << "!\n";
    showMenu(email, name); // Show the menu after signing up
}

void showMenu(const std::string& userEmail, const std::string& userName) {
    int choice;
    do {
        std::cout << "\nHello, " << userName << "! What would you like to do today?\n";
        std::cout << "1. View your registered passwords\n";
        std::cout << "2. Add a new password to your keychain\n";
        std::cout << "3. Exit the application\nChoose an option: ";
        std::cin >> choice;
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

        switch (choice) {
            case 1:
                showUserPasswords(userEmail);
                break;
            case 2:
                addUserPassword(userEmail);
                break;
            case 3:
                std::cout << "Goodbye, See you soon!\n";
                return;
        }
    } while (choice != 3);
}


void returningUser() {
    std::string email, password;
    std::cout << "Enter your email: ";
    getline(std::cin, email);
    std::cout << "Enter your password: ";
    getline(std::cin, password);

    std::ifstream file(email + ".txt");
    if (!file.is_open()) {
        std::cout << "User not found.\n";
        return;
    }

    std::string storedEmail, storedPassword, name;
    getline(file, storedEmail); // First line is email
    getline(file, storedPassword); // Second line is password
    getline(file, name); // Third line is name
    file.close();

    if (email == storedEmail && password == storedPassword) {
        std::cout << "Welcome back, " << name << "!\n";
        showMenu(email, name);
    } else {
        std::cout << "Invalid email or password.\n";
    }
}

int main() {
    std::cout << "Welcome to Rita's Password Manager!\n";
    std::cout << "1. New user\n2. Returning user\nChoose an option: ";
    int option;
    std::cin >> option;
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // Clear buffer

    if (option == 1) {
        newUser(); // Assume newUser() is modified to call showMenu() with appropriate parameters.
    } else if (option == 2) {
        returningUser();
    } else {
        std::cout << "Invalid option.\n";
    }

    // No dynamic allocation was used, so no need for cleanup here.

    return 0;
}