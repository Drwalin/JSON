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

#ifndef JSON_INC_HPP
#define JSON_INC_HPP

#include "JSON.hpp"

template<typename T>
JSON::JSON(T* ptr, size_t count) {
	type = ARRAY;
	array = new array_t(ptr, ptr+count);
}

template<class Iterator>
JSON::JSON(const Iterator& begin, const Iterator& end) {
	type = OBJECT;
	object = new object_t(begin, end);
}


template<typename... Args>
inline void JSON::InitString(Args... args) {
	if(type == STRING)
		*string = string_t(args...);
	else {
		Destroy();
		type = STRING;
		string = new string_t(args...);
	}
}

template<typename... Args>
inline void JSON::InitObject(Args... args) {
	if(type == OBJECT)
		*object = object_t(args...);
	else {
		Destroy();
		type = OBJECT;
		object = new object_t(args...);
	}
}

template<typename... Args>
inline void JSON::InitArray(Args... args) {
	if(type == ARRAY)
		*array = array_t(args...);
	else {
		Destroy();
		type = ARRAY;
		array = new array_t(args...);
	}
}


inline const JSON& JSON::operator[](const char* key) const {
	return (*this)[(std::string)key];
}

inline const JSON& JSON::operator[](const std::string& key) const {
	auto o = GetObject();
	auto it = o.find(key);
	if(it == o.end()) {
		const static JSON _json;
		return _json;
	}
	return it->second;
}

inline const JSON& JSON::operator[](size_t id) const {
	auto a = GetArray();
	if(a.size() <= id) {
		const static JSON _json;
		return _json;
	}
	return a[id];
}


inline const JSON::string_t JSON::GetString() const {
	if(type == STRING)
		return *string;
	if(type == REAL)
		return std::to_string(real);
	if(type == INTEGER)
		return std::to_string(integer);
	if(type == BOOLEAN)
		return std::to_string(boolean);
	const static string_t _string;
	return _string;
}

inline const JSON::array_t& JSON::GetArray() const {
	if(type == ARRAY)
		return *array;
	const static array_t _array;
	return _array;
}

inline const JSON::object_t& JSON::GetObject() const {
	if(type == OBJECT)
		return *object;
	const static object_t _object;
	return _object;
}

inline const JSON::real_t JSON::GetReal() const {
	if(type == REAL)
		return real;
	if(type == INTEGER)
		return integer;
	if(type == BOOLEAN)
		return boolean;
	return 0.0;
}

inline const JSON::integer_t JSON::GetInteger() const {
	if(type == INTEGER)
		return integer;
	if(type == REAL)
		return real;
	if(type == BOOLEAN)
		return boolean;
	return 0;
}

inline const JSON::boolean_t JSON::GetBoolean() const {
	if(type == BOOLEAN)
		return boolean;
	if(type == REAL)
		return real;
	if(type == INTEGER)
		return integer;
	return false;
}


inline JSON& JSON::operator[](const char* key) {
	return (*this)[(std::string)key];
}

inline JSON& JSON::operator[](const std::string& key) {
	AssureType(OBJECT);
	return AccessObject()[key];
}

inline JSON& JSON::operator[](size_t id) {
	AssureType(ARRAY);
	if(array->size() <= id)
		array->resize(id+1);
	return array->at(id);
}

#endif

