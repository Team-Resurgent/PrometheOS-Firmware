#pragma once

#include "xboxinternals.h"

#pragma pack(push,1)
typedef struct semver {
  uint8_t major;
  uint8_t minor;
  uint8_t patch;
  uint8_t unused;
} semver;
#pragma pack(pop)

class versioning
{
public:
	static int compareVersion(semver x, semver y);
};
