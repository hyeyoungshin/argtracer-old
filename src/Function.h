#ifndef SIGNATR_FUNCTION_H
#define SIGNATR_FUNCTION_H

#include <string>
#include <vector>
#include "utilities.h"

class Function {
  public:
    Function(int fun_id,
             int fun_env_id,
             const std::string& fun_hash,
             const std::string& fun_def)
        : fun_id_(fun_id)
        , fun_name_(LAZR_NA_STRING)
        , anonymous_(FALSE)
        , qual_name_(LAZR_NA_STRING)
        , parent_fun_id_(NA_INTEGER)
        , fun_env_id_(fun_env_id)
        , call_count_(0)
        , fun_hash_(fun_hash)
        , fun_def_(fun_def) {
    }

    int get_id() {
        return fun_id_;
    }

    int get_fun_env_id() {
        return fun_env_id_;
    }

    void set_parent_id(int parent_fun_id) {
        parent_fun_id_ = parent_fun_id;
    }

    int get_parent_id() const {
        return parent_fun_id_;
    }

    bool has_parent() const {
        return parent_fun_id_ != NA_INTEGER;
    }

    const std::string& get_name() const {
        return fun_name_;
    }

    void set_name(const char* name) {
        fun_name_ = charptr_to_string(name);
    }

    bool has_name() const {
        return fun_name_ != LAZR_NA_STRING;
    }

    std::string get_qualified_name() const {
        return qual_name_;
    }

    void set_qualified_name(const std::string& qual_name, bool anonymous) {
        qual_name_ = qual_name;
        anonymous_ = anonymous;
    }

    bool has_qualified_name() const {
        return qual_name_ != LAZR_NA_STRING;
    }

    const std::string& get_hash() const {
        return fun_hash_;
    }

    void call() {
        ++call_count_;
    }

    int is_anonymous() const {
        return anonymous_;
    }

    void to_sexp(int index,
                 SEXP r_fun_id,
                 SEXP r_fun_name,
                 SEXP r_anonymous,
                 SEXP r_qual_name,
                 SEXP r_parent_fun_id,
                 SEXP r_fun_env_id,
                 SEXP r_call_count,
                 SEXP r_fun_hash,
                 SEXP r_fun_def) {
        SET_INTEGER_ELT(r_fun_id, index, fun_id_);
        SET_STRING_ELT(r_fun_name, index, make_char(fun_name_));
        SET_LOGICAL_ELT(r_anonymous, index, anonymous_);
        SET_STRING_ELT(r_qual_name, index, make_char(qual_name_));
        SET_INTEGER_ELT(r_parent_fun_id, index, parent_fun_id_);
        SET_INTEGER_ELT(r_fun_env_id, index, fun_env_id_);
        SET_INTEGER_ELT(r_call_count, index, call_count_);
        SET_STRING_ELT(r_fun_hash, index, make_char(fun_hash_));
        SET_STRING_ELT(r_fun_def, index, make_char(fun_def_));
    }

  private:
    int fun_id_;
    std::string fun_name_;
    bool anonymous_;
    std::string qual_name_;
    int parent_fun_id_;
    int fun_env_id_;
    int call_count_;
    std::string fun_hash_;
    std::string fun_def_;
};

#endif /* SIGNATR_FUNCTION_H */
