#include <fann.h>
#include <math.h>
#include <iostream>
#include <fstream>
#define NUM_INPUTS 6789
#define NUM_TEST 200
#define N_VECTORS 2000 
#define KOEFF 10

using namespace std;
int vectors[N_VECTORS][NUM_INPUTS*2];
int vectors_label[N_VECTORS];
  
float predict(struct fann *ann)
{
    fann_type *calc_out;
    float precision = 0;
    int predicted=0;
    fann_type inputs[NUM_INPUTS];
    ifstream f("./test.data");
    int temp;
    f >> temp >> temp >> temp;
    int polarity;
    bool neg=false,pos=false,truth=false;
    for(int i=0;i<NUM_TEST;i++)
    { 
		neg=false;pos=false;
		for(int j=0;j<NUM_INPUTS;j++)
		{
			f >> inputs[j];
		}
		f >> polarity;
		if(polarity==-1) neg=true;
		else if(polarity==1) pos=true;
		calc_out = fann_run(ann, inputs);
		truth=(calc_out[0]<0 && neg) || (calc_out[0]>=0 && pos);
		if(truth) predicted++;
    }
    precision = (float)predicted/NUM_TEST;
    f.close();
    return precision;
}

int main()
{
	const float alpha = 0.7;
	const float steepness = 0.5*log((alpha/(1-alpha))); 
	const unsigned int num_output = 1;
	const unsigned int num_layers = 3;
	const unsigned int num_neurons_hidden = 2;
	const float desired_error = (const float) 0.01;
	const unsigned int max_epochs = 500000;
	const unsigned int epochs_between_reports = 100;
	
	//fann_randomize_weights(ann,-0.1,0.2);
	//fann_init_weights(ann,fann_read_train_from_file("train.data"));
	//fann_print_parameters(ann);
	
	ifstream f("./vectors.data");
    int temp;
    f >> temp >> temp >> temp;
    for(int i=0;i<N_VECTORS;i++)
	{ 
		for(int j=0;j<NUM_INPUTS;j++)
		{
			f >> vectors[i][j] ;
		}
		f >> vectors_label[i]; 
	}
	f.close();
    double precision = 0;
    double max_precision = 0;
    double min_precision = 1;
    for(int k=0;k<N_VECTORS-NUM_TEST;k+=N_VECTORS/KOEFF)
    {
		struct fann *ann = fann_create_standard(num_layers, NUM_INPUTS, num_neurons_hidden, num_output);
		fann_set_training_algorithm(ann,FANN_TRAIN_QUICKPROP);
		fann_set_activation_steepness_hidden(ann, steepness);
		fann_set_activation_function_hidden(ann, FANN_SIGMOID_SYMMETRIC);
		fann_set_activation_steepness_hidden(ann, 1); 
		fann_set_activation_function_output(ann, FANN_LINEAR_PIECE_SYMMETRIC);
		fann_set_weight(ann,NUM_INPUTS+1,NUM_INPUTS+3,1);
		fann_set_weight(ann,NUM_INPUTS+2,NUM_INPUTS+3,-1);

		ofstream train_f("./train.data");
		ofstream test_f("./test.data");
		train_f << N_VECTORS-NUM_TEST << " " << NUM_INPUTS << " 1" << endl;
		test_f << NUM_TEST << " " << NUM_INPUTS << " 1" << endl;
		for(int i=0;i<k;i++)
		{ 
			for(int j=0;j<NUM_INPUTS;j++)
			{
				train_f << vectors[i][j] << " ";
			}
			train_f << endl << vectors_label[i] << endl; 
		}
		for(int i=k;i<k+NUM_TEST;i++)
		{ 
			for(int j=0;j<NUM_INPUTS;j++)
			{
				test_f << vectors[i][j] << " ";
			}
			test_f << endl << vectors_label[i] << endl; 
		}
		test_f.close();
		for(int i=k+NUM_TEST;i<N_VECTORS;i++)
		{ 
			for(int j=0;j<NUM_INPUTS;j++)
			{
				train_f << vectors[i][j] << " ";
			}
			train_f << endl << vectors_label[i] << endl; 
		}
		cout << "TRAIN AND TEST " << k << endl;
		train_f.close();
		fann_train_on_file(ann, "./train.data", max_epochs, epochs_between_reports, desired_error);
		fann_save(ann, "saved_train.net");
		double prediction = predict(ann);
		cout << "Ann predicted " << prediction*100 << "%" << endl;
		precision+=prediction;
		if(prediction>max_precision) max_precision = prediction;
		if(prediction<min_precision) min_precision = prediction;
		fann_destroy(ann);
	}
	precision = precision / (N_VECTORS-NUM_TEST)*N_VECTORS/KOEFF;
	//fann_print_parameters(ann);
	cout << "-----------------------------------------" << endl;
	cout << "END!!! Ann predicted " << precision*100 << "%" << endl;
	cout << "Min precision " << min_precision*100 << "%" << endl;
	cout << "Max precision " << max_precision*100 << "%" << endl;
	
	return 0;
}
