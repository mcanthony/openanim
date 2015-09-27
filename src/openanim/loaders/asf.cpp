#include "asf.h"

#include <fstream>
#include <cctype>
#include <iostream>

#include <boost/lexical_cast.hpp>
#include <boost/regex.hpp>

#include "Tokenizer.h"

using std::cout;
using std::endl;

namespace openanim {

namespace {
	// checks that the next values exactly match the vector, throws an exception otherwise
	void checkValues(std::vector<Token>::const_iterator begin, std::vector<Token>::const_iterator end, const std::vector<std::string>& values, const std::string& error) {
		if((unsigned)(end - begin) < values.size())
			throw std::runtime_error(error + " - incorrect value count.");

		auto i1 = begin;
		auto i2 = values.begin();
		while(i2 != values.end()) {
			if(!boost::regex_match(i1->value, boost::regex(*i2)))
				throw std::runtime_error(error + " - expected '" + *i2 + "', found '" + i1->value + "'");

			++i1;
			++i2;
		}
	}

	Imath::V3f parseV3f(std::vector<Token>::const_iterator begin, std::vector<Token>::const_iterator end, const std::string& error) {
		if(end - begin < 3)
			throw std::runtime_error(error + " - not enough values found.");

		return Imath::V3f(
			boost::lexical_cast<float>(begin->value),
			boost::lexical_cast<float>((begin+1)->value),
			boost::lexical_cast<float>((begin+2)->value)
		);
	}

	struct Joint {
		std::string name;
		Imath::V3f position, orientation, axis;
		int parent = -1;
	};
}

/// this is a silly temporary parser - it should be made much more robust
/// (boost::spirit, maybe?)
Skeleton loadASF(const boost::filesystem::path& path) {
	// tokenize the input file
	std::ifstream in(path.string().c_str());
	std::vector<Token> tokens = tokenize(in);

	// the parsed joints
	std::vector<Joint> joints;

	// now parse all the tokens
	std::size_t cursor = 0;
	while(cursor < tokens.size()) {
		// version should be 1.10
		if(tokens[cursor].value == ":version") {
			if((tokens.size() > cursor+1) && (tokens[cursor+1].value != "1.10"))
				throw std::runtime_error("Incorrect version - only 1.10 is supported.");
			cursor += 2;
		}

		// skip name, units and documentation (not relevant)
		else if((tokens[cursor].value == ":name") || (tokens[cursor].value == ":documentation") || (tokens[cursor].value == ":units")) {
			++cursor;
			while((cursor < tokens.size()) && (tokens[cursor].value[0] != ':'))
				++cursor;
		}
		
		// parsing the root object
		else if(tokens[cursor].value == ":root") {
			++cursor;

			assert(joints.empty());
			joints.push_back(Joint());
			joints.back().name = "root";

			while(cursor < tokens.size() && tokens[cursor].value[0] != ':') {
				if(tokens[cursor].value == "order") {
					checkValues(tokens.begin()+cursor+1, tokens.end(), 
						{"TX", "TY", "TZ", "RX|RZ", "RY", "RZ|RX"},
						"Error parsing root order value");
					cursor += 7;
				}
				else if(tokens[cursor].value == "axis") {
					checkValues(tokens.begin()+cursor+1, tokens.end(), 
						{"XYZ"},
						"Error parsing root axis value");
					cursor += 2;
				}
				else if(tokens[cursor].value == "position") {
					joints.back().position = 
						parseV3f(tokens.begin() + cursor + 1, tokens.end(), 
							"Error parsing root object's position");
					cursor += 4;
				}

				else if(tokens[cursor].value == "orientation") {
					joints.back().orientation = 
						parseV3f(tokens.begin() + cursor + 1, tokens.end(), 
							"Error parsing root object's orientation");
					cursor += 4;
				}

				else
					throw std::runtime_error("Error parsing root object - unknown token '" + tokens[cursor].value + "'.");
			}
		}

		// parsing the joints
		else if(tokens[cursor].value == ":bonedata") {
			++cursor;

			// until the next keyword
			while(cursor < tokens.size() && tokens[cursor].value[0] != ':') {
				// a joint starts with "begin"
				if(tokens[cursor].value != "begin")
					throw std::runtime_error("Error parsing bonedata value - found token '" + tokens[cursor].value + "', 'begin' is expected.");
				++cursor;

				// next, id and joint count should match
				checkValues(tokens.begin()+cursor, tokens.end(), 
					{"id", boost::lexical_cast<std::string>(joints.size())},
					"Error parsing joint ID");
				cursor += 2;

				// name should be next
				if((tokens[cursor].value != "name") || (cursor+2 >= tokens.size()))
					throw std::runtime_error("Error parsing bonedata value - found token '" + tokens[cursor].value + "', 'name <jointname>' is expected.");
				joints.push_back(Joint());
				joints.back().name = tokens[cursor+1].value;
				cursor += 2;

				// direction
				if(tokens[cursor].value != "direction")
					throw std::runtime_error("Error parsing bonedata value - found token '" + tokens[cursor].value + "', 'direction <x> <y> <z>' is expected.");
				joints.back().position = 
					parseV3f(tokens.begin() + cursor + 1, tokens.end(), 
						"Error parsing " + joints.back().name + " 's direction");
				cursor += 4;

				// length
				if((tokens[cursor].value != "length") || (cursor+2 >= tokens.size()))
					throw std::runtime_error("Error parsing bonedata value - found token '" + tokens[cursor].value + "', 'length <val>' is expected.");
				joints.back().position *= boost::lexical_cast<float>(tokens[cursor+1].value);
				cursor += 2;

				// axis
				if(tokens[cursor].value != "axis")
					throw std::runtime_error("Error parsing bonedata value - found token '" + tokens[cursor].value + "', 'axis <x> <y> <z>' is expected.");
				joints.back().axis = 
					parseV3f(tokens.begin() + cursor + 1, tokens.end(), 
						"Error parsing " + joints.back().name + " 's axis");
				cursor += 4;

				// XYZ (part of the axis description)
				if(tokens[cursor].value != "XYZ")
					throw std::runtime_error("Error parsing bonedata value - found token '" + tokens[cursor].value + "', 'XYZ' is expected.");
				++cursor;

				// optional DOF / limits (for now not doing anything with it - will be needed for AMC loading)
				if(tokens[cursor].value == "dof") {
					++cursor;

					std::vector<std::string> dofs;
					while((tokens[cursor].value == "rx") || (tokens[cursor].value == "ry") || (tokens[cursor].value == "rz")) {
						dofs.push_back(tokens[cursor].value);
						++cursor;
					}

					// limits, same count as dofs (ignoring for now)
					if(tokens[cursor].value != "limits")
						throw std::runtime_error("Error parsing bonedata value - found token '" + tokens[cursor].value + "', 'limits' is expected.");
					++cursor;

					for(auto& dof : dofs) {
						if(tokens[cursor].value != "(")
							throw std::runtime_error("Error parsing bonedata value - found token '" + tokens[cursor].value + "', '(' is expected.");
						cursor += 3;
						if(tokens[cursor].value != ")")
							throw std::runtime_error("Error parsing bonedata value - found token '" + tokens[cursor].value + "', ')' is expected.");
						++cursor;
					}
				}

				// and end
				if(tokens[cursor].value != "end")
					throw std::runtime_error("Error parsing bonedata value - found token '" + tokens[cursor].value + "', 'end' is expected.");
				++cursor;
			}
		}

		// and, finally, hierarchy
		else if(tokens[cursor].value == ":hierarchy") {
			++cursor;

			// begin
			if(tokens[cursor].value != "begin")
				throw std::runtime_error("Error parsing hierarchy value - found token '" + tokens[cursor].value + "', 'begin' is expected.");
			++cursor;

			while(tokens[cursor].value != "end") {
				// this is the only part of asf thats line-based, yay
				const unsigned line = tokens[cursor].line;

				// find the parent
				int parent = -1;
				for(int a=0;a<(int)joints.size();++a)
					if(joints[a].name == tokens[cursor].value)
						parent = a;
				if(parent == -1)
					throw std::runtime_error("Error parsing hierarchy value - parent joint '" + tokens[cursor].value + "' not found.");
				++cursor;

				// and the children
				while(tokens[cursor].line == line) {
					int chld = -1;
					for(int a=0;a<(int)joints.size();++a)
						if(joints[a].name == tokens[cursor].value)
							chld = a;
					if(chld == -1)
						throw std::runtime_error("Error parsing hierarchy value - child joint '" + tokens[cursor].value + "' not found.");

					joints[chld].parent = parent;

					++cursor;
				}
			}
			// skip the "end"
			++cursor;
		}

		else
			throw std::runtime_error("Error parsing ASF file - unknown token '" + tokens[cursor].value + "'.");
	}

	// and, finally, transfer the information from joints to a Skeleton instance
	Skeleton result;

	for(const auto& j : joints) {
		// root is simple
		if(j.parent < 0)
			result.addRoot(j.name, j.position);
		
		// non-roots require a search
		else {
			int parentJoint = -1;
			for(unsigned ji=0; ji<result.size(); ++ji)
				if(result[ji].name() == joints[j.parent].name)
					parentJoint = ji;
			assert(parentJoint >= 0);

			result.addChild(result[parentJoint], j.position, j.name);
		}
	}

	return result;
}

}
