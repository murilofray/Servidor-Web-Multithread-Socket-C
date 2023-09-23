# Projeto de Servidor Web Multithread em C

## Descrição
Este projeto implementa um servidor web em C capaz de tratar várias conexões simultaneamente usando multithreading. O servidor é responsável por enviar páginas HTML para os navegadores e, caso a página requisitada não seja encontrada, ele retorna um erro 404 com uma página HTML personalizada contendo a mensagem de erro.

## Configuração do Ambiente

### Dependências
- Compilador C (ex: GCC)
- Sistema operacional Windows

### Configuração do Linker (Windows)
Certifique-se de adicionar a seguinte opção de linker nas configurações do seu projeto:
1. Abra as configurações do projeto.
2. Navegue até "Settings" -> "Compiler" -> "Linker Settings".
3. Adicione a opção `-l wsock32` para garantir que as bibliotecas de sockets do Windows sejam vinculadas corretamente.

## Contribuição
Contribuições são bem-vindas! Sinta-se à vontade para abrir problemas ou enviar solicitações de pull.
