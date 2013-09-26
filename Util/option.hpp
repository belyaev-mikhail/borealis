/*
 * option.hpp
 *
 *  Created on: Oct 25, 2012
 *      Author: belyaev
 */

#ifndef OPTION_HPP_
#define OPTION_HPP_

#include <functional>
#include <iostream>
#include <memory>
#include <utility>

#include "Util/copying_ptr.hpp"
#include "Util/type_traits.hpp"

namespace borealis {
namespace util {

struct nothing_t {};

constexpr nothing_t nothing() {
    return nothing_t();
}

template< class OptionImpl >
struct option_iterator {
    OptionImpl* opt;
    option_iterator() : opt(nullptr) {};
    option_iterator(OptionImpl* opt) : opt(opt) {};
    option_iterator(const option_iterator&) = default;
    option_iterator& operator=(const option_iterator&) = default;

    bool operator==(const option_iterator& that) const {
        return opt == that.opt;
    }

    bool operator!=(const option_iterator& that) const {
        return opt != that.opt;
    }

    option_iterator operator++() {
        opt = nullptr;
        return *this;
    }

    option_iterator operator++(int) {
        auto* tmp = opt;
        opt = nullptr;
        return option_iterator(tmp);
    }

    typename OptionImpl::const_reference operator*() const {
        return opt->getUnsafe();
    }

    typename OptionImpl::reference operator*() {
        return opt->getUnsafe();
    }

    typename OptionImpl::const_pointer operator->() const {
        return opt->get();
    }

    typename OptionImpl::pointer operator->() {
        return opt->get();
    }
};

template< class OptionImpl >
struct const_option_iterator {
    const OptionImpl* opt;
    const_option_iterator() : opt(nullptr) {};
    const_option_iterator(const OptionImpl* opt) : opt(opt) {};
    const_option_iterator(const const_option_iterator&) = default;
    const_option_iterator& operator=(const const_option_iterator&) = default;

    bool operator==(const const_option_iterator& that) const {
        return opt == that.opt;
    }

    bool operator!=(const const_option_iterator& that) const {
        return opt != that.opt;
    }

    const_option_iterator operator++() {
        opt = nullptr;
        return *this;
    }

    const_option_iterator operator++(int) {
        auto* tmp = opt;
        opt = nullptr;
        return const_option_iterator(tmp);
    }

    typename OptionImpl::const_reference operator*() const {
        return opt->getUnsafe();
    }

    typename OptionImpl::const_pointer operator->() const {
        return opt->get();
    }
};

template<class T>
class option {
    typedef option self;

    copying_ptr<T> holder;

public:
    typedef T value_type;
    typedef T* pointer;
    typedef const T* const_pointer;
    typedef T& reference;
    typedef const T& const_reference;
    typedef option_iterator<self> iterator;
    typedef const_option_iterator<self> const_iterator;

    option() : holder{} {}
    option(nothing_t) : holder{} {}
    explicit option(const T& val) : holder{ new T(val) } {}
    explicit option(T&& val) : holder{ new T(val) } {}
    option(const self&) = default;
    option(self&&) = default;

    bool empty() const { return holder == nullptr; }

    void swap(self& that) {
        holder.swap(that.holder);
    }

    self& operator=(nothing_t) {
        holder.reset();
        return *this;
    }
    self& operator=(const self& that) = default;
    self& operator=(self&& that) = default;

    void push_back(const T& val) {
        operator=(option(val));
    }

    bool operator==(const self& that) const {
        if( empty() ) return that.empty();
        else if( that.empty() ) return false;
        else return *holder == *(that.holder);
    }

    bool operator==(const T& that) const {
        if( empty() ) return false;
        else return *holder == that;
    }

    bool operator==(const nothing_t&) const {
        return empty();
    }

    bool operator!=(const self& that) const {
        return !operator==(that);
    }

    bool operator!=(const T& that) const {
        return !operator==(that);
    }

    bool operator!=(const nothing_t&) const {
        return !empty();
    }

    bool operator!() const {
        return empty();
    }

    typedef struct unspec_{}* unspecified_pointer_type;
    operator unspecified_pointer_type() {
        static unspec_ unspec;
        if( empty() ) return nullptr;
        else return &unspec;
    }

    const T* get() const {
        return holder.get();
    }

    T* get() {
        return holder.get();
    }

    const T& getOrElse(const T& def) const {
        auto* ptr = holder.get();
        return ptr ? *ptr : def;
    }

    T getOrElse(T&& def) const {
        auto* ptr = holder.get();
        return ptr ? *ptr : def;
    }

    T& getOrElse(T& def) {
        auto* ptr = holder.get();
        return ptr ? *ptr : def;
    }

    const T& getUnsafe() const {
        return *holder;
    }

    T& getUnsafe() {
        return *holder;
    }

    T&& moveUnsafe() {
        return std::move(*holder);
    }

    const_iterator begin() const {
        if ( empty() ) return const_iterator();
        else return const_iterator(this);
    }

    const_iterator end() const {
        return const_iterator();
    }

    iterator begin() {
        if ( empty() ) return iterator();
        else return iterator(this);
    }

    iterator end() {
        return iterator();
    }
};

template<class T>
option<remove_const_reference_t<T>> just(T&& val) {
    return option<remove_const_reference_t<T>>{ std::forward<T>(val) };
}

template<class T>
inline option<T> nothing() {
    return nothing();
}

template<class T>
class option_ref {
    typedef option_ref self;

    T* holder;

public:
    typedef T value_type;
    typedef T* pointer;
    typedef const T* const_pointer;
    typedef T& reference;
    typedef const T& const_reference;
    typedef option_iterator<self> iterator;
    typedef const_option_iterator<self> const_iterator;

    option_ref() : holder(nullptr) {}
    option_ref(nothing_t) : holder(nullptr) {}
    explicit option_ref(T& val) : holder(&val) {}
    option_ref(const self& that): holder(that.holder) {}

    bool empty() const { return holder == nullptr; }

    void swap(self& that) {
        std::swap(holder, that.holder);
    }

    self& operator=(nothing_t) {
        self temp; // exception safety
        swap(temp);
        return *this;
    }
    self& operator=(const self& that) {
        self temp(that); // exception safety
        swap(temp);
        return *this;
    }
    self& operator=(self&& that) {
        holder = std::move(that.holder);
        return *this;
    }

    void push_back(T& val) {
        operator=(option_ref(val));
    }

    bool operator==(const self& that) const {
        if( empty() ) return that.empty();
        else if( that.empty() ) return false;
        else return *holder == *(that.holder);
    }

    bool operator==(const T& that) const {
        if( empty() ) return false;
        else return *holder == that;
    }

    bool operator==(const nothing_t&) const {
        return empty();
    }

    bool operator!=(const self& that) const {
        return !operator==(that);
    }

    bool operator!=(const T& that) const {
        return !operator==(that);
    }

    bool operator!=(const nothing_t&) const {
        return !empty();
    }

    typedef struct unspec_{}* unspecified_pointer_type;
    operator unspecified_pointer_type() {
        static unspec_ unspec;
        if( empty() ) return nullptr;
        else return &unspec;
    }

    bool operator!() const {
        return empty();
    }

    const T* get() const {
        return holder;
    }

    T* get() {
        return holder;
    }

    const T& getOrElse(const T& def) const {
        auto* ptr = this->get();
        return ptr ? *ptr : def;
    }

    T getOrElse(T&& def) const {
        auto* ptr = this->get();
        return ptr ? *ptr : def;
    }

    T& getOrElse(T& def) {
        auto* ptr = this->get();
        return ptr ? *ptr : def;
    }

    const T& getUnsafe() const {
        return *holder;
    }

    T& getUnsafe() {
        return *holder;
    }

    T&& moveUnsafe() {
        return std::move(*holder);
    }

    const_iterator begin() const {
        if ( empty() ) return const_iterator();
        else return const_iterator(this);
    }

    const_iterator end() const {
        return const_iterator();
    }

    iterator begin() {
        if ( empty() ) return iterator();
        else return iterator(this);
    }

    iterator end() {
        return iterator();
    }
};

template<class T>
option_ref<remove_reference_t<T>> justRef(T&& val) {
    return option_ref<remove_reference_t<T>>{ std::forward<T>(val) };
}

template<class T>
inline option_ref<T> nothingRef() {
    return nothing();
}

} // namespace util
} // namespace borealis

#endif /* OPTION_HPP_ */
