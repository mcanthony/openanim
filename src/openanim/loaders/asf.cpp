#include "asf.h"

#include <fstream>
#include <cctype>
#include <iostream>

#include <boost/lexical_cast.hpp>
#include <boost/regex.hpp>
#include <boost/algorithm/string.hpp> 

#include "Tokenizer.h"
#include "Tokenizer.h"

using std::cout;
using std::endl;

namespace openanim {

namespace {

	class AsfTokenizer : public Tokenizer {
		private:
			State start, keyword, comment;

		public:
			AsfTokenizer(std::istream& in) : Tokenizer(in), start(this), keyword(this), comment(this) {
				// start parsing in "start" state
				start.setActive();

				start = [this](char c) {
					// don't read any space-like characters
					if(std::isspace(c)) 
						reject();

					// comments start with #
					else if(c == '#')
						comment.setActive();

					// brackets are handled separately (no need for a state)
					else if(c == '(' || c == ')') {
						accept(c);
						emit();
					}

					// everything else is a "keyword"
					else
						keyword.setActive();
				};

				comment = [this](char c) {
					// endline character ends comment
					if(c == '\n')
						start.setActive();

					// reject everything
					reject();
				};

				keyword = [this](char c) {
					// space or bracket characters end a keyword
					if(std::isspace(c) || c == '(' || c == ')') {
						emit();
						start.setActive();
					}
					// otherwise just parse another character of the keyword
					else
						accept(c);
				};

				// read the first keyword into current
				next();
			}
	};

///////////////////

	struct Joint {
		std::string name;
		Imath::V3f position, axis;
		int parent = -1;

		Attributes attrs;
	};


	void readUnits(AsfTokenizer& tokenizer) {
		auto token = tokenizer.next();

		// for now just check that the file is ok, but don't try to do
		// anything with the unit values (this might change)
		while(token.value == "mass" || token.value == "length" || token.value == "angle") {
			// skip value
			tokenizer.next();

			// read next token
			token = tokenizer.next();
		}
	}

	void readDocumentation(AsfTokenizer& tokenizer) {
		tokenizer.next();

		// skip until next keyword
		while(!tokenizer.current().value.empty() && tokenizer.current().value[0] != ':')
			tokenizer.next();
	}

	Imath::V3f readVec3(AsfTokenizer& tokenizer) {
		return Imath::V3f(
			boost::lexical_cast<float>(tokenizer.next().value),
			boost::lexical_cast<float>(tokenizer.next().value),
			boost::lexical_cast<float>(tokenizer.next().value)
		);
	}

	void readRoot(AsfTokenizer& tokenizer, std::vector<Joint>& bones) {
		if(!bones.empty())
			throw std::runtime_error(":root tag is not placed before bonedata tag");

		// for now, just read whatever is required and skip it
		tokenizer.next();

		// the resulting joint value
		Joint root;
		root.name = "root";

		// initialise the DOF field in joint attributes (might end up empty)
		std::vector<std::string>& dofs = root.attrs["dof"].as<std::vector<std::string>>();

		while(!tokenizer.current().value.empty() && tokenizer.current().value[0] != ':') {
			if(tokenizer.current().value == "order") {
				static const boost::regex value("(T|R)(X|Y|Z)");
				while(boost::regex_match(tokenizer.next().value, value)) {
					std::string dof = tokenizer.current().value;
					boost::algorithm::to_lower(dof);

					dofs.push_back(dof);
				}
			}

			else if(tokenizer.current().value == "axis") {
				if(tokenizer.next().value != "XYZ")
					throw("unknown axis value in :root - " + tokenizer.current().value);
				tokenizer.next();
			}

			else if(tokenizer.current().value == "position") {
				root.position = readVec3(tokenizer);
				tokenizer.next();
			}

			else if(tokenizer.current().value == "orientation") {
				root.axis = readVec3(tokenizer);
				tokenizer.next();
			}

			else
				throw std::runtime_error("unknown value in :root section - " + tokenizer.current().value);
		}

		// assuming we've read the root bone data - make the bone
		//   ignoring orientation for now
		bones.push_back(root);
	}

	std::pair<Joint, unsigned> readJoint(AsfTokenizer& tokenizer) {
		// the result - Joint and its ID
		std::pair<Joint, unsigned>	result;
		// initialise the DOF field in joint attributes (might end up empty)
		std::vector<std::string>& dofs = result.first.attrs["dof"].as<std::vector<std::string>>();

		// skip the begin token
		tokenizer.next();

		while(true) {
			if(tokenizer.current().value == "end")
				break;

			else if(tokenizer.current().value == "id") {
				result.second = boost::lexical_cast<unsigned>(tokenizer.next().value);
				tokenizer.next();
			}

			else if(tokenizer.current().value == "name") {
				result.first.name = tokenizer.next().value;
				tokenizer.next();
			}

			else if(tokenizer.current().value == "direction") {
				result.first.position = readVec3(tokenizer);
				tokenizer.next();
			}

			else if(tokenizer.current().value == "length") {
				result.first.position *= boost::lexical_cast<float>(tokenizer.next().value);
				tokenizer.next();
			}

			else if(tokenizer.current().value == "axis") {
				result.first.axis = readVec3(tokenizer);
				if(tokenizer.next().value != "XYZ")
					throw std::runtime_error("invalid axis rotation order " + tokenizer.next().value);
				tokenizer.next();
			}

			else if(tokenizer.current().value == "dof") {
				static const boost::regex value("r(x|y|z)");
				while(boost::regex_match(tokenizer.next().value, value))
					dofs.push_back(tokenizer.current().value);
			}

			else if(tokenizer.current().value == "limits") {
				tokenizer.next();

				// read all expressions in brackets
				while(tokenizer.current().value == "(") {
					tokenizer.next();

					// two numbers
					tokenizer.next();
					tokenizer.next();

					// closing bracked
					tokenizer.next();
				}
			}

			else
				throw std::runtime_error("unknown value in joint section - " + tokenizer.current().value);
		}

		// skip the end token
		tokenizer.next();

		return result;
	}

	void readBoneData(AsfTokenizer& tokenizer, std::vector<Joint>& bones) {
		tokenizer.next();

		while(tokenizer.current().value == "begin") {
			std::pair<Joint, unsigned> j = readJoint(tokenizer);
			if(j.second != bones.size())
				throw std::runtime_error("invalid index for bone " + j.first.name);
			bones.push_back(j.first);
		}
	}

	unsigned findIndex(const std::vector<Joint>& bones, const std::string& name) {
		int result = -1;
		for(unsigned a=0;a<bones.size();++a)
			if(bones[a].name == name)
				result = a;

		if(result < 0)
			throw std::runtime_error(":hierarchy refers to bone " + name + " which was not defined in :bonedata section");

		return result;
	}

	void readHierarchy(AsfTokenizer& tokenizer, std::vector<Joint>& bones) {
		if(tokenizer.next().value != "begin")
			throw std::runtime_error(":hierarchy section does not have begin tag");
		tokenizer.next();

		while(tokenizer.current().value != "end") {
			const unsigned line = tokenizer.current().line;

			const unsigned parent = findIndex(bones, tokenizer.current().value);
			tokenizer.next();

			while(tokenizer.current().line == line) {
				bones[findIndex(bones, tokenizer.current().value)].parent = parent;
				tokenizer.next();
			}
		}

		tokenizer.next();
	}

	void readKeyword(AsfTokenizer& tokenizer, std::vector<Joint>& bones) {
		// get next token
		auto token = tokenizer.current();

		// check the version
		if(token.value == ":version") {
			if(tokenizer.next().value != "1.10")
				throw std::runtime_error("only version 1.10 is supported (" + tokenizer.next().value + " found)");
			tokenizer.next();
		}

		// ignore the name
		else if(token.value == ":name") {
			tokenizer.next();
			tokenizer.next();
		}

		// reading the units
		else if(token.value == ":units")
			readUnits(tokenizer);

		// reading the documentation
		else if(token.value == ":documentation")
			readDocumentation(tokenizer);

		else if(token.value == ":root")
			readRoot(tokenizer, bones);

		else if(token.value == ":bonedata")
			readBoneData(tokenizer, bones);

		else if(token.value == ":hierarchy")
			readHierarchy(tokenizer, bones);

		else
			throw std::runtime_error("unknown token '" + token.value + "' on line " + boost::lexical_cast<std::string>(token.line));
	}
}


Skeleton loadASF(const boost::filesystem::path& path) {
	// tokenize the input file
	std::ifstream in(path.string().c_str());

	std::vector<Joint> joints;

	AsfTokenizer tokenizer(in);

	try {
		while(!tokenizer.eof())
			readKeyword(tokenizer, joints);
	}
	catch(std::runtime_error& err) {
		throw std::runtime_error(std::string("Error reading ") + path.string() + " on line " + boost::lexical_cast<std::string>(tokenizer.current().line) + ": " + err.what());
	}

	// and, finally, transfer the information from joints to a Skeleton instance
	Skeleton result;
	result.attributes()["type"] = "asf";

	for(const auto& j : joints) {
		std::size_t index;

		// root is simple
		if(j.parent < 0) {
			result.addRoot(j.name, j.position);
			index = 0;
		}
		
		// non-roots require a search
		else {
			int parentJoint = -1;
			for(unsigned ji=0; ji<result.size(); ++ji)
				if(result[ji].name() == joints[j.parent].name)
					parentJoint = ji;
			assert(parentJoint >= 0);

			index = result.addChild(result[parentJoint], j.position, j.name);
		}

		result[index].attributes() = j.attrs;
	}

	return result;
}


}
