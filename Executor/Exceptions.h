/*
 * Exceptions.h
 *
 *  Created on: Feb 20, 2015
 *      Author: belyaev
 */

#ifndef EXECUTOR_EXCEPTIONS_H_
#define EXECUTOR_EXCEPTIONS_H_

#include <exception>
#include <stdexcept>

#include <tinyformat/tinyformat.h>



namespace borealis {

class memory_exception : public std::runtime_error {
    void* ptr_ = nullptr;

public:
    memory_exception(const char* format, void* ptr):
        std::runtime_error{ tinyformat::format(format, ptr) }, ptr_{ptr}{};

    void* getPtr() const { return ptr_; }

    virtual ~memory_exception() = 0;
};

class illegal_mem_read_exception : public memory_exception {
public:
    illegal_mem_read_exception(void* ptr): memory_exception("Memory read violation at 0x%x", ptr) {};
};

class illegal_mem_write_exception : public memory_exception {
public:
    illegal_mem_write_exception(void* ptr): memory_exception("Memory write violation at 0x%x", ptr) {};
};

class illegal_mem_free_exception : public memory_exception {
public:
    illegal_mem_free_exception(void* ptr): memory_exception("Illegal free() at 0x%x", ptr) {};
};

class out_of_memory_exception : public memory_exception {
public:
    out_of_memory_exception(): memory_exception("Executor run out of available memory", nullptr) {};
};


} // namespace borealis

#endif /* EXECUTOR_EXCEPTIONS_H_ */
