#ifndef FRONTEND_INCLUDE_ERROR_COLLECTOR_HPP
#define FRONTEND_INCLUDE_ERROR_COLLECTOR_HPP

#include "parser.hpp"
#include <ostream>
#include <string>
#include <string_view>
#include <vector>

namespace language {

class Error_collector final {
  private:
    const std::string program_file_;

    struct Error_info {
        const std::string program_file_;
        const yy::location loc_;
        const std::string msg_;
        const std::string line_with_error_;

        Error_info(const std::string program_file, const yy::location &loc,
                   std::string_view msg, std::string_view line_with_error)
            : program_file_(program_file), loc_(loc), msg_(msg),
              line_with_error_(line_with_error) {}

        Error_info(const std::string program_file, const yy::location &loc,
                   std::string_view msg)
            : program_file_(program_file), loc_(loc), msg_(msg) {}

        void print(std::ostream &os) const {
            os << program_file_ << ':' << loc_.begin.line << ':'
               << loc_.begin.column << ": error: " << msg_ << '\n'
               << '\t' << line_with_error_ << '\n'
               << '\t';
            for (int i = 1; i < loc_.begin.column; ++i)
                os << ' ';

            int length_error_token = loc_.end.column - loc_.begin.column;
            for (int i = 0; i < length_error_token; ++i)
                os << "^";
            os << '\n';
        }
    };

    std::vector<Error_info> errors_;

  public:
    Error_collector(const std::string &program_file)
        : program_file_(program_file) {}

    void add_error(const yy::location &loc, std::string_view msg,
                   std::string_view line_with_error) {
        errors_.push_back(Error_info{program_file_, loc, msg, line_with_error});
    }

    void add_error(const yy::location &loc, std::string_view msg) {
        errors_.push_back(Error_info{program_file_, loc, msg});
    }

    bool has_errors() const noexcept { return !errors_.empty(); }

    void print_errors(std::ostream &os) const {
        for (const auto &error : errors_)
            error.print(os);
    }
};

} // namespace language

#endif // FRONTEND_INCLUDE_ERROR_COLLECTOR_HPP
