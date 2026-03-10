#include <stdint.h>
#include "display.h"

class VM
{
private:
   uint8_t RAM[4096];   // Memória 4KB
   uint16_t PC;         // Contador de programa
   uint8_t V[16];       // Registradores V0–VF
   uint8_t SP;          // Ponteiro da pilha
   uint16_t I;          // Registrador de índice
   uint16_t stack[16];  // Pilha de 16 níveis
   uint8_t delay_timer; // Timer de delay
   uint8_t sound_timer; // Timer de som
   uint8_t teclado[16]; // Estado do teclado (16 teclas)

public:
   Display DISPLAY; // Display do sistema

   VM(int pixel_scale = 10); // Construtor

   void carregarROM(const char *arq_rom, uint16_t pc_inicial); // Carrega ROM na memória
   void executarInstrucao();                                   // Executa uma instrução
   void imprimirRegistradores();                               // Imprime registradores no console
   void definirTecla(uint8_t tecla, bool apertada);            // Define estado da tecla
   void atualizarTimers();                                     // Atualiza timers (delay e som)
};
