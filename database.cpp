#include "database.h"
#include <QFile>
#include <QDir>
#include <QSqlRecord>

Database& Database::instance()
{
    static Database instance;
    return instance;
}

Database::~Database()
{
    closeDatabase();
}

bool Database::initDatabase(const QString& dbName)
{
    m_db = QSqlDatabase::addDatabase("QSQLITE");
    m_db.setDatabaseName(dbName);

    if (!m_db.open()) {
        qDebug() << "Error: connection with database failed" << m_db.lastError();
        return false;
    }
    
    qDebug() << "Database connected:" << dbName;
    return createTable();
}

void Database::closeDatabase()
{
    if (m_db.isOpen()) {
        m_db.close();
        QSqlDatabase::removeDatabase(QSqlDatabase::defaultConnection);
    }
}

bool Database::createTable()
{
    QSqlQuery query(m_db);
    QString createStr = R"(
        CREATE TABLE IF NOT EXISTS products (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            name TEXT NOT NULL,
            quantity INTEGER NOT NULL,
            price REAL NOT NULL,
            supplier TEXT NOT NULL,
            date_added TEXT NOT NULL
        )
    )";

    if (!query.exec(createStr)) {
        qDebug() << "Failed to create table:" << query.lastError();
        return false;
    }

    return true;
}

bool Database::addProduct(const Product& product, QString* errorMsg)
{
    QSqlQuery query(m_db);
    query.prepare("INSERT INTO products (name, quantity, price, supplier, date_added) "
                  "VALUES (:name, :quantity, :price, :supplier, :date)");
    
    query.bindValue(":name", product.name);
    query.bindValue(":quantity", product.quantity);
    query.bindValue(":price", product.price);
    query.bindValue(":supplier", product.supplier);
    
    QString dateStr = product.dateAdded.isEmpty() ? QDateTime::currentDateTime().toString(Qt::ISODate) : product.dateAdded;
    query.bindValue(":date", dateStr);

    if (!query.exec()) {
        if (errorMsg) *errorMsg = query.lastError().text();
        qDebug() << "Add product failed:" << query.lastError();
        return false;
    }
    return true;
}

bool Database::updateProduct(const Product& product, QString* errorMsg)
{
    QSqlQuery query(m_db);
    query.prepare("UPDATE products SET name = :name, quantity = :quantity, "
                  "price = :price, supplier = :supplier WHERE id = :id");
                  
    query.bindValue(":name", product.name);
    query.bindValue(":quantity", product.quantity);
    query.bindValue(":price", product.price);
    query.bindValue(":supplier", product.supplier);
    query.bindValue(":id", product.id);

    if (!query.exec()) {
        if (errorMsg) *errorMsg = query.lastError().text();
        qDebug() << "Update product failed:" << query.lastError();
        return false;
    }
    return true;
}

bool Database::deleteProduct(int id, QString* errorMsg)
{
    QSqlQuery query(m_db);
    query.prepare("DELETE FROM products WHERE id = :id");
    query.bindValue(":id", id);

    if (!query.exec()) {
        if (errorMsg) *errorMsg = query.lastError().text();
        qDebug() << "Delete product failed:" << query.lastError();
        return false;
    }
    return true;
}

std::vector<Product> Database::getAllProducts()
{
    std::vector<Product> products;
    QSqlQuery query(m_db);
    query.prepare("SELECT * FROM products");
    
    if (query.exec()) {
        while (query.next()) {
            Product p;
            p.id = query.value("id").toInt();
            p.name = query.value("name").toString();
            p.quantity = query.value("quantity").toInt();
            p.price = query.value("price").toDouble();
            p.supplier = query.value("supplier").toString();
            p.dateAdded = query.value("date_added").toString();
            products.push_back(p);
        }
    } else {
        qDebug() << "Fetch all products failed:" << query.lastError();
    }
    
    return products;
}

std::vector<Product> Database::searchProducts(const QString& keyword)
{
    std::vector<Product> products;
    QSqlQuery query(m_db);
    
    // Search by ID (if keyword is a number) or by name
    bool isInt;
    int idSearch = keyword.toInt(&isInt);
    
    if (isInt) {
        query.prepare("SELECT * FROM products WHERE id = :id OR name LIKE :name");
        query.bindValue(":id", idSearch);
    } else {
        query.prepare("SELECT * FROM products WHERE name LIKE :name");
    }
    query.bindValue(":name", "%" + keyword + "%");
    
    if (query.exec()) {
        while (query.next()) {
            Product p;
            p.id = query.value("id").toInt();
            p.name = query.value("name").toString();
            p.quantity = query.value("quantity").toInt();
            p.price = query.value("price").toDouble();
            p.supplier = query.value("supplier").toString();
            p.dateAdded = query.value("date_added").toString();
            products.push_back(p);
        }
    }
    
    return products;
}

std::vector<Product> Database::filterLowStock(int threshold)
{
    std::vector<Product> products;
    QSqlQuery query(m_db);
    query.prepare("SELECT * FROM products WHERE quantity < :threshold");
    query.bindValue(":threshold", threshold);
    
    if (query.exec()) {
        while (query.next()) {
            Product p;
            p.id = query.value("id").toInt();
            p.name = query.value("name").toString();
            p.quantity = query.value("quantity").toInt();
            p.price = query.value("price").toDouble();
            p.supplier = query.value("supplier").toString();
            p.dateAdded = query.value("date_added").toString();
            products.push_back(p);
        }
    }
    
    return products;
}

int Database::getTotalProductCount()
{
    QSqlQuery query(m_db);
    query.prepare("SELECT COUNT(*) FROM products");
    if (query.exec() && query.next()) {
        return query.value(0).toInt();
    }
    return 0;
}

int Database::getLowStockCount(int threshold)
{
    QSqlQuery query(m_db);
    query.prepare("SELECT COUNT(*) FROM products WHERE quantity < :threshold");
    query.bindValue(":threshold", threshold);
    if (query.exec() && query.next()) {
        return query.value(0).toInt();
    }
    return 0;
}
