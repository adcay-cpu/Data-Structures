#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <limits>
#include <memory>
#include <utility>

using namespace std;

// This is the class for a single book! 📚
class Book {
private:
    string a, b, c; // a=title, b=author, c=isbn
    bool d;         // d=is_borrowed

public:
    // This is how we create a new book!
    Book(string i, string j, string k, bool l = false)
        : a(std::move(i)), b(std::move(j)), c(std::move(k)), d(l) {}

    // Functions to get info about the book
    string getTitle() const { return a; }
    string getAuthor() const { return b; }
    string getISBN() const { return c; }
    bool getStatus() const { return d; }

    // This lets us change the book's status (borrowed or not)
    void setStatus(bool l) { d = l; }

    void display() const {
        cout << "Title: " << a << endl;
        cout << "Author: " << b << endl;
        cout << "ISBN: " << c << endl;
        cout << "Status: " << (d ? "Borrowed" : "Available") << endl;
    }
};

// This is the class for a user (like a student!) 👩‍🎓
class User {
private:
    int x;             // x=user_ID
    string y;          // y=user_name
    vector<string> z;  // z=borrowed_books

public:
    // This is how we make a new user!
    User(int i, string j) : x(i), y(std::move(j)) {}

    // Functions to get user info
    int getID() const { return x; }
    string getName() const { return y; }
    const vector<string>& getBorrowedBooks() const { return z; }

    // Functions for borrowing and returning
    void addBook(const string& i) { z.push_back(i); }
    void removeBook(const string& i) {
        for (size_t k = 0; k < z.size(); ++k) {
            if (z[k] == i) {
                z.erase(z.begin() + k);
                return;
            }
        }
    }

    void display() const {
        cout << "User ID: " << x << endl;
        cout << "Name: " << y << endl;
    }

    void displayBorrowedBooks(const vector<shared_ptr<Book>>& books) const {
        cout << "Borrowed Books for " << y << ":" << endl;
        if (z.empty()) {
            cout << "  - None" << endl;
            return;
        }
        for (const auto& i : z) {
            for (const auto& j : books) {
                if (j->getISBN() == i) {
                    cout << "  - " << j->getTitle() << " (ISBN: " << i << ")" << endl;
                    break;
                }
            }
        }
    }
};

// This is the main class for our Library! 🏫
class Library {
private:
    vector<shared_ptr<Book>> b;  // b=books_list
    vector<shared_ptr<User>> u;  // u=users_list
    const string bf = "my_books.txt";
    const string uf = "my_users.txt";

public:
    // The library loads all the info when it starts
    Library() {
        loadBooks();
        loadUsers();
    }

    // Functions to save and load data
    void saveBooks() const {
        ofstream o(bf);
        if (!o.is_open()) {
            cerr << "Uh-oh, can't open the file to save books: " << bf << endl;
            return;
        }
        for (const auto& l : b) {
            o << l->getTitle() << "|" << l->getAuthor() << "|" << l->getISBN() << "|" << (l->getStatus() ? "1" : "0") << endl;
        }
    }

    void loadBooks() {
        ifstream i(bf);
        if (!i.is_open()) {
            cout << "No books file found! Let's make some starting books for you." << endl;
            b.push_back(make_shared<Book>("The Legend of Kings", "Antoine de Saint-Exupéry", "978-0156013925"));
            b.push_back(make_shared<Book>("Ang Pagong at Unggoy", "Jose Rizal", "978-9715102500"));
            b.push_back(make_shared<Book>("Ang pinagpalang kagwapohan ni Rey Sarza", "Adrian Cay", "978-9715102517"));
            b.push_back(make_shared<Book>("Rey the Great", "Ako & Me", "978-0140367373"));
            b.push_back(make_shared<Book>("Daig kayo ng Lola ko","Secret haha","911-0123456"));
			saveBooks();
            return;
        }
        string l;
        while (getline(i, l)) {
            string t, a, n, s;
            size_t p = 0;
            size_t pp = 0;

            p = l.find('|');
            t = l.substr(pp, p - pp);
            pp = p + 1;

            p = l.find('|', pp);
            a = l.substr(pp, p - pp);
            pp = p + 1;

            p = l.find('|', pp);
            n = l.substr(pp, p - pp);
            s = l.substr(p + 1);
            
            b.push_back(make_shared<Book>(t, a, n, s == "1"));
        }
        cout << "Loaded books from " << bf << endl;
    }

    void saveUsers() const {
        ofstream o(uf);
        if (!o.is_open()) {
            cerr << "Uh-oh, can't open the file to save users: " << uf << endl;
            return;
        }
        for (const auto& l : u) {
            o << l->getID() << "|" << l->getName();
            for (const auto& i : l->getBorrowedBooks()) {
                o << "|" << i;
            }
            o << endl;
        }
    }

    void loadUsers() {
        ifstream i(uf);
        if (!i.is_open()) {
            cout << "No users file found! Let's make some starting users for you." << endl;
            u.push_back(make_shared<User>(101, "Adrian"));
            u.push_back(make_shared<User>(102, "Rey"));
            u.push_back(make_shared<User>(103, "Justine"));
            saveUsers();
            return;
        }
        string l;
        while (getline(i, l)) {
            size_t p = 0;
            size_t pp = 0;

            p = l.find('|');
            int id = stoi(l.substr(pp, p - pp));
            pp = p + 1;

            p = l.find('|', pp);
            string n = l.substr(pp, p - pp);
            auto nu = make_shared<User>(id, n);
            pp = p + 1;

            while (pp < l.length()) {
                p = l.find('|', pp);
                if (p == string::npos) {
                    nu->addBook(l.substr(pp));
                    break;
                }
                string bi = l.substr(pp, p - pp);
                nu->addBook(bi);
                pp = p + 1;
            }
            u.push_back(nu);
        }
        cout << "Loaded users from " << uf << endl;
    }

    // Helper functions (these find a book or a user)
    shared_ptr<Book> findBookByISBN(const string& i) const {
        for (const auto& b : b) {
            if (b->getISBN() == i) {
                return b;
            }
        }
        return nullptr;
    }

    shared_ptr<User> findUserByID(int i) const {
        for (const auto& u : u) {
            if (u->getID() == i) {
                return u;
            }
        }
        return nullptr;
    }

    // Book Management functions
    void addBook(const string& t, const string& a, const string& i) {
        if (findBookByISBN(i)) {
            cout << "Oops! A book with that ISBN already exists. Can't add it again." << endl;
            return;
        }
        b.push_back(make_shared<Book>(t, a, i));
        saveBooks();
        cout << "Yay! The book was added successfully." << endl;
    }

    void removeBook(const string& i) {
        for (auto it = b.begin(); it != b.end(); ++it) {
            if ((*it)->getISBN() == i) {
                if ((*it)->getStatus()) {
                    cout << "Whoops, this book is borrowed! You can't remove it yet." << endl;
                    return;
                }
                b.erase(it);
                saveBooks();
                cout << "Book removed successfully! Bye-bye, book." << endl;
                return;
            }
        }
        cout << "Oh no! I couldn't find a book with that ISBN." << endl;
    }

    void displayAllBooks() const {
        cout << "\n--------------->> All the Books! <<--------------" << endl;
        if (b.empty()) {
            cout << "No books in the library. How sad! 😢" << endl;
            return;
        }
        for (const auto& l : b) {
            l->display();
            cout << "<<------------------------------------>>" << endl;
        }
    }

    // User Management functions
    void registerUser(int i, const string& n) {
        if (findUserByID(i)) {
            cout << "Oops! A user with that ID is already here." << endl;
            return;
        }
        u.push_back(make_shared<User>(i, n));
        saveUsers();
        cout << "Hello, " << n << "! You're all signed up." << endl;
    }

    void removeUser(int i) {
        for (auto it = u.begin(); it != u.end(); ++it) {
            if ((*it)->getID() == i) {
                if (!(*it)->getBorrowedBooks().empty()) {
                    cout << "Wait! This user still has books they need to return. Can't remove them yet." << endl;
                    return;
                }
                u.erase(it);
                saveUsers();
                cout << "User removed successfully. Goodbye!" << endl;
                return;
            }
        }
        cout << "Oh no! I couldn't find a user with that ID." << endl;
    }

    void displayAllUsers() const {
        cout << "\n---------->> All the Users! <<----------" << endl;
        if (u.empty()) {
            cout << "No users are registered. Let's get some new friends to join!" << endl;
            return;
        }
        for (const auto& l : u) {
            l->display();
            l->displayBorrowedBooks(b);
            cout << "<<-------------------->>" << endl;
        }
    }

    // Borrowing and Returning functions
    void borrowBook(int ui, const string& bi) {
        auto user = findUserByID(ui);
        auto book = findBookByISBN(bi);

        if (!user) {
            cout << "I couldn't find that user. Is the ID correct?" << endl;
            return;
        }
        if (!book) {
            cout << "I couldn't find that book. Is the ISBN correct?" << endl;
            return;
        }
        if (book->getStatus()) {
            cout << "Oh dear, this book is already borrowed! Maybe pick another one? 🤷" << endl;
            return;
        }

        user->addBook(bi);
        book->setStatus(true);
        saveUsers();
        saveBooks();
        cout << "Book borrowed successfully! Happy reading!" << endl;
    }

    void returnBook(int ui, const string& bi) {
        auto user = findUserByID(ui);
        auto book = findBookByISBN(bi);

        if (!user) {
            cout << "I couldn't find that user. Is the ID correct?" << endl;
            return;
        }
        if (!book) {
            cout << "I couldn't find that book. Is the ISBN correct?" << endl;
            return;
        }

        bool hasBorrowed = false;
        for (const auto& i : user->getBorrowedBooks()) {
            if (i == bi) {
                hasBorrowed = true;
                break;
            }
        }
        if (!hasBorrowed) {
            cout << "This user didn't even borrow that book! 🤨" << endl;
            return;
        }

        user->removeBook(bi);
        book->setStatus(false);
        saveUsers();
        saveBooks();
        cout << "Thanks for returning the book! It's now available for others." << endl;
    }

    // Display Menus
    void displayMainMenu() const {
        cout << "\n-----------------------------------" << endl;
        cout << " >>>>>>>>>>>  MY LIBRARY  <<<<<<<<<";
        cout << "\n-----------------------------------" << endl;
        cout << "1. Book Management" << endl;
        cout << "2. User Management" << endl;
        cout << "3. Borrow/Return Books" << endl;
        cout << "4. Exit" << endl;
        cout << "<<-------------------------------->>" << endl;
        cout << "What do you want to do? (Enter a number): ";
    }

    void displayBookMenu() const {
        cout << "\n---------->> Book Management <<----------" << endl;
        cout << "1. Add a New Book" << endl;
        cout << "2. Remove a Book" << endl;
        cout << "3. Show All Books" << endl;
        cout << "4. Go Back to Main Menu" << endl;
        cout << "<<-------------------------------->>" << endl;
        cout << "Enter your choice: ";
    }

    void displayUserMenu() const {
        cout << "\n---------->> User Management <<----------" << endl;
        cout << "1. Register a New User" << endl;
        cout << "2. Remove a User" << endl;
        cout << "3. Show All Users" << endl;
        cout << "4. Go Back to Main Menu" << endl;
        cout << "<<-------------------------------->>" << endl;
        cout << "Enter your choice: ";
    }

    void displayBorrowReturnMenu() const {
        cout << "\n---------->> Borrow/Return <<----------" << endl;
        cout << "1. Borrow a Book" << endl;
        cout << "2. Return a Book" << endl;
        cout << "3. Go Back to Main Menu" << endl;
        cout << "<<------------------------------>>" << endl;
        cout << "Enter your choice: ";
    }
};

// This function fixes the input problem so the program doesn't get stuck. 👍
void clearInputBuffer() {
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
}

int main() {
    Library l;
    int c;
    string t, a, i, n;
    int u;

    while (true) {
        l.displayMainMenu();
        cin >> c;
        clearInputBuffer(); // Always do this after reading a number!

        switch (c) {
            case 1: { // This is for books
                int bc;
                while (true) {
                    l.displayBookMenu();
                    cin >> bc;
                    clearInputBuffer(); // Do it here too!
                    if (bc == 4) break;

                    switch (bc) {
                        case 1:
                            cout << "Enter book title: ";
                            getline(cin, t);
                            cout << "Enter author: ";
                            getline(cin, a);
                            cout << "Enter ISBN: ";
                            getline(cin, i);
                            l.addBook(t, a, i);
                            break;
                        case 2:
                            cout << "Enter ISBN of book to remove: ";
                            getline(cin, i);
                            l.removeBook(i);
                            break;
                        case 3:
                            l.displayAllBooks();
                            break;
                        default:
                            cout << "Invalid choice. Please try again." << endl;
                    }
                }
                break;
            }
            case 2: { // This is for users
                int uc;
                while (true) {
                    l.displayUserMenu();
                    cin >> uc;
                    clearInputBuffer(); // And here!
                    if (uc == 4) break;

                    switch (uc) {
                        case 1:
                            cout << "Enter user name: ";
                            getline(cin, n);
                            cout << "Enter user ID: ";
                            cin >> u;
                            clearInputBuffer(); // And after every 'cin' for a number
                            l.registerUser(u, n);
                            break;
                        case 2:
                            cout << "Enter user ID to remove: ";
                            cin >> u;
                            clearInputBuffer(); // You get the idea!
                            l.removeUser(u);
                            break;
                        case 3:
                            l.displayAllUsers();
                            break;
                        default:
                            cout << "Invalid choice. Please try again." << endl;
                    }
                }
                break;
            }
            case 3: { // This is for borrowing and returning
                int brc;
                while (true) {
                    l.displayBorrowReturnMenu();
                    cin >> brc;
                    clearInputBuffer(); // Don't forget this!
                    if (brc == 3) break;

                    switch (brc) {
                        case 1:
                            cout << "Enter user ID: ";
                            cin >> u;
                            clearInputBuffer(); // A must-do
                            cout << "Enter book ISBN to borrow: ";
                            getline(cin, i);
                            l.borrowBook(u, i);
                            break;
                        case 2:
                            cout << "Enter user ID: ";
                            cin >> u;
                            clearInputBuffer(); // One more time!
                            cout << "Enter book ISBN to return: ";
                            getline(cin, i);
                            l.returnBook(u, i);
                            break;
                        default:
                            cout << "Invalid choice. Please try again." << endl;
                    }
                }
                break;
            }
            case 4:
                cout << "Exiting program. Goodbye! Hope you read some great books!" << endl;
                return 0;
            default:
                cout << "Invalid choice. Please try again." << endl;
        }
    }
    return 0;
}