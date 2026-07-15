#pragma once

#include "common/app/cli_parser/cli_parser.h"

#include "server/app/config/config/config.h"

namespace server {
using CliParser = common::CliParser<Config>;
} // namespace server
