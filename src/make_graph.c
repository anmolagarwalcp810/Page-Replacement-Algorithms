#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(){
	FILE* plotfile = popen("gnuplot -persistent","w");
	char* commands[] = {
		"set terminal png size 1000,400","set output './output/plot.png'","set title 'Page Faults vs Policy'",
		"set xlabel 'Policy'","set ylabel 'Page Faults'",
		"set style data histogram","set style fill solid","set yrange[0:*]",
		"plot './output/file.txt' using 2:xtic(1) title ''"
	};
	int n = sizeof(commands)/sizeof(commands[0]);
	for(int i=0;i<n;i++){
		fprintf(plotfile, "%s\n", commands[i]);
	}
	if(plotfile!=NULL)fclose(plotfile);
}
