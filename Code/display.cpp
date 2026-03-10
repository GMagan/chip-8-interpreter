#include "display.h"
#include <stdio.h>
#include <stdlib.h>
#include "SDL3/SDL.h"
#include <iostream>

Display::Display(int pixel_scale) : scale(pixel_scale)
{
   this->clear(); // Limpa o display

   if (SDL_Init(SDL_INIT_VIDEO) < 0) // Inicializa o subsistema de vídeo do SDL
   {
      fprintf(stderr, "Erro ao inicializar SDL: %s\n", SDL_GetError());
      fflush(stdout);
      exit(1);
   }
   else
   {
      printf("[DEBUG] SDL iniciado com sucesso!\n");
      fflush(stdout);
   }

   // Cria a janela principal
   window = SDL_CreateWindow(
       "CHIP-8",
       64 * scale,
       32 * scale,
       SDL_WINDOW_RESIZABLE);

   if (!window)
   {
      printf("Erro ao criar janela: %s\n", SDL_GetError());
      exit(1);
   }

   // Cria o renderizador
   renderer = SDL_CreateRenderer(window, NULL);

   if (!renderer)
   {
      printf("Erro ao criar renderer: %s\n", SDL_GetError());
      exit(1);
   }

   // Define a cor de fundo como preto e limpa a tela
   SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
   SDL_RenderClear(renderer);
   SDL_RenderPresent(renderer);
}

Display::~Display()
{
   SDL_DestroyRenderer(renderer); // Destroi o renderizador
   SDL_DestroyWindow(window);     // Destroi a janela
   SDL_Quit();                    // Encerra o SDL
}

void Display::clear()
{
   // Zera todos os pixels do display
   for (int i = 0; i < 64 * 32; i++)
   {
      this->DISPLAY[i] = 0;
   }
}

uint8_t Display::drawSprite(int x, int y, uint8_t *sprite, int n)
{
   uint8_t colisao = 0; // Flag de colisão
   int xcoord = x % 64; // Coordenada X ajustada
   int ycoord = y % 32; // Coordenada Y ajustada

   for (int i = 0; i < n; i++) // Para cada linha do sprite
   {
      x &= 63;
      y &= 31;
      uint8_t linha = sprite[i]; // Linha do sprite (8 bits)

      for (int j = 0; j < 8; j++) // Percorre cada bit
      {
         if ((linha & (0x80 >> j)) != 0) // Se o bit estiver ativo
         {
            int px = (x + j) % 64;
            int py = (y + i) % 32;
            int idx = py * 64 + px;

            if (DISPLAY[idx] == 1)
            {
               colisao = 1; // Marca colisão se pixel já estava ativo
            }
            DISPLAY[idx] ^= 1; // Inverte o pixel
         }
      }
   }
   return colisao; // Retorna se houve colisão
}

void Display::render()
{
   // Limpa a tela (preto)
   SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
   SDL_RenderClear(renderer);

   // Define a cor de desenho (branco)
   SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);

   // Desenha todos os pixels ativos do display
   for (int y = 0; y < 32; y++)
   {
      for (int x = 0; x < 64; x++)
      {
         if (DISPLAY[y * 64 + x])
         {
            SDL_FRect rect = {(float)(x * scale), (float)(y * scale), (float)scale, (float)scale};
            SDL_RenderFillRect(renderer, &rect); // Desenha o pixel
         }
      }
   }

   SDL_RenderPresent(renderer); // Atualiza o display
}

bool Display::isOpen()
{
   SDL_Event event;
   // Verifica eventos (ex: fechamento da janela)
   while (SDL_PollEvent(&event))
   {
      if (event.type == SDL_EVENT_QUIT)
      {
         return false; // Janela foi fechada
      }
   }
   return true; // Janela continua aberta
}
