
typedef struct dad{
   
	char name;
	struct dad *link;
}lee;

void name(lee *n){
	char result;
	result=n->name+n->link->name;
}

int main(){
	
	lee l;

	l->name="shin";

	name(&l);
	
	printf("result: %s\n",l.result);
}

