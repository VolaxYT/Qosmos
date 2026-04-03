#pragma once
#include "config.h"

namespace Quantum {

inline double factorial(int n){
    double result = 1.0;
    for(int i = 2; i <= n; i++) result *= i;
    return result;
}

inline double laguerre(int n, int alpha, double x){
    if(n == 0) return 1.0;
    if(n == 1) return 1.0 + alpha - x;

    double L_prev2 = 1.0;
    double L_prev1 = 1.0 + alpha - x;
    double L_curr  = 0.0;

    for(int k = 2; k <= n; k++){
        L_curr  = ((2*k - 1 + alpha - x) * L_prev1
                 - (k - 1 + alpha)       * L_prev2) / k;
        L_prev2 = L_prev1;
        L_prev1 = L_curr;
    }
    return L_curr;
}

inline double legendre(int l, int m, double x){
    m = std::abs(m);

    double Pmm    = 1.0;
    double factor = 1.0;
    double sqt    = std::sqrt(std::max(0.0, 1.0 - x*x));
    for(int i = 1; i <= m; i++){
        Pmm   *= -factor * sqt;
        factor += 2.0;
    }

    if(l == m) return Pmm;

    double Pmm1 = x * (2*m + 1) * Pmm;
    if(l == m + 1) return Pmm1;

    double Pll = 0.0;
    for(int ll = m + 2; ll <= l; ll++){
        Pll  = ((2*ll - 1) * x * Pmm1 - (ll + m - 1) * Pmm) / (ll - m);
        Pmm  = Pmm1;
        Pmm1 = Pll;
    }
    return Pll;
}

inline double R_nl(int n, int l, double r, int Z = 1){
    double rho = 2.0 * Z * r / n;

    double N = std::sqrt(
        std::pow(2.0 * Z / n, 3)
        * factorial(n - l - 1)
        / (2.0 * n * factorial(n + l))
    );

    double poly = laguerre(n - l - 1, 2*l + 1, rho);

    return N * std::exp(-rho / 2.0) * std::pow(rho, l) * poly;
}

inline double psi_squared_real(int n, int l, int m,
                                double r, double theta, double phi,
                                int Z = 1)
{
    if(r < 1e-10) return 0.0;

    double R  = R_nl(n, l, r, Z);
    int    am = std::abs(m);

    double cos_theta = std::cos(theta);
    double P         = legendre(l, am, cos_theta);

    double norm = (2*l + 1) / (4.0 * M_PI)
                * factorial(l - am) / factorial(l + am);

    double Theta = std::sqrt(norm) * P;

    double angular;
    if(m == 0){
        angular = Theta;
    } else if(m > 0){
        angular = Theta * std::sqrt(2.0) * std::cos(am * phi);
    } else {
        angular = Theta * std::sqrt(2.0) * std::sin(am * phi);
    }

    return R * R * angular * angular;
}

inline void cartesian_to_spherical(double x, double y, double z,
                                    double& r, double& theta, double& phi)
{
    r     = std::sqrt(x*x + y*y + z*z);
    theta = (r > 1e-10) ? std::acos(z / r) : 0.0;
    phi   = std::atan2(y, x);
}

inline glm::vec3 inferno(float t){
    t = glm::clamp(t, 0.0f, 1.0f);

    const glm::vec3 c0(0.000f, 0.000f, 0.000f);
    const glm::vec3 c1(0.180f, 0.020f, 0.320f);
    const glm::vec3 c2(0.560f, 0.050f, 0.380f);
    const glm::vec3 c3(0.870f, 0.220f, 0.100f);
    const glm::vec3 c4(0.980f, 0.640f, 0.040f);
    const glm::vec3 c5(1.000f, 1.000f, 1.000f);

    if(t < 0.2f) return glm::mix(c0, c1, t / 0.2f);
    if(t < 0.4f) return glm::mix(c1, c2, (t - 0.2f) / 0.2f);
    if(t < 0.6f) return glm::mix(c2, c3, (t - 0.4f) / 0.2f);
    if(t < 0.8f) return glm::mix(c3, c4, (t - 0.6f) / 0.2f);
                 return glm::mix(c4, c5, (t - 0.8f) / 0.2f);
}

} // namespace Quantum