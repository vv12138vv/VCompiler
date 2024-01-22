#ifndef EXCEPTION_HPP
#define EXCEPTION_HPP

#include<exception>
#include<string>
#endif
using namespace std;

class Exception: public exception{
private:
    string error_msg_;
    string error_args_;
public:
    const char* what() const noexcept override;
    Exception(string  error_msg,string  error_args);

};