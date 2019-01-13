Sposób kompilacji programu:

gcc -o <nazwa.out> go.c -lpthread -Wall
gcc -o <nazwa2.out> klient.c -Wall


Sposób uruchomienia programu:

serwer:
./nazwa.out <nr_gniazda_1> <nr_gniazda_2>
 
klient:
./nazwa2.out <nr_gniazda_1> <nr_gniazda_2> <adres ip serwera>
