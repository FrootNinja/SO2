#include <iostream>
#include <thread> 
#include <ncurses.h> 
#include <vector> 
#include <cstdlib>
#include <chrono>
#include <mutex>

using namespace std;
const int M=20;  //szerokość
const int N=15; //wysokość
int tab[N][M];
int suma=1;
mutex kolejka1Guard;
mutex kolejka2Guard;
mutex tabGuard;
vector <int> kolejka1;
vector <int> kolejka2;

void producent(){
	while(true){
		kolejka1Guard.lock();
		kolejka2Guard.lock();
		kolejka1.push_back((rand() % M));
		kolejka2.push_back((rand() % N));
		kolejka1Guard.unlock();
		kolejka2Guard.unlock();
		this_thread::sleep_for(chrono::seconds(1));
	}
}
void przetwarzacz(){
int zm1=0;
int zm2=0;
int zm3=0;
int zm4=0;
this_thread::sleep_for(chrono::seconds(1));
while(true){
	//sumowanie z kolejki zależnie od wielkości
	if(kolejka1.size()==1 || kolejka2.size()==1) {
		kolejka1Guard.lock();
		kolejka2Guard.lock();
		suma = kolejka1[0]+kolejka2[0];
		kolejka1Guard.unlock();
		kolejka2Guard.unlock();
	}
	if(kolejka1.size()==0 || kolejka2.size()==0) {
		suma = 1;
	}
	else {
		zm1 = kolejka1.size();
		zm2= kolejka2.size();
		zm3=(rand()%zm1);
		zm4=(rand()%zm2);
		kolejka1Guard.lock();
		kolejka2Guard.lock();
		suma = kolejka1[zm3] + kolejka2[zm4];
		kolejka1Guard.unlock();
		kolejka2Guard.unlock();
	}
	//przesunięcie
	for (int i = N; i >= 0; i--) {
		for (int j = M; j >= 0; j--) {
			tabGuard.lock();
			if (i == 0 && j == 0) {
				//pierwsza wart.
			} else if (i != 0 && j == 0) {
				tab[i][j] = tab[i-1][N-1]; //do pierwszego pola w wierszu bierzemy z wyższego wiersza ostatniego pola
			} else {
				tab[i][j] = tab[i][j-1];
			}
			tabGuard.unlock();
		}
	}
	/*for(int k=M;k>=0;k=k-1)
	{
		for(int l=N;l>=0; l=l-1){
			tabGuard.lock();
			if(l==0) {
				tab[k][l] = tab[k-1][N-1];
			}
			else if(k==0 && l==0) {}
			else {
				tab[k][l]=tab[k][l-1];
			}
			tabGuard.unlock();
		}
	}*/
	tabGuard.lock();
	tab[0][0]=suma; //wstawienie zsumowanej wczesniej wartości do jako pierwszy element macierzy
	tabGuard.unlock();
	this_thread::sleep_for(chrono::seconds(1));
	}
}

void ncurses(){
this_thread::sleep_for(chrono::milliseconds(1500));
while(true){
	clear();
	for(int n=0; n<N;n++){
		printw("\n");
		for(int o=0; o<M;o++){
			tabGuard.lock();
			printw("%d ", tab[n][o]);
			tabGuard.unlock();
		}
	}
	refresh();
	this_thread::sleep_for(chrono::seconds(1));

}
}
int main()
{
initscr();
srand (time(NULL));
auto watekProducent = thread(producent);
auto watekPrzetwarzacz = thread(przetwarzacz);
auto watekNcurses = thread(ncurses);
watekProducent.join();
watekPrzetwarzacz.join();
watekNcurses.join();
getch();
endwin();
return 0;
}
