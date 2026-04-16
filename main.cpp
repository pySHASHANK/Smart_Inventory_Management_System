    #include "mainwindow.h"

    #include <QApplication>
    #include <QStyleFactory>
    #include <QPalette>

    void setupDarkMode(QApplication& app)
    {
        app.setStyle(QStyleFactory::create("Fusion"));
        
        QPalette darkPalette;
        darkPalette.setColor(QPalette::Window, QColor(53, 53, 53));
        darkPalette.setColor(QPalette::WindowText, Qt::white);
        darkPalette.setColor(QPalette::Base, QColor(35, 35, 35));
        darkPalette.setColor(QPalette::AlternateBase, QColor(53, 53, 53));
        darkPalette.setColor(QPalette::ToolTipBase, QColor(25, 25, 25));
        darkPalette.setColor(QPalette::ToolTipText, Qt::white);
        darkPalette.setColor(QPalette::Text, Qt::white);
        darkPalette.setColor(QPalette::Button, QColor(53, 53, 53));
        darkPalette.setColor(QPalette::ButtonText, Qt::white);
        darkPalette.setColor(QPalette::BrightText, Qt::red);
        darkPalette.setColor(QPalette::Link, QColor(42, 130, 218));
        darkPalette.setColor(QPalette::Highlight, QColor(42, 130, 218));
        darkPalette.setColor(QPalette::HighlightedText, Qt::black);

        // Disabled text color
        darkPalette.setColor(QPalette::Disabled, QPalette::Text, Qt::darkGray);
        darkPalette.setColor(QPalette::Disabled, QPalette::ButtonText, Qt::darkGray);
        darkPalette.setColor(QPalette::Disabled, QPalette::WindowText, Qt::darkGray);

        app.setPalette(darkPalette);
        
        app.setStyleSheet("QToolTip { color: #ffffff; background-color: #2a82da; border: 1px solid white; }"
                        "QTableWidget { gridline-color: #444444; }"
                        "QHeaderView::section { background-color: #353535; padding: 4px; border: 1px solid #444444; }");
    }

    int main(int argc, char *argv[])
    {
        QApplication a(argc, argv);
        
        // Apply Dark Mode Theme
        setupDarkMode(a);
        
        MainWindow w;
        w.show();
        return a.exec();
    }
