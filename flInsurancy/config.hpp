#ifndef CONFIG_HPP
#define CONFIG_HPP

#ifdef _WIN32
#define WIN_OS
#elif __linux__
#define LIN_OS
#elif __unix__
#define UNIX_OS
#elif __APPLE__
#define MAC_OS
#endif // _WIN32

#endif // CONFIG_HPP
