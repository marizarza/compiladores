#include "ansint.c"


/**Prototipos de funciones**/

void sintaxis_trad();
void element_trad();
void array_trad();
void arrayB_trad();
void element_list_trad();
void element_listB_trad();
void object_trad();
void objectB_trad();
void attribute_trad();
void attribute_list_trad();
void attribute_listB_trad();
void attribute_name_trad();
void attribute_value_trad();
int espacios=0;

/**Funcion auxiliar**/

void string_sin_comillas(char* );
void imprimir_espacios();


// sintaxis_trad -> element.trad 
void sintaxis_trad(){
    element_trad();
}

//element.trad -> object.trad | array.trad
void element_trad(){
    if(t.compLexico == L_LLAVE){
        object_trad();
    }
    else if(t.compLexico == L_CORCHETE){
        array_trad();
    }
}

//array.trad -> arrayB.trad 
void array_trad(){  
    if(t.compLexico == L_CORCHETE){
        match(L_CORCHETE);
        arrayB_trad();
    }
}

//arrayA.trad -> <item> element_list.trad </item> | ε
void arrayB_trad(){
    if(t.compLexico == L_CORCHETE || t.compLexico == L_LLAVE){
		fputs("\n",salida);
        imprimir_espacios();
		fputs("<item>\n",salida);
		espacios+=1;
        element_list_trad();
        match(R_CORCHETE);
        imprimir_espacios();
		fputs("</item>\n",salida);
    }
    else if(t.compLexico == R_CORCHETE){
		espacios-=1;
        match(R_CORCHETE);
    }
}

//element_list.trad -> element.trad element_listB.trad 
void element_list_trad(){
    if(t.compLexico == L_CORCHETE || t.compLexico == L_LLAVE){
        element_trad();
        element_listB_trad();
        
    }
}

//element_listB.trad -> </item> <item> element.trad element_listB.trad | ε
void element_listB_trad(){
    if(t.compLexico == COMA){  
        match(COMA);
		imprimir_espacios();
		fputs("</item>\n",salida);
		imprimir_espacios();
		fputs("<item>\n",salida);
		espacios+=1;
        element_trad();
        element_listB_trad();     
    }
}

//object.trad -> objectB.trad 
void object_trad(){
    if(t.compLexico == L_LLAVE){
        match(L_LLAVE);
        objectB_trad();
    }
}

//objectB.trad -> attribute_list.trad | ε
void objectB_trad(){
    if(t.compLexico == LITERAL_CADENA){
        attribute_list_trad();
        match(R_LLAVE);

    }
    else if(t.compLexico == R_LLAVE){
        match(R_LLAVE);
    }
}

//attribute_list.trad -> attribute.trad attribute_listB.trad
void attribute_list_trad(){
    if(t.compLexico == LITERAL_CADENA){
        attribute_trad();
        attribute_listB_trad();
    }
}

//attribute_listB.trad ->attribute.trad attribute_listB.trad | ε
void attribute_listB_trad(){
    if (t.compLexico == R_LLAVE){
        espacios-=1;
		return;
    }
    if(t.compLexico == COMA){
        match(COMA);
        attribute_trad();
        attribute_listB_trad();
    }
}

//attribute.trad -> <attribute_name.trad> attribute_value.trad </attribute_name.trad>
void attribute_trad(){
    if(t.compLexico == LITERAL_CADENA){
		imprimir_espacios();
        char lexema[TAMLEX];
        strcpy(lexema,t.lexema);
		fputs("<",salida);
        attribute_name_trad();
		fputs(">",salida);
        match(DOS_PUNTOS);
        attribute_value_trad();
		fputs("</",salida);
        string_sin_comillas(lexema);
		fputs(">\n",salida);		
    }
}

//attribute_name.trad -> string.trad_sin_comilla
void attribute_name_trad(){
    if(t.compLexico == LITERAL_CADENA){    
        string_sin_comillas(t.lexema);
        match(LITERAL_CADENA);
    }
}

//attribute_value.trad -> element_trad | string | number | true | false | null

void attribute_value_trad(){
    if(t.compLexico == L_CORCHETE || t.compLexico == L_LLAVE){
		espacios += 1;
        element_trad();
    }
    else if(t.compLexico == LITERAL_CADENA){
		fputs(t.lexema, salida);
        match(LITERAL_CADENA);
    }
    else if(t.compLexico == LITERAL_NUM){
		fputs(t.lexema, salida);
        match(LITERAL_NUM);
    }
    else if(t.compLexico == PR_TRUE){
		fputs(t.lexema, salida);
        match(PR_TRUE);
    }
    else if(t.compLexico == PR_FALSE){
		fputs(t.lexema, salida);
        match(PR_FALSE);
    }
    else if(t.compLexico == PR_NULL){
		fputs(t.lexema, salida);
        match(PR_NULL);
    }
}

void string_sin_comillas(char* cadena){
    char string[TAMLEX];
	char cad[TAMLEX] = "";
    strcpy(string,cadena);
    int i=1;
    while(string[i] != '"'){
		cad[i-1] = string[i];
        i++;
    }
	fputs(cad, salida);
}

void imprimir_espacios(){
    int i;
    for (i=1;i<=espacios;i++){
        fputs("\t", salida);
    }
}


/*Main*/
int main (int argc,char* args[]){
    if(argc > 1)
    {
        if (!(archivo=fopen(args[1],"rt")))
        {
            printf("Fuente no encontrada.\n");
            exit(1);
        }
        sigLexema();
        json();
        if(aceptar){
            fclose(archivo);
            archivo=fopen(args[1],"rt");
			salida=(fopen("output.xml","w"));
            sigLexema();
            sintaxis_trad();
        }
		fclose(salida);
    }else{
        printf("Debe pasar como parametro el path al archivo fuente.\n");
        exit(1);
    }

    return 0;
}
