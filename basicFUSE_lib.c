/*
 * FUSE: Filesystem in Userspace
 * DSO 2014
 * Ejemplo para montar un libro de poesía como sistema de ficheros
 * Cada capítulo del libro será un fichero diferente
 * 
 * Compilarlo con make
 *  
 * uso:  basicFUSE [opciones FUSE] fichero_inicial punto_de_montaje
 * 
 *  ./basicFUSE proverbiosycatares.txt punto_montaje
 * 
*/

#include "basicFUSE_lib.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

void lineas(){
	printf("---------------------------------------\n\n");
	
}
/***********************************
 * Busca un fichero path en mi estructura de datos
 * */
int buscar_fichero(const char *path, struct estructura_mis_datos *mis_datos)
{
	int i;
	
	for(i=0; i< mis_datos->numero_ficheros;i++)
	{
		if(strcmp(mis_datos->lista_ficheros[i],path+1)==0)
		{
			return i;
		}
	}
	return -1;
}



/***********************************
 * */
void error_parametros()
{
    fprintf(stderr, "uso:  basicFUSE [opciones FUSE] fichero_inicial punto_de_montaje\n");
    exit(-1);
}
/*
void leer_fichero(struct estructura_mis_datos *mis_datos)
{
	FILE *f;
	int i=0;
	char nombre[MAX_CONTENIDO];
	char contenido[MAX_CONTENIDOS];
	struct stat fileStat;
	
	f=fopen(mis_datos->fichero_inicial,"rc");
	if (f==NULL) error_parametros();
	
	nombre[0]='X';
	
	while (!feof(f))
	{
		if(fscanf(f," X%[LXVI]s ",nombre+1))
		{
			if(strcmp(nombre,"XX")==0) 
				mis_datos->nombre_ficheros[i]=strdup("largo");
			else
				mis_datos->nombre_ficheros[i]=strdup(nombre);
			fscanf(f," %[^X]s ",contenido);
			mis_datos->contenido_ficheros[i]=strdup(contenido);
			i++;
		}
	}
	
	mis_datos->numero_ficheros=i;
	fstat(fileno(f), &fileStat);
	mis_datos->st_uid= fileStat.st_uid;
	mis_datos->st_gid= fileStat.st_gid;
	mis_datos->st_atime = fileStat.st_atime;
	mis_datos->st_ctime = fileStat.st_ctime;
	mis_datos->st_mtime = fileStat.st_mtime;
		      
	fclose(f);
}
* */
/*
int crear_f(const char *nombre, struct structura_mis_datos *mis_datos){
	struct stat fileStat;
	FILE *f = fopen(nombre, "w+");
	//if(strlen(nombre)>MAX_NOMBRE) return -EACCES;

	strcpy(mis_datos->nombre_ficheros[mis_datos->numero_ficheros+1], nombre);
	mis_datos->numero_ficheros++;
	
	fstat(fileno(f), &fileStat);
	mis_datos->st_uid= fileStat.st_uid;
	mis_datos->st_gid= fileStat.st_gid;
	mis_datos->st_atime = fileStat.st_atime;
	mis_datos->st_ctime = fileStat.st_ctime;
	mis_datos->st_mtime = fileStat.st_mtime;
	
	fclose(f);
	return 0;
}
*/

void anyadir_directorio(const char * nombre_directorio, struct estructura_mis_datos *mis_datos){
	//mis_datos->directorio[];
	mis_datos->numero_directorios++;
	strcpy(mis_datos->lista_directorios[mis_datos->numero_directorios],nombre_directorio);
}

void anyadir_archivo(const char * nombre_fichero, struct estructura_mis_datos *mis_datos){
	mis_datos->numero_ficheros++;
	strcpy(mis_datos->lista_ficheros[mis_datos->numero_ficheros],nombre_fichero);
	mis_datos->indice_contenido_archivo++;
	strcpy(mis_datos->contenido_archivo[mis_datos->numero_ficheros], "" );
}

int es_archivo(const char * path, struct estructura_mis_datos *mis_datos){
	path++;
	for(int i = 0; i<= mis_datos->numero_ficheros;i++){
		if(strcmp( path, mis_datos->lista_ficheros[ i ] ) == 0 ) return 1;
	}
	return 0;
}
int es_directorio(const char * path, struct estructura_mis_datos *mis_datos){
	path++;
	for(int i = 0; i<= mis_datos->numero_directorios;i++){
		if(strcmp( path, mis_datos->lista_directorios[ i ] ) == 0 ) return 1;
	}
	return 0;
}

int buscar_directorio(const char * path, struct estructura_mis_datos *mis_datos){
	int i;
	
	for(i=0; i< mis_datos->numero_directorios;i++)
	{
		if(strcmp(mis_datos->lista_directorios[i],path+1)==0)
		{
			return i;
		}
	}
	return -1;
}

void escribir(const char * path,const char *datos ,  struct estructura_mis_datos *mis_datos){
	int i = buscar_fichero(path,mis_datos);
	if(i == -1)return;
	
	strcpy( mis_datos->contenido_archivo[ i ], datos );
}
 





