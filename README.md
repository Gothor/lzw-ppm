# LSW-PPM

Bon, c'est pas encore le cas, mais un jour, peut-être, ce programme pourra
compresser des images au format ppm à l'aide de l'algorithme LZW
(Lempel-Ziv-Welch).

## Compilation

### Sous Linux

    $> make

### Sous Windows (avec mingw32)

    $> mingw32-make FORWINDOWS=1
    
### Mode Debug

Il est possible de compiler le programme en mode debug. Pour cela utilisez :

    $> make debug
    
ou, sous Windows (avec mingw32) :

    $> mingw32-make debug FORWINDOWS=1

## Utilisation

Pour l'instant il n'y a pas vraiment d'options... Ni de fichier destination
d'ailleurs. En fait, le programme actuel ne fait rien du tout ! Par contre il
vous prévient si vous n'utilisez pas la bonne syntaxe pour l'exécuter.

    $> lzw-ppm [options] src [dst]

Sous Windows (avec mingw32), appelez :

    $> lzw-ppm.exe [options] src [dst]