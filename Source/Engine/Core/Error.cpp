
#include <Core/Error.h>

Error::Error(int level, std::string message, std::string detail) {
    this->level = level;
    this->message = message;
    this->detail = detail;
}
