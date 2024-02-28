#pragma once
#ifndef VCFE_VCF_PRINTER_H
#define VCFE_VCF_PRINTER_H

#include <fstream>
#include "vcfe_data_containers.h"

class VcfPrinter {
private:
	void print_vcf_name(const Names& names, std::ofstream& ss) const;
	void print_vcf_phonetics(const PhoneticName& phonetics, std::ofstream& ss) const;
	void print_vcf_nickname(const NickName& nickname, std::ofstream& ss) const;
	void print_vcf_telephones(const std::vector<Telephones>& tels, std::ofstream& ss) const;
	void print_vcf_email(const std::vector<Emails>& emails, std::ofstream& ss) const;
	void print_vcf_address(const std::vector<Addresses>& addresses, std::ofstream& ss) const;
	void print_vcf_company(const WorkInfo& work, std::ofstream& ss) const;
	void print_vcf_url(const std::vector<UrlString>& urls, std::ofstream& ss) const;
	void print_vcf_note(const NoteString& note, std::ofstream& ss) const;
	void print_vcf_event(const std::vector<Event>& events, std::ofstream& ss) const;
	void print_vcf_socials(const std::vector<SocialNet>& socials, std::ofstream& ss) const;
	void print_vcf_relations(const std::vector<Relation>& relations, std::ofstream& ss) const;
public:
	void print_to_file(const std::vector<ContactData>& cards, const std::string& vcf_name) const;
	void print_to_console(const std::vector<ContactData>& cards) const;
};



#endif // VCFE_VCF_PRINTER_H
