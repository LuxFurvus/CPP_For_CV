#pragma once
#ifndef VCFE_VCF_PRINTER_H
#define VCFE_VCF_PRINTER_H

#include <fstream>
#include <functional>
#include "vcfe_data_containers.hpp"

class VcfPrinter {
private:

	bool check_for_non_ascii(
		const std::initializer_list<std::reference_wrapper<const std::string>>& strings) const;

	void print_vcf_name(const NameRecord& names, std::ofstream& ss) const;
	void print_vcf_phonetics(const PhoneticRecord& phonetics, std::ofstream& ss) const;
	void print_vcf_nickname(const NickNameRecord& nickname, std::ofstream& ss) const;
	void print_vcf_telephones(const std::vector<TelephoneRecord>& telephones, std::ofstream& ss) const;
	void print_vcf_email(const std::vector<EmailRecord>& emails, std::ofstream& ss) const;
	void print_vcf_address(const std::vector<AddressRecord>& addresses, std::ofstream& ss) const;
	void print_vcf_company(const WorkInfoRecord& work, std::ofstream& ss) const;
	void print_vcf_url(const std::vector<UrlRecord>& urls, std::ofstream& ss) const;
	void print_vcf_note(const NoteRecord& note, std::ofstream& ss) const;
	void print_vcf_event(const std::vector<EventRecord>& events, std::ofstream& ss) const;
	void print_vcf_socials(const std::vector<SocialNetRecord>& socials, std::ofstream& ss) const;
	void print_vcf_relations(const std::vector<RelationRecord>& relations, std::ofstream& ss) const;
public:
	void print_to_file(const std::vector<ContactData>& cards, const std::string& vcf_name) const;
	void print_to_console(const std::vector<ContactData>& cards) const;
};



#endif // VCFE_VCF_PRINTER_H
