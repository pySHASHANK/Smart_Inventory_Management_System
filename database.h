#ifndef DATABASE_H
#define DATABASE_H

#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QString>
#include <QVariant>
#include <QDebug>
#include <QDateTime>
#include <vector>

struct Product {
    int id;
    QString name;
    int quantity;
    double price;
    QString supplier;
    QString dateAdded;
};

class Database
{
public:
    static Database& instance();
    
    // Disable copy/move
    Database(const Database&) = delete;
    Database& operator=(const Database&) = delete;
    Database(Database&&) = delete;
    Database& operator=(Database&&) = delete;

    bool initDatabase(const QString& dbName = "products.db");
    void closeDatabase();

    // CRUD
    bool addProduct(const Product& product, QString* errorMsg = nullptr);
    bool updateProduct(const Product& product, QString* errorMsg = nullptr);
    bool deleteProduct(int id, QString* errorMsg = nullptr);
    
    // Fetch
    std::vector<Product> getAllProducts();
    std::vector<Product> searchProducts(const QString& keyword);
    std::vector<Product> filterLowStock(int threshold = 10);
    
    // Stats
    int getTotalProductCount();
    int getLowStockCount(int threshold = 10);

private:
    Database() = default;
    ~Database();
    
    bool createTable();

    QSqlDatabase m_db;
};

#endif // DATABASE_H
