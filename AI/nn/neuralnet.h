//
// 2020-02-21, jjuiddong
// Neural Network Algorithm
//	- Mat Buckland 2002
//
#pragma once


namespace ai
{

	struct sNeuron
	{
		uint numInputs;
		double output;
		vector<double> weight;
		sNeuron(uint NumInputs0);
	};


	struct sNeuronLayer
	{
		uint numNeurons;
		vector<sNeuron> neurons;
		sNeuronLayer(uint NumNeurons, uint NumInputsPerNeuron);
	};


	class cNeuralNet
	{
	public:
		cNeuralNet(const uint numInputs, const uint numOutputs
			, const uint numHidden, const uint neuronsPerHiddenLayer);
		virtual ~cNeuralNet();

		void CreateNet();
		vector<double> GetWeights() const;
		int GetNumberOfWeights() const;
		void PutWeights(const vector<double> &weights);
		vector<double> Update(INOUT vector<double> &inputs);
		inline double Sigmoid(double activation, double response);


	public:
		uint m_numInputs;
		uint m_numOutputs;
		uint m_numHiddenLayers;
		uint m_neuronsPerHiddenLyr;
		vector<sNeuronLayer> m_layers;
		double m_dActivationResponse;
	};

}
