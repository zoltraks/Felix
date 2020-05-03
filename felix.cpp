#include <iostream>
#include <experimental/coroutine>
#include "BusMaster.hpp"
#include "CPU.hpp"

int main()
{
  BusMaster bus;
  CPU cpu;
  CpuLoop loop = cpuLoop( cpu );
  loop.setBusMaster( &bus );
  bus.process( 16000000 / 60 );
  return 0;
}
