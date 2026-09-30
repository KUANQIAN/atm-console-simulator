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

// One record in the session's transaction history
struct Transaction {
    int       reference;
    string    type;
    long long amountSen;
    long long balanceAfterSen;
    string    timestamp;
};

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

// ==================================================================
//  Input helpers (basic version - validation to be added later)
// ==================================================================

long long readWholeNumber(const string& prompt) {
    long long value = 0;
    cout << prompt;
    cin >> value;
    return value;
}

bool readYesNo(const string& prompt) {
    char answer;
    cout << prompt;
    cin >> answer;
    return answer == 'Y' || answer == 'y';
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

void displayMenu() {
    printHeader("MAIN MENU");
    cout << "  [1] Balance Inquiry\n"
         << "  [2] Cash Withdrawal\n"
         << "  [3] Cash Deposit\n"
         << "  [4] Exit & Return Card\n"
         << "  [5] About ATM Technology\n";
    printLine('-');
}

void printReceipt(const Transaction& record) {
    cout << '\n';
    printLine('-');
    printCentered(BANK_NAME);
    printCentered("TRANSACTION RECEIPT");
    printLine('-');
    printRow("Date / Time", record.timestamp);
    printRow("Reference No.", "TXN" + to_string(record.reference));
    printRow("Account", maskAccountNumber(ACCOUNT_NUMBER));
    printRow("Transaction", record.type);
    printRow("Amount", formatRM(record.amountSen));
    printRow("Available Balance", formatRM(record.balanceAfterSen));
    printLine('-');
    printCentered("Thank you for banking with us");
    printLine('-');
}

// Links the program back to the Part 1 innovation life cycle poster.
// Keep these lines consistent with the dates used on your poster.
void showAboutAtm() {
    printHeader("ABOUT ATM TECHNOLOGY");
    cout << "  1966  James Goodfellow patents the idea of a\n"
         << "        machine-readable card with a PIN.\n"
         << "  1967  First cash dispenser opens at Barclays,\n"
         << "        Enfield, London (John Shepherd-Barron).\n"
         << "  1969  Magnetic-stripe cards used in US ATMs.\n"
         << "  Today Networked ATMs offer 24/7 self-service.\n";
    printLine('-');
    cout << "  Features simulated in this program:\n"
         << "   - PIN verification with 3-attempt card lockout\n"
         << "   - Real-time balance check before dispensing\n"
         << "   - Daily withdrawal limit (fraud control)\n"
         << "   - Note-based dispensing and cash deposit\n"
         << "   - Printed transaction receipt\n";
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
//  Core transactions
// ==================================================================

void showBalance(long long balanceSen, long long withdrawnTodayRM) {
    printHeader("BALANCE INQUIRY");
    printRow("Account Holder", ACCOUNT_HOLDER);
    printRow("Account Number", maskAccountNumber(ACCOUNT_NUMBER));
    printRow("Available Balance", formatRM(balanceSen));
    printRow("Withdrawn Today", formatRM(withdrawnTodayRM * SEN_PER_RM));
    printRow("Daily Limit Left", formatRM((DAILY_LIMIT_RM - withdrawnTodayRM) * SEN_PER_RM));
}


// Splits the amount into the fewest notes (largest note first)
void dispenseNotes(long long amountRM) {
    const int NOTE_VALUES[] = {50, 20, 10};
    cout << "\n  Dispensing cash, please collect your notes:\n";
    for (int note : NOTE_VALUES) {
        long long count = amountRM / note;
        amountRM = amountRM % note;
        if (count > 0) {
            cout << "    RM" << setw(3) << left << note << " x " << count << '\n';
        }
    }
}

void processWithdrawal(long long& balanceSen, long long& withdrawnTodayRM,
                       vector<Transaction>& history) {
    printHeader("CASH WITHDRAWAL");
    long long limitLeftRM = DAILY_LIMIT_RM - withdrawnTodayRM;
    printRow("Available Balance", formatRM(balanceSen));
    printRow("Daily Limit Left", formatRM(limitLeftRM * SEN_PER_RM));
    printRow("Notes Available", "RM50, RM20, RM10");

    long long amountRM = readWholeNumber("\n  Enter amount to withdraw (RM): ");

    // Validation order: sign -> minimum -> note multiple -> balance -> daily limit
    if (amountRM <= 0) {
        cout << "  [X] Amount must be greater than zero.\n";
    } else if (amountRM < MIN_AMOUNT_RM) {
        cout << "  [X] Minimum withdrawal is RM" << MIN_AMOUNT_RM << ".\n";
    } else if (amountRM % NOTE_MULTIPLE_RM != 0) {
        cout << "  [X] Amount must be in multiples of RM" << NOTE_MULTIPLE_RM
             << " (notes only).\n";
    } else if (amountRM > balanceSen / SEN_PER_RM) {
        // Integer comparison avoids overflow for very large inputs
        cout << "  [X] Insufficient funds. Your available balance is "
             << formatRM(balanceSen) << ".\n";
    } else if (amountRM > limitLeftRM) {
        cout << "  [X] Daily withdrawal limit exceeded. You can withdraw up to "
             << formatRM(limitLeftRM * SEN_PER_RM) << " more today.\n";
    } else {
        if (!readYesNo("  Confirm withdrawal of " + formatRM(amountRM * SEN_PER_RM) + "? (Y/N): ")) {
            cout << "  Transaction cancelled. No money was deducted.\n";
            return;
        }
        balanceSen       -= amountRM * SEN_PER_RM;
        withdrawnTodayRM += amountRM;

        Transaction record = {static_cast<int>(history.size()) + 1001, "Cash Withdrawal",
                              amountRM * SEN_PER_RM, balanceSen, currentTimestamp()};
        history.push_back(record);

        dispenseNotes(amountRM);
        printReceipt(record);
    }
}

void processDeposit(long long& balanceSen, vector<Transaction>& history) {
    printHeader("CASH DEPOSIT");
    cout << "  Accepted notes: RM10, RM20, RM50, RM100\n"
         << "  Coins are not accepted.\n";

    long long amountRM = readWholeNumber("\n  Enter amount to deposit (RM): ");

    if (amountRM <= 0) {
        cout << "  [X] Invalid amount. Deposit must be greater than zero.\n";
    } else if (amountRM % NOTE_MULTIPLE_RM != 0) {
        cout << "  [X] Amount must be in multiples of RM" << NOTE_MULTIPLE_RM
             << " (notes only).\n";
    } else if (amountRM > MAX_DEPOSIT_RM) {
        cout << "  [X] Maximum deposit per transaction is "
             << formatRM(MAX_DEPOSIT_RM * SEN_PER_RM) << ".\n";
    } else {
        balanceSen += amountRM * SEN_PER_RM;

        Transaction record = {static_cast<int>(history.size()) + 1001, "Cash Deposit",
                              amountRM * SEN_PER_RM, balanceSen, currentTimestamp()};
        history.push_back(record);

        cout << "  [OK] Deposit successful. New balance: " << formatRM(balanceSen) << '\n';
        printReceipt(record);
    }
}

void displaySessionSummary(const vector<Transaction>& history, long long balanceSen) {
    printHeader("SESSION SUMMARY");
    if (history.empty()) {
        cout << "  No transactions were made in this session.\n";
    } else {
        cout << "  " << left << setw(10) << "Ref" << setw(18) << "Type"
             << right << setw(18) << "Amount" << '\n';
        printLine('-');
        for (const Transaction& record : history) {
            cout << "  " << left << setw(10) << ("TXN" + to_string(record.reference))
                 << setw(18) << record.type
                 << right << setw(18) << formatRM(record.amountSen) << '\n';
        }
    }
    printLine('-');
    printRow("Closing Balance", formatRM(balanceSen));
    printLine('=');
    printCentered("Please take your card. Goodbye!");
    printLine('=');
}

// ==================================================================
//  Main program: authentication, then the session loop
// ==================================================================

int main() {
    long long balanceSen       = OPENING_BALANCE_SEN;
    long long withdrawnTodayRM = 0;
    vector<Transaction> history;

    displayWelcomeScreen();

    if (!authenticatePin()) {
        displayCardBlocked();
        return 0;
    }

    bool sessionActive = true;
    do {
        displayMenu();
        long long choice = readWholeNumber("  Select an option (1-5): ");

        switch (choice) {
            case 1:
                showBalance(balanceSen, withdrawnTodayRM);
                break;
            case 2:
                processWithdrawal(balanceSen, withdrawnTodayRM, history);
                break;
            case 3:
                processDeposit(balanceSen, history);
                break;
            case 4:
                sessionActive = false;
                break;
            case 5:
                showAboutAtm();
                break;
            default:
                cout << "  [!] Invalid option. Please choose a number from 1 to 5.\n";
                continue;   // show the menu again straight away
        }

        if (sessionActive) {
            sessionActive = readYesNo("\n  Perform another transaction? (Y/N): ");
        }
    } while (sessionActive);

    displaySessionSummary(history, balanceSen);
    return 0;
}
