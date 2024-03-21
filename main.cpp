#include <iostream>
#include <string>
#include <limits>
#include <vector>
#include <cstdlib>
#include <ctime>

struct PasswordEntry {
    std::string label;
    std::string password;
};

std::vector<PasswordEntry> passwordEntries;

// Random password generator
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

// Function for the user to add a password to their keychain
void addUserPassword() {
    std::string label, password;
    std::cout << "Please enter:\nLabel: ";
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    getline(std::cin, label);
    while (label.empty()) {
        std::cout << "Label cannot be left empty. Please enter a label: ";
        getline(std::cin, label);
    }

    std::cout << "Password: ";
    getline(std::cin, password);
    while (password.empty()) {
        std::cout << "Password cannot be left empty. Please enter a password: ";
        getline(std::cin, password);
    }

    passwordEntries.push_back({label, password});
    std::cout << "Congrats! You have successfully added a password to your keychain!\n";
}

void waitForExit() {
    std::cout << "Press Enter to exit...";
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    std::cin.get();
}

// Function to list and implement all the options for the user
void showMenu() {
    int choice;
    do {
        std::cout << "What would you like to do today?\n";
        std::cout << "1. View your registered passwords\n";
        std::cout << "2. Add a new password to your keychain\n";
        std::cout << "3. Update one of your passwords\n";
        std::cout << "4. Generate a password\n";
        std::cout << "5. Delete your account \n";
        std::cout << "6. Exit the application\n";

        std::cin >> choice;
        switch (choice) {
            case 1:
                // Placeholder for viewing passwords
                for (auto& entry : passwordEntries) {
                    std::cout << "Label: " << entry.label << ", Password: " << entry.password << std::endl;
                }
                break;
            case 2:
                addUserPassword();
                break;
            case 4: {
                std::string generatedPassword = generateRandomPassword();
                std::cout << "Generated Password: " << generatedPassword << std::endl;
                std::string add;
                std::cout << "Would you like to add it to your keychain? (yes/no): ";
                std::cin >> add;
                if (add == "yes") {
                    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                    addUserPassword();
                }
                break;
            }
            case 6:
                std::cout << "Goodbye, See you soon!\n";
                waitForExit();
                return; 
            default:
                std::cout << "Option not implemented.\n";
        }

        std::string doMore;
        std::cout << "Would you like to do anything else? (yes/no): ";
        std::cin >> doMore;
        if (doMore != "yes") {
            std::cout << "Goodbye, See you soon!\n";
            waitForExit(); 
            break;
        }
    } while (true);
}

// Function to create a new user
void newUser() {
    std::string name, email, password, confirmPassword;

    std::cout << "Hello There! Let's create an account for you, please enter:\n";
    std::cout << "your name: ";
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    getline(std::cin, name);
    std::cout << "your email: ";
    getline(std::cin, email);
    std::cout << "set your password: ";
    getline(std::cin, password);
    std::cout << "confirm your password: ";
    getline(std::cin, confirmPassword);

    if (password == confirmPassword) {
        std::cout << "Thank you for signing up with us!\n";
        showMenu();
    } else {
        std::cout << "Passwords do not match. Please restart the application and try again.\n";
    }
}

// Function for logging in

void returningUser() {
    std::string email, password;

    std::cout << "Please log in to your account.\n";

    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

    std::cout << "Your email: ";
    std::getline(std::cin, email);
    while (email.empty()) {
        std::cout << "Email cannot be left empty. Please enter your email: ";
        std::getline(std::cin, email);
    }

    std::cout << "Your password: ";
    std::getline(std::cin, password);
    while (password.empty()) {
        std::cout << "Password cannot be left empty. Please enter your password: ";
        std::getline(std::cin, password);
    }
    std::cout << "Welcome Back! What would you like to do today?\n";
    showMenu();
}


int main() {
    int userType = 0;

    std::cout << "Welcome to Rita's Password Manager!\n";
    std::cout << "Are you a:\n";
    std::cout << "1. New user\n";
    std::cout << "2. Returning user\n";
    std::cout << "Please enter 1 or 2 to continue...\n";

    std::cin >> userType;

    if (userType == 1) {
        newUser();
    } else if (userType == 2) {
        returningUser();
    } else {
        std::cout << "Invalid selection. Please restart the application and choose either 1 or 2.\n";
    }
    return 0;
}

