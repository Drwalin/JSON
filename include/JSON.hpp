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

#define DEBUG {fprintf(stderr, "\n %s  %s: %i", __FUNCTION__, __FILE__, __LINE__); fflush(stderr);}

inline std::istream& operator>>(std::istream& in, class JSON& json);
inline std::ostream& operator<<(std::ostream& out, const class JSON& json);

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
	template<typename T>
	JSON(T* ptr, size_t count) {
		type = ARRAY;
		array = new array_t(ptr, ptr+count);
	}
	template<class Iterator>
	JSON(const Iterator& begin, const Iterator& end) {
		type = OBJECT;
		object = new object_t(begin, end);
	}
	
	JSON(const JSON& other) {
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
	JSON(JSON&& other) {
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
	
	~JSON() {
		switch(type) {
			case STRING: delete string; break;
			case OBJECT: delete object; break;
			case ARRAY:  delete  array; break;
			default: break;
		}
		string = NULL;
	}
	
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
	inline JSON& operator=(JSON&& other) {
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
	inline JSON& operator=(const JSON& other) {
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
	
	
	inline void Init(Type newType) {
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
	inline void AssureType(Type newType) {
		if(type != newType)
			Init(newType);
	}
	
	template<typename... Args>
	inline void InitString(Args... args) {
		if(type == STRING)
			*string = string_t(args...);
		else {
			Destroy();
			type = STRING;
			string = new string_t(args...);
		}
	}
	template<typename... Args>
	inline void InitObject(Args... args) {
		if(type == OBJECT)
			*object = object_t(args...);
		else {
			Destroy();
			type = OBJECT;
			object = new object_t(args...);
		}
	}
	template<typename... Args>
	inline void InitArray(Args... args) {
		if(type == ARRAY)
			*array = array_t(args...);
		else {
			Destroy();
			type = ARRAY;
			array = new array_t(args...);
		}
	}
	inline void InitIntegr(integer_t value) {
		Destroy();
		integer = value;
		type = INTEGER;
	}
	inline void InitReal(real_t value) {
		Destroy();
		real = value;
		type = REAL;
	}
	inline void InitBoolean(boolean_t value) {
		Destroy();
		boolean = value;
		type = BOOLEAN;
	}
	
	JSON Convert(Type _type) {
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
						return JSON(atoll(string->c_str()));
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
	
	
	inline const JSON& operator[](const char* key) const {
		return (*this)[(std::string)key];
	}
	inline const JSON& operator[](const std::string& key) const {
		auto o = GetObject();
		auto it = o.find(key);
		if(it == o.end()) {
			const static JSON _json;
			return _json;
		}
		return it->second;
	}
	inline const JSON& operator[](size_t id) const {
		auto a = GetArray();
		if(a.size() <= id) {
			const static JSON _json;
			return _json;
		}
		return a[id];
	}
	
	inline operator const array_t&() const {return GetArray();}
	inline operator const object_t&() const {return GetObject();}
	inline operator const string_t() const {return GetString();}
	inline operator const real_t() const {return GetReal();}
	inline operator const boolean_t() const {return GetBoolean();}
	inline operator const integer_t() const {return GetInteger();}
	
	inline const array_t& Array() const {return GetArray();}
	inline const object_t& Object() const {return GetObject();}
	inline const string_t String() const {return GetString();}
	inline const real_t Real() const {return GetReal();}
	inline const integer_t Integer() const {return GetInteger();}
	inline const boolean_t Boolean() const {return GetBoolean();}
	
	inline const string_t GetString() const {
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
	inline const array_t& GetArray() const {
		if(type == ARRAY)
			return *array;
		const static array_t _array;
		return _array;
	}
	inline const object_t& GetObject() const {
		if(type == OBJECT)
			return *object;
		const static object_t _object;
		return _object;
	}
	inline const real_t GetReal() const {
		if(type == REAL)
			return real;
		if(type == INTEGER)
			return integer;
		if(type == BOOLEAN)
			return boolean;
		return 0.0;
	}
	inline const integer_t GetInteger() const {
		if(type == INTEGER)
			return integer;
		if(type == REAL)
			return real;
		if(type == BOOLEAN)
			return boolean;
		return 0;
	}
	inline const boolean_t GetBoolean() const {
		if(type == BOOLEAN)
			return boolean;
		if(type == REAL)
			return real;
		if(type == INTEGER)
			return integer;
		return false;
	}
	
	
	inline JSON& operator[](const char* key) {
		return (*this)[(std::string)key];
	}
	inline JSON& operator[](const std::string& key) {
		AssureType(OBJECT);
		return AccessObject()[key];
	}
	inline JSON& operator[](size_t id) {
		AssureType(ARRAY);
		if(array->size() <= id) {
			array->resize(id+1);
		}
		return array->at(id);
	}
	
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
	
	inline array_t& AccessArray() {
		AssureType(ARRAY);
		return *array;
	}
	inline object_t& AccessObject() {
		AssureType(OBJECT);
		return *object;
	}
	inline string_t& AccessString() {
		AssureType(STRING);
		return *string;
	}
	inline real_t& AccessReal() {
		AssureType(REAL);
		return real;
	}
	inline integer_t& AccessInteger() {
		AssureType(INTEGER);
		return integer;
	}
	inline boolean_t& AccessBoolean() {
		AssureType(BOOLEAN);
		return boolean;
	}
	
	
	
	inline void Clear() {
		Destroy();
	}
	
	inline void WriteCompact(std::ostream& out) const {
		WriteBeautyfull(out, "", "");
	}
	
	inline void WriteBeautyfull(std::ostream& out,
			const std::string& indent="\t",
			const std::string& newline="\n",
			size_t depth=0) const {
		bool next = false;
		switch(type) {
			case ARRAY:
			case OBJECT:
				{
					std::string ind = newline;
					for(size_t i=1; i<depth; ++i)
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
							p.second.WriteBeautyfull(out, indent, newline, depth+2);
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
	
	static inline void WriteString(std::ostream& out, const std::string& str) {
		out << '"';
		for(char c : str) {
			switch(c) {
				case '\t': out<<"\\t"; break;
				case '\n': out<<"\\n"; break;
				case '\r': out<<"\\r"; break;
				case '\'': out<<"\\'"; break;
				case '"': out<<"\\\""; break;
				case '\\': out<<"\\";  break;
				case 0:     out<<'"'; return;
				default: out<<c;
			}
		}
		out << '"';
	}
	
	inline void Read(std::istream& in) {
		DEBUG;
		ReadWs(in);
		switch(in.peek()) {
			case '[':	// is array
				in.get();
		DEBUG;
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
		DEBUG;
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
		DEBUG;
							Error("Invalid object value");
						}
					} else {
		DEBUG;
						Error("Invalid object key");
						std::cerr << *this << "   " << (char)in.peek();
					}
				}
				break;
			case 't':	// is boolean true
			case 'T':
			case 'y':
			case 'Y':
		DEBUG;
				*this = true;
				ClearUntilEndVar(in);
				return;
			case 'f':	// is boolean false
			case 'F':
			case 'n':
			case 'N':
		DEBUG;
				*this = false;
				ClearUntilEndVar(in);
				return;
			case '\'':	// is string
			case '"':
		DEBUG;
				*this = ReadString(in);
				return;
			default:	// is number or invalid
		DEBUG;
				{
					std::string value = GetUntilEndVar(in);
					if(value == "") {
						Destroy();
					}
					if(std::any_of(value.begin(), value.end(), [](char c)->bool{return c=='.'||c=='e'||c=='E';})) {	// is probably real
						*this = atof(value.c_str());
					} else { // is probably a integer integer
						*this = atoll(value.c_str());
					}
				}
		}
	}
	
	inline static std::string GetUntilEndVar(std::istream& in) {
		std::string str;
		while(!in.eof()) {
			ReadWs(in);
			switch(in.peek()) {
				case ',':
				case ']':
				case '}':
					return str;
				default:
					str += (char)in.get();
			}
		}
		return str;
	}
	
	inline static void ClearUntilEndVar(std::istream& in) {
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
	
	inline static void ReadWs(std::istream& in) {
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
	
	inline static std::string ReadString(std::istream& in) {
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
	
	
	
	
	static void Error(const std::string& str) {
		if(str == "")
			return;
		std::lock_guard<std::mutex> guard(Mutex());
		Errors().push_back(str);
	}
	
	static size_t ErrorsCount() {
		return Errors().size();
	}
	
	static std::string Error() {
		std::lock_guard<std::mutex> guard(Mutex());
		if(Errors().size()) {
			std::string msg = Errors().front();
			Errors().pop_front();
			return msg;
		}
		return "";
	}
	
private:
	
	static std::list<std::string>& Errors() {
		static std::list<std::string> errors;
		return errors;
	}
	
	static std::mutex& Mutex() {
		static std::mutex mutex;
		return mutex;
	}
	
	inline void Destroy() {
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

inline std::istream& operator>>(std::istream& in, JSON& json) {
	json.Read(in);
	return in;
}

inline std::ostream& operator<<(std::ostream& out, const JSON& json) {
	json.WriteCompact(out);
	return out;
}

#endif

