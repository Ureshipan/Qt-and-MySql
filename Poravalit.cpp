#pragma execution_character_set("utf-8")
#include "Poravalit.h"


void Poravalit::connect_bd(std::string adress, std::string pass, std::string name)
{
	conn = mysql_init(NULL);
	if (!mysql_real_connect(conn, adress.c_str(), "root", pass.c_str(), name.c_str(), NULL, NULL, 0)) //labamysql information_schema performance_schema
	{
		// ≈сли нет возможности установить соединение с сервером
		// базы данных выводим сообщение об ошибке
		qDebug() << stderr << "Error: can'tconnect to database %s\n";
	}
}

bool Poravalit::get_table_data(std::string tname, std::string search = "none", std::string search_col = "none")
{
	std::string zapros;
	if (tname != "poravalit")
	{
		zapros = "SELECT * FROM " + tname;
		if (search == "none")
		{
			zapros += ";";
		}
		else
		{
			std::transform(search.begin(), search.end(), search.begin(), [](unsigned char c) { return std::tolower(c); });
			zapros += " WHERE LOWER(" + tname + "." + search_col + ") LIKE '%" + search + "%';";
		}
		ui.pushButton->setEnabled(true);
		ui.pushButton_5->setEnabled(true);
	}
	else
	{
		zapros = "SELECT orders.id, clients.name, orders.order_date, tours.title, tours.price FROM poravalit.clients JOIN poravalit.orders JOIN poravalit.tours WHERE orders.client_id = clients.client_id AND orders.tour_id = tours.id";
		if (search == "none")
		{
			zapros += ";";
		}
		else
		{
			std::transform(search.begin(), search.end(), search.begin(), [](unsigned char c) { return std::tolower(c); });
			zapros += " AND LOWER(" + search_col + ") LIKE '%" + search + "%';";
		}
		ui.pushButton->setEnabled(false);
		ui.pushButton_5->setEnabled(false);
	}
	mysql_query(conn, zapros.c_str());
	if (res = mysql_store_result(conn))
	{
		return true;
	}
	else
	{
		return false;
	}
}


void Poravalit::clear_table()
{
	ui.tableWidget->clear();
	int ccount = ui.tableWidget->columnCount();
	int rcount = ui.tableWidget->rowCount();
	for (int i = 0; i < ccount; i++)
	{
		ui.tableWidget->removeColumn(0);
	}
	for (int i = 0; i < rcount; i++)
	{
		ui.tableWidget->removeRow(0);
	}
}

void Poravalit::update_table(QString table, std::string search = "none", std::string search_col = "none")
{
	std::map<QString, std::vector <QString>> colnames;
	colnames["clients"] = { "client_id", "name", "email" };
	colnames["tours"] = { "id", "title", "email", "price" };
	colnames["orders"] = { "id", "client_id", "tour_id", "order_date" };
	colnames["poravalit"] = { "id", "name", "order_date", "title", "price" };


	clear_table();
	qDebug() << get_table_data(table.toStdString(), search, search_col);
	MYSQL_ROW row;
	int rcount = 0;
	int unsigned columns = mysql_num_fields(res);
	ui.comboBox->clear();
	for (int i = 0; i < columns; i++)
	{
		ui.tableWidget->insertColumn(i);
		ui.tableWidget->setColumnWidth(i, ui.tableWidget->width() / columns - 1);
		//ui.tableWidget->horizontalHeaderItem(i)->setText(colnames[item->text(0)][i]);
		QTableWidgetItem* header = new QTableWidgetItem();
		header->setText(colnames[table][i]);
		ui.tableWidget->setHorizontalHeaderItem(i, header);
		ui.comboBox->addItem(colnames[table][i]);
	}
	//ui.tableWidget->horizontalHeaderItem(0)->setText("bruh");
	while ((row = mysql_fetch_row(res)) != NULL)
	{

		ui.tableWidget->insertRow(rcount);
		for (int i = 0; i < colnames[table].size(); i++)
		{
			QString text = row[i];
			QTableWidgetItem* cell = new QTableWidgetItem();
			cell->setText(text);
			ui.tableWidget->setItem(rcount, i, cell);
		}
		rcount++;
	}
}


void Poravalit::change_cell(std::string table, std::string idn, std::string targ_id, std::string targ_col, std::string targ_val)
{
	qDebug() << idn << targ_id << targ_col << targ_val;
	//UPDATE `poravalit`.`clients` SET `name` = 'Neko Arkk' WHERE (`client_id` = '8');
	std::string zapros = "UPDATE poravalit." + table + " SET " + targ_col + " = '" + targ_val + "' WHERE " + idn + " = '" + targ_id + "';";
	mysql_query(conn, zapros.c_str());
}


void Poravalit::add_row(std::string table, std::vector<std::string> colnames, std::vector<std::string> vals)
{
	// INSERT INTO `poravalit`.`clients` (`name`, `email`) VALUES ('name', 'email');
	std::string zapros_start = "INSERT INTO poravalit." + table + " (";
	std::string zapros_end = ") VALUES (";
	for (int i = 0; i < colnames.size(); i++)
	{
		zapros_start += colnames[i];
		zapros_end += "'" + vals[i] + "'";
		if (i < colnames.size() - 1)
		{
			zapros_start += ", ";
			zapros_end += ", ";
		}
		else
		{
			zapros_end += ");";
		}
	}
	zapros_start += zapros_end;
	mysql_query(conn, zapros_start.c_str());
}

void Poravalit::delete_row(std::string table, std::string idn, std::string targ_id)
{
	qDebug() << table << idn << targ_id;
	// DELETE FROM `poravalit`.`clients` WHERE (`client_id` = '10');
	if (table == "poravalit")
	{
		table = "orders";
	}
	std::string zapros = " DELETE FROM poravalit." + table + " WHERE " + idn + " = '" + targ_id + "';";
	mysql_query(conn, zapros.c_str());
}


Poravalit::Poravalit(QWidget *parent)
    : QMainWindow(parent)
{
    ui.setupUi(this);
	connect_bd("localhost", "277353", "poravalit");
	
	qDebug() << "connected";
	MYSQL_ROW row;
	mysql_query(conn, "show tables;");
	res = mysql_store_result(conn);
	ui.treeWidget->topLevelItem(0)->setExpanded(true);
	while ((row = mysql_fetch_row(res)) != NULL)
	{
		qDebug() << *row;
		QTreeWidgetItem* another_child = new QTreeWidgetItem();
		another_child->setText(0, *row);
		ui.treeWidget->topLevelItem(0)->addChild(another_child);
	}
}

Poravalit::~Poravalit()
{}


//SELECT orders.id, clients.name, orders.order_date, tours.title, tours.price FROM poravalit.clients JOIN poravalit.orders JOIN poravalit.tours WHERE orders.client_id = clients.client_id AND orders.tour_id = tours.id;

void Poravalit::on_treeWidget_itemClicked(QTreeWidgetItem* item, int column)
{
	update_table(item->text(0));
}

void Poravalit::resizeEvent(QResizeEvent* event)
{
	/*int ccount = ui.tableWidget->columnCount();
	for (int i = 0; i < ccount; i++)
	{
		ui.tableWidget->setColumnWidth(i, ui.tableWidget->width() / ccount - 1);
	}*/
	QWidget::resizeEvent(event);
}

void Poravalit::on_pushButton_clicked() // Push changes
{
	std::map<QString, std::vector <QString>> colnames;
	colnames["clients"] = { "client_id", "name", "email" };
	colnames["tours"] = { "id", "title", "email", "price" };
	colnames["orders"] = { "id", "client_id", "tour_id", "order_date" };
	colnames["poravalit"] = { "id", "name", "order_date", "title", "price" };

	qDebug() << get_table_data(ui.treeWidget->currentItem()->text(0).toStdString(), ui.lineEdit->text().toStdString(), ui.comboBox->currentText().toStdString());
	MYSQL_ROW row;
	int unsigned columns = mysql_num_fields(res);
	ui.comboBox->clear();
	//ui.tableWidget->horizontalHeaderItem(0)->setText("bruh");
	//while ((row = mysql_fetch_row(res)) != NULL)
	for( int j = 0; j < ui.tableWidget->rowCount(); j++)
	{
		if ((row = mysql_fetch_row(res)) != NULL)
		{
			for (int i = 1; i < colnames[ui.treeWidget->currentItem()->text(0)].size(); i++)
			{
				qDebug() << ui.tableWidget->item(j, 0)->text() << ui.tableWidget->item(j, i)->text() << row[i];
				if (ui.tableWidget->item(j, i)->text().toStdString() != row[i])
				{
					change_cell(ui.treeWidget->currentItem()->text(0).toStdString(),
						colnames[ui.treeWidget->currentItem()->text(0)][0].toStdString(),
						ui.tableWidget->item(j, 0)->text().toStdString(),
						colnames[ui.treeWidget->currentItem()->text(0)][i].toStdString(),
						ui.tableWidget->item(j, i)->text().toStdString());
				}
			}
		}
		else
		{
			std::vector<std::string> tempcolnames;
			std::vector<std::string> vals;
			for (int i = 1; i < colnames[ui.treeWidget->currentItem()->text(0)].size(); i++)
			{
				QTableWidgetItem* item1(ui.tableWidget->item(j, i));
				qDebug() << item1;
				if (not item1)
				{
					vals.push_back("");
				}
				else
				{
					vals.push_back(ui.tableWidget->item(j, i)->text().toStdString());
				}
				tempcolnames.push_back(colnames[ui.treeWidget->currentItem()->text(0)][i].toStdString());
			}
			add_row(ui.treeWidget->currentItem()->text(0).toStdString(),
				tempcolnames,
				vals);
		}
	}
	update_table(ui.treeWidget->currentItem()->text(0));
}

void Poravalit::on_pushButton_2_clicked() // Delete
{
	std::map<QString, std::vector <QString>> colnames;
	colnames["clients"] = { "client_id", "name", "email" };
	colnames["tours"] = { "id", "title", "email", "price" };
	colnames["orders"] = { "id", "client_id", "tour_id", "order_date" };
	colnames["poravalit"] = { "id", "name", "order_date", "title", "price" };
	
	QList<QTableWidgetSelectionRange> rangesList = ui.tableWidget->selectedRanges();

	foreach(QTableWidgetSelectionRange range, rangesList)
	{
		qDebug() << range.topRow();
		qDebug() << range.bottomRow();
		for (int i = range.topRow(); i < range.bottomRow() + 1; i++)
		{
			delete_row(ui.treeWidget->currentItem()->text(0).toStdString(),
				colnames[ui.treeWidget->currentItem()->text(0)][0].toStdString(),
				ui.tableWidget->item(i, 0)->text().toStdString());
		}
	}

	/*std::vector<QString> selected_ids;
	QList<QTableWidgetItem*> items(ui.tableWidget->selectedItems());
	for (int i = 0; i < items.size(); i++)
	{
		ui.tableWidget->selected
		QTableWidgetItem* item_ghost(items[i]);
		int column = item_ghost->column();
		if (column == 0)
		{
			QString text = item_ghost->text();
			if (std::find(selected_ids.begin(), selected_ids.end(), text) == selected_ids.end())
			{
				selected_ids.push_back(text);
				delete_row(ui.treeWidget->currentItem()->text(0).toStdString(),
					colnames[ui.treeWidget->currentItem()->text(0)][0].toStdString(),
					text.toStdString());
				ui.tableWidget->removeRow(item_ghost->row());
			}
		}
	}*/
	update_table(ui.treeWidget->currentItem()->text(0));
}

void Poravalit::on_pushButton_3_clicked() // Search
{
	if (not ui.lineEdit->text().isEmpty())
	{
		update_table(ui.treeWidget->currentItem()->text(0), ui.lineEdit->text().toStdString(), ui.comboBox->currentText().toStdString());
	}
	else
	{
		update_table(ui.treeWidget->currentItem()->text(0));
	}
}

void Poravalit::on_pushButton_4_clicked() // Reload data
{
	update_table(ui.treeWidget->currentItem()->text(0));
}

void Poravalit::on_pushButton_5_clicked() // Add row
{
	ui.tableWidget->insertRow(ui.tableWidget->rowCount());
}
