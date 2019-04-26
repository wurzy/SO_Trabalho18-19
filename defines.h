#ifndef DEFINES_H
#define DEFINES_H

#define DEFAULT_SIZE   100

/* tamanho de "casas" que tem o indicador de artigos */
#define NUMBER_LEN    "8"
#define NUMBER_LEN_I   8
/* tamanho de "casas" que tem o tem o offset do nome */
#define POINTER_LEN   "8"
#define POINTER_LEN_I  8
/* tamanho de "casas" que tem o preco do artigo      */
#define PRICE_LEN     "10"
#define PRICE_LEN_I    10
/* tamanho de "casas" que tem o artigo               */
#define ARTIGO_LENG    28

/* length do INT_MAX */
#define MAX_INT_LEN    10

/* tamanho de "casas" q tem as linhas do deprecated  */
#define DEPREC_LEN     POINTER_LEN_I + NUMBER_LEN_I + MAX_INT_LEN + 2


/* macro para por o numero com NUMBER_LEN casas      */
#define NUMBER_SIZE   "%0"NUMBER_LEN"ld"
/* macro para por o numero com POINTER_LEN casas     */
#define POINTER_SIZE  "%0"POINTER_LEN"ld"
/* macro para por o numero com PRICE_LEN casas       */
#define PRICE_SIZE    "%0"PRICE_LEN".2f"


#endif
