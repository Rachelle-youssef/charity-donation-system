#include <iostream>
#include <sstream>
#include <fstream>
#include <string>   
#include <cctype> // for isdigit, isalpha
#include <ctime>
#include <iomanip> // for std::put_time
#include <algorithm> // for sorting
#include <vector>
#include <map>
#include <functional> // for std::hash

using namespace std;

// A simple structure for holding date and time
struct Date {
    string date;
    string time;
};

// A donation made by a user
struct Donation {
    int donationID;
    int charityID;
    double amount;
    Date d;
    string message; // Optional message for the charity
};

// A charity campaign
struct Charity {
    int charityID;
    string name;
    string description;
    double targetAmount;
    double currentAmount;
    Date d; // date of creation
    string status; // for example: "Active" or "Closed"
};

// A client or user of the system
struct Client {
    int userID;
    int nbDonations;
    string firstName;
    string lastName;
    string password;
    string phone;
    string email;
    string role;
    Donation* donations; // keeps all donations made by this user
};

// Function to hash a password
string hashPassword(const string& password) {
    hash<string> hasher;
    size_t hash = hasher(password);
    return to_string(hash);
}

void showMainMenu(int& choice) {
    cout << "Welcome to the Charity Donation System!" << endl;
    cout << "1. I have an account (Login)" << endl;
    cout << "2. I don't have an account (Register)" << endl;
    cout << "3. Exit" << endl;
    cout << "Enter your choice: ";
    cin >> choice;

    switch (choice) {
    case 1:
        cout << "You chose Login." << endl;
        break;
    case 2:
        cout << "You chose Register." << endl;
        break;
    case 3:
        cout << "Thank you! Goodbye!" << endl;
        break;
    default:
        cout << "Invalid choice. Try again." << endl;
        showMainMenu(choice); // show again if input is invalid
    }
}

bool isValidPassword(string password) {
    bool hasLetter = false, hasDigit = false, hasSpecial = false;

    if (password.length() < 8)
        return false;

    for (char ch : password) {
        if (isalpha(ch)) hasLetter = true;        //Letter availability verification
        else if (isdigit(ch)) hasDigit = true;    //digit availability verification
        else hasSpecial = true;                   //special character availability verification
    }

    return hasLetter && hasDigit && hasSpecial;   //verify availability of all categories
}

bool isValidEmail(string email) {
    // Check if email is empty
    if (email.empty()) return false;

    // Must contain exactly one @
    int atPos = email.find('@');
    if (atPos == string::npos) return false; // No @ found
    if (email.find('@', atPos + 1) != string::npos) return false; // More than one @

    // Must have a dot after the @
    int dotPos = email.find('.', atPos);
    if (dotPos == string::npos) return false; // No . found after @

    // Cannot start or end with @ or .
    if (email[0] == '@' || email[0] == '.' ||
        email[email.length() - 1] == '@' || email[email.length() - 1] == '.') {
        return false;
    }

    // No spaces or special characters allowed
    if (email.find(' ') != string::npos) return false;
    return true; // Passed all checks
}

bool isValidPhone(string phone) {
    if (phone.length() != 8) return false;
    for (char ch : phone) {
        if (!isdigit(ch)) return false;
    }
    return true;
}

int generateUserID() {
    static int lastID = 1000; 
    return ++lastID;
}

void registerUser() {
    string firstName, lastName, email, phone, password;

    cout << "Enter First Name: ";
    cin >> firstName;
    cout << "Enter Last Name: ";
    cin >> lastName;

    // Ensure email is unique
    do {
        cout << "Enter Email: ";
        cin >> email;

        if (!isValidEmail(email)) {
            cout << "Error: Invalid email format. Please try again.\n";
            continue; // Go back and re-prompt
        }

        ifstream inFile("users.txt");
        string line, existingEmail;
        bool emailExists = false;

        while (getline(inFile, line)) {
            stringstream ss(line);
            // Skip userID, firstName, lastName, password
            string dummy;
            ss >> dummy >> dummy >> dummy >> dummy;
            ss >> existingEmail; // Read email from file
            if (existingEmail == email) {
                emailExists = true;
                break;
            }
        }
        inFile.close();

        if (emailExists) {
            cout << "Error: Email already exists. Please use a different email." << endl;
        }
        else {
            break;
        }
    } while (true);

    do {
        cout << "Enter Phone (8 digits): ";
        cin >> phone;
        if (!isValidPhone(phone)) {
            cout << "Error: Phone must be exactly 8 digits. Please try again.\n";
        }
    } while (!isValidPhone(phone));

    do {
        cout << "Enter Password (at least 8 characters, must include letter, number, and symbol): ";
        cin >> password;
        if (!isValidPassword(password)) {
            cout << "Error: Password must be at least 8 characters and contain letters, numbers, and special characters.\n";
        }
    } while (!isValidPassword(password));

    // Hash the password before storing
    string hashedPassword = hashPassword(password);

    int userID = generateUserID();

    // Ask user for their role
    string role;
    int roleChoice;
    cout << "Select your role:\n";
    cout << "1. Donor\n";
    cout << "2. Admin\n";
    cout << "Enter choice (1-2): ";
    cin >> roleChoice;

    // Validate role choice
    while (roleChoice != 1 && roleChoice != 2) {
        cout << "Invalid choice. Please enter 1 for Donor or 2 for Admin: ";
        cin >> roleChoice;
    }

    role = (roleChoice == 1) ? "donor" : "admin";

    // Save to file
    ofstream outFile("users.txt", ios::app); // Append mode
    outFile << userID << " " << firstName << " " << lastName << " " << hashedPassword << " " << email << " " << phone << " " << role << "\n";
    outFile.close();

    cout << "Registration successful! Your user ID is: " << userID << endl;
    cout << "You are registered as a " << role << "." << endl;
}

// Generate current date and time
Date getCurrentDateTime() {
    Date currentDT;
    time_t now = time(0);
    tm localtm;
    localtime_s(&localtm, &now);

    // Format date (YYYY-MM-DD)
    stringstream dateStream;//Creates a string stream to build the date as a string.
    dateStream << put_time(&localtm, "%Y-%m-%d");
    currentDT.date = dateStream.str();//Converts the stream to a string and stores it in currentDT.date.

    // Format time (HH:MM)
    stringstream timeStream;
    timeStream << put_time(&localtm, "%H:%M");
    currentDT.time = timeStream.str();

    return currentDT;
}

int addCharity() {
    Charity newCharity;
    cout << "\n--- Add Charity ---\n";

    // Input charity details
    cout << "Enter Charity Name: ";
    cin.ignore(); // Clear input buffer
    getline(cin, newCharity.name);

    cout << "Enter Charity Description: ";
    getline(cin, newCharity.description);

    cout << "Enter Target Amount: ";
    cin >> newCharity.targetAmount;

    // Initialize current amount to 0 and set status to "Active"
    newCharity.currentAmount = 0.0;
    newCharity.status = "Active";

    // Generate a unique charity ID
    ifstream inFile("charities.txt");
    int lastID = 0;
    string line;
    while (getline(inFile, line)) {
        stringstream ss(line);
        int charityID;
        ss >> charityID;
        if (charityID > lastID) {
            lastID = charityID; // Keep track of the last ID
        }
    }
    inFile.close();
    newCharity.charityID = lastID + 1;

    // Get current date and time
    newCharity.d = getCurrentDateTime();

    // Check for duplicates in charities.txt
    inFile.open("charities.txt");
    while (getline(inFile, line)) {
        stringstream ss(line);
        int id;
        string name;
        ss >> id;
        ss.ignore(); // Skip the space after ID
        getline(ss, name, '|');

        // Trim leading/trailing spaces from name
        name.erase(0, name.find_first_not_of(" \t"));
        name.erase(name.find_last_not_of(" \t") + 1);

        if (name == newCharity.name) {
            cout << "Error: Charity with this name already exists.\n";
            inFile.close();
            return -1; // Indicate failure
        }
    }
    inFile.close();

    // Save the new charity to charities.txt
    ofstream outFile("charities.txt", ios::app);
    outFile << newCharity.charityID << " | " << newCharity.name << " | " << newCharity.description
        << " | " << newCharity.targetAmount << " | " << newCharity.currentAmount
        << " | " << newCharity.d.date << " | " << newCharity.d.time << " | " << newCharity.status << "\n";
    outFile.close();

    cout << "Charity added successfully with ID: " << newCharity.charityID << endl;
    return newCharity.charityID; // Indicate success and return the new charity ID
}

void removeCharity() {
    int charityID;
    cout << "\n--- Remove Charity ---\n";
    cout << "Enter the Charity ID to remove: ";
    cin >> charityID;

    ifstream inFile("charities.txt");
    if (!inFile) {
        cout << "Error: Cannot open charities.txt" << endl;
        return;
    }

    ofstream tempFile("temp.txt");
    if (!tempFile) {
        cout << "Error: Cannot create temporary file." << endl;
        inFile.close();
        return;
    }

    string line;
    bool found = false;

    // Read each line and copy all except the one to be removed
    while (getline(inFile, line)) {
        stringstream ss(line);
        int currentID;
        ss >> currentID;

        if (currentID == charityID) {
            found = true; // Charity to remove found
            continue;     // Skip writing this line to the temp file
        }

        tempFile << line << "\n"; // Write other lines to the temp file
    }

    inFile.close();
    tempFile.close();

    if (found) {
        // Replace the original file with the temp file
        remove("charities.txt");
        rename("temp.txt", "charities.txt");
        cout << "Charity with ID " << charityID << " has been removed successfully." << endl;
    }
    else {
        // Cleanup temp file if no charity was found
        remove("temp.txt");
        cout << "Error: Charity with ID " << charityID << " not found." << endl;
    }
}

void modifyCharity() {
    int charityID;
    cout << "\n--- Modify Charity ---\n";
    cout << "Enter the Charity ID to modify: ";
    cin >> charityID;

    ifstream inFile("charities.txt");
    if (!inFile) {
        cout << "Error: Cannot open charities.txt" << endl;
        return;
    }

    ofstream tempFile("temp.txt");
    if (!tempFile) {
        cout << "Error: Cannot create temporary file." << endl;
        inFile.close();
        return;
    }

    string line;
    bool found = false;

    // Read each line and modify the specified charity
    while (getline(inFile, line)) {
        stringstream ss(line);
        Charity charity;

        // Parse charity data
        ss >> charity.charityID;
        ss.ignore(3); // Skip " | "

        getline(ss, charity.name, '|');
        charity.name.erase(0, charity.name.find_first_not_of(" \t"));
        charity.name.erase(charity.name.find_last_not_of(" \t") + 1);

        getline(ss, charity.description, '|');
        charity.description.erase(0, charity.description.find_first_not_of(" \t"));
        charity.description.erase(charity.description.find_last_not_of(" \t") + 1);

        ss >> charity.targetAmount;
        ss.ignore(3); // Skip " | "

        ss >> charity.currentAmount;
        ss.ignore(3); // Skip " | "

        getline(ss, charity.d.date, '|');
        charity.d.date.erase(0, charity.d.date.find_first_not_of(" \t"));
        charity.d.date.erase(charity.d.date.find_last_not_of(" \t") + 1);

        getline(ss, charity.d.time, '|');
        charity.d.time.erase(0, charity.d.time.find_first_not_of(" \t"));
        charity.d.time.erase(charity.d.time.find_last_not_of(" \t") + 1);

        getline(ss, charity.status);
        charity.status.erase(0, charity.status.find_first_not_of(" \t"));
        charity.status.erase(charity.status.find_last_not_of(" \t") + 1);

        if (charity.charityID == charityID) {
            found = true;

            // Display current details
            cout << "Current Details of Charity ID " << charityID << ":\n";
            cout << "Name: " << charity.name << "\n";
            cout << "Description: " << charity.description << "\n";
            cout << "Target Amount: $" << charity.targetAmount << "\n";
            cout << "Current Amount: $" << charity.currentAmount << "\n";
            cout << "Status: " << charity.status << "\n";

            // Modify details
            cout << "\nEnter new details (leave blank to keep current value):\n";

            cout << "New Name: ";
            cin.ignore(); // Clear input buffer
            string newName;
            getline(cin, newName);
            if (!newName.empty()) charity.name = newName;

            cout << "New Description: ";
            string newDescription;
            getline(cin, newDescription);
            if (!newDescription.empty()) charity.description = newDescription;

            cout << "New Target Amount: ";
            string newTargetAmount;
            getline(cin, newTargetAmount);
            if (!newTargetAmount.empty()) charity.targetAmount = stod(newTargetAmount);

            cout << "New Status (Active/Closed): ";
            string newStatus;
            getline(cin, newStatus);
            if (!newStatus.empty()) charity.status = newStatus;

            cout << "Charity details updated successfully.\n";
        }

        // Write updated or unchanged charity back to temp file
        tempFile << charity.charityID << " | " << charity.name << " | " << charity.description
            << " | " << charity.targetAmount << " | " << charity.currentAmount
            << " | " << charity.d.date << " | " << charity.d.time << " | " << charity.status << "\n";
    }

    inFile.close();
    tempFile.close();

    if (found) {
        // Replace the original file with the temp file
        remove("charities.txt");
        rename("temp.txt", "charities.txt");
    }
    else {
        cout << "Error: Charity with ID " << charityID << " not found.\n";
        remove("temp.txt");
    }
}

void adminMenu() {
    int choice;
    do {
        cout << "\n--- Admin Menu ---\n";
        cout << "1. Add Charity" << endl;
        cout << "2. Remove Charity" << endl;
        cout << "3. Modify Charity" << endl;
        cout << "4. Exit Admin Menu" << endl;
        cout << "Enter choice: ";
        cin >> choice;

        switch (choice) {
        case 1:
            addCharity(); // Function to add a new charity
            break;
        case 2:
            removeCharity(); // Function to remove an existing charity
            break;
        case 3:
            modifyCharity(); // Function to modify an existing charity
            break;
        case 4:
            cout << "Exiting Admin Menu." << endl;
            break;
        default:
            cout << "Invalid choice. Please try again." << endl;
        }

    } while (choice != 4);
}

bool loadUserDonations(Client& user) {
    // Allocate memory for donations array
    user.donations = new Donation[100]; // Assuming max 100 donations per user
    user.nbDonations = 0;

    ifstream inFile("donations.txt");
    if (!inFile) {
        cout << "No donations file found." << endl;
        return false;
    }

    string line;
    while (getline(inFile, line)) {
        stringstream ss(line);
        int donationID, userID, charityID;
        double amount;
        string date, time, message;

        ss >> donationID;
        ss.ignore(3); // Skip " | "
        ss >> userID;
        ss.ignore(3); // Skip " | "
        ss >> charityID;
        ss.ignore(3); // Skip " | "
        ss >> amount;
        ss.ignore(3); // Skip " | "

        getline(ss, date, '|');
        date.erase(0, date.find_first_not_of(" \t"));
        date.erase(date.find_last_not_of(" \t") + 1);

        getline(ss, time, '|');
        time.erase(0, time.find_first_not_of(" \t"));
        time.erase(time.find_last_not_of(" \t") + 1);

        // Read message (if exists)
        getline(ss, message);
        message.erase(0, message.find_first_not_of(" \t"));
        if (!message.empty()) {
            message.erase(message.find_last_not_of(" \t") + 1);
        }

        if (userID == user.userID) {
            user.donations[user.nbDonations].donationID = donationID;
            user.donations[user.nbDonations].charityID = charityID;
            user.donations[user.nbDonations].amount = amount;
            user.donations[user.nbDonations].d.date = date;
            user.donations[user.nbDonations].d.time = time;
            user.donations[user.nbDonations].message = message;
            user.nbDonations++;
        }
    }

    inFile.close();
    return true;
}

void saveDonationsToPDF(Client& user) {
    if (user.nbDonations == 0) {
        cout << "No donations to save to PDF." << endl;
        return;
    }

    // Sort donations by amount and date
    // Simple bubble sort for demonstration
    for (int i = 0; i < user.nbDonations - 1; i++) {
        for (int j = 0; j < user.nbDonations - i - 1; j++) {
            // Sort by amount first
            if (user.donations[j].amount > user.donations[j + 1].amount) {
                Donation temp = user.donations[j];
                user.donations[j] = user.donations[j + 1];
                user.donations[j + 1] = temp;
            }
            // If amounts are equal, sort by date
            else if (user.donations[j].amount == user.donations[j + 1].amount &&
                user.donations[j].d.date > user.donations[j + 1].d.date) {
                Donation temp = user.donations[j];
                user.donations[j] = user.donations[j + 1];
                user.donations[j + 1] = temp;
            }
        }
    }

    // Create a filename with user ID
    string filename = "donations_" + to_string(user.userID) + ".txt"; // Using .txt instead of .pdf for simplicity

    ofstream outFile(filename);
    if (!outFile) {
        cout << "Error: Cannot create PDF file." << endl;
        return;
    }

    // Write user information
    outFile << "Donations Report for User: " << user.firstName << " " << user.lastName << "\n";
    outFile << "Email: " << user.email << "\n";
    outFile << "Phone: " << user.phone << "\n\n";
    outFile << "Donations (sorted by amount and date in ascending order):\n";
    outFile << "----------------------------------------------------\n";

    // Write donation details
    for (int i = 0; i < user.nbDonations; i++) {
        // Get charity name from charity ID
        ifstream charityFile("charities.txt");
        string charityName = "Unknown Charity";
        string line;

        while (getline(charityFile, line)) {
            stringstream ss(line);
            int id;
            string name;
            ss >> id;
            if (id == user.donations[i].charityID) {
                ss.ignore(3); // Skip " | "
                getline(ss, name, '|');
                name.erase(0, name.find_first_not_of(" \t"));
                name.erase(name.find_last_not_of(" \t") + 1);
                charityName = name;
                break;
            }
        }
        charityFile.close();

        outFile << "Donation " << (i + 1) << ":\n";
        outFile << "  Donation ID: " << user.donations[i].donationID << "\n";
        outFile << "  Charity: " << charityName << " (ID: " << user.donations[i].charityID << ")\n";
        outFile << "  Amount: $" << user.donations[i].amount << "\n";
        outFile << "  Date: " << user.donations[i].d.date << "\n";
        outFile << "  Time: " << user.donations[i].d.time << "\n";
        if (!user.donations[i].message.empty()) {
            outFile << "  Message: " << user.donations[i].message << "\n";
        }
        outFile << "----------------------------------------------------\n";
    }

    outFile.close();
    cout << "Donations report saved to " << filename << endl;
}

bool loginUser(Client& loggedUser) {
    string email, password;
    cout << "Enter your email: ";
    cin >> email;
    cout << "Enter your password: ";
    cin >> password;

    // Hash the password for comparison
    string hashedPassword = hashPassword(password);

    ifstream inFile("users.txt");
    if (!inFile) {
        cout << "Error: Cannot open users.txt" << endl;
        return false;
    }

    string line;
    while (getline(inFile, line)) {
        stringstream ss(line);
        Client user;
        ss >> user.userID >> user.firstName >> user.lastName >> user.password >> user.email >> user.phone >> user.role;

        if (email == user.email && hashedPassword == user.password) {
            loggedUser = user; // Copy matched user into the loggedUser reference
            cout << "Login successful! Welcome, " << user.firstName << "!" << endl;

            // Load user's donations
            loadUserDonations(loggedUser);

            // Check user role
            if (user.role == "admin") {
                cout << "You are logged in as Admin.\n";
                adminMenu();
            }
            else {
                cout << "You are logged in as Donor.\n";

            }

            return true;
        }
    }

    cout << "Login failed. Email or password is incorrect." << endl;
    return false;
}

void viewCharities() {
    ifstream inFile("charities.txt");
    if (!inFile) {
        cout << "Error: Cannot open charities.txt" << endl;
        return;
    }

    cout << "\n--- Available Charities ---\n";
    string line;
    bool found = false;

    while (getline(inFile, line)) {
        stringstream ss(line);
        Charity charity;
        ss >> charity.charityID;
        ss.ignore(3); // Skip " | "

        getline(ss, charity.name, '|');
        charity.name.erase(0, charity.name.find_first_not_of(" \t"));
        charity.name.erase(charity.name.find_last_not_of(" \t") + 1);

        getline(ss, charity.description, '|');
        charity.description.erase(0, charity.description.find_first_not_of(" \t"));
        charity.description.erase(charity.description.find_last_not_of(" \t") + 1);

        ss >> charity.targetAmount;
        ss.ignore(3); // Skip " | "

        ss >> charity.currentAmount;
        ss.ignore(3); // Skip " | "

        getline(ss, charity.d.date, '|');
        charity.d.date.erase(0, charity.d.date.find_first_not_of(" \t"));
        charity.d.date.erase(charity.d.date.find_last_not_of(" \t") + 1);

        getline(ss, charity.d.time, '|');
        charity.d.time.erase(0, charity.d.time.find_first_not_of(" \t"));
        charity.d.time.erase(charity.d.time.find_last_not_of(" \t") + 1);

        getline(ss, charity.status);
        charity.status.erase(0, charity.status.find_first_not_of(" \t"));
        charity.status.erase(charity.status.find_last_not_of(" \t") + 1);

        if (charity.status == "Active") {
            found = true;
            cout << "ID: " << charity.charityID << "\n"
                << "Name: " << charity.name << "\n"
                << "Description: " << charity.description << "\n"
                << "Target Amount: $" << charity.targetAmount << "\n"
                << "Current Amount: $" << charity.currentAmount << "\n"
                << "Status: " << charity.status << "\n"
                << "--------------------------\n";
        }
    }

    if (!found) {
        cout << "No active charities available at the moment.\n";
    }

    inFile.close();
}

int generateDonationID() {
    ifstream inFile("donations.txt");
    if (!inFile) {
        return 1; // Start from 1 if file doesn't exist
    }

    int maxID = 0;
    string line;
    while (getline(inFile, line)) {
        stringstream ss(line);
        int donationID;
        ss >> donationID;
        if (donationID > maxID) {
            maxID = donationID;
        }
    }
    inFile.close();
    return maxID + 1;
}

void makeDonation(Client& user) {
    int charityID;
    double amount;
    string message;

    cout << "\n--- Make a Donation ---\n";
    viewCharities(); // Show available charities

    cout << "Enter the Charity ID you want to donate to: ";
    cin >> charityID;

    cout << "Enter the amount you want to donate: $";
    cin >> amount;

    if (amount <= 0) {
        cout << "Error: Donation amount must be greater than zero.\n";
        return;
    }

    // Optional message
    cout << "Would you like to add a message with your donation? (y/n): ";
    char choice;
    cin >> choice;

    if (choice == 'y' || choice == 'Y') {
        cout << "Enter your message: ";
        cin.ignore(); // Clear input buffer
        getline(cin, message);
    }

    ifstream inFile("charities.txt");
    ofstream tempFile("temp.txt");
    if (!inFile || !tempFile) {
        cout << "Error: Cannot access charity files.\n";
        return;
    }

    string line;
    bool found = false;
    Charity donatedCharity;

    while (getline(inFile, line)) {
        stringstream ss(line);
        Charity charity;
        ss >> charity.charityID;
        ss.ignore(3); // Skip " | "

        getline(ss, charity.name, '|');
        charity.name.erase(0, charity.name.find_first_not_of(" \t"));
        charity.name.erase(charity.name.find_last_not_of(" \t") + 1);

        getline(ss, charity.description, '|');
        charity.description.erase(0, charity.description.find_first_not_of(" \t"));
        charity.description.erase(charity.description.find_last_not_of(" \t") + 1);

        ss >> charity.targetAmount;
        ss.ignore(3); // Skip " | "

        ss >> charity.currentAmount;
        ss.ignore(3); // Skip " | "

        getline(ss, charity.d.date, '|');
        charity.d.date.erase(0, charity.d.date.find_first_not_of(" \t"));
        charity.d.date.erase(charity.d.date.find_last_not_of(" \t") + 1);

        getline(ss, charity.d.time, '|');
        charity.d.time.erase(0, charity.d.time.find_first_not_of(" \t"));
        charity.d.time.erase(charity.d.time.find_last_not_of(" \t") + 1);

        getline(ss, charity.status);
        charity.status.erase(0, charity.status.find_first_not_of(" \t"));
        charity.status.erase(charity.status.find_last_not_of(" \t") + 1);

        // Continuing from where the code was cut off in makeDonation function
        if (charity.charityID == charityID) {
            found = true;
            donatedCharity = charity;

            // Check if charity is active
            if (charity.status != "Active") {
                cout << "Error: This charity is not currently accepting donations." << endl;
                inFile.close();
                tempFile.close();
                remove("temp.txt");
                return;
            }

            // Update charity's current amount
            charity.currentAmount += amount;

            // Check if target has been reached
            if (charity.currentAmount >= charity.targetAmount) {
                cout << "Congratulations! Your donation has helped this charity reach its target goal!" << endl;
                charity.status = "Closed"; // Automatically close when target reached
            }
        }

        // Write the charity to temp file (either updated or unchanged)
        tempFile << charity.charityID << " | " << charity.name << " | " << charity.description
            << " | " << charity.targetAmount << " | " << charity.currentAmount
            << " | " << charity.d.date << " | " << charity.d.time << " | " << charity.status << "\n";
    }

    inFile.close();
    tempFile.close();

    if (!found) {
        cout << "Error: Charity with ID " << charityID << " not found." << endl;
        remove("temp.txt");
        return;
    }

    // Replace the original charity file with the updated one
    remove("charities.txt");
    rename("temp.txt", "charities.txt");

    // Generate donation ID and get current date/time
    int donationID = generateDonationID();
    Date currentDT = getCurrentDateTime();

    // Save donation to file
    ofstream donationFile("donations.txt", ios::app);
    donationFile << donationID << " | " << user.userID << " | " << charityID << " | " << amount
        << " | " << currentDT.date << " | " << currentDT.time;

    if (!message.empty()) {
        donationFile << " | " << message;
    }
    donationFile << "\n";
    donationFile.close();

    // Store donation in user's donations array
    if (user.nbDonations == 0) {
        user.donations = new Donation[10]; // Initial allocation
    }
    else if (user.nbDonations % 10 == 0) {
        // Expand array if needed
        Donation* newArray = new Donation[user.nbDonations + 10];
        for (int i = 0; i < user.nbDonations; i++) {
            newArray[i] = user.donations[i];
        }
        delete[] user.donations;
        user.donations = newArray;
    }

    // Add donation to user's donations array
    user.donations[user.nbDonations].donationID = donationID;
    user.donations[user.nbDonations].charityID = charityID;
    user.donations[user.nbDonations].amount = amount;
    user.donations[user.nbDonations].d = currentDT;
    user.donations[user.nbDonations].message = message;
    user.nbDonations++;

    cout << "Donation of $" << amount << " to '" << donatedCharity.name << "' completed successfully!" << endl;
}

void viewMyDonations(Client& user) {
    if (user.nbDonations == 0) {
        cout << "You haven't made any donations yet." << endl;
        return;
    }

    cout << "\n--- Your Donations ---\n";
    double totalDonated = 0.0;

    for (int i = 0; i < user.nbDonations; i++) {
        // Get charity name from charity ID
        string charityName = "Unknown Charity";
        ifstream charityFile("charities.txt");
        string line;

        while (getline(charityFile, line)) {
            stringstream ss(line);
            int id;
            string name;
            ss >> id;
            if (id == user.donations[i].charityID) {
                ss.ignore(3); // Skip " | "
                getline(ss, name, '|');
                name.erase(0, name.find_first_not_of(" \t"));
                name.erase(name.find_last_not_of(" \t") + 1);
                charityName = name;
                break;
            }
        }
        charityFile.close();

        cout << "Donation ID: " << user.donations[i].donationID << "\n"
            << "Charity: " << charityName << "\n"
            << "Amount: $" << user.donations[i].amount << "\n"
            << "Date: " << user.donations[i].d.date << "\n"
            << "Time: " << user.donations[i].d.time << "\n";

        if (!user.donations[i].message.empty()) {
            cout << "Message: " << user.donations[i].message << "\n";
        }
        cout << "--------------------------\n";

        totalDonated += user.donations[i].amount;
    }

    cout << "Total amount donated: $" << totalDonated << endl;
}

void cancelDonation(Client& user) {
    if (user.nbDonations == 0) {
        cout << "You haven't made any donations yet." << endl;
        return;
    }

    viewMyDonations(user);

    int donationID;
    cout << "\nEnter the ID of the donation you want to cancel: ";
    cin >> donationID;

    // Find the donation in user's donations array
    int donationIndex = -1;
    for (int i = 0; i < user.nbDonations; i++) {
        if (user.donations[i].donationID == donationID) {
            donationIndex = i;
            break;
        }
    }

    if (donationIndex == -1) {
        cout << "Error: Donation with ID " << donationID << " not found in your donations." << endl;
        return;
    }

    // Get amount and charity ID for updating charity information
    double amount = user.donations[donationIndex].amount;
    int charityID = user.donations[donationIndex].charityID;

    // Update charity's current amount
    ifstream inFile("charities.txt");
    ofstream tempFile("temp.txt");
    if (!inFile || !tempFile) {
        cout << "Error: Cannot access charity files." << endl;
        return;
    }

    string line;
    bool charityFound = false;

    while (getline(inFile, line)) {
        stringstream ss(line);
        Charity charity;
        ss >> charity.charityID;
        ss.ignore(3); // Skip " | "

        getline(ss, charity.name, '|');
        charity.name.erase(0, charity.name.find_first_not_of(" \t"));
        charity.name.erase(charity.name.find_last_not_of(" \t") + 1);

        getline(ss, charity.description, '|');
        charity.description.erase(0, charity.description.find_first_not_of(" \t"));
        charity.description.erase(charity.description.find_last_not_of(" \t") + 1);

        ss >> charity.targetAmount;
        ss.ignore(3); // Skip " | "

        ss >> charity.currentAmount;
        ss.ignore(3); // Skip " | "

        getline(ss, charity.d.date, '|');
        charity.d.date.erase(0, charity.d.date.find_first_not_of(" \t"));
        charity.d.date.erase(charity.d.date.find_last_not_of(" \t") + 1);

        getline(ss, charity.d.time, '|');
        charity.d.time.erase(0, charity.d.time.find_first_not_of(" \t"));
        charity.d.time.erase(charity.d.time.find_last_not_of(" \t") + 1);

        getline(ss, charity.status);
        charity.status.erase(0, charity.status.find_first_not_of(" \t"));
        charity.status.erase(charity.status.find_last_not_of(" \t") + 1);

        if (charity.charityID == charityID) {
            charityFound = true;
            charity.currentAmount -= amount;

            // If charity was closed because target was reached, reopen it
            if (charity.status == "Closed" && charity.currentAmount < charity.targetAmount) {
                charity.status = "Active";
            }
        }

        tempFile << charity.charityID << " | " << charity.name << " | " << charity.description
            << " | " << charity.targetAmount << " | " << charity.currentAmount
            << " | " << charity.d.date << " | " << charity.d.time << " | " << charity.status << "\n";
    }

    inFile.close();
    tempFile.close();

    if (!charityFound) {
        cout << "Error: Charity with ID " << charityID << " not found." << endl;
        remove("temp.txt");
        return;
    }

    // Update charities file
    remove("charities.txt");
    rename("temp.txt", "charities.txt");

    // Remove donation from donations.txt
    ifstream donationInFile("donations.txt");
    ofstream donationTempFile("donationTemp.txt");
    if (!donationInFile || !donationTempFile) {
        cout << "Error: Cannot access donation files." << endl;
        return;
    }

    bool donationRemoved = false;
    while (getline(donationInFile, line)) {
        stringstream ss(line);
        int currentDonationID;
        ss >> currentDonationID;

        if (currentDonationID != donationID) {
            donationTempFile << line << "\n";
        }
        else {
            donationRemoved = true;
        }
    }

    donationInFile.close();
    donationTempFile.close();

    if (!donationRemoved) {
        cout << "Error: Donation not found in file system." << endl;
        remove("donationTemp.txt");
        return;
    }

    // Update donations file
    remove("donations.txt");
    rename("donationTemp.txt", "donations.txt");

    // Remove donation from user's donations array
    for (int i = donationIndex; i < user.nbDonations - 1; i++) {
        user.donations[i] = user.donations[i + 1];
    }
    user.nbDonations--;

    cout << "Donation with ID " << donationID << " has been canceled successfully!" << endl;
}

void modifyDonation(Client& user) {
    if (user.nbDonations == 0) {
        cout << "You haven't made any donations yet." << endl;
        return;
    }

    viewMyDonations(user);

    int donationID;
    cout << "\nEnter the ID of the donation you want to modify: ";
    cin >> donationID;

    // Find the donation in user's donations array
    int donationIndex = -1;
    for (int i = 0; i < user.nbDonations; i++) {
        if (user.donations[i].donationID == donationID) {
            donationIndex = i;
            break;
        }
    }

    if (donationIndex == -1) {
        cout << "Error: Donation with ID " << donationID << " not found in your donations." << endl;
        return;
    }

    // Get original values for updating charity information
    double originalAmount = user.donations[donationIndex].amount;
    int originalCharityID = user.donations[donationIndex].charityID;

    // Show modification options
    int choice;
    cout << "\nWhat would you like to modify?\n";
    cout << "1. Amount\n";
    cout << "2. Target Charity\n";
    cout << "3. Message\n";
    cout << "4. Cancel Modification\n";
    cout << "Enter your choice: ";
    cin >> choice;

    double newAmount = originalAmount;
    int newCharityID = originalCharityID;
    string newMessage = user.donations[donationIndex].message;

    if (choice == 1) {
        cout << "Current amount: $" << originalAmount << endl;
        cout << "Enter new amount: $";
        cin >> newAmount;

        if (newAmount <= 0) {
            cout << "Error: Amount must be greater than zero." << endl;
            return;
        }
    }
    else if (choice == 2) {
        cout << "Current charity ID: " << originalCharityID << endl;
        viewCharities();
        cout << "Enter new charity ID: ";
        cin >> newCharityID;

        // Check if new charity exists and is active
        ifstream charityFile("charities.txt");
        string line;
        bool charityFound = false;
        bool charityActive = false;

        while (getline(charityFile, line)) {
            stringstream ss(line);
            int id;
            string name, status;

            // Read charity ID
            ss >> id;

            if (id == newCharityID) {
                charityFound = true;

                // Skip to status (last field)
                ss.ignore(3); // Skip " | "
                getline(ss, name, '|'); // Skip name
                ss.ignore(3); // Skip " | "
                getline(ss, name, '|'); // Skip description
                ss.ignore(3); // Skip " | "
                ss.ignore(50, '|'); // Skip target amount
                ss.ignore(3); // Skip " | "
                ss.ignore(50, '|'); // Skip current amount
                ss.ignore(3); // Skip " | "
                ss.ignore(50, '|'); // Skip date
                ss.ignore(3); // Skip " | "
                ss.ignore(50, '|'); // Skip time

                getline(ss, status);
                status.erase(0, status.find_first_not_of(" \t"));
                status.erase(status.find_last_not_of(" \t") + 1);

                if (status == "Active") {
                    charityActive = true;
                }
                break;
            }
        }
        charityFile.close();

        if (!charityFound) {
            cout << "Error: Charity with ID " << newCharityID << " not found." << endl;
            return;
        }

        if (!charityActive) {
            cout << "Error: The selected charity is not currently active." << endl;
            return;
        }
    }
    else if (choice == 3) {
        cout << "Current message: " << (newMessage.empty() ? "(none)" : newMessage) << endl;
        cout << "Enter new message (or press Enter for none): ";
        cin.ignore();
        getline(cin, newMessage);
    }
    else if (choice == 4) {
        cout << "Modification canceled." << endl;
        return;
    }
    else {
        cout << "Invalid choice. Modification canceled." << endl;
        return;
    }

    // Update charities.txt for both original and new charity IDs
    vector<Charity> charities;
    ifstream inFile("charities.txt");
    if (!inFile) {
        cout << "Error: Cannot access charity files." << endl;
        return;
    }

    string line;
    while (getline(inFile, line)) {
        stringstream ss(line);
        Charity charity;
        ss >> charity.charityID;
        ss.ignore(3); // Skip " | "

        getline(ss, charity.name, '|');
        charity.name.erase(0, charity.name.find_first_not_of(" \t"));
        charity.name.erase(charity.name.find_last_not_of(" \t") + 1);

        getline(ss, charity.description, '|');
        charity.description.erase(0, charity.description.find_first_not_of(" \t"));
        charity.description.erase(charity.description.find_last_not_of(" \t") + 1);

        ss >> charity.targetAmount;
        ss.ignore(3); // Skip " | "

        ss >> charity.currentAmount;
        ss.ignore(3); // Skip " | "

        getline(ss, charity.d.date, '|');
        charity.d.date.erase(0, charity.d.date.find_first_not_of(" \t"));
        charity.d.date.erase(charity.d.date.find_last_not_of(" \t") + 1);

        getline(ss, charity.d.time, '|');
        charity.d.time.erase(0, charity.d.time.find_first_not_of(" \t"));
        charity.d.time.erase(charity.d.time.find_last_not_of(" \t") + 1);

        getline(ss, charity.status);
        charity.status.erase(0, charity.status.find_first_not_of(" \t"));
        charity.status.erase(charity.status.find_last_not_of(" \t") + 1);

        // Update amounts
        if (charity.charityID == originalCharityID) {
            charity.currentAmount -= originalAmount;

            // If charity was closed because target was reached, reopen it
            if (charity.status == "Closed" && charity.currentAmount < charity.targetAmount) {
                charity.status = "Active";
            }
        }

        if (charity.charityID == newCharityID) {
            charity.currentAmount += newAmount;

            // Check if target has been reached
            if (charity.currentAmount >= charity.targetAmount) {
                charity.status = "Closed";
            }
        }

        charities.push_back(charity);
    }
    inFile.close();

    // Write updated charities back to file
    ofstream outFile("charities.txt");
    for (const auto& charity : charities) {
        outFile << charity.charityID << " | " << charity.name << " | " << charity.description
            << " | " << charity.targetAmount << " | " << charity.currentAmount
            << " | " << charity.d.date << " | " << charity.d.time << " | " << charity.status << "\n";
    }
    outFile.close();

    // Update donations.txt
    ifstream donationInFile("donations.txt");
    ofstream donationTempFile("donationTemp.txt");
    if (!donationInFile || !donationTempFile) {
        cout << "Error: Cannot access donation files." << endl;
        return;
    }

    while (getline(donationInFile, line)) {
        stringstream ss(line);
        int currentDonationID;
        ss >> currentDonationID;

        if (currentDonationID != donationID) {
            donationTempFile << line << "\n";
        }
        else {
            // Extract the user ID
            ss.ignore(3); // Skip " | "
            int userID;
            ss >> userID;

            // Create updated donation entry
            Date currentDT = getCurrentDateTime();
            donationTempFile << donationID << " | " << userID << " | " << newCharityID << " | " << newAmount
                << " | " << currentDT.date << " | " << currentDT.time;

            if (!newMessage.empty()) {
                donationTempFile << " | " << newMessage;
            }
            donationTempFile << "\n";
        }
    }

    donationInFile.close();
    donationTempFile.close();

    // Update donations file
    remove("donations.txt");
    rename("donationTemp.txt", "donations.txt");

    // Update user's donations array
    user.donations[donationIndex].charityID = newCharityID;
    user.donations[donationIndex].amount = newAmount;
    user.donations[donationIndex].message = newMessage;
    user.donations[donationIndex].d = getCurrentDateTime();

    cout << "Donation with ID " << donationID << " has been modified successfully!" << endl;
}

void donorMenu(Client& user) {
    int choice;
    do {
        cout << "\n--- Donor Menu ---\n";
        cout << "1. Browse Charities" << endl;
        cout << "2. Make a Donation" << endl;
        cout << "3. View My Donations" << endl;
        cout << "4. Cancel a Donation" << endl;
        cout << "5. Modify a Donation" << endl;
        cout << "6. Save Donations to PDF" << endl;
        cout << "7. Exit Donor Menu" << endl;
        cout << "Enter choice: ";
        cin >> choice;

        switch (choice) {
        case 1:
            viewCharities(); // Function to view available charities
            break;
        case 2:
            makeDonation(user); // Function to make a donation
            break;
        case 3:
            viewMyDonations(user); // Function to view user's donations
            break;
        case 4:
            cancelDonation(user); // Function to cancel a donation
            break;
        case 5:
            modifyDonation(user); // Function to modify a donation
            break;
        case 6:
            saveDonationsToPDF(user); // Function to save donations to PDF
            break;
        case 7:
            cout << "Exiting Donor Menu." << endl;
            break;
        default:
            cout << "Invalid choice. Please try again." << endl;
        }

    } while (choice != 7);
}

int main() {
    Client loggedUser;
    int choice;
    do {
        showMainMenu(choice);

        switch (choice) {
        case 1:
            if (loginUser(loggedUser)) {
                if (loggedUser.role == "donor") {
                    donorMenu(loggedUser);
                }
                // Admin menu is already called within loginUser if role is admin
            }
            break;
        case 2:
            registerUser();
            break;
        case 3:
            cout << "Thank you for using our Charity Donation System! Goodbye!" << endl;
            break;
        default:
            cout << "Invalid choice. Please try again." << endl;
        }

    } while (choice != 3);

    // Clean up dynamic memory
    if (loggedUser.nbDonations > 0) {
        delete[] loggedUser.donations;
    }

    return 0;
}