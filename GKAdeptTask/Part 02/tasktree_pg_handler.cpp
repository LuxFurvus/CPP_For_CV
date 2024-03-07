
#include <iostream>
#include <pqxx/pqxx>
#include "tasktree_pg_handler.hpp"


PostgreHandler::PostgreHandler(const std::string& connection_string) : c_(connection_string) {
	check_and_create_table();
}

void PostgreHandler::check_and_create_table() {
	try {
		pqxx::work w(c_);
		std::string check_table_query = "SELECT EXISTS (SELECT FROM pg_tables WHERE schemaname = 'public' AND tablename  = 'tree_elements');";
		pqxx::result r = w.exec(check_table_query);
		if (!r[0][0].as<bool>()) {
			w.exec("CREATE TABLE tree_elements (id INT PRIMARY KEY, name VARCHAR(255) NOT NULL);");
			w.commit();
			std::cout << "Table tree_elements created.\n";
		}
		else {
			std::cout << "Table tree_elements already exists.\n";
		}
	}
	catch (const pqxx::sql_error& e) {
		std::cerr << "SQL error in check_and_create_table: " << e.what() << "\n";
		std::cerr << "Query was: " << e.query() << "\n";
	}
	catch (const std::exception& e) {
		std::cerr << "Exception in check_and_create_table: " << e.what() << "\n";
	}
}

bool PostgreHandler::add_element(int id, const std::string& name) {
	try {
		pqxx::work w(c_);
		if (element_exists(id, w)) {
			std::cout << "Element with ID " << id << " already exists.\n";
			return false;
		}
		std::string insert_query = "INSERT INTO tree_elements (id, name) VALUES (" + w.quote(id) + ", " + w.quote(name) + ");";
		w.exec(insert_query);
		w.commit();
		std::cout << "Element added.\n";
		return true;
	}
	catch (const pqxx::sql_error& e) {
		std::cerr << "SQL error in add_element: " << e.what() << "\n";
		std::cerr << "Query was: " << e.query() << "\n";
		return false;
	}
	catch (const std::exception& e) {
		std::cerr << "Exception in add_element: " << e.what() << "\n";
		return false;
	}
}

bool PostgreHandler::edit_element_name(int id, const std::string& new_name) {
	try {
		pqxx::work w(c_);
		if (!element_exists(id, w)) {
			std::cout << "Element with ID " << id << " does not exist.\n";
			return false;
		}
		std::string update_query = "UPDATE tree_elements SET name = " + w.quote(new_name) + " WHERE id = " + w.quote(id) + ";";
		w.exec(update_query);
		w.commit();
		std::cout << "Element name updated.\n";
		return true;
	}
	catch (const pqxx::sql_error& e) {
		std::cerr << "SQL error in edit_element_name: " << e.what() << "\n";
		std::cerr << "Query was: " << e.query() << "\n";
		return false;
	}
	catch (const std::exception& e) {
		std::cerr << "Exception in edit_element_name: " << e.what() << "\n";
		return false;
	}
}

bool PostgreHandler::edit_element_id(int current_id, int new_id) {
	try {
		pqxx::work w(c_);
		if (!element_exists(current_id, w)) {
			std::cout << "Element with ID " << current_id << " does not exist.\n";
			return false;
		}
		if (current_id != new_id && element_exists(new_id, w)) {
			std::cout << "Element with new ID " << new_id << " already exists.\n";
			return false;
		}
		std::string update_query = "UPDATE tree_elements SET id = " + w.quote(new_id) + " WHERE id = " + w.quote(current_id) + ";";
		w.exec(update_query);
		w.commit();
		std::cout << "Element ID updated.\n";
		return true;
	}
	catch (const pqxx::sql_error& e) {
		std::cerr << "SQL error in edit_element_id: " << e.what() << "\n";
		std::cerr << "Query was: " << e.query() << "\n";
		return false;
	}
	catch (const std::exception& e) {
		std::cerr << "Exception in edit_element_id: " << e.what() << "\n";
		return false;
	}
}
bool PostgreHandler::delete_element(int id) {
	try {
		pqxx::work w(c_);
		if (!element_exists(id, w)) {
			std::cout << "Element with ID " << id << " does not exist.\n";
			return false;
		}
		std::string delete_query = "DELETE FROM tree_elements WHERE id = " + w.quote(id) + ";";
		w.exec(delete_query);
		w.commit();
		std::cout << "Element deleted.\n";
		return true;
	}
	catch (const pqxx::sql_error& e) {
		std::cerr << "SQL error in delete_element: " << e.what() << "\n";
		std::cerr << "Query was: " << e.query() << "\n";
		return false;
	}
	catch (const std::exception& e) {
		std::cerr << "Exception in delete_element: " << e.what() << "\n";
		return false;
	}
}

bool PostgreHandler::element_exists(int id, pqxx::work& w) const {
	std::string check_query("SELECT COUNT(*) FROM tree_elements WHERE id = ");
	check_query.append(w.quote(id)).append(";");
	pqxx::result r = w.exec(check_query);
	return r[0][0].as<int>() > 0;
}

const std::vector<std::pair<int, std::string>> PostgreHandler::get_all_nodes() {
	try {
		pqxx::work w(c_);
		std::vector<std::pair<int, std::string>> nodes;
		pqxx::result r = w.exec("SELECT id, name FROM tree_elements;");
		for (auto row : r) {
			nodes.emplace_back(row[0].as<int>(), row[1].as<std::string>());
		}
		return nodes;
	}
	catch (const pqxx::sql_error& e) {
		std::cerr << "SQL error in get_all_nodes: " << e.what() << "\n";
		std::cerr << "Query was: " << e.query() << "\n";
		return {};  // Return an empty vector in case of exception
	}
	catch (const std::exception& e) {
		std::cerr << "Exception in get_all_nodes: " << e.what() << "\n";
		return {};  // Return an empty vector in case of exception
	}
}
