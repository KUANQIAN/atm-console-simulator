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
//  Money calculation helpers
// ==================================================================

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


// ==================================================================
//  Main program
// ==================================================================

int main() {
    long long balanceSen = OPENING_BALANCE_SEN;

    cout << "Account number        : " << ACCOUNT_NUMBER << '\n';
    cout << "Opening balance       : " << formatRM(balanceSen) << '\n';
    cout << "Daily withdrawal limit: " << formatRM(DAILY_LIMIT_RM * SEN_PER_RM) << '\n';
    return 0;
}
