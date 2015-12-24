/*
 * json_traits.hpp
 *
 *  Created on: Apr 12, 2013
 *      Author: belyaev
 */

#ifndef JSON_TRAITS_HPP_
#define JSON_TRAITS_HPP_

#include <map>
#include <set>
#include <unordered_set>
#include <unordered_map>
#include <vector>

#include "Util/json.hpp"

namespace borealis {
namespace util {

template<class Container, class Appender>
struct container_json_traits {
    using value_type = Container;
    using optional_ptr_t = std::unique_ptr<value_type>;

    static Json::Value toJson(const value_type& val) {
        Json::Value ret = Json::ValueType::arrayValue;
        for(const auto& m : val) ret.append(util::toJson(m));
        return ret;
    }

    static optional_ptr_t fromJson(const Json::Value& val) {
        using T = std::decay_t<decltype(*std::begin(std::declval<Container>()))>;

        value_type ret;
        if(!val.isArray()) return nullptr;

        for(const auto& m : val) {
            if(auto v = util::fromJson<T>(m)) {
                Appender{}(ret, *v);
            } else return nullptr;
        }

        return optional_ptr_t { new value_type{std::move(ret)} };
    }
};

namespace impl_ {

struct push_backer {
    template<class Con, class Val>
    void operator()(Con& con, Val&& v) const {
        con.push_back(std::forward<Val>(v));
    }
};

struct inserter {
    template<class Con, class Val>
    void operator()(Con& con, Val&& v) const {
        con.insert(std::forward<Val>(v));
    }
};

} /* namespace impl_ */

template<class T, class Allocator>
struct json_traits<std::vector<T, Allocator>> : container_json_traits<std::vector<T, Allocator>, impl_::push_backer> {};
template<class T, class Compare, class Allocator>
struct json_traits<std::set<T, Compare, Allocator>> : container_json_traits<std::set<T, Compare, Allocator>, impl_::inserter> {};
template<class T, class Hash, class Compare, class Allocator>
struct json_traits<std::unordered_set<T, Hash, Compare, Allocator>> : container_json_traits<std::unordered_set<T, Hash, Compare, Allocator>, impl_::inserter> {};

template<class K, class V, class Hash, class Equal, class Alloc>
struct json_traits<std::unordered_map<K, V, Hash, Equal, Alloc>> {
    using theMap_t = std::unordered_map<K, V, Hash, Equal, Alloc>;
    using optional_ptr_t = std::unique_ptr<theMap_t>;

    static Json::Value toJson(const theMap_t& val) {
        Json::Value ret = Json::arrayValue;
        for(const auto& kv : val) {
            Json::Value field = Json::objectValue;
            field["key"] = util::toJson(kv.first);
            field["value"] = util::toJson(kv.second);
            ret.append(field);
        }
        return ret;
    }

    static optional_ptr_t fromJson(const Json::Value& val) {
        theMap_t ret;
        if(!val.isArray()) return nullptr;
        for(const auto& kv : val) {
            if(!kv.isObject()) return nullptr;
            ret.insert(std::make_pair( *util::fromJson<K>(kv["key"]), *util::fromJson<V>(kv["value"]) ));
        }

        return optional_ptr_t { new theMap_t{ std::move(ret) } };
    }
};

template<class A, class B>
struct json_traits<std::pair<A, B>> {
    using value_t = std::pair<A, B>;
    using optional_ptr_t = std::unique_ptr<std::pair<A, B>>;

    static Json::Value toJson(const value_t& v) {
        Json::Value ret = Json::arrayValue;
        ret.append(util::toJson(v.first));
        ret.append(util::toJson(v.second));
        return ret;
    }

    static optional_ptr_t fromJson(const Json::Value& val) {
        if(!val.isArray() || val.size() != 2) return nullptr;
        auto&& jfirst = util::fromJson<A>(val[0]);
        auto&& jsecond = util::fromJson<B>(val[1]);
        if(!jfirst || !jsecond) return nullptr;

        return optional_ptr_t{ new value_t{ *jfirst, *jsecond } };
    }
};

} // namespace util
} // namespace borealis

#endif /* JSON_TRAITS_HPP_ */
