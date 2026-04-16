#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTableWidgetItem>
#include <QtCharts/QChartView>
#include <QtCharts/QPieSeries>
#include <QtCharts/QChart>
#include "database.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_btnAdd_clicked();
    void on_btnUpdate_clicked();
    void on_btnDelete_clicked();
    void on_btnClear_clicked();
    
    void on_btnSearch_clicked();
    void on_btnShowAll_clicked();
    void on_checkBox_lowStock_stateChanged(int arg1);
    
    void on_tableWidget_itemSelectionChanged();
    
    void on_btnExportCsv_clicked();
    void on_btnImportCsv_clicked();

private:
    Ui::MainWindow *ui;
    void setupTable();
    void refreshTable(const std::vector<Product>& products);
    void updateDashboard();
    void updateChart();
    void showStatusMessage(const QString& message, bool isError = false);
    
    bool validateInput();
    void clearForm();
    
    QChart* m_chart;
    QChartView* m_chartView;
};

#endif // MAINWINDOW_H
