clear all;clc;%fclose(instrfind);

% entradas digitais
VENT = hex2dec('00');
VSAI = hex2dec('01');
AQUE = hex2dec('02');
RESF = hex2dec('03');
AGIT = hex2dec('04');
% saídas digitais
BALT = hex2dec('10');
BBAI = hex2dec('11');
TMIN  =hex2dec('12');
TMAX = hex2dec('13');
% entradas analógicas
ATMI = hex2dec('20');
ATMA = hex2dec('21');
% saídas analógicas
VOLU = hex2dec('30');
TEMP = hex2dec('31');

LIGA = hex2dec('01');
DESLIGA = hex2dec('00');

% Comunicacao Serial
s2 = serial('COM8','BaudRate',19200);
fopen(s2);

% fwrite(s2, hex2dec('FF'));
% fwrite(s2, hex2dec('FF'));
% fwrite(s2, hex2dec('FF'));

fwrite(s2, ATMA);
valori = 35;

aux=dec2hex(valori);
if length(aux)>2
    fwrite(s2, hex2dec(aux(1:2)));
    fwrite(s2, hex2dec(aux(3:4)));
else
    fwrite(s2, hex2dec('00'));
    fwrite(s2, hex2dec(aux(1:2)));
end

fwrite(s2,VENT);
fwrite(s2,LIGA);

fwrite(s2, VENT);
fwrite(s2, DESLIGA);


fwrite(s2, VSAI);
fwrite(s2, LIGA);

fwrite(s2,VSAI);
fwrite(s2,DESLIGA);


fwrite(s2, AQUE);
fwrite(s2, LIGA);

fwrite(s2, AQUE);
fwrite(s2, DESLIGA);

fwrite(s2, RESF);
fwrite(s2, LIGA);

fwrite(s2, RESF);
fwrite(s2, DESLIGA);


fwrite(s2, AGIT);
fwrite(s2, LIGA);

fwrite(s2, AGIT);
fwrite(s2, DESLIGA);


fwrite(s2,TMAX);
valorl = fread(s2, 1)

fwrite(s2,BALT);
valorl = fread(s2, 1)

fwrite(s2, BBAI);
valorl = fread(s2, 1)

fwrite(s2, VOLU);
valorh = fread(s2, 1);
valorl = fread(s2, 1);
volume = valorh*100+valorl

fwrite(s2, TEMP);
valorh = fread(s2, 1);
valorl = fread(s2, 1);
temperatura = valorh*100+valorl

