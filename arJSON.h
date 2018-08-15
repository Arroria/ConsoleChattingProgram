#pragma once
#include <vector>
#include <string>
#include <map>

#ifndef OUT
#define OUT
#endif

using arJSONKey = std::string;
class arJSONValue;
class arJSON
{
public:
	inline arJSONValue &		operator[](const std::string& key);
	inline const arJSONValue &	operator[](const std::string& key) const;

	inline auto	begin();
	inline auto	end();
	inline auto	begin() const;
	inline auto	end() const;

	bool ToJSON(OUT std::string& jsonStr) const;

private:
	std::map<arJSONKey, arJSONValue> m_json;
};


class arJSONValue
{
public:
	enum class Type_t
	{
		_NULL,
		_int,
		_float,
		_str,
		_sub,
		_arr,
	};

public:
	arJSONValue();

	inline void operator=(int value);
	inline void operator=(float value);
	inline void operator=(const std::string& value);
	inline void operator=(const arJSON& value);
	inline void Append(const arJSON& value);


	inline Type_t		Type() const;
	inline int			Int() const;
	inline float		Float() const;
	inline std::string	Str() const;

	inline const arJSON&		Sub() const;
	inline const arJSONValue&	operator[](const std::string& key) const;

	inline size_t		ArrSize() const;
	inline arJSON&		operator[](size_t index);
	inline const arJSON&operator[](size_t index) const;
	inline auto			begin();
	inline auto			end();
	inline auto			begin() const;
	inline auto			end() const;

private:
	Type_t m_type;

	int m_int;
	float m_float;
	std::string m_str;
	arJSON m_sub;
	std::vector<arJSON> m_arr;
};
#include "arJSON.inl"



bool JSON_To_arJSON(const std::string& jsonStr, OUT arJSON& json);
