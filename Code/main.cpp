#include "c8vm.h"
#include "defs.h"

int main(int argc, char **argv) {
   VM vm;

   VM_Inicializar(&vm, 0x200);
   VM_CarregarROM(&vm, argv[1], 0x200);

   #ifdef DEBUG
   VM_ImprimirRegistradores(&vm);
   #endif
   
   while (1){
      VM_ExecutarInstrucao(&vm);
      #ifdef DEBUG
      VM_ImprimirRegistradores(&vm);
      #endif
   }

}