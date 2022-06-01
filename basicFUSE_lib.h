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

#ifndef __BASIC_FUSE__
#define __BASIC_FUSE__

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

/***********************************
 * macros y estructura
 * */


#define MAX_CONTENIDOS 4096*10 	/* tamaño máximo de un capítulo 40KB*/
#define MAX_DIRECTERORIOS 4
#define MAX_FICHEROS 4
#define MAX_CONTENIDO 256
#define MAX_DIRECCIONES 4
struct estructura_mis_datos{// /
	char *lista_directorios[MAX_DIRECCIONES];//  /hola 
	char *lista_ficheros[MAX_FICHEROS];
	char *contenido_archivo[MAX_CONTENIDO];//punteros
	//char fd[];
	int numero_ficheros;     				/* número de capitulos encontrados */
	int numero_directorios;
	int indice_contenido_archivo;
	struct estructura_mis_datos *directorios[MAX_FICHEROS];
	struct timespec st_atim;  				/* fechas del fichero */
    struct timespec st_mtim; 
    struct timespec st_ctim;  
    uid_t     st_uid;        				/* El usuario y grupo */
    gid_t     st_gid;  
};


/***********************************
 * Busca un fichero path en mi estructura de datos
 * */
 
void anyadir_directorio(const char * nombre_directorio, struct estructura_mis_datos *mis_datos);

void anyadir_archivo(const char * path, struct estructura_mis_datos *mis_datos);

int es_archivo(const char * path, struct estructura_mis_datos *mis_datos);
int es_directorio(const char * path, struct estructura_mis_datos *mis_datos);
//int buscar_archivo(const char * path, struct estructura_mis_datos *mis_datos);
int buscar_directorio(const char * path, struct estructura_mis_datos *mis_datos);

void escribir(const char * path,const char *datos ,struct estructura_mis_datos *mis_datos);
 
int buscar_fichero(const char *path, struct estructura_mis_datos *mis_datos);

/***********************************
 * error en parametros de entrada
 * */
void error_parametros();

/***********************************
 * lee fichero con los capítulos y rellena estructura mis_datos
 * */
//void leer_fichero(struct estructura_mis_datos *mis_datos);
void lineas();
int crear_f(const char *nombre, struct estructura_mis_datos *mis_datos);
#endif
