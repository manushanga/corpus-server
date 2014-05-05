#include<iostream>
using namespace std;
int main(){
	ofstream of("CR.txt");
	string ss;
	while (of.good()){
		of>>ss;
		cout<<ss;
	}
	return 0;
}
