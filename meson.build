project('main', ['cpp', 'c'], default_options: ['cpp_std=c++17'], version: '0.1')

# include directory
inc = include_directories('include')

# compiler executable
executable(meson.project_name(), '.'.join(meson.project_name(), 'cpp'), include_directories: inc)
