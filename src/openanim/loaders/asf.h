#pragma once

#include <boost/filesystem/path.hpp>

#include "openanim/Skeleton.h"

namespace openanim {

Skeleton loadASF(const boost::filesystem::path& path);

}
