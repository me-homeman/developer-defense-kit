#include <iostream>
#include <string>
#include <vector>
#include <stdexcept>
#include <memory>
#include <cstdio> // For remove()

#include <sqlite3.h>

// --- Custom Exception for SQLite Errors ---
class SQLiteException : public std::runtime_error {
public:
    SQLiteException(const std::string& message, sqlite3* db = nullptr)
        : std::runtime_error(message + (db ? ": " + std::string(sqlite3_errmsg(db)) : "")) {}
};

// --- RAII Wrapper for sqlite3 connection ---
class Database {
private:
    sqlite3* db;

public:
    Database(const std::string& db_name) : db(nullptr) {
        if (sqlite3_open(db_name.c_str(), &db) != SQLITE_OK) {
            throw SQLiteException("Failed to open database", db);
        }
        std::cout << "Database connection opened to '" << db_name << "'." << std::endl;
    }

    ~Database() {
        if (db) {
            sqlite3_close(db);
            std::cout << "Database connection closed." << std::endl;
        }
    }

    // Accessor for the raw pointer
    sqlite3* get() const {
        return db;
    }

    // Disable copy and assign
    Database(const Database&) = delete;
    Database& operator=(const Database&) = delete;
};

// --- RAII Wrapper for sqlite3_stmt ---
class Statement {
private:
    sqlite3_stmt* stmt;
    sqlite3* db; // For error messages

public:
    Statement(const Database& db_obj, const std::string& sql) : stmt(nullptr), db(db_obj.get()) {
        if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
            throw SQLiteException("Failed to prepare statement for SQL: " + sql, db);
        }
    }

    ~Statement() {
        if (stmt) {
            sqlite3_finalize(stmt);
        }
    }
    
    void bind(int index, int value) {
        if (sqlite3_bind_int(stmt, index, value) != SQLITE_OK) {
            throw SQLiteException("Failed to bind int", db);
        }
    }
    
    void bind(int index, const std::string& value) {
        if (sqlite3_bind_text(stmt, index, value.c_str(), -1, SQLITE_STATIC) != SQLITE_OK) {
            throw SQLiteException("Failed to bind text", db);
        }
    }
    
    int step() {
        return sqlite3_step(stmt);
    }
    
    void reset() {
        if (sqlite3_reset(stmt) != SQLITE_OK) {
            throw SQLiteException("Failed to reset statement", db);
        }
    }
    
    int column_int(int col) {
        return sqlite3_column_int(stmt, col);
    }

    std::string column_text(int col) {
        return std::string(reinterpret_cast<const char*>(sqlite3_column_text(stmt, col)));
    }


    // Disable copy and assign
    Statement(const Statement&) = delete;
    Statement& operator=(const Statement&) = delete;
};

// --- Function Prototypes ---
void execute_sql(Database& db, const std::string& sql);
void setup_database(Database& db);
void demonstrate_safe_fetching(Database& db);
void demonstrate_injection_prevention(Database& db);


// --- Main Execution ---
int main() {
    const std::string db_file = "production_cpp_example.db";

    try {
        // The Database object's lifetime is managed by RAII
        Database db(db_file);

        setup_database(db);

        std::cout << "\n--- DEMONSTRATING SAFE AND CONVENIENT FETCHING ---" << std::endl;
        demonstrate_safe_fetching(db);

        std::cout << "\n--- DEMONSTRATING SQL INJECTION PREVENTION ---" << std::endl;
        demonstrate_injection_prevention(db);

    } catch (const SQLiteException& e) {
        std::cerr << "A database error occurred: " << e.what() << std::endl;
        // Cleanup will happen automatically via destructors
        remove(db_file.c_str());
        return 1;
    } catch (const std::exception& e) {
        std::cerr << "An unexpected error occurred: " << e.what() << std::endl;
        remove(db_file.c_str());
        return 1;
    }
    
    // Clean up the database file
    if (remove(db_file.c_str()) == 0) {
        std::cout << "Cleaned up " << db_file << "." << std::endl;
    } else {
        std::cerr << "Error cleaning up " << db_file << "." << std::endl;
    }

    return 0;
}


// --- Helper Functions ---

void execute_sql(Database& db, const std::string& sql) {
    char* err_msg = nullptr;
    if (sqlite3_exec(db.get(), sql.c_str(), 0, 0, &err_msg) != SQLITE_OK) {
        std::string err_str = err_msg;
        sqlite3_free(err_msg);
        throw SQLiteException("Failed to execute SQL: " + sql + ". Error: " + err_str);
    }
}

void setup_database(Database& db) {
    std::cout << "Setting up the 'users' table..." << std::endl;

    execute_sql(db, "DROP TABLE IF EXISTS users");
    execute_sql(db, 
        "CREATE TABLE users("
        "id INTEGER PRIMARY KEY, "
        "username TEXT NOT NULL UNIQUE, "
        "email TEXT);"
    );
    
    std::vector<std::tuple<int, std::string, std::string>> users_to_add = {
        {1, "alice", "alice@example.com"},
        {2, "bob", "bob@example.com"},
        {3, "charlie", "charlie@example.com"}
    };
    
    Statement stmt(db, "INSERT INTO users (id, username, email) VALUES (?, ?, ?);");

    for(const auto& user : users_to_add) {
        stmt.bind(1, std::get<0>(user));
        stmt.bind(2, std::get<1>(user));
        stmt.bind(3, std::get<2>(user));
        
        if (stmt.step() != SQLITE_DONE) {
            throw SQLiteException("Failed to insert user", db.get());
        }
        stmt.reset();
    }

    std::cout << "Database setup complete. Users inserted." << std::endl;
}

void demonstrate_safe_fetching(Database& db) {
    Statement stmt(db, "SELECT id, username FROM users ORDER BY id");
    
    std::cout << "[GOOD] Fetched all users:" << std::endl;
    while (stmt.step() == SQLITE_ROW) {
        std::cout << "  ID: " << stmt.column_int(0) << ", Username: " << stmt.column_text(1) << std::endl;
    }

    // GOOD: Fetch one specific user
    int user_id = 2;
    Statement stmt_one(db, "SELECT * FROM users WHERE id = ?");
    stmt_one.bind(1, user_id);

    if (stmt_one.step() == SQLITE_ROW) {
        std::cout << "[GOOD] Fetched user with ID " << user_id << ": "
                  << "  ID: " << stmt_one.column_int(0) 
                  << ", Username: " << stmt_one.column_text(1)
                  << ", Email: " << stmt_one.column_text(2) << std::endl;
    }
}

void demonstrate_injection_prevention(Database& db) {
    std::string malicious_input = "2; DROP TABLE users; --";
    Statement stmt(db, "SELECT * FROM users WHERE id = ?");
    
    // The malicious input is bound as a single string parameter, not executed as SQL
    stmt.bind(1, malicious_input);

    std::cout << "[SAFE] Tried to fetch user with malicious ID. Result:" << std::endl;
    if (stmt.step() == SQLITE_ROW) {
        std::cout << "  Malicious query returned a row (it shouldn't have!): "
                  << "  ID: " << stmt.column_int(0) << ", Name: " << stmt.column_text(1) << std::endl;
    } else {
        std::cout << "  No user found with ID '" << malicious_input << "', as expected." << std::endl;
    }
    
    // Verify that the users table still exists and is unharmed
    Statement verify_stmt(db, "SELECT username FROM users");
    std::cout << "[SAFE] Data is unharmed. Users remaining:" << std::endl;
    while (verify_stmt.step() == SQLITE_ROW) {
        std::cout << "  - " << verify_stmt.column_text(0) << std::endl;
    }
}
