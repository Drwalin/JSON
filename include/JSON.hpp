/*
 *  This file is one-file JSON implementation using stl.
 *  Copyright (C) 2021 Marek Zalewski aka Drwalin
 *
 *  This is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#ifndef JSON_HPP
#define JSON_HPP

#include <unordered_map>
#include <string>
#include <vector>
#include <iostream>
#include <mutex>
#include <list>
#include <algorithm>

#include <cstdlib>
#include <cinttypes>

std::istream& operator>>(std::istream& in, class JSON& json);
std::ostream& operator<<(std::ostream& out, const class JSON& json);

class JSON {
public:
	
	using string_t = std::string;
	using array_t = std::vector<JSON>;
	using object_t = std::unordered_map<std::string, JSON>;
	using integer_t = int64_t;
	using real_t = double;
	using boolean_t = bool;
	
	enum Type {
		NONE=0,
		STRING=1,
		ARRAY=2,
		OBJECT=3,
		INTEGER=4,
		REAL=5,
		BOOLEAN=6
	};
	
	JSON() : type(NONE) {}
	JSON(const string_t& value) : string(new string_t(value)), type(STRING) {}
	JSON(string_t&& value) : string(new string_t(value)), type(STRING) {}
	JSON(const char* value) : string(new string_t(value)), type(STRING) {}
	JSON(int64_t value) : integer(value), type(INTEGER) {}
	JSON(int32_t value) : integer(value), type(INTEGER) {}
	JSON(int16_t value) : integer(value), type(INTEGER) {}
	JSON(int8_t value) : integer(value), type(INTEGER) {}
	JSON(uint64_t value) : integer(value), type(INTEGER) {}
	JSON(uint32_t value) : integer(value), type(INTEGER) {}
	JSON(uint16_t value) : integer(value), type(INTEGER) {}
	JSON(uint8_t value) : integer(value), type(INTEGER) {}
	JSON(double value) : real(value), type(REAL) {}
	JSON(float value) : real(value), type(REAL) {}
	JSON(long double value) : real(value), type(REAL) {}
	JSON(boolean_t value) : boolean(value), type(BOOLEAN) {}
	JSON(const array_t& value) : array(new array_t(value)), type(ARRAY) {}
	JSON(std::istream& in) {Read(in);}
	template<typename T>
	JSON(T* ptr, size_t count);
	template<class Iterator>
	JSON(const Iterator& begin, const Iterator& end);
	
	JSON(const JSON& other);
	JSON(JSON&& other);
	
	~JSON();
	
	inline JSON& operator=(const string_t& value) {return this->operator=(JSON(value));}
	inline JSON& operator=(string_t&& value) {return this->operator=(JSON(value));}
	inline JSON& operator=(const char* value) {return this->operator=(JSON(value));}
	inline JSON& operator=(int64_t value) {return this->operator=(JSON(value));}
	inline JSON& operator=(int32_t value) {return this->operator=(JSON(value));}
	inline JSON& operator=(int16_t value) {return this->operator=(JSON(value));}
	inline JSON& operator=(int8_t value) {return this->operator=(JSON(value));}
	inline JSON& operator=(uint64_t value) {return this->operator=(JSON(value));}
	inline JSON& operator=(uint32_t value) {return this->operator=(JSON(value));}
	inline JSON& operator=(uint16_t value) {return this->operator=(JSON(value));}
	inline JSON& operator=(uint8_t value) {return this->operator=(JSON(value));}
	inline JSON& operator=(double value) {return this->operator=(JSON(value));}
	inline JSON& operator=(float value) {return this->operator=(JSON(value));}
	inline JSON& operator=(long double value) {return this->operator=(JSON(value));}
	inline JSON& operator=(boolean_t value) {return this->operator=(JSON(value));}
	inline JSON& operator=(const array_t& value) {return this->operator=(JSON(value));}
	JSON& operator=(JSON&& other);
	JSON& operator=(const JSON& other);
	
	void Init(Type newType);
	
	inline void AssureType(Type newType);
	
	template<typename... Args>
	inline void InitString(Args... args);
	template<typename... Args>
	inline void InitObject(Args... args);
	template<typename... Args>
	inline void InitArray(Args... args);
	inline void InitIntegr(integer_t value) {Destroy(); integer=value; type=INTEGER;}
	inline void InitReal(real_t value) {Destroy(); real=value; type=REAL;}
	inline void InitBoolean(boolean_t value) {Destroy(); boolean=value; type=BOOLEAN;}
	
	JSON Convert(Type _type);

	inline bool IsObject() const {return type == OBJECT;}
	inline bool IsArray() const {return type == ARRAY;}
	inline bool IsString() const {return type == STRING;}
	inline bool IsInteger() const {return type == INTEGER;}
	inline bool IsReal() const {return type == REAL;}
	inline bool IsBoolean() const {return type == BOOLEAN;}
	
	inline const JSON& operator[](const char* key) const;
	inline const JSON& operator[](const std::string& key) const;
	inline const JSON& operator[](size_t id) const;
	inline const JSON& operator[](int id) const {return (*this)[(size_t)id];}
	
	inline operator const array_t&() const {return GetArray();}
	inline operator const object_t&() const {return GetObject();}
	inline operator const string_t() const {return GetString();}
	inline operator const long double() const {return GetReal();}
	inline operator const double() const {return GetReal();}
	inline operator const float() const {return GetReal();}
	inline operator const boolean_t() const {return GetBoolean();}
	inline operator const int8_t() const {return GetInteger();}
	inline operator const int16_t() const {return GetInteger();}
	inline operator const int32_t() const {return GetInteger();}
	inline operator const int64_t() const {return GetInteger();}
	inline operator const uint8_t() const {return GetInteger();}
	inline operator const uint16_t() const {return GetInteger();}
	inline operator const uint32_t() const {return GetInteger();}
	inline operator const uint64_t() const {return GetInteger();}
	
	inline const array_t& Array() const {return GetArray();}
	inline const object_t& Object() const {return GetObject();}
	inline const string_t String() const {return GetString();}
	inline const real_t Real() const {return GetReal();}
	inline const integer_t Integer() const {return GetInteger();}
	inline const boolean_t Boolean() const {return GetBoolean();}
	
	inline const string_t GetString() const;
	inline const array_t& GetArray() const;
	inline const object_t& GetObject() const;
	inline const real_t GetReal() const;
	inline const integer_t GetInteger() const;
	inline const boolean_t GetBoolean() const;
	
	inline JSON& operator[](const char* key);
	inline JSON& operator[](const std::string& key);
	inline JSON& operator[](size_t id);
	inline JSON& operator[](int id) {return (*this)[(size_t)id];}
	
	inline operator array_t&() {return AccessArray();}
	inline operator object_t&() {return AccessObject();}
	inline operator string_t&() {return AccessString();}
	inline operator real_t&() {return AccessReal();}
	inline operator boolean_t&() {return AccessBoolean();}
	inline operator integer_t&() {return AccessInteger();}
	
	inline array_t& Array() {return AccessArray();}
	inline object_t& Object() {return AccessObject();}
	inline string_t& String() {return AccessString();}
	inline real_t& Real() {return AccessReal();}
	inline integer_t& Integer() {return AccessInteger();}
	inline boolean_t& Boolean() {return AccessBoolean();}
	
	inline array_t& AccessArray() {AssureType(ARRAY); return *array;}
	inline object_t& AccessObject() {AssureType(OBJECT); return *object;}
	inline string_t& AccessString() {AssureType(STRING); return *string;}
	inline real_t& AccessReal() {AssureType(REAL); return real;}
	inline integer_t& AccessInteger() {AssureType(INTEGER); return integer;}
	inline boolean_t& AccessBoolean() {AssureType(BOOLEAN); return boolean;}

	inline size_t size() const;
	
	void Clear();
	
	std::string Write() const;
	void Write(std::ostream& out) const;

	void WriteCompact(std::ostream& out) const;
	void WriteBeautyfull(std::ostream& out,
		const std::string& indent="\t",
		const std::string& newline="\n",
		size_t depth=0) const;
	static void WriteString(std::ostream& out, const std::string& str);

	void Read(const std::string& str);
	void Read(std::istream& in);
	static std::string GetUntilEndVar(std::istream& in);
	static std::string GetNoQuoteStringUntilEndVar(std::istream& in);
	static void ClearUntilEndVar(std::istream& in);
	static void ReadWs(std::istream& in);
	static std::string ReadString(std::istream& in);
	
	
	
	static void Error(const std::string& str);
	static size_t ErrorsCount();
	static std::string Error();
	
private:
	
	static std::list<std::string>& Errors();
	static std::mutex& Mutex();
	
	void Destroy();
	
	union {
		object_t  *object;
		array_t   *array;
		string_t  *string;
		integer_t integer;
		real_t    real;
		boolean_t boolean;
	};
	Type type;
};

#include "JSON_inc.hpp"

#endif

