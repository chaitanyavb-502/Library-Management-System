 __   ____ ____ ____   __   ____ _  _    __  __   __   _  _   __   ___ ____ __  __ ____ _  _ ____    ___ _  _ ___ ____ ____ __  __ 
(  ) (_  _(  _ (  _ \ /__\ (  _ ( \/ )  (  \/  ) /__\ ( \( ) /__\ / __( ___(  \/  ( ___( \( (_  _)  / __( \/ / __(_  _( ___(  \/  )
 )(__ _)(_ ) _ <)   //(__)\ )   /\  /    )    ( /(__)\ )  ( /(__)( (_-.)__) )    ( )__) )  (  )(    \__ \\  /\__ \ )(  )__) )    ( 
(____(____(____(_)\_(__)(__(_)\_)(__)   (_/\/\_(__)(__(_)\_(__)(__\___(____(_/\/\_(____(_)\_)(__)   (___/(__)(___/(__)(____(_/\/\_)

Overview

The Library Management System is a C++ application built using object-oriented programming (OOP) principles. It simulates a digital library where different user types (Students, Faculty, and Librarians) interact with the system via a command-line interface (CLI). The system demonstrates key OOP concepts such as inheritance, encapsulation, polymorphism, and abstraction. Data including book records and user account details (with borrowing history and fines) are persisted across sessions using file I/O.
Features

    User Roles
        Students: Can borrow up to 3 books at a time for 15 days. Overdue returns incur fines (10 rupees per day).
        Faculty: Can borrow up to 5 books at a time for 30 days with extended privileges and no overdue fines.
        Librarians: Manage books and user accounts (cannot borrow books).

    Book Management
        Add, remove, and update book records.
        Each book record includes: title, author, publisher, year, ISBN, and current status (Available, Borrowed, Reserved).

    Account Management
        Automatic recording of borrowing date and due date.
        Automatic calculation of overdue fines based on return date.
        Detailed account persistence including currently borrowed books (with borrow and due dates) and borrowing history.

    Data Persistence
        Book data is stored in books.txt.
        User data, along with detailed account information, is stored in users.txt in a CSV-like format.

    Aesthetic CLI
        The CLI features ASCII art headers on the main menu and each user menu for an enhanced visual experience.

Requirements

    Compiler: A C++ compiler supporting C++11 or higher (e.g., g++).
    Operating System: Compatible with Linux, Windows, or macOS.
    Dependencies: Uses standard C++ libraries; no external libraries are required.

Installation & Compilation

    Download the Source Code:
    Ensure the library.cpp file is in your working directory.

    Open a Terminal/Command Prompt:
    Navigate to the directory containing library.cpp.

    Compile the Code:
    For Linux/macOS, use:

    g++ -std=c++11 -o library_system library.cpp

    For Windows, use a similar command with your preferred compiler (e.g., using MinGW).

Running the Application

After compilation, run the executable:

    Linux/macOS:

./library_system

Windows:

    library_system.exe

How to Use
Main Menu

Upon starting the application, an ASCII art banner is displayed along with the main menu options:

    Login:
    Enter your user ID and password to access your account.
    Register New User:
    For new users to register (choose between Student and Faculty).
    List All Books:
    Displays all the books in the library.
    Help/Instructions:
    Provides an overview of how to use the system.
    Exit:
    Closes the application and saves all data.

User Interaction

    Students & Faculty:
        Borrow Books:
        When borrowing a book, the system automatically records the current day and sets a due period (15 days for students, 30 days for faculty). The confirmation message displays, for example:
        "Book 'XYZ' borrowed successfully. Due after 15 days."
        Return Books:
        When returning a book, the system automatically calculates the overdue fine (if applicable) and updates your account.
        View Account Details:
        Displays currently borrowed books, borrowing history, and outstanding fines.
    Librarians:
        Book Management:
        Add, remove, or update book records.
        User Management:
        Add new users, remove users, or update user details.

Data Persistence

    Books Data:
    Saved in books.txt.
    Users Data:
    Saved in users.txt using a CSV-like format with detailed account information.

When the application exits, all data is automatically saved, and it is reloaded the next time the application is run.
Customization & Further Enhancements

    Due Dates & Fines:
    Modify the borrowing periods and fine rates directly in the source code.
    ASCII Art:
    Customize the ASCII art in the printMainHeader() and printUserHeader() functions to personalize the CLI.
    File Format:
    The current file format uses simple CSV-like text files. This can be extended to a more robust solution if needed.

Contact:

For any questions or further assistance, please contact:
Chaitanya Vishwas Bramhapurikar
Email: bcvishwas23@iitk.ac.in | chaitanyavb2020@gmail.com