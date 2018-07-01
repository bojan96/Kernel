
#include <stdint.h>

//void writePortDword(int16_t port, int32_t data);
//void writePortWord(int16_t port, int16_t data);
void writePortByte(uint16_t port, uint8_t data);
//int32_t readPortDword(int16_t port);
//int16_t readPortWord(int16_t port);
uint8_t readPortByte(uint16_t port);

