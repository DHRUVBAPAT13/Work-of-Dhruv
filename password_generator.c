
#include <stdio.h> 
#include <stdlib.h> 
#include <time.h> 
#include <string.h>
#include <ctype.h>

#define MAX_PASSWORD_LENGTH 128

#define MAX_PASSWORDS 50

typedef struct {
    int length;
    int useLower;
    int useUpper;
    int useDigits;
    int useSpecial;
} PasswordConfig;

typedef struct {
    char passwords[MAX_PASSWORDS][MAX_PASSWORD_LENGTH];
    int count;
} PasswordList;

// Function prototypes
void clearScreen();
void displayMainMenu();
void displayCharacterMenu();
void displaySettingsMenu();
void generatePassword(char *password, PasswordConfig config);
void displayPasswords(PasswordList *list);
void savePasswordsToFile(PasswordList *list);
void showPasswordStrength(const char *password);
int getValidatedInput(int min, int max);
void getUserSettings(PasswordConfig *config);

// Clear screen for better UX
void clearScreen() {
    #ifdef _WIN32
        system("cls");
    #else
        system("clear");
    #endif
}

// Display fancy separator
void printSeparator() {
    printf("\n");
    for (int i = 0; i < 50; i++) printf("═");
    printf("\n\n");
}

// Main menu
void displayMainMenu() {
    clearScreen();
    printf("\n");
    printf("    ╔════════════════════════════════════════════╗\n");
    printf("    ║    🔐 PASSWORD GENERATOR v1.0              ║\n");
    printf("    ╚════════════════════════════════════════════╝\n");
    printSeparator();
    printf("  1. ⚡ Generate Passwords (Quick)\n");
    printf("  2. ⚙️  Configure Settings\n");
    printf("  3. 💾 Save Passwords to File\n");
    printf("  4. ℹ️  About This App\n");
    printf("  5. 🚪 Exit\n");
    printSeparator();
    printf("  Enter your choice (1-5): ");
}

// Character selection menu
void displayCharacterMenu() {
    printf("\n");
    printf("  ╔════════════════════════════════════════════╗\n");
    printf("  ║     SELECT CHARACTER TYPES TO INCLUDE     ║\n");
    printf("  ╚══════���═════════════════════════════════════╝\n");
    printSeparator();
}

// Settings menu
void displaySettingsMenu(PasswordConfig *config) {
    clearScreen();
    printf("\n");
    printf("  ╔════════════════════════════════════════════╗\n");
    printf("  ║           PASSWORD SETTINGS               ║\n");
    printf("  ╚════════════════════════════════════════════╝\n");
    printSeparator();
    printf("  Current Settings:\n");
    printf("  • Length: %d characters\n", config->length);
    printf("  • Lowercase: %s\n", config->useLower ? "✓ Enabled" : "✗ Disabled");
    printf("  • Uppercase: %s\n", config->useUpper ? "✓ Enabled" : "✗ Disabled");
    printf("  • Digits: %s\n", config->useDigits ? "✓ Enabled" : "✗ Disabled");
    printf("  • Special Characters: %s\n", config->useSpecial ? "✓ Enabled" : "✗ Disabled");
    printSeparator();
    printf("  1. Change Password Length\n");
    printf("  2. Toggle Character Types\n");
    printf("  3. Reset to Default\n");
    printf("  4. Back to Main Menu\n");
    printSeparator();
    printf("  Enter your choice (1-4): ");
}

// Get validated integer input
int getValidatedInput(int min, int max) {
    int choice;
    while (scanf("%d", &choice) != 1 || choice < min || choice > max) {
        while (getchar() != '\n'); // Clear input buffer
        printf("  ❌ Invalid input! Please enter a number between %d and %d: ", min, max);
    }
    while (getchar() != '\n'); // Clear newline from buffer
    return choice;
}

// Generate a single password
void generatePassword(char *password, PasswordConfig config) {
    char lowercase[] = "abcdefghijklmnopqrstuvwxyz";
    char uppercase[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
    char digits[] = "0123456789";
    char special[] = "!@#$%^&*-_+=[]{}|;:,.<>?";
    
    char charset[200] = "";
    
    // Build character set
    if (config.useLower) strcat(charset, lowercase);
    if (config.useUpper) strcat(charset, uppercase);
    if (config.useDigits) strcat(charset, digits);
    if (config.useSpecial) strcat(charset, special);
    
    // Generate password
    for (int i = 0; i < config.length; i++) {
        int randomIndex = rand() % strlen(charset);
        password[i] = charset[randomIndex];
    }
    password[config.length] = '\0';
}

// Evaluate password strength
void showPasswordStrength(const char *password) {
    int length = strlen(password);
    int hasLower = 0, hasUpper = 0, hasDigit = 0, hasSpecial = 0;
    
    for (int i = 0; i < length; i++) {
        if (islower(password[i])) hasLower = 1;
        if (isupper(password[i])) hasUpper = 1;
        if (isdigit(password[i])) hasDigit = 1;
        if (!isalnum(password[i])) hasSpecial = 1;
    }
    
    int strength = hasLower + hasUpper + hasDigit + hasSpecial;
    
    printf("  Strength: ");
    if (length < 8) {
        printf("🔴 Weak (too short)\n");
    } else if (strength == 4 && length >= 12) {
        printf("🟢 Very Strong\n");
    } else if (strength == 4 || length >= 12) {
        printf("🟢 Strong\n");
    } else if (strength >= 2) {
        printf("🟡 Medium\n");
    } else {
        printf("🔴 Weak\n");
    }
}

// Display generated passwords
void displayPasswords(PasswordList *list) {
    if (list->count == 0) {
        printf("  ❌ No passwords generated yet!\n");
        return;
    }
    
    printf("\n  ╔════════════════════════════════════════════╗\n");
    printf("  ║        GENERATED PASSWORDS (%d)            ║\n", list->count);
    printf("  ╚════════════════════════════════════════════╝\n");
    printSeparator();
    
    for (int i = 0; i < list->count; i++) {
        printf("  Password %d: %s\n", i + 1, list->passwords[i]);
        showPasswordStrength(list->passwords[i]);
        printf("\n");
    }
}

// Get user settings
void getUserSettings(PasswordConfig *config) {
    displayCharacterMenu();
    
    printf("  Include lowercase letters (a-z)? (1/0): ");
    config->useLower = getValidatedInput(0, 1);
    
    printf("  Include uppercase letters (A-Z)? (1/0): ");
    config->useUpper = getValidatedInput(0, 1);
    
    printf("  Include digits (0-9)? (1/0): ");
    config->useDigits = getValidatedInput(0, 1);
    
    printf("  Include special characters (!@#$%%)? (1/0): ");
    config->useSpecial = getValidatedInput(0, 1);
    
    if (!config->useLower && !config->useUpper && !config->useDigits && !config->useSpecial) {
        printf("\n  ⚠️  Warning: Select at least one character type!\n");
        printf("  Using default: Uppercase + Lowercase + Digits\n");
        config->useLower = 1;
        config->useUpper = 1;
        config->useDigits = 1;
    }
    
    printf("\n  ✓ Settings updated!\n");
}

// Save passwords to file
void savePasswordsToFile(PasswordList *list) {
    if (list->count == 0) {
        printf("\n  ❌ No passwords to save!\n");
        return;
    }
    
    FILE *file = fopen("generated_passwords.txt", "w");
    if (file == NULL) {
        printf("\n  ❌ Error: Could not create file!\n");
        return;
    }
    
    fprintf(file, "═══════════════════════════════════════════\n");
    fprintf(file, "       PASSWORD GENERATOR - OUTPUT\n");
    fprintf(file, "═══════════════════════════════════════════\n\n");
    
    for (int i = 0; i < list->count; i++) {
        fprintf(file, "Password %d: %s\n", i + 1, list->passwords[i]);
    }
    
    fprintf(file, "\n═══════════════════════════════════════════\n");
    fprintf(file, "Generated on: ");
    time_t now = time(NULL);
    fprintf(file, "%s", ctime(&now));
    
    fclose(file);
    printf("\n  ✓ Passwords saved to 'generated_passwords.txt'\n");
}

// Display about information
void displayAbout() {
    clearScreen();
    printf("\n");
    printf("  ╔════════════════════════════════════════════╗\n");
    printf("  ║          ABOUT THIS APPLICATION           ║\n");
    printf("  ╚════════════════════════════════════════════╝\n");
    printSeparator();
    printf("  Password Generator v1.0\n");
    printf("  Created for MIT WPU First Year Students\n\n");
    printf("  Features:\n");
    printf("  • Generate secure random passwords\n");
    printf("  • Customize character types and length\n");
    printf("  • Evaluate password strength\n");
    printf("  • Save passwords to file\n");
    printf("  • User-friendly interface\n");
    printSeparator();
    printf("  Press Enter to continue...");
    getchar();
}

// Main program
int main() {
    srand(time(0));
    
    PasswordConfig config = {
        .length = 12,
        .useLower = 1,
        .useUpper = 1,
        .useDigits = 1,
        .useSpecial = 1
    };
    
    PasswordList passwordList = {
        .count = 0
    };
    
    int running = 1;
    
    while (running) {
        displayMainMenu();
        int choice = getValidatedInput(1, 5);
        
        switch (choice) {
            case 1: {
                clearScreen();
                printf("\n  ╔════════════════════════════════════════════╗\n");
                printf("  ║        QUICK PASSWORD GENERATION           ║\n");
                printf("  ╚════════════════════════════════════════════╝\n");
                printSeparator();
                
                printf("  How many passwords to generate? (1-%d): ", MAX_PASSWORDS);
                int count = getValidatedInput(1, MAX_PASSWORDS);
                
                printf("\n  ⏳ Generating %d password(s)...\n", count);
                
                passwordList.count = count;
                for (int i = 0; i < count; i++) {
                    generatePassword(passwordList.passwords[i], config);
                }
                
                displayPasswords(&passwordList);
                printf("  Press Enter to continue...");
                getchar();
                break;
            }
            
            case 2: {
                int settingsRunning = 1;
                while (settingsRunning) {
                    displaySettingsMenu(&config);
                    int settingsChoice = getValidatedInput(1, 4);
                    
                    switch (settingsChoice) {
                        case 1: {
                            clearScreen();
                            printf("\n  Enter password length (8-128): ");
                            config.length = getValidatedInput(8, 128);
                            printf("  ✓ Length updated to %d!\n", config.length);
                            printf("  Press Enter to continue...");
                            getchar();
                            break;
                        }
                        case 2: {
                            getUserSettings(&config);
                            printf("  Press Enter to continue...");
                            getchar();
                            break;
                        }
                        case 3: {
                            config = (PasswordConfig){12, 1, 1, 1, 1};
                            printf("\n  ✓ Settings reset to default!\n");
                            printf("  Press Enter to continue...");
                            getchar();
                            break;
                        }
                        case 4:
                            settingsRunning = 0;
                            break;
                    }
                }
                break;
            }
            
            case 3: {
                clearScreen();
                printSeparator();
                savePasswordsToFile(&passwordList);
                printf("  Press Enter to continue...");
                getchar();
                break;
            }
            
            case 4: {
                displayAbout();
                break;
            }
            
            case 5: {
                clearScreen();
                printf("\n");
                printf("  ╔════════════════════════════════════════════╗\n");
                printf("  ║   Thank you for using Password Generator!  ║\n");
                printf("  ║            Goodbye! 👋                     ║\n");
                printf("  ╚════════════════════════════════════════════╝\n");
                printf("\n");
                running = 0;
                break;
            }
        }
    }
    
    return 0;
}