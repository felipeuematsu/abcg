# Projeto de Computacao Grafica

## A1- Aplicacao grafica interativa com graficos 2d

Felipe Leite Uematsu 21008216
Tamara Cavalcante dos Santos 21060012


Utilizamos como base o projeto asteroides.

Foi criado um menu e scores em openglwindow.cpp.

A logica do jogo em player.cpp de forma com que o objetivo eh atraves do quadrado buscar 10 triangulos roxos, ao mesmo tempo que evita colidir com as pedras verdes (objects.cpp), cada colisao com cada pedra verde resulta na perda de uma vida. O jogador possui 3 vidas ao iniciar o jogo, dessa forma se colidir com 3 pedras antes de ter buscado 10 triangulos, perde o jogo.
O jogador pode se esquivar das pedras e buscar os triangulos movendo-se para cima, baixo, direita e esquerda.


Link gitHub projeto: https://github.com/felipeuematsu/abcg/tree/main/examples/baleiaPoints

Link pag web com jogo rodando em WebAssembly: https://felipeuematsu.github.io/abcg/

