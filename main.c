#include <stdio.h>//atoi
#include <stdlib.h>//directorios
#include <dirent.h>//crear directorios
#include <sys/stat.h>//comparacion de cadenas
#include <string.h>//time
#include <time.h>
#include <stdbool.h>
#include <ctype.h>
#include <sys/stat.h>


//******************************** Estructuras del Archivo
//+++++++++++++ struct del Disco *********
struct EstDisk{ //struct del disco
    char nombre[100];
    int tamano;
    char particiones[9];
    char pnt[9]; //direccion del mbr -- punteros
};

struct Partition{
    char Part_Status[4];//indica si esta activa  o no
    char Part_Type[4];//indica tipo de particion Primario o Extendida p o E
    char Part_Fit[4];//Tipo de ajuste Valores BF (Best) FF(First WF(Worst)
    int Part_Start;//Indica en que Byte Inicia el Disco
    int Part_Size;//Contiene el Tamano total de la particion
    char Part_Name[16];//Nombre de la particion
    int Uso;//si ya fue asignada esta particion a un espacio de disco
};

struct MasterBoot{//Mbr
    int Mbr_Tamano;//tamano total del disco en bytes
    //struct tm time;//time //probar sin puntero despues con disco
    char Mbr_Fecha_Creacion[16];// fecha y hora de la creacion del disco
    int Mbr_Disk_Signature;//numero random de forma unica a cada disco
    struct Partition Mbr_Partition_1;//estructura del disco 1
    struct Partition Mbr_Partition_2;//Estructura del disco 2
    struct Partition Mbr_Partition_3;//Estructura del disco 3
    struct Partition Mbr_Partition_4;//estructura del disco 4
};

struct ExtendedBoot{//Ebr
    char Part_Status[4];//indica si esta activa  o no
    char Part_Fit[4];//Tipo de ajuste Valores BF (Best) FF(First WF(Worst)
    int Part_Start;//Indica en que Byte Inicia el Disco
    int Part_Size;//Contiene el Tamano total de la particion
    int Part_Next;//Byte en el que indica el proximo EBR -1 si no hay siguiente
    int Part_Name[16];//Nombre de la particion+ 4 por .disk
};

struct EstMbr{
    int Mbr_Tamano;
    //time tiempo;
    int Mbr_Disk_Assignature;//random


    char mbr_byte_particion[10];//Contiene el byte donde inicia la partición.
    char mbr_byte_particion_desactivada[10];//Contiene el byte donde inicia la partición.
    char mbr_tipo_sistema_archivos[7];//Contiene el tipo de sistema de archivos.
    char mbr_tipo_particion[12];//Contiene el tipo de partición.
    char mbr_particion_activa[4];//Indica si la partición es la activa, está será la por defecto que iniciará para realizar operaciones sobre el sistema de archivos.
    char mbr_size_bytes[10];//Contiene el tamaño en bytes del sistema de archivos.
};

struct LtsMountDisk{
    int Numero;// numero que lo identifica
    int Letra;//letra que identifica
    //char Id[16];//vd + letra + numero
    char Disco[16];//nombre del disco
    char Particion[16];// Nombre de la particion
    char Directorio[150];
    struct LtsMountDisk *Abajo;
    struct LtsMountDisk *Siguiente;
    //struct LtsMountDisk *Arriba;
};

struct LtsMountDisk *primero, *ultimo, *arriba, *abajo;

//******************************** Var. Global
//guarda la ruta de los discos duros del sistema
char Path[250]="";
char Dir[250];//direccion del disco actual

char sistema_reporte[17]="reporte/";
char directorio_reporte[640];
char buffer[1];//bit utilizado para guardar 0's
int ifor=0;//contador de fors
DIR *d;//variable directorio global
struct dirent *di;//para listar discos
char fecha[16];//fecha actual
int login=0;//FAT login (0= no ha iniciado sesion, 1= inicio sesion)

void SetDisk(char nombre [100]){
    sprintf(Dir, "%s%s", Path, nombre);
}
//crea direcctorios
void Directorio(){
    char direc [200];
    strcpy(direc, "mkdir -p ");
    strcat(direc, Path);
    printf("Directorios:: %s \n:", direc);
    system(direc);

}
//crear disco

char LetraMount(int pos){

    char abc[30] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
    int i;
    for(i = 0; i < pos; i++){
        if(i == pos){
            return abc[i];
        }
    }
}

char LetraMountSig(char lets[4]){
    char abc[30];
    strcpy(abc,"ABCDEFGHIJKLMNOPQRSTUVWXYZ");
    char res[4];
    int i;
    for(i = 0; i < 30; i++){
        strcpy(res, "");
        strcat(res, abc[i]);
        if(strcmp(res, lets) ==0){
            strcpy(res, "");
            strcat(res, abc[i+1]);
            return res;
        }
    }
}

/*

        struct LtsMountDisk *aux;
        aux = primero;
        while(aux != NULL){
            id = LetraMountSig(aux->Letra);
            if(strcmp(aux->Disco, nomb) == 0){
                struct LtsMountDisk *aux2;
                aux2 = arriba;
                num = true;
                strcpy(sil, "");
                strcat(sil, aux->Letra);
                while(aux2 != NULL){
                    if(strcmp(aux2->Particion, Name) == 0){
                        letr = true;
                        nletra = numero;
                    }
                    numero++;
                    aux = aux->Abajo;
                }
            }
            aux = aux->Siguiente;
        }

        if(num == true && letr == true){
            printf("Error - La Particion ya esta Montada\n");
            return;
        }else if(num == true){
            strcpy(NMount->Letra, sil);
            NMount->Numero = numero;
            strcpy(NMount->Particion, Name);
            strcpy(NMount->Disco, nomb);
            ultimo->Siguiente = NMount;
        }else if(num == false){
            strcpy(NMount->Letra, id);
            NMount->Numero = 1;
            strcpy(NMount->Particion, Name);
            strcpy(NMount->Disco, nomb);
            abajo->Abajo = NMount;
        }
*/

void MountDisk(char direc[150], char Name[16]){
    //Comprueba si existe el Disco, si Existe devuelve su inf mbr
    FILE *f = fopen (direc, "rb+");
    struct EstDisk tdisco;
    struct MasterBoot mbr;
    if (f) {
        fread(&tdisco,sizeof(tdisco),1,f);
        fread(&mbr, sizeof(mbr), 1,f);
        fclose(f);
    } else {
        printf("Error - El Disco No existe Colocque Uno existente\n");
        return;
    }

    char nomb_disk[16];//nombre de la particion
    bool Mnumero = false;
    bool Mletra = false;

    int Mlts;
    int Mnum;
    int conL;
    int conN;

    //lee la cadena y concatena el nobre del disco ej:: DISK.dsk
    strcpy(nomb_disk, "");
    int i;
    for(i = 0; i < strlen(direc); i++ ){
        if(direc[i] == '/'){
            strcat(nomb_disk, "");
        }else{
            strcat(nomb_disk, direc[i]);
        }
    }
    //Nuevo nodo struct
    struct LtsMountDisk*NMount;
    NMount = (struct LtsMountDisk *)malloc(sizeof(struct LtsMountDisk));

    if(NMount == NULL){
        printf("NO hay Memoria suiciente para el Mount\n");
    }

    if(primero == NULL){// si es null
        NMount->Letra = 65;
        NMount->Numero = 1;
        strcpy(NMount->Particion, Name);
        strcpy(NMount->Disco, nomb_disk);
        strcpy(NMount->Directorio, direc);
        primero = NMount;
        //NMount->Arriba = NMount;
        //NMount->Abajo = NMount;
        //arriba = NMount;
        ultimo = NMount;
        //abajo = NMount;
    }else{
        strcpy(NMount->Particion, Name);
        strcpy(NMount->Disco, nomb_disk);
        strcpy(NMount->Directorio, direc);

        struct LtsMountDisk *aux;
        struct LtsMountDisk *aux2;
        aux = primero;
        while(aux != NULL){
            aux2 = aux;
            if(strcmp(aux2->Disco, nomb_disk) == 0){//si entra es porque si existe el disco montado
                Mletra = true;
                Mlts = aux->Letra;
                while (aux2 != NULL) {
                    if(strcmp(aux2->Particion, Name) == 0){
                        Mnumero = true;
                        Mnum = aux2->Numero;
                    }
                    aux2 = aux2->Abajo;
                }
                //verifico que no exista de la misma particion
                //verifica que no tengan ningun mismo numero;
                if(Mnumero == true){
                    printf("Error - La particion que desea Montar ya existe.\n");
                }else{
                    NMount->Letra = Mlts;
                    aux2 = aux;
                    conN = 1;
                    struct LtsMountDisk *aux3;//para toma el ultimo de la fila
                    while (aux2 != NULL) {
                        if(conN == aux2->Numero){
                            conN++;
                            aux2 = aux;
                        }else{
                            aux3 = aux2;
                            aux2 = aux2->Abajo;
                        }
                    }
                    NMount->Numero = conN;
                    aux3->Abajo = NMount;
                    //NMount->Abajo = NULL;
                }
                break;//salgo del ciclo
            }
            aux = aux->Siguiente;
        }
        conL = 65;
        if(Mletra == true && Mnumero == true){
            printf("Error - La particion ya fuen Montada\n");
        }else if(Mletra == false && Mnumero == false){
            aux = primero;
            while(aux != NULL){
                if(conL == aux->Letra){
                    conL++;
                    aux = primero;
                }else{
                    aux2 = aux;
                    aux = aux->Siguiente;
                }
            }
            NMount->Letra = conL;
            NMount->Numero = 1;

            ultimo->Siguiente = NMount;
            NMount->Siguiente = ultimo;
            ultimo = NMount;
        }
    }
}

void ViewMount(){
    struct LtsMountDisk *aux;
    aux = primero;
    while(aux != NULL){

            struct LtsMountDisk *aux2;
            aux2 = arriba;
            while(aux2 != NULL){

                aux = aux->Abajo;
            }
        aux = aux->Siguiente;
    }
}

void NewDisk(char name[100], int tam, int unidad, char direc[150]){//crea el disco //crear_disco
    struct EstDisk Ndisck;
    struct MasterBoot Mbr;
    struct Partition part = {};
    part.Uso = 11;// no se asignado el disco en el mbr

    printf("\n");
    printf("************   *********\n");
    printf("Creando Disco...\n");
    printf("************   *********\n");
    //strcpy(Ndisck.nombre, name);
    sprintf(Path, "%s", direc);// cambiamos el lugar donde se almacena
    Directorio();
    sprintf(Ndisck.nombre, "%s", name);

    //libreria sprintf concatena y asigna en forma de arreglo
    sprintf(Ndisck.particiones, "%d", 0);
    sprintf(Ndisck.pnt, "%d", (int)sizeof(Ndisck));
    printf("Creando disco...\n");
    //creando el disco
    SetDisk(Ndisck.nombre);
    printf("%s\n", Dir);
    FILE *f = fopen (Dir, "w+b");

    if(unidad == 1){// 0 == Mk 1==kb
        for(ifor=0;ifor < tam;ifor++)
            fwrite (buffer, sizeof(buffer), 1024, f);
        //guardando informacion del disco
        tam = tam*1024;
    }else if(unidad == 0){
        for(ifor=0;ifor < tam*1024;ifor++)
            fwrite (buffer, sizeof(buffer), 1024, f);
        //guardando informacion del disco
        tam = tam*1024*1024;
    }else{
        for(ifor=0;ifor < tam*1024;ifor++)
            fwrite (buffer, sizeof(buffer), 1024, f);
        //guardando informacion del disco
        tam = tam*1024*1024;
    }
    Ndisck.tamano = tam;

    rewind(f);
    fwrite(&Ndisck,sizeof(Ndisck),1,f);

    //mbr del disco
    //random
    int rdn = rand();
    timer_t now;
    struct tm *ts;
    char buff[16];
    now = time(0);
    ts = localtime(&now);
    strftime(buff, sizeof(buff), "%d/%m/%y %H:%M", ts);
    Mbr.Mbr_Tamano = tam;
    sprintf(Mbr.Mbr_Fecha_Creacion, "%s", buff);
    Mbr.Mbr_Disk_Signature = rdn;
    Mbr.Mbr_Partition_1 = part;
    Mbr.Mbr_Partition_2 = part;
    Mbr.Mbr_Partition_3 = part;
    Mbr.Mbr_Partition_4 = part;

    fwrite(&Mbr,sizeof(Mbr),1,f);


    fclose(f);
    printf("\n=====================================================\n");
    printf("Se creo:\n");
    printf("Disco: %s\n", Ndisck.nombre);
    printf("Tamano: %d Bytes\n", Mbr.Mbr_Tamano);
    printf("Identificador: %d\n", Mbr.Mbr_Disk_Signature);
    printf("Fecha y Hora: %s\n", Mbr.Mbr_Fecha_Creacion);
    printf("=====================================================\n");
    printf("Disco %s creado exitosamente\n\n", Ndisck.nombre);

}
//remover disco
void RmDisk(char direc[100]){
    if(remove(direc)==0) // Eliminamos el archivo
        printf("El Disco fue Eliminado Correctamente\n");
    else
        printf("No se pudo eliminar el Disco Verifique su Inf.\n");
}
//crea particiones
void FDisk(int Size, char Direc[100], char Name[16], int Unit, int Type, int Fit){


    //fdisk -Size::300 -path::"/home/gremory/Escritorio/casas/Disco1.dsk" -name::"Particion1"
    //fdisk -Size::1024 -path::"/home/gremory/Escritorio/casas/Disco1.dsk" -name::"Particion2"
    //fdisk -Size::1024 -path::"/home/gremory/Escritorio/casas/Disco1.dsk" -name::"Particion3"
    //fdisk -Size::1024 -path::"/home/gremory/Escritorio/casas/Disco1.dsk" -name::"Particion4"
    //fdisk -Size::300 -path::"/home/gremory/Escritorio/casas/Disco1.dsk" -name::"Particion01"

    struct EstDisk EDisk;
    struct MasterBoot Mbr;
    struct Partition Part;

    //creamos archivo
    FILE *f = fopen (Direc, "rb+");//Buscamo el Archivo y lo abrimos
    fseek(f,Direc,SEEK_SET);//tomamo los punteros del datos del archivo
    //tomamos la informacion de la memorio
    fread(&EDisk,sizeof(EDisk),1,f);
    fread(&Mbr, sizeof(Mbr), 1,f);

    int part1_ini;
    int part2_ini;
    int part3_ini;
    int part4_ini;

    int part1_size;
    int part2_size;
    int part3_size;
    int part4_size;

    int part1_fina;
    int part2_fina;
    int part3_fina;
    int part4_fina;

    int part1_libr;
    int part2_libr;
    int part3_libr;
    int part4_libr;

    int ndsk1_size;
    int ndsk2_size;
    int ndsk3_size;
    int ndsk4_size;

    int mbr_size = 0;
    int ndisk_size = 0;

    //asigna el tamno del disco con la que se asigna
    if(Unit == 0){
        Size = Size*1024*1024;//tamano convertido en bytes
    }else if(Unit ==1){
        Size = Size*1024;//tamano convertido en bytes
    }

    //tipo de particion de la que se crea
    char tipo[4];
    if(Type == 0){
        strcpy(tipo, "P");
    }else if(Type == 1){
        strcpy(tipo, "E");
    }else if(Type == 2){
        strcpy(tipo, "L");
        printf("No se puede Crear Particion Logica Como Principal");
    }

    //tipo de colocacion
    char fitT[4];
    if(Fit == 0){
        strcpy(fitT, "BF");
    }else if(Fit == 1){
        strcpy(fitT, "FF");
    }else if(Fit == 2){
        strcpy(fitT, "WF");
    }

    mbr_size = sizeof(Mbr) + sizeof(EDisk);//tamano de la estructura del mbr y del disco
    ndisk_size = Mbr.Mbr_Tamano - mbr_size;//tamano del disco, restamos la informacion del disco; total del disco disponible

    //asignamos los valors para el struct del disco que deseamos
    sprintf(Part.Part_Status, "%s", "S");//activa si o no
    sprintf(Part.Part_Type, "%s", tipo);
    sprintf(Part.Part_Fit, "%s", fitT);
    //
    Part.Part_Size = Size;
    sprintf(Part.Part_Name, "%s", Name);
    Part.Uso = 22;


    //si ya existe una particion extendida, retorna y no crea
    if((strcmp(Mbr.Mbr_Partition_1.Part_Type, "E") == 0 || strcmp(Mbr.Mbr_Partition_2.Part_Type, "E") == 0 || strcmp(Mbr.Mbr_Partition_3.Part_Type, "E") == 0 || strcmp(Mbr.Mbr_Partition_4.Part_Type, "E") == 0) && strcmp(tipo, "E") == 0){
        printf("Error - No puede Crear mas de 1 Particion Extendida\n");
        printf("Puede Crear 4 Particiones Principales o\n");
        printf("1 Particion Extendida y 3 Particiones Principales\n");
        fclose(f);
        return;
    }

    if((strcmp(Mbr.Mbr_Partition_1.Part_Name, Name) ==0 || strcmp(Mbr.Mbr_Partition_2.Part_Name, Name) == 0 || strcmp(Mbr.Mbr_Partition_3.Part_Name, Name) == 0 || strcmp(Mbr.Mbr_Partition_4.Part_Name, Name) == 0)){
        printf("Error - No se Pudo Crear Particion, Nombre ya exite en otra particion\n");
        printf("Verifique sus Datos\n");
    }

    //tomo los tamano de disco
    part1_size = Mbr.Mbr_Partition_1.Part_Size;
    part2_size = Mbr.Mbr_Partition_2.Part_Size;
    part3_size = Mbr.Mbr_Partition_3.Part_Size;
    part4_size = Mbr.Mbr_Partition_4.Part_Size;

    //tomo donde inicia las particiones
    part1_ini = Mbr.Mbr_Partition_1.Part_Start;
    part2_ini = Mbr.Mbr_Partition_2.Part_Start;
    part3_ini = Mbr.Mbr_Partition_3.Part_Start;
    part4_ini = Mbr.Mbr_Partition_4.Part_Start;

    //donde finaliza la particion de disco
    part1_fina = part1_ini + part1_size;
    part2_fina = part2_ini + part2_size;
    part3_fina = part3_ini + part3_size;
    part4_fina = part4_ini + part4_size;

    ndsk1_size = ndisk_size - part1_size;
    ndsk2_size = ndisk_size - part1_size - part2_size;
    ndsk3_size = ndisk_size - part1_size - part2_size - part3_size;


    //temporisador de 4 milisengundos para poder capturar el espacion de la particion
    printf("T ::%d\n", ndisk_size);
    printf("T ::%d\n", ndsk1_size);
    printf("T ::%d\n", ndsk2_size);
    printf("T ::%d\n", ndsk3_size);

    if(Mbr.Mbr_Partition_1.Part_Size == 0 && Mbr.Mbr_Partition_2.Part_Size == 0 && Mbr.Mbr_Partition_3.Part_Size == 0 && Mbr.Mbr_Partition_4.Part_Size == 0){
        rewind(f);
        if(ndisk_size > Size){

            Part.Part_Start = mbr_size;//asignacion donde inicia el disco +1
            Mbr.Mbr_Partition_1 = Part;
            //tomamos la posicion donde se almacena o cambiamos la informacion
            fseek(f, sizeof(EDisk),SEEK_SET); //pueod usarlo y modifica info
            fwrite(&Mbr,sizeof(Mbr),1,f);
            printf("Se Creo Particion Exitosamente\n\n");
        }else{
            printf("Error - Tamano de la particion es demsiado grande\n");
        }
    }else if(Mbr.Mbr_Partition_1.Part_Size != 0 && Mbr.Mbr_Partition_2.Part_Size == 0 && Mbr.Mbr_Partition_3.Part_Size == 0 && Mbr.Mbr_Partition_4.Part_Size == 0){
        rewind(f);
        if(ndsk1_size > Size){
            Part.Part_Start = mbr_size + part1_size;//asignacion donde inicia el disco +2
            Mbr.Mbr_Partition_2 = Part;
            //tomamos la posicion donde se almacena o cambiamos la informacion
            fseek(f, sizeof(EDisk),SEEK_SET); //pueod usarlo y modifica info
            fwrite(&Mbr,sizeof(Mbr),1,f);

            fseek(f, Part.Part_Start,SEEK_SET);
            fwrite(&Mbr.Mbr_Partition_2,sizeof(Mbr.Mbr_Partition_2),1,f);

            printf("Se Creo Particion Exitosamente\n\n");
        }else{
            printf("Error - Tamano de la particion es demsiado grande\n");
        }
    }else if(Mbr.Mbr_Partition_1.Part_Size != 0 && Mbr.Mbr_Partition_2.Part_Size != 0 && Mbr.Mbr_Partition_3.Part_Size == 0 && Mbr.Mbr_Partition_4.Part_Size == 0){
        rewind(f);
        if(ndsk2_size > Size){
            Part.Part_Start = mbr_size + part1_size + part2_size;//asignacion donde inicia el disco
            Mbr.Mbr_Partition_3 = Part;
            //tomamos la posicion donde se almacena o cambiamos la informacion
            fseek(f, sizeof(EDisk),SEEK_SET); //pueod usarlo y modifica info
            fwrite(&Mbr,sizeof(Mbr),1,f);

            fseek(f, Part.Part_Start,SEEK_SET);
            fwrite(&Part,sizeof(Part),1,f);
            printf("Se Creo Particion Exitosamente\n\n");
        }else{
            printf("Error - Tamano de la particion es demsiado grande\n");
        }
    }else if(Mbr.Mbr_Partition_1.Part_Size != 0 && Mbr.Mbr_Partition_2.Part_Size != 0 && Mbr.Mbr_Partition_3.Part_Size != 0 && Mbr.Mbr_Partition_4.Part_Size == 0){
        rewind(f);

        if(ndsk3_size > Size){
            Part.Part_Start = mbr_size + part1_size + part2_size + part3_size;//asignacion donde inicia el disco
            Mbr.Mbr_Partition_4 = Part;
            //tomamos la posicion donde se almacena o cambiamos la informacion
            fseek(f, sizeof(EDisk),SEEK_SET); //pueod usarlo y modifica info
            fwrite(&Mbr,sizeof(Mbr),1,f);

            fseek(f, Part.Part_Start,SEEK_SET);
            fwrite(&Part,sizeof(Part),1,f);
            printf("Se Creo Particion Exitosamente\n\n");

        }else{
            printf("Error - Tamano de la particion es demsiado grande\n");
        }
    }else if(Mbr.Mbr_Partition_1.Part_Size != 0 && Mbr.Mbr_Partition_2.Part_Size != 0 && Mbr.Mbr_Partition_3.Part_Size != 0 && Mbr.Mbr_Partition_4.Part_Size != 0){
        printf("Error - Solo puede Crear 4 particiones");
        fclose(f);
        return;
    }else if((part1_fina < part2_ini) && (ndisk_size >= Size)){
        part1_libr = part2_ini - part1_fina;
        if(part1_libr >= Size){
            struct Partition partaux;
            struct Partition partaux2;
            partaux = Mbr.Mbr_Partition_2;

            rewind(f);
            //verificar ndisk_size para ver si funcion de lo contrario quitarlo
            ndisk_size = ndisk_size - part1_size - part2_size - part3_size - part4_size;
            if(ndisk_size >= Size){
                Part.Part_Start = mbr_size + part1_size;//asignacion donde inicia el disco +2
                Mbr.Mbr_Partition_2 = Part;
                partaux2 = Mbr.Mbr_Partition_3;
                Mbr.Mbr_Partition_3 = partaux;//particion2
                Mbr.Mbr_Partition_4 = partaux2;

                //tomamos la posicion donde se almacena o cambiamos la informacion
                fseek(f, sizeof(EDisk),SEEK_SET); //pueod usarlo y modifica info
                fwrite(&Mbr,sizeof(Mbr),1,f);

                fseek(f, Part.Part_Start,SEEK_SET);
                fwrite(&Part,sizeof(Part),1,f);
                printf("Se Creo Particion Exitosamente\n\n");
            }else{
                printf("Error - Tamano de la particion es demsiado grande\n");
            }
        }
    }else if((part2_fina < part3_ini) && (part2_libr >= Size)){
        part2_libr = part3_ini - part2_fina;
        if(part2_libr >= Size){
            struct Partition partaux;
            partaux = Mbr.Mbr_Partition_3;
            rewind(f);
            ndisk_size = ndisk_size - part1_size - part2_size - part3_size - part4_size;
            if(ndisk_size >= Size){
                Part.Part_Start = mbr_size + part1_size + part2_size;//asignacion donde inicia el disco + 3
                Mbr.Mbr_Partition_3 = Part;
                Mbr.Mbr_Partition_4 = partaux;//pasa a se aux 4

                //tomamos la posicion donde se almacena o cambiamos la informacion
                fseek(f, sizeof(EDisk),SEEK_SET); //pueod usarlo y modifica info
                fwrite(&Mbr,sizeof(Mbr),1,f);

                fseek(f, Part.Part_Start,SEEK_SET);
                fwrite(&Part,sizeof(Part),1,f);
                printf("Se Creo Particion Exitosamente\n\n");
            }else{
                printf("Error - Tamano de la particion es demsiado grande\n");
            }
        }
    }else if((part3_fina < part4_ini) && (part3_libr >= Size)){
        part3_libr = part4_ini - part3_fina;
        if(part3_libr >= Size){
            rewind(f);
            ndisk_size = ndisk_size - part1_size - part2_size - part3_size - part4_size;
            if(ndisk_size >= Size){
                Part.Part_Start = mbr_size + part1_size + part2_size + part3_size;//asignacion donde inicia el disco + 4
                Mbr.Mbr_Partition_4 = Part;

                //tomamos la posicion donde se almacena o cambiamos la informacion
                fseek(f, sizeof(EDisk),SEEK_SET); //pueod usarlo y modifica info
                fwrite(&Mbr,sizeof(Mbr),1,f);

                fseek(f, Part.Part_Start,SEEK_SET);
                fwrite(&Part,sizeof(Part),1,f);
                printf("Se Creo Particion Exitosamente\n\n");
            }else{
                printf("Error - Tamano de la particion es demsiado grande\n");
            }
        }
    }else{
        printf("Error - NO hay Suficiente espacio libre para poder crea la Particion\n");
    }
    fclose(f);

}

//*********************************************************************************************************//
//**************************************** Analizador de Entrada ******************************************//

void Comando(char Cadena[300]){
    //char Cadena [300];
    char comando[100];
    char cmd[20];
    //******** booleanos  de Comandos principales
    bool mkdisk = false;
    bool rmdisk = false;
    bool fdisk = false;
    bool mount = false;
    bool comen = false;
    bool execA = false;
    int ElimDisk = 2;
    //size
    bool Size = false;
    int TamDisk;
    //unit
    bool UnitB = false;
    int Unit = 0;
    //path
    bool PthDirB = false;
    char PthDir[100];
    //nombre
    char NameDisk[50];
    bool NameDiskB = false;
    //type
    int TypeDisk = 0;
    bool TypeDiskB = true;
    //fit
    int Fit = 2;
    bool FitB = true;
    //delete
    int Delt = 0;
    bool DeltB = false;
    //add pos and neg
    int Pola = 1;
    bool AddS = false;

    //gets(ps);
    //fflush(stdin);
    //printf("%s \n",ps);
    //gets(Cadena);
    //scanf("%s",&Comando);
    //fflush(stdin);
    //printf(" %s :: %d \n", Cadena, strlen(Cadena));
    int i;
    strcpy(comando, "");
    char *aux;
    for(i=0; i <= strlen(Cadena); i++){

        if(Cadena[i] == '+' || Cadena[i] == '-' || Cadena[i] == ' '){
            aux = Cadena[i];
        }else if(Cadena[i] == '\0' || Cadena[i] == 0){
            //printf("Es un salto de linea\n");
        }else{
            aux = tolower(Cadena[i]);
        }

        if(strcmp(comando, "mkdisk") == 0){
            strcpy(cmd, comando);
            strcpy(comando, "");
            mkdisk = true;
        }else if(strcmp(comando, "fdisk") == 0){
            Unit = 1;
            strcpy(cmd, comando);
            strcpy(comando, "");
            fdisk = true;
        }else if(strcmp(comando, "rmdisk") == 0){
            strcpy(cmd, comando);
            strcpy(comando, "");
            rmdisk = true;
            printf("Esta Seguro de Eliminar Disco...\n");
            printf("0.- Cancelar\n");
            printf("1.- Aceptar\n");
            scanf("%d", &ElimDisk);
            getchar();
            while(ElimDisk != 0 && ElimDisk != 1){
                printf("Seleccione Una Opcion Admitida\n");
                scanf("%d", &ElimDisk);
                getchar();
            }
        }else if(strcmp(comando, "mount") == 0){
            mount = true;
            strcpy(comando, "");
        }else if(strcmp(comando, "exec") == 0){
            int j;
            while(Cadena[i] == ' ' || Cadena[i] == '\n'){
                i++;
                strcpy(comando, "");
            }
            for(j = i; j<strlen(Cadena); j++){
                aux = Cadena[j];
                strcat(comando, &aux);
                if(Cadena[j] == '\n'){
                    break;
                }
                i++;
            }
            i--;
            FileIn(comando);
            strcpy(comando, "");
        }else if(Cadena[i] == ' ' || strcmp(comando, " ") == 0){
            strcpy(comando, "");
        }else if(strcmp(comando, "-size::") == 0){
            int j;
            strcpy(comando, "");
            //strcat(comando, &aux);
            for(j = i; j < strlen(Cadena); j++){
                if(Cadena[i] == '+' || Cadena[i] == '-' || Cadena[i] == ' '){
                    aux = Cadena[j];
                }else{
                    aux = tolower(Cadena[j]);
                }
                if(Cadena[j] == ' '){
                    TamDisk = atoi(comando);
                    break;
                }else{
                    strcat(comando, &aux);
                }
                i++;
            }
            Size = true;
            strcpy(comando, "");
        }else if(strcmp(comando, "+add::") == 0){
            int j;
            strcpy(comando, "");
            for(j = i; j < strlen(Cadena); j++){
                if(Cadena[i] == '+'){// uno positivo
                    Pola = 1;
                }else if( Cadena[i] == '-'){// cero negativo
                    Pola = 0;
                    aux = Cadena[j];
                }else if( Cadena[i] == ' '){
                    aux = Cadena[j];
                }else{
                    aux = tolower(Cadena[j]);
                }
                if(Cadena[j] == ' '){
                    TamDisk = atoi(comando);
                    break;
                }else{
                    strcat(comando, &aux);
                }
                i++;
            }
            AddS = true;
            strcpy(comando, "");
        }else if(strcmp(comando, "+type::") == 0){
            int j;
            strcpy(comando, "");
            //strcat(comando, &aux);
            for(j = i; j < strlen(Cadena); j++){
                if(Cadena[i] == '+' || Cadena[i] == '-' || Cadena[i] == ' '){
                    aux = Cadena[j];
                }else{
                    aux = tolower(Cadena[j]);
                }
                if(Cadena[j] == ' '){
                    if(strcmp(comando, "p") == 0){//primaria 0
                        TypeDisk = 0;
                    }else if(strcmp(comando, "e") == 0){// extendida 1
                        TypeDisk = 1;
                    }else if(strcmp(comando, "l") == 0){//logica 2
                        TypeDisk = 2;
                    }else{
                        TypeDisk = 3;
                        printf("Tipo de Formato no Existe *****\n");
                    }
                    break;
                }else{
                    strcat(comando, &aux);
                }
                i++;
            }
            if(TypeDisk == 3){
                TypeDiskB = false;
            }else{
                TypeDiskB = true;
            }
            strcpy(comando, "");
        }else if(strcmp(comando, "+unit::")==0){
            int j;
            strcpy(comando, "");
            //strcat(comando, &aux);
            for(j = i; j < strlen(Cadena); j++){
                if(Cadena[i] == '+' || Cadena[i] == '-' || Cadena[i] == ' '){
                    aux = Cadena[j];
                }else{
                    aux = tolower(Cadena[j]);
                }
                if(Cadena[j] == ' '){
                    if(strcmp(comando, "k") == 0){//kilobyte 1
                        Unit = 1;
                    }else if(strcmp(comando, "m") == 0){//megabyte 0
                        Unit = 0;
                    }else if(strcmp(comando, "b") == 0){//bytes 2
                        Unit = 2;
                    }else{
                        Unit = 0;
                    }
                    break;
                }else{
                    strcat(comando, &aux);
                }
                i++;
            }
            UnitB = true;
            strcpy(comando, "");
        }else if(strcmp(comando, "+fit::")==0){//fit
            int j;
            strcpy(comando, "");
            //strcat(comando, &aux);
            for(j = i; j < strlen(Cadena); j++){
                if(Cadena[i] == '+' || Cadena[i] == '-' || Cadena[i] == ' '){
                    aux = Cadena[j];
                }else{
                    aux = tolower(Cadena[j]);
                }
                if(Cadena[j] == ' '){
                    if(strcmp(comando, "bf") == 0){//best fit 1
                        Fit = 0;
                    }else if(strcmp(comando, "ff") == 0){//first fit 0
                        Fit = 1;
                    }else if(strcmp(comando, "wf") == 0){//worst fit 2
                        Fit = 2;
                    }else{
                        Fit = 3;//errro
                    }
                    break;
                }else{
                    strcat(comando, &aux);
                }
                i++;
            }
            if(Fit == 3){
                FitB = false;
            }else{
                FitB = true;
            }
            strcpy(comando, "");
        }else if(strcmp(comando, "+delete::")==0){//delete
            int j;
            strcpy(comando, "");
            //strcat(comando, &aux);
            for(j = i; j < strlen(Cadena); j++){
                if(Cadena[i] == '+' || Cadena[i] == '-' || Cadena[i] == ' '){
                    aux = Cadena[j];
                }else{
                    aux = tolower(Cadena[j]);
                }
                if(Cadena[j] == ' '){
                    if(strcmp(comando, "fast")  == 0){//fast delete fit 1
                        Delt = 0;
                    }else if(strcmp(comando, "full") == 0){//full delte fit 0
                        Delt = 1;
                    }else{
                        Delt = 2;
                    }
                    break;
                }else{
                    strcat(comando, &aux);
                }
                i++;
            }
            DeltB = true;
            strcpy(comando, "");
        }else if(strcmp(comando, "-path::")==0){
            int comi = 0;
            int j;
            strcpy(comando, "");
            //strcat(comando, &aux);
            for(j = i; j < strlen(Cadena); j++){
                if(Cadena[i] == '+' || Cadena[i] == '-' || Cadena[i] == ' ' || Cadena[i] == '"'){
                    aux = Cadena[j];
                }else{
                    aux = (Cadena[j]);
                }
                if(Cadena[j] == '"'){
                    if(comi != 0){
                        strcpy(PthDir, comando);
                        break;
                    }else{
                        comi = 1;
                    }
                }else{
                    strcat(comando, &aux);
                }
                i++;
            }
            PthDirB = true;
            strcpy(comando, "");
        }else if(strcmp(comando, "-name::")==0){
            char extt[5];
            strcpy(extt, "");
            int comi = 0;
            int ext = 0;
            int j;
            strcpy(comando, "");
            //strcat(comando, &aux);
            for(j = i; j < strlen(Cadena); j++){
                if(Cadena[i] == '+' || Cadena[i] == '-' || Cadena[i] == ' ' || Cadena[i] == '"'){
                    aux = Cadena[j];
                }else if(Cadena[i] == '.'){
                    ext = 1;
                    aux = Cadena[i];
                }else{
                    aux = (Cadena[j]);
                }
                if(Cadena[j] == '"'){
                    if(comi != 0){
                        strcpy(NameDisk, comando);
                        break;
                    }else{
                        comi = 1;
                    }
                }else{
                    if(ext == 1){
                        strcat(extt, &aux);
                    }
                    if(strcmp(extt, ".dsk") == 0){
                        ext = 2;
                    }
                    strcat(comando, &aux);
                }
                i++;
            }
            if((ext == 1 || ext == 0) && mkdisk == true){
                printf("!!Error!! No Tiene Extension\n");
                NameDiskB = false;
            }else{
                NameDiskB = true;
            }
            strcpy(comando, "");
        }else if(strcmp(comando, "\\") == 0 || Cadena[i] == '\\'){
            gets(Cadena);
            fflush(stdin);
            i = -1;
            strcpy(comando, "");
        }else if(strcmp(comando, "#")==0){
            int j;
            for(j = i; j < strlen(Cadena); j++){
                aux = Cadena[j];
                strcat(comando, &aux);
                if(Cadena[j] == '\0' || Cadena[j] == 0){
                    //fin de linea
                    printf("%s", comando);
                    strcpy(comando, "");
                    break;
                }
                i++;
            }
            i--;
            printf(" ");
        }else{
            strcat(comando, &aux);
            //printf("%c", aux);
        }

    }

    if(mkdisk == true){
        if(NameDiskB == true && Size == true && PthDirB == true){
            NewDisk(NameDisk, TamDisk, Unit, PthDir);
        }else{
            printf("Error Al intentar Crear Disco, Verifique sus Datos\n");
        }
    }else if(rmdisk == true){
        if(PthDirB == true && ElimDisk == 1){
            RmDisk(PthDir);
        }else{
            if(ElimDisk != 0){
                printf("Error Al intentar Remover Disco, Verifique sus Datos\n");
            }
        }
    }else if(fdisk == true){
        if(Size == true && PthDirB == true && NameDiskB && FitB == true && TypeDiskB == true){
            FDisk(TamDisk, PthDir, NameDisk, Unit, TypeDisk, Fit);
        }else{
            printf("Error al Intentar crear Particion, Verifique sus Datos");
        }
    }else if(mount == true){

    }
}

void Prueba(){
    FILE *f = fopen ("/home/gremory/Escritorio/casas/Disco1.dsk", "rb+");
    struct EstDisk tdisco;
    struct MasterBoot mbr;
    struct Partition part;
    fread(&tdisco,sizeof(tdisco),1,f);
    fread(&mbr, sizeof(mbr), 1,f);

    printf("tamaño: %d MB \n", tdisco.tamano);
    printf("Nombre: %s\n\n", tdisco.nombre);
    printf("Mbr*******:\n");
    printf("tamaño: %d MB \n", mbr.Mbr_Tamano);
    printf("Fecha: %s\n", mbr.Mbr_Fecha_Creacion);
    printf("Ramdon: %d\n", mbr.Mbr_Disk_Signature);
    mbr.Mbr_Disk_Signature = 99999;
    mbr.Mbr_Tamano = 1024;
    //rewind(f);//para posicionarse al principio//si

    part = mbr.Mbr_Partition_1;
    printf("Imprimer info de la particion que se creo\n\n");
    printf("Estatus: %s\n", part.Part_Status);
    printf("Type: %s\n", part.Part_Type);
    printf("Fit: %s\n", part.Part_Fit);
    printf("Inicio: %d\n", part.Part_Start);
    printf("Size: %d\n", part.Part_Size);
    printf("Nombre: %s\n", part.Part_Name);
    printf("Uso: %d\n", part.Uso);
    printf("**************************\n");
    printf("Nombre 1 %s\n", mbr.Mbr_Partition_1.Part_Name);
    printf("Nombre 2 %s\n", mbr.Mbr_Partition_2.Part_Name);
    printf("Nombre 3 %s\n", mbr.Mbr_Partition_3.Part_Name);
    printf("Nombre 4 %s\n", mbr.Mbr_Partition_4.Part_Name);
    int nn = sizeof(mbr) + sizeof(tdisco);
    printf("tamanod utilizado: %d\n", nn);

    /*
    char Part_Status[4];//indica si esta activa  o no
    char Part_Type[4];//indica tipo de particion Primario o Extendida p o E
    char Part_Fit[4];//Tipo de ajuste Valores BF (Best) FF(First WF(Worst)
    int Part_Start;//Indica en que Byte Inicia el Disco
    int Part_Size;//Contiene el Tamano total de la particion
    char Part_Name[16];//Nombre de la particion
    */


    //fseek(f, sizeof(tdisco),SEEK_SET); //pueod usarlo y modifica info//si
    //fwrite(&mbr,sizeof(mbr),1,f);//si
    //fwrite(&tdisco,sizeof(tdisco),1,f);
    fclose(f);
    //Mkdisk -Size::4096 +unit::K -path::"/home/gremory/Escritorio/casas/" -name::"Disco1.dsk"
}

void FileIn(char direc[150]){
    FILE *fichero;
    char texto[350];

    fichero = fopen(direc,"r");
    if (fichero == NULL) {
        printf( "Error - No se pudo leer el Fichero.\n" );
        return;
    }

    char Cadena [300];
    char comando[100];
    char cmd[20];
    //******** booleanos  de Comandos principales
    bool mkdisk = false;
    bool rmdisk = false;
    bool fdisk = false;
    bool mount = false;
    bool comen = false;
    int ElimDisk = 2;
    bool Size = false;
    int TamDisk;
    bool UnitB = false;
    int Unit = 0;
    bool PthDirB = false;
    char PthDir[100];
    char NameDisk[50];
    bool NameDiskB = false;
    int TypeDisk = 0;
    bool TypeDiskB = true;
    int Fit = 2;
    bool FitB = true;
    int Delt = 0;
    bool DeltB = false;
    int Pola = 1;
    bool AddS = false;

    strcpy(comando, "");
    char *aux;

    printf("Inicio de Analiis del Fichero:              ******************\n" );
    fgets(Cadena,350,fichero);//leemos la primera linea del archivo
    while (feof(fichero) == 0) {

        comando[100];
        cmd[20];
        //******** booleanos  de Comandos principales
        mkdisk = false;
        rmdisk = false;
        fdisk = false;
        mount = false;
        comen = false;
        ElimDisk = 2;
        Size = false;
        TamDisk;
        UnitB = false;
        Unit = 0;
        PthDirB = false;
        PthDir[100];
        NameDisk[50];
        NameDiskB = false;
        TypeDisk = 0;
        TypeDiskB = true;
        Fit = 2;
        FitB = true;
        Delt = 0;
        DeltB = false;
        Pola = 1;
        AddS = false;

        int i;
        strcpy(comando, "");
        printf( "%s",Cadena);
        strcpy(comando, "");
        for(i=0; i < strlen(Cadena); i++){

            if(Cadena[i] == '+' || Cadena[i] == '-' || Cadena[i] == ' ' || Cadena[i] == '#'){
                aux = Cadena[i];
            }else if(Cadena[i] == '\0' || Cadena[i] == 0){
                //printf("Es un salto de linea\n");
            }else{
                aux = tolower(Cadena[i]);
            }

            if(strcmp(comando, "mkdisk") == 0){
                strcpy(cmd, comando);
                strcpy(comando, "");
                mkdisk = true;
            }else if(strcmp(comando, "fdisk") == 0){
                Unit = 1;
                strcpy(cmd, comando);
                strcpy(comando, "");
                fdisk = true;
            }else if(strcmp(comando, "rmdisk") == 0){
                strcpy(cmd, comando);
                strcpy(comando, "");
                rmdisk = true;
                printf("Esta Seguro de Eliminar Disco...\n");
                printf("0.- Cancelar\n");
                printf("1.- Aceptar\n");
                scanf("%d", &ElimDisk);
                getchar();
                while(ElimDisk != 0 && ElimDisk != 1){
                    printf("Seleccione Una Opcion Admitida\n");
                    scanf("%d", &ElimDisk);
                    getchar();
                }
            }else if(strcmp(comando, "mount") == 0){
                mount = true;
                strcpy(comando, "");
            }else if(Cadena[i] == ' ' || strcmp(comando, " ") == 0){
                strcpy(comando, "");
            }else if(strcmp(comando, "-size::") == 0){
                int j;
                strcpy(comando, "");
                //strcat(comando, &aux);
                for(j = i; j < strlen(Cadena); j++){
                    if(Cadena[i] == '+' || Cadena[i] == '-' || Cadena[i] == ' '){
                        aux = Cadena[j];
                    }else{
                        aux = tolower(Cadena[j]);
                    }
                    if(Cadena[j] == ' '){
                        TamDisk = atoi(comando);
                        break;
                    }else{
                        strcat(comando, &aux);
                    }
                    i++;
                }
                Size = true;
                strcpy(comando, "");
            }else if(strcmp(comando, "+add::") == 0){
                int j;
                strcpy(comando, "");
                for(j = i; j < strlen(Cadena); j++){
                    if(Cadena[i] == '+'){// uno positivo
                        Pola = 1;
                    }else if( Cadena[i] == '-'){// cero negativo
                        Pola = 0;
                        aux = Cadena[j];
                    }else if( Cadena[i] == ' '){
                        aux = Cadena[j];
                    }else{
                        aux = tolower(Cadena[j]);
                    }
                    if(Cadena[j] == ' '){
                        TamDisk = atoi(comando);
                        break;
                    }else{
                        strcat(comando, &aux);
                    }
                    i++;
                }
                AddS = true;
                strcpy(comando, "");
            }else if(strcmp(comando, "+type::") == 0){
                int j;
                strcpy(comando, "");
                //strcat(comando, &aux);
                for(j = i; j < strlen(Cadena); j++){
                    if(Cadena[i] == '+' || Cadena[i] == '-' || Cadena[i] == ' '){
                        aux = Cadena[j];
                    }else{
                        aux = tolower(Cadena[j]);
                    }
                    if(Cadena[j] == ' '){
                        if(strcmp(comando, "p") == 0){//primaria 0
                            TypeDisk = 0;
                        }else if(strcmp(comando, "e") == 0){// extendida 1
                            TypeDisk = 1;
                        }else if(strcmp(comando, "l") == 0){//logica 2
                            TypeDisk = 2;
                        }else{
                            TypeDisk = 3;
                            printf("Tipo de Formato no Existe *****\n");
                        }
                        break;
                    }else{
                        strcat(comando, &aux);
                    }
                    i++;
                }
                if(TypeDisk == 3){
                    TypeDiskB = false;
                }else{
                    TypeDiskB = true;
                }
                strcpy(comando, "");
            }else if(strcmp(comando, "+unit::")==0){
                int j;
                strcpy(comando, "");
                //strcat(comando, &aux);
                for(j = i; j < strlen(Cadena); j++){
                    if(Cadena[i] == '+' || Cadena[i] == '-' || Cadena[i] == ' '){
                        aux = Cadena[j];
                    }else{
                        aux = tolower(Cadena[j]);
                    }
                    if(Cadena[j] == ' '){
                        if(strcmp(comando, "k") == 0){//kilobyte 1
                            Unit = 1;
                        }else if(strcmp(comando, "m") == 0){//megabyte 0
                            Unit = 0;
                        }else if(strcmp(comando, "b") == 0){//bytes 2
                            Unit = 2;
                        }else{
                            Unit = 0;
                        }
                        break;
                    }else{
                        strcat(comando, &aux);
                    }
                    i++;
                }
                UnitB = true;
                strcpy(comando, "");
            }else if(strcmp(comando, "+fit::")==0){//fit
                int j;
                strcpy(comando, "");
                //strcat(comando, &aux);
                for(j = i; j < strlen(Cadena); j++){
                    if(Cadena[i] == '+' || Cadena[i] == '-' || Cadena[i] == ' '){
                        aux = Cadena[j];
                    }else{
                        aux = tolower(Cadena[j]);
                    }
                    if(Cadena[j] == ' '){
                        if(strcmp(comando, "bf") == 0){//best fit 1
                            Fit = 0;
                        }else if(strcmp(comando, "ff") == 0){//first fit 0
                            Fit = 1;
                        }else if(strcmp(comando, "wf") == 0){//worst fit 2
                            Fit = 2;
                        }else{
                            Fit = 3;//errro
                        }
                        break;
                    }else{
                        strcat(comando, &aux);
                    }
                    i++;
                }
                if(Fit == 3){
                    FitB = false;
                }else{
                    FitB = true;
                }
                strcpy(comando, "");
            }else if(strcmp(comando, "+delete::")==0){//delete
                int j;
                strcpy(comando, "");
                //strcat(comando, &aux);
                for(j = i; j < strlen(Cadena); j++){
                    if(Cadena[i] == '+' || Cadena[i] == '-' || Cadena[i] == ' '){
                        aux = Cadena[j];
                    }else{
                        aux = tolower(Cadena[j]);
                    }
                    if(Cadena[j] == ' '){
                        if(strcmp(comando, "fast")  == 0){//fast delete fit 1
                            Delt = 0;
                        }else if(strcmp(comando, "full") == 0){//full delte fit 0
                            Delt = 1;
                        }else{
                            Delt = 2;
                        }
                        break;
                    }else{
                        strcat(comando, &aux);
                    }
                    i++;
                }
                DeltB = true;
                strcpy(comando, "");
            }else if(strcmp(comando, "-path::")==0){
                int comi = 0;
                int j;
                strcpy(comando, "");
                //strcat(comando, &aux);
                for(j = i; j < strlen(Cadena); j++){
                    if(Cadena[i] == '+' || Cadena[i] == '-' || Cadena[i] == ' ' || Cadena[i] == '"'){
                        aux = Cadena[j];
                    }else{
                        aux = (Cadena[j]);
                    }
                    if(Cadena[j] == '"'){
                        if(comi != 0){
                            strcpy(PthDir, comando);
                            break;
                        }else{
                            comi = 1;
                        }
                    }else{
                        strcat(comando, &aux);
                    }
                    i++;
                }
                PthDirB = true;
                strcpy(comando, "");
            }else if(strcmp(comando, "-name::")==0){
                char extt[5];
                strcpy(extt, "");
                int comi = 0;
                int ext = 0;
                int j;
                strcpy(comando, "");
                //strcat(comando, &aux);
                for(j = i; j < strlen(Cadena); j++){
                    if(Cadena[i] == '+' || Cadena[i] == '-' || Cadena[i] == ' ' || Cadena[i] == '"'){
                        aux = Cadena[j];
                    }else if(Cadena[i] == '.'){
                        ext = 1;
                        aux = Cadena[i];
                    }else{
                        aux = (Cadena[j]);
                    }
                    if(Cadena[j] == '"'){
                        if(comi != 0){
                            strcpy(NameDisk, comando);
                            break;
                        }else{
                            comi = 1;
                        }
                    }else{
                        if(ext == 1){
                            strcat(extt, &aux);
                        }
                        if(strcmp(extt, ".dsk") == 0){
                            ext = 2;
                        }
                        strcat(comando, &aux);
                    }
                    i++;
                }
                if((ext == 1 || ext == 0) && mkdisk == true){
                    printf("!!Error!! No Tiene Extension\n");
                    NameDiskB = false;
                }else{
                    NameDiskB = true;
                }
                strcpy(comando, "");
            }else if(strcmp(comando, "\\") == 0 || Cadena[i] == '\\'){
                fgets(Cadena,350,fichero);
                i = -1;
                strcpy(comando, "");
            }else{
                strcat(comando, &aux);
            }

        }
        if(mkdisk == true){
            if(NameDiskB == true && Size == true && PthDirB == true){
                NewDisk(NameDisk, TamDisk, Unit, PthDir);
            }else{
                printf("Error Al intentar Crear Disco, Verifique sus Datos\n");
            }
        }else if(rmdisk == true){
            if(PthDirB == true && ElimDisk == 1){
                RmDisk(PthDir);
            }else{
                if(ElimDisk != 0){
                    printf("Error Al intentar Remover Disco, Verifique sus Datos\n");
                }
            }
        }else if(fdisk == true){
            if(Size == true && PthDirB == true && NameDiskB && FitB == true && TypeDiskB == true){
                FDisk(TamDisk, PthDir, NameDisk, Unit, TypeDisk, Fit);
            }else{
                printf("Error al Intentar crear Particion, Verifique sus Datos");
            }
        }else if(mount == true){

        }

        fgets(Cadena,350,fichero);
    }

    if (fclose(fichero)!=0)
        printf( "Problemas al cerrar el fichero\n" );
}
/*
else if(strcmp(comando, "#")==0){
                int j;
                for(j = i; j < strlen(Cadena); j++){
                    aux = Cadena[j];
                    strcat(comando, &aux);
                    printf("%s", comando);
                    strcpy(comando, "");
                    i++;
                }
                i--;
                printf(" ");
            }
*/
//*********************************************************************************************************//
//********************************* Menu de Aplicacion **************************************//

int MenuSelect(){
    int SelectOp = 0;
    printf("********************** Particion de Discos *********************\n");
    printf("0.- Salir del Sistema\n");
    printf("1.- Ingreso de Comandos\n");
    printf("\n\n");
    printf("Elige una Opcion\n");
    scanf("%d", &SelectOp);
    getchar();
    return SelectOp;
}

void Menu(){
    char OptionSelect;
    do{
        OptionSelect = MenuSelect();
        switch (OptionSelect) {
        case 1:

            break;
        default:
            break;
        }
    }while(OptionSelect != '0');
}

int main(int argc, char *argv[])
{

    char Cadena[300];
    strcpy(Cadena, "");
    while(strcmp(Cadena, "exit") < 0 || strcmp(Cadena, "exit") > 0 ){
        gets(Cadena);
        fflush(stdin);
        Comando(Cadena);
    }

    //Menu();
    //NewDisk();

    //int a = 65;
    //printf("%c\n", (char)a);
    //char b [4];
    //strcpy(b, "C");
    //printf("%d\n", (int)b[0]);


    //Comando();
    //Prueba();
    return 0;
}

void ComandosIN(){

}

/*
 * char *aux;
        if(Cadena[i] == '-' || Cadena[i] == '+'){
            aux = (Cadena[i]);
        }else{
            aux = tolower(Cadena[i]);
        }
        */
//multiples carpetas al mismo tiempo/ mkdir -p path


//if dnde verificaba parte no completo

/*
    else if(Mbr.Mbr_Partition_1.Part_Size != 0 && Mbr.Mbr_Partition_2.Part_Size == 0 && Mbr.Mbr_Partition_3.Part_Size == 0 && Mbr.Mbr_Partition_4.Part_Size == 0){
        rewind(f);
        ndisk_size = ndisk_size - part1_size;
        if(ndisk_size > Size){
            Part.Part_Start = mbr_size + part1_size;//asignacion donde inicia el disco +2
            Mbr.Mbr_Partition_2 = Part;
            //tomamos la posicion donde se almacena o cambiamos la informacion
            fseek(f, sizeof(EDisk),SEEK_SET); //pueod usarlo y modifica info
            fwrite(&Mbr,sizeof(Mbr),1,f);
        }else{
            printf("Error - Tamano de la particion es demsiado grande\n");
        }
    }else if(Mbr.Mbr_Partition_1.Part_Size != 0 && Mbr.Mbr_Partition_2.Part_Size != 0 && Mbr.Mbr_Partition_3.Part_Size == 0 && Mbr.Mbr_Partition_4.Part_Size == 0){
        rewind(f);
        ndisk_size = ndisk_size - part1_size - part2_size;
        if(ndisk_size > Size){
            Part.Part_Start = mbr_size + part1_size + part2_size;//asignacion donde inicia el disco
            Mbr.Mbr_Partition_3 = Part;
            //tomamos la posicion donde se almacena o cambiamos la informacion
            fseek(f, sizeof(EDisk),SEEK_SET); //pueod usarlo y modifica info
            fwrite(&Mbr,sizeof(Mbr),1,f);
        }else{
            printf("Error - Tamano de la particion es demsiado grande\n");
        }
    }else if(Mbr.Mbr_Partition_1.Part_Size != 0 && Mbr.Mbr_Partition_2.Part_Size != 0 && Mbr.Mbr_Partition_3.Part_Size != 0 && Mbr.Mbr_Partition_4.Part_Size == 0){
        rewind(f);
        ndisk_size = ndisk_size - part1_size - part2_size - part3_size;
        if(ndisk_size > Size){
            Part.Part_Start = mbr_size + part1_size + part2_size;//asignacion donde inicia el disco
            Mbr.Mbr_Partition_4 = Part;
            //tomamos la posicion donde se almacena o cambiamos la informacion
            fseek(f, sizeof(EDisk),SEEK_SET); //pueod usarlo y modifica info
            fwrite(&Mbr,sizeof(Mbr),1,f);
        }else{
            printf("Error - Tamano de la particion es demsiado grande\n");
        }
    }




    else if(Mbr.Mbr_Partition_1.Part_Size == 0 && Mbr.Mbr_Partition_2.Part_Size == 0 && Mbr.Mbr_Partition_3.Part_Size == 0 && Mbr.Mbr_Partition_4.Part_Size != 0){
        rewind(f);
        part4_size = Mbr.Mbr_Partition_4.Part_Size;

        ndisk_size = ndisk_size - part4_size;
        if(ndisk_size > Size){
            Part.Part_Start = mbr_size + 1;//asignacion donde inicia el disco
            Mbr.Mbr_Partition_1 = Part;
            //tomamos la posicion donde se almacena o cambiamos la informacion
            fseek(f, sizeof(EDisk),SEEK_SET); //pueod usarlo y modifica info
            fwrite(&Mbr,sizeof(Mbr),1,f);
        }else{
            printf("Error - Tamano de la particion es demsiado grande\n");
        }
    }else if(Mbr.Mbr_Partition_1.Part_Size != 0 && Mbr.Mbr_Partition_2.Part_Size == 0 && Mbr.Mbr_Partition_3.Part_Size == 0 && Mbr.Mbr_Partition_4.Part_Size != 0){
        rewind(f);
        part1_size = Mbr.Mbr_Partition_1.Part_Size;
        part4_size = Mbr.Mbr_Partition_4.Part_Size;

        ndisk_size = ndisk_size - part1_size - part4_size;
        if(ndisk_size > Size){
            Part.Part_Start = mbr_size + part1_size + part4_size + 3;//asignacion donde inicia el disco
            Mbr.Mbr_Partition_2 = Part;
            //tomamos la posicion donde se almacena o cambiamos la informacion
            fseek(f, sizeof(EDisk),SEEK_SET); //pueod usarlo y modifica info
            fwrite(&Mbr,sizeof(Mbr),1,f);
        }else{
            printf("Error - Tamano de la particion es demsiado grande\n");
        }
    }else if(Mbr.Mbr_Partition_1.Part_Size != 0 && Mbr.Mbr_Partition_2.Part_Size != 0 && Mbr.Mbr_Partition_3.Part_Size != 0 && Mbr.Mbr_Partition_4.Part_Size != 0){
        printf("Error - Solo puede Crear 4 particiones");
        fclose(f);
    }else{

    }

*/
