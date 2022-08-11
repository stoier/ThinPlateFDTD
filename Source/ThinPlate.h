/*
  ==============================================================================

    ThinPlate.h
    Created: 20 Jul 2022 5:03:27am
    Author:  Benjamin Støier

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

class ThinPlate  : public juce::Component
{
public:
    ThinPlate (double k); // initialise the model with the time step
    ~ThinPlate() override;
    
void initParameters();

void updateParameters(const double sig0ToSet, const double sig1ToSet, const double LxToSet, const double LyToSet, const double excXToSet, const double excYToSet, const double lisXToSet, const double lisYToSet, const double thicknessToSet, const double excFToSet, const double excTToSet);
    
void updatePlateMaterial(int plateMaterialToSet);
    
void getSampleRate(double fsToSet);

void calculateScheme();
    
void plateHit();

void updateStates();

float getOutput() {return u[static_cast <int> (floor(lisXpos*Nx))][static_cast <int> (floor(lisYpos*Ny))]*0.000001;};
    
private:
    
    // Variables
    double k; // Time step (in s)
    double c; // Wave speed (in m/s)
    double h; // Grid spacing (in m)
    double Lx; // Length of x dimension(in m)
    double Ly; // Length of y dimension(in m)
    
    double T60;
    double sigma0; // frequency independent damping
    double sigma1; //frequency dependent damping
    double rho; //material density
    double H; //plate thickness
    double E; //Young's modulus
    double nu; //Poisson's ratio
    
    double D; // stifness coefficient
    double kappa; //stifness parameter
    double mu;
    double muSq;
    double S;
    double maxForce;
    double excTime;
    double excXpos;
    double excYpos;
    double excXposRatio;
    double excYposRatio;
    double lisXpos;
    double lisYpos;
    double hx;
    double hy;
    double lambda;
    int li;
    int mi;
    double alphaX;
    double alphaY;
    double J;
    double malletForce;
    
    int n;
    int plateMaterial;
    
    int Nx; // grid steps (x)
    int Ny; // grid steps (y)
    int N; // grid steps (total)
    
 
    
    double fs; //sample rate
    double t;
    double t0;
    
    
    std::vector<double>* uNext;
    std::vector<double>* u; // state pointers
    std::vector<double>* uPrev;
    std::vector<std::vector<std::vector<double>>> uStates;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ThinPlate)
};
