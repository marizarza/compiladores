/*********** Inclusión de cabecera **************/
#include "anlex.h"

/************* Variables globales **************/

token t;				    // token global para recibir componentes del Analizador Lexico


/***** Variables para el analizador lexico *****/

FILE *archivo;			    // Fuente json
FILE *salida;				// Salida del analizador lexico
char id[TAMLEX];		    // Utilizado por el analizador lexico
int numLinea=1;			    // Numero de Linea

/**************** Funciones **********************/


// Rutinas del analizador lexico

void error(const char* mensaje)
{
	printf("\nLin %d: Error Lexico. %s",numLinea,mensaje);	
}

void lexema()
{
	int i=0;
    char car=0;
	int acepto=0;
	int estado=0;
	int bandera=0;
	char msg[60];
	char aux[TAMSEC] = " "; // Array secundario para true false null
     
   	while((car=fgetc(archivo))!=EOF)
	{
        if(car=='\n')
	    {
		    //incrementar el numero de linea
		    numLinea++;
			fputs("\n",salida);
		    continue;
	    }
	    else if (car==' ')
	    {
            do
            {
                // se ocupa de cargar los espacios
				fputs(" ",salida);
                car=fgetc(archivo); 
            }while(car ==' ');
            car=ungetc(car,archivo);
	    }
        else if (car=='\t')
        { 
            //si es un tabulador que se guarde
            while(car=='\t')
            {
				fputs("\t",salida);
	        	car=fgetc(archivo);
            }
        }
    	else if (isdigit(car))
	    {
            //es un numero
            i=0;
            estado=0;
            acepto=0;
            id[i]=car;
			while(!acepto)
			{
				switch(estado)
				{
				    case 0: //una secuencia netamente de digitos, puede ocurrir . o e
						car=fgetc(archivo);
						if (isdigit(car))
						{
							id[++i]=car;
							estado=0;
						}
						else if(car=='.')
						{
							id[++i]=car;
							estado=1;
						}
						else if(tolower(car)=='e')
						{
							id[++i]=car;
							estado=3;
						}
						else
							estado=6;
						break;		
					case 1://un punto, debe seguir un digito 
						car=fgetc(archivo);						
						if (isdigit(car))
						{
							id[++i]=car;
							estado=2;
						}
						else{
							sprintf(msg,"No se esperaba '%c' despues del . ",car);
							fputs("Error lexico",salida);
							estado=-1;
						}
						break;
					case 2://la fraccion decimal, pueden seguir los digitos o e
						car=fgetc(archivo);
						if (isdigit(car))
						{
							id[++i]=car;
							estado=2;
						}
						else if(tolower(car)=='e')
						{
							id[++i]=car;
							estado=3;
						}
						else
							estado=6;
						break;
					case 3://una e, puede seguir +, - o una secuencia de digitos
						car=fgetc(archivo);
						if (car=='+' || car=='-')
						{
							id[++i]=car;
							estado=4;
						}
						else if(isdigit(car))
						{
							id[++i]=car;
							estado=5;
						}
						else
						{
							sprintf(msg,"Se esperaba algun signo o digitos despues del exponente");
							fputs("Error lexico",salida);
							estado=-1;
						}
						break;
					case 4://necesariamente debe venir por lo menos un digito
						car=fgetc(archivo);
						if (isdigit(car))
						{
							id[++i]=car;
							estado=5;
						}
						else
						{
							sprintf(msg,"No se esperaba '%c' despues del signo",car);
							fputs("Error lexico",salida);
							estado=-1;
						}
						break;
					case 5://una secuencia de digitos correspondiente al exponente
						car=fgetc(archivo);
						if (isdigit(car))
						{
							id[++i]=car;
							estado=5;
						}
						else
							estado=6;
						break;
					case 6://estado de aceptacion, devolver el caracter correspondiente a otro componente lexico
						if (car!=EOF)
							ungetc(car,archivo);
						else
                            car=0;
						id[++i]='\0';
						acepto=1;
                        t.compLexico=LITERAL_NUM;
						strcpy(t.lexema,id);
						fputs("LITERAL_NUMBER",salida);
						fputs(" ",salida);
						break;
					case -1:
						if (car==EOF){
                            error("No se esperaba el fin de archivo\n");
							fputs("Error lexico",salida);
						}
						else
                            error(msg);
                        acepto=1;
                    t.compLexico=VACIO;
                    while(car!='\n')
                        car=fgetc(archivo);
                    ungetc(car,archivo);
					break;
				}
			}
			break;
		}
        else if (car=='\"')
		{
            //un caracter o una cadena de caracteres
			i=0;
			id[i]=car;
			i++;
			do
			{
				car=fgetc(archivo);
				if (car=='\"')
				{
                    id[i]=car;
                    i++;
                    bandera=1;
                    break;
				}
                else if(car==EOF || car==',' || car=='\n' || car==':')
				{
                    sprintf(msg,"Se esperaba que finalice el literal cadena");
					error(msg);
					fputs("Error lexico",salida);
                    
                    while(car!='\n')
                        car=fgetc(archivo);

                    ungetc(car,archivo);
                    break;                       
				}
				else
				{
					id[i]=car;
					i++;
				}
			}while(isascii(car) || bandera==0);
			    id[i]='\0';
            strcpy(t.lexema,id);
			t.compLexico = LITERAL_CADENA;
			fputs("LITERAL_CADENA",salida);
			fputs(" ",salida);
			break;
		}
		else if (car==':')
		{
            //puede ser un :
            t.compLexico=DOS_PUNTOS;
            strcpy(t.lexema,":");
			fputs("DOS_PUNTOS",salida);
			fputs(" ",salida);
            break;
		}
		else if (car==',')
		{
			t.compLexico=COMA;
			strcpy(t.lexema,",");
			fputs("COMA",salida);
			fputs(" ",salida);
			break;
		}
		else if (car=='[')
		{
			t.compLexico=L_CORCHETE;
			strcpy(t.lexema,"[");
			fputs("L_CORCHETE",salida);
			fputs(" ",salida);
			break;
		}
		else if (car==']')
		{
			t.compLexico=R_CORCHETE;
			strcpy(t.lexema,"]");
			fputs("R_CORCHETE",salida);
			fputs(" ",salida);
			break;
		}
		else if (car=='{')
		{
			t.compLexico=L_LLAVE;
			strcpy(t.lexema,"{");
			fputs("L_LLAVE",salida);
			fputs(" ",salida);
			break;		
        }
        else if (car=='}')
		{
			t.compLexico=R_LLAVE;
			strcpy(t.lexema,"}");
			fputs("R_LLAVE",salida);
			fputs(" ",salida);			
			break;		
        }
		else if (car=='n' || car=='N')
        {
            ungetc(car,archivo);
            fgets(aux,5,archivo);//ver si es null
            if (strcmp(aux, "null")==0 || strcmp(aux, "NULL")==0)
            {
                t.compLexico = PR_NULL;
                strcpy(t.lexema,aux);
				fputs("PR_NULL",salida);
				fputs(" ",salida);
            }
            else
            {
                sprintf(msg,"%c no esperado",car);
			    error(msg);
				fputs("Error lexico",salida);

                while(car!='\n')
                    car=fgetc(archivo);

                t.compLexico = VACIO;
                ungetc(car,archivo);
            }
            break;
        }   
        else if (car=='f' || car=='F')
        {
            ungetc(car,archivo);
            fgets(aux,6,archivo);		//evalua si es null
            if (strcmp(aux, "false")==0 || strcmp(aux, "FALSE")==0)
            {
                t.compLexico = PR_FALSE;
                strcpy(t.lexema,aux);
				fputs("PR_FALSE",salida);
				fputs(" ",salida);
            }
            else{
                sprintf(msg,"%c no esperado",car);
			    error(msg);
				fputs("Error lexico",salida);

                while(car!='\n')
                    car=fgetc(archivo);    

                t.compLexico = VACIO;
                ungetc(car,archivo);
            }
            break;
        }   
        else if (car=='t' || car=='T')
        {
            ungetc(car,archivo);
            fgets(aux,5,archivo);		//evalua si es null
            if (strcmp(aux, "true")==0 || strcmp(aux, "TRUE")==0)
            {
                t.compLexico = PR_TRUE;
                strcpy(t.lexema,aux);
				fputs("PR_TRUE",salida);
				fputs(" ",salida);
            }
            else
            {
                sprintf(msg,"%c no esperado",car);
			    error(msg);
				fputs("Error lexico",salida);

                while(car!='\n')
                    car=fgetc(archivo);

                t.compLexico = VACIO;
                ungetc(car,archivo);
            }
            break;
        }
        else if (car!=EOF)
		{
			sprintf(msg,"%c no esperado",car);
			error(msg);
			fputs("Error lexico",salida);
            while(car!='\n')
                car=fgetc(archivo);
            ungetc(car,archivo);
		}
	}
	if (car==EOF)
	{
		t.compLexico=EOF;
		strcpy(t.lexema,"EOF");
		sprintf(t.lexema,"EOF");
		fputs("EOF",salida);
		fputs(" ",salida);
	}
}

int main(int argc,char* args[])
{
	//Tratamiento del archivo fuente
	if(argc > 1)
	{
		if (!(archivo=fopen(args[1],"rt")))
		{
			printf("Archivo no encontrado.\n");
			exit(1);
		}
		salida = fopen("output.txt","w");
		while (t.compLexico!=EOF)
		{
			lexema();
		}
		fclose(archivo);
		fclose(salida);
	}
	else
	{
		printf("Debe pasar como parametro el path al archivo fuente.\n");
		exit(1);
	}
	return 0;
}
