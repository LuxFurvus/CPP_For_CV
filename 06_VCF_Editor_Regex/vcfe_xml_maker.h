
#pragma once
#ifndef VCFE_XML_MAKER_H
#define VCFE_XML_MAKER_H

#include <iostream>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>
#include "vcfe_data_containers.h"
#include "vcfe_serializer.hpp"

class XmlMakerBoost : public IBaseSerializer, public PropertyTreeSerializer {
public:

	inline void print_to_file(const std::string& filename) const noexcept override {
		std::ofstream output_file(filename);
		if (!output_file.is_open()) return;
		boost::property_tree::xml_writer_settings<std::string> settings(' ', 2);
		boost::property_tree::write_xml(output_file, card_tree, settings);
		output_file.close();
	}
	inline void print_to_console() const noexcept override {
		boost::property_tree::xml_writer_settings<std::string> settings(' ', 2);
		boost::property_tree::write_xml(std::cout, card_tree, settings);
	}
};

#endif // VCFE_XML_MAKER_H
