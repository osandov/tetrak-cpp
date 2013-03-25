#ifndef TETRAK_EXCEPTION_HPP
#define TETRAK_EXCEPTION_HPP

namespace Tetrak {

struct TetrakException : public std::exception {
public:
	enum class Enum : unsigned char {
		Exit,
		NotImplemented,
		
		Argument,
		Type,
		
		InvalidFormat,
		FileIO,
		
		Parsing,
		Option,
		Command,
		
		Run,
		
		Fatal_Generic,
		Fatal_OutOfMemory,
		
		Final // Placeholder, do not use
	};
	
	explicit TetrakException(Enum code) throw() : code_(code) {}
	
	TetrakException(Enum errorCode, const char* specificMessage) throw() :
		code_(errorCode), message_(specificMessage)
	{
		message_.insert(0, ": ");
	}
	
	TetrakException(Enum errorCode, const std::string& specificMessage) throw() :
		code_(errorCode), message_(specificMessage)
	{
		message_.insert(0, ": ");
	}
	
	Enum code() const throw() {
		return code_;
	}
	
	const char* what() const throw() {
		return (error_descrip_[static_cast<unsigned char>(code_)] + message_).c_str();
	}
	
private:
	Enum code_;
	std::string message_;
	
	static constexpr unsigned char num_enums_ = static_cast<unsigned char>(Enum::Final);
	
	static constexpr const char* error_descrip_[num_enums_] = {
		"Exited",
		"Implementation error (not implemented)",
		
		"Argument error",
		"Invalid type",
		
		"Invalid format",
		"File I/O error",
		
		"Parsing error",
		"Invalid option",
		"Invalid command",
		
		"Running error",
		
		"Generic fatal error",
		"Fatal out-of-memory error",
	};
};

typedef TetrakException::Enum TetrakErrorCode;
} 

#endif /* TETRAK_EXCEPTION_HPP */
