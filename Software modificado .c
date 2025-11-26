#include <stdio.h>
#include <string.h>
#include <stdlib.h> 


//TIPOS DE DATOS Y CONSTANTES

//Constantes para definir el tamaño y el stock mínimo.
#define MAX_PRODUCTOS 100
#define MIN_EXISTENCIAS_REQUERIDAS 100
#define MAX_ARTICULOS_VENTA 100
#define MAX_NOMBRE_LONGITUD 50

//Estructura para almacenar la información de cada producto.
struct Producto {
    char nombre[MAX_NOMBRE_LONGITUD];
    float precio;
    int existencia_actual; 
    int unidades_vendidas; 
};

//Estructura para un item dentro de una transacción.
struct Ventaarticulos {
    char nombre[MAX_NOMBRE_LONGITUD];
    float precio;
    int cantidad;
};


 //VARIABLES GLOBALES
struct Producto inventario_global[MAX_PRODUCTOS];
int total_productos = 0; 

struct Ventaarticulos venta_actual[MAX_ARTICULOS_VENTA];
int total_articulos_venta = 0;
float total_a_pagar = 0.0; 

//FUNCIONES AUXILIARES
//Limpia el buffer de entrada (stdin) para evitar errores de lectura.
void limpiar_buffer() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

//Busca un producto por nombre en el inventario.
int buscar_producto(const char* nombre_buscar) {
    int i; 
    for (i = 0; i < total_productos; i++) {
        if (strcmp(nombre_buscar, inventario_global[i].nombre) == 0) {
            return i;
        }
    }
    return -1;
}

//Función de comparación para qsort. Ordena por unidades vendidas descendente.
int comparar_vendidos(const void *a, const void *b) {
    return ((struct Producto *)b)->unidades_vendidas - ((struct Producto *)a)->unidades_vendidas;
}


//GESTION DE VENTAS
//Reinicia las variables globales de la venta actual.
void iniciar_nueva_venta() {
    total_articulos_venta = 0;
    total_a_pagar = 0.0;
    printf("\n--- NUEVA VENTA INICIADA ---\n");
}

//Agrega un producto a la transacción actual.
void agregar_producto_venta() {
    char nombre_buscar[MAX_NOMBRE_LONGITUD];
    int cantidad = 0; 
    int indice_producto;
    
    if (total_articulos_venta >= MAX_ARTICULOS_VENTA) {
        printf("Limite de productos en una sola venta alcanzado (%d).\n", MAX_ARTICULOS_VENTA);
        return;
    }
    
    printf("Ingrese nombre del producto: ");
    scanf("%49s", nombre_buscar); 
    
    indice_producto = buscar_producto(nombre_buscar);
    
    if (indice_producto != -1) {
        //Validación de stock
        if (inventario_global[indice_producto].existencia_actual <= 0) {
            printf("Producto agotado. No se puede vender.\n");
            return;
        }
        
        printf(" >> %s | Precio: %.2f | Existencia: %d\n", 
            inventario_global[indice_producto].nombre, 
            inventario_global[indice_producto].precio, 
            inventario_global[indice_producto].existencia_actual);
        
        printf("Cantidad (Max %d): ", inventario_global[indice_producto].existencia_actual);
        
        //Lectura de cantidad con manejo de error.
        if (scanf("%d", &cantidad) != 1) { 
            printf("Error de entrada de cantidad.\n");
            limpiar_buffer(); 
            return;
        }
        
        if (cantidad > inventario_global[indice_producto].existencia_actual || cantidad <= 0) {
            printf("Cantidad invalida o insuficiente. Venta no agregada.\n");
            return;
        }
        
        //Agregar ítem a la venta.
        strcpy(venta_actual[total_articulos_venta].nombre, inventario_global[indice_producto].nombre);
        venta_actual[total_articulos_venta].precio = inventario_global[indice_producto].precio;
        venta_actual[total_articulos_venta].cantidad = cantidad;
        
        total_a_pagar += venta_actual[total_articulos_venta].precio * 
		venta_actual[total_articulos_venta].cantidad;
        total_articulos_venta++;
        
        printf("Producto agregado. Total acumulado: %.2f\n", total_a_pagar);
    } else {
        printf("Producto no encontrado.\n");
    }
}

//Procesa el pago, calcula cambio, actualiza inventario y genera factura.
void procesar_pago() {
    float monto_entregado = 0.0;
    float cambio = 0.0;
    char confirmacion;
    int i; 

    if (total_articulos_venta == 0) {
        printf("No hay productos en la venta actual.\n");
        return;
    }
    
    printf("\nTOTAL A PAGAR: %.2f\n", total_a_pagar);
    
    printf("Ingrese monto entregado: ");
    if (scanf("%f", &monto_entregado) != 1) { 
        printf("Error de entrada.\n"); 
        limpiar_buffer(); 
        return; 
    }
    
    if (monto_entregado < total_a_pagar) {
        printf("Dinero insuficiente. Pago cancelado.\n");
        return;
    }
    
    cambio = monto_entregado - total_a_pagar;
    printf("CAMBIO: %.2f\n", cambio);
    
    printf("Confirmar pago (S/N): ");
    scanf(" %c", &confirmacion);
    
    if (confirmacion == 'S' || confirmacion == 's') {
        //Bucle para actualizar stock e historial de ventas.
        for (i = 0; i < total_articulos_venta; i++) {
            int indice = buscar_producto(venta_actual[i].nombre);
            if (indice != -1) {
                inventario_global[indice].existencia_actual -= venta_actual[i].cantidad;
                inventario_global[indice].unidades_vendidas += venta_actual[i].cantidad;
            }
        }
        
        //Generación de Factura.
        printf("\n=========================================\n");
        printf("         FACTURA DE VENTA\n");
        printf("=========================================\n");
        printf("Producto | Precio | Cantidad | Subtotal\n");
        for (i = 0; i < total_articulos_venta; i++) { 
            printf("%s | %.2f | %d | %.2f\n", 
                venta_actual[i].nombre, 
                venta_actual[i].precio, 
                venta_actual[i].cantidad, 
                venta_actual[i].precio * venta_actual[i].cantidad);
        }
        printf("-----------------------------------------\n");
        printf("TOTAL: %.2f | PAGO: %.2f | CAMBIO: %.2f\n", total_a_pagar, monto_entregado, cambio);
        printf("=========================================\n");
        
        iniciar_nueva_venta();
    } else {
        printf("Pago cancelado.\n");
    }
}

//Menú principal para la gestión de ventas.
void menu_ventas() {
    int sub_opcion = 0; 
    do {
        printf("\n--- MENU VENTA ---\n");
        printf("1. Ingresar Producto\n");
        printf("2. Pagar y Finalizar Venta\n");
        printf("3. Iniciar Nueva Venta (Borrar Todo)\n");
        printf("4. Cancelar Venta (Volver al Menu Principal)\n");
        printf("TOTAL ACTUAL: %.2f | ARTICULOS: %d\n", total_a_pagar, total_articulos_venta);
        printf("Seleccione una opcion: ");
        
        if (scanf("%d", &sub_opcion) != 1) { 
            limpiar_buffer(); 
            sub_opcion = 0; 
            continue; 
        }
        
        if (sub_opcion == 1) {
            agregar_producto_venta();
        } else if (sub_opcion == 2) {
            procesar_pago();
            if (total_articulos_venta == 0) break;
        } else if (sub_opcion == 3) {
            iniciar_nueva_venta();
        } else if (sub_opcion == 4) {
            if (total_articulos_venta > 0) {
                printf("Venta con productos. Confirme cancelacion (S/N): ");
                char conf;
                scanf(" %c", &conf);
                if (conf == 'S' || conf == 's') {
                    iniciar_nueva_venta();
                } else {
                    continue;
                }
            }
            break;
        }
    } while (1);
}

//GESTION DE INVENTARIO
//Muestra la lista completa de productos con el estado del stock.
void ver_inventario() {
    int i; 
    printf("\n--- LISTA DE PRODUCTOS (%d en total) ---\n", total_productos);
    for (i = 0; i < total_productos; i++) {
        printf("%d. %s | Precio: %.2f | Existencia: %d de %d\n", 
            i + 1, 
            inventario_global[i].nombre, 
            inventario_global[i].precio, 
            inventario_global[i].existencia_actual, 
            MIN_EXISTENCIAS_REQUERIDAS);
            
        if (inventario_global[i].existencia_actual < MIN_EXISTENCIAS_REQUERIDAS) {
            printf("  (Faltan %d para llegar al minimo)\n", MIN_EXISTENCIAS_REQUERIDAS - 
			inventario_global[i].existencia_actual);
        }
    }
}

//Agrega un nuevo producto al inventario global.
void agregar_producto() {
    if (total_productos < MAX_PRODUCTOS) {
        printf("\nIngrese nombre del producto: ");
        scanf("%49s", inventario_global[total_productos].nombre);
        
        printf("Ingrese precio: ");
        scanf("%f", &inventario_global[total_productos].precio);
        
        printf("Ingrese existenica inicial: ");
        scanf("%d", &inventario_global[total_productos].existencia_actual);
        
        inventario_global[total_productos].unidades_vendidas = 0;
        total_productos++;
        
        printf("Producto agregado con exito.\n");
    } else {
        printf("Inventario lleno. Maximo %d productos.\n", MAX_PRODUCTOS);
    }
}

//Menú para gestionar el inventario (Agregar, Ver, Buscar).
void menu_inventario() {
    int sub;
    char buscar[MAX_NOMBRE_LONGITUD];
    int encontrado;
    do {
        printf("\n--- MENU INVENTARIO ---\n");
        printf("1. Agregar nuevo producto\n");
        printf("2. Ver inventario (existencia minima %d)\n", MIN_EXISTENCIAS_REQUERIDAS);
        printf("3. Buscar Producto\n");
        printf("4. Volver al Menu Principal\n");
        printf("Seleccione una opcion: ");
        
        if (scanf("%d", &sub) != 1) { 
            limpiar_buffer(); 
            sub = 0; 
            continue; 
        }
        
        if (sub == 1) {
            agregar_producto();
        } else if (sub == 2) {
            ver_inventario();
        } else if (sub == 3) {
            printf("Ingrese nombre del producto a buscar: ");
            scanf("%49s", buscar);
            encontrado = buscar_producto(buscar);
            if (encontrado != -1) {
                printf("ENCONTRADO: %s | Precio: %.2f | Existencia: %d de %d\n", 
                    inventario_global[encontrado].nombre, 
                    inventario_global[encontrado].precio, 
                    inventario_global[encontrado].existencia_actual, 
                    MIN_EXISTENCIAS_REQUERIDAS);
            } else {
                printf("Producto no encontrado.\n");
            }
        }
    } while (sub != 4);
}

//CALCULOS Y ESTADISTICAS
//Muestra las estadísticas de los productos más y menos vendidos.
void menu_calculos() {
    int i; 
    printf("\n--- CALCULOS / ESTADISTICAS ---\n");
    if (total_productos == 0) { 
        printf("No hay productos registrados para estadisticas.\n"); 
        return; 
    }
    
    //Se crea una copia para ordenar sin afectar el inventario global.
    struct Producto temporal_inventario[MAX_PRODUCTOS]; 
    for(i=0; i<total_productos; i++) { 
        temporal_inventario[i] = inventario_global[i]; 
    }
    
    //Se ordena el inventario temporal.
    qsort(temporal_inventario, total_productos, sizeof(struct Producto), comparar_vendidos);
    
    printf("\n--- TOP 5 PRODUCTOS MAS VENDIDOS ---\n");
    for (i = 0; i < total_productos && i < 5; i++) { 
        printf("%d. %s (Vendidos: %d)\n", i + 1, temporal_inventario[i].nombre, 
		temporal_inventario[i].unidades_vendidas);
    }
    
    printf("\n--- TOP 3 PRODUCTOS MENOS VENDIDOS ---\n");
    //Se itera desde el final del arreglo ordenado (los menos vendidos).
    for (i = total_productos - 1; i >= 0 && i >= total_productos - 3; i--) { 
        printf("%d. %s (Vendidos: %d)\n", total_productos - i, temporal_inventario[i].nombre, 
		temporal_inventario[i].unidades_vendidas);
    }
}


//INICIO DE SESION
//Maneja la lógica de inicio de sesión.
void iniciar_sesion() {
    const char usuario_correcto[] = "admin";
    const char contrasena_correcta[] = "1234";
    char usuario[20];
    char contrasena[20];
    int inicio = 0;
    
    while (inicio == 0) {
        printf("\n=== INICIAR SESION ===\n");
        printf("Usuario: ");
        scanf("%19s", usuario);
        printf("Contrasena: ");
        scanf("%19s", contrasena);
        
        if (strcmp(usuario, usuario_correcto) == 0 && strcmp(contrasena, contrasena_correcta) == 0) {
            printf("\nInicio de sesion exitoso.\n");
            inicio = 1;
        } else {
            printf("\nUsuario o contrasena incorrectos. Intente nuevamente.\n");
        }
    }
}

//Carga todos los productos iniciales al inventario.
void cargar_inventario_inicial() {

    //Producto 1
    strcpy(inventario_global[total_productos].nombre, "Dona");
    inventario_global[total_productos].precio = 10.00;
    inventario_global[total_productos].existencia_actual = 100;
    inventario_global[total_productos].unidades_vendidas = 0;
    total_productos++;
    
    //Producto 2 
    strcpy(inventario_global[total_productos].nombre, "Rellidona");
    inventario_global[total_productos].precio = 15.00;
    inventario_global[total_productos].existencia_actual = 100;
    inventario_global[total_productos].unidades_vendidas = 0;
    total_productos++;
    
    //Producto 3
    strcpy(inventario_global[total_productos].nombre, "Oreja");
    inventario_global[total_productos].precio = 8.50;
    inventario_global[total_productos].existencia_actual = 100;
    inventario_global[total_productos].unidades_vendidas = 0;
    total_productos++;
    
    //Producto 4 
    strcpy(inventario_global[total_productos].nombre, "Concha");
    inventario_global[total_productos].precio = 6.50;
    inventario_global[total_productos].existencia_actual = 100;
    inventario_global[total_productos].unidades_vendidas = 0;
    total_productos++;
    
    //Producto 5 
    strcpy(inventario_global[total_productos].nombre, "Bisquet");
    inventario_global[total_productos].precio = 12.50;
    inventario_global[total_productos].existencia_actual = 100;
    inventario_global[total_productos].unidades_vendidas = 0;
    total_productos++;
    
    //Producto 6 
    strcpy(inventario_global[total_productos].nombre, "Muffin");
    inventario_global[total_productos].precio = 15.50;
    inventario_global[total_productos].existencia_actual = 100;
    inventario_global[total_productos].unidades_vendidas = 0;
    total_productos++;
    
    //Producto 7
    strcpy(inventario_global[total_productos].nombre, "Mantequilla");
    inventario_global[total_productos].precio = 6.50;
    inventario_global[total_productos].existencia_actual = 100;
    inventario_global[total_productos].unidades_vendidas = 0;
    total_productos++;    
    
    //Producto 8 
    strcpy(inventario_global[total_productos].nombre, "Abanico");
    inventario_global[total_productos].precio = 6.50;
    inventario_global[total_productos].existencia_actual = 100;
    inventario_global[total_productos].unidades_vendidas = 0;
    total_productos++;    
    
    //Producto 9 
    strcpy(inventario_global[total_productos].nombre, "Galleta");
    inventario_global[total_productos].precio = 8.50;
    inventario_global[total_productos].existencia_actual = 100;
    inventario_global[total_productos].unidades_vendidas = 0;
    total_productos++;    
    
    //Producto 10 
    strcpy(inventario_global[total_productos].nombre, "Polvoron");
    inventario_global[total_productos].precio = 6.50;
    inventario_global[total_productos].existencia_actual = 100;
    inventario_global[total_productos].unidades_vendidas = 0;
    total_productos++;
    
    //Producto 11 
    strcpy(inventario_global[total_productos].nombre, "Ojos_de_Buey"); 
    inventario_global[total_productos].precio = 8.50;
    inventario_global[total_productos].existencia_actual = 100;
    inventario_global[total_productos].unidades_vendidas = 0;
    total_productos++;	    
    
    //Producto 12 
    strcpy(inventario_global[total_productos].nombre, "Croissant");
    inventario_global[total_productos].precio = 15.50;
    inventario_global[total_productos].existencia_actual = 100;
    inventario_global[total_productos].unidades_vendidas = 0;
    total_productos++;    
    
    //Producto 13 
    strcpy(inventario_global[total_productos].nombre, "Corbata");
    inventario_global[total_productos].precio = 6.50;
    inventario_global[total_productos].existencia_actual = 100;
    inventario_global[total_productos].unidades_vendidas = 0;
    total_productos++;    
    
    //Producto 14 
    strcpy(inventario_global[total_productos].nombre, "Galleta_chispas"); 
    inventario_global[total_productos].precio = 12.50;
    inventario_global[total_productos].existencia_actual = 100;
    inventario_global[total_productos].unidades_vendidas = 0;
    total_productos++;    
    
    //Producto 15 
    strcpy(inventario_global[total_productos].nombre, "Campechana");
    inventario_global[total_productos].precio = 8.50;
    inventario_global[total_productos].existencia_actual = 100;
    inventario_global[total_productos].unidades_vendidas = 0;
    total_productos++;    
    
    //Producto 16 
    strcpy(inventario_global[total_productos].nombre, "Panque");
    inventario_global[total_productos].precio = 12.50;
    inventario_global[total_productos].existencia_actual = 100;
    inventario_global[total_productos].unidades_vendidas = 0;
    total_productos++;    
    
    //Producto 17 
    strcpy(inventario_global[total_productos].nombre, "Gelatina_cafe"); 
    inventario_global[total_productos].precio = 20.00;
    inventario_global[total_productos].existencia_actual = 100;
    inventario_global[total_productos].unidades_vendidas = 0;
    total_productos++;    
    
    //Producto 18 
    strcpy(inventario_global[total_productos].nombre, "Gusano");
    inventario_global[total_productos].precio = 6.50;
    inventario_global[total_productos].existencia_actual = 100;
    inventario_global[total_productos].unidades_vendidas = 0;
    total_productos++;    
    
    //Producto 19 
    strcpy(inventario_global[total_productos].nombre, "Empanada_dulce");
    inventario_global[total_productos].precio = 8.50;
    inventario_global[total_productos].existencia_actual = 100;
    inventario_global[total_productos].unidades_vendidas = 0;
    total_productos++;    
    
    //Producto 20
    strcpy(inventario_global[total_productos].nombre, "Roles");
    inventario_global[total_productos].precio = 12.50;
    inventario_global[total_productos].existencia_actual = 100;
    inventario_global[total_productos].unidades_vendidas = 0;
    total_productos++;    
    
    //Producto 21
    strcpy(inventario_global[total_productos].nombre, "Bolillo");
    inventario_global[total_productos].precio = 4.50;
    inventario_global[total_productos].existencia_actual = 100;
    inventario_global[total_productos].unidades_vendidas = 0;
    total_productos++;	    
    
    //Producto 22 
    strcpy(inventario_global[total_productos].nombre, "Telera");
    inventario_global[total_productos].precio = 5.50;
    inventario_global[total_productos].existencia_actual = 100;
    inventario_global[total_productos].unidades_vendidas = 0;
    total_productos++;    
    
    //Producto 23 
    strcpy(inventario_global[total_productos].nombre, "Cubilete_queso"); 
    inventario_global[total_productos].precio = 15.50;
    inventario_global[total_productos].existencia_actual = 100;
    inventario_global[total_productos].unidades_vendidas = 0;
    total_productos++;    
    
    //Producto 24 
    strcpy(inventario_global[total_productos].nombre, "Chocolatin");
    inventario_global[total_productos].precio = 8.50;
    inventario_global[total_productos].existencia_actual = 100;
    inventario_global[total_productos].unidades_vendidas = 0;
    total_productos++;    
    
    //Producto 25 
    strcpy(inventario_global[total_productos].nombre, "Hojaldra");
    inventario_global[total_productos].precio = 8.50;
    inventario_global[total_productos].existencia_actual = 100;
    inventario_global[total_productos].unidades_vendidas = 0;
    total_productos++;	
    
    //Producto 26 
    strcpy(inventario_global[total_productos].nombre, "Cuernito");
    inventario_global[total_productos].precio = 8.50;
    inventario_global[total_productos].existencia_actual = 100;
    inventario_global[total_productos].unidades_vendidas = 0;
    total_productos++;    
    
    //Producto 27 
    strcpy(inventario_global[total_productos].nombre, "Rosca_reyes_chica"); 
    inventario_global[total_productos].precio = 85.00;
    inventario_global[total_productos].existencia_actual = 100;
    inventario_global[total_productos].unidades_vendidas = 0;
    total_productos++;    
    
    //Producto 28 
    strcpy(inventario_global[total_productos].nombre, "Rosca_reyes_mediana"); 
    inventario_global[total_productos].precio = 170.00;
    inventario_global[total_productos].existencia_actual = 100;
    inventario_global[total_productos].unidades_vendidas = 0;
    total_productos++;    
    
    //Producto 29 
    strcpy(inventario_global[total_productos].nombre, "Rosca_reyes_grande"); 
    inventario_global[total_productos].precio = 235.00;
    inventario_global[total_productos].existencia_actual = 100;
    inventario_global[total_productos].unidades_vendidas = 0;
    total_productos++;    
    
    //Producto 30 
    strcpy(inventario_global[total_productos].nombre, "Pan_de_muerto"); 
    inventario_global[total_productos].precio = 35.00;
    inventario_global[total_productos].existencia_actual = 100;
    inventario_global[total_productos].unidades_vendidas = 0;
    total_productos++;    
    
    //Producto 31 
    strcpy(inventario_global[total_productos].nombre, "Pan_espanol_dulce"); 
    inventario_global[total_productos].precio = 15.50;
    inventario_global[total_productos].existencia_actual = 100;
    inventario_global[total_productos].unidades_vendidas = 0;
    total_productos++;    
    
    //Producto 32 
    strcpy(inventario_global[total_productos].nombre, "Pan_espanol_salado"); 
    inventario_global[total_productos].precio = 12.50;
    inventario_global[total_productos].existencia_actual = 100;
    inventario_global[total_productos].unidades_vendidas = 0;
    total_productos++;    
    
    //Producto 33
    strcpy(inventario_global[total_productos].nombre, "Mil_hojas"); 
    inventario_global[total_productos].precio = 8.50;
    inventario_global[total_productos].existencia_actual = 100;
    inventario_global[total_productos].unidades_vendidas = 0;
    total_productos++;
    
    //Producto 34 
    strcpy(inventario_global[total_productos].nombre, "Cocol");
    inventario_global[total_productos].precio = 6.50;
    inventario_global[total_productos].existencia_actual = 100;
    inventario_global[total_productos].unidades_vendidas = 0;
    total_productos++;
    
    //Producto 35 
    strcpy(inventario_global[total_productos].nombre, "Piedra");
    inventario_global[total_productos].precio = 4.00;
    inventario_global[total_productos].existencia_actual = 100;
    inventario_global[total_productos].unidades_vendidas = 0;
    total_productos++;
    
    //Producto 36
    strcpy(inventario_global[total_productos].nombre, "Cerrillo");
    inventario_global[total_productos].precio = 6.50;
    inventario_global[total_productos].existencia_actual = 100;
    inventario_global[total_productos].unidades_vendidas = 0;
    total_productos++;
    
    //Producto 37 
    strcpy(inventario_global[total_productos].nombre, "Borrego");
    inventario_global[total_productos].precio = 6.50;
    inventario_global[total_productos].existencia_actual = 100;
    inventario_global[total_productos].unidades_vendidas = 0;
    total_productos++;
    
    //Producto 38 
    strcpy(inventario_global[total_productos].nombre, "Pay");
    inventario_global[total_productos].precio = 12.50;
    inventario_global[total_productos].existencia_actual = 100;
    inventario_global[total_productos].unidades_vendidas = 0;
    total_productos++;
    
    //Producto 39 
    strcpy(inventario_global[total_productos].nombre, "Pan_de_mermelada"); 
    inventario_global[total_productos].precio = 8.50;
    inventario_global[total_productos].existencia_actual = 100;
    inventario_global[total_productos].unidades_vendidas = 0;
    total_productos++;
    
    //Producto 40
    strcpy(inventario_global[total_productos].nombre, "Baguettes");
    inventario_global[total_productos].precio = 12.50;
    inventario_global[total_productos].existencia_actual = 100;
    inventario_global[total_productos].unidades_vendidas = 0;
    total_productos++;
    
    //Producto 41
    strcpy(inventario_global[total_productos].nombre, "Pan_de_caja"); 
    inventario_global[total_productos].precio = 8.50;
    inventario_global[total_productos].existencia_actual = 100;
    inventario_global[total_productos].unidades_vendidas = 0;
    total_productos++;
    
    //Producto 42 
    strcpy(inventario_global[total_productos].nombre, "Pinguinos");
    inventario_global[total_productos].precio = 12.50;
    inventario_global[total_productos].existencia_actual = 100;
    inventario_global[total_productos].unidades_vendidas = 0;
    total_productos++;
    
    //Producto 43 
    strcpy(inventario_global[total_productos].nombre, "Patelitos");
    inventario_global[total_productos].precio = 15.50;
    inventario_global[total_productos].existencia_actual = 100;
    inventario_global[total_productos].unidades_vendidas = 0;
    total_productos++;
    
    //Producto 44 
    strcpy(inventario_global[total_productos].nombre, "Chinos");
    inventario_global[total_productos].precio = 8.50;
    inventario_global[total_productos].existencia_actual = 100;
    inventario_global[total_productos].unidades_vendidas = 0;
    total_productos++;
    
    //Producto 45 
    strcpy(inventario_global[total_productos].nombre, "Beso");
    inventario_global[total_productos].precio = 6.50;
    inventario_global[total_productos].existencia_actual = 100;
    inventario_global[total_productos].unidades_vendidas = 0;
    total_productos++;
}

int main() {
    int opcion_menu = 0; 
    
    iniciar_sesion();
    cargar_inventario_inicial();

    //Bucle principal que controla la navegación.
    while (opcion_menu != 4) {
        printf("\n=== MENU PRINCIPAL ===\n");
        printf("1. Venta/Cobrar\n");
        printf("2. Inventario (Gestion y Consulta)\n");
        printf("3. Calculos (Estadisticas de Venta)\n");
        printf("4. Salir\n");
        printf("Seleccione una opcion: ");
        
        if (scanf("%d", &opcion_menu) != 1) { 
            limpiar_buffer(); 
            opcion_menu = 0; 
            continue; 
        }
        
        if (opcion_menu == 1) {
            menu_ventas();
        } else if (opcion_menu == 2) {
            menu_inventario();
        } else if (opcion_menu == 3) {
            menu_calculos();
        } else if (opcion_menu == 4) {
            printf("\nSaliendo del programa...\n");
        } else {
            printf("\nOpcion no valida. Intente de nuevo.\n");
        }
    }
    return 0;
}
