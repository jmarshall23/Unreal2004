@echo off
ucc dumpint -safe %1 %2 %3 %4 %5 %6 %7 %8 %9
ucc exportcache -a CacheRecords.ucl %1 %2 %3 %4 %5 %6 %7 %8 %9
