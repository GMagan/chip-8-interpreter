#include <cstdio>
#include <cstdlib>
#include "c8vm.h"

VM::VM(int pixel_scale) : DISPLAY(pixel_scale)
{
   PC = 0x200;      // Endereço inicial de execução da ROM
   SP = 0;          // Inicializa ponteiro da pilha
   I = 0;           // Inicializa registrador de índice
   delay_timer = 0; // Inicializa timer de delay
   sound_timer = 0; // Inicializa timer de som
   DISPLAY.clear(); // Limpa o display

   // Limpa a memória
   for (int i = 0; i < 4096; i++)
   {
      this->RAM[i] = 0;
   }

   // Zera registradores, pilha e teclado
   for (int i = 0; i < 16; i++)
   {
      this->V[i] = 0;
      this->stack[i] = 0;
      this->teclado[i] = 0;
   }

   // Fonte padrão do CHIP-8 (0-F
   static const uint8_t fontset[80] = {
       0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
       0x20, 0x60, 0x20, 0x20, 0x70, // 1
       0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
       0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
       0x90, 0x90, 0xF0, 0x10, 0x10, // 4
       0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
       0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
       0xF0, 0x10, 0x20, 0x40, 0x40, // 7
       0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
       0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
       0xF0, 0x90, 0xF0, 0x90, 0x90, // A
       0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
       0xF0, 0x80, 0x80, 0x80, 0xF0, // C
       0xE0, 0x90, 0x90, 0x90, 0xE0, // D
       0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
       0xF0, 0x80, 0xF0, 0x80, 0x80  // F
   };

   // Carrega a fonte na memória
   for (int i = 0; i < 80; i++)
   {
      this->RAM[i] = fontset[i];
   }
}

// Carrega ROM na memória
void VM::carregarROM(const char *arq_rom, uint16_t pc_inicial)
{
   FILE *rom = fopen(arq_rom, "rb");

   if (!rom)
   {
      printf("Erro ao abrir ROM: %s\n", arq_rom);
      exit(1);
   }

   fseek(rom, 0, SEEK_END);
   int tam_rom = ftell(rom);
   rewind(rom);
   fread(&RAM[pc_inicial], 1, tam_rom, rom);
   fclose(rom);
}

// Atualiza estado de tecla (pressionada ou solta)
void VM::definirTecla(uint8_t tecla, bool apertado)
{
   if (tecla < 16)
   {
      teclado[tecla] = apertado;
   }
}

// Executa instrução apontada pelo PC
void VM::executarInstrucao()
{
   // Busca instrução
   uint16_t inst = (RAM[PC] << 8) | RAM[PC + 1];
   printf("Instrucao: 0x%04X\n", inst);
   PC += 2;

   // Decodifica campos da instrução
   uint8_t grupo = inst >> 12;
   uint8_t X = (inst & 0x0F00) >> 8;
   uint8_t Y = (inst & 0x00F0) >> 4;
   uint8_t N = (inst & 0x000F);
   uint8_t NN = (inst & 0x00FF);
   uint16_t NNN = (inst & 0x0FFF);

   switch (grupo)
   {
   case 0:
      if (inst == 0x00E0) // CLS
      {
         DISPLAY.clear();
      }
      else if (inst == 0x00EE) // retorna da sub-rotina
      {
         if (SP == 0)
         {
            printf("Stack underflow!\n");
            exit(1);
         }
         SP--;
         PC = stack[SP];
      }
      else if (inst == 0x0000) // Nada
      {
         break;
      }
      else
      {
         printf("Instrução 0x0 não reconhecida: 0x%04X\n", inst);
         exit(1);
      }
      break;

   case 1: // Salta para endereço NNN
      PC = NNN;
      break;

   case 2: // Chama sub-rotina
      stack[SP++] = PC;
      PC = NNN;
      break;

   case 3: // Se V[X] == NN, pula próxima instrução
      if (V[X] == NN)
      {
         PC += 2;
      }
      break;

   case 4: // Se V[X] != NN, pula próxima instrução
      if (V[X] != NN)
      {
         PC += 2;
      }
      break;

   case 5: // Se V[X] == V[Y], pula próxima instrução
      if (V[X] == V[Y])
      {
         PC += 2;
      }
      break;

   case 6: // Coloca o valor NN no registrador X
      V[X] = NN;
      break;

   case 7: // Soma o valor NN no registrador X
      V[X] += NN;
      break;

   case 8: // Operações lógicas
      switch (N)
      {
      case 0:
         V[X] = V[Y];
         break;

      case 1:
         V[X] |= V[Y];
         break;

      case 2:
         V[X] &= V[Y];
         break;

      case 3:
         V[X] ^= V[Y];
         break;

      case 4:
      {
         uint16_t soma = V[X] + V[Y];
         V[0xF] = (soma > 255);
         V[X] = soma & 0xFF;
         break;
      }

      case 5:
      {
         V[0xF] = (V[X] > V[Y]) ? 1 : 0;
         V[X] = V[X] - V[Y];
         break;
      }

      case 6:
         V[0xF] = V[X] & 0x01;
         V[X] >>= 1;
         break;

      case 7:
         V[0xF] = (V[Y] > V[X]);
         V[X] = V[Y] - V[X];
         break;

      case 0xE:
         V[0xF] = V[X] >> 7;
         V[X] <<= 1;
         break;

      default:
         printf("Operação 8XY%X não implementada\n", N);
         exit(1);
      }
      break;

   case 9: // Se V[X] != V[Y], pula próxima instrução
      if (V[X] != V[Y])
      {
         PC += 2;
      }
      break;

   case 0xA: // Coloca o valor de NNN no index I
      I = NNN;
      break;

   case 0xB: // Muda a instrução para NNN mais o valor no registrador 0
      PC = NNN + V[0];
      break;

   case 0xC: // Coloca um valor aleatório em X 
      V[X] = (rand() % 256) & NN;
      break;

   case 0xD: // Desenha um sprite
      V[0xF] = DISPLAY.drawSprite(V[X], V[Y], &RAM[I], N);
      break;

   case 0xE: // Instruções com teclado
      if (NN == 0x9E)
      {
         if (V[X] < 16 && teclado[V[X]] == true)
         {
            PC += 2;
         }
      }
      else if (NN == 0xA1)
      {
         if (V[X] < 16 && teclado[V[X]] == false)
         {
            PC += 2;
         }
      }
      break;

   case 0xF: 
      switch (NN)
      {
      case 0x07:
         V[X] = delay_timer;
         break;

      case 0x15:
         delay_timer = V[X];
         break;

      case 0x18:
         sound_timer = V[X];
         break;

      case 0x1E:
         I += V[X];
         break;

      case 0x0A:
      {
         bool teclaPressionada = false;
         for (int i = 0; i < 16; i++)
         {
            if (teclado[i])
            {
               V[X] = i;
               teclaPressionada = true;
               break;
            }
         }
         if (!teclaPressionada)
            PC -= 2;
         break;
      }

      case 0x29:
         I = (V[X] & 0xF) * 5;
         break;

      case 0x33:
         RAM[I] = V[X] / 100;
         RAM[I + 1] = (V[X] / 10) % 10;
         RAM[I + 2] = V[X] % 10;
         break;

      case 0x55:
         for (int i = 0; i <= X; i++)
         {
            RAM[I + i] = V[i];
         }
         break;

      case 0x65:
         for (int i = 0; i <= X; i++)
         {
            V[i] = RAM[I + i];
         }
         break;
      }
      break;

   default:
      printf("Grupo não implementado: 0x%X (instr: 0x%04X)\n", grupo, inst);
      exit(1);
   }
}

// Imprime registradores no console
void VM::imprimirRegistradores()
{
   printf("PC: 0x%04X I: 0x%04X SP: 0x%02X\n", PC, I, SP);
   for (int i = 0; i < 16; i++)
      printf("V[%X]: 0x%02X ", i, V[i]);
   printf("\n");
}

// Atualiza timers (delay e som)
void VM::atualizarTimers()
{
   if (this->delay_timer > 0)
   {
      --this->delay_timer;
   }
   if (this->sound_timer > 0)
   {
      --this->sound_timer;
   }

   if (this->sound_timer > 0)
   {
   }
   else
   {
   }
}