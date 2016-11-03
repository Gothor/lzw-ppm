# LSW-PPM

Bon, c'est pas encore le cas, mais un jour, peut-être, ce programme pourra
compresser des images au format ppm à l'aide de l'algorithme LZW
(Lempel-Ziv-Welch).

## Compilation

### Sous Linux

    $> make

### Sous Windows (avec mingw32)

    $> mingw32-make FORWINDOWS=1

## Utilisation

    $> lzw-ppm [options] src [dst]

Sous Windows, appelez lzw-ppm.exe.