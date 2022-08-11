/*
  ==============================================================================

    ThinPlate.cpp
    Created: 20 Jul 2022 5:03:27am
    Author:  Benjamin Støier

  ==============================================================================
*/

#include "ThinPlate.h"



//==============================================================================
ThinPlate::ThinPlate (double kIn) : k (kIn) // <- This is an initialiser list. It initialises the member variable 'k' (in the "private" section in OneDWave.h), using the argument of the constructor 'kIn'.
{
    Lx= 0.5;
    Ly = 0.5; //side length (y)
    c = 343; //speed of sound
    sigma0 = 1; //frequency independent damping
    sigma1 = 0.005; //frequency dependent damping
    rho = 7700; //material density
    H = 5 * pow(10,-3); //Thickness
    E = 2 * pow(10,11); //Young's modulus
    nu = 0.3; //Poisson's ratio
    
    D = E*pow(H,3)/(12*(1-pow(nu,2))); // stifness coefficient
    kappa = sqrt(D/(rho*H)); // stifness paramater
    
    h = 2*sqrt(k*(sigma1+sqrt(pow(kappa,2)+pow(sigma1,2))));
    Nx = floor(Lx/h); //grid steps (x)
    Ny = floor(Ly/h); //grid steps (y)
    h = std::min (Lx / Nx, Ly / Ny); //redefine grid spacing (using the smallest dimension)

    N = (Nx+1)*(Ny+1);
    
    mu = kappa*k/pow(h,2);
    muSq = mu * mu;
    S = 2 * sigma1 * k /pow(h,2);
    maxForce = 10; //maximum force
    excTime = 0.001;
    t0 = 0;
    t = 0;
    excXpos = 0.3;
    excYpos = 0.3;
    hx = Lx/Nx;
    hy = Ly/Ny;
    li = floor(excXpos/hx);
    mi = floor(excYpos/hy);
    alphaX = excXpos/hx-li;
    alphaY = excYpos/hy-mi;
    
    lisXpos = 0.5;
    lisYpos = 0.5;
    n=0;
    
    J=0;
    
    uStates = std::vector<std::vector<std::vector<double>>> (3, std::vector<std::vector<double>>(Nx, std::vector<double>(Ny, 0)));
    uNext = &uStates[0][0];
    u = &uStates[1][0];
    uPrev = &uStates[2][0];
}

ThinPlate::~ThinPlate()
{
}

void ThinPlate::getSampleRate(double fsToSet)
{
    fs = fsToSet;
}



void ThinPlate::initParameters()
{
    D = E*pow(H,3)/(12*(1-pow(nu,2))); // stifness coefficient
    kappa = sqrt(D/(rho*H)); // stifness paramater
    h = 2*sqrt(k*(sigma1+sqrt(pow(kappa,2)+pow(sigma1,2))));
    Nx = floor(Lx/h); //grid steps (x)
    Ny = floor(Ly/h); //grid steps (y)
    h = std::min (Lx / Nx, Ly / Ny); //redefine grid spacing (using the smallest dimension)
    N = (Nx+1)*(Ny+1);
    
    mu = kappa*k/pow(h,2);
    muSq = mu * mu;
    S = 2 * sigma1 * k /pow(h,2);
    
    hx = Lx/Nx;
    hy = Ly/Ny;
    
    excXpos = excXposRatio*Lx;
    excYpos = excYposRatio*Ly;
    li = floor(excXpos/hx);
    mi = floor(excYpos/hy);
    alphaX = excXpos/hx-li;
    alphaY = excYpos/hy-mi;
    
    J=0;
    
    uStates = std::vector<std::vector<std::vector<double>>> (3, std::vector<std::vector<double>>(Nx, std::vector<double>(Ny, 0)));
    uNext = &uStates[0][0]; //Initialise time step u^n+1
    u = &uStates[1][0]; //Initialise time step u^n
    uPrev = &uStates[2][0]; //Initialise time step u^n-1
}

void ThinPlate::updateParameters(const double sig0ToSet, const double sig1ToSet, const double LxToSet, const double LyToSet, const double excXToSet, const double excYToSet, const double lisXToSet, const double lisYToSet, const double thicknessToSet, const double excFToSet, const double excTToSet)
{
    sigma0 = sig0ToSet;
    sigma1 = sig1ToSet;
    Lx = LxToSet;
    Ly = LyToSet;
    excXposRatio = excXToSet;
    excYposRatio = excYToSet;
    lisXpos = lisXToSet;
    lisYpos = lisYToSet;
    H = thicknessToSet*0.001f;
    maxForce = excFToSet;
    excTime = excTToSet*0.001f;
}

void ThinPlate::updatePlateMaterial(int plateMaterialToSet)
{
    plateMaterial=plateMaterialToSet;
    if (plateMaterial == 1) //brass
    {
        rho=8530;
        E = 110 * pow(10, 9);
        nu = 0.34;
    }
    else if (plateMaterial == 2) //bronze
    {
        rho = 8770;
        E = 103 * pow(10, 9);
        nu = 0.34;
    }
    else if (plateMaterial == 3) //iron
    {
        rho = 7874;
        E = 211 * pow(10, 9);
        nu = 0.26;
    }
    else if (plateMaterial == 4) // aluminium
    {
        rho = 2700;
        E = 70 * pow(10, 9);
        nu = 0.33;
    }
    else if (plateMaterial == 5) //gold
    {
        rho = 19300;
        E = 79 * pow(10, 9);
        nu = 0.42;
    }
    else if (plateMaterial == 6) //silver
    {
        rho = 10490;
        E = 83 * pow(10, 9);
        nu = 0.37;
    }
    else if (plateMaterial == 7) //copper
    {
        rho = 8920;
        E = 120 * pow(10, 9);
        nu = 0.36;
    }
}

void ThinPlate::plateHit()
{
    n = 0;
    t = 0;
}

void ThinPlate::calculateScheme()
{
    if (n < floor(excTime*fs))
    {
        malletForce= maxForce/2*(1-std::cos((2*juce::MathConstants<double>::pi*(t-t0))/(excTime)));
        t = t+k;
    }
    else
    {
        malletForce = 0;
    }
    n++;
    for (int l = 2; l < Nx-2; ++l) // clamped boundaries
    {
        for (int m = 2; m < Ny-2; ++m) // clamped boundaries
        {
            if (l == li && m == mi && 0 < malletForce)
                J = ((1-alphaX)*(1-alphaY))/(hx*hy);
            else if (l == li && m == mi+1 && 0 < malletForce)
                J = ((1-alphaX)*alphaY)/(hx*hy);
            else if (l == li+1 && m == mi && 0 < malletForce)
                J = (alphaX*(1-alphaY))/(hx*hy);
            else if (l == li+1 && m == mi+1 && 0 < malletForce)
                J = (alphaX*alphaY)/(hx*hy);
            else
                J = 0;
            
            uNext[l][m] =
            (2-20*muSq-4*S)*u[l][m]
            + (8*muSq+S) * (u[l+1][m] + u[l-1][m] + u[l][m+1] + u[l][m-1])
            - 2*muSq * (u[l+1][m+1] + u[l-1][m+1] + u[l+1][m-1] + u[l-1][m-1])
            - muSq * (u[l+2][m] + u[l-2][m] + u[l][m+2] + u[l][m-2])
            + (sigma0*k-1+4*S) * uPrev[l][m]
            - S * (uPrev[l+1][m] + uPrev[l-1][m] + uPrev[l][m+1] + u[l][m-1])
            + J * malletForce;
        }
    }
    updateStates();
}

void ThinPlate::updateStates()
{
        std::vector<double>* uTmp = uPrev;
        uPrev = u;
        u= uNext;
        uNext = uTmp;
}
