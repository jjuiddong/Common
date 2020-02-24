
#include "stdafx.h"
#include "neuralnet.h"

using namespace ai;

//------------------------------------------------------------------------
// sNeuron, sNeuronLayer
const double dBias = 0.f;

sNeuron::sNeuron(uint numInputs0)
	: numInputs(numInputs0 + 1)
{
	//we need an additional weight for the bias hence the +1
	for (uint i = 0; i < numInputs0 + 1; ++i)
		weight.push_back(RandomClamped());
}

sNeuronLayer::sNeuronLayer(uint numNeurons0, uint numInputsPerNeuron0) 
	: numNeurons(numNeurons0)
{
	for (uint i = 0; i < numNeurons0; ++i)
		neurons.push_back(sNeuron(numInputsPerNeuron0));
}


//------------------------------------------------------------------------
// cNeuralNet
cNeuralNet::cNeuralNet(const uint numInputs, const uint numOutputs
	, const uint numHidden, const uint neuronsPerHiddenLayer)
{
	m_dActivationResponse = 1.f;
	m_numInputs = numInputs;
	m_numOutputs = numOutputs;
	m_numHiddenLayers = numHidden;
	m_neuronsPerHiddenLyr = neuronsPerHiddenLayer;
	CreateNet();
}


cNeuralNet::~cNeuralNet()
{
}


// this method builds the ANN. The weights are all initially set to
// random values -1 < w < 1
void cNeuralNet::CreateNet()
{
	if (m_numHiddenLayers > 0)
	{
		m_layers.push_back(sNeuronLayer(m_neuronsPerHiddenLyr, m_numInputs));
		for (uint i = 0; i < m_numHiddenLayers - 1; ++i)
		{
			m_layers.push_back(sNeuronLayer(m_neuronsPerHiddenLyr,
				m_neuronsPerHiddenLyr));
		}
		m_layers.push_back(sNeuronLayer(m_numOutputs, m_neuronsPerHiddenLyr));
	}
	else
	{
		m_layers.push_back(sNeuronLayer(m_numOutputs, m_numInputs));
	}
}


// returns a vector containing the weights
vector<double> cNeuralNet::GetWeights() const
{
	vector<double> weights;
	for (uint i = 0; i < m_numHiddenLayers + 1; ++i)
		for (uint j = 0; j < m_layers[i].numNeurons; ++j)
			for (uint k = 0; k < m_layers[i].neurons[j].numInputs; ++k)
				weights.push_back(m_layers[i].neurons[j].weight[k]);
	return weights;
}


// given a vector of doubles this function replaces the weights in the NN
// with the new values
void cNeuralNet::PutWeights(const vector<double> &weights)
{
	int cWeight = 0;
	for (uint i = 0; i < m_numHiddenLayers + 1; ++i)
		for (uint j = 0; j < m_layers[i].numNeurons; ++j)
			for (uint k = 0; k < m_layers[i].neurons[j].numInputs; ++k)
				m_layers[i].neurons[j].weight[k] = weights[cWeight++];
}


// returns the total number of weights needed for the net
int cNeuralNet::GetNumberOfWeights() const
{
	int weights = 0;
	for (uint i = 0; i < m_numHiddenLayers + 1; ++i)
		for (uint j = 0; j < m_layers[i].numNeurons; ++j)
			weights += m_layers[i].neurons[j].numInputs;
	return weights;
}


// given an input vector this function calculates the output vector
vector<double> cNeuralNet::Update(INOUT vector<double> &inputs)
{
	vector<double> outputs;

	int cWeight = 0;
	if (inputs.size() != m_numInputs)
		return outputs;

	for (uint i = 0; i < m_numHiddenLayers + 1; ++i)
	{
		if (i > 0)
			inputs = outputs;

		outputs.clear();

		cWeight = 0;

		//for each neuron sum the (inputs * corresponding weights).Throw 
		//the total at our sigmoid function to get the output.
		for (uint j = 0; j < m_layers[i].numNeurons; ++j)
		{
			double netinput = 0;

			const uint numInputs = m_layers[i].neurons[j].numInputs;
			for (uint k = 0; k < numInputs - 1; ++k)
				netinput += m_layers[i].neurons[j].weight[k] * inputs[cWeight++];

			netinput += m_layers[i].neurons[j].weight[numInputs - 1] * dBias;

			// we can store the outputs from each layer as we generate them. 
			// The combined activation is first filtered through the sigmoid 
			// function
			const double val = Sigmoid(netinput, m_dActivationResponse) - 0.5f;
			outputs.push_back(val);
			m_layers[i].neurons[j].output = val;
			cWeight = 0;
		}
	}

	return outputs;
}


// Sigmoid function
double cNeuralNet::Sigmoid(double netinput, double response)
{
	return (1 / (1 + exp(-netinput / response)));
}
