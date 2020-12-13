// BruteForce.cpp : Ten plik zawiera funkcję „main”. W nim rozpoczyna się i kończy wykonywanie programu.
//

#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <tuple>

#include "Time.h"
#include "Manager.h"
#include <conio.h>
#include <vector>



using std::cout;
using std::cin;
using namespace std;
//using namespace chrono;

//etap 0,1,2,3
int readedSize; // rozmiar tabeli (do odczytu)
int** allVertex; // macierz zawierająca wszystkie odległości pomiędzy wierzchołkami
int optimumSize; // najmniejsza odleglosc (pobierana z pliku)
int lenght = 0; //aktualna dlugosc ciagu (zmienia sie po iteracji algorytmu)

int percentOfMistake;
int smallerLenght; //aktualnie najmniejsza znaleziona dlugosc ciagu
int* shortestPath; //najkrotsza sciezka

double getP(int newCost, int currentCost, double t)
{
	return exp(-(newCost - currentCost) / t);
}

double getG(double t, int counter)
{

	return (t / (1 + 0.00001));
}

double getRandom()
{
	return ((rand() % 9999) + 1) / 10000;
}

vector<int> getFirstPath()
{
	vector<int> path;
	int changesLeft = readedSize * readedSize;
	int* newPath = new int[readedSize];

	int a, b;

	int rand1 = 0;
	int rand2 = 0;


	for (int i = 0; i < readedSize; i++)
	{
		newPath[i] = i;
	}
	for (int i = 0; i < changesLeft; i++)
	{

		rand1 = rand() % readedSize;
		rand2 = rand() % readedSize;

		if (rand1 == rand2) rand2++;
		if (rand2 >= readedSize) rand2 = 0;
		swap(newPath[rand1], newPath[rand2]);
	}
	for (int i = 0; i < readedSize; i++)
		path.push_back(newPath[i]);

	delete[]newPath;
	return path;
}

int pathLenght(vector<int> path) {
	lenght = 0;

	for (int i = 0; i < readedSize - 1; i++)
		lenght += allVertex[path[i]][path[i + 1]];

	lenght += allVertex[path.back()][path.front()];

	return lenght;
}

string pathToString(vector<int> shortestPath) {
	string path = "";
	for (int i = 0; i < shortestPath.size(); i++)
	{
		path.append(to_string(shortestPath[i]));
		path.append("-");
	}
	path.append(to_string(shortestPath[0]));
	return path;
}


std::tuple<int, string, int> sa(int optimumSize)
{
	vector<int> newPath;
	vector<int> currentPath = getFirstPath();
	vector<int> bestPath = currentPath;

	int newCost;
	int currentCost = pathLenght(currentPath);
	int optimumCost = currentCost;

	double t = 1;
	//double t = pow(readedSize, 5); //poczatkowa temperatura
	double tMin = 0.00001; // koncowa temperatura


	int counter = 0;
	int antystagnacja = 0; 
	int tempVal = readedSize * 4; //wartosc antystagnacji
	int rand1, rand2;
	srand(time(NULL));
	while (t > tMin)
	{
		if (antystagnacja > tempVal)
		{
			currentPath = getFirstPath();
			antystagnacja = 0;
		}
		//nowa currentPath z zamienionymi sąsiadami
		rand1 = rand() % readedSize;
		do {
			rand2 = rand() % readedSize;
		} while (rand1 == rand2);

		newPath = currentPath;
		swap(newPath[rand1], newPath[rand2]);

		newCost = pathLenght(newPath);

		if (newCost < optimumCost)
		{
			bestPath = newPath;
			optimumCost = newCost;
			antystagnacja = 0;
		}

		else if (getRandom() < getP(newCost, currentCost, t))
		{
			currentPath = newPath;
			currentCost = newCost;
			antystagnacja = 0;
		}
		else
		{
			antystagnacja++;
		}
		t = getG(t, counter);

		if (counter % 25000 == 0) {
			cout << "Iteracja: " << counter << "   Rozwiazanie: " << optimumCost << "  Optymalne rozwiazanie: " << optimumSize << "   Aktualna temperatura: " << t << endl;

			if (optimumCost == optimumSize)
			{
				cout << "Znalazlem optymalne rozwiazanie na " << counter << " iteracji. Rozwiazanie: " << optimumSize << endl;
				break;
			}
		}
		counter++;
	}

	return make_tuple(optimumCost, pathToString(bestPath), counter);
}


//Funkcje do zapisu danych
void addCsvDescription(string csvFileName)
{
	fstream fout;
	fout.open(csvFileName, ios::out | ios::app);
	fout << "File name" << ", " << "Time(s)" << ", " << "Best path" << ", " << "Path lenght" << ", " << "Optimum" << ", " << "Best path" << ", " << "% of mistake" << ", " << "Iteration with soulution/stop algorithm\n";
	fout.close();
}

void autoSave(string time, string csvFileName, string matrixFile, string optimumPath2, int optimumSize, string foundedPath, int iteracja, int smallerLenght)
{
	fstream fout;
	fout.open(csvFileName, ios::out | ios::app);

	if (optimumSize != 0) percentOfMistake = ((smallerLenght - optimumSize) * 100) / optimumSize ;
	else percentOfMistake = -1;

	fout << matrixFile << ", " << time << ", " << foundedPath << ", " << smallerLenght << ", " << optimumSize << ", " << optimumPath2 << ", " << percentOfMistake << ", " << iteracja << "\n";
	fout.close();
}


int main(int argc, char *argv[])
{
	string iniFile;
	cout << "Podaj nazwe pliku inicjalizujacego lub nacisnij wpisz 1 aby wczytac plik test.ini\n";
	cin >> iniFile;

	if (iniFile == "1") iniFile = "test.ini";

	string* matrixFile; //tablica plikow
	int* optimumSize; //tablica optimow
	int* repeatsVal; //tablica powtorzen
	string* optimumPath; //tablica sciezek
	string csvFile; // plik do zapisu
	int iniFileSize, breakTime; //zmienne pomocnicze

	Manager m; //utworzenie obiektu zarzadzajacy pozostalymi plikami
	
	// funkcja loadFromIniFile zwraca wszystkie wartosci pobrane z pliku inicjalizujacego
	tie(matrixFile, optimumSize, repeatsVal, optimumPath, csvFile, iniFileSize, breakTime) = m.loadFromIniFile(iniFile); 

	if (breakTime == 0) return 0; //przerwanie programmu w przypadku bledu

	for (int i = 0; i < iniFileSize; i++) //petla wykonuje sie tyle razy ile wczytano plikow
	{
		if (optimumSize[i] == 0) break; //jesli nie znalazla optimum to sie przerwie
		
		cout << "\n\nRozpoczynam prace nad plikiem: " << matrixFile[i]
			<< ":  optimum: " << optimumSize[i]
			<< "   ilosc powtorzen: " << repeatsVal[i] << endl << endl;

		Time t;
		addCsvDescription(csvFile); // na poczatku dodanie do pliku csv naglowka
		for (int j = 0; j < repeatsVal[i]; j++)
		{
			tie(readedSize, allVertex) = m.TSPFromFile(matrixFile[i]); //wczytanie macierzy z pliku
			t.timeStart(); //rozpoczecie liczenia czasu algorytmu/metoy
			int iteracja, temperature;
			string foundedPath;
			tie(smallerLenght, foundedPath, iteracja) = sa(optimumSize[i]);
			t.timeStop(); //zakonczenie liczenia czasu algorytmu/metody
			string time = to_string(t.CountTime() * 0.001); //obliczenie czasu wykonywania
			//autoSave(time, csvFile, matrixFile[i], optimumPath[i], optimumSize[i]); //zapis wyniku do pliku
			autoSave(time, csvFile, matrixFile[i], optimumPath[i], optimumSize[i], foundedPath, iteracja, smallerLenght); //zapis wyniku do pliku
		}
	}

	return 0;
}
