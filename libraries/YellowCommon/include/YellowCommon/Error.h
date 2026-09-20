#ifndef YELLOW_COMMON_ERROR_H
#define YELLOW_COMMON_ERROR_H

#include <SupportDefs.h>
#include <String.h>

#include "Defines.h"

namespace YellowCommon {

class Error {
public:
    Error(int32 code = YELLOW_SUCCESS, const char* message = nullptr);
    Error(const Error& other);
    ~Error();

    Error& operator=(const Error& other);

    int32 GetCode() const;
    const BString& GetMessage() const;
    bool IsSuccess() const;
    bool IsError() const;

    void SetCode(int32 code);
    void SetMessage(const char* message);
    void SetMessage(const BString& message);

    static Error Success();
    static Error FromErrno(int32 errnoValue);

private:
    int32 fCode;
    BString fMessage;
};

} // namespace YellowCommon

#endif // YELLOW_COMMON_ERROR_H
