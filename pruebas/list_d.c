#include "list_d.h"

struct internal{
  list process;
};

struct nodo
{
  void *elem;
  struct nodo *next;
};

static struct nodo *createNodo()
{
	struct nodo *temp=malloc(sizeof(struct nodo));

	if (temp==NULL){
		printf("Agotated memory\n");
		exit(EXIT_FAILURE);
	}
	return temp;

}

list createList(){
	struct nodo *l=createNodo();
	l->next=NULL;
	return l;
}

int isEmptyList(list l){
	return (l->next==NULL);
}

void insertElement(void *x,pos p){
	struct nodo *temp=createNodo();
	temp->elem=x;
	temp->next=p->next;
	p->next=temp;
}

int removeE(list l ,pos p){
	struct nodo *temp;
	if(isEmptyList(l))
		return 0;
	if (first(l)!=p){
		temp=l;
		while(temp->next!=p)
			temp=temp->next;
		temp->next=p->next;
		free(temp);
	}else{
		l->next=p->next;
		free(l);
	}
	return 0;
}


int removeAll(list l){
	struct nodo *temp,*p=l;
	if(!isEmptyList(l)){
		temp=p->next;
		p->next=temp->next;
		free(temp);
	}else
		free(l);

	return 0;
}

pos first(list l){
	return l->next;
}

pos nextE(pos p){
	return p->next;
}

int last(pos p){
	return(p==NULL);
}

void *getElement(pos p){
	return p->elem;
}

pos lookFor(list l,void *e,int (*comp)(const void *x,const void *y)){
	struct nodo *p=l->next;
	while(p!=NULL && 0!=(*comp)(p->elem,e))
		p=p->next;
	return p;
}