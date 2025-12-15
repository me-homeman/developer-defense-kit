#include <stdio.h>
#include <stdlib.h>
#include <sqlite3.h>
#include <string.h>

// --- Function Prototypes ---
void setup_database(sqlite3 *db);
void demonstrate_safe_fetching(sqlite3 *db);
void demonstrate_injection_prevention(sqlite3 *db);
void execute_sql(sqlite3 *db, const char *sql);
void check_error(int rc, char *err_msg, sqlite3 *db);

// --- Main Execution ---
int main(int argc, char* argv[]) {
    sqlite3 *db;
    char *err_msg = 0;
    const char *db_file = "production_c_example.db";

    // Open the database
    int rc = sqlite3_open(db_file, &db);

    if (rc != SQLITE_OK) {
        fprintf(stderr, "Cannot open database: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        return 1;
    }
    printf("Database connection opened to '%s'.\n", db_file);

    setup_database(db);

    printf("\n--- DEMONSTRATING SAFE AND CONVENIENT FETCHING ---\n");
    demonstrate_safe_fetching(db);

    printf("\n--- DEMONSTRATING SQL INJECTION PREVENTION ---\n");
    demonstrate_injection_prevention(db);

    // Close the database
    sqlite3_close(db);
    printf("Database connection to '%s' closed.\n", db_file);
    
    // Clean up the database file
    if (remove(db_file) == 0) {
        printf("Cleaned up %s.\n", db_file);
    } else {
        fprintf(stderr, "Error cleaning up %s.\n", db_file);
    }

    return 0;
}

// --- Helper Functions ---

void check_error(int rc, char *err_msg, sqlite3 *db) {
    if (rc != SQLITE_OK) {
        fprintf(stderr, "SQL error: %s\n", err_msg);
        sqlite3_free(err_msg);
        sqlite3_close(db);
        exit(1);
    }
}

void execute_sql(sqlite3 *db, const char *sql) {
    char *err_msg = 0;
    int rc = sqlite3_exec(db, sql, 0, 0, &err_msg);
    check_error(rc, err_msg, db);
}

void setup_database(sqlite3 *db) {
    printf("Setting up the 'users' table...\n");

    execute_sql(db, "DROP TABLE IF EXISTS users");
    execute_sql(db, 
        "CREATE TABLE users("
        "id INTEGER PRIMARY KEY, "
        "username TEXT NOT NULL UNIQUE, "
        "email TEXT);"
    );
    
    const char *users_to_add[][3] = {
        {"1", "alice", "alice@example.com"},
        {"2", "bob", "bob@example.com"},
        {"3", "charlie", "charlie@example.com"}
    };

    sqlite3_stmt *stmt;
    const char *sql = "INSERT INTO users (id, username, email) VALUES (?, ?, ?);";
    
    int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, 0);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Failed to prepare statement: %s\n", sqlite3_errmsg(db));
        return;
    }

    for (int i = 0; i < 3; ++i) {
        sqlite3_bind_int(stmt, 1, atoi(users_to_add[i][0]));
        sqlite3_bind_text(stmt, 2, users_to_add[i][1], -1, SQLITE_STATIC);
        sqlite3_bind_text(stmt, 3, users_to_add[i][2], -1, SQLITE_STATIC);
        
        if (sqlite3_step(stmt) != SQLITE_DONE) {
            fprintf(stderr, "Failed to insert user %s: %s\n", users_to_add[i][1], sqlite3_errmsg(db));
        }
        sqlite3_reset(stmt);
    }

    sqlite3_finalize(stmt);
    printf("Database setup complete. Users inserted.\n");
}

void demonstrate_safe_fetching(sqlite3 *db) {
    sqlite3_stmt *stmt;
    const char *sql = "SELECT id, username FROM users ORDER BY id";

    int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, 0);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Failed to prepare statement: %s\n", sqlite3_errmsg(db));
        return;
    }

    printf("[GOOD] Fetched all users:\n");
    while ((rc = sqlite3_step(stmt)) == SQLITE_ROW) {
        int id = sqlite3_column_int(stmt, 0);
        const unsigned char *username = sqlite3_column_text(stmt, 1);
        printf("  ID: %d, Username: %s\n", id, username);
    }
    sqlite3_finalize(stmt);

    // GOOD: Fetch one specific user
    int user_id = 2;
    sql = "SELECT * FROM users WHERE id = ?";
    rc = sqlite3_prepare_v2(db, sql, -1, &stmt, 0);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Failed to prepare statement: %s\n", sqlite3_errmsg(db));
        return;
    }
    sqlite3_bind_int(stmt, 1, user_id);

    if ((rc = sqlite3_step(stmt)) == SQLITE_ROW) {
        printf("[GOOD] Fetched user with ID %d: ID: %d, Username: %s, Email: %s\n", 
            user_id,
            sqlite3_column_int(stmt, 0),
            sqlite3_column_text(stmt, 1),
            sqlite3_column_text(stmt, 2)
        );
    }
    sqlite3_finalize(stmt);
}

void demonstrate_injection_prevention(sqlite3 *db) {
    sqlite3_stmt *stmt;
    const char *malicious_input = "2; DROP TABLE users; --";
    const char *sql = "SELECT * FROM users WHERE id = ?";

    int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, 0);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Failed to prepare statement: %s\n", sqlite3_errmsg(db));
        return;
    }
    
    // The malicious input is bound as a single string parameter, not executed as SQL
    sqlite3_bind_text(stmt, 1, malicious_input, -1, SQLITE_STATIC);
    
    printf("[SAFE] Tried to fetch user with malicious ID. Result:\n");
    if ((rc = sqlite3_step(stmt)) == SQLITE_ROW) {
         printf("  Malicious query returned a row (it shouldn\'t have!): ID: %d, Name: %s\n", sqlite3_column_int(stmt, 0), sqlite3_column_text(stmt, 1));
    } else {
        printf("  No user found with ID '%s', as expected.\n", malicious_input);
    }
    sqlite3_finalize(stmt);

    // Verify that the users table still exists and is unharmed
    sql = "SELECT username FROM users";
    rc = sqlite3_prepare_v2(db, sql, -1, &stmt, 0);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Failed to prepare statement: %s\n", sqlite3_errmsg(db));
        return;
    }

    printf("[SAFE] Data is unharmed. Users remaining:\n");
    while ((rc = sqlite3_step(stmt)) == SQLITE_ROW) {
        const unsigned char *username = sqlite3_column_text(stmt, 0);
        printf("  - %s\n", username);
    }
    sqlite3_finalize(stmt);
}
