
#include <string>
#include <fstream>
#include <iostream>
#include <exception>

#include "vcfe_pgdb_handler.hpp"

void PG_DB_Handler::saferun_put_query(pqxx::work& W, const std::string& query, const char* func) {
	try {
		W.exec(query);
	}
	catch (const pqxx::sql_error& e) {
		std::cerr << "SQL error in " << func << ": " << e.what() << '\n';
	}
	catch (const std::exception& e) {
		std::cerr << "Exception in " << func << ": " << e.what() << '\n';
	}
}

bool PG_DB_Handler::execute_query_from_file(const QueryTask& query_task) {
	std::ifstream file(query_task.filename);
	if (!file.is_open()) {
		std::cerr << query_task.filename << " cannot be opened!\n";
		return false;
	}

	// Read the entire content of the file into a single string (query)
	std::string query((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());

	try {
		if (!is_connected || !c_) {
			std::cerr << "Database connection is not established.\n";
			return false;
		}

		// Create a work object to execute the query
		pqxx::work txn(*c_);

		// Execute the query
		txn.exec(query);
		txn.commit(); // Commit the transaction to make sure changes are applied
	}
	catch (const pqxx::sql_error& e) {
		std::cerr << "SQL error while executing query from file: " << e.what() << '\n';
		return false;
	}
	catch (const std::exception& e) {
		std::cerr << "Exception caught while executing query from file: " << e.what() << '\n';
		return false;
	}

	std::cout << query_task.success_msg << "\n";
	return true;
}

///////////////////////////////////////////////////////////////////

PG_DB_Handler::PG_DB_Handler(const std::string& connection_string) {
	try {
		c_ = std::make_unique<pqxx::connection>(connection_string);

		// Try to establish a connection and check if it's open
		if (c_->is_open()) {
			std::cout << "Connected to " << c_->dbname() << "\n";
			is_connected = true;
		}
		else {
			// If the connection is not open, print an error message
			std::cerr << "Failed to connect to " << c_->dbname() << " database\n";
			is_connected = false;
		}
	}
	catch (const pqxx::sql_error& e) {
		// Catch SQL-related errors
		std::cerr << "SQL error in PG_DB_Handler constructor: " << e.what() << '\n';
	}
	catch (const pqxx::broken_connection& e) {
		// Catch broken connection-specific errors
		std::cerr << "Connection error in PG_DB_Handler constructor: " << e.what() << '\n';
	}
	catch (const std::exception& e) {
		// Catch all other standard exceptions
		std::cerr << "Exception caught in PG_DB_Handler constructor: " << e.what() << '\n';
	}
}

///////////////////////////////////////////////////////////////////

void PG_DB_Handler::put_contacts_in_db(const std::vector<ContactData>& contacts) {
	try {
		int contact_id;
		for (auto card : contacts) {
			pqxx::work W(*c_);
			insert_contact_version(W, card.version);
			contact_id = get_contact_id(W);
			if (contact_id < 0) throw std::exception("Can't read contact ID\n");
			insert_name_record(W, card.names, contact_id);
			insert_phonetic_record(W, card.phonetic_name, contact_id);
			insert_nickname_record(W, card.nickname, contact_id);
			insert_note_record(W, card.note, contact_id);
			insert_workinfo_record(W, card.workinfo, contact_id);
			insert_telephone_records(W, card.telephones, contact_id);
			insert_url_records(W, card.urls, contact_id);
			insert_address_records(W, card.addresses, contact_id);
			insert_email_records(W, card.emails, contact_id);
			insert_event_records(W, card.events, contact_id);
			insert_social_net_records(W, card.socials, contact_id);
			insert_relation_records(W, card.relations, contact_id);
			W.commit();
		}
	}
	catch (const std::exception& e) {
		std::cerr << "Exception in " << __FUNCTION__ << ": " << e.what() << '\n';
		std::cerr << "All changes are not commited! \n";
		return;
	}
}

int PG_DB_Handler::get_contact_id(pqxx::work& W) {
	const std::string query = "SELECT contact_id FROM contacts ORDER BY contact_id DESC LIMIT 1;";
	try {
		pqxx::result r = W.exec(query);
		if (!r.empty()) {
			return r[0][0].as<int>();
		}
	}
	catch (const std::exception& e) {
		std::cerr << "Exception in " __FUNCTION__ ": " << e.what() << '\n';
		throw;
	}
	return -1;
}

void PG_DB_Handler::insert_contact_version(pqxx::work& W, const std::string& version) {
	const std::string query = "INSERT INTO contacts (version) VALUES (" + W.quote(version) + ");";
	saferun_put_query(W, query, __FUNCTION__);
}

void PG_DB_Handler::insert_name_record(pqxx::work& W, const NameRecord& names, int contact_id) {
	std::string query = "INSERT INTO names (contact_id, family, personal, father, address_form, suffix) VALUES (" +
		W.quote(contact_id) + ", " +
		W.quote(names.family) + ", " +
		W.quote(names.personal) + ", " +
		W.quote(names.father) + ", " +
		W.quote(names.address_form) + ", " +
		W.quote(names.suffix) + ");";
	saferun_put_query(W, query, __FUNCTION__);
}

void PG_DB_Handler::insert_phonetic_record(pqxx::work& W, const PhoneticRecord& phonetic, int contact_id) {
	std::string query = "INSERT INTO phonetics (contact_id, first_name, middle_name, last_name) VALUES (" +
		W.quote(contact_id) + ", " +
		W.quote(phonetic.first) + ", " +
		W.quote(phonetic.middle) + ", " +
		W.quote(phonetic.last) + ");";
	saferun_put_query(W, query, __FUNCTION__);
}

void PG_DB_Handler::insert_nickname_record(pqxx::work& W, const NickNameRecord& nickname, int contact_id) {
	std::string query = "INSERT INTO nicknames (contact_id, nick) VALUES (" +
		W.quote(contact_id) + ", " +
		W.quote(nickname.nick) + ");";
	saferun_put_query(W, query, __FUNCTION__);
}

void PG_DB_Handler::insert_workinfo_record(pqxx::work& W, const WorkInfoRecord& workinfo, int contact_id) {
	std::string query = "INSERT INTO workinfos (contact_id, company, department, title) VALUES (" +
		W.quote(contact_id) + ", " +
		W.quote(workinfo.company) + ", " +
		W.quote(workinfo.department) + ", " +
		W.quote(workinfo.title) + ");";
	saferun_put_query(W, query, __FUNCTION__);
}

void PG_DB_Handler::insert_note_record(pqxx::work& W, const NoteRecord& note, int contact_id) {
	std::string query = "INSERT INTO notes (contact_id, note_text) VALUES (" +
		W.quote(contact_id) + ", " +
		W.quote(note.note_text) + ");";
	saferun_put_query(W, query, __FUNCTION__);
}

void PG_DB_Handler::insert_telephone_records(pqxx::work& W, const std::vector<TelephoneRecord>& telephones, int contact_id) {
	for (const auto& telephone : telephones) {
		std::string query = "INSERT INTO telephones (contact_id, tel_type, tel_number) VALUES (" +
			W.quote(contact_id) + ", " +
			W.quote(telephone.type) + ", " +
			W.quote(telephone.number) + ");";
		saferun_put_query(W, query, __FUNCTION__);
	}
}

void PG_DB_Handler::insert_email_records(pqxx::work& W, const std::vector<EmailRecord>& emails, int contact_id) {
	for (const auto& email : emails) {
		std::string query = "INSERT INTO emails (contact_id, email_type, address) VALUES (" +
			W.quote(contact_id) + ", " +
			W.quote(email.type) + ", " +
			W.quote(email.address) + ");";
		saferun_put_query(W, query, __FUNCTION__);
	}
}

void PG_DB_Handler::insert_address_records(pqxx::work& W, const std::vector<AddressRecord>& addresses, int contact_id) {
	for (const auto& address : addresses) {
		std::string query = "INSERT INTO addresses (contact_id, address_type, street, city, region, mail_index, country) VALUES (" +
			W.quote(contact_id) + ", " +
			W.quote(address.type) + ", " +
			W.quote(address.street) + ", " +
			W.quote(address.city) + ", " +
			W.quote(address.region) + ", " +
			W.quote(address.index) + ", " +
			W.quote(address.country) + ");";
		saferun_put_query(W, query, __FUNCTION__);
	}
}

void PG_DB_Handler::insert_url_records(pqxx::work& W, const std::vector<UrlRecord>& urls, int contact_id) {
	for (const auto& url : urls) {
		std::string query = "INSERT INTO urls (contact_id, url_address) VALUES (" +
			W.quote(contact_id) + ", " +
			W.quote(url.url_address) + ");";
		saferun_put_query(W, query, __FUNCTION__);
	}
}

void PG_DB_Handler::insert_event_records(pqxx::work& W, const std::vector<EventRecord>& events, int contact_id) {
	for (const auto& event : events) {
		std::string query = "INSERT INTO events (contact_id, event_name, event_day, event_month, event_year, event_type) VALUES (" +
			W.quote(contact_id) + ", " +
			W.quote(event.event_name) + ", " +
			W.quote(event.day) + ", " +
			W.quote(event.month) + ", " +
			W.quote(event.year) + ", " +
			W.quote(static_cast<int>(event.event_type)) + ");";
		saferun_put_query(W, query, __FUNCTION__);
	}
}

void PG_DB_Handler::insert_social_net_records(pqxx::work& W, const std::vector<SocialNetRecord>& socials, int contact_id) {
	for (const auto& social : socials) {
		std::string query = "INSERT INTO socialnets (contact_id, sns_name, sns_contact) VALUES (" +
			W.quote(contact_id) + ", " +
			W.quote(social.name) + ", " +
			W.quote(social.contact) + ");";
		saferun_put_query(W, query, __FUNCTION__);
	}
}

void PG_DB_Handler::insert_relation_records(pqxx::work& W, const std::vector<RelationRecord>& relations, int contact_id) {
	for (const auto& relation : relations) {
		std::string query = "INSERT INTO relations (contact_id, rel_name, rel_typename, rel_typenum) VALUES (" +
			W.quote(contact_id) + ", " +
			W.quote(relation.name) + ", " +
			W.quote(relation.type_name) + ", " +
			W.quote(relation.type_num) + ");";
		saferun_put_query(W, query, __FUNCTION__);
	}
}

///+///+///+///+///+///+///+///+///+///+///+///+///+///+///+///+///+///+///+///+

const std::vector<int> PG_DB_Handler::get_all_ids(pqxx::read_transaction& trx) {
	std::vector<int> contact_ids;

	if (!is_connected || !c_) {
		std::cerr << "Database connection is not established.\n";
		return contact_ids;  // Return an empty vector if there's no database connection
	}

	pqxx::result R;
	const std::string query = "SELECT contact_id FROM contacts;";

	auto get_ids = [&](pqxx::result& R) {
		R = trx.exec(query);
		for (auto row : R) {
			contact_ids.push_back(row[0].as<int>());
		}
		};

	saferun_get_query(trx, __FUNCTION__, query, get_ids);

	return contact_ids;
}

void PG_DB_Handler::saferun_get_query(pqxx::read_transaction& trx, const char* func, const std::string& query, std::function<void(pqxx::result&)> get_lambda) {
	try {
		pqxx::result r = trx.exec(query);
		if (!r.empty()) {
			get_lambda(r);
		}
	}
	catch (const pqxx::sql_error& e) {
		std::cerr << "SQL error in " << func << ": " << e.what() << '\n';
	}
	catch (const std::exception& e) {
		std::cerr << "Exception in " << func << ": " << e.what() << '\n';
	}
}
void PG_DB_Handler::get_contacts_from_db(std::vector<ContactData>& contacts) {
	pqxx::read_transaction trx(*c_);

	const std::vector<int> contact_ids{ get_all_ids(trx) };

	for (const int& id : contact_ids) {
		ContactData data;
		get_name_records(trx, id, data.names);
		get_phonetic_record(trx, id, data.phonetic_name);
		get_nickname_record(trx, id, data.nickname);
		get_workinfo_record(trx, id, data.workinfo);
		get_note_record(trx, id, data.note);
		get_telephone_records(trx, id, data.telephones);
		get_email_records(trx, id, data.emails);
		get_address_records(trx, id, data.addresses);
		get_url_records(trx, id, data.urls);
		get_event_records(trx, id, data.events);
		get_social_net_records(trx, id, data.socials);
		get_relation_records(trx, id, data.relations);
		contacts.push_back(data);
	}
}

void PG_DB_Handler::get_name_records(pqxx::read_transaction& trx, int contact_id, NameRecord& name_record) {
	const std::string query = "SELECT family, personal, father, address_form, suffix FROM names WHERE contact_id = " + trx.quote(contact_id).append(";");

	auto fill_records = [&](pqxx::result& r) {
		const auto& row = r[0];
		name_record.family = row["family"].c_str();
		name_record.personal = row["personal"].c_str();
		name_record.father = row["father"].c_str();
		name_record.address_form = row["address_form"].c_str();
		name_record.suffix = row["suffix"].c_str();
		};

	saferun_get_query(trx, __FUNCTION__, query, fill_records);
}

void PG_DB_Handler::get_phonetic_record(pqxx::read_transaction& trx, int contact_id, PhoneticRecord& phonetic_record) {
	const std::string query = "SELECT first_name, middle_name, last_name FROM phonetics WHERE contact_id = " + trx.quote(contact_id).append(";");

	auto fill_records = [&](pqxx::result& r) {
		const auto& row = r[0];
		phonetic_record.first = row["first_name"].c_str();
		phonetic_record.middle = row["middle_name"].c_str();
		phonetic_record.last = row["last_name"].c_str();
		};

	saferun_get_query(trx, __FUNCTION__, query, fill_records);
}

void PG_DB_Handler::get_nickname_record(pqxx::read_transaction& trx, int contact_id, NickNameRecord& nickname_record) {
	const std::string query = "SELECT nick FROM nicknames WHERE contact_id = " + trx.quote(contact_id).append(";");

	auto fill_records = [&](pqxx::result& r) {
		const auto& row = r[0];
		nickname_record.nick = row["nick"].c_str();
		};

	saferun_get_query(trx, __FUNCTION__, query, fill_records);
}

void PG_DB_Handler::get_workinfo_record(pqxx::read_transaction& trx, int contact_id, WorkInfoRecord& workinfo_record) {
	const std::string query = "SELECT company, department, title FROM workinfos WHERE contact_id = " + trx.quote(contact_id).append(";");

	auto fill_records = [&](pqxx::result& r) {
		const auto& row = r[0];
		workinfo_record.company = row["company"].c_str();
		workinfo_record.department = row["department"].c_str();
		workinfo_record.title = row["title"].c_str();
		};

	saferun_get_query(trx, __FUNCTION__, query, fill_records);
}

void PG_DB_Handler::get_note_record(pqxx::read_transaction& trx, int contact_id, NoteRecord& note_record) {
	const std::string query = "SELECT note_text FROM notes WHERE contact_id = " + trx.quote(contact_id).append(";");

	auto fill_records = [&](pqxx::result& r) {
		const auto& row = r[0];
		note_record.note_text = row["note_text"].c_str();
		};

	saferun_get_query(trx, __FUNCTION__, query, fill_records);
}

void PG_DB_Handler::get_telephone_records(pqxx::read_transaction& trx, int contact_id, std::vector<TelephoneRecord>& telephone_records) {
	const std::string query = "SELECT tel_type, tel_number FROM telephones WHERE contact_id = " + trx.quote(contact_id).append(";");

	auto fill_records = [&](pqxx::result& r) {
		for (auto row : r) {
			TelephoneRecord record;
			record.type = row["tel_type"].c_str();
			record.number = row["tel_number"].c_str();
			telephone_records.push_back(record);
		}
		};

	saferun_get_query(trx, __FUNCTION__, query, fill_records);
}

void PG_DB_Handler::get_email_records(pqxx::read_transaction& trx, int contact_id, std::vector<EmailRecord>& email_records) {
	const std::string query = "SELECT email_type, address FROM emails WHERE contact_id = " + trx.quote(contact_id).append(";");

	auto fill_records = [&](pqxx::result& r) {
		for (auto row : r) {
			EmailRecord record;
			record.type = row["email_type"].c_str();
			record.address = row["address"].c_str();
			email_records.push_back(record);
		}
		};

	saferun_get_query(trx, __FUNCTION__, query, fill_records);
}

void PG_DB_Handler::get_address_records(pqxx::read_transaction& trx, int contact_id, std::vector<AddressRecord>& address_records) {
	const std::string query = "SELECT address_type, street, city, region, mail_index, country FROM addresses WHERE contact_id = " + trx.quote(contact_id).append(";");

	auto fill_records = [&](pqxx::result& r) {
		for (auto row : r) {
			AddressRecord record;
			record.type = row["address_type"].c_str();
			record.street = row["street"].c_str();
			record.city = row["city"].c_str();
			record.region = row["region"].c_str();
			record.index = row["mail_index"].c_str();
			record.country = row["country"].c_str();
			address_records.push_back(record);
		}
		};

	saferun_get_query(trx, __FUNCTION__, query, fill_records);
}

void PG_DB_Handler::get_url_records(pqxx::read_transaction& trx, int contact_id, std::vector<UrlRecord>& url_records) {
	const std::string query = "SELECT url_address FROM urls WHERE contact_id = " + trx.quote(contact_id).append(";");

	auto fill_records = [&](pqxx::result& r) {
		for (auto row : r) {
			UrlRecord record;
			record.url_address = row["url_address"].c_str();
			url_records.push_back(record);
		}
		};

	saferun_get_query(trx, __FUNCTION__, query, fill_records);
}

void PG_DB_Handler::get_event_records(pqxx::read_transaction& trx, int contact_id, std::vector<EventRecord>& event_records) {
	const std::string query = "SELECT event_name, event_day, event_month, event_year, event_type FROM events WHERE contact_id = " + trx.quote(contact_id).append(";");

	auto fill_records = [&](pqxx::result& r) {
		for (auto row : r) {
			EventRecord record;
			record.event_name = row["event_name"].c_str();
			record.day = row["event_day"].c_str();
			record.month = row["event_month"].c_str();
			record.year = row["event_year"].c_str();
			record.event_type = static_cast<EventType>(row["event_type"].as<int>());
			event_records.push_back(record);
		}
		};

	saferun_get_query(trx, __FUNCTION__, query, fill_records);
}

void PG_DB_Handler::get_social_net_records(pqxx::read_transaction& trx, int contact_id, std::vector<SocialNetRecord>& social_net_records) {
	const std::string query = "SELECT sns_name, sns_contact FROM socialnets WHERE contact_id = " + trx.quote(contact_id).append(";");

	auto fill_records = [&](pqxx::result& r) {
		for (auto row : r) {
			SocialNetRecord record;
			record.name = row["sns_name"].c_str();
			record.contact = row["sns_contact"].c_str();
			social_net_records.push_back(record);
		}
		};

	saferun_get_query(trx, __FUNCTION__, query, fill_records);
}

void PG_DB_Handler::get_relation_records(pqxx::read_transaction& trx, int contact_id, std::vector<RelationRecord>& relation_records) {
	const std::string query = "SELECT rel_name, rel_typename, rel_typenum FROM relations WHERE contact_id = " + trx.quote(contact_id).append(";");

	auto fill_records = [&](pqxx::result& r) {
		for (auto row : r) {
			RelationRecord record;
			record.name = row["rel_name"].c_str();
			record.type_name = row["rel_typename"].c_str();
			record.type_num = row["rel_typenum"].as<int>();
			relation_records.push_back(record);
		}
		};

	saferun_get_query(trx, __FUNCTION__, query, fill_records);
}
