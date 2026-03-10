Projeto: Interpretador CHIP-8 (C++ / SDL3)

Dependências Necessárias
| Dependência         | Versão Recomendada     | Descrição                                       |
| ------------------- | ---------------------- | ----------------------------------------------- |
| **Compilador C++**  | g++ (MinGW ou similar) | Suporte a C++17 ou superior                     |
| **SDL3**            | 3.2.24                 | Biblioteca de renderização e eventos de entrada |
| **Make (opcional)** | —                      | Facilita automação da compilação                |

Instalação das Dependências

Baixe e extraia a biblioteca SDL3 da página oficial:
🔗 https://github.com/libsdl-org/SDL

Copie as pastas de include e lib para um diretório de fácil acesso, por exemplo:
C:/Users/<seu_usuario>/Desktop/libs/SDL3-3.2.24/

Certifique-se de que dentro desse diretório existam as subpastas:
include/
lib/

Execute no terminal (dentro da pasta do projeto):
g++ -o c8interpreter main.cpp c8vm.cpp display.cpp
-I "C:/Users/<user>/Desktop/libs/SDL3-3.2.24/<versão>/include" 
-L "C:/Users/<user>/Desktop/libs/SDL3-3.2.24/<versão>/lib"
-lSDL3

Ao fim você deve ter um arquivo nomeado c8interpreter.exe