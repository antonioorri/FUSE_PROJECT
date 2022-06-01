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

#include "proyecto_lib.h"

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

static const char* cuerdas_path = "/cuerda";
static const char* viento_path = "/viento";

/***********************************
 * */
static int mi_getattr(const char *path, struct stat *stbuf)
{
	/* completar */
	struct structura_mis_datos *mis_datos= (struct structura_mis_datos *) fuse_get_context()->private_data;

	int i;

	int res = 0;

	memset(stbuf, 0, sizeof(struct stat));
	if (strcmp(path, "/") == 0) {
		stbuf->st_mode = S_IFDIR | 0777;
		stbuf->st_nlink = 2;
		stbuf->st_uid = mis_datos->st_uid;
		stbuf->st_gid = mis_datos->st_gid;

		stbuf->st_atime = mis_datos->st_atime; //mod time
		stbuf->st_mtime = mis_datos->st_mtime;
		stbuf->st_ctime = mis_datos->st_ctime;
		stbuf->st_size = 4096; //tamaño (4096) pret 1024
		stbuf->st_blocks = 8; //tamaño dividido entre 512 //8 anterior 2
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

	} else {
		res = -ENOENT;
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
            if (filler(buf, mis_datos->nombre_ficheros[i], NULL, 0) != 0) return -ENOMEM;
        }

      }else
        return -ENOMEM;

	return 0;

}

/***********************************
 * */
static int mi_open(const char *path, struct fuse_file_info *fi)
{
	 int i;

    struct structura_mis_datos* mis_datos = (struct structura_mis_datos*)fuse_get_context()->private_data; //usarla para acceder a la estructura


    if ((i=buscar_fichero(path , mis_datos)) >= 0) //=
    {
        if ((fi->flags & 3) != O_RDONLY) return -EACCES;
        fi->fh=i;
        return 0;
    }else
        return -ENOENT;

}


/***********************************
 * */
static int mi_read(const char *path, char *buf, size_t size, off_t offset,
		      struct fuse_file_info *fi)
{
 size_t len;
    int i = fi->fh;
    struct structura_mis_datos* mis_datos = (struct structura_mis_datos*)fuse_get_context()->private_data; //usarla para acceder a la estructura

    len = strlen(mis_datos->contenido_ficheros[i]);

    if (offset < len) {
        if (offset + size > len) size = len - offset;
        memcpy(buf, mis_datos->contenido_ficheros[i] + offset, size);
    } else
        size = 0;

    return size;
}


/***********************************
 * operaciones FUSE
 * */
static struct fuse_operations basic_oper = {
	.getattr	= mi_getattr,
	.readdir	= mi_readdir,
	.open		= mi_open,
	.read		= mi_read,
};


/***********************************
 * */
int main(int argc, char *argv[])
{
	struct structura_mis_datos *mis_datos;

	mis_datos=malloc(sizeof(struct structura_mis_datos));

	// análisis parámetros de entrada
	if ((argc < 3) || (argv[argc-2][0] == '-') || (argv[argc-1][0] == '-')) error_parametros();

	mis_datos->fichero_inicial = strdup(argv[argc-2]); // fichero donde están los capítulos
    argv[argc-2] = argv[argc-1];
    argv[argc-1] = NULL;
    argc--;

    leer_fichero(mis_datos);

/*    int i;
    for(i=0; i<mis_datos->numero_ficheros; i++)
    {
		printf("----->  %s\n", mis_datos->nombre_ficheros[i]);
		printf("%s",mis_datos->contenido_ficheros[i]);
	}
	exit(0);
*/

	return fuse_main(argc, argv, &basic_oper, mis_datos);
}
