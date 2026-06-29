#pragma once

#include "common/app/cli/cli_parser/cli_parser.h"

#include "server/app/config/config/config.h"

namespace server {
using CLIParser = common::CLIParser<Config>;
} // namespace server
