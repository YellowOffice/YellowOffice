#include "YellowCommon/Error.h"

#include <string.h>
#include <errno.h>

namespace YellowCommon {

// Error implementation
Error::Error(int32 code, const char* message)
    : fCode(code)
{
    if (message) {
        fMessage = message;
    }
}

Error::Error(const Error& other)
    : fCode(other.fCode),
      fMessage(other.fMessage)
{
}

Error::~Error()
{
}

Error& Error::operator=(const Error& other)
{
    if (this != &other) {
        fCode = other.fCode;
        fMessage = other.fMessage;
    }
    return *this;
}

int32 Error::GetCode() const
{
    return fCode;
}

const BString& Error::GetMessage() const
{
    return fMessage;
}

bool Error::IsSuccess() const
{
    return fCode >= 0;
}

bool Error::IsError() const
{
    return fCode < 0;
}

void Error::SetCode(int32 code)
{
    fCode = code;
}

void Error::SetMessage(const char* message)
{
    if (message) {
        fMessage = message;
    }
}

void Error::SetMessage(const BString& message)
{
    fMessage = message;
}

Error Error::Success()
{
    return Error(YELLOW_SUCCESS, "Success");
}

Error Error::FromErrno(int32 errnoValue)
{
    const char* message = strerror(errnoValue);
    return Error(errnoValue, message);
}

} // namespace YellowCommon
