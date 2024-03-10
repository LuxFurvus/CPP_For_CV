#define NOUSE
#ifdef NOUSE

#include <iostream>
#include <string>

#include "vcfe_serializer.h"
#include "vcfe_db_settings.hpp"
#include "vcfe_pgdb_handler.hpp"

#ifdef OOOOOO
#endif

int main() {
	system("chcp 65001 >NUL");

	DB_ConnectSettings set;
	set.parse_settings("DBSettings.txt");

	PG_DB_Handler pdb(set.get_settings(true));

	QueryTask task_drop_tables{ "DropTables.sql" , "All tables dropped!" };
	pdb.execute_query_from_file(task_drop_tables);
	
	QueryTask task_make_tables{ "CreateTables.sql" , "All tables created!" };
	pdb.execute_query_from_file(task_make_tables);

	const std::string input_file{ "AAA.vcf" };

	ContactSerializer cas;
	cas.parse_file(input_file);

	pdb.put_contacts_in_db(cas.get_all_contacts());




	return 0;
}

#endif // NOUSE