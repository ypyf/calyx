#ifndef calyx_exception_h
#define calyx_exception_h

#include <exception>
#include <cstdarg> // vararg
#include <cstdio> // vsnprintf
#include <cstring> // strncpy
#include <string>

namespace calyx
{

class Exception : public std::exception
{
public:

	Exception(const char *fmt, ...);
	virtual ~Exception() throw() {}

	inline virtual const char *what() const throw()
	{
		return message.c_str();
	}

private:

	std::string message;

}; // Exception

} // namespace calyx

#endif // calyx_exception_h