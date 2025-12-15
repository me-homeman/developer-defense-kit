#!/usr/bin/env python3

import sqlite3
import re
import os
import logging
from typing import Any, Iterable, Optional, List

# --- Setup basic logging ---
logging.basicConfig(level=logging.INFO, format='%(asctime)s - %(levelname)s - %(message)s')

class SafeDBWrapper:
    """
    A production-ready database wrapper that prevents SQL injection, provides
    context management for connections, and offers granular transaction control.

    This class enforces parameterized queries, the standard defense against
    SQL injection, by separating SQL logic from data.
    """

    def __init__(self, db_name: str):
        """Initializes the wrapper but does not open the connection yet."""
        self.db_name = db_name
        self.connection: Optional[sqlite3.Connection] = None
        self.cursor: Optional[sqlite3.Cursor] = None

    def __enter__(self):
        """
        Opens the database connection and returns the wrapper instance.
        Enables use of the `with` statement for automatic connection handling.
        """
        try:
            self.connection = sqlite3.connect(self.db_name)
            # Set row_factory to sqlite3.Row to get dict-like rows
            self.connection.row_factory = sqlite3.Row
            self.cursor = self.connection.cursor()
            logging.info(f"Database connection opened to '{self.db_name}'.")
            return self
        except sqlite3.Error as e:
            logging.error(f"Failed to connect to database: {e}")
            raise

    def __exit__(self, exc_type, exc_val, exc_tb):
        """
        Closes the database connection. If an exception occurred within the
        `with` block, it rolls back the transaction. Otherwise, it commits.
        """
        if self.connection:
            if exc_type:
                logging.warning(f"Exception occurred, rolling back transaction. Details: {exc_val}")
                self.connection.rollback()
            else:
                logging.info("Committing transaction.")
                self.connection.commit()
            self.connection.close()
            logging.info(f"Database connection to '{self.db_name}' closed.")

    def _pre_execution_check(self, query: str):
        """
        Performs a basic check for obviously unsafe query patterns.
        This is a secondary defense; the primary defense is parameterization.
        """
        # This regex is a simple heuristic to catch common mistakes.
        unsafe_pattern = re.compile(r"""\bWHERE\b\s+\w+\s*=\s*['"].*['"]""", re.IGNORECASE)
        if unsafe_pattern.search(query):
            raise ValueError(
                "Potential SQL Injection risk detected. Do not use string "
                "formatting or concatenation for values in a WHERE clause. "
                "Use '?' placeholders and the 'params' argument instead."
            )

    def execute(self, query: str, params: Iterable[Any] = ()):
        """
        Executes a SQL query safely using parameter substitution.
        Does NOT commit automatically, allowing for multiple operations in one transaction.

        Args:
            query (str): The SQL query with '?' placeholders.
            params (Iterable, optional): A tuple or list of values.

        Returns:
            sqlite3.Cursor: The cursor object for manual fetching if needed.
        """
        if not self.cursor:
            raise sqlite3.Error("Cursor is not available. Is the connection open?")
        self._pre_execution_check(query)
        logging.info(f"Executing query: {query}")
        try:
            return self.cursor.execute(query, params)
        except sqlite3.Error as e:
            logging.error(f"Database error on execute: {e}")
            raise

    def fetch_one(self) -> Optional[sqlite3.Row]:
        """Fetches a single result from the last executed query."""
        if not self.cursor:
            raise sqlite3.Error("Cursor is not available.")
        return self.cursor.fetchone()

    def fetch_all(self) -> List[sqlite3.Row]:
        """Fetches all results from the last executed query."""
        if not self.cursor:
            raise sqlite3.Error("Cursor is not available.")
        return self.cursor.fetchall()

    def execute_and_fetch_all(self, query: str, params: Iterable[Any] = ()) -> List[sqlite3.Row]:
        """A convenience method to execute a query and fetch all results."""
        self.execute(query, params)
        return self.fetch_all()

    def execute_and_fetch_one(self, query: str, params: Iterable[Any] = ()) -> Optional[sqlite3.Row]:
        """A convenience method to execute a query and fetch a single result."""
        self.execute(query, params)
        return self.fetch_one()


# --- New and Improved Usage Example ---

def setup_database(db: SafeDBWrapper):
    """A helper to create a fresh table for the example."""
    logging.info("Setting up the 'users' table...")
    # The `with` block handles commits/rollbacks, so no need for manual calls here.
    db.execute("DROP TABLE IF EXISTS users")
    db.execute("""
        CREATE TABLE users (
            id INTEGER PRIMARY KEY,
            username TEXT NOT NULL UNIQUE,
            email TEXT
        )
    """)
    # Insert multiple records in a single transaction
    users_to_add = [
        (1, 'alice', 'alice@example.com'),
        (2, 'bob', 'bob@example.com'),
        (3, 'charlie', 'charlie@example.com'),
    ]
    for user in users_to_add:
        db.execute("INSERT INTO users VALUES (?, ?, ?)", user)
    logging.info("Database setup complete. Users inserted.")


if __name__ == "__main__":
    DB_FILE = "production_example.db"

    # Using the context manager for clean, safe handling of the connection
    try:
        with SafeDBWrapper(DB_FILE) as db:
            setup_database(db)

        print("\n--- DEMONSTRATING SAFE AND CONVENIENT FETCHING ---")
        with SafeDBWrapper(DB_FILE) as db:
            # GOOD: Fetch all users using a helper
            all_users = db.execute_and_fetch_all("SELECT id, username FROM users ORDER BY id")
            print(f"[GOOD] Fetched all users: {[dict(u) for u in all_users]}")

            # GOOD: Fetch one specific user
            user_id = 2
            user = db.execute_and_fetch_one("SELECT * FROM users WHERE id = ?", (user_id,))
            if user:
                print(f"[GOOD] Fetched user with ID {user_id}: {dict(user)}")

        print("\n--- DEMONSTRATING SQL INJECTION PREVENTION ---")
        with SafeDBWrapper(DB_FILE) as db:
            # Attacker tries to inject a payload to delete data
            malicious_input = "2; DROP TABLE users; --"

            # The parameterized query treats the input as a literal string.
            # No user has the ID "2; DROP TABLE users; --", so nothing is found.
            # The DROP TABLE command is NEVER executed.
            result = db.execute_and_fetch_one("SELECT * FROM users WHERE id = ?", (malicious_input,))
            print(f"[SAFE] Tried to fetch user with malicious ID. Result: {result}")

            # Verify that the users table still exists and is unharmed
            remaining_users = db.execute_and_fetch_all("SELECT username FROM users")
            print(f"[SAFE] Data is unharmed. Users remaining: {[u['username'] for u in remaining_users]}")

    except Exception as e:
        logging.error(f"An unexpected error occurred in main execution: {e}")
    finally:
        # Clean up the database file
        if os.path.exists(DB_FILE):
            os.remove(DB_FILE)
            logging.info(f"Cleaned up {DB_FILE}.")

