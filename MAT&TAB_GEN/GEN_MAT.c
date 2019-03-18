#include "../INSTALL/pqpcs.h"

int main(){
int32_t i=0;
pqp.MAT = ec_malloc(pqp.nb_entry_mat,sizeof(u_int32_t));
manage_matrice("create");
for(i=0;i<pqp.nb_entry_mat;i++)
   printf(" %d ",pqp.MAT[i]);
return EXIT_SUCCESS;
}