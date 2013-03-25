#include "Tetrak/Base/TempoTrackerBase.hpp"

namespace Tetrak {
	
// Constructor
TetrakArg::TetrakArg() {
	type_ = Type::None;
}

TetrakArg::TetrakArg(const TetrakArg& rhs){
	*this = rhs;
}

template <>
TetrakArg::TetrakArg<int>(const int& arg) {
	int_ = arg;
	type_ = Type::Int;
}

template <>
TetrakArg::TetrakArg<float>(const float& arg) {
	float_ = arg;
	type_ = Type::Float;
}

template <>
TetrakArg::TetrakArg<double>(const double& arg) {
	double_ = arg;
	type_ = Type::Double;
}

template <>
TetrakArg::TetrakArg<const char*>(const char* const& arg) {
	type_ = Type::None;
	set(arg);
}

template <>
TetrakArg::TetrakArg<std::string>(const std::string& arg) {
	type_ = Type::None;
	set(arg);
}

template <>
TetrakArg::TetrakArg<TetrakArg::CommandMarker>(const CommandMarker& arg) {
	marker_ = arg;
	type_ = Type::Marker;
}

// Assignment/copy
TetrakArg& TetrakArg::operator=(const TetrakArg& rhs) {
	switch (rhs.getType()) {
		case Type::Int:
			set(rhs.get<int>());
		break;
		case Type::Float:
			set(rhs.get<float>());
		break;
		case Type::Double:
			set(rhs.get<double>());
		break;
		case Type::String:
			set(rhs.get<std::string>());
		break;
		case Type::Marker:
			set(rhs.get<CommandMarker>());
		break;
		default:
			throw TetrakException(TetrakErrorCode::Type);
		break;
	}
	
	return *this;
}

// Set
template <>
void TetrakArg::set<int>(const int& arg) {
	int_ = arg;
	type_ = Type::Int;
}

template <>
void TetrakArg::set<float>(const float& arg) {
	float_ = arg;
	type_ = Type::Float;
}

template <>
void TetrakArg::set<double>(const double& arg) {
	double_ = arg;
	type_ = Type::Double;
}


template <>
void TetrakArg::set<std::string>(const std::string& arg) {
	string_ = arg;
	type_ = Type::String;
}

template <>
void TetrakArg::set<const char*>(const char* const& arg) {
	string_ = arg;
	type_ = Type::String;
}

template <>
void TetrakArg::set<TetrakArg::CommandMarker>(const CommandMarker& arg) {
	marker_ = arg;
	type_ = Type::Marker;
}

// Get
template <>
int TetrakArg::get() const {
	assert(type_ == Type::Int);
	return int_;
}

template <>
float TetrakArg::get() const {
	assert(type_ == Type::Float);
	return float_;
}

template <>
double TetrakArg::get() const {
	assert(type_ == Type::Double);
	return double_;
}

template <>
std::string TetrakArg::get() const {
	assert(type_ == Type::String);
	return std::string(string_);
}

template <>
const char* TetrakArg::get() const {
	assert(type_ == Type::String);
	return string_.c_str();
}

template <>
TetrakArg::CommandMarker TetrakArg::get() const {
	assert(type_ == Type::Marker);
	return marker_;
}

// Convert
template <>
const int& TetrakArg::convert() {
	switch (type_) {
		case Type::Int:
			// No conversion necessary
		break;
		case Type::Float:
			int_ = int(float_);
		break;
		case Type::Double:
			int_ = int(double_);
		break;
		case Type::String:
			int_ = atoi(string_.c_str());
		break;
		default:
			throw TetrakException(TetrakErrorCode::Type,
								  "Unsupported conversion");
		break;
	}
	type_ = Type::Int;
	return int_;
}

template <>
const float& TetrakArg::convert() {
	switch (type_) {
		case Type::Int:
			float_ = float(int_);
		break;
		case Type::Float:
			// No conversion necessary
		break;
		case Type::Double:
			float_ = float(double_);
		break;
		case Type::String:
			float_ = float(atof(string_.c_str()));
		break;
		default:
			throw TetrakException(TetrakErrorCode::Type,
								  "Unsupported conversion");
		break;
	}
	type_ = Type::Float;
	return float_;
}

template <>
const double& TetrakArg::convert() {
	switch (type_) {
		case Type::Int:
			double_ = double(int_);
		break;
		case Type::Float:
			double_ = double(float_);
		break;
		case Type::Double:
			// No conversion necessary
		break;
		case Type::String:
			double_ = atof(string_.c_str());
		break;
		default:
			throw TetrakException(TetrakErrorCode::Type,
								  "Unsupported conversion");
		break;
	}
	type_ = Type::Double;
	return double_;
}

template <>
const TetrakArg::CommandMarker& TetrakArg::convert() {
	if (type_ != Type::String)
		throw TetrakException(TetrakErrorCode::Type,
							  "Unsupported conversion");
	
	if (string_ == ",")
		marker_ = CommandMarker::Separator;
	
	else if (string_ == "param")
		marker_ = CommandMarker::Parameters;
	
	else if (string_ == "input" || string_ == "in")
		marker_ = CommandMarker::Input;
		
	else if (string_ == "process")
		marker_ = CommandMarker::Process;
	
	else if (string_ == "extract")
		marker_ = CommandMarker::EventExtract;
	
	else if (string_ == "find")
		marker_ = CommandMarker::TempoFind;
	
	else if (string_ == "output" || string_ == "out")
		marker_ = CommandMarker::Output;
	
	else
		throw TetrakException(TetrakErrorCode::Command, string_);
	
	type_ = Type::Marker;
	return marker_;
}

template <>
const TetrakArg::None& TetrakArg::convert() {
	type_ = Type::None;
	return none_;
}

}
