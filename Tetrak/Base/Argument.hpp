#ifndef TETRAK_ARGUMENT_HPP
#define TETRAK_ARGUMENT_HPP

namespace Tetrak {

class TetrakArg {
public:
	enum class CommandMarker {
		Separator,
		Parameters,
		Input,
		Process,
		EventExtract,
		TempoFind,
		Output
	};
	
	enum class Type {
		None,
		Int,
		Float,
		Double,
		String,
		Marker,
	} type_;
	
	// Dummy class
	class None {};
	
private:
	union {
		int int_;
		float float_;
		double double_;
		CommandMarker marker_;
		None none_;
	};
	// Using it within the union is a hassle, even for C++11
	std::string string_;

public:
	TetrakArg();
	
	TetrakArg(const TetrakArg& rhs);
	
	template <typename T>
	TetrakArg(const T& arg);
	
	TetrakArg& operator=(const TetrakArg& rhs);
	
	template <typename T>
	void set(const T& arg);
	
	template <typename T>
	T get() const;
	
	template <typename T>
	const T& convert();
	
	Type getType() const {
		return type_;
	}
};

template <> void TetrakArg::set<int>(const int& arg);
template <> void TetrakArg::set<float>(const float& arg);
template <> void TetrakArg::set<double>(const double& arg);
template <> void TetrakArg::set<std::string>(const std::string& arg);
template <> void TetrakArg::set<const char*>(const char* const& arg);
template <>
void TetrakArg::set<TetrakArg::CommandMarker>(const TetrakArg::CommandMarker& arg);

template <> int TetrakArg::get<int>() const;
template <> float TetrakArg::get<float>() const;
template <> double TetrakArg::get<double>() const;
template <> std::string TetrakArg::get<std::string>() const;
template <> const char* TetrakArg::get<const char*>() const;
template <>
TetrakArg::CommandMarker TetrakArg::get<TetrakArg::CommandMarker>() const;

} 

#endif /* TETRAK_ARGUMENT_HPP */
