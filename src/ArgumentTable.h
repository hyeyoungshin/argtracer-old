#ifndef SIGNATR_ARGUMENT_TABLE_H
#define SIGNATR_ARGUMENT_TABLE_H

#include "Argument.h"
#include "Environment.h"
#include "Function.h"
#include "Call.h"
#include <unordered_map>
#include <string>
#include <vector>
#include "utilities.h"
#include <instrumentr/instrumentr.h>

class ArgumentTable {
  public:
    ArgumentTable(): size_(0) {
    }

    ~ArgumentTable() {
        for (auto iter = table_.begin(); iter != table_.end(); ++iter) {
            for (auto& argument: iter->second) {
                delete argument;
            }
        }
        table_.clear();
    }
    /* TODO: make this call_env */
    void insert(instrumentr_value_t argument,
                int formal_pos,
                const std::string& arg_name,
                Call* call_data,
                Function* function_data,
                Environment* environment_data) {
        int dot_pos = NA_INTEGER;

        if (arg_name == "..." || instrumentr_value_is_dot(argument)) {
            insert_dot_(argument,
                        formal_pos,
                        arg_name,
                        call_data,
                        function_data,
                        environment_data);
        }

        else if (instrumentr_value_is_missing(argument)) {
            insert_missing_(instrumentr_value_as_missing(argument),
                            formal_pos,
                            dot_pos,
                            arg_name,
                            call_data,
                            function_data,
                            environment_data);
        }

        else if (instrumentr_value_is_promise(argument)) {
            insert_promise_(instrumentr_value_as_promise(argument),
                            formal_pos,
                            dot_pos,
                            arg_name,
                            call_data,
                            function_data,
                            environment_data);
        }

        else {
            insert_value_(argument,
                          formal_pos,
                          dot_pos,
                          arg_name,
                          call_data,
                          function_data,
                          environment_data);
        }
    }

    const std::vector<Argument*>& lookup(int arg_id) {
        auto result = table_.find(arg_id);

        if (result == table_.end()) {
            Rf_error("cannot find argument with id %d", arg_id);
        }
        return result->second;
    }

    Argument* lookup(int arg_id, int call_id) {
        const std::vector<Argument*>& arguments = lookup(arg_id);

        for (auto argument: arguments) {
            if (argument->get_call_id() == call_id) {
                return argument;
            }
        }

        Rf_error(
            "cannot find argument with id %d and call id %d", arg_id, call_id);

        return NULL;
    }

    Argument* lookup_permissive(int arg_id, int call_id) {
        auto result = table_.find(arg_id);

        if (result == table_.end()) {
            return NULL;
        }

        const std::vector<Argument*>& arguments = result->second;

        for (auto argument: arguments) {
            if (argument->get_call_id() == call_id) {
                return argument;
            }
        }
        return NULL;
    }

    SEXP to_sexp() {
        SEXP r_arg_id = PROTECT(allocVector(INTSXP, size_));
        SEXP r_call_id = PROTECT(allocVector(INTSXP, size_));
        SEXP r_fun_id = PROTECT(allocVector(INTSXP, size_));
        SEXP r_call_env_id = PROTECT(allocVector(INTSXP, size_));
        SEXP r_arg_name = PROTECT(allocVector(STRSXP, size_));
        SEXP r_formal_pos = PROTECT(allocVector(INTSXP, size_));
        SEXP r_dot_pos = PROTECT(allocVector(INTSXP, size_));
        SEXP r_force_pos = PROTECT(allocVector(INTSXP, size_));
        SEXP r_actual_pos = PROTECT(allocVector(INTSXP, size_));
        SEXP r_default_arg = PROTECT(allocVector(LGLSXP, size_));
        SEXP r_vararg = PROTECT(allocVector(LGLSXP, size_));
        SEXP r_missing = PROTECT(allocVector(LGLSXP, size_));
        SEXP r_arg_type = PROTECT(allocVector(STRSXP, size_));
        SEXP r_expr_type = PROTECT(allocVector(STRSXP, size_));
        SEXP r_val_type = PROTECT(allocVector(STRSXP, size_));
        SEXP r_preforced = PROTECT(allocVector(INTSXP, size_));
        SEXP r_cap_force = PROTECT(allocVector(INTSXP, size_));
        SEXP r_cap_meta = PROTECT(allocVector(INTSXP, size_));
        SEXP r_cap_lookup = PROTECT(allocVector(INTSXP, size_));
        SEXP r_escaped = PROTECT(allocVector(LGLSXP, size_));
        SEXP r_esc_force = PROTECT(allocVector(INTSXP, size_));
        SEXP r_esc_meta = PROTECT(allocVector(INTSXP, size_));
        SEXP r_esc_lookup = PROTECT(allocVector(INTSXP, size_));
        SEXP r_con_force = PROTECT(allocVector(INTSXP, size_));
        SEXP r_con_lookup = PROTECT(allocVector(INTSXP, size_));
        SEXP r_force_depth = PROTECT(allocVector(INTSXP, size_));
        SEXP r_meta_depth = PROTECT(allocVector(INTSXP, size_));
        SEXP r_comp_pos = PROTECT(allocVector(INTSXP, size_));
        SEXP r_event_seq = PROTECT(allocVector(STRSXP, size_));
        SEXP r_self_effect_seq = PROTECT(allocVector(STRSXP, size_));
        SEXP r_effect_seq = PROTECT(allocVector(STRSXP, size_));
        SEXP r_self_ref_seq = PROTECT(allocVector(STRSXP, size_));
        SEXP r_ref_seq = PROTECT(allocVector(STRSXP, size_));
        SEXP r_parent_fun_id = PROTECT(allocVector(INTSXP, size_));
        SEXP r_parent_formal_pos = PROTECT(allocVector(INTSXP, size_));
        SEXP r_parent_call_id = PROTECT(allocVector(INTSXP, size_));
        SEXP r_parent_arg_id = PROTECT(allocVector(INTSXP, size_));

        int index = 0;

        for (auto iter = table_.begin(); iter != table_.end(); ++iter) {
            auto& arguments = iter->second;

            for (Argument* argument: arguments) {
                argument->to_sexp(index,
                                  r_arg_id,
                                  r_call_id,
                                  r_fun_id,
                                  r_call_env_id,
                                  r_arg_name,
                                  r_formal_pos,
                                  r_dot_pos,
                                  r_force_pos,
                                  r_actual_pos,
                                  r_default_arg,
                                  r_vararg,
                                  r_missing,
                                  r_arg_type,
                                  r_expr_type,
                                  r_val_type,
                                  r_preforced,
                                  r_cap_force,
                                  r_cap_meta,
                                  r_cap_lookup,
                                  r_escaped,
                                  r_esc_force,
                                  r_esc_meta,
                                  r_esc_lookup,
                                  r_con_force,
                                  r_con_lookup,
                                  r_force_depth,
                                  r_meta_depth,
                                  r_comp_pos,
                                  r_event_seq,
                                  r_self_effect_seq,
                                  r_effect_seq,
                                  r_self_ref_seq,
                                  r_ref_seq,
                                  r_parent_fun_id,
                                  r_parent_formal_pos,
                                  r_parent_call_id,
                                  r_parent_arg_id);
                ++index;
            }
        }

        std::vector<SEXP> columns({r_arg_id,
                                   r_call_id,
                                   r_fun_id,
                                   r_call_env_id,
                                   r_formal_pos,
                                   r_dot_pos,
                                   r_force_pos,
                                   r_actual_pos,
                                   r_default_arg,
                                   r_arg_name,
                                   r_vararg,
                                   r_missing,
                                   r_arg_type,
                                   r_expr_type,
                                   r_val_type,
                                   r_preforced,
                                   r_cap_force,
                                   r_cap_meta,
                                   r_cap_lookup,
                                   r_escaped,
                                   r_esc_force,
                                   r_esc_meta,
                                   r_esc_lookup,
                                   r_con_force,
                                   r_con_lookup,
                                   r_force_depth,
                                   r_meta_depth,
                                   r_comp_pos,
                                   r_event_seq,
                                   r_self_effect_seq,
                                   r_effect_seq,
                                   r_self_ref_seq,
                                   r_ref_seq,
                                   r_parent_fun_id,
                                   r_parent_formal_pos,
                                   r_parent_call_id,
                                   r_parent_arg_id});

        std::vector<std::string> names({"arg_id",
                                        "call_id",
                                        "fun_id",
                                        "call_env_id",
                                        "formal_pos",
                                        "dot_pos",
                                        "force_pos",
                                        "actual_pos",
                                        "default",
                                        "arg_name",
                                        "vararg",
                                        "missing",
                                        "arg_type",
                                        "expr_type",
                                        "val_type",
                                        "preforced",
                                        "cap_force",
                                        "cap_meta",
                                        "cap_lookup",
                                        "escaped",
                                        "esc_force",
                                        "esc_meta",
                                        "esc_lookup",
                                        "con_force",
                                        "con_lookup",
                                        "force_depth",
                                        "meta_depth",
                                        "comp_pos",
                                        "event_seq",
                                        "self_effect_seq",
                                        "effect_seq",
                                        "self_ref_seq",
                                        "ref_seq",
                                        "parent_fun_id",
                                        "parent_formal_pos",
                                        "parent_call_id",
                                        "parent_arg_id"});

        SEXP df = create_data_frame(names, columns);

        UNPROTECT(37);

        return df;
    }

  private:
    std::unordered_map<int, std::vector<Argument*>> table_;
    int size_;

    void insert_dot_(instrumentr_value_t dot,
                     int formal_pos,
                     const std::string& arg_name,
                     Call* call_data,
                     Function* function_data,
                     Environment* environment_data) {
        int arg_id = instrumentr_value_get_id(dot);
        int call_id = call_data->get_id();
        int fun_id = function_data->get_id();
        int call_env_id = environment_data->get_id();
        int vararg = 1;
        int missing = 0;
        std::string arg_type = "vararg";
        std::string expr_type = LAZR_NA_STRING;
        std::string val_type = LAZR_NA_STRING;
        int preforced = 0;
        int dot_pos = 0;
        int default_arg = NA_LOGICAL;

        Argument* argument_data = new Argument(arg_id,
                                               call_id,
                                               fun_id,
                                               call_env_id,
                                               arg_name,
                                               formal_pos,
                                               dot_pos,
                                               default_arg,
                                               vararg,
                                               missing,
                                               arg_type,
                                               expr_type,
                                               val_type,
                                               preforced);

        insert_(argument_data);
    }

    void insert_missing_(instrumentr_missing_t missing_val,
                         int formal_pos,
                         int dot_pos,
                         const std::string& arg_name,
                         Call* call_data,
                         Function* function_data,
                         Environment* environment_data) {
        int arg_id = instrumentr_missing_get_id(missing_val);
        int call_id = call_data->get_id();
        int fun_id = function_data->get_id();
        int call_env_id = environment_data->get_id();
        int vararg = 0;
        int missing = 1;
        std::string arg_type = "missing";
        std::string expr_type = LAZR_NA_STRING;
        std::string val_type = LAZR_NA_STRING;
        int preforced = 0;
        int default_arg = NA_LOGICAL;

        Argument* argument_data = new Argument(arg_id,
                                               call_id,
                                               fun_id,
                                               call_env_id,
                                               arg_name,
                                               formal_pos,
                                               dot_pos,
                                               default_arg,
                                               vararg,
                                               missing,
                                               arg_type,
                                               expr_type,
                                               val_type,
                                               preforced);

        insert_(argument_data);
    }

    void insert_promise_(instrumentr_promise_t promise,
                         int formal_pos,
                         int dot_pos,
                         const std::string& arg_name,
                         Call* call_data,
                         Function* function_data,
                         Environment* environment_data) {
        instrumentr_value_type_t prom_expr_type = instrumentr_value_get_type(
            instrumentr_promise_get_expression(promise));

        instrumentr_value_type_t prom_val_type =
            instrumentr_value_get_type(instrumentr_promise_get_value(promise));

        instrumentr_value_t environment =
            instrumentr_promise_get_environment(promise);

        int arg_id = instrumentr_promise_get_id(promise);
        int call_id = call_data->get_id();
        int fun_id = function_data->get_id();
        int call_env_id = environment_data->get_id();
        int default_arg =
            instrumentr_value_get_id(environment) == call_env_id;
        int vararg = 0;
        int missing = 0;
        std::string arg_type = "promise";
        std::string expr_type = instrumentr_value_type_get_name(prom_expr_type);
        std::string val_type = instrumentr_value_type_get_name(prom_val_type);
        int preforced = instrumentr_promise_is_forced(promise);

        Argument* argument_data = new Argument(arg_id,
                                               call_id,
                                               fun_id,
                                               call_env_id,
                                               arg_name,
                                               formal_pos,
                                               dot_pos,
                                               default_arg,
                                               vararg,
                                               missing,
                                               arg_type,
                                               expr_type,
                                               val_type,
                                               preforced);

        insert_(argument_data);
    }

    void insert_value_(instrumentr_value_t value,
                       int formal_pos,
                       int dot_pos,
                       const std::string& arg_name,
                       Call* call_data,
                       Function* function_data,
                       Environment* environment_data) {
        int arg_id = instrumentr_value_get_id(value);
        int call_id = call_data->get_id();
        int fun_id = function_data->get_id();
        int call_env_id = environment_data->get_id();
        int vararg = 0;
        int missing = 0;
        std::string arg_type =
            instrumentr_value_type_get_name(instrumentr_value_get_type(value));
        std::string expr_type = LAZR_NA_STRING;
        std::string val_type = LAZR_NA_STRING;
        int preforced = 0;
        int default_arg = NA_LOGICAL;

        Argument* argument_data = new Argument(arg_id,
                                               call_id,
                                               fun_id,
                                               call_env_id,
                                               arg_name,
                                               formal_pos,
                                               dot_pos,
                                               default_arg,
                                               vararg,
                                               missing,
                                               arg_type,
                                               expr_type,
                                               val_type,
                                               preforced);

        insert_(argument_data);
    }

    void insert_(Argument* argument) {
        ++size_;
        int arg_id = argument->get_id();

        auto result = table_.insert({arg_id, {argument}});
        if (!result.second) {
            result.first->second.push_back(argument);
        }
    }
};

#endif /* SIGNATR_ARGUMENT_TABLE_H */
