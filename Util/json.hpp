/*
 * json.hpp
 *
 *  Created on: Apr 12, 2013
 *      Author: belyaev
 */

#ifndef JSON_HPP_
#define JSON_HPP_

#include <string>
#include <memory>

#include <jsoncpp/json/json.h>

#include "Util/meta.hpp"

namespace borealis {
namespace util{

template<class T, typename SFINAE = void>
struct json_traits;

#include "Util/macros.h"

template<>
struct json_traits<Json::Value> {
    typedef std::unique_ptr<Json::Value> optional_ptr_t;

    static Json::Value toJson(const Json::Value& val) {
        return val;
    }

    static optional_ptr_t fromJson(const Json::Value& json) {
        return optional_ptr_t{ new Json::Value{ json } };
    }
};

template< class T >
struct json_traits<T, GUARD(std::is_same<T, bool>::value || std::is_floating_point<T>::value)> {
    typedef std::unique_ptr<T> optional_ptr_t;

    static Json::Value toJson(T val) {
        return Json::Value(val);
    }

    static optional_ptr_t fromJson(const Json::Value& json) {
        return json.isBool()   ? optional_ptr_t(new T(json.asBool())) :
               json.isInt()    ? optional_ptr_t(new T(json.asInt())) :
               json.isUInt()   ? optional_ptr_t(new T(json.asUInt())) :
               json.isDouble() ? optional_ptr_t(new T(json.asDouble())) :
                                 nullptr;
    }
};

template< class T >
struct json_traits<T, GUARD(std::is_arithmetic<T>::value && std::is_signed<T>::value)> {
    typedef std::unique_ptr<T> optional_ptr_t;

    static Json::Value toJson(T val) {
        return Json::Value(static_cast<Json::Int>(val));
    }

    static optional_ptr_t fromJson(const Json::Value& json) {
        return json.isBool()   ? optional_ptr_t(new T(json.asBool())) :
               json.isInt()    ? optional_ptr_t(new T(json.asInt())) :
               json.isUInt()   ? optional_ptr_t(new T(json.asUInt())) :
               json.isDouble() ? optional_ptr_t(new T(json.asDouble())) :
                                 nullptr;
    }
};

template< class T >
struct json_traits<T, GUARD(std::is_arithmetic<T>::value && std::is_unsigned<T>::value)> {
    typedef std::unique_ptr<T> optional_ptr_t;

    static Json::Value toJson(T val) {
        return Json::Value(static_cast<Json::UInt>(val));
    }

    static optional_ptr_t fromJson(const Json::Value& json) {
        return json.isBool()   ? optional_ptr_t(new T(json.asBool())) :
               json.isInt()    ? optional_ptr_t(new T(json.asInt())) :
               json.isUInt()   ? optional_ptr_t(new T(json.asUInt())) :
               json.isDouble() ? optional_ptr_t(new T(json.asDouble())) :
                                 nullptr;
    }
};

#include "Util/unmacros.h"

template<>
struct json_traits<std::string> {
    typedef std::unique_ptr<std::string> optional_ptr_t;

    static Json::Value toJson(const std::string& val) {
        return Json::Value(val);
    }

    static optional_ptr_t fromJson(const Json::Value& json) {
        return json.isString() ?
                optional_ptr_t(new std::string(json.asString())):
                nullptr;
    }
};

template<class T>
Json::Value toJson(const T& val) {
    return json_traits<T>::toJson(val);
}

template<class T>
typename json_traits<T>::optional_ptr_t fromJson(const Json::Value& json) {
    return json_traits<T>::fromJson(json);
}

template<class T>
typename json_traits<T>::optional_ptr_t read_as_json(std::istream& ist) {
    Json::Reader reader;
    Json::Value val;
    reader.parse(ist, val, false);
    return json_traits<T>::fromJson(val);
}

template<class T>
void write_as_json(std::ostream& ost, const T& val) {
    Json::StyledStreamWriter writer;
    writer.write(ost, json_traits<T>::toJson(val));
}

template<class T>
struct Jsoner {
    T* value;

    friend std::ostream& operator<< (std::ostream& ost, Jsoner<T> json) {
        write_as_json(ost, *json.value);
        return ost;
    }

    friend std::istream& operator>> (std::istream& ist, Jsoner<T> json) {
        auto read = read_as_json<T>(ist);
        if(read) *json.value = *read;
        else ist.clear(std::ios::failbit);
        return ist;
    }

};

template<class T>
struct const_Jsoner {
    const T* value;

    friend std::ostream& operator<< (std::ostream& ost, const_Jsoner<T> json) {
        write_as_json(ost, *json.value);
        return ost;
    }
};

template<class T>
Jsoner<T> jsonify(T& value) { return Jsoner<T> { &value }; }
template<class T>
const_Jsoner<T> jsonify(const T& value) { return const_Jsoner<T> { &value }; }

namespace impl {

static bool allptrs() {
    return true;
}

template<class H, class ...T>
static bool allptrs(const H& h, const T&... t) {
    if(!h) return false;
    else return allptrs(t...);
}

template<class F, class S>
struct type_K_comb {
    typedef F type;
};

template<class F, class S>
using type_K_comb_q = typename type_K_comb<F,S>::type;

}//namespace impl

template<class Obj, class ...Args>
struct json_object_builder {
    std::vector<std::string> keys;

    // this is a constructor that takes a number of std::strings
    // exactly the same as the number of types in Args
    json_object_builder(const impl::type_K_comb_q<std::string, Args>&... keys):
        keys{ keys... } {}

    template<size_t ...Ixs>
    Obj* build_(const Json::Value& val, util::indexer<Ixs...>) const {
        if(!val.isObject()) return nullptr;

        std::tuple<typename json_traits<util::index_in_row_q<Ixs, Args...>>::optional_ptr_t...> ptrs {
            util::fromJson<util::index_in_row_q<Ixs, Args...>>(val[keys[Ixs]])...
        };

        if(!impl::allptrs(std::get<Ixs>(ptrs)...)) return nullptr;

        return new Obj {
            *std::get<Ixs>(ptrs)...
        };
    }

    Obj* build(const Json::Value& val) const{
        return build_(val, typename util::make_indexer<Args...>::type());
    }
};




}
}




#endif /* JSON_HPP_ */
