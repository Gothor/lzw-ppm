#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>

static int delete = 0;

int main(int argc, char* argv[]) {
    int i;
    int opt = 0;
    FILE* f = NULL;
    char* filename = NULL;
    
    for (i = 0; i < argc; i++) {
        if (**(argv + i) == '-') {
            opt++;
            switch (*(*(argv + i) + 1)) {
            case 'h':
            usage:
                fprintf(stderr, "Usage: %s [options] src\n", filename);
                fprintf(stderr, "Create or delete src. Creates it by default.\n");
                fprintf(stderr, "options:\n");
                fprintf(stderr, "\t-h prints this help\n");
                fprintf(stderr, "\t-d delete src\n");
                return -1;
            case 'd':
                delete = 1;
                break;
            }
        }
    }
    
    if (argc - opt < 2) {
        goto usage;
    }
    
    filename = argv[argc - 1];
    
    if (delete) {
        if ((i = remove(filename)) == 0) {
            printf("Le fichier %s a été supprimé avec succès.\n", filename);
            return 0;
        }
        
        fprintf(stderr, "Erreur lors de la suppression du fichier %s.\n%s\n", filename, strerror(errno));
        return -1;
    }
    
    f = fopen(filename, "r");
    if (f != NULL) {
        fprintf(stderr, "Le fichier %s existe déjà.\n", filename);
        fclose(f);
        return -1;
    }
    
    f = fopen(filename, "w");
    if (f == NULL) {
        fprintf(stderr, "Erreur lors de la création de %s.\n", filename);
        return -1;
    }
    fclose(f);
    
    printf("Le fichier %s a bien été créé.\n", filename);
    
    return 0;
}