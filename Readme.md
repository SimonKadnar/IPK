Autor:
Šimon Kadnár xkadna00

Popis:
Program vytvára server ktorí funguje len pre localhost. Server komunikuje pomocov HTTP a vie vrátiť 3 informácie o systéme 
(jeho hostname, cpu-name, a cpu vytaženie).

Použitie: 
./hinfosvc "číslo portu"

./hinfosvc 12345

prípradne ./hinfosvc 12345 &
curl http://localhost:12345/hostname
curl http://localhost:12345/cpu-name
curl http://localhost:12345/load