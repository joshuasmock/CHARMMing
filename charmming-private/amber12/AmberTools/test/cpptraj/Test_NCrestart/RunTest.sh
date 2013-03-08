#!/bin/bash

. ../MasterTest.sh

# Clean
CleanFiles ncr.in nc.rst7* text.rst7*

TOP="../tz2.parm7"

CheckNetcdf

# Test 1  Convert text to netcdf restart
cat > ncr.in <<EOF
noprogress
trajin ../tz2.rst7
trajout nc.rst7 ncrestart title "trajectory generated by ptraj"
EOF
INPUT="ncr.in"
RunCpptraj "NetCDF Restart Test - TXT->NetCDF"

# Test 2  Convert netcdf to text restart
cat > ncr.in <<EOF
noprogress
trajin nc.rst7
# time0 of -1 means dont write time
trajout text.rst7 restart title "trajectory generated by ptraj" time0 -1 
EOF
INPUT="ncr.in"
RunCpptraj "NetCDF Restart Test - NetCDF->TXT"
# Tell test to ignore whitespace, this fixes mismatch in title line,
# cpptraj always writes out 80 chars
DoTest ../tz2.rst7 text.rst7 -w

CheckTest

EndTest

exit 0
