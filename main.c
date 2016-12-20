#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include "lzw.h"
#include "ppm-exploder.h"

#define YES 'Y'
#define NO  'N'

/* Options */
static int uncompress = 0;
static int splitted = 0;
static int color_mode_hsv = 0;
static int given_dst_name = 0;

int confirm() {
    int c = fgetc(stdin);
    while(c != (YES + 0x20) && c != YES && c != (NO + 0x20) && c != NO) {
        fflush(stdin);
        c = fgetc(stdin);
    }
    if (c == (YES + 0x20) || c == YES)
        return 1;
    return 0;
}

char* remove_extension(const char* s) {
    int i, e = -1;
    char* res = NULL;
    
    for (i = 0; s[i] != '\0'; i++) {
        if (s[i] == '.') e = i;
    }
    if (e == -1) e = i;
    
    res = malloc(sizeof(*res) * (e + 1));
    assert(res);
    
    for (i = 0; i < e; i++)
        res[i] = s[i];
    res[e] = '\0';
    
    return res;
}

void print_usage() {
    fprintf(stderr, "Usage: lzw-ppm [options] src [dst]\n");
    fprintf(stderr, "options:\n");
    fprintf(stderr, "\t-h prints this help\n");
    fprintf(stderr, "\t-u to uncompress instead of compress src\n");
    fprintf(stderr, "\t-s to split the image before compression or to specify a"
        " splitted image image before decompression\n");
    fprintf(stderr, "\t-c to convert into HSV mode when splitting or to convert"
        " from HSV mode when merging\n");
}

int file_exists(char* s) {
    FILE* f = NULL;
    f = fopen(s, "r");
    if (f == NULL)
        return 0;
    fclose(f);
    return 1;
}

int main (int argc, char* argv[]) {
    int i, j;
    int opt = 0;
    char* src_name[5] = {NULL, NULL, NULL, NULL, NULL};
    char* dst_name[5] = {NULL, NULL, NULL, NULL, NULL};
    char* tmp_name[5] = {NULL, NULL, NULL, NULL, NULL};
    FILE* src[4] = {NULL, NULL, NULL, NULL};
    FILE* dst[4] = {NULL, NULL, NULL, NULL};
    const char* RGB_EXTENSIONS[4] = {".ippm", ".rppm", ".gppm", ".bppm"};
    const char* HSV_EXTENSIONS[4] = {".ippm", ".hppm", ".sppm", ".vppm"};
    int name_size = 0;
    
    // Lecture des options
    for (i = 0; i < argc; i++) {
        if (**(argv + i) == '-') {
            opt++;
            switch (*(*(argv + i) + 1)) {
            case 'h':
                print_usage();
                return -1;
            case 'u':
                uncompress = 1;
                break;
            case 's':
                splitted = 1;
                break;
            case 'c':
                color_mode_hsv = 1;
                break;
            }
        }
    }
    
    // Non... Pas comme ça...
    if (argc - opt < 2 || argc - opt > 3) {
        print_usage();
        return -1;
    }
    
    /***************************************************************************
        Splitted source/destination
    ***************************************************************************/
    if (splitted) {
        // Si on décompresse
        if (uncompress) {
            // C'était quoi les noms des fichiers déjà ?
            name_size = strlen(argv[opt + 1]);
            src_name[0] = malloc(sizeof(*src_name) * name_size);
            assert(src_name[0]);
            strcpy(src_name[0], argv[opt + 1]);
            tmp_name[0] = malloc(sizeof(*src_name) * name_size);
            assert(tmp_name[0]);
            strcpy(tmp_name[0], argv[opt + 1]);
            for (i = 1; i < 5; i++) {
                // Les fichiers sources
                src_name[i] = malloc(sizeof(*dst_name) * (name_size + 5));
                assert(src_name[i]);
                strcpy(src_name[i], src_name[0]);
                if (color_mode_hsv)
                    strcat(src_name[i], HSV_EXTENSIONS[i - 1]);
                else
                    strcat(src_name[i], RGB_EXTENSIONS[i - 1]);
                
                // Les fichiers temporaires
                tmp_name[i] = malloc(sizeof(*tmp_name) * (name_size + 10));
                assert(tmp_name[i]);
                strcpy(tmp_name[i], tmp_name[0]);
                strcat(tmp_name[i], ".tmp");
                if (color_mode_hsv)
                    strcat(tmp_name[i], HSV_EXTENSIONS[i - 1]);
                else
                    strcat(tmp_name[i], RGB_EXTENSIONS[i - 1]);
            }
            if (argc - opt == 3) {
                given_dst_name = 1;
                dst_name[0] = malloc(sizeof(*dst_name) * (strlen(argv[opt + 2])));
                assert(dst_name[0]);
                strcpy(dst_name[0], argv[opt + 2]);
            }
            else {
                dst_name[0] = malloc(sizeof(*dst_name) * (name_size + 5));
                assert(dst_name[0]);
                strcpy(dst_name[0], src_name[0]);
                strcat(dst_name[0], ".ppm");
            }
        
            // On vérifie que les fichiers n'existent pas déjà
            for (i = 1; i < 5; i++) {
                if (file_exists(tmp_name[i])) {
                    printf("Le fichier %s existe déjà. Êtes-vous sûr de vouloir"
                        " le remplacer ? [%c/%c] ", tmp_name[i], YES, NO);
                    if (!confirm()) {
                        for (j = 0; j < 5; j++) {
                            if (src_name[j] != NULL)
                                free(src_name[j]);
                            if (tmp_name[j] != NULL)
                                free(tmp_name[j]);
                        }
                        if (dst_name[0] != NULL)
                            free(dst_name[0]);
                        return -1;
                    }
                }
            }
            if (file_exists(dst_name[0])) {
                printf("Le fichier %s existe déjà. Êtes-vous sûr de vouloir"
                    " le remplacer ? [%c/%c] ", dst_name[0], YES, NO);
                if (!confirm()) {
                    for (j = 0; j < 5; j++) {
                        if (src_name[j] != NULL)
                            free(src_name[j]);
                        if (tmp_name[j] != NULL)
                            free(tmp_name[j]);
                    }
                    if (dst_name[0] != NULL)
                        free(dst_name[0]);
                    return -1;
                }
            }
            
            // On ouvre les sources
            for (i = 1; i < 5; i++) {
                src[i - 1] = fopen(src_name[i], "rb");
                if (src[i - 1] == NULL) {
                    fprintf(stderr, "Erreur lors de l'ouverture du fichier "
                        "source %s.\n", src_name[i]);
                    for (j = 0; j < 5; j++) {
                        if (src_name[j] != NULL)
                            free(src_name[j]);
                        if (tmp_name[j] != NULL)
                            free(tmp_name[j]);
                    }
                    if (dst[0] != NULL)
                        free(dst[0]);
                    for (j = 0; j < i - 1; j++)
                        if (src[j] != NULL)
                            fclose(src[j]);
                    return -1;
                }
            }
                
            // On ouvre les destinations (temporaires)
            for (i = 1; i < 5; i++) {
                dst[i - 1] = fopen(tmp_name[i], "wb");
                if (dst[i - 1] == NULL) {
                    fprintf(stderr, "Erreur lors de l'ouverture du fichier "
                        "destination temporaire %s.\n", tmp_name[i]);
                    for (j = 0; j < 5; j++) {
                        if (src_name[j] != NULL)
                            free(src_name[j]);
                        if (tmp_name[j] != NULL)
                            free(tmp_name[j]);
                    }
                    if (dst[0] != NULL)
                        free(dst[0]);
                    for (j = 0; j < 4; j++)
                        if (src[j] != NULL)
                            fclose(src[j]);
                    for (j = 0; j < i - 1; j++)
                        if (dst[j] != NULL)
                            fclose(dst[j]);
                    return -1;
                }
            }
            
            printf("Décompression de %s en cours...\n", dst_name[0]);
            for (i = 0; i < 4; i++)
                unlzw_ppm(src[i], dst[i]);
            
            // On ferme nos fichiers
            for (i = 0; i < 4; i++) {
                if (src[i] != NULL)
                    fclose(src[i]);
                if (dst[i] != NULL)
                    fclose(dst[i]);
            }
            
            // On ouvre les sources (temporaires)
            for (i = 1; i < 5; i++) {
                src[i - 1] = fopen(tmp_name[i], "rb");
                if (src[i - 1] == NULL) {
                    fprintf(stderr, "Erreur lors de l'ouverture du fichier "
                        "source temporaire %s.\n", tmp_name[i]);
                    for (j = 0; j < 5; j++) {
                        if (src_name[j] != NULL)
                            free(src_name[j]);
                        if (tmp_name[j] != NULL)
                            free(tmp_name[j]);
                    }
                    for (j = 0; j < i - 1; j++)
                        if (src[j] != NULL)
                            fclose(src[j]);
                    return -1;
                }
            }
            
            // On ouvre la destination
            dst[0] = fopen(dst_name[0], "wb");
            if (dst[0] == NULL) {
                fprintf(stderr, "Erreur lors de l'ouverture du fichier "
                    "destination %s.\n", dst_name[0]);
                for (j = 0; j < 5; j++) {
                    if (src_name[j] != NULL)
                        free(src_name[j]);
                    if (tmp_name[j] != NULL)
                        free(tmp_name[j]);
                }
                for (j = 0; j < 4; j++)
                    if (src[j] != NULL)
                        fclose(src[j]);
                if (dst[0] != NULL)
                    fclose(dst[j]);
                return -1;
            }
            
            merge_ppm(src[0], src[1], src[2], src[3], dst[0], color_mode_hsv);
            
            // On ferme nos fichiers
            for (i = 0; i < 4; i++) {
                if (src[i] != NULL)
                    fclose(src[i]);
                if (tmp_name[i + 1] != NULL)
                    remove(tmp_name[i + 1]);
                if (dst[i] != NULL)
                    fclose(dst[i]);
            }
            
            // On libère les noms... (Envolez-vous !)
            for (j = 0; j < 5; j++) {
                if (src_name[j] != NULL)
                    free(tmp_name[j]);
                if (tmp_name[j] != NULL)
                    free(tmp_name[j]);
            }
            if (dst_name[0] != NULL)
                free(dst_name[0]);
        }
        // Si on compresse
        else {
            // C'était quoi les noms des fichiers déjà ?
            src_name[0] = argv[opt + 1];
            if (argc - opt == 3) {
                given_dst_name = 1;
                tmp_name[0] = remove_extension(argv[opt + 2]);
                dst_name[0] = remove_extension(argv[opt + 2]);
            }
            else {
                tmp_name[0] = remove_extension(src_name[0]);
                dst_name[0] = remove_extension(src_name[0]);
            }
            name_size = strlen(tmp_name[0]);
            for (i = 1; i < 5; i++) {
                // Les fichiers temporaires
                tmp_name[i] = malloc(sizeof(*tmp_name) * (name_size + 10));
                assert(tmp_name[i]);
                strcpy(tmp_name[i], tmp_name[0]);
                strcat(tmp_name[i], ".tmp");
                if (color_mode_hsv)
                    strcat(tmp_name[i], HSV_EXTENSIONS[i - 1]);
                else
                    strcat(tmp_name[i], RGB_EXTENSIONS[i - 1]);
                
                // Les fichiers destinations
                dst_name[i] = malloc(sizeof(*dst_name) * (name_size + 5));
                assert(dst_name[i]);
                strcpy(dst_name[i], dst_name[0]);
                if (color_mode_hsv)
                    strcat(dst_name[i], HSV_EXTENSIONS[i - 1]);
                else
                    strcat(dst_name[i], RGB_EXTENSIONS[i - 1]);
            }
        
            // On vérifie que les fichiers n'existent pas déjà
            for (i = 1; i < 5; i++) {
                if (file_exists(tmp_name[i])) {
                    printf("Le fichier %s existe déjà. Êtes-vous sûr de vouloir"
                        " le remplacer ? [%c/%c] ", tmp_name[i], YES, NO);
                    if (!confirm()) {
                        for (j = 0; j < 5; j++) {
                            if (tmp_name[j] != NULL)
                                free(tmp_name[j]);
                            if (dst_name[j] != NULL)
                                free(dst_name[j]);
                        }
                        return -1;
                    }
                }
            }
            for (i = 1; i < 5; i++) {
                if (file_exists(dst_name[i])) {
                    printf("Le fichier %s existe déjà. Êtes-vous sûr de vouloir"
                        " le remplacer ? [%c/%c] ", dst_name[i], YES, NO);
                    if (!confirm()) {
                        for (j = 0; j < 5; j++) {
                            if (tmp_name[j] != NULL)
                                free(tmp_name[j]);
                            if (dst_name[j] != NULL)
                                free(dst_name[j]);
                        }
                        return -1;
                    }
                }
            }
        
            // On ouvre la source
            src[0] = fopen(src_name[0], "rb");
            if (src[0] == NULL) {
                fprintf(stderr, "Erreur lors de l'ouverture du fichier source "
                    "%s.\n", src_name[0]);
                free(tmp_name[0]);
                return -1;
            }
                
            // On ouvre les destinations (temporaires)
            for (i = 1; i < 5; i++) {
                dst[i - 1] = fopen(tmp_name[i], "wb");
                if (dst[i - 1] == NULL) {
                    fprintf(stderr, "Erreur lors de l'ouverture du fichier "
                        "destination temporaire %s.\n", tmp_name[i]);
                    for (j = 0; j < 5; j++) {
                        if (tmp_name[j] != NULL)
                            free(tmp_name[j]);
                        if (dst_name[j] != NULL)
                            free(dst_name[j]);
                    }
                    for (j = 0; j < i - 1; j++)
                        if (dst[j] != NULL)
                            fclose(dst[j]);
                    fclose(src[0]);
                    return -1;
                }
            }
            
            explode_ppm(src[0], dst[0], dst[1], dst[2], dst[3], color_mode_hsv);
            
            // On ferme nos fichiers
            if (src[0] != NULL)
                fclose(src[0]);
            for (i = 0; i < 4; i++)
                if (dst[i] != NULL)
                    fclose(dst[i]);
            
            // On ouvre les sources (temporaires)
            for (i = 1; i < 5; i++) {
                src[i - 1] = fopen(tmp_name[i], "rb");
                if (src[i - 1] == NULL) {
                    fprintf(stderr, "Erreur lors de l'ouverture du fichier "
                        "source temporaire %s.\n", tmp_name[i]);
                    for (j = 0; j < 5; j++) {
                        if (tmp_name[j] != NULL)
                            free(tmp_name[j]);
                        if (dst_name[j] != NULL)
                            free(dst_name[j]);
                    }
                    for (j = 0; j < i - 1; j++)
                        if (src[j] != NULL)
                            fclose(src[j]);
                    return -1;
                }
            }
            
            // On ouvre les destinations
            for (i = 1; i < 5; i++) {
                dst[i - 1] = fopen(dst_name[i], "wb");
                if (dst[i - 1] == NULL) {
                    fprintf(stderr, "Erreur lors de l'ouverture du fichier "
                        "destination %s.\n", dst_name[i]);
                    for (j = 0; j < 5; j++) {
                        if (tmp_name[j] != NULL)
                            free(tmp_name[j]);
                        if (dst_name[j] != NULL)
                            free(dst_name[j]);
                    }
                    for (j = 0; j < 4; j++)
                        if (src[j] != NULL)
                            fclose(src[j]);
                    for (j = 0; j < i - 1; j++)
                        if (dst[j] != NULL)
                            fclose(dst[j]);
                    return -1;
                }
            }
            
            printf("Compression de %s en cours...\n", src_name[0]);
            for (i = 0; i < 4; i++)
                lzw_ppm(src[i], dst[i]);
            
            // On ferme nos fichiers
            for (i = 0; i < 4; i++) {
                if (src[i] != NULL)
                    fclose(src[i]);
                if (tmp_name[i + 1] != NULL)
                    remove(tmp_name[i + 1]);
                if (dst[i] != NULL)
                    fclose(dst[i]);
            }
            
            // On libère les noms... (Envolez-vous !)
            for (j = 0; j < 5; j++) {
                if (tmp_name[j] != NULL)
                    free(tmp_name[j]);
                if (dst_name[j] != NULL)
                    free(dst_name[j]);
            }
        }
    }
    /***************************************************************************
        Not splitted source/destination
    ***************************************************************************/
    else {
        // C'était quoi les noms des fichiers déjà ?
        src_name[0] = argv[opt + 1];
        if (argc - opt == 3) {
            given_dst_name = 1;
            dst_name[0] = malloc(sizeof(*dst_name) * (strlen(argv[opt + 2]) + 5));
            assert(dst_name[0]);
            strcpy(dst_name[0], argv[opt + 2]);
        }
        else {
            dst_name[0] = remove_extension(src_name[0]);
        }
        
        // On nomme la destination correctement
        if (!given_dst_name) {
            if (uncompress)
                strcat(dst_name[0], ".ppm");
            else
                strcat(dst_name[0], ".xppm");
        }
        
        // On ouvre la source
        src[0] = fopen(src_name[0], "rb");
        if (src[0] == NULL) {
            fprintf(stderr, "Erreur lors de l'ouverture du fichier source %s."
                "\n", src_name[0]);
            free(dst_name[0]);
            return -1;
        }
        
        // On vérifie que le fichier n'existe pas déjà (ce serait bête...)
        if (file_exists(dst_name[0])) {
            printf("Le fichier %s existe déjà. Êtes-vous sûr de vouloir le "
                "remplacer ? [%c/%c] ", dst_name[0], YES, NO);
            if (!confirm()) {
                free(dst_name[0]);
                fclose(src[0]);
                return -1;
            }
        }
        
        // On ouvre la destination
        dst[0] = fopen(dst_name[0], "wb");
        if (dst[0] == NULL) {
            fprintf(stderr, "Erreur lors de l'ouverture du fichier destination "
                "%s.\n", dst_name[0]);
            free(dst_name[0]);
            fclose(src[0]);
            return -1;
        }
        
        // On bosse (oui, on dirait pas... Mais si... on bosse...)
        if (uncompress) {
            printf("Décompression de %s en cours...\n", src_name[0]);
            unlzw_ppm(src[0], dst[0]);
        }
        else {
            printf("Compression de %s en cours...\n", src_name[0]);
            lzw_ppm(src[0], dst[0]);
        }
        
        // On ferme ! Revenez demain !
        fclose(src[0]);
        fclose(dst[0]);
        
        if (dst_name != NULL)
            free(dst_name[0]);
    }
    
    return 0;
}
