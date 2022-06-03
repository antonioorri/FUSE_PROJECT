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
/* Version 0.1 ❤️😒😊😭😩😍😔😁💕💕💕💕😊😊😊😊
*/
#define FUSE_USE_VERSION 26

#include "basicFUSE_lib.h"

#include <stdlib.h>
#include <fuse.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>


/*
 *  Para usar los datos pasados a FUSE usar en las funciones:
 *
	struct structura_mis_datos *mis_datos= (struct structura_mis_datos *) fuse_get_context()->private_data;
 *
 * */


/***********************************
 * */
 
 
 
 
static void *mi_init(struct fuse_conn_info *conn){
	struct structura_mis_datos *mis_datos= (struct structura_mis_datos *) fuse_get_context()->private_data;
	mis_datos->af = (struct struct anal_format*)malloc(sizeof(struct anal_format));
	
	
	//leemos el formato de nuestro file system. que se encuentra en el primer sector.
	pread(mis_datos->fh,(void*) mis_datos->af,sizeof(struct anal_format),0);
	
	mis_datos->finfo = (struct struct file_inf*)malloc(sizeof(struct file_inf));
	//leemos la estructura que nos da información de los archivos. En nuestro caso se encuentra en el sector 1.(reservado)
	pread(mis_datos->fh,(void*) mis_datos->file_inf,sizeof(struct file_inf),mis_dato->af->bytes_sector);
	
	mis_datos->mi_offset = (mis_datos->af->bytes_sector * mis_datos->af->reserved_sectors);// aquí empezaremos a escribir.
	
	mis_datos->st_uid = getuid(); // The owner of the file/directory is the user who mounted the filesystem
    mis_datos->st_gid = getgid(); // The group of the file/directory is the same as the group of the user who mounted the filesystem
    mis_datos->st_atime = time( NULL ); // The last "a"ccess of the file/directory is right now
    mis_datos->st_mtime = time( NULL ); // The last "m"odification of the file/directory is right now
    mis_datos->st_ctime = time( NULL ); // The last "m"odification of the file/directory is right now
    
    return mis_datos;
} 
 

static int mi_getattr(const char *path, struct stat *stbuf)
{
	/* completar */
	struct structura_mis_datos *mis_datos= (struct structura_mis_datos *) fuse_get_context()->private_data;

	int i;

	int res = 0;

	memset(stbuf, 0, sizeof(struct stat));
	
	if (strcmp(path, "/") == 0) {
		stbuf->st_mode = S_IFDIR | 0755;
		stbuf->st_nlink = 2;
		stbuf-> st_size = 512;
		stbuf->st_blocks = 8;
		
		
		stbuf->st_uid = mis_datos->st_uid;
		stbuf->st_gid = mis_datos->st_gid;
		stbuf->st_atime = mis_datos->st_atime; //mod time
		stbuf->st_mtime = mis_datos->st_mtime;
		stbuf->st_ctime = mis_datos->st_ctime;
		 //tamaño dividido entre 512 //8 anterior 2
		//aqui se controlan los directorios nuevos

	} else {
		stbuf->st_mode = S_IFREG | 0777;
		stbuf->st_nlink = 1;

		stbuf->st_uid = mis_datos->st_uid;
		stbuf->st_gid = mis_datos->st_gid;

		stbuf->st_atime = mis_datos->st_atime;
		stbuf->st_mtime = mis_datos->st_mtime;
		stbuf->st_ctime = mis_datos->st_ctime;

		stbuf->st_size = 512;
		stbuf->st_blocks = stbuf->st_size/512 + (stbuf->st_size%512)? 1 : 0;

	} 

	return res;
}

/*********************************** DONE
 * */

static int mi_readdir(const char *path, void *buf, fuse_fill_dir_t filler,
			 off_t offset, struct fuse_file_info *fi)
{
struct structura_mis_datos *mis_datos= (struct structura_mis_datos *) fuse_get_context()->private_data;

	/* completar */
	int i;

	(void) offset;
	(void) fi;

  if (strcmp(path, "/") == 0) {
        if (filler(buf, ".", NULL, 0) != 0) return -ENOMEM;
        if (filler(buf, "..", NULL, 0) != 0) return -ENOMEM;
		
        for (i = 0; i < mis_datos->numero_ficheros; i++) {
            if (filler(buf, mis_datos->file_inf[i+2]->nombre, NULL, 0) != 0) return -ENOMEM;
        }

      }else
        return -ENOMEM;

	return 0;

}

static int mi_mknod(const char *path,mode_t mode, dev_t rdev){
	
	struct structura_mis_datos *mis_datos= (struct structura_mis_datos *) fuse_get_context()->private_data;
	mis_datos->finfo = (struct struct file_inf*)malloc(sizeof(struct file_inf));
	path++;//para quitar el /
	
	mis_datos->numero_ficheros++;
	mis_datos->mi_offset++;
	memcpy(finfo->nombre,path,strlen(path);
	finfo->id=numero_ficheros;
	
}


/***********************************
 * operaciones FUSE
 * */
static struct fuse_operations basic_oper = {
	.init		= mi_init,
	.getattr	= mi_getattr,
	.readdir	= mi_readdir,
	//.open		= mi_open,
	//.read		= mi_read,
	//.mknod		= mi_mknod,
};


/***********************************
 * */
int main(int argc, char *argv[])
{
	struct structura_mis_datos *mis_datos;

	mis_datos=malloc(sizeof(struct structura_mis_datos));

	// análisis parámetros de entrada
	if ((argc < 3) || (argv[argc-2][0] == '-') || (argv[argc-1][0] == '-')) error_parametros();
    mis_datos -> fichero_imagen = strup(argv[argc-2]);
    mis_datos->fh=open(mis_datos->fichero_imagen, O_RDONLY);
	if(mis_datos->fh<0){
		perror("ERROR al abrir el acrchivo");
		exit(-1);
	}
	argv[argc-2] = argv[argc-1];
    argv[argc-1] = NULL;
    argc--;
	return fuse_main(argc, argv, &basic_oper, mis_datos);
}
