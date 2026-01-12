#ifndef DIAG_H
#define DIAG_H

#include "stdnoreturn.h"
#include "token.h"

noreturn void diag_error(Token* tok, char* msg);

#endif  // DIAG_H
