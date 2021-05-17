#include "anlex.c"

void element(int synchset[], int size);
void array(int synchset[], int size);
void arr__(int synchset[], int size);
void elementList(int synchset[], int size);
void elementL__(int synchset[], int size);
void object(int synchset[], int size);
void obj__(int synchset[], int size);
void attributeList(int synchset[], int size);
void attributeL__(int synchset[], int size);
void attribute(int synchset[], int size);
void attributeName(int synchset[], int size);
void attributeValue(int synchset[], int size);
int aceptar = 1;

void match (int expToken) {
    if (t.compLexico = expToken) {
    	sigLexema();
    }
}

int in(int vector[], int tam, int comp){
    int i = 0;
    while (i < tam) {
        if (vector[i] == comp) {
            return 1;
        }
        i++;
    }
    return 0;
}

void scanTo(int synchset[], int size) {
    while (in(synchset, size, t.compLexico) == 0 && t.compLexico != EOF) {
    	sigLexema();
    }
    sigLexema();
}

void sintaxError(){
    aceptar = 0;
    printf("\nLin %d: Error de sintaxis. No se esperaba %s. ", numLinea, t.comp);
}

void checkInput(int primero[], int siguiente[], int tam1, int tam2) {
    int vunion[tam1+tam2];
    int i = 0, j = 0;
    while (i < tam1) {
        if (t.compLexico == primero[i]) { return;}
        i++;
    }
    i = 0;
    while (i < tam1) {
        vunion[i] = primero[i];
        i++;
    }
    while (j < tam2) {
        vunion[i] = siguiente[j];
        i++;
        j++;
    }
    sintaxError();
    scanTo(vunion, tam1+tam2);
}

// json --> element
void json(){
    int primero[2] = {L_CORCHETE, L_LLAVE};
    int siguiente[1] = {EOF};
    checkInput(primero, siguiente, 2, 1);
    element(siguiente, 1);
}

//element --> object | array
void element(int synchset[], int size){
    int primero[2] = {L_CORCHETE, L_LLAVE};
    int siguiente[4] = {COMA, R_CORCHETE, R_LLAVE, EOF};
    int i = 0, is = 0;
    checkInput(primero, synchset, 2, size);
    while (i < size) {
        if (t.compLexico == synchset[i]) {
            is = 1;
        }
        i++;
    }
    if (is == 0) {
        if (t.compLexico == L_LLAVE) {
            object(siguiente, 4);
        }
        else if (t.compLexico == L_CORCHETE) {
            array(siguiente, 4);
        }
    }
    else {
        sintaxError();
    }
    checkInput(siguiente, primero, 4, 2);
}

//object --> { obj__
void object(int synchset[], int size){
    int primero[1] = {L_LLAVE};
    int siguiente[4] = {COMA, R_CORCHETE, R_LLAVE, EOF};
    int i = 0, is = 0;
    checkInput(primero, synchset, 1, size);
    while (i < size) {
        if (t.compLexico == synchset[i]) {
            is = 1;
        }
        i++;
    }
    if (is == 0) {
        if (t.compLexico == L_LLAVE) {
            match(L_LLAVE);
            obj__(siguiente, 4);
        }
    }
    else {
        sintaxError();
    }
    checkInput(siguiente, primero, 4, 1);
}

//obj__ --> attributeList} | }
void obj__(int synchset[], int size){
    int primero[2] = {R_LLAVE, STRING};
    int siguiente[4] = {COMA, R_CORCHETE, R_LLAVE, EOF};
    int i = 0, is = 0;
    checkInput(primero, synchset, 2, size);
    while (i < size) {
        if (t.compLexico == synchset[i]) {
            is = 1;
        }
        i++;
    }
    if (is == 0) {
        if (t.compLexico == R_LLAVE) {
            match(R_LLAVE);
        }
        else if (t.compLexico == STRING) {
            attributeList(siguiente, 4);
            match(R_LLAVE);
        }
    }
    else {
        sintaxError();
    }
    checkInput(siguiente, primero, 4, 2);
}

//array --> [ arr__ 
void array(int synchset[], int size){
    int primero[1] = {L_CORCHETE};
    int siguiente[4] = {COMA, R_CORCHETE, R_LLAVE, EOF};
    int i = 0, is = 0;
    checkInput(primero, synchset, 1, size);
    while (i < size) {
        if (t.compLexico == synchset[i]) {
            is = 1;
        }
        i++;
    }
    if (is == 0) {
        if (t.compLexico == L_CORCHETE) {
            match(L_CORCHETE);
            arr__(siguiente, 4);
        }
    }
    else {
        sintaxError();
    }
    checkInput(siguiente, primero, 4, 1);
}

//arr__ --> elementList] | ]
void arr__(int synchset[], int size){
    int primero[3] = {R_CORCHETE, L_CORCHETE, L_LLAVE};
    int siguiente[4] = {COMA, R_CORCHETE, R_LLAVE, EOF};
    checkInput(primero, synchset, 3, size);
    if (t.compLexico == R_CORCHETE) {
        match(R_CORCHETE);
    }
    else if (t.compLexico == L_LLAVE || t.compLexico == L_CORCHETE) {
        elementList(siguiente, 4);
        match(R_CORCHETE);
    }
    checkInput(siguiente, primero, 4, 3);
}

//attributeList --> attribute attributeL__
void attributeList(int synchset[], int size){
    int primero[1] = {STRING};
    int siguiente[1] = {R_LLAVE};
    int i = 0, is = 0;
    checkInput(primero, synchset, 1, size);
    while (i < size) {
        if (t.compLexico == synchset[i]) {
            is = 1;
        }
        i++;
    }
    if (is == 0) {
        if (t.compLexico == STRING) {
            attribute(siguiente, 1);
            attributeL__(siguiente, 1);
        }
    }
    else {
        sintaxError();
    }
    checkInput(siguiente, primero, 1, 1);
}

//attributeL__ --> , attribute attributeL__ | E (empty string)
void attributeL__(int synchset[], int size){
    if (t.compLexico == R_LLAVE){
        return;
    }

    int primero[1] = {COMA};
    int siguiente[1] = {R_LLAVE};
    int i = 0, is = 0;
    checkInput(primero, synchset, 1, size);
    while (i < size) {
        if (t.compLexico == synchset[i]) {
            is = 1;
        }
        i++;
    }
    if (is == 0) {
        if (t.compLexico == COMA) {
            match(COMA);
            attribute(siguiente, 1);
            attributeL__(siguiente, 1);
        }
    }
    checkInput(siguiente, primero, 1, 1);
}

//elementList -> element elementL__
void elementList(int synchset[], int size){
    int primero[2] = {L_LLAVE, L_CORCHETE};
    int siguiente[1] = {R_CORCHETE};
    int i = 0, is = 0;
    checkInput(primero, synchset, 2, size);
    while (i < size) {
        if (t.compLexico == synchset[i]) {
            is = 1;
        }
        i++;
    }
    if (is == 0) {
        if (t.compLexico == L_CORCHETE || t.compLexico == L_LLAVE) {
            element(siguiente, 1);
            elementL__(siguiente, 1);
        }
    }
    else {
        sintaxError();
    }
    checkInput(siguiente, primero, 1, 2);
}

//elementL__ -> , element elementL__ | E (empty string)
void elementL__(int synchset[], int size){
    if(t.compLexico == R_CORCHETE){ 
       return;
    }
    int primero[1] = {COMA};
    int siguiente[1] = {R_CORCHETE};
    checkInput(primero, synchset, 1, size);
    if (t.compLexico == COMA) {
        match(COMA);
        element(siguiente, 1);
        elementL__(siguiente, 1);
    }
    checkInput(siguiente, primero, 1, 1);
}

//attribute -> attributeName : attributeValue
void attribute(int synchset[], int size){
    int primero[1] = {STRING};
    int siguiente[2] = {COMA, R_LLAVE};
    int i = 0, is = 0;
    checkInput(primero, synchset, 1, size);
    while (i < size) {
        if (t.compLexico == synchset[i]) {
            is = 1;
        }
        i++;
    }
    if (is == 0) {
        if (t.compLexico == STRING) {
            attributeName(siguiente, 2);
            match(DOS_PUNTOS);
            attributeValue(siguiente, 2);
        }
    }
    else {
        sintaxError();
    }
    checkInput(siguiente, primero, 2, 1);
}

//attributeName -> string
void attributeName(int synchset[], int size){
    int primero[1] = {STRING};
    int siguiente[1] = {DOS_PUNTOS};
    int i = 0, is = 0;
    checkInput(primero, synchset, 1, size);
    while (i < size) {
        if (t.compLexico == synchset[i]) {
            is = 1;
        }
        i++;
}
    if (is == 0) {
        if (t.compLexico == STRING){
            match(STRING);
        }
    }
    else {
        sintaxError();
    }
    checkInput(siguiente, primero, 1, 1);
}

//attributeValue -> element | string | num | true | false | null
void attributeValue(int synchset[], int size){
    int primero[7] = {L_LLAVE, L_CORCHETE, STRING, NUMBER, PR_TRUE, PR_FALSE, PR_NULL};
    int siguiente[2] = {COMA, R_LLAVE};
    int i = 0, is = 0;
    checkInput(primero, synchset, 7, size);
    while (i < size) {
        if (t.compLexico == synchset[i]) {
            is = 1;
        }
        i++;
    }
    if (is == 0) {
        if (t.compLexico == L_LLAVE || t.compLexico == L_CORCHETE) {
            element(siguiente, 2);
        }
        else if (t.compLexico == STRING) {
            match(STRING);
        }
        else if (t.compLexico == NUMBER) {
            match(NUMBER);
        }
        else if (t.compLexico == PR_TRUE) {
            match(PR_TRUE);
        }
        else if (t.compLexico == PR_FALSE) {
            match(PR_FALSE);
        }
        else if (t.compLexico == PR_NULL) {
            match(PR_NULL);
        }
}
    else {
        sintaxError();
    }
    checkInput(siguiente, primero, 2, 7);
}


int main(int argc,char* args[]){

    if(argc > 1){
        if (!(archivo=fopen(args[1],"rt"))){
            printf("Archivo no encontrado.\n");
            exit(1);
        }
        sigLexema();
        json();
        if (aceptar == 1) {
            printf("Analisis exitoso. El fuente es sintacticamente correcto\n");
        }
        fclose(archivo);
    }
    else{
        printf("Debe pasar como parametro el path al fuente.\n");
        exit(1);
    }
    return 0;
}
