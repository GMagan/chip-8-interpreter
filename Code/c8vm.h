#include <stdint.h>

typedef struct VM {
   uint8_t RAM[4096];
   uint16_t PC;
   uint8_t V[16];
   uint8_t SP;
   uint16_t I;
   uint16_t stack;
   uint8_t DISPLAY[64 * 32];
} VM;

void VM_Inicializar(VM *vm, uint16_t pc_inicial);
void VM_CarregarROM(VM *vm, char *arq_rom, uint16_t pc_inicial);
void VM_ExecutarInstrucao(VM *vm);
void VM_ImprimirRegistradores(VM *vm);