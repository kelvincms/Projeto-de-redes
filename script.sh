#!/bin/bash
SERVER_ADDRESS=http://localhost:8888/teste.html
SERVER_ADDRESS_ERROR_1=http://localhost:8888/teste.aah
SERVER_ADDRESS_ERROR_2=http://localhost:8888/aah.html
echo "Fazendo requisicoes no endereco :  $SERVER_ADDRESS"
#Quantidade de requisições: 20

#Requisições para testar o limite de 10
curl  $SERVER_ADDRESS --http1.0 & 
curl  $SERVER_ADDRESS --http1.0 & 
curl  $SERVER_ADDRESS --http1.0 &
curl  $SERVER_ADDRESS --http1.0 & 
curl  $SERVER_ADDRESS --http1.0 &
curl  $SERVER_ADDRESS --http1.0 &
curl  $SERVER_ADDRESS --http1.0 &
curl  $SERVER_ADDRESS --http1.0 &
curl  $SERVER_ADDRESS --http1.0 &
curl  $SERVER_ADDRESS --http1.0 &
curl  $SERVER_ADDRESS --http1.0 &
curl  $SERVER_ADDRESS --http1.0 &
curl  $SERVER_ADDRESS --http1.0 &
#Requisições para testar com protocolo errado
curl  $SERVER_ADDRESS &
curl  $SERVER_ADDRESS &
curl  $SERVER_ADDRESS &
#Requisições para testar com extensão de arquivo errado
curl  $SERVER_ADDRESS_ERROR_1 --http1.0 &
curl  $SERVER_ADDRESS_ERROR_1 --http1.0 &
#Requisições para testar com nome de arquivo errado
curl  $SERVER_ADDRESS_ERROR_2 --http1.0 &
curl  $SERVER_ADDRESS_ERROR_2 --http1.0 &
wait