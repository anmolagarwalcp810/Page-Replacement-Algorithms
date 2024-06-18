#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <limits.h>


struct element{
	int page_number;
	int reference_bit;
	int counter;
	int reference_array[8];		// needed for LRU only
};

int* reference_string;
struct element* memory;
int page_frames, reference_string_length;

void initialize_memory(void){
	for(int i=0;i<page_frames;i++){
		memory[i].page_number=-1;
		memory[i].reference_bit=0;
		memory[i].counter = 0;
		for(int j=0;j<8;j++){
			memory[i].reference_array[j]=0;
		}
	}
}

int find_element(int n){
	// return -1, if not found
	// else return index
	for(int i=0;i<page_frames;i++){
		if(memory[i].page_number==n){
			return i;
		}
	}
	return -1;
}

int find_min_index_LFU(void){
	int min_index,min_value=INT_MAX;
	for(int i=0;i<page_frames;i++){
		if(memory[i].counter<min_value){
			min_index=i;
			min_value=memory[i].counter;
		}
	}
	return min_index;
}

int LRU_min_index(void){
	int min_index,max_value=-1;
	for(int i=0;i<page_frames;i++){
		int j=0;
		for(j=0;j<8;j++){
			if(memory[i].reference_array[j]==1){
				break;
			}
		}
		if(j>max_value){
			max_value=j;
			min_index=i;
		}
	}
	return min_index;
}

void shift_right(int index){
	for(int i=7;i>=1;i--){
		memory[index].reference_array[i]=memory[index].reference_array[i-1];
	}
	memory[index].reference_array[0]=0;
}

// performs FIFO and returns page faults
int FIFO(void){
	initialize_memory();
	int pointer = 0;
	int page,index;
	int page_faults=0;
	for(int i=0;i<reference_string_length;i++){
		page = reference_string[i];
		// if(NOT FOUND): page_fault++;
		// if already all empty then memory[pointer].page_number==-1, don't increase pointer
		// if some empty, then memory[(pointer+1)%page_frames]==-1, increase pointer and then replace at that pointer
		// if none empty then neither of the two above conditions hold, hence replace at pointer and then 
		// increase pointer.

		// but if(FOUND): then do nothing

		index = find_element(page);
		if(index==-1){
			page_faults++;
			if(memory[pointer].page_number==-1){
				memory[pointer].page_number=page;
			}
			else if(memory[(pointer+1)%page_frames].page_number==-1){
				pointer = (pointer+1)%page_frames;
				memory[pointer].page_number=page;
			}
			else{
				memory[pointer].page_number=page;
				pointer=(pointer+1)%page_frames;
			}
		}
	}

	return page_faults;
}

// LFU, returns page_faults
int LFU(void){
	initialize_memory();
	int pointer = 0;
	int page,index;
	int page_faults=0;
	for(int i=0;i<reference_string_length;i++){
		page = reference_string[i];

		index = find_element(page);
		if(index==-1){
			page_faults++;
			if(memory[pointer].page_number==-1){
				memory[pointer].page_number=page;
				memory[pointer].counter=0;
			}
			else if(memory[(pointer+1)%page_frames].page_number==-1){
				pointer = (pointer+1)%page_frames;
				memory[pointer].page_number=page;
				memory[pointer].counter=0;
			}
			else{
				// needs replacement of minimum counter
				index = find_min_index_LFU();
				memory[index].page_number=page;
				memory[index].counter=0;
			}
		}
		else{
			memory[index].counter++;
		}
	}

	return page_faults;
}

// LRU, returns page_faults
int LRU(void){
	initialize_memory();
	int pointer = 0;
	int page,index;
	int page_faults=0;
	for(int i=0;i<reference_string_length;i++){
		page = reference_string[i];

		index = find_element(page);
		if(index==-1){
			page_faults++;
			if(memory[pointer].page_number==-1){
				// no need to re-initialize reference_array
				for(int j=0;j<page_frames;j++){
					if(memory[j].page_number!=-1)shift_right(j);
				}
				memory[pointer].page_number=page;
				// memory[pointer].reference_array={1,0,0,0,0,0,0,0};
				memory[pointer].reference_array[0]=1;
				for(int j=1;j<8;j++){
					memory[pointer].reference_array[j]=0;
				}
			}
			else if(memory[(pointer+1)%page_frames].page_number==-1){
				// no need to re-initialize reference_array
				for(int j=0;j<page_frames;j++){
					if(memory[j].page_number!=-1)shift_right(j);
				}
				pointer = (pointer+1)%page_frames;
				memory[pointer].page_number=page;
				// memory[pointer].reference_array={1,0,0,0,0,0,0,0};
				memory[pointer].reference_array[0]=1;
				for(int j=1;j<8;j++){
					memory[pointer].reference_array[j]=0;
				}
			}
			else{
				// needs replacement of minimum counter
				index = LRU_min_index();
				for(int j=0;j<page_frames;j++){
					if(memory[j].page_number!=-1)shift_right(j);
				}
				memory[index].page_number=page;
				// memory[index].reference_array={1,0,0,0,0,0,0,0};
				memory[index].reference_array[0]=1;
				for(int j=1;j<8;j++){
					memory[index].reference_array[j]=0;
				}
			}
		}
		else{
			for(int j=0;j<page_frames;j++){
				if(memory[j].page_number!=-1)shift_right(j);
			}
			memory[index].reference_array[0]=1;
		}
	}

	return page_faults;
}

// Clock, returns page_faults
int Clock(void){
	initialize_memory();
	int pointer = 0;
	int page,index;
	int page_faults=0;
	for(int i=0;i<reference_string_length;i++){
		page = reference_string[i];

		index = find_element(page);
		if(index==-1){
			page_faults++;
			if(memory[pointer].page_number==-1){
				memory[pointer].page_number=page;
				pointer=(pointer+1)%page_frames;
			}
			else{
				// no frame is empty
				// NEED REPLACEMENT
				while(1){
					if(memory[pointer].reference_bit==0){
						// replace
						memory[pointer].page_number=page;
						pointer=(pointer+1)%page_frames;
						break;
					}
					else{
						memory[pointer].reference_bit=0;
						pointer=(pointer+1)%page_frames;
					}
				}
			}
		}
		else{
			memory[index].reference_bit=1;
		}
	}

	return page_faults;
}

// N chance, returns page_faults
int N_Chance(int N){
	initialize_memory();
	int pointer = 0;
	int page,index;
	int page_faults=0;
	for(int i=0;i<reference_string_length;i++){
		page = reference_string[i];

		index = find_element(page);
		if(index==-1){
			page_faults++;
			if(memory[pointer].page_number==-1){
				memory[pointer].page_number=page;
				pointer=(pointer+1)%page_frames;
			}
			else{
				// no frame is empty
				// NEED REPLACEMENT
				while(1){
					if(memory[pointer].reference_bit==0){
						memory[pointer].counter++;
						if(memory[pointer].counter==N-1){
							// now replace
							memory[pointer].page_number=page;
							memory[pointer].counter=0;
							pointer=(pointer+1)%page_frames;
							break;
						}
						pointer=(pointer+1)%page_frames;
					}
					else{
						memory[pointer].reference_bit=0;
						memory[pointer].counter=0;
						pointer=(pointer+1)%page_frames;
					}
				}
			}
		}
		else{
			memory[index].reference_bit=1;
		}
	}

	return page_faults;
}

int main(const int argc,char* argv[]){
	FILE* file;
	file = fopen(argv[1],"r");
	fscanf(file,"%d",&reference_string_length);
	fscanf(file,"%d",&page_frames);

	reference_string = (int*)malloc(sizeof(int)*reference_string_length);
	memory = (struct element*)malloc(sizeof(struct element)*page_frames);

	// read reference string and close file
	for(int i=0;i<reference_string_length;i++){
		fscanf(file,"%d",&reference_string[i]);
	}

	fclose(file);

	int page_faults=0;	// will be initialized to 0 after entering every if statement

	int N=5;

	file = fopen("./output/file.txt","w");

	if(argc==2){
		printf("FIFO: ");
		page_faults = FIFO();
		printf("%d\n",page_faults);

		fprintf(file, "%s %d\n","FF",page_faults);

		printf("LFU: ");
		page_faults = LFU();
		printf("%d\n",page_faults);

		fprintf(file, "%s %d\n","LF",page_faults);

		printf("LRU: ");
		page_faults = LRU();
		printf("%d\n",page_faults);

		fprintf(file, "%s %d\n","LR",page_faults);

		printf("Clock: ");
		page_faults = Clock();
		printf("%d\n",page_faults);

		fprintf(file, "%s %d\n","CL",page_faults);

		printf("%d Chance: ",N);
		page_faults = N_Chance(N);
		printf("%d\n",page_faults);

		fprintf(file, "%s %d\n","SC",page_faults);

	}
	else{
		for(int i=2;i<argc;i++){
			if(strcmp(argv[i],"FF")==0){
				printf("FIFO: ");
				page_faults = FIFO();
				printf("%d\n",page_faults);
			}
			else if(strcmp(argv[i],"LF")==0){
				printf("LFU: ");
				page_faults = LFU();
				printf("%d\n",page_faults);
			}
			else if(strcmp(argv[i],"LR")==0){
				printf("LRU: ");
				page_faults = LRU();
				printf("%d\n",page_faults);
			}
			else if(strcmp(argv[i],"CL")==0){
				printf("Clock: ");
				page_faults = Clock();
				printf("%d\n",page_faults);
			}
			else if(strcmp(argv[i],"SC")==0){
				printf("%d Chance: ",N);
				page_faults = N_Chance(N);
				printf("%d\n",page_faults);
			}
			else{
				printf("Wrong Input\n");
			}
			fprintf(file, "%s %d\n",argv[i],page_faults);
		}
	}
	fclose(file);
}