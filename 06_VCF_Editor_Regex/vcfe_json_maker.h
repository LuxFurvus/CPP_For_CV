#pragma once
#ifndef VCFE_JSON_MAKER_H
#define VCFE_JSON_MAKER_H

#include <functional>
#include <iostream>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include "vcfe_data_containers.h"
#include "vcfe_serializer.hpp"

class JsonMakerBoost : public IBaseSerializer, public PropertyTreeSerializer {
public:

	inline void print_to_file(const std::string& filename) const noexcept override {
		std::ofstream output_file(filename);
		if (!output_file.is_open()) return;
		boost::property_tree::write_json(output_file, card_tree, true);
		output_file.close();
	}
	inline void print_to_console() const noexcept override {
		boost::property_tree::write_json(std::cout, card_tree, true);
	}

};

#endif // VCFE_JSON_MAKER_H