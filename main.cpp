//Projeto de Guilherme Belendir Magan

#include "c8vm.h"
#include "defs.h"
#include "SDL3/SDL.h"
#include <iostream>

// Configurações de temporização
int TIMER_HERTZ = 60;                             // Atualização de timers por segundo
int CPU_HERTZ = 500;                              // Instruções por segundo
int CICLOS_POR_FRAME = (CPU_HERTZ / TIMER_HERTZ); // Quantas instruções por frame
int FRAME_DELAY = (1000 / TIMER_HERTZ);           // Delay entre frames em ms
int TAMANHO = 10;                                 // Escala dos pixels no display

int main(int argc, char **argv)
{
   // Verifica se o arquivo ROM foi passado
   if (argc < 2)
   {
      std::cerr << "Uso: " << argv[0] << " <arquivo_ROM>\n";
      return 1;
   }

   VM vm(TAMANHO);                 // Cria VM com tamanho de pixel definido
   vm.carregarROM(argv[1], 0x200); // Carrega ROM na memória a partir do endereço 0x200

   bool sair = false;

   while (!sair)
   {
      Uint32 frameInicial = SDL_GetTicks(); // Marca tempo de início do frame

      // Processa eventos SDL (teclado e fechamento de janela)
      SDL_Event event;
      while (SDL_PollEvent(&event))
      {
         if (event.type == SDL_EVENT_QUIT)
         {
            sair = true; // Sai do loop se a janela for fechada
            break;
         }

         // Atualiza estado das teclas
         if (event.type == SDL_EVENT_KEY_DOWN || event.type == SDL_EVENT_KEY_UP)
         {
            bool apertado = (event.type == SDL_EVENT_KEY_DOWN);
            SDL_Keycode tecla = event.key.key;

            // Mapeamento das teclas do CHIP-8
            switch (tecla)
            {
            case SDLK_1:
               vm.definirTecla(0x1, apertado);
               break;
            case SDLK_2:
               vm.definirTecla(0x2, apertado);
               break;
            case SDLK_3:
               vm.definirTecla(0x3, apertado);
               break;
            case SDLK_4:
               vm.definirTecla(0xC, apertado);
               break;
            case SDLK_Q:
               vm.definirTecla(0x4, apertado);
               break;
            case SDLK_W:
               vm.definirTecla(0x5, apertado);
               break;
            case SDLK_E:
               vm.definirTecla(0x6, apertado);
               break;
            case SDLK_R:
               vm.definirTecla(0xD, apertado);
               break;
            case SDLK_A:
               vm.definirTecla(0x7, apertado);
               break;
            case SDLK_S:
               vm.definirTecla(0x8, apertado);
               break;
            case SDLK_D:
               vm.definirTecla(0x9, apertado);
               break;
            case SDLK_F:
               vm.definirTecla(0xE, apertado);
               break;
            case SDLK_Z:
               vm.definirTecla(0xA, apertado);
               break;
            case SDLK_X:
               vm.definirTecla(0x0, apertado);
               break;
            case SDLK_C:
               vm.definirTecla(0xB, apertado);
               break;
            case SDLK_V:
               vm.definirTecla(0xF, apertado);
               break;
            }
         }
      }

      // Executa número de instruções por frame
      for (int i = 0; i < CICLOS_POR_FRAME; ++i)
      {
         vm.executarInstrucao();
      }

      vm.atualizarTimers(); // Atualiza timers de delay e som

      // Ajusta o tempo de frame para manter frequência constante
      Uint32 tempoFrame = SDL_GetTicks() - frameInicial;
      if (tempoFrame < FRAME_DELAY)
      {
         SDL_Delay(FRAME_DELAY - tempoFrame);
      }

      vm.DISPLAY.render();    // Renderiza o display
      SDL_Delay(FRAME_DELAY); // Pequeno delay adicional
   }

   return 0;
}
