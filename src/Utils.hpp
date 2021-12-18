#pragma once


inline bool isInDebug() {
#ifdef DEBUG
  return true;
#else
  return false;
#endif
}
