#include "exception.hpp"


const char *Exception::what() const noexcept {
    return (error_msg_+error_args_).c_str();
}

Exception::Exception(string error_msg, string error_args):error_msg_(std::move(error_msg)),error_args_(std::move(error_args)) {

}
