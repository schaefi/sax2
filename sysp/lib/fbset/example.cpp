#include "fbset.h"


int main (void) {
	FbData* data = FbGetData();
	printf ("%d\n",data->x); 
	printf ("%f %s %s\n",data->clock,data->ht,data->vt);
	return 0;
}
