# ServidorHTTP
> Projeto prático da disciplina de Redes de computadores 2021/1 - Universidade Federal do Pampa - UNIPAMPA/Bagé-RS
### Integrantes: Éric D. S. Rosso, Gabriel Correia, Kelvin Clovis
#### “Este projeto foi desenvolvido integralmente pela equipe, sem ajuda não autorizada de alunos não membros do projeto no processo de codificação com base no projeto de Özgür Hepsağ. Multi-threaded HTTP Server. Disponível em: https://github.com/ozgurhepsag/Multi-threaded-HTTP-Server. Acesso em: 20 de Agosto de 2021.”

Testado em ambiente Linux Ubuntu 20.04 (64 Bits)

As informações referentes as análises estão disponibilizadas na pasta de imagens

Caso queira testar todas as exceções e o tratamento das mesmas, basta executar o arquivo "Script.sh" 

o arquivo "Teste.html" é um arquivo html qualquer usado simplesmente para testar as requisições

 Para execução do projeto:
   * alterar a variável "PATH" para ser condizente ao ambiente de execução
   * Compilar o servidor : gcc -pthread -o server serverhttp10.c 
   * Inicializar o servidor: ./server
   * Fechar o servidor: CTRL + C.


 Acesso no navegador: 
 
 
   * http://localhost:8888/teste.html

 Acesso no terminal:
 
   * curl "http://localhost:8888/teste.html" --http1.0
