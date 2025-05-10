#pragma once
// Switches for optional solver features (set by build system)
#if !defined(SAT_USE_CDCL)
  #define SAT_USE_CDCL 0
#endif
#if !defined(SAT_USE_WATCHED)
  #define SAT_USE_WATCHED 1
#endif
