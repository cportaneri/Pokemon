#include <string.h>
#include <stdlib.h>
#include <stdio.h>
/* Un square est une zone de 16*16 pixel qui cadrille le terrain. Permet
l'affichage de chipset et définie les obstacles*/

typedef struct
{

  int type; /*1 si obstacle sinon 0*/
  int chipset; /* Chipset du square */

} Square;


/** \brief Creation d'un square
 *
 *
 * \return square
 *
 */

Square  CreatSquare(int chipset, int type);

