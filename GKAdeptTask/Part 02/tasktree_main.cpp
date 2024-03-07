
#include <iostream>

#include <QApplication> 

#include "tasktree_settings.hpp"
#include "tasktree_structure.hpp"
#include "tasktree_pg_handler.hpp"

void test_db_handling(PostgreHandler& dbHandler) {

	std::cout << "\nAdding elements...\n";
	dbHandler.add_element(1, "Node1");
	dbHandler.add_element(2, "Node2");

	std::cout << "\nEditing element name...\n";
	dbHandler.edit_element_name(1, "Node1 Updated");

	std::cout << "\nEditing element ID...\n";
	dbHandler.edit_element_id(1, 2);

	std::cout << "\nGetting all nodes...\n";
	auto nodes = dbHandler.get_all_nodes();
	for (const auto& node : nodes) {
		std::cout << "\tID: " << node.first << ", Name: " << node.second << "\n";
	}

	std::cout << "\nDeleting elements...\n";
	dbHandler.delete_element(1);
	dbHandler.delete_element(2);
	dbHandler.delete_element(3);

	return;
}

void test_tree_handling(DatabaseHandler* dbh) {
	
	TreeStructure ts(dbh);
	printf("\n");

	/// Valid commands
	ts.add_node_to_tree("Node One", 111);
	ts.edit_tree_node_id(111, 1000);
	ts.edit_tree_node_name(1000, "Node Two");
	ts.remove_node_from_tree(1000);

	printf("\n");

	/// Invalid Commands
	ts.add_node_to_tree("This node will not be added", 0);
	ts.edit_tree_node_id(999, 5000);
	ts.edit_tree_node_name(999, "Attempt to edit non-existent node");
	ts.remove_node_from_tree(999);

	ts.edit_tree_node_id(0, 5000);
	ts.edit_tree_node_name(0, "Attempt to edit root node");
	ts.remove_node_from_tree(0);

	return;
}

int main(int argc, char* argv[])
{
	// Settings to read unicode charachers from console
	system("chcp 65001 >NUL");
	setlocale(LC_ALL, "en_US.utf8");

	// Run QT base
	QApplication app(argc, argv);

	// Collect settings from the file
	Settings sets;
	sets.parse_settings("TaskTreeSettings.txt");

	// Use settings to make database handler
	PostgreHandler dbHandler(sets.get_settings(true));

	// Run test functions
	test_db_handling(dbHandler);
	test_tree_handling(&dbHandler);

	// It was just a test. We don't need loop here
	//return app.exec();
	return 0;
}
