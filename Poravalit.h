#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_Poravalit.h"
#include "mysql.h"
#include <string>
#include <vector>
#include <algorithm>
#pragma comment (lib, "ws2_32.lib")
#pragma comment (lib, "libmysql.lib")

class Poravalit : public QMainWindow
{
    Q_OBJECT

public:
    Poravalit(QWidget *parent = nullptr);
    ~Poravalit();
    MYSQL* conn;
    MYSQL_RES* res;
    void connect_bd(std::string adress, std::string pass, std::string name);
    bool get_table_data(std::string tname, std::string search, std::string search_col);
    void clear_table();
    void update_table(QString table, std::string search, std::string search_col);
    void change_cell(std::string table, std::string idn, std::string targ_id, std::string targ_col, std::string targ_val);
    void add_row(std::string table, std::vector<std::string> colnames, std::vector<std::string> vals);
    void delete_row(std::string table, std::string idn, std::string targ_id);

public slots:
    void on_treeWidget_itemClicked(QTreeWidgetItem* item, int column);
    void resizeEvent(QResizeEvent* event);
    void on_pushButton_clicked();
    void on_pushButton_2_clicked();
    void on_pushButton_3_clicked();
    void on_pushButton_4_clicked();
    void on_pushButton_5_clicked();

private:
    Ui::PoravalitClass ui;
};
