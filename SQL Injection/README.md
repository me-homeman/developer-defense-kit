# Secure Python Database Wrapper

A production-ready Python class `SafeDBWrapper` for `sqlite3` that provides a secure, convenient, and robust interface for database interactions. Its primary design goal is to eliminate SQL injection vulnerabilities while offering a developer-friendly API.

This script is a self-contained example of security best practices for database-driven applications in Python.

## Core Features

- **SQL Injection Prevention**: Exclusively uses parameterized queries, the industry-standard defense against SQL injection.
- **Context Management**: Utilizes `with` statements to ensure database connections are automatically and safely managed (opened, committed/rolled back, and closed).
- **Granular Transaction Control**: Operations within a single `with` block are treated as one atomic transaction.
- **Convenient Fetching Helpers**: Includes methods like `fetch_one()`, `fetch_all()`, `execute_and_fetch_all()` to reduce boilerplate code.
- **Developer Guardrails**: Includes a pre-execution check to warn developers if they accidentally write obviously unsafe queries.
- **Modern Python**: Uses type hints, logging, and a clean, object-oriented structure.

## Why It's Secure: The Power of Parameterization

SQL injection happens when a database mistakes user-supplied data for a command.

**Vulnerable (Bad) Example:**
```python
user_input = "' OR '1'='1' --"
query = f"SELECT * FROM users WHERE username = {user_input}" # NEVER DO THIS!
```
The database would see and execute: `SELECT * FROM users WHERE username = '' OR '1'='1' --`, potentially exposing all user data.

**Secure (Good) Example:**
This wrapper forces you to separate the command from the data:
```python
user_input = "' OR '1'='1' --"
query = "SELECT * FROM users WHERE username = ?" # The SQL command (template)
params = (user_input,) # The data (to be safely inserted)

db.execute_and_fetch_one(query, params)
```
The database driver does not combine these into a vulnerable string. Instead, it receives the query template and the parameters separately. It knows to treat the `user_input` string as a single, literal piece of data to be searched for, not as a command to be executed. The malicious string will fail to find a matching username, and the database remains secure.

## Installation

No external libraries are required. This script uses Python's standard `sqlite3`, `re`, `os`, and `logging` modules. Simply include the `sql_injection.py` file in your project.

## Usage

The `SafeDBWrapper` is designed to be used as a context manager.

### Connecting and Running a Transaction

All operations inside the `with` block are part of a single transaction. The transaction is automatically **committed** if the block completes successfully or **rolled back** if an exception occurs.

```python
from safe_db_wrapper import SafeDBWrapper

DB_FILE = "my_database.db"

try:
    with SafeDBWrapper(DB_FILE) as db:
        # Create a table (this runs in a transaction)
        db.execute("DROP TABLE IF EXISTS products")
        db.execute("""
            CREATE TABLE products (
                id INTEGER PRIMARY KEY,
                name TEXT NOT NULL,
                price REAL
            )
        """)

        # Insert multiple items in the same transaction
        products_to_add = [
            ('Laptop', 999.99),
            ('Mouse', 24.95),
            ('Keyboard', 75.50)
        ]
        for p in products_to_add:
            db.execute("INSERT INTO products (name, price) VALUES (?, ?)", p)

except Exception as e:
    print(f"An error occurred: {e}")

```

### Fetching Data

Use the convenient helper methods to retrieve data. The wrapper returns dictionary-like `sqlite3.Row` objects.

```python
with SafeDBWrapper(DB_FILE) as db:
    # Fetch all products
    all_products = db.execute_and_fetch_all("SELECT * FROM products ORDER BY price DESC")
    print("All products:")
    for prod in all_products:
        print(f"- {prod['name']}: ${prod['price']:.2f}")

    # Fetch a single product
    product_name = "Mouse"
    mouse = db.execute_and_fetch_one(
        "SELECT * FROM products WHERE name = ?",
        (product_name,)
    )
    if mouse:
        print(f"\nFound product: {dict(mouse)}")
```

## Adaptability

While written for `sqlite3`, the class structure can be adapted for other database systems like PostgreSQL (`psycopg2`) or MySQL (`mysql-connector-python`). To adapt it, you would primarily need to:
1.  Change the `import` statement and the `connect()` method.
2.  Adjust the query placeholder style if necessary (e.g., from `?` to `%s`).
3.  Modify the exception handling to catch driver-specific errors.

```
