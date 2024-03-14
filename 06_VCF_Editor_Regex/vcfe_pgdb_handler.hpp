
#ifndef VCFE_PGDB_HANDLER_HPP
#define VCFE_PGDB_HANDLER_HPP

#include <memory>

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

///////////////////////////////////////////////////////////////////
private:
	// Helper functions to put data into database
	int get_contact_id     (pqxx::work& W);
	void saferun_put_query (pqxx::work& W, const std::string& query, const char* func);

	void insert_contact_version    (pqxx::work& W, const std::string& version);
	void insert_name_record        (pqxx::work& W, const NameRecord& names, int contact_id);
	void insert_phonetic_record    (pqxx::work& W, const PhoneticRecord& phonetic, int contact_id);
	void insert_nickname_record    (pqxx::work& W, const NickNameRecord& nickname, int contact_id);
	void insert_workinfo_record    (pqxx::work& W, const WorkInfoRecord& workinfo, int contact_id);
	void insert_note_record        (pqxx::work& W, const NoteRecord& note, int contact_id);

	void insert_telephone_records  (pqxx::work& W, const std::vector<TelephoneRecord>& telephones, int contact_id);
	void insert_email_records      (pqxx::work& W, const std::vector<EmailRecord>& emails, int contact_id);
	void insert_address_records    (pqxx::work& W, const std::vector<AddressRecord>& addresses, int contact_id);
	void insert_url_records        (pqxx::work& W, const std::vector<UrlRecord>& urls, int contact_id);
	void insert_event_records      (pqxx::work& W, const std::vector<EventRecord>& events, int contact_id);
	void insert_social_net_records (pqxx::work& W, const std::vector<SocialNetRecord>& socials, int contact_id);
	void insert_relation_records   (pqxx::work& W, const std::vector<RelationRecord>& relations, int contact_id);

///////////////////////////////////////////////////////////////////
private:
	// Helper functions to get data from each table
	void get_name_records       (pqxx::read_transaction& trx, int contact_id, NameRecord& name_records);
	void get_phonetic_record    (pqxx::read_transaction& trx, int contact_id, PhoneticRecord& phonetic_record);
	void get_nickname_record    (pqxx::read_transaction& trx, int contact_id, NickNameRecord& nickname_record);
	void get_workinfo_record    (pqxx::read_transaction& trx, int contact_id, WorkInfoRecord& workinfo_record);
	void get_note_record        (pqxx::read_transaction& trx, int contact_id, NoteRecord& note_record);
	void get_telephone_records  (pqxx::read_transaction& trx, int contact_id, std::vector<TelephoneRecord>& telephone_records);		  
	void get_email_records      (pqxx::read_transaction& trx, int contact_id, std::vector<EmailRecord>& email_records);			  
	void get_address_records    (pqxx::read_transaction& trx, int contact_id, std::vector<AddressRecord>& address_records);		  
	void get_url_records        (pqxx::read_transaction& trx, int contact_id, std::vector<UrlRecord>& url_records);
	void get_event_records      (pqxx::read_transaction& trx, int contact_id, std::vector<EventRecord>& event_records);			  
	void get_social_net_records (pqxx::read_transaction& trx, int contact_id, std::vector<SocialNetRecord>& social_net_records);	  
	void get_relation_records   (pqxx::read_transaction& trx, int contact_id, std::vector<RelationRecord>& relation_records);

	// Utility function to fetch contact IDs
	const std::vector<int> get_all_ids(pqxx::read_transaction& trx);
	void saferun_get_query(pqxx::read_transaction& trx, const char* func, const std::string& query, std::function<void(pqxx::result&)> get_lambda);

///////////////////////////////////////////////////////////////////
public:
	explicit PG_DB_Handler(const std::string& connection_string);

	bool is_db_connected() const { return is_connected; }
	bool execute_query_from_file(const QueryTask& query);
	void put_contacts_in_db(const std::vector<ContactData>& contacts);

	// Main function to populate a vector of ContactData from the database
	void get_contacts_from_db(std::vector<ContactData>& contacts);
};

#endif // VCFE_PGDB_HANDLER_HPP
