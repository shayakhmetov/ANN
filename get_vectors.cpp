#include <iostream>
#include <vector>
#include <string>
#include <sys/types.h>
#include <dirent.h>
#include <fstream>
#include <algorithm>
#include <streambuf>
#define NUM_VECTORS 1000

using namespace std;

vector<string> pos,neg;//words
vector<string> pos_data,neg_data;//filepaths
int num_pos=0,num_neg=0;
int num_pos_data=0,num_neg_data=0;

vector<int> get_vector(string filepath)
{
  ifstream f(filepath.c_str());
	vector<int> result(num_neg+num_pos);
	for(int i=0;i<num_neg+num_pos;i++)
		result[i]=0;
	string temp;
	string str((istreambuf_iterator<char>(f)),istreambuf_iterator<char>());
	//cout << str << endl;
	vector<string>::iterator it;
	unsigned long index1 = str.find_first_of(" .,?!:;'""()$#{}][/|\\`~@%^*\n"),index0=0;
	f.close();
	//cout << "-------" << endl;
	while(index1!=string::npos)
	{
		temp = str.substr(index0,index1-index0);
		//cout << temp << ' ';
		if((it=find(pos.begin(),pos.end(),temp))!=pos.end())
			result[it-pos.begin()]++;
		else if((it=find(neg.begin(),neg.end(),temp))!=neg.end())
			result[num_pos+(it-neg.begin())]++;
		index0=index1+1;
		index1= str.find_first_of(" .,?!:;""'()$#{}][/|\\`~@%^*\n",index1+1);
	} 
	return result;
}

int main()
{
	ifstream fpos("./lexicon/pos.txt"),fneg("./lexicon/neg.txt");
	string temp;
	for(num_pos=0;;num_pos++)
	{
		fpos >> temp; 
		if(!fpos.eof()) pos.push_back(temp);
		else break;
	}
	for(num_neg=0;;num_neg++)
	{
		fneg >> temp; 
		if(!fneg.eof()) neg.push_back(temp);
		else break;
	}
	cout << num_pos << " " << num_neg << endl;
	fpos.close(); fneg.close();
	
	DIR *dpos=opendir("./train_set/pos/");
	DIR *dneg=opendir("./train_set/neg/");
	struct dirent *dir = readdir(dpos);
	//pos
	while(dir->d_name[0]=='.') dir = readdir(dpos);
	for(num_pos_data=0; dir!=NULL; num_pos_data++)
	{
		pos_data.push_back(string(dir->d_name));
		dir = readdir(dpos);
	}
	cout << num_pos_data << endl;
	closedir(dpos);
	//neg
	dir = readdir(dneg);
	while(dir->d_name[0]=='.') dir = readdir(dneg);
	for(num_neg_data=0; dir!=NULL; num_neg_data++)
	{
		neg_data.push_back(string(dir->d_name));
		dir = readdir(dneg);
	}
	cout << num_neg_data << endl;
	closedir(dneg);
	
	
	ofstream vectors("./vectors.data");
	int size = num_neg+num_pos;
	vectors << 2*NUM_VECTORS << ' ' << size << ' ' << '1' << endl; 
	for(int i=0;i<NUM_VECTORS;i++)
	{
		vector<int> temp = get_vector("./train_set/pos/"+pos_data[i]);
		for(int j=0; j<size;j++)
		{
			vectors << temp[j] << ' ';
		}
		vectors << endl << '1' << endl;
		temp = get_vector("./train_set/neg/"+neg_data[i]);
		for(int j=0; j<size;j++)
		{
			vectors << temp[j] << ' ';
		}
		vectors << endl << "-1" << endl;
	}
	vectors.close();
	cout << "vectors" << endl;
	
	return 0;
}
