def FlagsForFile(filename, **kwargs):
  return {
    'flags': ['c', '-Wall', '-Wextra', '-Werror', '-fopenmp', 'lm', '-I./2pcf_plugins/Corrfunc/include/', '-I./2pcf_plugins/Corrfunc/utils/']
  }


