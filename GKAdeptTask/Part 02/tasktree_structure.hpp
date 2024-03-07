#pragma once
#ifndef _TASKTREE_STRUCTURE_HPP_
#define _TASKTREE_STRUCTURE_HPP_

#include <iostream>
#include <unordered_map>

#include <QString>
#include <QTreeWidget>

#include "tasktree_db_handler.hpp"

class TreeStructure {

	struct TreeNode {
		std::string name;
		QTreeWidgetItem* item;
	};

private:
	QTreeWidget* tree_root;
	std::unordered_map<int, TreeNode> node_map;
	DatabaseHandler* db_handler;
	int root_id{ 0 };

public:
	explicit TreeStructure(DatabaseHandler* dbh);

	~TreeStructure() {
		delete tree_root;
	}

	TreeStructure(const TreeStructure&) = delete;
	TreeStructure& operator=(const TreeStructure&) = delete;

	void add_node_to_tree(const std::string& name, int id, int parent_id = 0);
	void remove_node_from_tree(int id);
	void edit_tree_node_id(int id, int new_id);
	void edit_tree_node_name(int id, const std::string& new_name);
};

////////////////////////////////////////////////////////////////

#endif // _TASKTREE_STRUCTURE_HPP_