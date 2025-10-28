#include <stdio.h>
#include <stdlib.h>
#include "c8vm.h"

void VM_Inicializar(VM *vm, uint16_t pc_inicial){

   vm->PC = pc_inicial;
   vm->SP = 0;
   vm->I = 0;
   vm->delay_timer = 0;
   vm->sound_timer = 0; 

}

void VM_CarregarROM(VM *vm, char *arq_rom, uint16_t pc_inicial){

   FILE* rom = fopen(arq_rom, "rb");
   fseek(rom, 0, SEEK_END);
   int tam_rom = ftell(rom);
   rewind(rom);

   fread(&vm->RAM[pc_inicial], 1, tam_rom, rom);

   fclose(rom);

}

void VM_ExecutarInstrucao(VM *vm){

   uint16_t inst = (vm->RAM[vm->PC] << 8) | vm->RAM[vm->PC + 1];
   printf(" Instrucao: 0X%04X", inst);
   printf("\n");
   vm->PC += 2;

   uint8_t grupo = inst >> 12;            // Grupo (4 bits mais significativos)
   uint8_t X     = (inst & 0x0F00) >> 8;  // Registrador X (4 bits)
   uint8_t Y     = (inst & 0x00F0) >> 4;  // Registrador Y (4 bits)
   uint8_t N     = (inst & 0x000F);       // Nibble (4 bits)
   uint8_t NN    = (inst & 0x00FF);       // Byte (8 bits)
   uint8_t NNN   = (inst & 0x0FFF);       // Endereço (12 bits)

   switch(grupo){
      case 0:
         //CLS
         if(inst == 0x00E0){
            for(int i = 0; i < 64 * 32; i++){
               vm->DISPLAY[i] = 0;
            }
            break;
         } else if(inst == 0x00EE){
            if (vm->SP == 0){
               printf("Stack underflow!\n");
               exit(1);
            }
            vm->SP--;
            vm->PC = vm->stack[vm->SP];
            break;
         } else if(inst == 0x0000){
            break;
         }else {
            printf("Instrução 0x0 não reconhecida: 0x%04X\n", inst);
            exit(1);
         }
      
      case 1:
         vm->PC = NNN;
         break;

      case 2:
         vm->stack[vm->SP] = vm->PC;
         vm->SP++;
         vm->PC = NNN;
         break;

      case 6:
         vm->V[X] = NN;
         break;

      case 7:
         vm->V[X] += NN;
         break;

      case 8:
         switch (N){
            case 0:
               vm->V[X] = vm->V[Y];
               break;

            case 1:
               vm->V[X] = vm->V[X] or vm->V[Y];
               break;

            case 2:
               vm->V[X] = vm->V[X] and vm->V[Y];
               break;

            case 3:
               vm->V[X] = vm->V[X] xor vm->V[Y];
               break;

            case 4:
               if ((vm->V[X] + vm->V[Y]) > 255){
                  vm->V[X] = vm->V[X] + vm->V[Y];
                  vm->V[0xF] = 1;
               } else {
                  vm->V[X] = vm->V[X] + vm->V[Y];
                  vm->V[0xF] = 0;
               }
               break;

            case 5:
               if(vm->V[X] > vm->V[Y]){
                  vm->V[0xF] = 1;
                  vm->V[X] = vm->V[X] - vm->V[Y];
               } else {
                  vm->V[0xF] = 0;
                  vm->V[X] = vm->V[X] - vm->V[Y];
               }
               break;

            case 6:
               vm->V[0xF] = vm->V[X] & 0x01;
               vm->V[X] >>= 1;
               break;

            case 7:
               if(vm->V[Y] > vm->V[X]){
                  vm->V[0xF] = 1;
                  vm->V[X] = vm->V[Y] - vm->V[X];
               } else {
                  vm->V[0xF] = 0;
                  vm->V[X] = vm->V[Y] - vm->V[X];
               }
               break;

            case 0xE:
               vm->V[0xF] = vm->V[X] >> 7;
               vm->V[X] <<= 1;
               break;
            
            default:
               break;
         }
         break;

      case 0xA:
         vm->I = NNN;
         break;

      case 0xD:
         int coordenadaX = vm->V[X] & 63;
         int coordenadaY = vm->V[Y] & 31;
         vm->V[15] = 0;

         for(int i = 0; i < N; i++){
            uint8_t spriteByte = vm->RAM[vm->I+i];
            
            if((coordenadaY + i) >= 32){
               break;
            }
            for(int j = 0; j < 8; j++){
               if((coordenadaX + j) >= 64){
                  break;
               }

               int spritePixel = (spriteByte >> (7 - j)) & 1;
               if(spritePixel == 0){
                  if(vm->DISPLAY[((coordenadaY + i) * 64) + (coordenadaX + j)] == 0){
                     vm->DISPLAY[((coordenadaY + i) * 64) + (coordenadaX + j)] = 1;
                  } else {
                     vm->DISPLAY[((coordenadaY + i) * 64) + (coordenadaX + j)] = 0;
                     vm->V[15] = 1;
                  }
               }

            }
         break;
      }

      default:
         printf(" grupo nao implementado! Instrucao: 0x%04X\n", inst);
         exit(1);
   }

}

void VM_ImprimirRegistradores(VM* vm) {
   printf("PC: 0x%04X I: 0x%04X SP: 0x%02X\n", vm->PC, vm->I, vm->SP);
   for(int i = 0; i < 16; i++) {
      printf("V[%X]: 0x%02X ", i, vm->V[i]);
   }
   printf("\n");
}