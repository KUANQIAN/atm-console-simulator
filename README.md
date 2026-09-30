# Interactive ATM Console Simulator — C++ Program
LDCW6123 Group Project · Part 2: Interactive System (C++)

A console program inspired by the ATM technology traced in our Part 1 Innovation Life Cycle poster. It simulates PIN authentication with a 3-attempt card lockout, balance inquiry, cash withdrawal, cash deposit, printed receipts and a session summary.

---

## Setup Instructions

### Step 1 — Get the source code
Download or clone this repository:

    git clone https://github.com/KUANQIAN/atm-console-simulator.git

Make sure these files are in the SAME folder:
- main.cpp
- README.md
- .gitignore

### Step 2 — Install a C++ compiler
Recommended:
- Dev-C++ (Windows), or any g++ compiler

The program needs C++11 or newer.

### Step 3 — Compile the program

**Dev-C++:**
1. Open main.cpp
2. Go to Tools → Compiler Options → Settings → Code Generation
3. Set Language standard (-std) to ISO C++11
4. Press F11 (Compile & Run)

If you skip step 3, Dev-C++ compiles in C++98 mode and shows errors such as
"'to_string' was not declared" and "range-based 'for' loops are not allowed".

**Command line:**

    g++ -std=c++11 -Wall -Wextra -o atm main.cpp

### Step 4 — Run the program
- Dev-C++: runs automatically after F11
- Windows command line: atm.exe
- macOS / Linux: ./atm

Demo login details:

- PIN: 2580
- Opening balance: RM 1,250.75
- Daily withdrawal limit: RM 1,500.00
- Maximum deposit per transaction: RM 5,000.00

## Program Features

1. **Balance Inquiry** — Shows the current balance, amount withdrawn today and remaining daily limit.

2. **Cash Withdrawal** — Checks the amount, balance and daily withdrawal limit before dispensing cash.

3. **Cash Deposit** — Allows deposits using valid note amounts and prints a receipt.

4. **Exit & Return Card** — Ends the session and shows the transaction summary.

5. **About ATM Technology** — Gives some information related to our Part 1 poster.

## Input Validation

The program checks different types of invalid input before processing a transaction.

- **Wrong PIN:** The card will be blocked after 3 wrong PIN attempts. (Function: authenticatePin())

- **PIN format:** The PIN must contain exactly 4 digits. An incorrect PIN format does not count as an attempt. (Function: authenticatePin())

- **Invalid number input:** Letters, decimals and mixed input such as 12abc are rejected. (Functions: cin.fail(), readWholeNumber())

- **Transaction amount:** The amount must be more than 0, at least RM10 and a multiple of RM10. (Functions: processWithdrawal(), processDeposit())

- **Withdrawal limit:** The withdrawal amount cannot be more than the account balance or the daily withdrawal limit. (Function: processWithdrawal())

- **Money calculation:** Money is stored in sen (whole numbers) to avoid floating-point errors. (Used throughout the program)

## Test Cases

Restart the program before each test.

### T01 - Normal transaction flow
Input: 2580, 2, 180, Y, Y, 3, 200, Y, 4

Expected result:
- RM50 x 3, RM20 x 1 and RM10 x 1 are dispensed.
- Balance changes from RM1,250.75 to RM1,070.75 after the withdrawal, then to RM1,270.75 after the deposit.
- Session summary shows 2 transactions.

### T02 - PIN lockout
Input: 1111, 2222, 3333

Expected result:
- 2 attempts left after the first wrong PIN.
- 1 attempt left after the second wrong PIN.
- Card is blocked after the third wrong PIN.

### T03 - Insufficient funds
Input: 2580, 2, 1300

Expected result:
- "Insufficient funds" is shown.
- Balance remains unchanged.

### T04 - Amount is not a multiple of RM10
Input: 2580, 2, 55

Expected result:
- "Amount must be in multiples of RM10" is shown.

### T05 - Negative / zero amount
Input: 2580, 2, -50, then 2, 0

Expected result:
- "Amount must be greater than zero" is shown.

### T06 - Deposit over the limit
Input: 2580, 3, 6000

Expected result:
- "Maximum deposit per transaction is RM 5,000.00" is shown.

### T07 - Letters in menu
Input: 2580, abc

Expected result:
- "Please enter numbers only" is shown.
- The program does not enter an infinite loop.

### T08 - Mixed input
Input: 2580, 12abc, then 50.5

Expected result:
- "Enter a whole number only" is shown.

### T09 - Invalid menu option
Input: 2580, 9

Expected result:
- "Please choose a number from 1 to 5" is shown.

### T10 - Cancel withdrawal
Input: 2580, 2, 100, N

Expected result:
- "Transaction cancelled" is shown.
- No money is deducted from the balance.

### T11 - Wrong PIN format
Input: ab12, 12345, 0000

Expected result:
- Format errors do not use up an attempt.
- After entering 0000, "2 attempts remaining" is shown.

### T12 - Daily withdrawal limit
Input: 2580, 3, 1000, Y, 2, 1000, Y, Y, 2, 600

Expected result:
- "Daily withdrawal limit exceeded" is shown.
- RM500.00 remains available for withdrawal.

## Development History
View the commit history with:

    git log --oneline --graph

## Team (Sub-Team 2)

- Member 4: Fan Shen Jie (261UE2612C) — Core System Logic and Transaction Validation
- Member 5: Ee Shin Yee (262UE2645C) — Security, UX and Input Error Handling
- Member 6: Loh Kuan Qian (253UE256TL) — GitHub, Testing and Documentation
