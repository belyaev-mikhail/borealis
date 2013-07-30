/*
 * MathSAT.cpp
 *
 *  Created on: Jul 17, 2013
 *      Author: Sam Kolton
 */

#include <functional>

#include "SMT/MathSAT/MathSAT.h"

#include "Util/macros.h"

#define ASSERTMSAT(name, arg) \
    ASSERTC(! name(arg))
#define ASSERTMSAT_TERM(arg) \
    ASSERTMSAT(MSAT_ERROR_TERM, arg)
#define ASSERTMSAT_DECL(arg) \
    ASSERTMSAT(MSAT_ERROR_DECL, arg)

namespace borealis {
namespace mathsat {

////////////////////////////////////////////////////////////////////////////////

unsigned Sort::bv_size() const {
	ASSERTC(is_bv());
	size_t width;
	msat_is_bv_type(env_, type_, &width);
	return width;
}

bool operator==(const Sort& a, const Sort& b) {
    return msat_type_equals(a.type_, b.type_);
}

////////////////////////////////////////////////////////////////////////////////

Expr Decl::operator()(const std::vector<Expr>& args) {
	std::vector<msat_term> msat_args(args.begin(), args.end());
	auto new_term = msat_make_term(env_, decl_, msat_args.data());
	ASSERTMSAT_TERM(new_term);
	return Expr(env_, new_term);
}

Expr Decl::operator()(const Expr& arg1) {
    return operator()(std::vector<Expr>({arg1}));
}

Expr Decl::operator()(const Expr& arg1, const Expr& arg2) {
    return operator()({arg1, arg2});
}

Expr Decl::operator()(const Expr& arg1, const Expr& arg2, const Expr& arg3) {
    return operator()({arg1, arg2, arg3});
}

////////////////////////////////////////////////////////////////////////////////

Env::Env(const Config& config) {
	env_ = msat_create_env(config);
	ASSERTMSAT(MSAT_ERROR_ENV, env_);
}

void Env::reset() {
	int res = msat_reset_env(env_);
	ASSERTC(!res);
}

Sort Env::bool_sort() {
	return Sort(*this, msat_get_bool_type(env_));
}
Sort Env::int_sort() {
	return Sort(*this, msat_get_integer_type(env_));
}
Sort Env::rat_sort() {
	return Sort(*this, msat_get_rational_type(env_));
}
Sort Env::bv_sort(unsigned size) {
	return Sort(*this, msat_get_bv_type(env_, size));
}
Sort Env::array_sort(const Sort& idx, const Sort& elm) {
	return Sort(*this, msat_get_array_type(env_, idx, elm));
}
Sort Env::simple_sort(const std::string& name) {
	return Sort(*this, msat_get_simple_type(env_, name.c_str()));
}

Expr Env::constant(const std::string& name, const Sort& type) {
	auto new_decl = msat_declare_function(env_, name.c_str(), type);
	ASSERTMSAT_DECL(new_decl);
	auto new_term = msat_make_constant(env_, new_decl);
	ASSERTMSAT(MSAT_ERROR_TERM, new_term)
	return Expr(*this, new_term);
}

Expr Env::bool_const(const std::string& name) {
	return constant(name, bool_sort());
}
Expr Env::int_const(const std::string& name) {
	return constant(name, int_sort());
}
Expr Env::rat_const(const std::string& name) {
	return constant(name, rat_sort());
}
Expr Env::bv_const(const std::string& name, unsigned size) {
	return constant(name, bv_sort(size));
}

Expr Env::bool_val(bool b) {
	auto new_term = b ? msat_make_true(env_) : msat_make_false(env_);
	ASSERTMSAT_TERM(new_term);
	return Expr(*this, new_term);
}

Expr Env::num_val(int i) {
	auto int_str = util::toString(i);
	auto new_term = msat_make_number(env_, int_str.c_str());
	ASSERTMSAT_TERM(new_term);
	return Expr(*this, new_term);
}

Expr Env::bv_val(int i, unsigned size) {
    auto int_str = util::toString(i);
	auto new_term = msat_make_bv_number(env_, int_str.c_str(), size, 10);
	ASSERTMSAT_TERM(new_term)
	return Expr(*this, new_term);
}

Decl Env::function(const std::string& name, const std::vector<Sort>& params, const Sort& ret) {
	std::vector<msat_type> msat_param(params.begin(), params.end());
	auto type = msat_get_function_type(env_, msat_param.data(), msat_param.size(), ret);
	auto func_type = Sort(*this, type);
	auto new_decl = msat_declare_function(env_, name.c_str(), func_type);
	ASSERTMSAT_DECL(new_decl);
	return Decl(*this, new_decl);
}

////////////////////////////////////////////////////////////////////////////////

Config::Config(const std::string& logic) {
	config_ = msat_create_default_config(logic.c_str());
	ASSERTMSAT(MSAT_ERROR_CONFIG, config_);
}

Config::Config(FILE* f) {
	config_ = msat_parse_config_file(f);
	ASSERTMSAT(MSAT_ERROR_CONFIG, config_);
}

////////////////////////////////////////////////////////////////////////////////

void Expr::visit(visit_function func, void* data) {
	int ret = msat_visit_term(env_, term_, func, data);
	ASSERTC(!ret);
}

Decl Expr::decl() const {
	msat_decl ret = msat_term_get_decl(term_);
	ASSERTMSAT(MSAT_ERROR_DECL, ret);
	return Decl(env_, ret);
}

Sort Expr::arg_sort(unsigned i) const {
	ASSERTC(num_args() < i)
	auto type = msat_decl_get_arg_type(decl(), i);
	return Sort(env_, type);
}



Expr operator !(const Expr& a){
	ASSERTC(a.is_bool());
	auto new_term = msat_make_not(a.env_, a.term_);
	ASSERTMSAT_TERM(new_term);
	return Expr(a.env_, new_term);
}
Expr operator -(const Expr& a) {
    ASSERTC(a.is_bv());
    msat_term new_term = msat_make_bv_neg(a.env_, a.term_);
    ASSERTMSAT_TERM(new_term);
    return Expr(a.env_, new_term);
}
Expr operator ~(const Expr& a){
    ASSERTC(a.is_bool());
    auto new_term = msat_make_bv_not(a.env_, a.term_);
    ASSERTMSAT_TERM(new_term);
    return Expr(a.env_, new_term);
}



Expr operator &&(const Expr& a, const Expr& b){
	ASSERTC(a.is_bool() && b.is_bool());
	auto new_term = msat_make_and(a.env_, a.term_, b.term_);
	ASSERTMSAT_TERM(new_term);
	return Expr(a.env_, new_term);
}
Expr operator &&(const Expr& a, bool b){
	Expr bb = a.env_.bool_val(b);
	return a && bb;
}
Expr operator &&(bool a, const Expr& b){
	Expr aa = b.env_.bool_val(a);
	return aa && b;
}



Expr operator ||(const Expr& a, const Expr& b){
	ASSERTC(a.is_bool() && b.is_bool());
	auto new_term = msat_make_or(a.env_, a.term_, b.term_);
	ASSERTMSAT_TERM(new_term);
	return Expr(a.env_, new_term);
}
Expr operator ||(const Expr& a, bool b){
    Expr bb = a.env_.bool_val(b);
    return a || bb;
}
Expr operator ||(bool a, const Expr& b){
    Expr aa = b.env_.bool_val(a);
    return aa || b;
}



Expr operator ==(const Expr& a, const Expr& b) {
	auto new_term = msat_make_equal(a.env_, a.term_, b.term_);
	ASSERTMSAT_TERM(new_term);
	return Expr(a.env_, new_term);
}
Expr operator ==(const Expr& a, int b){
	auto bb = a.env_.num_val(b);
	return a == bb;
}
Expr operator ==(int a, const Expr& b){
    auto aa = b.env_.num_val(a);
    return aa == b;
}

#define FORWARD_OP_EXPR_INT(OP, A, B) \
    Expr BB = A.env_.bool_val(true); \
    if (A.is_bv()) { \
        BB = A.env_.bv_val(B, A.get_sort().bv_size()); \
    } else { \
        BB = A.env_.num_val(B); \
    } \
    return A OP BB;
#define FORWARD_OP_INT_EXPR(OP, A, B) \
    Expr AA = B.env_.bool_val(true); \
    if (B.is_bv()) { \
        AA = B.env_.bv_val(A, B.get_sort().bv_size()); \
    } else { \
        AA = B.env_.num_val(A); \
    } \
    return AA OP B;

Expr operator +(const Expr& a, const Expr& b) {
	msat_term new_term;
	if (( a.is_rat() || a.is_int() ) && ( b.is_rat() || b.is_int() )) {
		new_term = msat_make_plus(a.env_, a.term_, b.term_);
	} else if (a.is_bv() && b.is_bv()) {
		ASSERTC(a.get_sort().bv_size() == b.get_sort().bv_size());
		new_term = msat_make_bv_plus(a.env_, a.term_, b.term_);
	} else {
		BYE_BYE(Expr, "Cannot add " + util::toString(a) + " and " + util::toString(b));
	}
	ASSERTMSAT_TERM(new_term);
	return Expr(a.env_, new_term);
}
Expr operator +(const Expr& a, int b){
    FORWARD_OP_EXPR_INT(+, a, b)
}
Expr operator +(int a, const Expr& b) {
    FORWARD_OP_INT_EXPR(+, a, b)
}



Expr operator *(const Expr& a, const Expr& b) {
	msat_term new_term;
	if (( a.is_rat() || a.is_int() ) && ( b.is_rat() || b.is_int() )) {
		new_term = msat_make_times(a.env_, a.term_, b.term_);
	} else if (a.is_bv() && b.is_bv()) {
		ASSERTC(a.get_sort().bv_size() == b.get_sort().bv_size());
		new_term = msat_make_bv_times(a.env_, a.term_, b.term_);
	} else {
	    BYE_BYE(Expr, "Cannot multiply " + util::toString(a) + " and " + util::toString(b));
	}
	ASSERTMSAT_TERM(new_term);
	return Expr(a.env_, new_term);
}
Expr operator *(const Expr& a, int b){
    FORWARD_OP_EXPR_INT(*, a, b)
}
Expr operator *(int a, const Expr& b) {
    FORWARD_OP_INT_EXPR(*, a, b)
}



Expr operator /(const Expr& a, const Expr& b) {
	ASSERTC(a.is_bv() && b.is_bv());
	msat_term new_term = msat_make_bv_sdiv(a.env_, a.term_, b.term_);
	ASSERTMSAT_TERM(new_term);
	return Expr(a.env_, new_term);
}
Expr operator /(const Expr& a, int b){
	auto b_str = util::toString(b);
	Expr bb = a.env_.bv_const(b_str, a.get_sort().bv_size());
	return a / bb;
}
Expr operator /(int a, const Expr& b){
    auto a_str = util::toString(a);
    Expr aa = b.env_.bv_const(a_str, b.get_sort().bv_size());
    return aa / b;
}



Expr operator -(const Expr& a, const Expr& b) {
    ASSERTC(a.is_bv() && b.is_bv());
    msat_term new_term = msat_make_bv_minus(a.env_, a.term_, b.term_);
    ASSERTMSAT_TERM(new_term);
    return Expr(a.env_, new_term);
}
Expr operator -(const Expr& a, int b){
    auto b_str = util::toString(b);
    Expr bb = a.env_.bv_const(b_str, a.get_sort().bv_size());
    return a - bb;
}
Expr operator -(int a, const Expr& b){
    auto a_str = util::toString(a);
    Expr aa = b.env_.bv_const(a_str, b.get_sort().bv_size());
    return aa - b;
}



Expr operator <=(const Expr& a, const Expr& b) {
	msat_term new_term;
	if (( a.is_rat() || a.is_int() ) && ( b.is_rat() || b.is_int() )) {
		new_term = msat_make_leq(a.env_, a.term_, b.term_);
	} else if (a.is_bv() && b.is_bv()) {
		ASSERTC(a.get_sort().bv_size() == b.get_sort().bv_size());
		new_term = msat_make_bv_sleq(a.env_, a.term_, b.term_);
	}
	else {
	    BYE_BYE(Expr, "Cannot compare " + util::toString(a) + " and " + util::toString(b));
	}
	ASSERTMSAT_TERM(new_term);
	return Expr(a.env_, new_term);
}
Expr operator <=(const Expr& a, int b){
    FORWARD_OP_EXPR_INT(<=, a, b)
}
Expr operator <=(int a, const Expr& b){
    FORWARD_OP_INT_EXPR(<=, a, b)
}



Expr operator >=(const Expr& a, const Expr& b) {
	msat_term new_term;
	if (( a.is_rat() || a.is_int() ) && ( b.is_rat() || b.is_int() )) {
		new_term = msat_make_leq(a.env_, a.term_, b.term_);
		new_term = msat_make_not(a.env_, new_term);
		auto eq_term = msat_make_equal(a.env_, a.term_, b.term_);
		new_term = msat_make_or(a.env_, new_term, eq_term);
	} else if (a.is_bv() && b.is_bv()) {
		ASSERTC(a.get_sort().bv_size() == b.get_sort().bv_size());
		new_term = msat_make_bv_sleq(a.env_, a.term_, b.term_);
		new_term = msat_make_bv_not(a.env_, new_term);
		auto eq_term = msat_make_equal(a.env_, a.term_, b.term_);
		new_term = msat_make_bv_or(a.env_, new_term, eq_term);
	} else {
	    BYE_BYE(Expr, "Cannot compare " + util::toString(a) + " and " + util::toString(b));
	}
	ASSERTMSAT_TERM(new_term);
	return Expr(a.env_, new_term);
}
Expr operator >=(const Expr& a, int b) {
    FORWARD_OP_EXPR_INT(>=, a, b)
}
Expr operator >=(int a, const Expr& b){
    FORWARD_OP_INT_EXPR(>=, a, b)
}



Expr operator <(const Expr& a, const Expr& b) {
	msat_term new_term;
	if (( a.is_rat() || a.is_int() ) && ( b.is_rat() || b.is_int() )) {
		new_term = msat_make_leq(a.env_, a.term_, b.term_);
		auto eq_term = msat_make_equal(a.env_, a.term_, b.term_);
		eq_term = msat_make_not(a.env_, eq_term);
		new_term = msat_make_and(a.env_, new_term, eq_term);
	} else if (a.is_bv() && b.is_bv()) {
		ASSERTC(a.get_sort().bv_size() == b.get_sort().bv_size());
		new_term = msat_make_bv_sleq(a.env_, a.term_, b.term_);
		auto eq_term = msat_make_equal(a.env_, a.term_, b.term_);
		eq_term = msat_make_bv_not(a.env_, eq_term);
		new_term = msat_make_bv_and(a.env_, new_term, eq_term);
	}
	else {
	    BYE_BYE(Expr, "Cannot compare " + util::toString(a) + " and " + util::toString(b));
	}
	ASSERTMSAT_TERM(new_term);
	return Expr(a.env_, new_term);
}
Expr operator <(const Expr& a, int b){
    FORWARD_OP_EXPR_INT(<, a, b)
}
Expr operator <(int a, const Expr& b){
    FORWARD_OP_INT_EXPR(<, a, b)
}



Expr operator >(const Expr& a, const Expr& b) {
	msat_term new_term;
	if (( a.is_rat() || a.is_int() ) && ( b.is_rat() || b.is_int() )) {
		new_term = msat_make_leq(a.env_, a.term_, b.term_);
		new_term = msat_make_not(a.env_, new_term);
	} else if (a.is_bv() && b.is_bv()) {
		ASSERTC(a.get_sort().bv_size() == b.get_sort().bv_size());
		new_term = msat_make_bv_sleq(a.env_, a.term_, b.term_);
		new_term = msat_make_bv_not(a.env_, new_term);
	} else {
	    BYE_BYE(Expr, "Cannot compare " + util::toString(a) + " and " + util::toString(b));
	}
	ASSERTMSAT_TERM(new_term);
	return Expr(a.env_, new_term);
}
Expr operator >(const Expr& a, int b){
    FORWARD_OP_EXPR_INT(>, a, b)
}
Expr operator >(int a, const Expr& b){
    FORWARD_OP_INT_EXPR(>, a, b)
}



Expr operator &(const Expr& a, const Expr& b){
	ASSERTC(a.is_bv() && b.is_bv());
    ASSERTC(a.get_sort().bv_size() == b.get_sort().bv_size());
	auto new_term = msat_make_bv_and(a.env_, a.term_, b.term_);
	ASSERTMSAT_TERM(new_term);
	return Expr(a.env_, new_term);
}
Expr operator &(const Expr& a, int b){
	Expr bb = a.env_.bv_val(b, a.get_sort().bv_size());
	return a & bb;
}
Expr operator &(int a, const Expr& b){
    Expr aa = b.env_.bv_val(a, b.get_sort().bv_size());
	return aa & b;
}



Expr operator ^(const Expr& a, const Expr& b){
	ASSERTC(a.is_bv() && b.is_bv());
    ASSERTC(a.get_sort().bv_size() == b.get_sort().bv_size());
	auto new_term = msat_make_bv_xor(a.env_, a.term_, b.term_);
	ASSERTMSAT_TERM(new_term);
	return Expr(a.env_, new_term);
}
Expr operator ^(const Expr& a, int b){
    Expr bb = a.env_.bv_val(b, a.get_sort().bv_size());
	return a ^ bb;
}
Expr operator ^(int a, const Expr& b){
    Expr aa = b.env_.bv_val(a, b.get_sort().bv_size());
	return aa ^ b;
}



Expr operator |(const Expr& a, const Expr& b){
	ASSERTC(a.is_bv()&& b.is_bv())
    ASSERTC(a.get_sort().bv_size() == b.get_sort().bv_size());
	auto new_term = msat_make_bv_or(a.env_, a.term_, b.term_);
	ASSERTMSAT_TERM(new_term);
	return Expr(a.env_, new_term);
}
Expr operator |(const Expr& a, int b){
    Expr bb = a.env_.bv_val(b, a.get_sort().bv_size());
	return a | bb;
}
Expr operator |(int a, const Expr& b){
    Expr aa = b.env_.bv_val(a, b.get_sort().bv_size());
	return aa | b;
}

// TODO sam Try to implement R/Z div and minus using smtlib

Expr iff(const Expr& a, const Expr& b) {
	ASSERTC(a.is_bool() && b.is_bool());
	auto new_term = msat_make_iff(a.env_, a.term_, b.term_);
	ASSERTMSAT_TERM(new_term);
	return Expr(a.env_, new_term);
}

Expr ite(const Expr& cond, const Expr& then_, const Expr& else_) {
	ASSERTC(cond.is_bool());
	auto new_term = msat_make_term_ite(cond.env_, cond, then_, else_);
	ASSERTMSAT_TERM(new_term);
	return Expr(cond.env_, new_term);
}

Expr concat(const Expr& a, const Expr& b) {
	ASSERTC(a.is_bv() && b.is_bv())
	auto new_term = msat_make_bv_concat(a.env_, a, b);
	ASSERTMSAT_TERM(new_term);
	return Expr(a.env_, new_term);
}

Expr extract(const Expr& a, unsigned msb, unsigned lsb) {
	ASSERTC(a.is_bv());
	auto new_term = msat_make_bv_extract(a.env_, msb, lsb, a);
	ASSERTMSAT_TERM(new_term);
	return Expr(a.env_, new_term);
}

#define BV_FUNC_GENERATOR(FUNC, MSAT_FUNC) \
    Expr FUNC(const Expr& a, const Expr& b) { \
        ASSERTC(a.is_bv() && b.is_bv()); \
        ASSERTC(a.get_sort().bv_size() == b.get_sort().bv_size()); \
        auto new_term = MSAT_FUNC(a.env_, a, b); \
        ASSERTMSAT_TERM(new_term); \
        return Expr(a.env_, new_term); \
    }

BV_FUNC_GENERATOR(lshl, msat_make_bv_lshl)
BV_FUNC_GENERATOR(lshr, msat_make_bv_lshr)
BV_FUNC_GENERATOR(ashr, msat_make_bv_ashr)
BV_FUNC_GENERATOR(udiv, msat_make_bv_udiv)

Expr udiv(const Expr& a, int b) {
	auto bb = a.env_.num_val(b);
	return udiv(a, bb);
}
Expr udiv(int a, const Expr& b) {
	auto aa = b.env_.num_val(a);
	return udiv(aa, b);
}

BV_FUNC_GENERATOR(comp, msat_make_bv_comp)

Expr rol(const Expr& a, unsigned b) {
	ASSERTC(a.is_bv());
	auto new_term = msat_make_bv_rol(a.env_, b, a);
	ASSERTMSAT_TERM(new_term);
	return Expr(a.env_, new_term);
}

Expr ror(const Expr& a, unsigned b) {
	ASSERTC(a.is_bv());
	auto new_term = msat_make_bv_ror(a.env_, b, a);
	ASSERTMSAT_TERM(new_term);
	return Expr(a.env_, new_term);
}

BV_FUNC_GENERATOR(ule, msat_make_bv_uleq)
Expr ule(const Expr& a, const int b) {
	auto bb = a.env_.num_val(b);
	return ule(a, bb);
}

Expr ule(const int a, const Expr& b) {
	auto aa = b.env_.num_val(a);
	return ule(aa, b);
}

BV_FUNC_GENERATOR(ult, msat_make_bv_ult)
Expr ult(const Expr& a, const int b) {
	auto bb = a.env_.num_val(b);
	return ult(a, bb);
}

Expr ult(const int a, const Expr& b) {
	auto aa = b.env_.num_val(a);
	return ult(aa, b);
}

Expr uge(const Expr& a, const Expr& b) {
	ASSERTC(a.is_bv() && b.is_bv());
	ASSERTC(a.get_sort().bv_size() == b.get_sort().bv_size());
	auto new_term = msat_make_bv_not(a.env_, msat_make_bv_ult(a.env_, a, b));
	ASSERTMSAT_TERM(new_term);
	return Expr(a.env_, new_term);
}
Expr uge(const Expr& a, const int b) {
	auto bb = a.env_.num_val(b);
	return uge(a, bb);
}
Expr uge(const int a, const Expr& b) {
	auto aa = b.env_.num_val(a);
	return uge(aa, b);
}

Expr ugt(const Expr& a, const Expr& b) {
	ASSERTC(a.is_bv() && b.is_bv());
	ASSERTC(a.get_sort().bv_size() == b.get_sort().bv_size());
	auto new_term = msat_make_bv_not(a.env_, msat_make_bv_uleq(a.env_, a, b));
	ASSERTMSAT_TERM(new_term);
	return Expr(a.env_, new_term);
}
Expr ugt(const Expr &a, const int b) {
	auto bb = a.env_.num_val(b);
	return ugt(a, bb);
}
Expr ugt(const int a, const Expr &b) {
	auto aa = b.env_.num_val(a);
	return ugt(aa, b);
}

#undef BV_FUNC_GENERATOR



Expr Expr::from_string(Env& env, const std::string& data) {
	auto new_term = msat_from_string(env, data.c_str());
	ASSERTMSAT_TERM(new_term);
	return Expr(env, new_term);
}

Expr Expr::from_smtlib(Env& env, const std::string& data) {
	auto new_term = msat_from_smtlib1(env, data.c_str());
	ASSERTMSAT_TERM(new_term);
	return Expr(env, new_term);
}

Expr Expr::from_smtlib2(Env& env, const std::string& data) {
	auto new_term = msat_from_smtlib2(env, data.c_str());
	ASSERTMSAT_TERM(new_term);
	return Expr(env, new_term);
}

////////////////////////////////////////////////////////////////////////////////

void Solver::add(const Expr& e) {
	int res = msat_assert_formula(env_, e);
	ASSERTC(!res)
}

void Solver::push() {
	int res = msat_push_backtrack_point(env_);
	ASSERTC(!res)
}
void Solver::pop() {
	int res = msat_pop_backtrack_point(env_);
	ASSERTC(!res)
}

msat_result Solver::check(const std::vector<Expr>& assumptions) {
    std::vector<msat_term> msat_assumptions(assumptions.begin(), assumptions.end());
	return msat_solve_with_assumptions(env_, msat_assumptions.data(), msat_assumptions.size());
}

std::vector<Expr> Solver::assertions() {
	size_t size;
	auto msat_exprs = util::uniq(msat_get_asserted_formulas(env_, &size));
	std::vector<Expr> exprs;
	exprs.reserve(size);
	for (unsigned idx = 0; idx < size; ++idx) {
		exprs.emplace_back(env_, (msat_exprs.get())[idx]);
	}
	return exprs;
}

std::vector<Expr> Solver::unsat_core() {
	size_t size;
	auto msat_exprs = util::uniq(msat_get_unsat_core(env_, &size));
	std::vector<Expr> exprs;
	exprs.reserve(size);
	for (unsigned idx = 0; idx < size; ++idx) {
		exprs.emplace_back(env_, (msat_exprs.get())[idx]);
	}
	return exprs;
}

void Solver::set_interp_group(InterpolationGroup gr) {
	int res = msat_set_itp_group(env_, gr);
	ASSERTC(!res)
}

Expr Solver::get_interpolant(const std::vector<InterpolationGroup>& A) {
    std::vector<InterpolationGroup> AA(A.begin(), A.end());
	auto new_term = msat_get_interpolant(env_, AA.data(), AA.size());
	ASSERTMSAT_TERM(new_term);
	return Expr(env_, new_term);
}

} // namespace mathsat
} // namespace borealis

#undef ASSERTMSAT_DECL
#undef ASSERTMSAT_TERM
#undef ASSERTMSAT

#include "Util/unmacros.h"
