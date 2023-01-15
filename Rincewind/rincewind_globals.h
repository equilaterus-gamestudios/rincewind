//#include <stdint.h>

#define local_variable static
#define global_variable static
#define function static
#define internal static

#define Kilobytes(Value) ((Value)*1024)
#define Megabytes(Value) (Kilobytes(Value)*1024)
#define Gigabytes(Value) (Megabytes(Value)*1024)

//TODO(pipecaniza) set as uc
//typedef int8_t int8;
typedef char uint8;
