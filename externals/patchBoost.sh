#!/bin/sh
sed -i "s/    case ENOTRECOVERABLE: return make_error_condition( state_not_recoverable );/  # if ECONNRESET != ENOTRECOVERABLE\n    case ENOTRECOVERABLE: return make_error_condition( state_not_recoverable );\n  # endif \/\/ ECONNRESET != ENOTRECOVERABLE/g" ./Boost/libs/system/src/error_code.cpp

sed -i "s/    case EOWNERDEAD: return make_error_condition( owner_dead );/  # if ECONNABORTED != EOWNERDEAD\n    case EOWNERDEAD: return make_error_condition( owner_dead );\n  # endif \/\/ ECONNABORTED != EOWNERDEAD/g" ./Boost/libs/system/src/error_code.cpp

