#include <stdint.h>
#include "SDL3/SDL.h"

class Display
{
private:
   uint8_t DISPLAY[64 * 32]; // Tela de 64x32 pixels (monocromática)
   SDL_Window *window;       // Janela SDL
   SDL_Renderer *renderer;   // Renderizador SDL
   int scale = 10;           // Escala dos pixels

public:
   Display(int pixel_scale);                                 // Construtor
   ~Display();                                               // Destrutor
   void clear();                                             // Limpa a tela
   uint8_t drawSprite(int x, int y, uint8_t *sprite, int n); // Desenha sprite na tela
   void render();                                            // Atualiza a tela
   bool isOpen();                                            // Verifica se a janela está aberta
};
