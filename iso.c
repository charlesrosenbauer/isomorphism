#include "stdint.h"
#include "stdlib.h"
#include "stdio.h"


int loadFile(char* fname, uint8_t** buffer, int* fsize){
	FILE*  pFile = fopen (fname,"r");
	size_t result;
	if (pFile == NULL) return 0;
	fseek (pFile , 0 , SEEK_END);
	*fsize = ftell (pFile);
	rewind (pFile);

	// allocate memory to contain the whole file:
	*buffer = malloc((sizeof(uint8_t)*(*fsize)) + 4);
	if (buffer == NULL) { printf("Memory error\n",stderr); exit(2); }

	// copy the file into the buffer:
	result = fread (*buffer,1,(*fsize),pFile);
	if (result != (*fsize)) { printf("Reading error\n",stderr); exit(3); }
	
	fclose(pFile);
	return 1;
}


typedef struct{
	uint32_t**	vertEdges;
	uint32_t*	edgeCts;
	uint32_t*	edges;
	
	uint32_t*	as;
	uint32_t*	bs;
	
	uint32_t	vct, ect;
}Graph;

int parseInt(char* text, int ix, uint32_t* ret){
	*ret = 0;
	while((text[ix] >= '0') && (text[ix] <= '9')){
		*ret = ((*ret) * 10) + (text[ix] - '0');
		ix++;
	}
	return ix;
}

int skipWhitespace(char* text, int ix){
	while(text[ix] <= ' ') ix++;
	return ix;
}

int parseGraph(char* text, int size, Graph* g){
	int ix = 0;
	ix = skipWhitespace(text, ix);
	if(ix >= size) return 0;
	ix = parseInt(text, ix, &g->vct);
	if(ix >= size) return 0;
	ix = skipWhitespace(text, ix);
	if(ix >= size) return 0;
	ix = parseInt(text, ix, &g->ect);
	if(ix >= size) return 0;
	ix = skipWhitespace(text, ix);
	if(ix >= size) return 0;
	
	g->as = malloc(sizeof(uint32_t) * g->ect);
	g->bs = malloc(sizeof(uint32_t) * g->ect);
	
	int ct = 0;
	while(ix < size){
		if(ct > g->ect) return 0;
		
		uint32_t a, b;
		ix = parseInt(text, ix, &a);
		if(ix >= size) return 0;
		ix = skipWhitespace(text, ix);
		if(ix >= size) return 0;
		ix = parseInt(text, ix, &b);
		if(ix >= size) return 0;
		ix = skipWhitespace(text, ix);
		
		g->as[ct] = a;
		g->bs[ct] = b;
		if(!a || (a > g->vct)) return 0;
		if(!b || (b > g->vct)) return 0;
		ct++;
	}
	
	if(ct != g->ect) return 0;
	
	g->edgeCts = malloc(sizeof(uint32_t) * g->vct);
	for(int i = 0; i < g->vct; i++) g->edgeCts[i] = 0;
	for(int i = 0; i < g->ect; i++){
		g->edgeCts[g->as[i]-1]++;
		g->edgeCts[g->bs[i]-1]++;
	}
	g->edges     = malloc(sizeof(uint32_t ) * g->ect * 2);
	g->vertEdges = malloc(sizeof(uint32_t*) * g->vct);
	ct = 0;
	for(int i = 0; i < g->vct; i++){
		g->vertEdges[i] = &g->edges[ct];
		ct += g->edgeCts[i];
		g->edgeCts[i]   = 0;	// This gets reconstructed
	}
	for(int i = 0; i < g->ect; i++){
		uint32_t a = g->as[i] - 1;
		uint32_t b = g->bs[i] - 1;
		g->vertEdges[a][g->edgeCts[a]] = b;
		g->edgeCts[a]++;
		g->vertEdges[b][g->edgeCts[b]] = a;
		g->edgeCts[b]++;
	}
	
	return 1;
}

void printGraph(Graph g){
	for(int i = 0; i < g.ect; i++)
		printf("(%i,%i) ", g.as[i], g.bs[i]);
	printf("\n");
	
	for(int i = 0; i < g.vct; i++){
		printf("V%i : ", i+1);
		for(int j = 0; j < g.edgeCts[i]; j++) printf("%i ", g.vertEdges[i][j]+1);
		printf("\n");
	}
}



int main(int argc, char** args){
	for(int i = 1; i < argc; i++){
		uint8_t* buffer;
		int      fsize;
		if(!loadFile(args[i], &buffer, &fsize)){
			printf("Could not load file %s\n", args[i]);
			continue;
		}
		Graph g;
		if(!parseGraph((char*)buffer, fsize, &g)){
			printf("Bad graph description\n");
			continue;
		}
		
		// TODO: Get a pair of graphs, check isomorphism
		printGraph(g);
	}
}
