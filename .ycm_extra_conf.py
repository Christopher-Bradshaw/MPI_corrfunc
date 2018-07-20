def FlagsForFile(filename, **kwargs):
  return {
    'flags': [
        'c', '-Wall', '-Wextra', '-Werror', '-fopenmp', '-lm', # basic
        '-I./2pcf_plugins/Corrfunc/include/', '-I./2pcf_plugins/Corrfunc/utils/', # corrfunc
        '-I/usr/include/openmpi-x86_64/', '-L/usr/lib64/openmpi/lib/', '-lmpi', # mpi
    ]
  }


