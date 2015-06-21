#ifndef CONFIG_HPP
#define CONFIG_HPP

#ifdef _WIN32
#define WIN_OS
#else
#ifdef _LINUX
#define LIN_OS
#endif // _LINUX
#endif // _WIN32

#endif // CONFIG_HPP
