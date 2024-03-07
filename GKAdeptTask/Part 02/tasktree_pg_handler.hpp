#pragma once

#ifndef TASKTREE_PG_HANDLER_HPP
#define TASKTREE_PG_HANDLER_HPP

#include <utility>
#include <pqxx/pqxx>
#include "tasktree_db_handler.hpp"

class PostgreHandler : public DatabaseHandler {
private:
	pqxx::connection c_;
	bool element_exists(int id, pqxx::work& w) const;
	void check_and_create_table();
public:
	explicit PostgreHandler(const std::string& connection_string);
	const std::vector<std::pair<int, std::string>> get_all_nodes() override;
	bool add_element(int id, const std::string& name) override;
	bool edit_element_id(int current_id, int new_id) override;
	bool edit_element_name(int id, const std::string& new_name) override;
	bool delete_element(int id) override;
};

#endif // TASKTREE_PG_HANDLER_HPP
