//vcfe_pgdb_handler.hpp

#include <memory>
#include <string>
#include <fstream>
#include <iostream>
#include <exception>

#include <pqxx/pqxx>

#include "vcfe_data_containers.hpp"

struct QueryTask {
	const std::string filename;
	const std::string success_msg;
};

class PG_DB_Handler {
private:
	std::unique_ptr<pqxx::connection> c_;
	bool is_connected{ false };
private:
	int get_contact_id(pqxx::work& W);
	void safe_run_query(pqxx::work& W, const std::string& query, const char* func);
	void insert_contact_version(pqxx::work& W, const std::string& version);
	void insert_name_record(pqxx::work& W, const NameRecord& names, int contact_id);
	void insert_phonetic_record(pqxx::work& W, const PhoneticRecord& phonetic, int contact_id);
	void insert_nickname_record(pqxx::work& W, const NickNameRecord& nickname, int contact_id);
	void insert_workinfo_record(pqxx::work& W, const WorkInfoRecord& workinfo, int contact_id);
	void insert_note_record(pqxx::work& W, const NoteRecord& note, int contact_id);

	void insert_telephone_records(pqxx::work& W, const std::vector<TelephoneRecord>& telephones, int contact_id);
	void insert_email_records(pqxx::work& W, const std::vector<EmailRecord>& emails, int contact_id);
	void insert_address_records(pqxx::work& W, const std::vector<AddressRecord>& addresses, int contact_id);
	void insert_url_records(pqxx::work& W, const std::vector<UrlRecord>& urls, int contact_id);
	void insert_event_records(pqxx::work& W, const std::vector<EventRecord>& events, int contact_id);
	void insert_social_net_records(pqxx::work& W, const std::vector<SocialNetRecord>& socials, int contact_id);
	void insert_relation_records(pqxx::work& W, const std::vector<RelationRecord>& relations, int contact_id);
public:
	explicit PG_DB_Handler(const std::string& connection_string);

	bool is_db_connected() const { return is_connected; }
	bool execute_query_from_file(const QueryTask& query);
	void put_contacts_in_db(const std::vector<ContactData>& contacts);
};

///////////////////////////////////////////////////////////////////

void PG_DB_Handler::safe_run_query(pqxx::work& W, const std::string& query, const char* func) {
	try {
		W.exec(query);
	}
	catch (const std::exception& e) {
		std::cerr << "Exception in " << func << ": " << e.what() << '\n';
		throw;
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
	safe_run_query(W, query, __FUNCTION__);
}

void PG_DB_Handler::insert_name_record(pqxx::work& W, const NameRecord& names, int contact_id) {
	std::string query = "INSERT INTO names (contact_id, family, personal, father, address_form, suffix) VALUES (" +
		W.quote(contact_id) + ", " +
		W.quote(names.family) + ", " +
		W.quote(names.personal) + ", " +
		W.quote(names.father) + ", " +
		W.quote(names.address_form) + ", " +
		W.quote(names.suffix) + ");";
	safe_run_query(W, query, __FUNCTION__);
}

void PG_DB_Handler::insert_phonetic_record(pqxx::work& W, const PhoneticRecord& phonetic, int contact_id) {
	std::string query = "INSERT INTO phonetics (contact_id, first_name, middle_name, last_name) VALUES (" +
		W.quote(contact_id) + ", " +
		W.quote(phonetic.first) + ", " +
		W.quote(phonetic.middle) + ", " +
		W.quote(phonetic.last) + ");";
	safe_run_query(W, query, __FUNCTION__);
}

void PG_DB_Handler::insert_nickname_record(pqxx::work& W, const NickNameRecord& nickname, int contact_id) {
	std::string query = "INSERT INTO nicknames (contact_id, nick) VALUES (" +
		W.quote(contact_id) + ", " +
		W.quote(nickname.nick) + ");";
	safe_run_query(W, query, __FUNCTION__);
}

void PG_DB_Handler::insert_workinfo_record(pqxx::work& W, const WorkInfoRecord& workinfo, int contact_id) {
	std::string query = "INSERT INTO workinfos (contact_id, company, department, title) VALUES (" +
		W.quote(contact_id) + ", " +
		W.quote(workinfo.company) + ", " +
		W.quote(workinfo.department) + ", " +
		W.quote(workinfo.title) + ");";
	safe_run_query(W, query, __FUNCTION__);
}

void PG_DB_Handler::insert_note_record(pqxx::work& W, const NoteRecord& note, int contact_id) {
	std::string query = "INSERT INTO notes (contact_id, note_text) VALUES (" +
		W.quote(contact_id) + ", " +
		W.quote(note.note_text) + ");";
	safe_run_query(W, query, __FUNCTION__);
}

void PG_DB_Handler::insert_telephone_records(pqxx::work& W, const std::vector<TelephoneRecord>& telephones, int contact_id) {
	for (const auto& telephone : telephones) {
		std::string query = "INSERT INTO telephones (contact_id, tel_type, tel_number) VALUES (" +
			W.quote(contact_id) + ", " +
			W.quote(telephone.type) + ", " +
			W.quote(telephone.number) + ");";
		safe_run_query(W, query, __FUNCTION__);
	}
}

void PG_DB_Handler::insert_email_records(pqxx::work& W, const std::vector<EmailRecord>& emails, int contact_id) {
	for (const auto& email : emails) {
		std::string query = "INSERT INTO emails (contact_id, email_type, address) VALUES (" +
			W.quote(contact_id) + ", " +
			W.quote(email.type) + ", " +
			W.quote(email.address) + ");";
		safe_run_query(W, query, __FUNCTION__);
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
		safe_run_query(W, query, __FUNCTION__);
	}
}

void PG_DB_Handler::insert_url_records(pqxx::work& W, const std::vector<UrlRecord>& urls, int contact_id) {
	for (const auto& url : urls) {
		std::string query = "INSERT INTO urls (contact_id, url_address) VALUES (" +
			W.quote(contact_id) + ", " +
			W.quote(url.url_address) + ");";
		safe_run_query(W, query, __FUNCTION__);
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
		safe_run_query(W, query, __FUNCTION__);
	}
}

void PG_DB_Handler::insert_social_net_records(pqxx::work& W, const std::vector<SocialNetRecord>& socials, int contact_id) {
	for (const auto& social : socials) {
		std::string query = "INSERT INTO socialnets (contact_id, sns_name, sns_contact) VALUES (" +
			W.quote(contact_id) + ", " +
			W.quote(social.name) + ", " +
			W.quote(social.contact) + ");";
		safe_run_query(W, query, __FUNCTION__);
	}
}

void PG_DB_Handler::insert_relation_records(pqxx::work& W, const std::vector<RelationRecord>& relations, int contact_id) {
	for (const auto& relation : relations) {
		std::string query = "INSERT INTO relations (contact_id, rel_name, rel_typename, rel_typenum) VALUES (" +
			W.quote(contact_id) + ", " +
			W.quote(relation.name) + ", " +
			W.quote(relation.type_name) + ", " +
			W.quote(relation.type_num) + ");";
		safe_run_query(W, query, __FUNCTION__);
	}
}