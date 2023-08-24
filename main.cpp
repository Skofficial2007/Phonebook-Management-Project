#include <iostream>  // For standard input and output
#include <fstream>   // For file input and output
#include <vector>    // For using the vector container
#include <cstring>   // Include this header for strcpy
#include <limits>    // Include this header for std::numeric_limits
#include <algorithm> // For std::transform
#include <conio.h>   // To use getch()

#ifdef _WIN32 // If the target platform is Windows
#include <windows.h>
#else // If the target platform is not Windows (assumed to be Unix-like)
#include <unistd.h>
#endif

// Clear screen based on the platform
void clearScreen()
{
#ifdef _WIN32
    system("cls"); // clears the output screen on Windows
#else
    system("clear"); // clears the output screen on non-Windows systems
#endif
}

void sleepForOneSecond(int n)
{
#ifdef _WIN32
    Sleep(n * 1000); // Pause for 1000 milliseconds (1 second)
#else
    sleep(n);        // Pause for 1 second
#endif
}

// Function to clear input buffer
void clearInputBuffer()
{
    std::cin.clear();
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
}

// Utility function to perform case-insensitive string comparison
bool caseInsensitiveCompareExact(const std::string &str1, const std::string &str2)
{
    // Convert both strings to lowercase before comparison
    std::string str1Lower = str1;
    std::string str2Lower = str2;
    std::transform(str1Lower.begin(), str1Lower.end(), str1Lower.begin(), ::tolower);
    std::transform(str2Lower.begin(), str2Lower.end(), str2Lower.begin(), ::tolower);

    // Use the == operator to compare the transformed strings
    return str1Lower == str2Lower;
}

// Function to check if a string contains a substring (case-insensitive)
bool containsSubstringCaseInsensitive(const std::string &str, const std::string &substr)
{
    // Convert both strings to lowercase before comparison
    std::string strLower = str;
    std::string substrLower = substr;
    std::transform(strLower.begin(), strLower.end(), strLower.begin(), ::tolower);
    std::transform(substrLower.begin(), substrLower.end(), substrLower.begin(), ::tolower);
    // Use find() to check if the substring exists in the main string
    return strLower.find(substrLower) != std::string::npos;
}

void displayMenu()
{
    std::cout << "\n\n\t\t\t\t--------------------------------------------------- \n";
    std::cout << "\t\t\t\t\t   >>> PHONE BOOK MANAGEMENT <<< \n";
    std::cout << "\t\t\t\t--------------------------------------------------- \n\n";
    std::cout << "\t\t\t\t1. Show all contacts            2. Search with Name\n\n";
    std::cout << "\t\t\t\t3. Search with Phone No.        4. Search with Group\n\n";
    std::cout << "\t\t\t\t5. Add a new contact            6. Modify a contact\n\n";
    std::cout << "\t\t\t\t7. Delete a contact             8. Delete all contacts\n\n";
    std::cout << "\t\t\t\t9. Sort by name                 10. Exit\n\n";
}

// Contact class to hold contact information
class Contact
{
public:
    char name[50];
    char phoneNo[15];
    char email[50];
    char group[20];

    // Default constructor
    Contact()
    {
        name[0] = '\0';
        phoneNo[0] = '\0';
        email[0] = '\0';
        group[0] = '\0';
    }

    bool operator<(const Contact &other) const
    {
        return std::strcmp(name, other.name) < 0;
    }

    bool operator==(const Contact &other) const
    {
        return std::strcmp(name, other.name) == 0;
    }
};

// Phonebook class to manage contacts
class Phonebook
{
private:
    std::vector<Contact> contacts;

    // Vector to store predefined custom groups
    std::vector<std::string> customGroups{"Family", "Friend", "Work", "Other"};

    // Method to validate phone number
    bool isValidPhoneNumber(const char *phoneNo) const
    {
        // Check if the phone number is exactly 10 digits and contains only digits
        return (std::strlen(phoneNo) == 10 && std::strspn(phoneNo, "0123456789") == 10);
    }

    // Method to validate email
    bool isValidEmail(char *email) const
    {
        // Convert the email to lowercase before comparison
        std::string emailLower = email;
        std::transform(emailLower.begin(), emailLower.end(), emailLower.begin(), ::tolower);

        // Case-insensitive comparison with "na"
        if (emailLower == "na")
        {
            return true; // "na" is considered valid
        }

        // Find the position of the '@' character in the email
        const char *atPosition = std::strchr(email, '@');

        // Check if the '@' character is missing or if it's at the beginning or the end
        if (!atPosition || atPosition == email || *(atPosition + 1) == '\0')
        {
            // If any of these conditions are met, the email is invalid
            return false; // No '@' character or '@' is at the beginning or end
        }

        // Check if the domain is not valid in email
        const char *domain = atPosition + 1;
        if (!(std::strstr(domain, "gmail.com") || std::strstr(domain, "yahoo.com") || std::strstr(domain, "email.com")))
        {
            return false; // Valid domain not found
        }

        return true; // Valid email
    }

    // Method to display group options and get user's choice
    int getGroupChoice()
    {
        std::cout << "Select a group:" << std::endl;
        for (size_t i = 0; i < customGroups.size(); ++i)
        {
            std::cout << i + 1 << ". " << customGroups[i] << std::endl;
        }
        std::cout << customGroups.size() + 1 << ". Add your own group" << std::endl;

        int choice = 0;

        while (choice < 1 || choice > customGroups.size() + 1)
        {
            std::cout << "Enter your choice: ";
            char choiceChar;
            std::cin >> choiceChar;
            clearInputBuffer();

            choice = choiceChar - '0';

            if (choice < 1 || choice > customGroups.size() + 1)
            {
                std::cout << "Invalid choice. Please enter a valid option." << std::endl;
            }
        }

        return choice;
    }

public:
    // Method to load contacts from a binary file
    void loadFromFile(const char *filename)
    {
        contacts.clear(); // Clear the vector before loading data

        std::ifstream inFile(filename, std::ios::binary | std::ios::in);
        if (!inFile)
        {
            std::cerr << "Error opening file for reading." << std::endl;
            return;
        }

        Contact contact;
        while (inFile.read(reinterpret_cast<char *>(&contact), sizeof(Contact)))
        {
            contacts.push_back(contact);
        }

        inFile.close();
    }

    // Add a contact to the phonebook
    void addContact(const Contact &contact)
    {
        contacts.push_back(contact);
    }

    // Save contacts to a binary file
    void saveToFile(const char *filename)
    {
        std::ofstream outFile(filename, std::ios::binary | std::ios::out);
        if (!outFile)
        {
            std::cerr << "Error opening file for writing." << std::endl;
            return;
        }

        for (const Contact &contact : contacts)
        {
            outFile.write(reinterpret_cast<const char *>(&contact), sizeof(Contact));
        }

        outFile.close();
    }

    // Function to add a new contact through user input
    void addContactFromUserInput()
    {
        Contact newContact;

        std::cout << "\n.......CREATE NEW PHONE RECORD.........\n";
        fflush(stdin); // clears the input buffers like '\n'
        std::cout << "Name: ";
        std::cin.getline(newContact.name, sizeof(newContact.name));

        // Input validation for Phone Number
        bool validPhone = false;
        do
        {
            std::cout << "Phone: ";
            std::cin.getline(newContact.phoneNo, sizeof(newContact.phoneNo));

            validPhone = isValidPhoneNumber(newContact.phoneNo);

            if (!validPhone)
            {
                std::cout << "Invalid phone number. Phone number should be exactly 10 digits and contain only digits." << std::endl;
            }
        } while (!validPhone);

        // Input validation for Email
        bool validEmail = false;
        do
        {
            std::cout << "Email (Enter 'NA' to leave empty): ";
            std::cin.getline(newContact.email, sizeof(newContact.email));

            // Convert the entered email to uppercase before comparison
            std::string enteredEmail = newContact.email;
            std::transform(enteredEmail.begin(), enteredEmail.end(), enteredEmail.begin(), ::toupper);

            // Check if the entered email is "NA" (case-insensitive)
            if (enteredEmail == "NA")
            {
                validEmail = true;
                std::strcpy(newContact.email, "NA");
            }
            else
            {
                validEmail = isValidEmail(newContact.email);

                // Truncate the email at ".com" if present
                char *comPosition = std::strstr(newContact.email, ".com");
                if (comPosition && *(comPosition + 4) != '\0')
                {
                    *(comPosition + 4) = '\0'; // Truncate the string at ".com"
                }

                if (!validEmail)
                {
                    std::cout << "Invalid email. Email should contain @gmail.com, @yahoo.com, or @email.com."
                              << std::endl;
                }
            }
        } while (!validEmail);

        // Get the group choice from the user
        int groupChoice = getGroupChoice();

        if (groupChoice == customGroups.size() + 1)
        {
            // User wants to add their own group
            std::string customGroup;
            std::cout << "Enter your own group: ";
            std::getline(std::cin, customGroup);
            customGroups.push_back(customGroup);

            // Set the newContact.group to the custom group
            std::strcpy(newContact.group, customGroups.back().c_str());
        }
        else
        {
            // Predefined or custom groups
            std::strcpy(newContact.group, customGroups[groupChoice - 1].c_str());
        }

        // Add the new contact to the phonebook
        addContact(newContact);

        // Save the updated phonebook to file
        saveToFile("contacts.dat");
    }

    // Print all contacts
    void printContacts()
    {
        if (contacts.empty())
        {
            std::cout << "\nPhonebook is empty." << std::endl;
            return;
        }

        // std::cout << "\n...............PHONE BOOK RECORD...............\n";
        std::cout << "\n\n\t\t\t\t--------------------------------------------------- \n";
        std::cout << "\t\t\t\t\t   >>> PHONE BOOK RECORD <<< \n";
        std::cout << "\t\t\t\t--------------------------------------------------- \n\n";
        for (const Contact &contact : contacts)
        {
            std::cout << "\n-----------------------------------" << std::endl;
            std::cout << "Name: " << contact.name << std::endl;
            std::cout << "Phone: " << contact.phoneNo << std::endl;
            std::cout << "Email: " << contact.email << std::endl;
            std::cout << "Group: " << contact.group << std::endl;
            std::cout << "-----------------------------------" << std::endl;
        }
    }

    // Modify the searchByName function
    void searchByName(const std::string &name)
    {
        if (contacts.empty())
        {
            std::cout << "Phonebook is empty. No contacts to search." << std::endl;
            return;
        }

        bool found = false;
        std::cout << "\nSearch Results by Name: " << name << std::endl;
        for (const Contact &contact : contacts)
        {
            // Check if the contact's name contains the partial name (case-insensitive)
            if (containsSubstringCaseInsensitive(contact.name, name))
            {
                found = true;
                std::cout << "-----------------------------------" << std::endl;
                std::cout << "Name: " << contact.name << std::endl;
                std::cout << "Phone: " << contact.phoneNo << std::endl;
                std::cout << "Email: " << contact.email << std::endl;
                std::cout << "Group: " << contact.group << std::endl;
                std::cout << "-----------------------------------" << std::endl;
            }
        }
        if (!found)
        {
            std::cout << "No contacts found with the given name." << std::endl;
        }
    }

    // Method to search contacts by a part of the phone number
    void searchByPhoneNumber(const std::string &partialPhoneNo)
    {
        if (contacts.empty())
        {
            std::cout << "Phonebook is empty. No contacts to search." << std::endl;
            return;
        }

        bool found = false;
        std::cout << "\nSearch Results by Phone Number: " << partialPhoneNo << std::endl;
        for (const Contact &contact : contacts)
        {
            if (std::strstr(contact.phoneNo, partialPhoneNo.c_str()))
            {
                found = true;
                std::cout << "-----------------------------------" << std::endl;
                std::cout << "Name: " << contact.name << std::endl;
                std::cout << "Phone: " << contact.phoneNo << std::endl;
                std::cout << "Email: " << contact.email << std::endl;
                std::cout << "Group: " << contact.group << std::endl;
                std::cout << "-----------------------------------" << std::endl;
            }
        }
        if (!found)
        {
            std::cout << "No contacts found with the given partial phone number." << std::endl;
        }
    }

    // Modify the searchByGroup function
    void searchByGroup(const std::string &group)
    {
        if (contacts.empty())
        {
            std::cout << "Phonebook is empty. No contacts to search." << std::endl;
            return;
        }

        bool found = false;
        std::cout << "\nSearch Results by Group: " << group << std::endl;
        for (const Contact &contact : contacts)
        {
            // Check if the contact's group contains the partial group name (case-insensitive)
            if (containsSubstringCaseInsensitive(contact.group, group))
            {
                found = true;
                std::cout << "-----------------------------------" << std::endl;
                std::cout << "Name: " << contact.name << std::endl;
                std::cout << "Phone: " << contact.phoneNo << std::endl;
                std::cout << "Email: " << contact.email << std::endl;
                std::cout << "Group: " << contact.group << std::endl;
                std::cout << "-----------------------------------" << std::endl;
            }
        }
        if (!found)
        {
            std::cout << "No contacts found in the given group." << std::endl;
        }
    }

    // Method to modify a contact's information
    void modifyContact(const std::string &name)
    {
        if (contacts.empty())
        {
            std::cout << "Phonebook is empty. Cannot modify contact." << std::endl;
            return;
        }

        bool found = false;

        // Iterate through the contacts to find the contact to be modified
        for (Contact &contact : contacts)
        {
            // Case-insensitive comparison for exact name match
            if (caseInsensitiveCompareExact(contact.name, name))
            {
                found = true;

                // Display the current contact information
                std::cout << "Current Contact Information:" << std::endl;
                std::cout << "Name: " << contact.name << std::endl;
                std::cout << "Phone: " << contact.phoneNo << std::endl;
                std::cout << "Email: " << contact.email << std::endl;
                std::cout << "Group: " << contact.group << std::endl;

                // Prompt the user for modification choice
                char modifyChoice;
                std::cout << "\nDo you want to modify the entire contact? (y/n): ";
                std::cin >> modifyChoice;

                clearInputBuffer(); // Clear input buffer after reading choice

                if (modifyChoice == 'y' || modifyChoice == 'Y')
                {
                    // Modify the entire contact
                    std::cout << "Enter new information for the contact:" << std::endl;
                    std::cout << "Name: ";
                    std::cin.getline(contact.name, sizeof(contact.name));

                    // Validate the new phone number
                    bool validPhone = false;
                    do
                    {
                        std::cout << "Enter new phone number: ";
                        std::cin.getline(contact.phoneNo, sizeof(contact.phoneNo));

                        validPhone = isValidPhoneNumber(contact.phoneNo);

                        if (!validPhone)
                        {
                            std::cout << "Invalid phone number. Phone number should be exactly 10 digits and contain only digits." << std::endl;
                        }
                    } while (!validPhone);

                    // Validate the new email
                    bool validEmail = false;
                    do
                    {
                        std::cout << "Enter new email: ";
                        std::cin.getline(contact.email, sizeof(contact.email));

                        // Convert the entered email to uppercase before comparison
                        std::string enteredEmail = contact.email;
                        std::transform(enteredEmail.begin(), enteredEmail.end(), enteredEmail.begin(), ::toupper);

                        // Check if the entered email is "NA" (case-insensitive)
                        if (enteredEmail == "NA")
                        {
                            validEmail = true;
                            std::strcpy(contact.email, "NA");
                        }
                        else
                        {
                            validEmail = isValidEmail(contact.email);

                            // Truncate the email at ".com" if present
                            char *comPosition = std::strstr(contact.email, ".com");
                            if (comPosition && *(comPosition + 4) != '\0')
                            {
                                *(comPosition + 4) = '\0'; // Truncate the string at ".com"
                            }

                            if (!validEmail)
                            {
                                std::cout << "Invalid email. Email should contain @gmail.com, @yahoo.com, or @email.com."
                                          << std::endl;
                            }
                        }
                    } while (!validEmail);

                    int groupChoice = getGroupChoice();
                    if (groupChoice == customGroups.size() + 1)
                    {
                        // User wants to add their own group
                        std::cout << "Enter your own group: ";
                        std::cin.getline(contact.group, sizeof(contact.group));
                        customGroups.push_back(contact.group);
                    }
                    else
                    {
                        std::strcpy(contact.group, customGroups[groupChoice - 1].c_str());
                    }
                }
                else
                {
                    // Modify specific parts of the contact
                    std::cout << "Choose the part you want to modify:" << std::endl;
                    std::cout << "1. Name" << std::endl;
                    std::cout << "2. Phone" << std::endl;
                    std::cout << "3. Email" << std::endl;
                    std::cout << "4. Group" << std::endl;
                    std::cout << "Enter your choice: ";
                    char partChoice;
                    std::cin >> partChoice;
                    clearInputBuffer();

                    switch (partChoice)
                    {
                    case '1':
                        std::cout << "Enter new name: ";
                        std::cin.getline(contact.name, sizeof(contact.name));
                        break;
                    case '2':
                    {
                        // Validate the new phone number
                        bool validPhone = false;
                        do
                        {
                            std::cout << "Enter new phone number: ";
                            std::cin.getline(contact.phoneNo, sizeof(contact.phoneNo));

                            validPhone = isValidPhoneNumber(contact.phoneNo);

                            if (!validPhone)
                            {
                                std::cout << "Invalid phone number. Phone number should be exactly 10 digits and contain only digits." << std::endl;
                            }
                        } while (!validPhone);
                    }
                    break;
                    case '3':
                    {
                        // Validate the new email
                        bool validEmail = false;
                        do
                        {
                            std::cout << "Enter new email: ";
                            std::cin.getline(contact.email, sizeof(contact.email));

                            // Convert the entered email to uppercase before comparison
                            std::string enteredEmail = contact.email;
                            std::transform(enteredEmail.begin(), enteredEmail.end(), enteredEmail.begin(), ::toupper);

                            // Check if the entered email is "NA" (case-insensitive)
                            if (enteredEmail == "NA")
                            {
                                validEmail = true;
                                std::strcpy(contact.email, "NA");
                            }
                            else
                            {
                                validEmail = isValidEmail(contact.email);

                                // Truncate the email at ".com" if present
                                char *comPosition = std::strstr(contact.email, ".com");
                                if (comPosition && *(comPosition + 4) != '\0')
                                {
                                    *(comPosition + 4) = '\0'; // Truncate the string at ".com"
                                }

                                if (!validEmail)
                                {
                                    std::cout << "Invalid email. Email should contain @gmail.com, @yahoo.com, or @email.com."
                                              << std::endl;
                                }
                            }
                        } while (!validEmail);
                    }
                    break;
                    case '4':
                    {
                        int groupChoice = getGroupChoice();
                        if (groupChoice == customGroups.size() + 1)
                        {
                            // User wants to add their own group
                            std::cout << "Enter your own group: ";
                            std::cin.getline(contact.group, sizeof(contact.group));
                            customGroups.push_back(contact.group);
                        }
                        else
                        {
                            std::strcpy(contact.group, customGroups[groupChoice - 1].c_str());
                        }
                    }
                    break;
                    default:
                        std::cout << "Invalid choice. Contact not modified." << std::endl;
                        break;
                    }
                }

                // Save the modified phonebook to file
                saveToFile("contacts.dat");

                std::cout << "\nContact information has been modified." << std::endl;
                break; // Exit the loop once the contact is modified
            }
        }

        if (!found)
        {
            std::cout << "\nNo contact found with the given name." << std::endl;
        }
    }

    // Method to delete a contact by name
    void deleteContact(const std::string &name)
    {
        // Check if the phonebook is already empty
        if (contacts.empty())
        {
            std::cout << "\nPhonebook is already empty. No contacts to delete." << std::endl;
            return;
        }

        // Use the remove_if algorithm to move contacts to be deleted to the end of the vector
        auto iter = std::remove_if(contacts.begin(), contacts.end(), [&name](const Contact &contact)
                                   { return caseInsensitiveCompareExact(contact.name, name); });

        // Check if any contact was found and marked for deletion
        if (iter != contacts.end())
        {
            // Erase the contacts marked for deletion from the vector
            contacts.erase(iter, contacts.end());
            std::cout << "\nContact '" << name << "' has been deleted." << std::endl;

            // Save the updated phonebook to file
            saveToFile("contacts.dat");
        }
        else
        {
            // No matching contact found for deletion
            std::cout << "\nNo contact found with the given name." << std::endl;
        }
    }

    // Method to delete all contacts
    void deleteAllContacts()
    {
        if (contacts.empty())
        {
            std::cout << "\nPhonebook is already empty. No contacts to delete." << std::endl;
            return;
        }

        contacts.clear();
        std::cout << "\nAll contacts have been deleted." << std::endl;
        saveToFile("contacts.dat"); // Save the empty phonebook to file
    }

    // Method to sort all contacts by name
    void sortContactsByName()
    {
        std::sort(contacts.begin(), contacts.end());
    }
};

int main()
{
    system("color 0A");
    Phonebook phonebook;

    int choice;
    std::string searchName;
    std::string partialPhoneNo;
    std::string searchGroup;
    std::string modifyName;
    std::string deleteName;

    // Load existing contacts from the file
    phonebook.loadFromFile("contacts.dat");

    // Sort contacts by name
    phonebook.sortContactsByName();

    do
    {
        clearScreen();
        displayMenu();

        // Getting user's choice
        std::cout << "Enter your choice: ";
        std::cin >> choice;

        if (std::cin.fail())
        {
            clearScreen();
            std::cout << "Invalid choice. Please enter a valid option.\n";
            sleepForOneSecond(1);
            clearInputBuffer(); // Clear input buffer
            continue;
        }

        clearInputBuffer(); // Clear input buffer after reading choice

        switch (choice)
        {
        case 1:
            // Show all contacts
            phonebook.printContacts();
            std::cout << "\n\n-> Press any key to continue : ";
            getch();
            break;
        case 2:
            // Search by Name
            std::cout << "Enter name to search: ";
            std::getline(std::cin, searchName);
            phonebook.searchByName(searchName);
            std::cout << "\n\n-> Press any key to continue : ";
            getch();
            break;
        case 3:
            // Search by Partial Phone Number
            std::cout << "Enter partial phone number to search: ";
            std::getline(std::cin, partialPhoneNo);
            phonebook.searchByPhoneNumber(partialPhoneNo);
            std::cout << "\n\n-> Press any key to continue : ";
            getch();
            break;
        case 4:
            // Search by Group
            std::cout << "Enter group to search: ";
            std::getline(std::cin, searchGroup);
            phonebook.searchByGroup(searchGroup);
            std::cout << "\n\n-> Press any key to continue : ";
            getch();
            break;
        case 5:
            // Add a new contact
            phonebook.addContactFromUserInput();
            break;
        case 6:
            // Modify a contact
            std::cout << "Enter the name of the contact to modify: ";
            std::getline(std::cin, modifyName);
            phonebook.modifyContact(modifyName);
            std::cout << "\n\n-> Press any key to continue : ";
            getch();
            break;
        case 7:
            // Delete a contact
            std::cout << "Enter the name of the contact to delete: ";
            std::getline(std::cin, deleteName);
            phonebook.deleteContact(deleteName);
            std::cout << "\n\n-> Press any key to continue : ";
            getch();
            break;
        case 8:
            // Delete all contacts
            phonebook.deleteAllContacts();
            std::cout << "\n\n-> Press any key to continue : ";
            getch();
            break;
        case 9:
            // Sort contacts by name
            phonebook.sortContactsByName();
            std::cout << "\nContacts sorted by name." << std::endl;
            std::cout << "\n\n-> Press any key to continue : ";
            getch();
            break;
        case 10:
            // Exit the program
            clearScreen();
            std::cout << "\n\n---------Exiting the Phonebook---------\n\n";
            sleepForOneSecond(1);
            break;
        default:
            clearScreen();
            std::cout << "Invalid choice. Please enter a valid option.\n";
            sleepForOneSecond(1);
            break;
        }
    } while (choice != 10);

    return 0;
}
