#pragma once
#include "config.h"
#include "quantum.h"
#include <random>

namespace Sampler {

inline void generate(
    int n, int l, int m,
    int N,
    std::vector<glm::vec3>& positions,
    std::vector<glm::vec3>& colors,
    std::vector<float>&     alphas,
    int Z = 1)
{
    positions.clear();
    colors.clear();
    alphas.clear();

    std::mt19937 rng(std::random_device{}());
    std::uniform_real_distribution<double> dist01(0.0, 1.0);

    double rmax = 8.0 * n * n / (double)Z;

    // --- Cherche p_max sur grille 3D ---
    // Obligatoire pour m != 0 car sin/cos(m*phi) dépend de phi
    double p_max = 0.0;
    int grid = 50;
    for(int i = 1; i <= grid; i++){
        double r = rmax * i / grid;
        for(int j = 1; j < grid; j++){
            double theta = M_PI * j / grid;
            for(int k = 0; k < grid; k++){
                double phi = 2.0 * M_PI * k / grid;
                double val = Quantum::psi_squared_real(n, l, m, r, theta, phi, Z)
                           * r * r * std::sin(theta);
                if(val > p_max) p_max = val;
            }
        }
    }
    p_max *= 1.2;

    // psi_max pour la couleur (sans jacobien)
    double psi_max = 0.0;
    for(int i = 1; i <= grid; i++){
        double r = rmax * i / grid;
        for(int j = 1; j < grid; j++){
            double theta = M_PI * j / grid;
            for(int k = 0; k < grid; k++){
                double phi = 2.0 * M_PI * k / grid;
                double val = Quantum::psi_squared_real(n, l, m, r, theta, phi, Z);
                if(val > psi_max) psi_max = val;
            }
        }
    }

    std::cerr << "rmax=" << rmax
              << " p_max=" << p_max
              << " psi_max=" << psi_max << std::endl;

    // --- Rejection sampling en coordonnées sphériques ---
    int accepted = 0;
    int attempts = 0;

    while(accepted < N){
        attempts++;

        double r     = rmax * dist01(rng);
        double theta = M_PI  * dist01(rng);
        double phi   = 2.0 * M_PI * dist01(rng);

        // Densité avec jacobien sphérique
        double p = Quantum::psi_squared_real(n, l, m, r, theta, phi, Z)
                 * r * r * std::sin(theta);

        if(dist01(rng) > p / p_max) continue;

        // Conversion sphérique -> cartésien normalisé
        float scale = (float)rmax;
        float x = (float)(r * std::sin(theta) * std::cos(phi)) / scale;
        float y = (float)(r * std::sin(theta) * std::sin(phi)) / scale;
        float z = (float)(r * std::cos(theta)) / scale;
        positions.push_back(glm::vec3(x, y, z));

        // Couleur inferno selon psi^2
        double psi2 = Quantum::psi_squared_real(n, l, m, r, theta, phi, Z);
        
        float  t    = (psi_max > 0.0) ? (float)std::pow(psi2 / psi_max, 0.15) : 0.0f;
        colors.push_back(Quantum::inferno(t));
        alphas.push_back(0.02f + 0.06f * t);

        accepted++;
    }

    std::cerr << "Acceptes=" << accepted
              << " Essais=" << attempts
              << " Efficacite=" << 100.0 * accepted / attempts << "%"
              << std::endl;
}

} // namespace Sampler