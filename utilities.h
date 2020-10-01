#pragma once

#define INFO(fmt, args...) (fprintf (stderr, "\x1B[34m%s:%d:%s(): " fmt "\x1B[0m\n", __FILE__, __LINE__, __func__, ##args))
#define FATAL(fmt, args...) (fprintf (stderr, "\x1B[31m%s:%d:%s(): " fmt "\x1B[0m\n", __FILE__, __LINE__, __func__, ##args), exit (-1))
