#include "keaCheckCPU_sse.hpp"

void
CPUResources::DiscoverSIMDAvailablilty ()
{
  s_kni_available = 1;
} 

bool
CPUResources::s_kni_available;
