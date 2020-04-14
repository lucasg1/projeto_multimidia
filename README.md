Deve-se primeiro compilar e depois executar os arquivos codificador.cpp, decodificador.cpp e psnr.py, nessa ordem.

Pode-se usar qualquer IDE para facilitar a execução desses arquivos, mas um passo-a-passo via linha de comando pode ser visto a seguir:

g++ codificador.cpp -o codificador.out  
./codificador.out

g++ decodificador.cpp -o decodificador.out  
./decodificador.out

python psnr.py