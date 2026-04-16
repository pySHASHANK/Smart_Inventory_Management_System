#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <QMessageBox>
#include <QFileDialog>
#include <QTextStream>
#include <QVBoxLayout>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    
    // Initialize Database
    Database::instance().initDatabase();
    
    // Setup UI components
    setupTable();
    
    // Setup Chart
    m_chart = new QChart();
    m_chart->setTitle("Inventory Status");
    m_chart->setAnimationOptions(QChart::SeriesAnimations);
    
    m_chartView = new QChartView(m_chart);
    m_chartView->setRenderHint(QPainter::Antialiasing);
    
    QVBoxLayout* chartLayout = new QVBoxLayout(ui->chartContainer);
    chartLayout->addWidget(m_chartView);
    chartLayout->setContentsMargins(0,0,0,0);
    
    // Load initial data
    on_btnShowAll_clicked();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::setupTable()
{
    ui->tableWidget->setColumnCount(6);
    ui->tableWidget->setHorizontalHeaderLabels({"ID", "Name", "Quantity", "Price ($)", "Supplier", "Date Added"});
    ui->tableWidget->horizontalHeader()->setStretchLastSection(true);
    ui->tableWidget->setAlternatingRowColors(true);
}

void MainWindow::refreshTable(const std::vector<Product>& products)
{
    ui->tableWidget->setRowCount(0);
    
    int row = 0;
    bool hasLowStock = false;
    
    for (const auto& p : products) {
        ui->tableWidget->insertRow(row);
        
        QTableWidgetItem* idItem = new QTableWidgetItem(QString::number(p.id));
        QTableWidgetItem* nameItem = new QTableWidgetItem(p.name);
        QTableWidgetItem* qtyItem = new QTableWidgetItem(QString::number(p.quantity));
        QTableWidgetItem* priceItem = new QTableWidgetItem(QString::number(p.price, 'f', 2));
        QTableWidgetItem* supplierItem = new QTableWidgetItem(p.supplier);
        QTableWidgetItem* dateItem = new QTableWidgetItem(p.dateAdded);
        
        // Low stock highlight
        if (p.quantity < 10) {
            hasLowStock = true;
            QColor lowStockColor(255, 100, 100, 100);
            idItem->setBackground(lowStockColor);
            nameItem->setBackground(lowStockColor);
            qtyItem->setBackground(lowStockColor);
            priceItem->setBackground(lowStockColor);
            supplierItem->setBackground(lowStockColor);
            dateItem->setBackground(lowStockColor);
        }
        
        ui->tableWidget->setItem(row, 0, idItem);
        ui->tableWidget->setItem(row, 1, nameItem);
        ui->tableWidget->setItem(row, 2, qtyItem);
        ui->tableWidget->setItem(row, 3, priceItem);
        ui->tableWidget->setItem(row, 4, supplierItem);
        ui->tableWidget->setItem(row, 5, dateItem);
        
        row++;
    }
    
    if (hasLowStock) {
        showStatusMessage("Warning: Some items have low stock (Quantity < 10)!", true);
    }
    
    updateDashboard();
    updateChart();
}

void MainWindow::updateDashboard()
{
    int total = Database::instance().getTotalProductCount();
    int lowStock = Database::instance().getLowStockCount();
    
    ui->label_totalCount->setText(QString("Total Products: %1").arg(total));
    ui->label_lowStockCount->setText(QString("Low Stock Items: %1").arg(lowStock));
}

void MainWindow::updateChart()
{
    m_chart->removeAllSeries();
    
    int total = Database::instance().getTotalProductCount();
    int lowStock = Database::instance().getLowStockCount();
    int normalStock = total - lowStock;
    
    if (total == 0) return;
    
    QPieSeries *series = new QPieSeries();
    series->append("Normal Stock", normalStock);
    QPieSlice *lowSlice = series->append("Low Stock", lowStock);
    
    if (lowStock > 0) {
        lowSlice->setExploded();
        lowSlice->setLabelVisible();
        lowSlice->setPen(QPen(Qt::darkRed, 2));
        lowSlice->setBrush(Qt::red);
    }
    
    m_chart->addSeries(series);
}

void MainWindow::showStatusMessage(const QString& message, bool isError)
{
    if (isError) {
        ui->statusbar->setStyleSheet("color: red; font-weight: bold;");
    } else {
        ui->statusbar->setStyleSheet("color: green; font-weight: bold;");
    }
    ui->statusbar->showMessage(message, 5000); // Show for 5 seconds
}

bool MainWindow::validateInput()
{
    if (ui->lineEdit_name->text().trimmed().isEmpty()) {
        QMessageBox::warning(this, "Input Error", "Product Name cannot be empty.");
        return false;
    }
    if (ui->lineEdit_supplier->text().trimmed().isEmpty()) {
        QMessageBox::warning(this, "Input Error", "Supplier Name cannot be empty.");
        return false;
    }
    if (ui->doubleSpinBox_price->value() <= 0) {
        QMessageBox::warning(this, "Input Error", "Price must be greater than 0.");
        return false;
    }
    return true;
}

void MainWindow::clearForm()
{
    ui->lineEdit_id->clear();
    ui->lineEdit_name->clear();
    ui->spinBox_qty->setValue(0);
    ui->doubleSpinBox_price->setValue(0.0);
    ui->lineEdit_supplier->clear();
    ui->tableWidget->clearSelection();
}

void MainWindow::on_btnAdd_clicked()
{
    if (!validateInput()) return;
    
    Product p;
    p.name = ui->lineEdit_name->text().trimmed();
    p.quantity = ui->spinBox_qty->value();
    p.price = ui->doubleSpinBox_price->value();
    p.supplier = ui->lineEdit_supplier->text().trimmed();
    
    QString error;
    if (Database::instance().addProduct(p, &error)) {
        showStatusMessage("Product added successfully!");
        clearForm();
        on_btnShowAll_clicked();
    } else {
        QMessageBox::critical(this, "Database Error", "Failed to add product: " + error);
    }
}

void MainWindow::on_btnUpdate_clicked()
{
    QString idStr = ui->lineEdit_id->text();
    if (idStr.isEmpty()) {
        QMessageBox::warning(this, "Selection Error", "Please select a product from the table to update.");
        return;
    }
    
    if (!validateInput()) return;
    
    Product p;
    p.id = idStr.toInt();
    p.name = ui->lineEdit_name->text().trimmed();
    p.quantity = ui->spinBox_qty->value();
    p.price = ui->doubleSpinBox_price->value();
    p.supplier = ui->lineEdit_supplier->text().trimmed();
    
    QString error;
    if (Database::instance().updateProduct(p, &error)) {
        showStatusMessage("Product updated successfully!");
        clearForm();
        on_btnShowAll_clicked();
    } else {
        QMessageBox::critical(this, "Database Error", "Failed to update product: " + error);
    }
}

void MainWindow::on_btnDelete_clicked()
{
    QString idStr = ui->lineEdit_id->text();
    if (idStr.isEmpty()) {
        QMessageBox::warning(this, "Selection Error", "Please select a product from the table to delete.");
        return;
    }
    
    int reply = QMessageBox::question(this, "Confirm Delete", 
                                      "Are you sure you want to delete this product?", 
                                      QMessageBox::Yes | QMessageBox::No);
                                      
    if (reply == QMessageBox::Yes) {
        QString error;
        if (Database::instance().deleteProduct(idStr.toInt(), &error)) {
            showStatusMessage("Product deleted successfully!");
            clearForm();
            on_btnShowAll_clicked();
        } else {
            QMessageBox::critical(this, "Database Error", "Failed to delete product: " + error);
        }
    }
}

void MainWindow::on_btnClear_clicked()
{
    clearForm();
}

void MainWindow::on_btnSearch_clicked()
{
    QString keyword = ui->lineEdit_search->text().trimmed();
    if (keyword.isEmpty()) {
        on_btnShowAll_clicked();
        return;
    }
    
    ui->checkBox_lowStock->setChecked(false);
    std::vector<Product> results = Database::instance().searchProducts(keyword);
    refreshTable(results);
    showStatusMessage(QString("Found %1 products.").arg(results.size()));
}

void MainWindow::on_btnShowAll_clicked()
{
    ui->lineEdit_search->clear();
    ui->checkBox_lowStock->setChecked(false);
    std::vector<Product> all = Database::instance().getAllProducts();
    refreshTable(all);
    showStatusMessage(QString("Displaying all %1 products.").arg(all.size()));
}

void MainWindow::on_checkBox_lowStock_stateChanged(int state)
{
    if (state == Qt::Checked) {
        std::vector<Product> lowStock = Database::instance().filterLowStock();
        refreshTable(lowStock);
        showStatusMessage(QString("Displaying %1 low stock products.").arg(lowStock.size()));
    } else {
        on_btnShowAll_clicked();
    }
}

void MainWindow::on_tableWidget_itemSelectionChanged()
{
    QList<QTableWidgetItem*> selectedItems = ui->tableWidget->selectedItems();
    if (selectedItems.isEmpty()) return;
    
    int row = selectedItems.first()->row();
    
    ui->lineEdit_id->setText(ui->tableWidget->item(row, 0)->text());
    ui->lineEdit_name->setText(ui->tableWidget->item(row, 1)->text());
    ui->spinBox_qty->setValue(ui->tableWidget->item(row, 2)->text().toInt());
    ui->doubleSpinBox_price->setValue(ui->tableWidget->item(row, 3)->text().toDouble());
    ui->lineEdit_supplier->setText(ui->tableWidget->item(row, 4)->text());
}

void MainWindow::on_btnExportCsv_clicked()
{
    QString fileName = QFileDialog::getSaveFileName(this, "Export CSV", "", "CSV Files (*.csv)");
    if (fileName.isEmpty()) return;
    
    QFile file(fileName);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QMessageBox::critical(this, "Export Error", "Cannot open file for writing.");
        return;
    }
    
    QTextStream out(&file);
    // Header
    out << "ID,Name,Quantity,Price,Supplier,DateAdded\n";
    
    std::vector<Product> all = Database::instance().getAllProducts();
    for (const auto& p : all) {
        out << p.id << "," 
            << "\"" << p.name << "\"," 
            << p.quantity << "," 
            << p.price << "," 
            << "\"" << p.supplier << "\"," 
            << p.dateAdded << "\n";
    }
    
    file.close();
    showStatusMessage("Data successfully exported to CSV.");
}

void MainWindow::on_btnImportCsv_clicked()
{
    QString fileName = QFileDialog::getOpenFileName(this, "Import CSV", "", "CSV Files (*.csv)");
    if (fileName.isEmpty()) return;
    
    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QMessageBox::critical(this, "Import Error", "Cannot open file for reading.");
        return;
    }
    
    QTextStream in(&file);
    bool isFirstLine = true;
    int importedCount = 0;
    
    while (!in.atEnd()) {
        QString line = in.readLine();
        if (isFirstLine) {
            isFirstLine = false;
            continue; // Skip header
        }
        
        QStringList parts = line.split(',');
        if (parts.size() >= 5) {
            Product p;
            p.name = parts[1].remove('"');
            p.quantity = parts[2].toInt();
            p.price = parts[3].toDouble();
            p.supplier = parts[4].remove('"');
            if (parts.size() >= 6) {
                p.dateAdded = parts[5];
            }
            
            if (Database::instance().addProduct(p)) {
                importedCount++;
            }
        }
    }
    
    file.close();
    on_btnShowAll_clicked();
    showStatusMessage(QString("Successfully imported %1 products from CSV.").arg(importedCount));
}
