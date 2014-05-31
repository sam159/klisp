#include <stdlib.h>
#include "shared.h"

#include "mpc.h"

mpc_parser_t* gLispy = NULL;
size_t gParserCount = 0;
mpc_parser_t** gParsers = NULL;