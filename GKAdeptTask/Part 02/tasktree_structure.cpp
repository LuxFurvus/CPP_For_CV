
#include <iostream>
#include "tasktree_structure.hpp"

TreeStructure::TreeStructure(DatabaseHandler* dbh)
	: tree_root(new QTreeWidget()), db_handler(dbh) {
	tree_root->setColumnCount(2);
	QStringList headers;
	headers << "ID" << "Name";
	tree_root->setHeaderLabels(headers);

	QTreeWidgetItem* root_item = new QTreeWidgetItem(tree_root);
	root_item->setText(0, QString::number(root_id));
	root_item->setText(1, "root");

	TreeNode root_node;
	root_node.name = "root";
	root_node.item = root_item;
	node_map.insert({ 0, root_node });
}

void TreeStructure::add_node_to_tree(const std::string& name, int id, int parent_id) {
	if (node_map.find(id) != node_map.end()) {
		std::cerr << "Error: Node with ID " << id << " already exists.\n";
		return;
	}

	if (parent_id != 0 && node_map.find(parent_id) == node_map.end()) {
		std::cerr << "Error: Parent ID " << parent_id << " does not exist.\n";
		return;
	}

	auto new_item = new QTreeWidgetItem();
	new_item->setText(0, QString::number(id));
	new_item->setText(1, QString::fromStdString(name));

	// Temporarily adding the new node to the tree and node_map for potential rollback
	if (parent_id == 0) {
		tree_root->addTopLevelItem(new_item);
	}
	else {
		node_map[parent_id].item->addChild(new_item);
	}

	TreeNode newNode;
	newNode.name = name;
	newNode.item = new_item;
	node_map[id] = std::move(newNode);

	// Check if the database operation is successful. If no, continue
	if (db_handler->add_element(id, name)) return;

	// Rollback the changes: remove the added node from the tree and node_map
	if (parent_id == 0) {
		tree_root->takeTopLevelItem(tree_root->indexOfTopLevelItem(new_item));
	}
	else {
		node_map[parent_id].item->removeChild(new_item);
	}
	node_map.erase(id);
	delete new_item;

	std::cerr << "Error: Database operation failed, changes rolled back.\n";
	return;
}

void TreeStructure::remove_node_from_tree(int id) {

	if (id == root_id) {
		std::cerr << "You can't delete root item.\n";
		return;
	}

	auto it = node_map.find(id);
	if (it == node_map.end()) {
		std::cerr << "Error: Node with ID " << id << " does not exist.\n";
		return;
	}

	TreeNode& node_to_remove = it->second;
	QTreeWidgetItem* item_to_remove = node_to_remove.item;

	// Store children for potential reattachment
	std::vector<QTreeWidgetItem*> children;
	while (item_to_remove->childCount() > 0) {
		QTreeWidgetItem* child = item_to_remove->takeChild(0);
		children.push_back(child);
	}

	// Attempt database operation
	if (!db_handler->delete_element(id)) {
		// Rollback: Reattach children if database operation fails
		for (auto* child : children) {
			item_to_remove->addChild(child);
		}
		std::cerr << "Error: Database operation failed. No changes were made to the tree.\n";
		return;
	}

	// Database operation succeeded, proceed with removing the node
	QTreeWidgetItem* parent_item = item_to_remove->parent();
	if (parent_item) {
		// Re-parent the children to the parent_item if it exists
		for (auto* child : children) {
			parent_item->addChild(child);
		}
	}
	else {
		// Re-parent the children to the root if there's no parent
		for (auto* child : children) {
			tree_root->addTopLevelItem(child);
		}
	}

	// Now it's safe to remove the node from the map and delete the item
	node_map.erase(it);
	delete item_to_remove;
}

void TreeStructure::edit_tree_node_name(int id, const std::string& new_name) {
	
	if (id == root_id) {
		std::cerr << "You can't edit root item.\n";
		return;
	}

	auto node_it = node_map.find(id);
	if (node_it == node_map.end()) {
		std::cerr << "Error: Node with ID " << id << " does not exist.\n";
		return;
	}

	// Store the old name for potential rollback
	std::string old_name = node_it->second.name;

	// Update the node name in the tree
	node_it->second.item->setText(1, QString::fromStdString(new_name));

	// Attempt to update the database and rollback if it fails
	if (!db_handler->edit_element_name(id, new_name)) {
		node_it->second.item->setText(1, QString::fromStdString(old_name));
		std::cerr << "Error: Database operation failed. Node name reverted to original.\n";
		return;
	}

	// Update the node name in the map
	node_it->second.name = new_name;
}

void TreeStructure::edit_tree_node_id(int id, int new_id) {

	if (id == root_id) {
		std::cerr << "You can't edit root item.\n";
		return;
	}

	if (id == new_id) return;

	auto node_it = node_map.find(id);
	if (node_it == node_map.end()) {
		std::cerr << "Error: Node with ID " << id << " does not exist.\n";
		return;
	}

	if (node_map.find(new_id) != node_map.end()) {
		std::cerr << "Error: Node with new ID " << new_id << " already exists.\n";
		return;
	}

	// Update the node ID in the tree
	node_it->second.item->setText(0, QString::number(new_id));

	// Attempt to update the database and rollback if it fails
	if (!db_handler->edit_element_id(id, new_id)) {
		node_it->second.item->setText(0, QString::number(id));
		std::cerr << "Error: Database operation failed. Node ID reverted to original.\n";
		return;
	}

	// Update the node ID in the map
	TreeNode node = std::move(node_it->second);
	node_map.erase(node_it);
	node_map[new_id] = std::move(node);
}
