#include <iostream>

int main() {
    int userType = 0;

    std::cout << "Welcome to Rita's Password Manager!" << std::endl;
    std::cout << "Are you a:" << std::endl;
    std::cout << "1. New user" << std::endl;
    std::cout << "2. Returning user" << std::endl;
    std::cout << "Please enter 1 or 2 to continue..." << std::endl;

    std::cin >> userType;

    if (userType == 1) {
        std::cout << "You've selected New User." << std::endl;
        // Handle new user registration here
    }
    else if (userType == 2) {
        std::cout << "Welcome back, Returning User!" << std::endl;
        // Handle user login here
    }
    else {
        std::cout << "Invalid selection. Please restart the application and choose either 1 or 2." << std::endl;
    }

    return 0;
}
