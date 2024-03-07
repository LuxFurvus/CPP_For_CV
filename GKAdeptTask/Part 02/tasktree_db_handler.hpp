
#ifndef TASKTREE_DB_HANDLER_HPP
#define TASKTREE_DB_HANDLER_HPP

#include <string>
#include <vector>

class DatabaseHandler {
public:
	virtual ~DatabaseHandler() = default;
	virtual bool add_element(int id, const std::string& name) = 0;
	virtual bool delete_element(int id) = 0;
	virtual bool edit_element_id(int current_id, int new_id) = 0;
	virtual bool edit_element_name(int id, const std::string& new_name) = 0;

	virtual const std::vector<std::pair<int, std::string>> get_all_nodes() = 0;
};

#endif // TASKTREE_DB_HANDLER_HPP
