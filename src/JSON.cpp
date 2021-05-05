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

#include "../include/JSON.hpp"

#include <unordered_map>
#include <string>
#include <vector>
#include <iostream>
#include <mutex>
#include <list>
#include <algorithm>
#include <sstream>

#include <cstdlib>
#include <cinttypes>

JSON::JSON(const JSON& other) {
	type = other.type;
	switch(type) {
		case NONE:    break;
		case STRING:  string = new string_t(*other.string); break;
		case ARRAY:   array = new array_t(*other.array);    break;
		case OBJECT:  object = new object_t(*other.object); break;
		case REAL:    real = other.real;                    break;
		case INTEGER: integer = other.integer;              break;
		case BOOLEAN: boolean = other.boolean;              break;
		default: type=NONE; return;
	}
}

JSON::JSON(JSON&& other) {
	type = other.type;
	switch(type) {
		case NONE:    break;
		case STRING:  string = other.string;   break;
		case ARRAY:   array = other.array;     break;
		case OBJECT:  object = other.object;   break;
		case REAL:    real = other.real;       break;
		case INTEGER: integer = other.integer; break;
		case BOOLEAN: boolean = other.boolean; break;
		default: type=NONE; break;
	}
	other.type = NONE;
}

JSON::~JSON() {
	switch(type) {
		case STRING: delete string; break;
		case OBJECT: delete object; break;
		case ARRAY:  delete  array; break;
		default: break;
	}
	string = NULL;
}

JSON& JSON::operator=(JSON&& other) {
	Destroy();
	type = other.type;
	switch(type) {
		case NONE:    break;
		case STRING:  string = other.string;   break;
		case ARRAY:   array = other.array;     break;
		case OBJECT:  object = other.object;   break;
		case REAL:    real = other.real;       break;
		case INTEGER: integer = other.integer; break;
		case BOOLEAN: boolean = other.boolean; break;
		default: type=NONE; break;
	}
	other.type = NONE;
	return *this;
}

JSON& JSON::operator=(const JSON& other) {
	Destroy();
	type = other.type;
	switch(type) {
		case NONE:    break;
		case STRING:  string = new string_t(*other.string); break;
		case ARRAY:   array = new array_t(*other.array);    break;
		case OBJECT:  object = new object_t(*other.object); break;
		case REAL:    real = other.real;                   break;
		case INTEGER: integer = other.integer;             break;
		case BOOLEAN: boolean = other.boolean;             break;
		default: type=NONE; break;
	}
	return *this;
}

void JSON::Init(Type newType) {
	Destroy();
	type = newType;
	switch(newType) {
		case NONE:    break;
		case STRING:  string = new string_t(); break;
		case ARRAY:   array = new array_t();   break;
		case OBJECT:  object = new object_t(); break;
		case REAL:    real = 0.0;              break;
		case INTEGER: integer = 0;             break;
		case BOOLEAN: boolean = false;         break;
		default: type=NONE; return;
	}
}

JSON JSON::Convert(Type _type) {
	switch(_type) {
		case NONE:
			return JSON();
		case ARRAY:
			if(type == ARRAY)
				return *this;
			break;
		case OBJECT:
			if(type == OBJECT)
				return *this;
			break;
		case STRING:
			switch(type) {
				case STRING:
					return *this;
				case INTEGER:
					return JSON(std::to_string(integer));
				case REAL:
					return JSON(std::to_string(real));
				case BOOLEAN:
					return JSON(std::to_string(boolean));
				default:
					break;
			}
			break;
		case INTEGER:
			switch(type) {
				case INTEGER:
					return *this;
				case REAL:
					return JSON((integer_t)real);
				case BOOLEAN:
					return JSON((integer_t)boolean);
				case STRING:
					return JSON((integer_t)atoll(string->c_str()));
				default:
					break;
			}
			break;
		case REAL:
			switch(type) {
				case REAL:
					return *this;
				case INTEGER:
					return JSON((real_t)real);
				case BOOLEAN:
					return JSON((real_t)boolean);
				case STRING:
					return JSON(atof(string->c_str()));
				default:
					break;
			}
			break;
		case BOOLEAN:
			switch(type) {
				case INTEGER:
					return *this;
				case REAL:
					return JSON((boolean_t)real);
				case BOOLEAN:
					return JSON((boolean_t)boolean);
				case STRING:
					{
						if(atoll(string->c_str())!=0 || atof(string->c_str())!=0.0)
							return JSON(true);
						std::string t = *string;
						for(auto& c : t)
							c = (c>='a'&&c<='z') ? c-('a'-'A') : c;
						return JSON(t=="TRUE" || t=="YES");
					}
				default:
					break;
			}
			break;
				
	}
	return JSON();
}


void JSON::Clear() {
	Destroy();
}


std::string JSON::Write() const {
	std::stringstream ss;
	WriteCompact(ss);
	return ss.str();
}

void JSON::Write(std::ostream& out) const {
	WriteBeautyfull(out);
}

void JSON::WriteCompact(std::ostream& out) const {
	WriteBeautyfull(out, "", "");
}

void JSON::WriteBeautyfull(std::ostream& out,
		const std::string& indent,
		const std::string& newline,
		size_t depth) const {
	bool next = false;
	switch(type) {
		case ARRAY:
		case OBJECT:
			{
				std::string ind = newline;
				for(size_t i=0; i<depth; ++i)
					ind += indent;
				if(type == ARRAY) {
					out << '[';
					for(const auto& p : *array) {
						if(next) out<<','; else next=true;
						out << ind << indent;
						p.WriteBeautyfull(out, indent, newline, depth+1);
					}
					if(next)
						out << ind;
					out << ']';
				} else {
					out << '{';
					for(const auto& p : *object) {
						if(next) out<<','; else next=true;
						out << ind << indent;
						WriteString(out, p.first);
						out << ':';
						p.second.WriteBeautyfull(out, indent, newline, depth+1);
					}
					if(next)
						out << ind;
					out << '}';
				}
			}
			break;
		case STRING:
			WriteString(out, *string);
			break;
		case INTEGER:
			out<<integer;
			break;
		case BOOLEAN:
			out<<(boolean?"true":"false");
			break;
		case REAL:
			out<<real;
			break;
		case NONE:
			break;
		default:
			break;
	}
}

void JSON::WriteString(std::ostream& out, const std::string& str) {
	out << '"';
	for(char c : str) {
		switch(c) {
			case '\t': out<<"\\t"; break;
			case '\n': out<<"\\n"; break;
			case '\r': out<<"\\r"; break;
			case '\'': out<<"\\'"; break;
			case '"': out<<"\\\""; break;
			case '\\': out<<"\\\\";  break;
			case 0:     out<<'"'; return;
			default: out<<c;
		}
	}
	out << '"';
}


void JSON::Read(const std::string& str) {
	std::stringstream ss(str);
	Read(ss);
}

void JSON::Read(std::istream& in) {
	ReadWs(in);
	switch(in.peek()) {
		case '[':	// is array
			in.get();
			AssureType(ARRAY);
			while(!in.eof()) {
				ReadWs(in);
				if(in.peek() == ']') {
					in.get();
					break;
				} else if(in.peek() == ',') {
					array->emplace_back();
					in.get();
				} else {
					if(array->empty())
						array->emplace_back();
					array->back().Read(in);
				}
			}
			break;
		case '{':	// is object
			in.get();
			AssureType(OBJECT);
			while(!in.eof()) {
				ReadWs(in);
				if(in.peek() == '}') {
					in.get();
					break;
				} else if(in.peek() == ',') {
					in.get();
				} else if(in.peek() == '\'' || in.peek() == '\"') {
					std::string key = ReadString(in);
					ReadWs(in);
					if(in.peek() == ':') {
						in.get();
						object->operator[](key).Read(in);
					} else {
						Error("Invalid object value");
					}
				} else {
					Error("Invalid object key");
					std::cerr << *this << "   " << (char)in.peek();
				}
			}
			break;
		case 't':	// is it boolean true
		case 'T':
		case 'y':
		case 'Y':
			{
				std::string value = GetUntilEndVar(in);
				std::string v = value;
				std::transform(v.begin(), v.end(), v.begin(),
						[](unsigned char c){return std::tolower(c);});
				if(v == "true" || v == "yes")
					*this = true;
				else
					*this = value;
			}
			return;
		case 'f':	// is it boolean false
		case 'F':
		case 'n':
		case 'N':
			{
				std::string value = GetUntilEndVar(in);
				std::string v = value;
				std::transform(v.begin(), v.end(), v.begin(),
						[](unsigned char c){return std::tolower(c);});
				if(v == "false" || v == "no")
					*this = false;
				else
					*this = value;
			}
			return;
		case '\'':	// is string
		case '"':
			*this = ReadString(in);
			return;
		default:	// is number or invalid
			{
				std::string value = GetUntilEndVar(in);
				if(value == "") {
					Destroy();
				} else if(std::all_of(value.begin(), value.end(),
							[](char c)->bool{
							return (
								(c>='0' && c<='9') ||
								c=='.' ||
								c=='-' ||
								c=='+' ||
								c=='e' ||
								c=='E');
							})) {	// is it a number
					if(std::any_of(value.begin(), value.end(),
								[](char c)->bool{
								return c=='.'||c=='e'||c=='E';
								})) {		// is real
						*this = atof(value.c_str());
					} else {		// is integer
						*this = (integer_t)atoll(value.c_str());
					}
				} else {	// is string without quotes
					*this = value;
				}
			}
	}
}

std::string JSON::GetUntilEndVar(std::istream& in) {
	std::string str;
	while(!in.eof()) {
		ReadWs(in);
		switch(in.peek()) {
			case ',':
			case ']':
			case '}':
			case EOF:
				return str;
			default:
				str += (char)in.get();
		}
	}
	return str;
}

void JSON::ClearUntilEndVar(std::istream& in) {
	while(!in.eof()) {
		ReadWs(in);
		switch(in.peek()) {
			case ',':
			case ']':
			case '}':
				return;
			default:
				in.get();
		}
	}
}

void JSON::ReadWs(std::istream& in) {
	while(!in.eof()) {
		in >> std::ws;
		if(in.peek() == '/') {
			in.get();
			if(in.peek() == '/') {
				while(in.peek()!='\n' && in.peek()!='\r' && !in.eof())
					in.get();
			} else if(in.peek() == '*') {
				while(true) {
					while(in.get() != '*') {
					}
					if(in.get() == '/')
						break;
				}
			}
		} else if(in.peek() == ';' || in.peek() == '%') {
			while(in.peek()!='\n' && in.peek()!='\r' && !in.eof())
				in.get();
		} else
			return;
	}
}

std::string JSON::ReadString(std::istream& in) {
	in.get(); // " or '
	std::string str;
	while(!in.eof()) {
		switch(in.peek()) {
			case '"':
			case '\'':
				in.get();
				return str;
			case '\\':
				in.get();
				
				switch(in.peek()) {
					case 'r': str += '\r'; break;
					case 'n': str += '\n'; break;
					case 't': str += '\t'; break;
					case '\\': str += '\\'; break;
					case '\'': str += '\''; break;
					case '"': str += '"'; break;
				}
				in.get();
				break;
			default:
				str += (char)in.get();
		}
	}
	return str;
}




void JSON::Error(const std::string& str) {
	if(str == "")
		return;
	std::lock_guard<std::mutex> guard(Mutex());
	Errors().push_back(str);
}

size_t JSON::ErrorsCount() {
	return Errors().size();
}

std::string JSON::Error() {
	std::lock_guard<std::mutex> guard(Mutex());
	if(Errors().size()) {
		std::string msg = Errors().front();
		Errors().pop_front();
		return msg;
	}
	return "";
}

std::list<std::string>& JSON::Errors() {
	static std::list<std::string> errors;
	return errors;
}

std::mutex& JSON::Mutex() {
	static std::mutex mutex;
	return mutex;
}

void JSON::Destroy() {
	switch(type) {
		case NONE: break;
		case STRING: delete string; break;
		case OBJECT: delete object; break;
		case ARRAY:  delete  array;  break;
		default: break;
	}
	string = NULL;
	type = NONE;
}

std::istream& operator>>(std::istream& in, JSON& json) {
	json.Read(in);
	return in;
}

std::ostream& operator<<(std::ostream& out, const JSON& json) {
	json.WriteCompact(out);
	return out;
}

