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

#define MAX_NOMBRE 1024        	/* tamaño máximo de nombre de fichero */
#define MAX_FICHEROS 128        /* número máximo de capítulos */
#define MAX_CONTENIDOS 4096*10 	/* tamaño máximo de un capítulo 40KB*/


typedef struct anal_format {
	char nombre[8];//nombre de formato, por ejemplo pepe
				   //Nuestro formato sera anal.ju (antonio alvato juan \
													//porque me sale de los huevos :)
													//61 6E 61 6C 2E 6A 75 asi sería en exadecimal.
	int bytes_sector; //en nuestro caso 512.
	int reserved_sectors;	//sectores reservados, en nuestro caso tendremos 2
								//1 para la estructura y otro
								//para la tabla de fats.
	/*int32_t   sectors_per_fat;
    int8_t    fat_amount;                 //file_allocation_tables_amount										  //2
	uint8_t dir_root_dir;//direccion del directorio raíz
	uint16_t max_root_entries;//en nuestro caso 244 porque sí
	uint16_t total_sectors;//sectores totals en nuestro caso 2880
	*/ 
};//12 bytes



typedef struct file_inf{
	char nombre[10];//nombre del archivo.
	char format[3];//formato del archivo.
	int id;
	uint32_t size;//tamaño del archivo
	uint8_t atributos;
	/*
      Attribute Bits:
      0:    read only
      1:    hidden      -   Shouldn't show in Dir listing
      2:    system      -   Belongs to system, shouldn't be moved
      3:    volume id   -   Filename is volume label
      4:    directory   -   Is a Subdirectory
      5:    archive     -   Has been changed since last backup, ignore
      6-7:  unused, should be 0
    */
};


typedef struct structura_mis_datos
{
	char *fichero_imagen;  				/* fichero formato analju */
	int numero_ficheros;     				/* Para contar el numero de ficheros. */
	struct file_inf *finfo;		/* Informacion de archivos */
	struct anal_format *af;
	int mi_offset;//donde inicia los datos.
	int max_tam;
	
	
	struct timespec st_atim;  				
    struct timespec st_mtim; 
    struct timespec st_ctim;  
    uid_t     st_uid;        				
    gid_t     st_gid;  
    
};



/***********************************
 * Busca un fichero path en mi estructura de datos
 * */
int buscar_fichero(const char *path, struct structura_mis_datos *mis_datos);

/***********************************
 * error en parametros de entrada
 * */
void error_parametros();

/***********************************
 * lee fichero con los capítulos y rellena estructura mis_datos
 * */
void leer_fichero(struct structura_mis_datos *mis_datos);
/***********************************
 * crear fichero con los capítulos y rellena estructura mis_datos
 * */
int crear_f(const char *nombre, struct structura_mis_datos *mis_datos);


#endif
