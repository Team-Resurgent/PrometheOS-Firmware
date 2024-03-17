#include "versioning.h"

inline int binaryCompare(uint8_t x, uint8_t y) 
{
  if (x == y)
  {
	  return 0;
  }
  if (x > y) 
  {
	  return 1;
  }
  return -1;
}

int versioning::compareVersion(semver x, semver y) 
{
  int res;
  if ((res = binaryCompare(x.major, y.major)) == 0) 
  {
    if ((res = binaryCompare(x.minor, y.minor)) == 0) 
	{
      return binaryCompare(x.patch, y.patch);
    }
  }
  return res;
}
