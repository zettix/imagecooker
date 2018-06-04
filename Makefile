imagecooker: main.o pngreader.o tiffreader.o emitpngs.o emitdata.o
	gcc -g main.o pngreader.o tiffreader.o emitpngs.o emitdata.o -ltiff -lpng -o imagecooker

main.o: main.c pngreader.h emitpngs.h emitdata.h
	gcc -g -c main.c

pngreader.o: pngreader.c pngreader.h
	gcc -g -c pngreader.c

tiffreader.o: tiffreader.c tiffreader.h
	gcc -g -c tiffreader.c

emitpngs.o: emitpngs.c emitpngs.h
	gcc -g -c emitpngs.c

emitdata.o: emitdata.c emitdata.h
	gcc -g -c emitdata.c

clean:
	@rm *.o imagecooker
