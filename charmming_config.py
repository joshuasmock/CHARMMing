#
#                            PUBLIC DOMAIN NOTICE
#
#  This software/database is a "United States Government Work" under the
#  terms of the United States Copyright Act.  It was written as part of
#  the authors' official duties as a United States Government employee and
#  thus cannot be copyrighted.  This software is freely available
#  to the public for use.  There is no restriction on its use or
#  reproduction.
#
#  Although all reasonable efforts have been taken to ensure the accuracy
#  and reliability of the software and data, NIH and the U.S.
#  Government do not and cannot warrant the performance or results that
#  may be obtained by using this software or data. NIH, NHLBI, and the U.S.
#  Government disclaim all warranties, express or implied, including
#  warranties of performance, merchantability or fitness for any
#  particular purpose.

# maximum number of atoms
maxatoms = 50000

# set to 1 if you have Q-Chem enabled in your CHARMM exe and want
# to allow your users to use QM/MM
haveqchem = 1

# root of the CHARMMing install
charmming_root = "/var/www/charmming"

# place where user directories are...
user_home = "/home/pdb_uploads"

# place where executables etc. are kept
data_home = "/usr/local/charmming"

# methods to use to try to generate topology/parameters for BADHET
# atoms. These methods will be tried in order until one succeeds 
# (or they all fail).
toppar_generators = 'cgenff,match,antechamber,genrtf'

# CGenFF host & port
cgenff_host = 'dogmans.umaryland.edu'
cgenff_port = 32108

# path to the single threaded CHARMM executable
#charmm_exe = "/usr/local/charmming/gfortran-xxlg-qc.one"
charmm_exe = "/usr/local/charmming/c35b4-apbs-qc.one"

# path to the MPI-enabled CHARMM executable
charmm_mpi_exe = "/usr/local/charmming/gfortran-xxlg-qc.ompi"

# path to mpirun (not used by default -- testing parallel)
mpirun_exe = "/bin/false"

# path to stride (needed for CG stuff)
stride_bin = "/usr/local/bin/stride"
