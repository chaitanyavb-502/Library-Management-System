#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <limits>
#include <algorithm>
#include <ctime>
using namespace std;

// ------------------------
// Enumerations & Utilities
// ------------------------
enum BookStatus { AVAILABLE, BORROWED, RESERVED };

string bookStatusToString(BookStatus status) {
    switch(status) {
        case AVAILABLE: return "Available";
        case BORROWED:  return "Borrowed";
        case RESERVED:  return "Reserved";
        default:        return "Unknown";
    }
}

// ------------------------
// Book Class
// ------------------------
class Book {
private:
    string title, author, publisher, isbn;
    int year;
    BookStatus status;
public:
    Book() : title(""), author(""), publisher(""), year(0), isbn(""), status(AVAILABLE) {}
    Book(const string &title, const string &author, const string &publisher, int year, const string &isbn, BookStatus status = AVAILABLE)
        : title(title), author(author), publisher(publisher), year(year), isbn(isbn), status(status) {}

    void setTitle(const string &t) { title = t; }
    string getTitle() const { return title; }
    
    void setAuthor(const string &a) { author = a; }
    string getAuthor() const { return author; }
    
    void setPublisher(const string &p) { publisher = p; }
    string getPublisher() const { return publisher; }
    
    void setYear(int y) { year = y; }
    int getYear() const { return year; }
    
    void setISBN(const string &i) { isbn = i; }
    string getISBN() const { return isbn; }
    
    void setStatus(BookStatus s) { status = s; }
    BookStatus getStatus() const { return status; }
    
    void printDetails() const {
        cout << "Title: " << title << "\nAuthor: " << author 
             << "\nPublisher: " << publisher << "\nYear: " << year
             << "\nISBN: " << isbn << "\nStatus: " << bookStatusToString(status) << "\n";
    }
};

// ------------------------
// Account Class with Borrow/History Records
// ------------------------
struct BorrowInfo {
    Book* book;
    int borrowDate;
    int dueDate;
};

struct HistoryRecord {
    Book* book;
    int borrowDate;
    int dueDate;
    int returnDate;
    double fineIncurred;
};

class Library; // Forward declaration needed for Account::deserialize

class Account {
public:
    vector<BorrowInfo> borrowedBooks;
    vector<HistoryRecord> history;
    double fines; // outstanding total fine

    Account() : fines(0) {}

    void addBorrowedBook(Book* book, int borrowDate, int dueDate) {
         borrowedBooks.push_back({book, borrowDate, dueDate});
    }

    // When returning a book, we compute overdue (if any) and update the fine.
    void returnBorrowedBook(Book* book, int returnDate, bool isFaculty) {
         auto it = find_if(borrowedBooks.begin(), borrowedBooks.end(),
               [book](const BorrowInfo &bi){ return bi.book == book; });
         if(it != borrowedBooks.end()){
              int due = it->dueDate;
              int overdue = (returnDate > due) ? (returnDate - due) : 0;
              double fine = 0;
              if(!isFaculty) {
                 fine = overdue * 10;  // 10 rupees per day
                 fines += fine;
              }
              history.push_back({book, it->borrowDate, due, returnDate, fine});
              borrowedBooks.erase(it);
         } else {
              cout << "Error: Book not found in your borrowed list.\n";
         }
    }

    void listBorrowedBooks() const {
         if(borrowedBooks.empty()){
            cout << "No books currently borrowed.\n";
            return;
         }
         cout << "Currently Borrowed Books:\n";
         for(auto &bi : borrowedBooks){
             cout << "- " << bi.book->getTitle() << " (Borrowed on day " << bi.borrowDate 
                  << ", Due on day " << bi.dueDate << ")\n";
         }
    }

    void listHistory() const {
         if(history.empty()){
            cout << "No borrowing history available.\n";
            return;
         }
         cout << "Borrowing History:\n";
         for(auto &hr : history){
             cout << "- " << hr.book->getTitle() << " (Borrowed on day " << hr.borrowDate 
                  << ", Due on day " << hr.dueDate << ", Returned on day " << hr.returnDate 
                  << ", Fine: " << hr.fineIncurred << ")\n";
         }
    }

    int getBorrowedCount() const {
         return borrowedBooks.size();
    }

    bool hasOverdueExceeding(int currentDay, int extraDays) const {
         for(auto &bi : borrowedBooks) {
              if(currentDay - bi.dueDate > extraDays)
                  return true;
         }
         return false;
    }

    // Serialize account details to a string.
    // Format: fines,borrowCount,borrowRecord1;borrowRecord2;...,historyCount,historyRecord1;historyRecord2;...
    // Each borrowRecord: ISBN:borrowDate:dueDate
    // Each historyRecord: ISBN:borrowDate:dueDate:returnDate:fineIncurred
    string serialize() const {
         ostringstream oss;
         oss << fines << "," << borrowedBooks.size() << ",";
         for(size_t i = 0; i < borrowedBooks.size(); i++){
             oss << borrowedBooks[i].book->getISBN() << ":" << borrowedBooks[i].borrowDate << ":" << borrowedBooks[i].dueDate;
             if(i != borrowedBooks.size()-1) oss << ";";
         }
         oss << "," << history.size() << ",";
         for(size_t i = 0; i < history.size(); i++){
             oss << history[i].book->getISBN() << ":" << history[i].borrowDate << ":" << history[i].dueDate 
                 << ":" << history[i].returnDate << ":" << history[i].fineIncurred;
             if(i != history.size()-1) oss << ";";
         }
         return oss.str();
    }

    // Deserialize account details from a string.
    void deserialize(const string &data, Library &lib);
};

// ------------------------
// Forward Declaration of Library
// (Its full definition will come later.)
class Library;

// ------------------------
// Abstract Class User
// ------------------------
class User {
private: 
    string password;
protected:
    int id;
    string name;
    Account account;
public:
    User(int id, const string &name, const string &password)
        : id(id), name(name), password(password) {}
    virtual ~User() {}

    int getId() const { return id; }
    string getName() const { return name; }
    void setName(const string &newName) { name = newName; }

    string getPassword() const { return password; }
    bool checkPassword(const string &pass) const { return pass == password; }
    void setPassword(const string &newPass) { password = newPass; }

    Account& getAccount() { return account; }

    virtual void borrowBook(Library &lib) = 0;
    virtual void returnBook(Library &lib) = 0;
    virtual void menu(Library &lib) = 0;
    virtual string getType() const = 0;
};

// ------------------------
// Derived Classes Declarations
// ------------------------
class Student : public User {
public:
    Student(int id, const string &name, const string &password) : User(id, name, password) {}
    virtual void borrowBook(Library &lib);
    virtual void returnBook(Library &lib);
    virtual void menu(Library &lib);
    virtual string getType() const { return "Student"; }
};

class Faculty : public User {
public:
    Faculty(int id, const string &name, const string &password) : User(id, name, password) {}
    virtual void borrowBook(Library &lib);
    virtual void returnBook(Library &lib);
    virtual void menu(Library &lib);
    virtual string getType() const { return "Faculty"; }
};

class Librarian : public User {
public:
    Librarian(int id, const string &name, const string &password) : User(id, name, password) {}
    virtual void borrowBook(Library &lib);
    virtual void returnBook(Library &lib);
    virtual void menu(Library &lib);
    virtual string getType() const { return "Librarian"; }
};

// ------------------------
// Library Class Definition
// ------------------------
class Library {
private:
    vector<Book> books;
    vector<User*> users; // stored as pointers
public:
    Library() {}
    ~Library() {
        for(auto user : users)
            delete user;
    }
    
    bool isBooksEmpty() const { return books.empty(); }
    bool isUsersEmpty() const { return users.empty(); }

    // Book Methods
    void addBook(const Book &book) {
        books.push_back(book);
    }

    void removeBook(const string &isbn) {
        auto it = remove_if(books.begin(), books.end(), [isbn](const Book &b) { return b.getISBN() == isbn; });
        if(it != books.end()){
            books.erase(it, books.end());
            cout << "Book with ISBN " << isbn << " removed.\n";
        } else {
            cout << "Book not found.\n";
        }
    }

    Book* findBookByISBN(const string &isbn) {
        for(auto &book : books) {
            if(book.getISBN() == isbn)
                return &book;
        }
        return nullptr;
    }

    void listBooks() {
        if(books.empty()){
            cout << "No books in the library.\n";
            return;
        }
        cout << "\n--- Library Books ---\n";
        for(auto &book : books) {
            book.printDetails();
            cout << "-------------------------\n";
        }
    }

    void searchBooks() {
        int option;
        cout << "\nSearch Books by:\n1. Title\n2. Author\n3. ISBN\nEnter choice: ";
        cin >> option;
        cin.ignore();
        string query;
        cout << "Enter search query: ";
        getline(cin, query);
        bool found = false;
        for(auto &book : books) {
            if((option == 1 && book.getTitle().find(query) != string::npos) ||
               (option == 2 && book.getAuthor().find(query) != string::npos) ||
               (option == 3 && book.getISBN().find(query) != string::npos)) {
                book.printDetails();
                cout << "-------------------------\n";
                found = true;
            }
        }
        if(!found)
            cout << "No matching books found.\n";
    }

    // User Methods
    void addUser(User *user) {
        if(findUserById(user->getId()) != nullptr) {
            cout << "User with ID " << user->getId() << " already exists. Cannot add duplicate.\n";
            delete user;
            return;
        }
        users.push_back(user);
    }

    User* findUserById(int id) {
        for(auto user : users) {
            if(user->getId() == id)
                return user;
        }
        return nullptr;
    }

    void removeUser(int id) {
        auto it = remove_if(users.begin(), users.end(), [id](User* u) { return u->getId() == id; });
        if(it != users.end()){
            delete *it;
            users.erase(it, users.end());
            cout << "User with ID " << id << " removed.\n";
        } else {
            cout << "User not found.\n";
        }
    }

    void updateUser(int id) {
        User* user = findUserById(id);
        if(user) {
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << "Enter new name for user with ID " << id << ": ";
            string newName;
            getline(cin, newName);
            user->setName(newName);
            cout << "User updated successfully.\n";
        } else {
            cout << "User not found.\n";
        }
    }

    void listUsers() {
        if(users.empty()){
            cout << "No users registered.\n";
            return;
        }
        cout << "\n--- Registered Users ---\n";
        for(auto user : users) {
            cout << "ID: " << user->getId() << " | Name: " << user->getName() << "\n";
        }
    }

    // Persistence Functions
    void saveData() {
        // Save books to books.txt
        ofstream fout("books.txt");
        if(fout.is_open()){
            for(auto &book : books) {
                fout << book.getTitle() << "," << book.getAuthor() << "," 
                     << book.getPublisher() << "," << book.getYear() << "," 
                     << book.getISBN() << "," << book.getStatus() << "\n";
            }
            fout.close();
            cout << "Books saved to books.txt\n";
        }
        // Save users to users.txt in format:
        // id|name|password|type|accountData
        ofstream fout2("users.txt");
        if(fout2.is_open()){
            for(auto user : users) {
                fout2 << user->getId() << "|" << user->getName() << "|" << user->getPassword()
                      << "|" << user->getType() << "|" << user->getAccount().serialize() << "\n";
            }
            fout2.close();
            cout << "Users saved to users.txt\n";
        }
    }

    void loadData() {
        // Load books
        ifstream fin("books.txt");
        if(fin.is_open()){
            books.clear();
            string line;
            while(getline(fin, line)){
                stringstream ss(line);
                string title, author, publisher, yearStr, isbn, statusStr;
                if(getline(ss, title, ',') && getline(ss, author, ',') && getline(ss, publisher, ',') &&
                   getline(ss, yearStr, ',') && getline(ss, isbn, ',') && getline(ss, statusStr)){
                    int year = stoi(yearStr);
                    int statInt = stoi(statusStr);
                    BookStatus status = static_cast<BookStatus>(statInt);
                    books.push_back(Book(title, author, publisher, year, isbn, status));
                }
            }
            fin.close();
            cout << "Books loaded from books.txt\n";
        }
        // Load users
        ifstream fin2("users.txt");
        if(fin2.is_open()){
            for(auto user : users)
                delete user;
            users.clear();
            string line;
            while(getline(fin2, line)){
                vector<string> parts;
                stringstream ss(line);
                string token;
                while(getline(ss, token, '|')) {
                    parts.push_back(token);
                }
                if(parts.size() != 5) continue;
                int id = stoi(parts[0]);
                string name = parts[1];
                string password = parts[2];
                string type = parts[3];
                string accountData = parts[4];
                User* user = nullptr;
                if(type == "Student")
                    user = new Student(id, name, password);
                else if(type == "Faculty")
                    user = new Faculty(id, name, password);
                else if(type == "Librarian")
                    user = new Librarian(id, name, password);
                if(user) {
                    user->getAccount().deserialize(accountData, *this);
                    users.push_back(user);
                }
            }
            fin2.close();
            cout << "Users loaded from users.txt\n";
        }
    }
};

// ------------------------
// Account::deserialize Implementation
// ------------------------
void Account::deserialize(const string &data, Library &lib) {
    vector<string> parts;
    stringstream ss(data);
    string token;
    while(getline(ss, token, ',')) {
         parts.push_back(token);
    }
    if(parts.size() < 5) return; // invalid format
    fines = stod(parts[0]);
    int borrowCount = stoi(parts[1]);
    borrowedBooks.clear();
    if(borrowCount > 0) {
         string borrowRecords = parts[2];
         stringstream ssb(borrowRecords);
         string rec;
         while(getline(ssb, rec, ';')) {
              vector<string> recParts;
              stringstream ssrec(rec);
              string field;
              while(getline(ssrec, field, ':')) {
                   recParts.push_back(field);
              }
              if(recParts.size() == 3) {
                  string isbn = recParts[0];
                  int bDate = stoi(recParts[1]);
                  int dDate = stoi(recParts[2]);
                  Book* b = lib.findBookByISBN(isbn);
                  if(b)
                     borrowedBooks.push_back({b, bDate, dDate});
              }
         }
    }
    int historyCount = stoi(parts[3]);
    history.clear();
    if(historyCount > 0) {
         string historyRecords = parts[4];
         stringstream ssh(historyRecords);
         string rec;
         while(getline(ssh, rec, ';')) {
              vector<string> recParts;
              stringstream ssrec(rec);
              string field;
              while(getline(ssrec, field, ':')) {
                   recParts.push_back(field);
              }
              if(recParts.size() == 5) {
                  string isbn = recParts[0];
                  int bDate = stoi(recParts[1]);
                  int dDate = stoi(recParts[2]);
                  int rDate = stoi(recParts[3]);
                  double fine = stod(recParts[4]);
                  Book* b = lib.findBookByISBN(isbn);
                  if(b)
                     history.push_back({b, bDate, dDate, rDate, fine});
              }
         }
    }
}

// ------------------------
// Derived Classes Member Function Definitions
// ------------------------

// Student
void Student::borrowBook(Library &lib) {
    if(account.getBorrowedCount() >= 3) {
         cout << "Borrowing limit reached (max 3 books allowed).\n";
         return;
    }
    if(account.fines > 0) {
         cout << "Please clear outstanding fines before borrowing.\n";
         return;
    }
    cout << "Enter ISBN of the book to borrow: ";
    string isbn;
    cin >> isbn;
    Book* book = lib.findBookByISBN(isbn);
    if(book == nullptr) {
         cout << "Book not found.\n";
         return;
    }
    if(book->getStatus() != AVAILABLE) {
         cout << "Book is currently not available.\n";
         return;
    }
    int currentDay = time(0) / (24 * 3600);
    int dueDate = currentDay + 15;
    book->setStatus(BORROWED);
    account.addBorrowedBook(book, currentDay, dueDate);
    cout << "Book \"" << book->getTitle() << "\" borrowed successfully" 
         << ". Due after 15 days" << ".\n";
}

void Student::returnBook(Library &lib) {
    cout << "Enter ISBN of the book to return: ";
    string isbn;
    cin >> isbn;
    Book* book = lib.findBookByISBN(isbn);
    if(book == nullptr) {
         cout << "Book not found.\n";
         return;
    }
    int currentDay = time(0) / (24 * 3600);
    account.returnBorrowedBook(book, currentDay, false);
    book->setStatus(AVAILABLE);
    cout << "Book \"" << book->getTitle() << "\" returned successfully" << ".\n";
}

void Student::menu(Library &lib) {
    int choice;
    do {
        cout << "\n===== Student Menu =====\n";
        cout << "1. Borrow Book\n2. Return Book\n3. View Account Details\n4. Pay Fines\n5. List All Books\n6. Search Books\n7. Logout\n";
        cout << "Enter your choice: ";
        cin >> choice;
        switch(choice) {
            case 1: borrowBook(lib); break;
            case 2: returnBook(lib); break;
            case 3:
                account.listBorrowedBooks();
                account.listHistory();
                cout << "Outstanding Fines: " << account.fines << " rupees\n";
                break;
            case 4: 
                if(account.fines > 0) {
                    cout << "Paying fine of " << account.fines << " rupees.\n";
                    account.fines = 0;
                } else {
                    cout << "No outstanding fines.\n";
                }
                break;
            case 5: lib.listBooks(); break;
            case 6: lib.searchBooks(); break;
            case 7: cout << "Logging out...\n"; break;
            default: cout << "Invalid choice. Please try again.\n";
        }
    } while(choice != 7);
}

// Faculty
void Faculty::borrowBook(Library &lib) {
    if(account.getBorrowedCount() >= 5) {
         cout << "Borrowing limit reached (max 5 books allowed).\n";
         return;
    }
    int currentDay = time(0) / (24 * 3600);
    if(account.hasOverdueExceeding(currentDay, 60)) {
         cout << "You have a book overdue by more than 60 days. Cannot borrow new books.\n";
         return;
    }
    cout << "Enter ISBN of the book to borrow: ";
    string isbn;
    cin >> isbn;
    Book* book = lib.findBookByISBN(isbn);
    if(book == nullptr) {
         cout << "Book not found.\n";
         return;
    }
    if(book->getStatus() != AVAILABLE) {
         cout << "Book is not available.\n";
         return;
    }
    int dueDate = currentDay + 30;
    book->setStatus(BORROWED);
    account.addBorrowedBook(book, currentDay, dueDate);
    cout << "Book \"" << book->getTitle() << "\" borrowed successfully"
         << ". Due after 30 days " << ".\n";
}

void Faculty::returnBook(Library &lib) {
    cout << "Enter ISBN of the book to return: ";
    string isbn;
    cin >> isbn;
    Book* book = lib.findBookByISBN(isbn);
    if(book == nullptr) {
         cout << "Book not found.\n";
         return;
    }
    int currentDay = time(0) / (24 * 3600);
    account.returnBorrowedBook(book, currentDay, true);
    book->setStatus(AVAILABLE);
    cout << "Book \"" << book->getTitle() << "\" returned successfully" << "\n";
}

void Faculty::menu(Library &lib) {
    int choice;
    do {
        cout << "\n===== Faculty Menu =====\n";
        cout << "1. Borrow Book\n2. Return Book\n3. View Account Details\n4. List All Books\n5. Search Books\n6. Logout\n";
        cout << "Enter your choice: ";
        cin >> choice;
        switch(choice) {
            case 1: borrowBook(lib); break;
            case 2: returnBook(lib); break;
            case 3:
                account.listBorrowedBooks();
                account.listHistory();
                cout << "Outstanding Fines: " << account.fines << " rupees\n";
                break;
            case 4: lib.listBooks(); break;
            case 5: lib.searchBooks(); break;
            case 6: cout << "Logging out...\n"; break;
            default: cout << "Invalid choice. Please try again.\n";
        }
    } while(choice != 6);
}

// Librarian
void Librarian::borrowBook(Library &lib) {
    cout << "Librarians cannot borrow books.\n";
}

void Librarian::returnBook(Library &lib) {
    cout << "Librarians do not borrow books.\n";
}

void Librarian::menu(Library &lib) {
    int choice;
    do {
        cout << "\n===== Librarian Menu =====\n";
        cout << "1. Add Book\n2. Remove Book\n3. Update Book\n4. Add User\n5. Remove User\n6. Update User\n7. List Books\n8. List Users\n9. Search Books\n10. Logout\n";
        cout << "Enter your choice: ";
        cin >> choice;
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        switch(choice) {
            case 1: {
                string title, author, publisher, isbn;
                int year;
                cout << "Enter book title: ";
                getline(cin, title);
                cout << "Enter author: ";
                getline(cin, author);
                cout << "Enter publisher: ";
                getline(cin, publisher);
                cout << "Enter publication year: ";
                cin >> year;
                cout << "Enter ISBN: ";
                cin >> isbn;
                Book newBook(title, author, publisher, year, isbn, AVAILABLE);
                lib.addBook(newBook);
                cout << "Book added successfully.\n";
                break;
            }
            case 2: {
                cout << "Enter ISBN of the book to remove: ";
                string isbn;
                cin >> isbn;
                lib.removeBook(isbn);
                break;
            }
            case 3: {
                cout << "Enter ISBN of the book to update: ";
                string isbn;
                cin >> isbn;
                Book* book = lib.findBookByISBN(isbn);
                if(book == nullptr) {
                    cout << "Book not found.\n";
                    break;
                }
                cin.ignore();
                cout << "Enter new title (or press enter to keep \"" << book->getTitle() << "\"): ";
                string newTitle;
                getline(cin, newTitle);
                if(!newTitle.empty())
                    book->setTitle(newTitle);
                cout << "Enter new author (or press enter to keep \"" << book->getAuthor() << "\"): ";
                string newAuthor;
                getline(cin, newAuthor);
                if(!newAuthor.empty())
                    book->setAuthor(newAuthor);
                cout << "Book updated successfully.\n";
                break;
            }
            case 4: {
                cout << "Enter type of user to add (1. Student, 2. Faculty): ";
                int type;
                cin >> type;
                int uid;
                cout << "Enter new user ID: ";
                cin >> uid;
                if(lib.findUserById(uid) != nullptr) {
                    cout << "User with ID " << uid << " already exists. Cannot add duplicate.\n";
                    break;
                }
                cin.ignore();
                cout << "Enter user name: ";
                string uname;
                getline(cin, uname);
                cout << "Enter user password: ";
                string pass;
                getline(cin, pass);
                if(type == 1) {
                    lib.addUser(new Student(uid, uname, pass));
                    cout << "Student added successfully.\n";
                } else if(type == 2) {
                    lib.addUser(new Faculty(uid, uname, pass));
                    cout << "Faculty added successfully.\n";
                } else {
                    cout << "Invalid type.\n";
                }
                break;
            }
            case 5: {
                cout << "Enter user ID to remove: ";
                int uid;
                cin >> uid;
                lib.removeUser(uid);
                break;
            }
            case 6: {
                cout << "Enter user ID to update: ";
                int uid;
                cin >> uid;
                lib.updateUser(uid);
                break;
            }
            case 7: lib.listBooks(); break;
            case 8: lib.listUsers(); break;
            case 9: lib.searchBooks(); break;
            case 10: cout << "Logging out...\n"; break;
            default: cout << "Invalid choice. Please try again.\n";
        }
    } while(choice != 10);
}

// ------------------------
// New User Registration Helper
// ------------------------
void registerNewUser(Library &lib) {
    cout << "\n--- New User Registration ---\n";
    cout << "Select user type:\n1. Student\n2. Faculty\nEnter choice: ";
    int type;
    cin >> type;
    int uid;
    cout << "Enter a new User ID (number): ";
    cin >> uid;
    if(lib.findUserById(uid) != nullptr) {
        cout << "User ID already exists. Registration aborted.\n";
        return;
    }
    cin.ignore();
    cout << "Enter your name: ";
    string uname;
    getline(cin, uname);
    cout << "Enter your password: ";
    string pass;
    getline(cin, pass);
    if(type == 1) {
        lib.addUser(new Student(uid, uname, pass));
        cout << "Student registered successfully!\n";
    } else if(type == 2) {
        lib.addUser(new Faculty(uid, uname, pass));
        cout << "Faculty registered successfully!\n";
    } else {
        cout << "Invalid user type selected.\n";
    }
}

// ------------------------
// Main Function
// ------------------------
int main() {
    Library library;
    library.loadData(); // Attempt to load data from files

    if(library.isBooksEmpty()){
        library.addBook(Book("The C++ Programming Language", "Bjarne Stroustrup", "Addison-Wesley", 2013, "9780321563842"));
        library.addBook(Book("Effective C++", "Scott Meyers", "O'Reilly Media", 2005, "9780321334879"));
        library.addBook(Book("Clean Code", "Robert C. Martin", "Prentice Hall", 2008, "9780132350884"));
        library.addBook(Book("Design Patterns", "Erich Gamma et al.", "Addison-Wesley", 1994, "9780201633610"));
        library.addBook(Book("The Pragmatic Programmer", "Andrew Hunt", "Addison-Wesley", 1999, "9780201616224"));
        library.addBook(Book("Introduction to Algorithms", "Cormen et al.", "MIT Press", 2009, "9780262033848"));
        library.addBook(Book("Head First Design Patterns", "Eric Freeman", "O'Reilly Media", 2004, "9780596007126"));
        library.addBook(Book("C++ Primer", "Stanley B. Lippman", "Addison-Wesley", 2012, "9780321714114"));
        library.addBook(Book("Modern Operating Systems", "Andrew S. Tanenbaum", "Pearson", 2014, "9780133591620"));
        library.addBook(Book("Computer Networks", "Andrew S. Tanenbaum", "Pearson", 2010, "9780132126953"));
    }
    if(library.isUsersEmpty()){
        library.addUser(new Student(101, "Alice", "pass123"));
        library.addUser(new Student(102, "Bob", "pass123"));
        library.addUser(new Student(103, "Charlie", "pass123"));
        library.addUser(new Student(104, "Diana", "pass123"));
        library.addUser(new Student(105, "Evan", "pass123"));
        library.addUser(new Faculty(201, "Prof. Xavier", "pass123"));
        library.addUser(new Faculty(202, "Prof. Yvonne", "pass123"));
        library.addUser(new Faculty(203, "Prof. Zach", "pass123"));
        library.addUser(new Librarian(301, "Librarian Linda", "libpass"));
    }

    int mainChoice;
    do {
        cout << R"(            __   ____ ____ ____   __   ____ _  _    __  __   __   _  _   __   ___ ____ __  __ ____ _  _ ____    ___ _  _ ___ ____ ____ __  __ 
(  ) (_  _(  _ (  _ \ /__\ (  _ ( \/ )  (  \/  ) /__\ ( \( ) /__\ / __( ___(  \/  ( ___( \( (_  _)  / __( \/ / __(_  _( ___(  \/  )
 )(__ _)(_ ) _ <)   //(__)\ )   /\  /    )    ( /(__)\ )  ( /(__)( (_-.)__) )    ( )__) )  (  )(    \__ \\  /\__ \ )(  )__) )    ( 
(____(____(____(_)\_(__)(__(_)\_)(__)   (_/\/\_(__)(__(_)\_(__)(__\___(____(_/\/\_(____(_)\_)(__)   (___/(__)(___/(__)(____(_/\/\_)
              )" << "\n";
          
        cout << "\n===== Main Menu =====\n";
        cout << "1. Login\n2. Register New User\n3. List All Books\n4. Help/Instructions\n5. Exit\n";
        cout << "Enter your choice: ";
        cin >> mainChoice;
        switch(mainChoice) {
            case 1: {
                cout << "Enter your User ID: ";
                int uid;
                cin >> uid;
                User* user = library.findUserById(uid);
                if(user) {
                    cout << "Enter password: ";
                    string pass;
                    cin >> pass;
                    if(!user->checkPassword(pass)) {
                        cout << "Invalid password. Returning to main menu.\n";
                        break;
                    }
                    cout << "Welcome, " << user->getName() << "!\n";
                    user->menu(library);
                } else {
                    char reg;
                    cout << "User ID not found. Are you new here and want to register? (Y/N): ";
                    cin >> reg;
                    if(reg == 'Y' || reg == 'y') {
                        registerNewUser(library);
                    } else {
                        cout << "Returning to main menu.\n";
                    }
                }
                break;
            }
            case 2:
                registerNewUser(library);
                break;
            case 3:
                library.listBooks();
                break;
            case 4:
                cout << "\n--- Help / Instructions ---\n";
                cout << "1. Login with your user ID and password. If you are new, choose to register.\n";
                cout << "2. Students and Faculty can borrow/return books. When borrowing, the current day is automatically recorded and a due date is set (15 days for students, 30 days for faculty).\n";
                cout << "3. When returning, the system automatically calculates overdue fines (10 rupees per day for students).\n";
                cout << "4. All account details (including borrowed books with dates and history) are saved in users.txt.\n";
                cout << "5. Librarians have extended privileges to manage books and users.\n";
                cout << "----------------------------\n";
                break;
            case 5:
                cout << "Exiting the system. Goodbye!\n";
                break;
            default:
                cout << "Invalid choice. Please try again.\n";
        }
    } while(mainChoice != 5);

    library.saveData();
    return 0;
}
