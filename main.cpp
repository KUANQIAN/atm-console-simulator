/*
 * ================================================================
 *  Project : Interactive ATM Console Simulator
 *  Course  : LDCW6123 Fundamentals of Digital Competence for Programmer
 *  Part 2  : Interactive C++ Program (linked to Part 1 ATM poster)
 *
 *  Purpose : Simulates the core customer operations of a networked
 *            ATM terminal: PIN authentication (3-attempt lockout),
 *            balance inquiry, cash withdrawal (note + balance +
 *            daily-limit checks), cash deposit, and receipts.
 *
 *  Build   : g++ -std=c++17 -Wall -Wextra -o atm main.cpp
 *  Run     : ./atm        (Windows: atm.exe)
 *  Demo PIN: 2580
 * ================================================================
 */

#include <iostream>
#include <iomanip>
#include <string>
#include <vector>
#include <limits>
#include <ctime>
#include <cctype>
#include <cstdlib>

using namespace std;

// ------------------------------------------------------------------
// Configuration constants
// Money is stored in SEN (1 RM = 100 sen) as whole numbers, so the
// program never suffers from floating-point rounding errors.
// ------------------------------------------------------------------
const string    BANK_NAME            = "MMU DIGITAL BANK";
const string    ACCOUNT_HOLDER       = "LIM WEI JIE";
const string    ACCOUNT_NUMBER       = "1234567890";
const string    CORRECT_PIN          = "2580";   // stored as text so a PIN like "0123" keeps its leading zero
const int       MAX_PIN_ATTEMPTS     = 3;
const long long SEN_PER_RM           = 100;
const long long OPENING_BALANCE_SEN  = 125075;   // RM 1,250.75
const long long DAILY_LIMIT_RM       = 1500;
const long long MIN_AMOUNT_RM        = 10;
const long long NOTE_MULTIPLE_RM     = 10;       // smallest note the machine handles
const long long MAX_DEPOSIT_RM       = 5000;     // per-transaction deposit cap
const int       SCREEN_WIDTH         = 52;

// ==================================================================
//  Display helpers
// ==================================================================

void printLine(char symbol = '=') {
    cout << string(SCREEN_WIDTH, symbol) << '\n';
}

void printCentered(const string& text) {
    int padding = (SCREEN_WIDTH - static_cast<int>(text.length())) / 2;
    if (padding < 0) {
        padding = 0;
    }
    cout << string(padding, ' ') << text << '\n';
}

void printHeader(const string& title) {
    cout << '\n';
    printLine('=');
    printCentered(title);
    printLine('=');
}

// Prints "  Label ............ : value" in aligned columns
void printRow(const string& label, const string& value) {
    cout << "  " << left << setw(22) << label << ": " << value << '\n';
}

// Converts sen to a readable string, e.g. 125075 -> "RM 1,250.75"
string formatRM(long long sen) {
    bool isNegative = sen < 0;
    if (isNegative) {
        sen = -sen;
    }
    long long ringgit = sen / SEN_PER_RM;
    long long cents   = sen % SEN_PER_RM;

    string digits = to_string(ringgit);
    string withCommas;
    int digitCount = 0;
    for (int i = static_cast<int>(digits.length()) - 1; i >= 0; --i) {
        withCommas.insert(withCommas.begin(), digits[i]);
        digitCount++;
        if (digitCount % 3 == 0 && i > 0) {
            withCommas.insert(withCommas.begin(), ',');
        }
    }

    string centsText = (cents < 10 ? "0" : "") + to_string(cents);
    return string(isNegative ? "-" : "") + "RM " + withCommas + "." + centsText;
}

// Hides all but the last 4 digits, e.g. "******7890"
string maskAccountNumber(const string& accountNumber) {
    if (accountNumber.length() <= 4) {
        return accountNumber;
    }
    return string(accountNumber.length() - 4, '*') +
           accountNumber.substr(accountNumber.length() - 4);
}

string currentTimestamp() {
    time_t now = time(nullptr);
    char buffer[25];
    strftime(buffer, sizeof(buffer), "%d/%m/%Y %H:%M:%S", localtime(&now));
    return string(buffer);
}

bool isFourDigitPin(const string& text) {
    if (text.length() != 4) {
        return false;
    }
    for (char ch : text) {
        if (!isdigit(static_cast<unsigned char>(ch))) {
            return false;
        }
    }
    return true;
}

// ==================================================================
//  Screens
// ==================================================================

void displayWelcomeScreen() {
    cout << '\n';
    printLine('*');
    printCentered(BANK_NAME);
    printCentered("AUTOMATED TELLER MACHINE");
    printLine('*');
    printCentered("Please insert your card...");
    printCentered("[ Card detected: " + maskAccountNumber(ACCOUNT_NUMBER) + " ]");
    printLine('-');
    cout << "  Shield the keypad when entering your PIN.\n";
}

void displayCardBlocked() {
    printHeader("CARD BLOCKED");
    cout << "  Too many incorrect PIN attempts (" << MAX_PIN_ATTEMPTS << "/"
         << MAX_PIN_ATTEMPTS << ").\n"
         << "  Your card has been retained for security.\n"
         << "  Please contact your bank branch to reactivate it.\n";
    printLine('=');
}

// ==================================================================
//  Security: PIN authentication
// ==================================================================

// Returns true when the correct PIN is entered within 3 attempts.
bool authenticatePin() {
    int attemptsUsed = 0;
    string enteredPin;

    while (attemptsUsed < MAX_PIN_ATTEMPTS) {
        cout << "\n  Enter your 4-digit PIN: ";
        cin >> enteredPin;

        if (!isFourDigitPin(enteredPin)) {
            cout << "  [!] Invalid format. PIN must be exactly 4 digits (0-9).\n";
        } else if (enteredPin == CORRECT_PIN) {
            cout << "  [OK] PIN verified. Welcome, " << ACCOUNT_HOLDER << ".\n";
            return true;
        } else {
            attemptsUsed++;
            int remaining = MAX_PIN_ATTEMPTS - attemptsUsed;
            if (remaining > 0) {
                cout << "  [X] Incorrect PIN. " << remaining << " attempt"
                     << (remaining == 1 ? "" : "s") << " remaining.\n";
                if (remaining == 1) {
                    cout << "  [!] WARNING: One more wrong PIN will block your card.\n";
                }
            }
        }
    }
    return false;
}

// ==================================================================
//  Main program
// ==================================================================

int main() {
    displayWelcomeScreen();

    if (!authenticatePin()) {
        displayCardBlocked();
        return 0;
    }

    cout << "\n  Main menu coming soon.\n";
    return 0;
}
