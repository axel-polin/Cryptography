#include "../INSTALL/pqpcs.h"

int main(){
int32_t i=0,j=0;
manage_bridge("create",0,0);
for(i=0;i<5;i++){
    for(j=0;j<5;j++){
        printf(" %u ",pqp.table[i][j]);
     }
}
return EXIT_SUCCESS;
}